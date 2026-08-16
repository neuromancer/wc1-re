// Replay the exported WC1-to-WC2 function-name map on a clean WC2 program.
//
// Script arguments:
//   1. Path to wc1_wc2_name_map.tsv (a file chooser opens when omitted)
//   2. Optional "dry-run"
//   3. Optional "allow-conflicts" (skip non-default destination conflicts)
//
// The executable hashes, WC1 source addresses, source names, destination
// addresses, and destination name conflicts are validated before any change.
// Only names, function tags, and repeatable audit comments are applied.
//@category Wing Commander

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import ghidra.app.script.GhidraScript;
import ghidra.framework.model.DomainFile;
import ghidra.framework.model.ProjectData;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.FunctionTag;
import ghidra.program.model.listing.FunctionTagManager;
import ghidra.program.model.listing.Program;
import ghidra.program.model.symbol.SourceType;

public class ApplyWC1WC2NameMap extends GhidraScript {

    private static final String SOURCE_PATH = "/WC1%2FWC1.EXE";
    private static final String EXPECTED_SOURCE_SHA256 =
        "d517169e57f1d6254682c03dec5c254b5101ccd787d191f639b0cac1e8a616c4";
    private static final String EXPECTED_DESTINATION_SHA256 =
        "f5e133e8eca8eb1a02e21a7d1afbb44d082e384a38d39db58a694371fa583bbe";
    private static final String EXPECTED_HEADER =
        "source_address\tdestination_address\tfunction_name\tevidence" +
        "\treview_flags\tsimilarity\tconfidence\tassociation_votes" +
        "\tsource_length\tdestination_length\tsource_callers" +
        "\tdestination_callers\tsource_callees\tdestination_callees" +
        "\tcorrelator\taudit_comment";

    private static final class Row {
        String sourceAddress;
        String destinationAddress;
        String functionName;
        String evidence;
        String reviewFlags;
        String similarity;
        String confidence;
        String correlator;
        String auditComment;
    }

    private static final class NameMap {
        String sourceSha256;
        String destinationSha256;
        int declaredRows = -1;
        List<Row> rows = new ArrayList<>();
    }

    private static final class Validation {
        int sourceMissing;
        int sourceNameMismatch;
        int destinationMissing;
        int destinationConflict;
        int nameConflict;
        int duplicateSource;
        int duplicateDestination;
        int duplicateName;

        int errorCount() {
            return sourceMissing + sourceNameMismatch + destinationMissing +
                destinationConflict + nameConflict + duplicateSource +
                duplicateDestination + duplicateName;
        }
    }

