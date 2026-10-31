/*
 *  Communication menus and video transmissions.
 *
 *  Address range 430bc0-4318ef (see docs/ORDER.md).
 */
#include "wc1.h"

short nCommMenuChoiceCount = -1; /* 0x0046af60 */
short nCommMenuReuseMode = 0; /* 0x0046af64 */
unsigned char bMessageSpeed = 2; /* 0x0046af68 */
static signed char cPendingCommMenuAction = 1; /* 0x0046af6c */
unsigned char bRadioSilence = 0; /* 0x0046af70 */
unsigned char bVideoImagesSuppressed = 0; /* 0x0046af74 */
unsigned char bCommVideoEnabled = 1; /* 0x0046af78 */

/* 0x0046af80 */
const char *apszKilrathiAceNames[4] = {
    "Bhurak",
    "Dakhath",
    "Khajja",
    "Bakhtosh"
};
/* 0x0046af90 */
static const char *apszCommMenuText[13] = {
    "Never mind...",
    "Attack my target!",
    "Help me out here",
    "Return to base.",
    "Die furball!",
    "Slag off!",
    "Bite it cat face.",
    "Break and attack.",
    "Keep formation!",
    "Form on my wing.",
    "Keep radio silence",
    "Broadcast freely",
    "Request Landing"
};
signed char cCommMenuRecipient = -1; /* 0x0046afc4 */
short nCommSpeakerObject = 0; /* 0x0046afc8 */
short nCommSpeakerRating = 0; /* 0x0046afcc */
short nCommPortraitIndex = -1; /* 0x0046afd0 */


/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned char *apObjShapeArray[SPACE_OBJECT_COUNT]; /* 0x0059d2f0 */
short nPitchInput; /* 0x0059d3f0 */
short nYawInputValue; /* 0x0059d3f2 */
short nCurrentRollInputValue; /* 0x0059d3f4 */
short asShipWingLeaderArray[16]; /* 0x0059d400 */
/* The Win32 object allocator reserves slots 0 through 9 for live ships. */
short aasShipArmor[10][4]; /* 0x0059d420 */
short asShipWpnEnergyList[10]; /* 0x0059d470 */
signed char acFormationMemberLists[16]; /* 0x0059d490 */
signed char acShipIonDriveDmg[16]; /* 0x0059d4a0 */
short anShipMissionShips[16]; /* 0x0059d4b0 */
FixedVector vToTgt; /* 0x0059d4d0 */
short asCannedCommands[16]; /* 0x0059d4e0 */

/* Function start: 0x430BC0 */
short get_face(short rating, unsigned int side)
{
    if (rating == -1)
        return 13 + (side < SIDE_KILRATHI ? -1 : 0);
    if (side == SIDE_KILRATHI)
        rating--;
    return rating;
}

/* Function start: 0x430BF0 */
void LoadCommPortraitShape(short face, signed char alternate)
{
    short section;

    if (face >= 0 && face <= 7)
        section = face + 1;
    else if ((face > 7 && face < 12) || face == 13)
        section = 10;
    else
        section = -1;
    if (section != -1)
        apCommunicationPortraitShapesTable[face] =
            FetchDiskPacketRetrying(11, section,
                                                     (short)alternate);
}

/* Function start: 0x430C50 */
void ResetCommMenuChoices(short reuse)
{
    short choice;

    if (reuse == 0) {
        memset(abCommMenuChoiceCommands, -1,
               sizeof(abCommMenuChoiceCommands));
        for (choice = 0; choice < 7; choice++) {
            apszCommMenuChoiceTextTable[choice] = 0;
        }
    }
    nCommMenuChoiceCount = 0;
    nCommMenuReuseMode = reuse;
}

/* Function start: 0x430CA0 */
short IsCommMenuIdle(void)
{
    return nCommMenuChoiceCount == 0;
}

/* Function start: 0x430CB0 */
void AppendCommMenuChoice(const char *text, short command)
{
    int index;

    if (nCommMenuReuseMode == 1) {
        if (apszCommMenuChoiceTextTable[nCommMenuChoiceCount] != text ||
            abCommMenuChoiceCommands[nCommMenuChoiceCount] != command)
            nCommMenuReuseMode = 0;
    }
    index = nCommMenuChoiceCount++;
    apszCommMenuChoiceTextTable[index] = text;
    abCommMenuChoiceCommands[index] = (signed char)command;
}

