// Transfer WC1 function names into WC2 with Ghidra's Version Tracking engine.
//
// The script must be run with /WC2/WC2.EXE as currentProgram.  It creates a
// backup and a Version Tracking session before applying any names.  Only
// function names are applied; prototypes, variables, comments, labels, data
// types, and calling conventions are deliberately excluded.
//@category Wing Commander

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import ghidra.app.script.GhidraScript;
import ghidra.feature.vt.api.db.VTSessionDB;
import ghidra.feature.vt.api.main.VTAssociation;
import ghidra.feature.vt.api.main.VTAssociationStatus;
import ghidra.feature.vt.api.main.VTMatch;
import ghidra.feature.vt.api.main.VTMatchSet;
import ghidra.feature.vt.api.main.VTSession;
import ghidra.feature.vt.api.util.VTOptions;
import ghidra.feature.vt.gui.actions.AutoVersionTrackingTask;
import ghidra.feature.vt.gui.util.VTMatchApplyChoices.CallingConventionChoices;
import ghidra.feature.vt.gui.util.VTMatchApplyChoices.CommentChoices;
import ghidra.feature.vt.gui.util.VTMatchApplyChoices.FunctionNameChoices;
import ghidra.feature.vt.gui.util.VTMatchApplyChoices.FunctionSignatureChoices;
import ghidra.feature.vt.gui.util.VTMatchApplyChoices.LabelChoices;
import ghidra.feature.vt.gui.util.VTMatchApplyChoices.ParameterDataTypeChoices;
import ghidra.feature.vt.gui.util.VTMatchApplyChoices.ReplaceChoices;
import ghidra.feature.vt.gui.util.VTMatchApplyChoices.ReplaceDataChoices;
import ghidra.feature.vt.gui.util.VTMatchApplyChoices.SourcePriorityChoices;
import ghidra.feature.vt.gui.util.VTOptionDefines;
import ghidra.framework.model.DomainFile;
import ghidra.framework.model.DomainFolder;
import ghidra.framework.model.ProjectData;
import ghidra.framework.options.ToolOptions;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Program;

public class TransferWC1NamesToWC2 extends GhidraScript {

    private static final String SOURCE_PATH = "/WC1%2FWC1.EXE";
    private static final String DESTINATION_PATH = "/WC2/WC2.EXE";
    private static final String SESSION_FOLDER_PATH = "/WC2";
    private static final String SESSION_NAME = "WC1-to-WC2-auto-v1";
    private static final String BACKUP_FOLDER_NAME = "backups";
    private static final String BACKUP_NAME = "WC2-before-WC1-name-transfer";

    @Override
    public void run() throws Exception {
        if (currentProgram == null) {
            printerr("Open " + DESTINATION_PATH + " before running this script.");
            return;
        }
        if (!DESTINATION_PATH.equals(currentProgram.getDomainFile().getPathname())) {
            printerr("Refusing to run against " +
                currentProgram.getDomainFile().getPathname() + "; expected " +
                DESTINATION_PATH + ".");
            return;
        }
        if (!currentProgram.canSave()) {
            printerr("The WC2 destination program is read-only.");
            return;
        }

        ProjectData projectData = state.getProject().getProjectData();
        DomainFile sourceFile = projectData.getFile(SOURCE_PATH);
        DomainFolder sessionFolder = projectData.getFolder(SESSION_FOLDER_PATH);
        if (sourceFile == null || sessionFolder == null) {
            printerr("Could not find source " + SOURCE_PATH + " or folder " +
                SESSION_FOLDER_PATH + ".");
            return;
        }
        if (!Program.class.isAssignableFrom(sourceFile.getDomainObjectClass())) {
            printerr(SOURCE_PATH + " is not a Ghidra Program.");
            return;
        }
        if (sessionFolder.getFile(SESSION_NAME) != null) {
            printerr("Version Tracking session already exists: " +
                SESSION_FOLDER_PATH + "/" + SESSION_NAME);
            return;
        }

        Map<Address, String> namesBefore = snapshotFunctionNames(currentProgram);
        int defaultBefore = countDefaultNames(currentProgram);

        // End the transaction automatically opened for a GhidraScript.  Version
        // Tracking owns its own transactions and cannot run inside this one.
        end(true);
        currentProgram.save("Before WC1-to-WC2 name transfer", monitor);
        createBackupIfMissing(sessionFolder, currentProgram.getDomainFile());

        Program sourceProgram = null;
        VTSession session = null;
        try {
            sourceProgram = (Program) sourceFile.getDomainObject(this, false, false, monitor);
            session = new VTSessionDB(SESSION_NAME, sourceProgram, currentProgram, this);
            sessionFolder.createFile(SESSION_NAME, session, monitor);

            ToolOptions options = createNameOnlyOptions();
            AutoVersionTrackingTask task = new AutoVersionTrackingTask(session, options);
            task.run(monitor);

            currentProgram.save("Applied WC1 function names with Auto Version Tracking", monitor);
            session.save();

            int renamed = countChangedNames(currentProgram, namesBefore);
            int defaultAfter = countDefaultNames(currentProgram);
            printSessionSummary(session);
            println("AUTO_VT_STATUS=" + task.getStatusMsg());
            println("FUNCTION_NAMES_CHANGED=" + renamed);
            println("DEFAULT_NAMES_BEFORE=" + defaultBefore);
            println("DEFAULT_NAMES_AFTER=" + defaultAfter);
            println("BACKUP=" + SESSION_FOLDER_PATH + "/" + BACKUP_FOLDER_NAME +
                "/" + BACKUP_NAME);
            println("SESSION=" + SESSION_FOLDER_PATH + "/" + SESSION_NAME);
        }
        finally {
            if (session != null) {
                session.release(this);
            }
            if (sourceProgram != null) {
                sourceProgram.release(this);
            }
        }
    }