    @Override
    public void run() throws Exception {
        if (currentProgram == null) {
            printerr("Open a WC2 program before running this script.");
            return;
        }

        String[] arguments = getScriptArgs();
        File mapFile = arguments.length == 0 ?
            askFile("Select wc1_wc2_name_map.tsv", "Open") :
            new File(arguments[0]);
        boolean dryRun = hasArgument(arguments, "dry-run");
        boolean allowConflicts = hasArgument(arguments, "allow-conflicts");

        NameMap nameMap = readMap(mapFile);
        validateHash("map source", nameMap.sourceSha256, EXPECTED_SOURCE_SHA256);
        validateHash("map destination", nameMap.destinationSha256,
            EXPECTED_DESTINATION_SHA256);
        validateHash("current destination", currentProgram.getExecutableSHA256(),
            EXPECTED_DESTINATION_SHA256);

        ProjectData projectData = state.getProject().getProjectData();
        DomainFile sourceFile = projectData.getFile(SOURCE_PATH);
        if (sourceFile == null ||
                !Program.class.isAssignableFrom(sourceFile.getDomainObjectClass())) {
            printerr("Missing WC1 source program " + SOURCE_PATH + ".");
            return;
        }

        Program sourceProgram = null;
        try {
            sourceProgram =
                (Program) sourceFile.getDomainObject(this, false, false, monitor);
            validateHash("WC1 source", sourceProgram.getExecutableSHA256(),
                EXPECTED_SOURCE_SHA256);

            Validation validation = validateRows(
                nameMap.rows, sourceProgram, currentProgram);
            printValidation(validation, nameMap.rows.size());

            int blockingErrors = validation.errorCount();
            if (allowConflicts) {
                blockingErrors -= validation.destinationConflict;
                blockingErrors -= validation.nameConflict;
            }
            if (blockingErrors != 0) {
                printerr("Map validation failed; no changes were applied.");
                return;
            }
            if (dryRun) {
                println("DRY_RUN_OK=true");
                return;
            }

            Map<String, String> tagComments = tagComments();
            ensureTags(currentProgram, nameMap.rows, tagComments);

            int renamed = 0;
            int alreadyNamed = 0;
            int conflictsSkipped = 0;
            int tagsAdded = 0;
            int commentsSet = 0;
            Set<String> occupiedNames = collectOccupiedNames(currentProgram);

            for (Row row : nameMap.rows) {
                monitor.checkCancelled();
                Address destinationAddress = currentProgram.getAddressFactory().getAddress(
                    row.destinationAddress);
                Function function = currentProgram.getFunctionManager().getFunctionAt(
                    destinationAddress);
                if (function == null) {
                    throw new IOException("Destination function disappeared at " +
                        row.destinationAddress);
                }

                String currentName = function.getName();
                if (row.functionName.equals(currentName)) {
                    alreadyNamed++;
                }
                else if (isDefaultName(currentName) &&
                        !occupiedNames.contains(row.functionName)) {
                    function.setName(row.functionName, SourceType.USER_DEFINED);
                    occupiedNames.add(row.functionName);
                    renamed++;
                }
                else {
                    conflictsSkipped++;
                    if (!allowConflicts) {
                        throw new IOException("Unexpected destination conflict at " +
                            row.destinationAddress);
                    }
                    continue;
                }

                if (!hasTag(function, row.evidence) && function.addTag(row.evidence)) {
                    tagsAdded++;
                }
                for (String flag : splitFlags(row.reviewFlags)) {
                    if (!hasTag(function, flag) && function.addTag(flag)) {
                        tagsAdded++;
                    }
                }

                String auditComment = makeAuditComment(row);
                String oldComment = function.getRepeatableComment();
                String newComment = mergeAuditComment(oldComment, auditComment);
                if (!same(oldComment, newComment)) {
                    function.setRepeatableComment(newComment);
                    commentsSet++;
                }
            }

            println("MAP_ROWS=" + nameMap.rows.size());
            println("FUNCTIONS_RENAMED=" + renamed);
            println("FUNCTIONS_ALREADY_NAMED=" + alreadyNamed);
            println("CONFLICTS_SKIPPED=" + conflictsSkipped);
            println("TAGS_ADDED=" + tagsAdded);
            println("COMMENTS_SET=" + commentsSet);

            end(true);
            currentProgram.save("Applied WC1-to-WC2 name map", monitor);
            println("APPLY_OK=true");
        }
        finally {
            if (sourceProgram != null) {
                sourceProgram.release(this);
            }
        }
    }

    private NameMap readMap(File file) throws IOException {
        if (!file.isFile()) {
            throw new IOException("Map file does not exist: " + file);
        }

        NameMap result = new NameMap();
        boolean foundHeader = false;
        try (BufferedReader reader = Files.newBufferedReader(
                file.toPath(), StandardCharsets.UTF_8)) {
            String line;
            int lineNumber = 0;
            while ((line = reader.readLine()) != null) {
                lineNumber++;
                if (line.isEmpty()) {
                    continue;
                }
                if (line.startsWith("# ")) {
                    readMetadata(result, line.substring(2));
                    continue;
                }
                if (!foundHeader) {
                    if (!EXPECTED_HEADER.equals(line)) {
                        throw new IOException("Unexpected map header at line " + lineNumber);
                    }
                    foundHeader = true;
                    continue;
                }

                String[] fields = line.split("\\t", -1);
                if (fields.length != 16) {
                    throw new IOException("Expected 16 fields at line " + lineNumber +
                        ", got " + fields.length);
                }
                Row row = new Row();
                row.sourceAddress = unescape(fields[0]);
                row.destinationAddress = unescape(fields[1]);
                row.functionName = unescape(fields[2]);
                row.evidence = unescape(fields[3]);
                row.reviewFlags = unescape(fields[4]);
                row.similarity = unescape(fields[5]);
                row.confidence = unescape(fields[6]);
                row.correlator = unescape(fields[14]);
                row.auditComment = unescape(fields[15]);
                validateRowFields(row, lineNumber);
                result.rows.add(row);
            }
        }

        if (!foundHeader) {
            throw new IOException("Map header is missing.");
        }
        if (result.declaredRows != result.rows.size()) {
            throw new IOException("Map declares " + result.declaredRows +
                " rows but contains " + result.rows.size());
        }
        return result;
    }

    private void readMetadata(NameMap nameMap, String text) throws IOException {
        int equals = text.indexOf('=');
        if (equals < 0) {
            return;
        }
        String key = text.substring(0, equals);
        String value = text.substring(equals + 1);
        if ("source_sha256".equals(key)) {
            nameMap.sourceSha256 = value;
        }
        else if ("destination_sha256".equals(key)) {
            nameMap.destinationSha256 = value;
        }
        else if ("rows".equals(key)) {
            nameMap.declaredRows = Integer.parseInt(value);
        }
    }