/* Function start: 0x430D30 */
void SendCommMenuChoice(short i)
{
    AppendCommMenuChoice(apszCommMenuText[i], i);
}

/* Function start: 0x430D50 */
void OpenCommMenuForTarget(const char *heading, const char *message)
{
    CockpitMessage(message, cYellowColour, -1);
    pszCommMenuHeading = heading;
}

/* Function start: 0x430D80 */
short IsCommChoiceMenuOpen(void)
{
    return get_mode(1) == 4;
}

/* Function start: 0x430DA0 */
short GetPendingMenuAction(void)
{
    return (short)cPendingCommMenuAction;
}

/* Function start: 0x430DB0 */
void SetPendingMenuAction(unsigned char v)
{
    cPendingCommMenuAction = (signed char)v;
}

/* Function start: 0x430DC0 */
void OpenCommRecipientMenu(void)
{
    push_mode(1, 4);
    SetPendingMenuAction(1);
}

/* Function start: 0x430DE0 */
void CloseCommChoiceMenu(void)
{
    if (get_mode(1) == 4) {
        pop_mode(1);
        return;
    }
    ShutdownEventManager();
    exit_squadron("!stop");
}

/* Function start: 0x430E10 */
short wingman_dead(void)
{
    return nYourWingman == -1;
}

/* Function start: 0x430E30 */
short have_target(void)
{
    return unactive(acShipTargetTable[0]) == 0;
}

/* Function start: 0x430E50 */
unsigned short CanOpenCommMenu(void)
{
    if (have_target() == 0) {
        if (wingman_dead())
            return 0;
    }
    return 1;
}

/* Function start: 0x430E70 */
void SelectCommRecipient(short recipient)
{
    cCommMenuRecipient = recipient;
    SetPendingMenuAction(2);
}

/* Function start: 0x430E90 */
void BuildCommunicationRecipientMenu(void)
{
    short target;

    ResetCommMenuChoices(nCommMenuReuseMode);
    OpenCommMenuForTarget("VID-COM SYSTEM\n\nSend message to?\n\n",
                          "SELECT");
    if (wingman_dead() != 0) {
        SelectCommRecipient(acShipTargetTable[0]);
        return;
    }
    if (have_target() == 0 ||
        acShipTargetTable[0] == nYourWingman) {
        SelectCommRecipient(nYourWingman);
        return;
    }
    AppendCommMenuChoice(
        apWingmanPilotsList[
            (signed char)acShipRating[
                nYourWingman]]->callsign,
        1);
    target = acShipTargetTable[0];
    if (target != -1) {
        if (aeShipSide[target] == SIDE_KILRATHI &&
            aeObjectClass[target] == OBJECT_CLASS_SHIP)
            AppendCommMenuChoice("ENEMY TARGET", 2);
        else if (aeShipSide[target] == SIDE_IMPERIAL &&
                 ((aeObjectClass[target] == OBJECT_CLASS_SHIP &&
                   any_enemy(0, 14000) != 0) ||
                  aeObjectTypeList[target] ==
                      OBJECT_TYPE_TIGERS_CLAW))
            AppendCommMenuChoice(
                aObjectTypeData[
                    aeObjectTypeList[acShipTargetTable[0]]].displayName,
                3);
    }
    SendCommMenuChoice(0);
}

