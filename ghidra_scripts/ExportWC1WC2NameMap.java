// Export every WC1-derived name that changed WC2 from its pristine backup.
//
// Output is tab-separated and printed between MAP_BEGIN/MAP_END markers so it
// can be captured without mixing in Ghidra's script compiler diagnostics.  The
// optional script arguments are an output offset and limit.
//@category Wing Commander

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Set;

import ghidra.app.script.GhidraScript;
import ghidra.feature.vt.api.main.VTAssociation;
import ghidra.feature.vt.api.main.VTAssociationStatus;
import ghidra.feature.vt.api.main.VTAssociationType;
import ghidra.feature.vt.api.main.VTMatch;
import ghidra.feature.vt.api.main.VTMatchTag;
import ghidra.feature.vt.api.main.VTSession;
import ghidra.framework.model.DomainFile;
import ghidra.framework.model.ProjectData;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.FunctionTag;
import ghidra.program.model.listing.Program;

public class ExportWC1WC2NameMap extends GhidraScript {

    private static final String DESTINATION_PATH = "/WC2/WC2.EXE";
    private static final String SESSION_PATH = "/WC2/WC1-to-WC2-auto-v1";
    private static final String BACKUP_PATH =
        "/WC2/backups/WC2-before-WC1-name-transfer";

    private static final String[] REVIEW_TAGS = {
        "WC1-callgraph-transfer",
        "WC1-callgraph-propagated",
        "WC1-callgraph-single",
        "WC1-callgraph-single-low",
        "WC1-order-exact",
        "WC1-fuzzy-high",
        "WC1-fuzzy-medium",
        "WC1-fuzzy-low",
        "WC1-fuzzy-followup",
        "WC1-fuzzy-very-low"
    };

    private static final class Row {
        Function sourceFunction;
        Function destinationFunction;
        VTAssociation association;
        VTMatch match;
        String evidence;
    }

    @Override
    public void run() throws Exception {
        if (currentProgram == null ||
                !DESTINATION_PATH.equals(currentProgram.getDomainFile().getPathname())) {
            printerr("Open " + DESTINATION_PATH + " before running this script.");
            return;
        }

        int offset = 0;
        int limit = Integer.MAX_VALUE;
        String[] arguments = getScriptArgs();
        if (arguments.length >= 1) {
            offset = Math.max(0, Integer.parseInt(arguments[0]));
        }
        if (arguments.length >= 2) {
            limit = Math.max(0, Integer.parseInt(arguments[1]));
        }

        ProjectData projectData = state.getProject().getProjectData();
        DomainFile sessionFile = projectData.getFile(SESSION_PATH);
        DomainFile backupFile = projectData.getFile(BACKUP_PATH);
        if (sessionFile == null || backupFile == null) {
            printerr("Missing session " + SESSION_PATH + " or backup " + BACKUP_PATH + ".");
            return;
        }

        end(true);

        VTSession session = null;
        Program backupProgram = null;
        try {
            session = (VTSession) sessionFile.getDomainObject(this, false, false, monitor);
            backupProgram =
                (Program) backupFile.getDomainObject(this, false, false, monitor);

            Program sourceProgram = session.getSourceProgram();
            Program destinationProgram = session.getDestinationProgram();
            List<Row> rows = collectRows(
                session, sourceProgram, destinationProgram, backupProgram);

            int end = Math.min(rows.size(), offset + limit);
            println("MAP_VERSION=1");
            println("MAP_SOURCE_SHA256=" + sourceProgram.getExecutableSHA256());
            println("MAP_DESTINATION_SHA256=" + destinationProgram.getExecutableSHA256());
            println("MAP_TOTAL=" + rows.size());
            println("MAP_OFFSET=" + offset);
            println("MAP_COUNT=" + Math.max(0, end - offset));
            println("MAP_BEGIN");
            println(header());
            for (int index = offset; index < end; index++) {
                monitor.checkCancelled();
                println(formatRow(rows.get(index)));
            }
            println("MAP_END");
        }
        finally {
            if (backupProgram != null) {
                backupProgram.release(this);
            }
            if (session != null) {
                session.release(this);
            }
        }
    }

    private List<Row> collectRows(VTSession session, Program sourceProgram,
            Program destinationProgram, Program backupProgram) throws Exception {
        List<Row> result = new ArrayList<>();
        int missingAssociation = 0;

        FunctionIterator iterator =
            destinationProgram.getFunctionManager().getFunctions(true);
        while (iterator.hasNext()) {
            monitor.checkCancelled();
            Function destinationFunction = iterator.next();
            Address destinationAddress = destinationFunction.getEntryPoint();
            Address backupAddress = backupProgram.getAddressFactory().getAddress(
                destinationAddress.toString());
            Function backupFunction =
                backupProgram.getFunctionManager().getFunctionAt(backupAddress);

            if (backupFunction == null ||
                    !isDefaultName(backupFunction.getName()) ||
                    isDefaultName(destinationFunction.getName())) {
                continue;
            }

            VTAssociation association = findAcceptedAssociation(
                session, sourceProgram, destinationFunction);
            if (association == null) {
                missingAssociation++;
                continue;
            }

            Function sourceFunction = sourceProgram.getFunctionManager().getFunctionAt(
                association.getSourceAddress());
            Row row = new Row();
            row.sourceFunction = sourceFunction;
            row.destinationFunction = destinationFunction;
            row.association = association;
            row.evidence = getEvidence(destinationFunction);
            row.match = selectMatch(session.getMatches(association), row.evidence);
            result.add(row);
        }

        Collections.sort(result, Comparator.comparing(
            row -> row.destinationFunction.getEntryPoint()));
        println("MAP_MISSING_ASSOCIATION=" + missingAssociation);
        return result;
    }