    private void validateRowFields(Row row, int lineNumber) throws IOException {
        if (!row.sourceAddress.matches("[0-9a-fA-F]{8}") ||
                !row.destinationAddress.matches("[0-9a-fA-F]{8}")) {
            throw new IOException("Invalid address at line " + lineNumber);
        }
        if (row.functionName.isEmpty() || !row.evidence.startsWith("WC1-")) {
            throw new IOException("Invalid name or evidence tag at line " + lineNumber);
        }
        for (String flag : splitFlags(row.reviewFlags)) {
            if (!flag.startsWith("WC1-")) {
                throw new IOException("Invalid review flag at line " + lineNumber);
            }
        }
    }

    private Validation validateRows(List<Row> rows, Program sourceProgram,
            Program destinationProgram) throws Exception {
        Validation result = new Validation();
        Set<String> sourceAddresses = new HashSet<>();
        Set<String> destinationAddresses = new HashSet<>();
        Set<String> mappedNames = new HashSet<>();
        Map<String, Address> occupiedNames = collectOccupiedNameAddresses(
            destinationProgram);

        for (Row row : rows) {
            monitor.checkCancelled();
            if (!sourceAddresses.add(row.sourceAddress)) {
                result.duplicateSource++;
            }
            if (!destinationAddresses.add(row.destinationAddress)) {
                result.duplicateDestination++;
            }
            if (!mappedNames.add(row.functionName)) {
                result.duplicateName++;
            }

            Address sourceAddress = sourceProgram.getAddressFactory().getAddress(
                row.sourceAddress);
            Function sourceFunction = sourceProgram.getFunctionManager().getFunctionAt(
                sourceAddress);
            if (sourceFunction == null) {
                result.sourceMissing++;
            }
            else if (!row.functionName.equals(sourceFunction.getName())) {
                result.sourceNameMismatch++;
            }

            Address destinationAddress = destinationProgram.getAddressFactory().getAddress(
                row.destinationAddress);
            Function destinationFunction =
                destinationProgram.getFunctionManager().getFunctionAt(destinationAddress);
            if (destinationFunction == null) {
                result.destinationMissing++;
                continue;
            }

            String destinationName = destinationFunction.getName();
            if (!row.functionName.equals(destinationName) &&
                    !isDefaultName(destinationName)) {
                result.destinationConflict++;
            }
            Address occupiedAddress = occupiedNames.get(row.functionName);
            if (occupiedAddress != null && !occupiedAddress.equals(destinationAddress)) {
                result.nameConflict++;
            }
        }
        return result;
    }

    private void printValidation(Validation validation, int rows) {
        println("VALIDATED_ROWS=" + rows);
        println("SOURCE_MISSING=" + validation.sourceMissing);
        println("SOURCE_NAME_MISMATCH=" + validation.sourceNameMismatch);
        println("DESTINATION_MISSING=" + validation.destinationMissing);
        println("DESTINATION_CONFLICT=" + validation.destinationConflict);
        println("NAME_CONFLICT=" + validation.nameConflict);
        println("DUPLICATE_SOURCE=" + validation.duplicateSource);
        println("DUPLICATE_DESTINATION=" + validation.duplicateDestination);
        println("DUPLICATE_NAME=" + validation.duplicateName);
    }

    private void ensureTags(Program program, List<Row> rows,
            Map<String, String> comments) {
        Set<String> required = new HashSet<>();
        for (Row row : rows) {
            required.add(row.evidence);
            required.addAll(splitFlags(row.reviewFlags));
        }

        FunctionTagManager manager =
            program.getFunctionManager().getFunctionTagManager();
        for (String name : required) {
            FunctionTag tag = manager.getFunctionTag(name);
            String comment = comments.getOrDefault(name,
                "WC1-to-WC2 transfer evidence or review flag.");
            if (tag == null) {
                manager.createFunctionTag(name, comment);
            }
            else if (!comment.equals(tag.getComment())) {
                tag.setComment(comment);
            }
        }
    }