/* Function start: 0x430FC0 */
void BuildCommunicationCommandMenu(void)
{
    signed char rating;
    const char *name;

    ResetCommMenuChoices(nCommMenuReuseMode);
    if (cCommMenuRecipient == nYourWingman) {
        if (aeShipObjectiveList[nYourWingman] ==
                OBJECTIVE_HOLD_FORMATION &&
            any_enemy(0, 14000) != 0) {
            SendCommMenuChoice(7);
        }
        if (nAutoEngageTimer != -1)
            SendCommMenuChoice(8);
        else if (aeShipObjectiveList[nYourWingman] !=
                     OBJECTIVE_HOLD_FORMATION)
            SendCommMenuChoice(9);
        if (bRadioSilence != 0)
            SendCommMenuChoice(11);
        else
            SendCommMenuChoice(10);
    }
    if (aeShipSide[cCommMenuRecipient] ==
            aeShipSide[0]) {
        if (aeObjectTypeList[cCommMenuRecipient] ==
                OBJECT_TYPE_TIGERS_CLAW &&
            bLandingAuthorized == 0) {
            SendCommMenuChoice(12);
        }
        if (have_target() != 0 &&
            aeShipSide[acShipTargetTable[0]] ==
                SIDE_KILRATHI) {
            SendCommMenuChoice(1);
        }
        if (evaluate_damage(0) < 50 && any_enemy(0, 14000) != 0)
            SendCommMenuChoice(2);
    }
    if (cCommMenuRecipient == nYourWingman)
        SendCommMenuChoice(3);
    if (aeShipSide[cCommMenuRecipient] ==
            SIDE_KILRATHI) {
        SendCommMenuChoice(4);
        SendCommMenuChoice(5);
        SendCommMenuChoice(6);
    }
    if (IsCommMenuIdle() != 0)
        CloseCommChoiceMenu();
    else
        SendCommMenuChoice(0);

    if (IsCommChoiceMenuOpen() != 0) {
        strcpy(szCommMenuHeadingBuf,
               "VID-COM SYSTEM\n\nTo: ");
        rating = acShipRating[cCommMenuRecipient];
        if (rating == -1) {
            name = aObjectTypeData[
                aeObjectTypeList[
                    cCommMenuRecipient]].displayName;
        } else if (rating < 8) {
            name = apWingmanPilotsList[rating]->callsign;
        } else {
            name = apszKilrathiAceNames[rating - 9];
        }
        strcat(szCommMenuHeadingBuf, name);
        strcat(szCommMenuHeadingBuf, "\n");
        OpenCommMenuForTarget(szCommMenuHeadingBuf,
                              "CHOOSE");
    }
}

/* Function start: 0x431200 */
void RefreshCommunicationMenu(void)
{
    if (IsCommChoiceMenuOpen() != 0) {
        if (GetPendingMenuAction() == 1)
            BuildCommunicationRecipientMenu();
        if (GetPendingMenuAction() == 2)
            BuildCommunicationCommandMenu();
        if (nCommMenuReuseMode == 0)
            InvalidateVduMode(1);
    }
}

/* Function start: 0x431240 */
void HandleCommunicationMenuRequest(void)
{
    if (IsCommChoiceMenuOpen() != 0)
        CloseCommChoiceMenu();
    if (message_showing() == 0 &&
        IsCommChoiceMenuOpen() == 0 && CanOpenCommMenu() != 0) {
        OpenCommRecipientMenu();
        ResetCommMenuChoices(0);
        RefreshCommunicationMenu();
    }
}

/* Function start: 0x431290 */
void show_communications_disp(void)
{
    signed char choice;
#ifdef SDL_PORT
    int selectedChoice;
    unsigned char normalColour;
#endif

    if (IsCommChoiceMenuOpen() == 0)
        HandleCommunicationMenuRequest();
    if (IsCommChoiceMenuOpen() != 0) {
        set_new_vdu(1);
        DrawTextAt(&stRightVduTxtContext, stRightVduViewport.left,
                   stRightVduViewport.top, pszCommMenuHeading, 2);
#ifdef SDL_PORT
        selectedChoice = SdlGetCommunicationMenuSelection();
        normalColour = stRightVduTxtContext.colour;
#endif
        for (choice = 0; choice < nCommMenuChoiceCount; choice++) {
#ifdef SDL_PORT
            if ((int)choice == selectedChoice)
                stRightVduTxtContext.colour = cYellowColour;
#endif
            DrawFormattedText("\n%d %s", choice + 1,
                              apszCommMenuChoiceTextTable[choice]);
#ifdef SDL_PORT
            stRightVduTxtContext.colour = normalColour;
#endif
        }
        DrawSpriteDefault(&stRightVduViewport,
                          (short)(stRightVduViewport.left + 36),
                          (short)(stRightVduViewport.top + 10),
                          pCommMenuPointerShape, 0x19);
        nCommMenuReuseMode = 1;
    }
}