    private VTAssociation findAcceptedAssociation(VTSession session,
            Program sourceProgram, Function destinationFunction) {
        Collection<VTAssociation> associations =
            session.getAssociationManager().getRelatedAssociationsByDestinationAddress(
                destinationFunction.getEntryPoint());
        for (VTAssociation association : associations) {
            if (association.getType() != VTAssociationType.FUNCTION ||
                    association.getStatus() != VTAssociationStatus.ACCEPTED) {
                continue;
            }
            Function sourceFunction = sourceProgram.getFunctionManager().getFunctionAt(
                association.getSourceAddress());
            if (sourceFunction != null && sourceFunction.getName().equals(
                    destinationFunction.getName())) {
                return association;
            }
        }
        return null;
    }

    private VTMatch selectMatch(List<VTMatch> matches, String evidence) {
        VTMatch best = null;
        for (VTMatch match : matches) {
            VTMatchTag tag = match.getTag();
            if (tag != null && evidence.equals(tag.getName())) {
                return match;
            }
            if (best == null || score(match) > score(best)) {
                best = match;
            }
        }
        return best;
    }

    private double score(VTMatch match) {
        if (match == null || match.getConfidenceScore() == null) {
            return Double.NEGATIVE_INFINITY;
        }
        return match.getConfidenceScore().getScore();
    }

    private String getEvidence(Function function) {
        Set<FunctionTag> tags = function.getTags();
        for (String reviewTag : REVIEW_TAGS) {
            for (FunctionTag tag : tags) {
                if (reviewTag.equals(tag.getName())) {
                    return reviewTag;
                }
            }
        }
        return "WC1-auto-vt";
    }

    private String header() {
        return "source_address\tdestination_address\tfunction_name\tevidence" +
            "\treview_flags\tsimilarity\tconfidence\tassociation_votes\tsource_length" +
            "\tdestination_length\tsource_callers\tdestination_callers" +
            "\tsource_callees\tdestination_callees\tcorrelator\taudit_comment";
    }

    private String formatRow(Row row) {
        VTMatch match = row.match;
        return join(
            row.sourceFunction.getEntryPoint().toString(),
            row.destinationFunction.getEntryPoint().toString(),
            row.destinationFunction.getName(),
            row.evidence,
            getReviewFlags(row.destinationFunction, row.evidence),
            match == null || match.getSimilarityScore() == null ? "" :
                Double.toString(match.getSimilarityScore().getScore()),
            match == null || match.getConfidenceScore() == null ? "" :
                Double.toString(match.getConfidenceScore().getScore()),
            Integer.toString(row.association.getVoteCount()),
            match == null ? "" : Integer.toString(match.getSourceLength()),
            match == null ? "" : Integer.toString(match.getDestinationLength()),
            functionList(row.sourceFunction.getCallingFunctions(monitor)),
            functionList(row.destinationFunction.getCallingFunctions(monitor)),
            functionList(row.sourceFunction.getCalledFunctions(monitor)),
            functionList(row.destinationFunction.getCalledFunctions(monitor)),
            match == null ? "" :
                match.getMatchSet().getProgramCorrelatorInfo().getName(),
            row.destinationFunction.getRepeatableComment());
    }

    private String functionList(Set<Function> functions) {
        List<Function> sorted = new ArrayList<>(functions);
        Collections.sort(sorted, Comparator.comparing(Function::getEntryPoint));
        List<String> values = new ArrayList<>();
        for (Function function : sorted) {
            values.add(function.getName() + "@" + function.getEntryPoint());
        }
        return String.join("|", values);
    }

    private String getReviewFlags(Function function, String evidence) {
        List<String> values = new ArrayList<>();
        for (FunctionTag tag : function.getTags()) {
            String name = tag.getName();
            if (name.startsWith("WC1-") && !name.equals(evidence)) {
                values.add(name);
            }
        }
        Collections.sort(values);
        return String.join("|", values);
    }

    private String join(String... fields) {
        List<String> escaped = new ArrayList<>();
        for (String field : fields) {
            escaped.add(escape(field));
        }
        return String.join("\t", escaped);
    }

    private String escape(String value) {
        if (value == null) {
            return "";
        }
        return value.replace("\\", "\\\\")
            .replace("\t", "\\t")
            .replace("\r", "\\r")
            .replace("\n", "\\n");
    }

    private boolean isDefaultName(String name) {
        return name.startsWith("FUN_") || name.startsWith("thunk_FUN_");
    }
}