    private Map<String, String> tagComments() {
        Map<String, String> result = new LinkedHashMap<>();
        result.put("WC1-auto-vt",
            "Conservative Ghidra Auto Version Tracking match from WC1.");
        result.put("WC1-callgraph-transfer",
            "Name inferred from WC1/WC2 caller and callee anchors in the Version Tracking graph; inspect the repeatable comment for the WC1 source address.");
        result.put("WC1-callgraph-propagated",
            "Name inferred by iterative caller/callee propagation from already accepted WC1/WC2 anchors; review the repeatable comment for the source address.");
        result.put("WC1-callgraph-single",
            "Name inferred from a unique one-edge caller/callee candidate with supporting structural evidence; review before relying on it.");
        result.put("WC1-callgraph-single-low",
            "Lower-confidence unique one-edge caller/callee inference retained as a manual-review lead.");
        result.put("WC1-order-exact",
            "Exact local-order match between consecutive accepted WC1/WC2 anchors; review the repeatable comment.");
        result.put("WC1-fuzzy-high",
            "Initial structural match: score >= 0.85 or at least 2 graph anchors. Heuristic, not proof; inspect the repeatable comment.");
        result.put("WC1-fuzzy-medium",
            "Initial structural match: score >= 0.615 or 1 graph anchor. Heuristic, not proof; inspect the repeatable comment.");
        result.put("WC1-fuzzy-low",
            "Initial structural match below the medium threshold. Review before relying on the name.");
        result.put("WC1-fuzzy-followup",
            "Second-pass match with structural score >= 0.50 or at least 2 graph anchors. Review before relying on the name.");
        result.put("WC1-fuzzy-very-low",
            "Permissive second-pass structural/call-graph match. Treat as a review lead, not an established identity.");
        result.put("WC1-review-module-mismatch",
            "Heuristic WC1-to-WC2 mapping crosses the inferred core/ix/CRT module region. Review manually; this is a warning, not proof of a bad match.");
        return result;
    }

    private String makeAuditComment(Row row) {
        if (row.auditComment != null && !row.auditComment.isEmpty()) {
            return row.auditComment;
        }
        return "[WC1 transfer] " + row.evidence + " from " + row.functionName +
            " @" + row.sourceAddress + "; similarity=" + row.similarity +
            ", confidence=" + row.confidence + ", correlator=" + row.correlator;
    }

    private String mergeAuditComment(String oldComment, String auditComment) {
        if (oldComment == null || oldComment.isEmpty() ||
                oldComment.startsWith("[WC1 transfer]")) {
            return auditComment;
        }
        if (oldComment.contains(auditComment)) {
            return oldComment;
        }
        return oldComment + "\n\n" + auditComment;
    }

    private Map<String, Address> collectOccupiedNameAddresses(Program program) {
        Map<String, Address> result = new HashMap<>();
        FunctionIterator iterator = program.getFunctionManager().getFunctions(true);
        while (iterator.hasNext()) {
            Function function = iterator.next();
            if (!isDefaultName(function.getName())) {
                result.put(function.getName(), function.getEntryPoint());
            }
        }
        return result;
    }

    private Set<String> collectOccupiedNames(Program program) {
        return new HashSet<>(collectOccupiedNameAddresses(program).keySet());
    }

    private List<String> splitFlags(String flags) {
        List<String> result = new ArrayList<>();
        if (flags == null || flags.isEmpty()) {
            return result;
        }
        for (String value : flags.split("\\|")) {
            if (!value.isEmpty()) {
                result.add(value);
            }
        }
        return result;
    }

    private String unescape(String value) throws IOException {
        StringBuilder result = new StringBuilder();
        for (int index = 0; index < value.length(); index++) {
            char character = value.charAt(index);
            if (character != '\\') {
                result.append(character);
                continue;
            }
            if (++index >= value.length()) {
                throw new IOException("Trailing escape in map field.");
            }
            char escaped = value.charAt(index);
            if (escaped == 't') {
                result.append('\t');
            }
            else if (escaped == 'r') {
                result.append('\r');
            }
            else if (escaped == 'n') {
                result.append('\n');
            }
            else if (escaped == '\\') {
                result.append('\\');
            }
            else {
                throw new IOException("Unknown map escape: \\" + escaped);
            }
        }
        return result.toString();
    }

    private void validateHash(String label, String actual, String expected)
            throws IOException {
        if (actual == null || !expected.equalsIgnoreCase(actual)) {
            throw new IOException(label + " SHA-256 mismatch: expected " +
                expected + ", got " + actual);
        }
    }

    private boolean hasArgument(String[] arguments, String wanted) {
        for (String argument : arguments) {
            if (wanted.equalsIgnoreCase(argument)) {
                return true;
            }
        }
        return false;
    }

    private boolean hasTag(Function function, String wanted) {
        for (FunctionTag tag : function.getTags()) {
            if (wanted.equals(tag.getName())) {
                return true;
            }
        }
        return false;
    }

    private boolean isDefaultName(String name) {
        return name.startsWith("FUN_") || name.startsWith("thunk_FUN_");
    }

    private boolean same(String first, String second) {
        return first == null ? second == null : first.equals(second);
    }
}