/* Function start: 0x431350 */
unsigned int Chosen_communicate_option(short choice)
{
    int action;

    PlaySfxWaveFileByNumber(0x19, -1, 0);
    action = GetPendingMenuAction();
    switch (action) {
    case 0:
        CloseCommChoiceMenu();
        return 0;
    case 1:
        if (abCommMenuChoiceCommands[choice] == 0) {
            CloseCommChoiceMenu();
            return 0;
        }
        if (abCommMenuChoiceCommands[choice] == 1) {
            SelectCommRecipient(nYourWingman);
            RefreshCommunicationMenu();
            return 0;
        }
        SelectCommRecipient(acShipTargetTable[0]);
        RefreshCommunicationMenu();
        return 0;
    case 2:
        CloseCommChoiceMenu();
        request(0, (short)cCommMenuRecipient,
                (short)abCommMenuChoiceCommands[choice]);
        return 0;
    }
    return 0;
}

/* Function start: 0x431400 */
void talk_equiv(void)
{
    RefreshCommunicationMenu();
}

/* Function start: 0x431410 */
void FreeCommDisplayResources(void)
{
#ifdef SDL_PORT
    /* The original indexes the portrait table with its -1 inactive sentinel,
       aliasing the final two palette-allocation words at 0x0059E17C. */
    if (nCommPortraitIndex != -1)
#endif
    FreePacketAndClear(&apCommunicationPortraitShapesTable[
        nCommPortraitIndex], 0);
    FreePacketAndClear(&pConfedCommBackground, 0);
    FreePacketAndClear(&pKilrathiCommBackground, 0);
    FreePacketAndClear(&pCommStaticShape, 0);
    nCommSpeakerRating = -1;
    nCommSpeakerObject = -1;
    nCommPortraitIndex = -1;
}