    private void createBackupIfMissing(DomainFolder sessionFolder, DomainFile destinationFile)
            throws Exception {
        DomainFolder backupFolder = sessionFolder.getFolder(BACKUP_FOLDER_NAME);
        if (backupFolder == null) {
            backupFolder = sessionFolder.createFolder(BACKUP_FOLDER_NAME);
        }
        if (backupFolder.getFile(BACKUP_NAME) != null) {
            println("Using existing backup " + backupFolder.getPathname() + "/" + BACKUP_NAME);
            return;
        }
        DomainFile copiedFile = destinationFile.copyTo(backupFolder, monitor);
        copiedFile.setName(BACKUP_NAME);
        println("Created backup " + copiedFile.getPathname());
    }

    private ToolOptions createNameOnlyOptions() {
        ToolOptions options = new VTOptions("WC1 to WC2 name transfer");

        options.setBoolean(VTOptionDefines.CREATE_IMPLIED_MATCHES_OPTION, true);
        options.setBoolean(VTOptionDefines.RUN_EXACT_SYMBOL_OPTION, true);
        options.setBoolean(VTOptionDefines.RUN_EXACT_DATA_OPTION, true);
        options.setBoolean(VTOptionDefines.RUN_EXACT_FUNCTION_BYTES_OPTION, true);
        options.setBoolean(VTOptionDefines.RUN_EXACT_FUNCTION_INST_OPTION, true);
        options.setBoolean(VTOptionDefines.RUN_DUPE_FUNCTION_OPTION, true);
        options.setBoolean(VTOptionDefines.RUN_REF_CORRELATORS_OPTION, true);
        options.setInt(VTOptionDefines.DATA_CORRELATOR_MIN_LEN_OPTION, 5);
        options.setInt(VTOptionDefines.SYMBOL_CORRELATOR_MIN_LEN_OPTION, 3);
        options.setInt(VTOptionDefines.FUNCTION_CORRELATOR_MIN_LEN_OPTION, 10);
        options.setInt(VTOptionDefines.DUPE_FUNCTION_CORRELATOR_MIN_LEN_OPTION, 10);
        options.setBoolean(VTOptionDefines.APPLY_IMPLIED_MATCHES_OPTION, true);
        options.setInt(VTOptionDefines.MIN_VOTES_OPTION, 2);
        options.setInt(VTOptionDefines.MAX_CONFLICTS_OPTION, 0);
        options.setDouble(VTOptionDefines.REF_CORRELATOR_MIN_SCORE_OPTION, 0.95);
        options.setDouble(VTOptionDefines.REF_CORRELATOR_MIN_CONF_OPTION, 10.0);

        // Auto Version Tracking still records all associations in the session,
        // but only this markup kind is allowed to modify WC2.
        options.setEnum(VTOptionDefines.FUNCTION_NAME,
            FunctionNameChoices.REPLACE_DEFAULT_ONLY);
        options.setEnum(VTOptionDefines.FUNCTION_SIGNATURE,
            FunctionSignatureChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.FUNCTION_RETURN_TYPE,
            ParameterDataTypeChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.CALLING_CONVENTION,
            CallingConventionChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.INLINE, ReplaceChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.NO_RETURN, ReplaceChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.CALL_FIXUP, ReplaceChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.VAR_ARGS, ReplaceChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.PARAMETER_DATA_TYPES,
            ParameterDataTypeChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.PARAMETER_NAMES, SourcePriorityChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.PARAMETER_COMMENTS, CommentChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.LABELS, LabelChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.PLATE_COMMENT, CommentChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.PRE_COMMENT, CommentChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.END_OF_LINE_COMMENT, CommentChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.REPEATABLE_COMMENT, CommentChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.POST_COMMENT, CommentChoices.EXCLUDE);
        options.setEnum(VTOptionDefines.DATA_MATCH_DATA_TYPE, ReplaceDataChoices.EXCLUDE);
        return options;
    }

    private Map<Address, String> snapshotFunctionNames(Program program) {
        Map<Address, String> result = new HashMap<>();
        FunctionIterator functions = program.getFunctionManager().getFunctions(true);
        while (functions.hasNext()) {
            Function function = functions.next();
            result.put(function.getEntryPoint(), function.getName());
        }
        return result;
    }

    private int countChangedNames(Program program, Map<Address, String> namesBefore) {
        int count = 0;
        FunctionIterator functions = program.getFunctionManager().getFunctions(true);
        while (functions.hasNext()) {
            Function function = functions.next();
            String oldName = namesBefore.get(function.getEntryPoint());
            if (oldName != null && !oldName.equals(function.getName())) {
                count++;
            }
        }
        return count;
    }

    private int countDefaultNames(Program program) {
        int count = 0;
        FunctionIterator functions = program.getFunctionManager().getFunctions(true);
        while (functions.hasNext()) {
            String name = functions.next().getName();
            if (name.startsWith("FUN_") || name.startsWith("thunk_FUN_")) {
                count++;
            }
        }
        return count;
    }

    private void printSessionSummary(VTSession session) {
        int matches = 0;
        Set<String> acceptedAssociations = new HashSet<>();
        for (VTMatchSet matchSet : session.getMatchSets()) {
            for (VTMatch match : matchSet.getMatches()) {
                matches++;
                VTAssociation association = match.getAssociation();
                if (association.getStatus() == VTAssociationStatus.ACCEPTED) {
                    acceptedAssociations.add(association.getSourceAddress() + "->" +
                        association.getDestinationAddress());
                }
            }
        }
        println("VT_MATCH_RECORDS=" + matches);
        println("VT_ACCEPTED_ASSOCIATIONS=" + acceptedAssociations.size());
    }
}