/* Function start: 0x431470 */
void EndCommSessionWithWingman(void)
{
#ifdef SDL_PORT
    if (nCommPortraitIndex != -1 &&
#else
    if (
#endif
        apCommunicationPortraitShapesTable[nCommPortraitIndex] != 0)
        malf_noise(1, 1, 12, 23, 1);
    FreeCommDisplayResources();
    if (get_mode(1) == 6)
        pop_mode(1);
}

/* Function start: 0x4314C0 */
void EndCommMenu(void)
{
    clear_message_time();
    if (get_mode(1) == 6)
        EndCommSessionWithWingman();
    pszPendingHudMessage = 0;
}

/* Function start: 0x4314F0 */
void ShowCentredPrompt(char *text, unsigned short arg)
{
    DosStrcpy(szHudMessageBuf, text);
    SetHudMessageText(szHudMessageBuf,
                      cYellowColour, arg);
}

/* Function start: 0x431520 */
short LoadCommDisplayResources(short rating, enum Side side)
{
    short loaded;

    loaded = 1;
    switch (side) {
    case SIDE_IMPERIAL:
        if (pConfedCommBackground == 0)
            pConfedCommBackground =
                FetchDiskPacketRetrying(11, 0, 0);
        loaded = pConfedCommBackground != 0;
        break;
    case SIDE_KILRATHI:
        if (pKilrathiCommBackground == 0)
            pKilrathiCommBackground =
                FetchDiskPacketRetrying(11, 9, 0);
        loaded = pKilrathiCommBackground != 0;
        break;
    }
    if (pCommStaticShape == 0)
        pCommStaticShape =
            FetchDiskPacketRetrying(11, 11, 0);
    if (loaded != 0 && pCommStaticShape != 0)
        return 1;
    return 0;
}

/* Function start: 0x4315C0 */
char *ExpandCommMessageTokens(const char *text)
{
    char *destination;
    const char *marker;
    short length;

    szTxtWorkspaceBuffer[0] = '\0';
    while ((marker = DosStrchr(text, '$')) != 0) {
        destination = DosStrchr(szTxtWorkspaceBuffer, '\0');
        while (marker != text)
            *destination++ = *text++;
        *destination = '\0';
        marker++;
        switch (*marker++) {
        case 'C':
            DosStrcat(
                szTxtWorkspaceBuffer,
                stCampaignState.currentPilot->callsign);
            break;
        case 'N':
        case 'P':
            DosStrcat(szTxtWorkspaceBuffer,
                      stCampaignState.currentPilot->name);
            break;
        case 'R':
            DosStrcat(szTxtWorkspaceBuffer,
                      apszPilotRankNames[
                          stCampaignState.currentPilot->rank]);
            length = DosStrlen(szTxtWorkspaceBuffer);
            if (szTxtWorkspaceBuffer[length - 1] == '.' &&
                *marker == '.') {
                length = DosStrlen(szTxtWorkspaceBuffer);
                szTxtWorkspaceBuffer[length - 1] = '\0';
            }
            break;
        }
        text = marker;
    }
    DosStrcat(szTxtWorkspaceBuffer, text);
    return szTxtWorkspaceBuffer;
}

/* Function start: 0x4316E0 */
void real_vid_transmit(short obj, short message)
{
    char text[84];
    char *expandedText;
    char *speech;

#ifdef SDL_PORT
    if (SdlSwcFlightActive()) {
        SdlSwcTransmit(obj, message);
        return;
    }
#endif
    nCommSpeakerObject = obj;
    nCommSpeakerRating =
        (short)acShipRating[obj];
    nCommPortraitIndex =
        get_face(nCommSpeakerRating,
                 aeShipSide[obj]);
    if (nCommPortraitIndex == -1)
        return;
    if (bCommVideoEnabled != 0 && bVideoImagesSuppressed == 0) {
        if (apCommunicationPortraitShapesTable[
                nCommPortraitIndex] == 0)
            LoadCommPortraitShape(nCommPortraitIndex, 0);
        if (apCommunicationPortraitShapesTable[
                nCommPortraitIndex] != 0 &&
            LoadCommDisplayResources(nCommSpeakerRating,
                aeShipSide[
                    nCommSpeakerObject]) != 0) {
            push_mode(1, 6);
            malf_noise(1, 3, 12, 23, 1);
            if (aeShipSide[nCommSpeakerObject] == SIDE_IMPERIAL)
                DrawSpriteDefault(&stRightVduViewport, stRightVduViewport.left,
                                  stRightVduViewport.top,
                                  pConfedCommBackground, 0);
            else
                DrawSpriteDefault(&stRightVduViewport, stRightVduViewport.left,
                                  stRightVduViewport.top,
                                  pKilrathiCommBackground, 0);
            DrawSpriteDefault(
                &stRightVduViewport, stRightVduViewport.left,
                stRightVduViewport.top,
                apCommunicationPortraitShapesTable[
                    nCommPortraitIndex],
                0);
        }
    }
    speech = aapszPilotSpeeches[
        nCommPortraitIndex][message];
    if (nCommSpeakerRating >= 0 &&
        nCommSpeakerRating <= 7) {
#ifdef SDL_PORT
        /* MSVC 4.20 accepts %Fs as its legacy far-string conversion. */
        sprintf(text, "%s: %s",
                apWingmanPilotsList[
                    nCommSpeakerRating]->callsign,
                speech);
#else
        sprintf(text, "%s: %Fs",
                apWingmanPilotsList[
                    nCommSpeakerRating]->callsign,
                speech);
#endif
    } else if (nCommSpeakerRating >= 9 &&
               nCommSpeakerRating <= 12) {
#ifdef SDL_PORT
        sprintf(text, "%s: %s",
                apszKilrathiAceNames[
                    nCommSpeakerRating - 9],
                speech);
#else
        sprintf(text, "%s: %Fs",
                apszKilrathiAceNames[
                    nCommSpeakerRating - 9],
                speech);
#endif
    } else {
#ifdef SDL_PORT
        sprintf(text, "%s: %s",
                aObjectTypeData[aeObjectTypeList[obj]].displayName,
                speech);
#else
        sprintf(text, "%s: %Fs",
                aObjectTypeData[aeObjectTypeList[obj]].displayName,
                speech);
#endif
    }
    expandedText = ExpandCommMessageTokens(text);
    ShowCentredPrompt(expandedText, (unsigned short)MeasureMessageWidth(text));
}
