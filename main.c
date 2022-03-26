// main.c

#include "common.h"
#include "io.h"
#include "gui.h"
#include "motor.h"

/* Globals */

/* Contains info relative to latest loaded RESULTS file */
SEARCHDATA SearchDataCurrent = { 0 };
/* Details of currently inspected results */
RESULTDATA ResultDataCurrent = { 0 };
/**/
REVERSEDSEED ReversedSeedCurrent = { 0 };
/* Used to confirm if a search should be launched in the search thread */
BOOL AuthorizeSearchThreads[SEARCH_THREADS_MAX] = { FALSE };
BOOL AuthorizeSearch = FALSE;
BOOL PauseSearch = FALSE;
BOOL AbortSearch = FALSE;

/* Results display modes, either files or results from a file */
APPSTATUS ResultsMode = MODE_FILES;

/* Search threads */
DWORD SearchThreadID[SEARCH_THREADS_MAX];
HANDLE SearchThread[SEARCH_THREADS_MAX];
u8 SearchThreadIndex[SEARCH_THREADS_MAX];
u8 SearchThreadsMax; //maximum number of threads that will be allocated

/* Functions */

static BOOL WINAPI EditCopy(void) {
    /* Clipboard function */
    u8 str[STRING_LENGTH_MAX] = { 0 };
    sprintf(str, "%08X", ReversedSeedCurrent.seed);

    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        HGLOBAL Clipboard = GlobalAlloc(GPTR, sizeof(str));
        if (Clipboard) {
            u8* pdata = (u8*)GlobalLock(Clipboard);
            if (pdata) {
                memcpy(pdata, str, sizeof(str));
                GlobalUnlock(Clipboard);
                SetClipboardData(CF_TEXT, Clipboard);
            }
        }
        CloseClipboard();
    }
    return TRUE;
}

static u8 GetLanguageActualIndex(u8 idx) {
    /* Convert index from LanguagesActual to Languages */
    if (idx < LANGUAGE__6) { return idx - 1; }
    return idx - 2;
}

static u8 GetLanguageFullIndex(u8 idx) {
    /* Convert index from Languages to LanguagesActual */
    if (idx < LANGUAGE_GE) { return idx + 1; }
    return idx + 2;
}

static APPSTATUS SetProfileSlotState(u8 slot) {
    /* Set active state and redraw each button */
    FILE* fp = fopen(ProfilesPath, "rb");

    if (fp == NULL) { return APP_ERR_OPEN_FILE; }

    for (u8 i = 0; i < PROFILE_SLOTS_MAX; i++) {
        PROFILE p;
        GetProfileFromResultsFile(&p, fp);
        ProfileSlotState[i] = IsEmptyProfile(&p) ? PSS_EMPTY : PSS_TAKEN;
        if (i == slot) { ProfileSlotState[i] ^= PSS_ACTIVE; } //only 1 can be active
        InvalidateRect(HWND_ProfileSlotButton[i], NULL, 0); //redraw button
    }

    fclose(fp);
    return APP_RESUME;
}

static APPSTATUS IsValidProfile(PROFILE* p) {
    /* Check each member of the PROFILE struct */
    /* TID, SID, Seed and Frames aren't checked because they can't technically be invalid */
    if (p->filter_item >= ITEMS_MAX) { return PROFILE_BAD_FILTER_ITEM; }
    if (p->filter_move >= MOVES_MAX) { return PROFILE_BAD_FILTER_MOVE; }
    if (p->filter_species >= SPECIES_MAX) { return PROFILE_BAD_FILTER_SPECIES; }
    if (p->language >= LANGUAGES_MAX) { return PROFILE_BAD_LANGUAGE; }
    if (p->version >= VERSIONS_MAX) { return PROFILE_BAD_VERSION; }
    if (p->wild >= OG_WILDS_MAX) { return PROFILE_BAD_WILD; }
    if (OGW_LangVers[p->language][p->version][p->wild] == NULL) { return PROFILE_BAD_WILD; }
    if (p->language == LANGUAGE_JP && p->aslr > ASLR_VALUE_MAX) { return PROFILE_BAD_ASLR; }
    else if (p->language == LANGUAGE_KO && p->aslr > ASLR_VALUE_MAX_KO) { return PROFILE_BAD_ASLR; }
    else if (p->aslr > ASLR_VALUE_MAX_EN) { return PROFILE_BAD_ASLR; }
    if (Aslrs[p->language][p->version >> 1][p->aslr] == 0) { return PROFILE_BAD_ASLR; }
    return PROFILE_OK;
}

static APPSTATUS GetProfileFromWindows(void) {
    /* Get the search parameters from the input fields and set them in PROFILE_Current */
    /* Return an error code to identify which filed is wrong */
    /* Note: the filters are automatically updated with their windows */
    PROFILE p = { 0 }; //temp
    memcpy(&p, &PROFILE_Current, sizeof(PROFILE)); //copies filters

    u8 str_tid[U16_DIGITS_DEC_MAX + 1] = { 0 };
    u8 str_sid[U16_DIGITS_DEC_MAX + 1] = { 0 };
    u8 str_seed[U32_DIGITS_HEX_MAX + 1] = { 0 };
    u8 str_frames[U32_DIGITS_DEC_MAX + 1] = { 0 };
    u8 str_aslr[ASLR_DIGITS_DEC_MAX + 1] = { 0 };

    p.version = (u8)SendMessageA(HWND_VersionInput, CB_GETCURSEL, 0, 0);
    p.language = (u8)SendMessageA(HWND_LanguageInput, CB_GETCURSEL, 0, 0);
    p.language = GetLanguageFullIndex(p.language);
    p.wild = (u8)SendMessageA(HWND_WildInput, CB_GETCURSEL, 0, 0);
    GetWindowTextA(HWND_AslrInput, str_aslr, ASLR_DIGITS_DEC_MAX + 1);
    p.aslr = AsciiToInt_dec16(str_aslr, ASLR_DIGITS_DEC_MAX);
    GetWindowTextA(HWND_TidInput, str_tid, U16_DIGITS_DEC_MAX + 1);
    GetWindowTextA(HWND_SidInput, str_sid, U16_DIGITS_DEC_MAX + 1);
    GetWindowTextA(HWND_SeedInput, str_seed, U32_DIGITS_HEX_MAX + 1);
    GetWindowTextA(HWND_FramesInput, str_frames, U32_DIGITS_DEC_MAX + 1);
    p.tid = AsciiToInt_dec16(str_tid, U16_DIGITS_DEC_MAX);
    p.sid = AsciiToInt_dec16(str_sid, U16_DIGITS_DEC_MAX);
    p.seed = AsciiToInt_hex32(str_seed);
    p.frames = (u32)AsciiToInt_dec32(str_frames);

    APPSTATUS err = IsValidProfile(&p);
    if (err != PROFILE_OK) { return err; }

    /* All good, copy temp profile into current profile */
    memcpy(&PROFILE_Current, &p, sizeof(PROFILE));
    return PROFILE_OK;
}

static void GetProfileRecapString(PROFILE* p, u8 str[256]) {
    /* Format a string with all the profile info */
    sprintf(str, "Version: %s\nLanguage: %s\nTID: %05d\nSID: %05d\nWild: %s\nSeed: 0x%08X\nFrames: %u\nASLR: %u (0x%08X)\n\nFilters:\nSpecies: %s\nMove: %s\nItem: %s",
        Versions[p->version],
        Languages[p->language],
        p->tid,
        p->sid,
        OgWilds[p->version][p->wild], //grouped version
        p->seed,
        p->frames,
        p->aslr,
        Aslrs[p->language][p->version >> 1][p->aslr], //grouped version
        Pokelist[p->filter_species],
        Moves[p->filter_move],
        Items[p->filter_item]
    );
}

static void SetWindowsFromProfile(PROFILE* p) {
    /* Set the text in the input windows from a given PROFILE */
    /* Sanitized with IsValidProfile */
    SetWindowTextFromInt(HWND_TidInput, "%05u", p->tid);
    SetWindowTextFromInt(HWND_SidInput, "%05u", p->sid);
    SetWindowTextFromInt(HWND_SeedInput, "%08X", p->seed);
    SetWindowTextFromInt(HWND_FramesInput, "%010u", p->frames);
    SetWindowTextFromInt(HWND_AslrInput, "%02u", p->aslr);
    SendMessageA(HWND_VersionInput, CB_SETCURSEL, p->version, 0);
    SendMessageA(HWND_LanguageInput, CB_SETCURSEL, GetLanguageActualIndex(p->language), 0);
    SendMessageA(HWND_WildInput, CB_SETCURSEL, p->wild, 0);
    SetWindowTextA(HWND_ItemFilterInput, Items[p->filter_item]);
    SetWindowTextA(HWND_MoveFilterInput, Moves[p->filter_move]);
    SetWindowTextA(HWND_SpeciesFilterInput, Pokelist[p->filter_species]);
}

static u8 GetProfileSlot(void) {
    /* Find and return the first active profile slot (there should only be 1) */
    for (u8 i = 0; i < PROFILE_SLOTS_MAX; i++) {
        if ((ProfileSlotState[i] & 0xf0) == PSS_ACTIVE) {
            return i;
        }
    }
    return 255; //couldn't find active slot
}

static APPSTATUS LoadProfileFromSlot(PROFILE* p, u8 slot) {
    /* Alternative to LoadProfileFromFile */
    FILE* fp = fopen(ProfilesPath, "rb");
    if (fp == NULL) { return APP_ERR_OPEN_FILE; } //couldn't find the file
    if (slot >= PROFILE_SLOTS_MAX) { return APP_ERR_PROFILE_SLOT; } //oob
    /* All good, retrieve the profile from slot offset */
    fseek(fp, slot * sizeof(PROFILE), SEEK_SET);
    fread(p, sizeof(PROFILE), 1, fp);
    fclose(fp);

    if (IsEmptyProfile(p)) { return APP_ERR_PROFILE_EMPTY; } //not really an error, just a warning
    APPSTATUS err = IsValidProfile(p);
    if (err != PROFILE_OK) { return err; }

    return APP_RESUME;
}

static APPSTATUS ConfirmLoadProfile(void) {
    /* Profile slot selection and loading procedure */
    u8 slot = GetProfileSlot();
    if (slot == 255) { return APP_ERR_PROFILE_SLOT; }

    APPSTATUS err = LoadProfileFromSlot(&PROFILE_Current, slot);

    if (err != APP_RESUME) {
        if (err == APP_ERR_PROFILE_EMPTY) {
            MessageBoxA(HWND_AppMain, "Profile loaded was empty.", "Notice", MB_OK | MB_ICONINFORMATION);
        }
        else if (err > PROFILE_OK) {
            ErrorMessageBox_BadProfile(err);
        }
        else {
            MessageBoxA(HWND_AppMain, "No profile loaded.\nYou may want to create and save a new profile.", "Notice", MB_OK | MB_ICONINFORMATION);
        }
        return APP_RESUME;
    }

    SetWindowsFromProfile(&PROFILE_Current); //success
    return APP_RESUME;
}

static APPSTATUS ConfirmSaveProfile(void) {
    /* Basic yes/no/cancel message box */
    APPSTATUS err = GetProfileFromWindows();
    if (err != PROFILE_OK) {
        ErrorMessageBox_BadProfile(err);
        return APP_RESUME;
    }

    u8 slot = GetProfileSlot();
    if (slot == 255) { return APP_ERR_PROFILE_SLOT; }

    u8 str_recap[256];
    GetProfileRecapString(&PROFILE_Current, str_recap);
    strcat(str_recap, "\n\nSave a new profile with these search parameters?");
    int answer = MessageBoxA(HWND_AppMain, str_recap, "ConfirmSaveProfile", MB_YESNOCANCEL | MB_ICONQUESTION);
    if (answer == IDYES) {
        APPSTATUS empty = LoadProfileFromSlot(&PROFILE_Load, slot);
        if (empty == APP_ERR_PROFILE_EMPTY) {
            SaveProfileToSlot(slot);
            SetProfileSlotState(slot);
        }
        else {
            int ans = MessageBoxA(HWND_AppMain, "This profile slot is already taken, do you want to overwrite it?", "Profile slot taken", MB_YESNO | MB_ICONQUESTION);
            if (ans == IDYES) {
                SaveProfileToSlot(slot);
                SetProfileSlotState(slot);
            }
        }
    }
    return APP_RESUME;
}

static void ResetSearchParameters(void) {
    /* Reset text in input windows and zero out current search parameters */
    SetWindowTextA(HWND_TidInput, "TID");
    SetWindowTextA(HWND_SidInput, "SID");
    SetWindowTextA(HWND_AslrInput, "ASLR");
    SetWindowTextA(HWND_FramesInput, "FRAMES");
    SetWindowTextA(HWND_SeedInput, "SEED");
    SendMessageA(HWND_VersionInput, CB_SETCURSEL, -1, 0);
    SendMessageA(HWND_LanguageInput, CB_SETCURSEL, -1, 0);
    SendMessageA(HWND_WildInput, CB_SETCURSEL, -1, 0);
    SetWindowTextA(HWND_SpeciesFilterInput, "SPECIES");
    SetWindowTextA(HWND_MoveFilterInput, "MOVE");
    SetWindowTextA(HWND_ItemFilterInput, "ITEM");
    memset(&PROFILE_Current, 0, sizeof(PROFILE_Current));
}

static APPSTATUS ConfirmResetSearchParameters(void) {
    /* Create YESNOCANCEL Message box and process user answer */
    int answer = MessageBoxA(HWND_AppMain, "Reset every search parameters?", "Notice", MB_YESNOCANCEL | MB_ICONWARNING);
    if (answer == IDYES) { ResetSearchParameters(); }
    return APP_RESUME;
}

static APPSTATUS LoadResultsFileDetails(u8 path[PATH_REL_LENGTH_MAX]) {
    /* Displays number of results in a file + profile info */
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) { return APP_ERR_OPEN_FILE; }

    int results = GetResultsCount(fp, TRUE);
    if (results == -1) { //file empty
        SetWindowTextA(HWND_DetailsList, "This results file is empty.");
        return APP_RESUME;
    }

    PROFILE p = { 0 };
    GetProfileFromResultsFile(&p, fp);
    fclose(fp);

    u8 str_details[512] = { 0 };
    sprintf(str_details, "%u results in this file.\nDouble-click to load.\n\nVersion         %s\nLanguage        %s\nTID             %05u\nSID             %05u\nWild            %s\nASLR            %02u (0x%08X)\nSeed            0x%08X\nFrames          %u\n\nFilters\n  Species       %s\n  Item          %s\n  Move          %s\n",
        results,
        Versions[p.version],
        Languages[p.language],
        p.tid,
        p.sid,
        OgWilds[p.version][p.wild], //grouped version
        p.aslr, Aslrs[p.language][p.version >> 1][p.aslr], //grouped version
        p.seed,
        p.frames,
        Pokelist[p.filter_species],
        Moves[p.filter_move],
        Items[p.filter_item]
    );
    SetWindowTextA(HWND_DetailsList, str_details);
    return APP_RESUME;
}

static APPSTATUS LoadResultDetails(int idx) {
    /* Read a result from the currently loaded results file and display its details in the details window */

    FILE* fp = fopen(SearchDataCurrent.path, "rb");
    if (fp == NULL) { return APP_ERR_OPEN_FILE; } //couldn't open file

    fseek(fp, (idx + 1) * sizeof(RESULTDATA), SEEK_SET); //go to index of selected result from in the file //+1 because of PROFILE header
    fread(&ResultDataCurrent, sizeof(RESULTDATA), 1, fp); //read data and write to ResultDataCurrent

    //TODO: species sprite icon? bonus

    u8 str_details[512] = { 0 }; //complete string with all the details. TODO: calc max size
    u8 str_species[STRING_LENGTH_MAX] = { 0 };
    u8 str_item[STRING_LENGTH_MAX] = { 0 };
    u8 str_moves[OWN_MOVES_MAX][STRING_LENGTH_MAX] = { 0 };
    u8 str_ability[STRING_LENGTH_MAX] = { 0 };
    u8 ivs[STATS_MAX] = { 0 };

    DecomposeIVs(ResultDataCurrent.ivs, ivs);
    HIDDENPOWER hiddenpower = GetHiddenPower(ivs);

    u8* str_fate = IsFatefulEncounter(ResultDataCurrent.fate) ? "Yes" : "No";
    u8* str_egg = IsEgg(ResultDataCurrent.ivs) ? "Yes" : "No";
    u8* str_shiny = IsShiny(ResultDataCurrent.pid, PROFILE_Load.tid, PROFILE_Load.sid) ? "Yes" : "No";
    u8* str_pkrs = HasPokerus(ResultDataCurrent.pokerus) ? "Yes" : "No";
    u8* str_catch = ResultDataCurrent.catch ? "Capture guaranteed" : "Catch with a Master Ball";

    SetString(str_species, ResultDataCurrent.species, Pokelist, SPECIES_MAX, "DPBox", "0x%04X");
    SetString(str_item, ResultDataCurrent.item, Items, ITEMS_MAX, "None", "0x%04X");
    SetString(str_ability, ResultDataCurrent.ability, Abilities, ABILITIES_MAX, "None", "0x%02X");
    for (u8 i = 0; i < OWN_MOVES_MAX; i++) {
        SetString(str_moves[i], ResultDataCurrent.moves[i], Moves, MOVES_MAX, "None", "0x%04X");
    }

    ReversedSeedCurrent = ReverseSeed(ResultDataCurrent.seed);

    sprintf(str_details, "Details of results #%u:\n\nSpecies: %s\nLevel: %u\nItem: %s\nAbility: %s\nMoves: %s / %s / %s / %s\nPID: 0x%08X\nNature: %s\nIVs: %02u / %02u / %02u / %02u / %02u / %02u\nHidden Power: %s %u\nShiny: %s\nFateful: %s\nEgg: %s\nPokerus: %s\nFriendship/Egg Cycles: %u\nForm ID: %u\n\nHit seed 0x%08X, advance by %u frame(s).\n%s.",
        idx,
        str_species,
        ResultDataCurrent.level,
        str_item,
        str_ability,
        str_moves[0], str_moves[1], str_moves[2], str_moves[3],
        ResultDataCurrent.pid,
        Natures[GetNatureId(ResultDataCurrent.pid)],
        ivs[hp], ivs[at], ivs[df], ivs[sa], ivs[sd], ivs[sp],
        Types[hiddenpower.type], hiddenpower.power,
        str_shiny,
        str_fate,
        str_egg,
        str_pkrs,
        ResultDataCurrent.friendship,
        GetFormId(ResultDataCurrent.fate),
        ReversedSeedCurrent.seed, ReversedSeedCurrent.frames,
        str_catch
    );

    SetWindowTextA(HWND_DetailsList, str_details);
    return APP_RESUME;
}

//static BOOL GetRelativeResultsPath(u8 abs[PATH_ABS_LENGTH_MAX]) {
//    /* Convert from absolute to relative RESULTS file path */
//    const u8 dir[] = "/.results/"; //directory string, guarantees correct location
//    u8 i = 0;
//    u8 pos = 255; //default
//    do { //seek dir string
//        u8 sum = 0;
//        for (u8 p = 0; p < sizeof(dir) - 1; p++) {
//            sum += (dir[p] ^ abs[i + p]);
//        }
//        if (sum == 0) { //string match
//            pos = i;
//            break;
//        }
//        i++;
//    } while (abs[i]); //reached end of abs
//    if (pos == 255) { return FALSE; } //dir string not found in abs string
//    u8 tmp[PATH_ABS_LENGTH_MAX] = { 0 };
//    memcpy(tmp, &abs[pos + 1], PATH_ABS_LENGTH_MAX - pos);
//    memcpy(abs, tmp, PATH_ABS_LENGTH_MAX);
//    return TRUE; //sucess
//}

static void AddResultToList(RESULTDATA* result, int index) {
    /* Format a result string and add it to the results list */
    u8 str_summary[2 * STRING_LENGTH_MAX] = { 0 };
    u8* str_egg = IsEgg(result->ivs) ? "E" : "-"; //E=Egg
    u8* str_shiny = IsShiny(result->pid, PROFILE_Load.tid, PROFILE_Load.sid) ? "S" : "-"; //S=Shiny
    u8* str_fate = IsFatefulEncounter(result->fate) ? "F" : "-"; //F=Fateful
    sprintf(str_summary, "%08X %-12s %s %s %s", result->seed, Pokelist[result->species], str_egg, str_fate, str_shiny);
    memcpy(ResultsListStrings[index], str_summary, sizeof(str_summary)); //sends string to list
    SendMessageA(HWND_ResultsList, LB_ADDSTRING, 0, (LPARAM)""); //notifies window that a string was added
}

static void ClearResults(void) {
    /* Empty results list and details window */
    memset(&ResultsListStrings, 0, sizeof(ResultsListStrings));
    SendMessageA(HWND_ResultsList, LB_RESETCONTENT, 0, 0);
    SetWindowTextA(HWND_DetailsList, "");
}

static APPSTATUS LoadResultsFromFile(u8* filepath) {
    /* Read data from a results file and add result strings to the list */
    ClearResults();

    FILE* fp = fopen(filepath, "rb");
    if (fp == NULL) { return APP_ERR_OPEN_FILE; } //abort, couldn't open file

    SearchDataCurrent.results = GetResultsCount(fp, TRUE);
    if (SearchDataCurrent.results == -1) { return APP_ERR_NORESULTS; } //no results

    //GetRelativeResultsPath(filepath);
    memcpy(SearchDataCurrent.path, filepath, sizeof(SearchDataCurrent.path));

    GetProfileFromResultsFile(&PROFILE_Load, fp);

    /* Load result data into HWND_ResultsList */
    for (u32 i = 0; i < SearchDataCurrent.results; i++) {
        RESULTDATA rd = { 0 };
        fread(&rd, sizeof(RESULTDATA), 1, fp);
        AddResultToList(&rd, i);
    }

    fclose(fp);
    return APP_RESUME; //success
}

static DWORD WINAPI MotorSearchLoopThreadProc(LPVOID param) {
    /* Multi-threaded search */

    while (1) {

        Sleep(1); //reduces CPU load

        u8 i = *(u8*)param; //retrieves search thread index

        if (AuthorizeSearchThreads[i]) {

            u8 tmp_filename[PATH_REL_LENGTH_MAX] = { 0 };
            sprintf(tmp_filename, ".results/RESULTS_tmp_%u", i);
            FILE* ftmp = fopen(tmp_filename, "wb+");

            if (ftmp == NULL) { return APP_ERR_OPEN_FILE; }

            u32 seed = SearchDataCurrent.seed[i];

            for (u32 frame = 0; frame < SearchDataCurrent.frames[i]; frame++) {

                while (PauseSearch) { Sleep(1); } //wait for ConfirmAbortSearch
                if (AbortSearch) { break; }

                if (frame != 0) { RngNext(&seed); } //advance the RNG everytime, except on the 0th frame

                PKMN wild = { 0 }; //0 init
                PKMN seven = { 0 }; //0 init

                MethodJSeedToPID(seed, &wild);
                SetBlocks(&wild);

                /* Block A */
                wild.data[wild.pos_a][0] = SearchDataCurrent.pOgWild->species; //species
                wild.data[wild.pos_a][1] = SearchDataCurrent.pOgWild->item; //held item
                wild.data[wild.pos_a][2] = PROFILE_Current.tid; //tid
                wild.data[wild.pos_a][3] = PROFILE_Current.sid; //sid
                wild.data[wild.pos_a][4] = SearchDataCurrent.pOgWild->xp1; //xp1
                wild.data[wild.pos_a][5] = SearchDataCurrent.pOgWild->xp2; //xp2
                wild.data[wild.pos_a][6] = SearchDataCurrent.pOgWild->frab; //ability and friendship concatenated
                wild.data[wild.pos_a][7] = SearchDataCurrent.w_language; //language
                /* Block B */
                for (u8 i = 0; i < OWN_MOVES_MAX; i++) { wild.data[wild.pos_b][i] = SearchDataCurrent.pOgWild->moves[i]; }//4 moves
                wild.data[wild.pos_b][4] = SearchDataCurrent.pOgWild->pp1and2; //pp1and2
                wild.data[wild.pos_b][5] = SearchDataCurrent.pOgWild->pp3and4; //pp3and4
                wild.data[wild.pos_b][8] = wild.iv1;
                wild.data[wild.pos_b][9] = wild.iv2;
                wild.data[wild.pos_b][12] = GetGender(wild.pid, SearchDataCurrent.pOgWild->species) | SearchDataCurrent.alt_form; //gender | alt_form
                /* Block C */
                for (u8 i = 0; i < 11; i++) { wild.data[wild.pos_c][i] = SearchDataCurrent.pOgWild->name[i]; } //11 characters for the name
                wild.data[wild.pos_c][11] = SearchDataCurrent.w_version; //version
                /* Block D */
                wild.data[wild.pos_d][13] = 0x0400; //pokeball
                wild.data[wild.pos_d][14] = SearchDataCurrent.pOgWild->level; //level
                /* Condition data */
                wild.cond[2] = SearchDataCurrent.pOgWild->level; //level again
                wild.cond[3] = IvToStat_HP(&wild, SearchDataCurrent.pOgWild);
                wild.cond[4] = wild.cond[3]; //current hp = max hp
                wild.cond[5] = IvToStat(&wild, SearchDataCurrent.pOgWild, at);
                wild.cond[6] = IvToStat(&wild, SearchDataCurrent.pOgWild, df);
                wild.cond[7] = IvToStat(&wild, SearchDataCurrent.pOgWild, sp);
                wild.cond[8] = IvToStat(&wild, SearchDataCurrent.pOgWild, sa);
                wild.cond[9] = IvToStat(&wild, SearchDataCurrent.pOgWild, sd);
                // wild.cond[10] = 0;
                // wild.cond[11] = 0;
                wild.cond[12] = SearchDataCurrent.w_language; //language again
                wild.cond[13] = 0xff00 | (SearchDataCurrent.w_version >> 8); //version variation
                for (u8 i = 14; i < 25; i++) { wild.cond[i] = 0xffff; } //14 to 24 = 0xffff
                // wild.cond[25] = 0;
                wild.cond[26] = 0xffff;
                // wild.cond[27] = 0;
                wild.cond[28] = 0xffff;
                wild.cond[29] = 0xffff;

                SetCheckum(&wild);
                EncryptBlocks(&wild);
                EncryptCondition(&wild);

                /* Initialize Seven */
                seven.pid = 0x00005544;
                seven.bef = 0x05a4; //after checksum check, changed to a Bad Egg (bit 4: 0->1)
                SetBlocks(&seven); //always ACBD (0x0213)

                /* Simulate the buffer overflow */
                /* Block A */
                seven.data[seven.pos_a][0] = (SearchDataCurrent.aslr + OppPartyOffBeg[SearchDataCurrent.grouped_version]) & 0xffff;
                seven.data[seven.pos_a][1] = (SearchDataCurrent.aslr + OppPartyOffBeg[SearchDataCurrent.grouped_version]) >> 16;
                seven.data[seven.pos_a][2] = (SearchDataCurrent.aslr + OppPartyOffEnd[SearchDataCurrent.grouped_version]) & 0xffff;
                seven.data[seven.pos_a][3] = (SearchDataCurrent.aslr + OppPartyOffEnd[SearchDataCurrent.grouped_version]) >> 16;
                for (u8 i = 0; i < 8; i++) { seven.data[seven.pos_a][i + 4] = SearchDataCurrent.pOgWild->gfx[i]; }
                seven.data[seven.pos_a][12] = 0x0006;
                seven.data[seven.pos_a][13] = 0x0000;
                seven.data[seven.pos_a][14] = 0x0001;
                seven.data[seven.pos_a][15] = 0x0000;
                /* Block C, B, D and Condition data - array out of bounds method */
                u16* wild_data = (u16*)(&wild.pid);
                u16* seven_data = (u16*)(&seven.data[1]);
                for (u8 i = 0; i < (BLOCKS - 1) * BLOCK_SIZE + COND_SIZE_S + STACK_OFFSET; i++) { seven_data[i] = wild_data[i]; }

                EncryptBlocks(&seven);

                /* If the 1st move of Seven is invalid, the game will crash right before showing the battle menu */
                if (seven.data[seven.pos_b][0] > MOVES_MAX + 2) { continue; }

                /* If the last 3 moves of Seven are invalid, the game will crash right before showing the battle menu (ASLR variation)*/
                //if (seven.data[seven.pos_b][1] > MOVES_MAX + 2 ||
                //	seven.data[seven.pos_b][2] > MOVES_MAX + 2 ||
                //	seven.data[seven.pos_b][3] > MOVES_MAX + 2) {
                //	continue;
                //}

                /* If the ball doesn't have a valid ID the battle won't load */
                u8 ballid = seven.data[seven.pos_d][13] >> 8;
                if (ballid > BALL_ID_MAX) { continue; }

                SetCheckum(&seven);
                EncryptBlocks(&seven);

                /* If heap ID of Opponent 1 Party is valid, the game will crash when returning to the overworld */
                u8 heapid = seven.data[seven.pos_a][10] & 0xff;
                if (heapid < HEAPID_MAX) { continue; }

                /* If partycount of Opponent 1 Party is invalid, the game will crash right before showing the battle menu */
                s32 partycount = seven.data[seven.pos_a][14] | (seven.data[seven.pos_a][15] << 16);
                if (IsInvalidPartyCount(partycount)) { continue; }

                /* If the Bad Egg flag is set or the Fast Mode flags aren't set, the PKMN will become a Bad Egg */
                if ((seven.data[seven.pos_c][2] & 7) != 3) { continue; }

                /* Get the new PID of the wild and deduce its new block order */
                wild.pid = seven.data[seven.pos_c][0] | (seven.data[seven.pos_c][1] << 16);
                SetBlocks(&wild);

                /* Get final species, item, ability and friendship to hatch - array out of bounds method */
                RESULTDATA rd = { 0 };

                rd.pid = wild.pid;
                rd.seed = seed;

                rd.species = seven.data[1 + wild.pos_a][STACK_OFFSET];
                rd.item = seven.data[1 + wild.pos_a][STACK_OFFSET + 1];
                rd.friendship = seven.data[1 + wild.pos_a][STACK_OFFSET + 6] & 0xff;
                rd.ability = seven.data[1 + wild.pos_a][STACK_OFFSET + 6] >> 8;

                /* Valid species, user-defined species and user-defined item filters */
                if (rd.species >= SPECIES_MAX) { continue; }
                if (PROFILE_Current.filter_species != 0 && rd.species != PROFILE_Current.filter_species) { continue; }
                if (PROFILE_Current.filter_item != 0 && rd.item != PROFILE_Current.filter_item) { continue; }

                /* Get final moveset, IVs, Egg friendship, Form ID and Fateful Encounter flag - array out of bounds method */
                for (u8 i = 0; i < OWN_MOVES_MAX; i++) { rd.moves[i] = seven.data[1 + wild.pos_b][STACK_OFFSET + i]; }
                rd.ivs = (seven.data[1 + wild.pos_b][STACK_OFFSET + 8]) | (seven.data[1 + wild.pos_b][STACK_OFFSET + 9] << 16);//?
                rd.fate = seven.data[2 + wild.pos_b][0]; //form id and fateful encouter flag will be derived from it

                /* Move filter */
                if (PROFILE_Current.filter_move != 0) {
                    /* If none of the 4 current ones match the user's move, continue search */
                    if ((rd.moves[0] != PROFILE_Current.filter_move) &&
                        (rd.moves[1] != PROFILE_Current.filter_move) &&
                        (rd.moves[2] != PROFILE_Current.filter_move) &&
                        (rd.moves[3] != PROFILE_Current.filter_move))
                    {
                        continue;
                    }
                }

                rd.level = seven.cond[BLOCK_SIZE + STACK_OFFSET + 2] & 0xff;
                rd.pokerus = seven.data[2 + wild.pos_d][1] & 0xff; //[13] of block D -> [1] of 2nd next block with offset

                /* HP and HP max determine catch rate */
                u32 f_hp = seven.cond[BLOCK_SIZE + STACK_OFFSET + 3];
                u32 f_hp_max = seven.cond[BLOCK_SIZE + STACK_OFFSET + 4];
                rd.catch = (3 * f_hp_max / 2 < f_hp) ? 1 : 0;

                fwrite(&rd, sizeof(rd), 1, ftmp); //Write to temp results file
                AddResultToList(&rd, SearchDataCurrent.results); //add to results list window

                SearchDataCurrent.results++;
                SearchDataCurrent.progress[i] = frame;
            }

            AuthorizeSearchThreads[i] = FALSE;
            fclose(ftmp);
        }
    }
}

static void SearchThreadsSplit(void) {
    /* Split frames and seeds evenly between the search threads */
    /* Frames */
    u32 frames_sum = 0;
    u32 frames_split = PROFILE_Current.frames / SearchThreadsMax;
    for (u8 i = 0; i < SearchThreadsMax; i++) {
        SearchDataCurrent.frames[i] = frames_split;
        frames_sum += SearchDataCurrent.frames[i];
    }
    while (frames_sum != PROFILE_Current.frames) {
        SearchDataCurrent.frames[0]++; //making sure all frames are accounted for
        frames_sum++;
    }
    /* Seeds */
    u32 seed = PROFILE_Current.seed;
    for (u8 i = 0; i < SearchThreadsMax; i++) {
        SearchDataCurrent.seed[i] = seed;
        if (i + 1 == SearchThreadsMax) { break; } //don't run the next loop on last thread
        for (u32 frames = 0; frames < SearchDataCurrent.frames[i]; frames++) {
            RngNext(&seed);
        }
    }
}

static APPSTATUS GenerateResultsTextFile(void) {
    /* Create a .txt version of a results file */
    FILE* results_file = fopen(SearchDataCurrent.path, "rb");
    if (results_file == NULL) { return APP_ERR_OPEN_FILE; }

    /* Replace .mtr by .txt */
    u8 filename_txt[PATH_REL_LENGTH_MAX] = { 0 };
    memcpy(filename_txt, SearchDataCurrent.path, sizeof(SearchDataCurrent.path));
    for (u8 i = sizeof(filename_txt) - 1; i > 0; i--) {
        if (filename_txt[i] == '.') {
            memcpy(&filename_txt[i], ".txt", sizeof(".txt"));
            break;
        }
    }

    FILE* fp = fopen(filename_txt, "w+");
    if (fp == NULL) { return APP_ERR_OPEN_FILE; }

    PROFILE p = { 0 };
    GetProfileFromResultsFile(&p, results_file);

    APPSTATUS err_profile = IsValidProfile(&p);
    if (err_profile != PROFILE_OK) { return err_profile; }

    //TODO: too many calls to fprintf?
    /* Header */
    fprintf(fp, "Motor v%s\nThis results file was generated automatically.\n\n[PROFILE]\n\n", MOTOR_VERSION);
    /* Profile */
    fprintf(fp, "Version         %s\n", Versions[p.version]);
    fprintf(fp, "Language        %s\n", Languages[p.language]);
    fprintf(fp, "TID             %05u\n", p.tid);
    fprintf(fp, "SID             %05u\n", p.sid);
    fprintf(fp, "Wild            %s\n", OgWilds[p.version][p.wild]);
    fprintf(fp, "ASLR            %02u (0x%08X)\n", p.aslr, Aslrs[p.language][p.version >> 1][p.aslr]);
    fprintf(fp, "Seed            0x%08X\n", p.seed);
    fprintf(fp, "Frames          %u\n\n", p.frames);
    fprintf(fp, "Filters\n");
    fprintf(fp, "  Species       %s\n", Pokelist[p.filter_species]);
    fprintf(fp, "  Item          %s\n", Items[p.filter_item]);
    fprintf(fp, "  Move          %s\n", Moves[p.filter_move]);

    fprintf(fp, "\n[RESULTS]\n\n");

    fprintf(fp, "Seed       | Reverse    | Advances | PID        | Level   | Species      | Form | Item           | Ability          | Hatch steps | Fateful | Pokerus | Shiny | Ball   | IVs               | Moves\n");

    for (u32 i = 0; i < (196 + 24); i++) { fprintf(fp, "-"); } //line separator
    fprintf(fp, "\n");

    /* Results */
    for (u32 i = 0; i < SearchDataCurrent.results; i++) {

        RESULTDATA rd = { 0 };
        fread(&rd, sizeof(RESULTDATA), 1, results_file);

        u8 str_species[STRING_LENGTH_MAX] = { 0 };
        u8 str_item[STRING_LENGTH_MAX] = { 0 };
        u8 str_moves[OWN_MOVES_MAX][STRING_LENGTH_MAX] = { 0 };
        u8 str_ability[STRING_LENGTH_MAX] = { 0 };
        u8 ivs[STATS_MAX] = { 0 };

        DecomposeIVs(rd.ivs, ivs);

        u8* str_fate = IsFatefulEncounter(rd.fate) ? "Fateful" : "-------";
        u8* str_shiny = IsShiny(rd.pid, p.tid, p.sid) ? "Shiny" : "-----";
        u8* str_pkrs = HasPokerus(rd.pokerus) ? "Pokerus" : "-------";
        u8* str_catch = rd.catch ? "Any   " : "Master";

        u16 steps = IsEgg(rd.ivs) * (rd.friendship + 1) * 255;

        SetString(str_species, rd.species, Pokelist, SPECIES_MAX, "DPBox", "0x%04X");
        SetString(str_item, rd.item, Items, ITEMS_MAX, "None", "0x%04X");
        SetString(str_ability, rd.ability, Abilities, ABILITIES_MAX, "None", "0x%02X");
        for (u8 i = 0; i < OWN_MOVES_MAX; i++) {
            SetString(str_moves[i], rd.moves[i], Moves, MOVES_MAX, "None", "0x%04X");
        }

        REVERSEDSEED rs = ReverseSeed(rd.seed);

        fprintf(fp, "0x%08X | 0x%08X | %-8u | 0x%08X | Lv. %-3u | %-12s | %-4u | %-14s | %-16s | %-5u steps | %s | %s | %s | %s | ",
            rd.seed, rs.seed, rs.frames, rd.pid, rd.level, str_species, GetFormId(rd.fate), str_item, str_ability, steps, str_fate, str_pkrs, str_shiny, str_catch);
        fprintf(fp, "%02u/%02u/%02u/%02u/%02u/%02u | ", ivs[hp], ivs[at], ivs[df], ivs[sa], ivs[sd], ivs[sp]);
        fprintf(fp, "%s, %s, %s, %s\n", str_moves[0], str_moves[1], str_moves[2], str_moves[3]);
    }

    fclose(fp);
    fclose(results_file);
    return APP_RESUME;
}

static APPSTATUS CompileResultsToFile(void) {
    /* Copy the contents of the temporary results file into a single one, delete the tmp files  */
    FILE* results_file = fopen(SearchDataCurrent.path, "wb+"); //byte mode
    if (results_file == NULL) { return APP_ERR_OPEN_FILE; }

    fwrite(&PROFILE_Current, sizeof(PROFILE), 1, results_file); //header=profile

    for (u8 i = 0; i < SearchThreadsMax; i++) {

        u8 tmp_filename[PATH_REL_LENGTH_MAX] = { 0 };
        sprintf(tmp_filename, ".results/RESULTS_tmp_%u", i);
        FILE* tmp = fopen(tmp_filename, "rb");
        if (tmp == NULL) { return APP_ERR_OPEN_FILE; }

        int results = GetResultsCount(tmp, FALSE);
        if (results != -1) { //file isn't empty
            for (int i = 0; i < results; i++) {
                RESULTDATA rd = { 0 };
                fread(&rd, sizeof(RESULTDATA), 1, tmp);
                fwrite(&rd, sizeof(RESULTDATA), 1, results_file);
            }
        }

        fclose(tmp);
        remove(tmp_filename); //delete tmp file
    }
    fclose(results_file);

    APPSTATUS err_txt = GenerateResultsTextFile();
    if (err_txt != APP_RESUME) {
        if (err_txt == APP_ERR_OPEN_FILE) {
            //TODO: something?
        }
        else { ErrorMessageBox_BadProfile(err_txt); }
    }
    return APP_RESUME;
}

static APPSTATUS MotorSearch(void) {
    /* Initialize search parameters for the search threads */
    SearchDataCurrent.grouped_version = PROFILE_Current.version >> 1; //Group Diamond and Pearl together
    SearchDataCurrent.w_version = (PROFILE_Current.version + 10) << 8; //convert for use in pkmn data
    SearchDataCurrent.w_language = PROFILE_Current.language << 8; //convert for use in pkmn data
    SearchDataCurrent.pOgWild = OGW_LangVers[PROFILE_Current.language][PROFILE_Current.version][PROFILE_Current.wild];
    SearchDataCurrent.aslr = Aslrs[PROFILE_Current.language][SearchDataCurrent.grouped_version][PROFILE_Current.aslr]; //depends on language, version and user choice
    if (PROFILE_Current.language == LANGUAGE_KO) { SearchDataCurrent.aslr += KOREAN_OFFSET; } //RAM thing
    SearchDataCurrent.alt_form = (PROFILE_Current.version == VERSION_PLATINUM && PROFILE_Current.wild == OGW_PT_GIRATINA_O) ? 8 : 0; //Giratina Origin

    SearchThreadsSplit();

    for (u8 i = 0; i < SearchThreadsMax; i++) { AuthorizeSearchThreads[i] = TRUE; } //launches the search

    clock_t begin = clock(); //timer starts

    while (1) {

        Sleep(1); //reduces CPU load

        if (PauseSearch) {
            if (ConfirmAbortSearch() == APP_ABORT_SEARCH) {
                AbortSearch = TRUE; //tells search threads to break
            }
            PauseSearch = FALSE;
        }

        u8 finished = 0;
        u64 progress = 0;
        for (u8 i = 0; i < SearchThreadsMax; i++) {
            if (AuthorizeSearchThreads[i] == FALSE) { finished++; }
            progress += SearchDataCurrent.progress[i];
        }
        /* Update progress bar */
        progress = 100 * progress / (u64)PROFILE_Current.frames;
        SendMessageA(HWND_ProgressBar, PBM_SETPOS, (u32)progress, 0);
        if (finished == SearchThreadsMax) { break; } //break once all search threads finish
    }

    clock_t end = clock(); //timer ends
    SearchDataCurrent.time = ((double)end - (double)begin) / CLOCKS_PER_SEC; //calculate time elapsed since start of search

    CompileResultsToFile(); //TODO: use return value

    return APP_RESUME;
}

static DWORD WINAPI MotorThreadProc(LPVOID param) {
    /* Parent thread function for the search  */

    while (1) {

        Sleep(1); //reduces CPU load

        if (AuthorizeSearch) {

            ClearResults();
            ResultsMode = MODE_RESULTS;

            SendMessageA(HWND_ProgressBar, PBM_SETPOS, 0, 0); //progress bar empty
            InvalidateRect(HWND_SearchButton, NULL, 0); //tells search button to draw itself

            u8 path[PATH_REL_LENGTH_MAX] = { 0 };
            sprintf(path, ".results/RESULTS_%s_%s_%s_%u_%05u_%05u.mtr",
                Versions_short[PROFILE_Current.version],
                Languages_short[PROFILE_Current.language],
                OgWilds[PROFILE_Current.version][PROFILE_Current.wild],
                PROFILE_Current.aslr,
                PROFILE_Current.tid,
                PROFILE_Current.sid);

            SearchDataCurrent = (SEARCHDATA){ 0 }; //init
            memcpy(SearchDataCurrent.path, path, sizeof(path));

            APPSTATUS err = MotorSearch(); //launch search and get the return value (unused?)

            SendMessageA(HWND_ProgressBar, PBM_SETPOS, 100, 0); //progress bar full

            u8 str[5 * STRING_LENGTH_MAX] = { 0 };
            sprintf(str, "%u results found in %.2f seconds.\nSaved to \"%s\".", SearchDataCurrent.results, SearchDataCurrent.time, SearchDataCurrent.path);
            MessageBoxA(HWND_AppMain, str, "Search complete", MB_OK); //

            SendMessageA(HWND_ProgressBar, PBM_SETPOS, 0, 0); //progress bar empty

            if (LoadResultsFromFile(SearchDataCurrent.path) != APP_RESUME) {
                MessageBoxA(HWND_AppMain, "Couln't load results file.", "ERROR", MB_OK | MB_ICONERROR);
            }

            PauseSearch = FALSE;
            AbortSearch = FALSE;
            AuthorizeSearch = FALSE;
            InvalidateRect(HWND_SearchButton, NULL, 0); //tells search button to draw itself
        }
    }

    return 0;
}

static APPSTATUS ConfirmSearchLaunch(void) {
    /* Creates a dialog box which recaps the search parameters and asks for confirmation to launch the search */
    int err = GetProfileFromWindows();
    if (err != PROFILE_OK) {
        ErrorMessageBox_BadProfile(err);
        return APP_RESUME;
    }

    u8 str_recap[256];
    GetProfileRecapString(&PROFILE_Current, str_recap);
    strcat(str_recap, "\n\nLaunch a search with these parameters?");

    int answer = MessageBoxA(HWND_AppMain, str_recap, "ConfirmSearchLaunch", MB_YESNOCANCEL | MB_ICONQUESTION);
    if (answer == IDYES) {
        AuthorizeSearch = TRUE;
    }
    return APP_RESUME;
}

static APPSTATUS GetResultsPath(u8 index, u8 path[PATH_REL_LENGTH_MAX]) {
    /* Get the relative path of the results file pointed to by the index of the selected item */
    WIN32_FIND_DATAA wfd = { 0 };
    int i = 0;
    HANDLE file = FindFirstFileA(".results/RESULTS_*.mtr", &wfd);

    if (file == INVALID_HANDLE_VALUE) { return APP_ERR_OPEN_FILE; } //not really an error, just no file in the directory

    memcpy(path, wfd.cFileName, PATH_REL_LENGTH_MAX);

    while (FindNextFileA(file, &wfd) && i != index) {
        i++;
        memcpy(path, wfd.cFileName, PATH_REL_LENGTH_MAX);
    }

    u8 tmp[PATH_REL_LENGTH_MAX] = { 0 };
    sprintf(tmp, ".results/%s", path); //add ResultsDirectory 
    memcpy(path, tmp, sizeof(tmp));

    FindClose(file);
    return APP_RESUME;
}

static APPSTATUS ListResultsFiles(void) {
    /* Lists the files in a directory and adds them to HWND_ResultsList */
    ClearResults();

    WIN32_FIND_DATAA wfd = { 0 };
    int i = 0;
    HANDLE file = FindFirstFileA(".results/RESULTS_*.mtr", &wfd);

    if (file == INVALID_HANDLE_VALUE) { return APP_ERR_OPEN_FILE; } //not really an error, just no file in the directory

    do {
        /* Gets rid of RESULTS_ prefix and .mtr file extension */
        //TODO: fix end of string shenanigans when file name is too long
        u8 tmp[PATH_REL_LENGTH_MAX] = { 0 };
        memcpy(tmp, wfd.cFileName + sizeof("RESULTS_") - 1, sizeof(tmp));
        for (u8 i = sizeof(tmp) - 1; i > 0; i--) {
            if (tmp[i] == '.') {
                memcpy(&tmp[i], "\0\0\0", sizeof("\0\0\0")); //truncate string to get rid of .mtr
                break;
            }
        }
        memcpy(ResultsListStrings[i], tmp, sizeof(ResultsListStrings[0])); //sends string to list
        SendMessageA(HWND_ResultsList, LB_ADDSTRING, 0, (LPARAM)""); //notifies window that a string was added to ResultsListStrings
        i++;
    } while (FindNextFileA(file, &wfd));

    FindClose(file);
    return APP_RESUME;
}

/* Window procedures and entry point  */

static LRESULT WINAPI SearchParametersProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    /* Handles messages sent to HWND_SearchParameters */
    switch (uMsg)
    {

    case WM_HOTKEY:
    {
        /* Tab input window switch */
        if (GetForegroundWindow() != HWND_AppMain) { break; } //don't process if not focused
        if (wParam != VK_TAB) { break; } //useless if only VK_TAB is registered
        HWND prev = GetFocus();
        HWND next = GetNextSearchParamTabStop(prev);
        SetFocus(next);
        break;
    }

    case WM_MEASUREITEM:
    {
        /* Set the height of each item */
        LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
        lpmis->itemHeight = COMBOBOX_HEIGHT;
        return TRUE;
    }

    case WM_SETCURSOR:
    {
        if (LOWORD(lParam) == HTCLIENT) { //cursor is in the client area of a window
            HWND hwnd = (HWND)wParam;
            int menu = (int)GetMenu(hwnd);
            switch (menu)
            {
            case ID_SEARCH_INPUT:
            case ID_SPECIES_FILTER:
            case ID_ITEM_FILTER:
            case ID_MOVE_FILTER:
                SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_IBEAM));
                break;
            case ID_LANGUAGES_LIST:
            case ID_VERSIONS_LIST:
            case ID_WILDS_LIST:
            case ID_LOAD_BUTTON:
            case ID_SAVE_BUTTON:
            case ID_RESET_BUTTON:
            case ID_SEARCH_BUTTON:
                SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_HAND));
                break;
            default:
                SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
            }
        }
        return TRUE;
    }

    case WM_COMMAND:
    {
        u16 notification = HIWORD(wParam);
        switch (notification)
        {
        case EN_SETFOCUS:
        {
            //TODO: fix, works only with filters and TAB?
            //SendMessageA((HWND)lParam, EM_SETSEL, 0, -1); //select all chars in an edit control (TAB only)
            break;
        }
        case EN_KILLFOCUS:
        {
            SendMessageA((HWND)lParam, EM_SETSEL, 0, -1); //select all chars in an edit control (TAB only)
            if (AuthorizeSearch) { break; } //search is running, don't process commands below
            SetTextInput_dec16(HWND_TidInput);
            SetTextInput_dec16(HWND_SidInput);
            SetTextInput_hex32(HWND_SeedInput);
            SetTextInput_dec32(HWND_FramesInput);
            SetTextInput_aslr(HWND_AslrInput);
            SetFilterInput(HWND_SpeciesFilterInput, Pokelist, SPECIES_MAX, &PROFILE_Current.filter_species);
            SetFilterInput(HWND_ItemFilterInput, Items, ITEMS_MAX, &PROFILE_Current.filter_item);
            SetFilterInput(HWND_MoveFilterInput, Moves, MOVES_MAX, &PROFILE_Current.filter_move);
            break;
        }
        case BN_CLICKED:
        {
            switch (wParam) //control id
            {
            case ID_SEARCH_BUTTON:
                if (!AuthorizeSearch) { ConfirmSearchLaunch(); } //only allow to launch if a search is not already running
                else { PauseSearch = TRUE; }
                break;
            case ID_SAVE_BUTTON:
                if (AuthorizeSearch) { break; } //search is running, don't process commands below
                ConfirmSaveProfile();
                break;
            case ID_LOAD_BUTTON:
                if (AuthorizeSearch) { break; } //search is running, don't process commands below
                ConfirmLoadProfile();
                break;
            case ID_RESET_BUTTON:
                if (AuthorizeSearch) { break; } //search is running, don't process commands below
                ConfirmResetSearchParameters();
                break;
            case ID_PROFILE_SLOT_BUTTON + 0:
            case ID_PROFILE_SLOT_BUTTON + 1:
            case ID_PROFILE_SLOT_BUTTON + 2:
            case ID_PROFILE_SLOT_BUTTON + 3:
            case ID_PROFILE_SLOT_BUTTON + 4:
            case ID_PROFILE_SLOT_BUTTON + 5:
            case ID_PROFILE_SLOT_BUTTON + 6:
            case ID_PROFILE_SLOT_BUTTON + 7:
                if (AuthorizeSearch) { break; } //search is running, don't process commands below
                SetProfileSlotState(wParam - ID_PROFILE_SLOT_BUTTON);
                break;
            }
            break;
        }
        }
        return 0;
    }

    case WM_CTLCOLOREDIT:
    {
        /* Input windows esketit */
        HWND hwnd = (HWND)lParam;
        HDC hdc = (HDC)wParam;
        RECT rct = { 0 };
        /* Compare the addresses, known at run time */
        if (hwnd == HWND_ItemFilterInput || hwnd == HWND_SpeciesFilterInput || hwnd == HWND_MoveFilterInput) {
            SelectObject(hdc, HPEN_Pump_h);
            SetTextColor(hdc, MOTOR_COLOR_PUMP_H);
        }
        else {
            SelectObject(hdc, HPEN_Teal_h);
            SetTextColor(hdc, MOTOR_COLOR_TEAL_H);
        }

        SelectObject(hdc, HBRUSH_Dark);
        Edit_GetRect(hwnd, &rct);

        if (rct.right != TEXT_INPUT_WIDTH) { //dirty but detects comboboxes
            RoundRect(hdc, -1, -1, TEXT_INPUT_WIDTH, COMBOBOX_HEIGHT + 7, APP_BORDER_RADIUS_S, APP_BORDER_RADIUS_S);
        }
        else {
            RoundRect(hdc, -1, -1, TEXT_INPUT_WIDTH + 1, TEXT_INPUT_HEIGHT + 1, APP_BORDER_RADIUS_S, APP_BORDER_RADIUS_S); //edit
        }
        SelectObject(hdc, myFont);
        SetBkColor(hdc, MOTOR_COLOR_DARK);
        return (LRESULT)HBRUSH_Dark; //need to return handle to brush for use
    }

    case WM_PAINT:
    {
        /* HWND_SearchParameters esketit */
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(HWND_SearchParameters, &ps);
        SelectObject(hdc, HPEN_Teal);
        SelectObject(hdc, HBRUSH_Dark);
        RoundRect(hdc, 0, 0, SEARCH_PARAMS_WIDTH, SEARCH_PARAMS_HEIGHT, APP_BORDER_RADIUS_M, APP_BORDER_RADIUS_M);
        EndPaint(HWND_SearchParameters, &ps);
        return 0;
    }

    case WM_DRAWITEM:
    {

        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

        switch (wParam) {
        case ID_PROFILE_SLOT_BUTTON + 0:
        case ID_PROFILE_SLOT_BUTTON + 1:
        case ID_PROFILE_SLOT_BUTTON + 2:
        case ID_PROFILE_SLOT_BUTTON + 3:
        case ID_PROFILE_SLOT_BUTTON + 4:
        case ID_PROFILE_SLOT_BUTTON + 5:
        case ID_PROFILE_SLOT_BUTTON + 6:
        case ID_PROFILE_SLOT_BUTTON + 7:
        {
            u8 label[3] = { 0 };
            sprintf(label, "P%u", wParam - ID_PROFILE_SLOT_BUTTON + 1);
            int color = ((ProfileSlotState[wParam - ID_PROFILE_SLOT_BUTTON] & 0xf0) == PSS_ACTIVE) ? MOTOR_COLOR_PUMP : MOTOR_COLOR_TEAL;
            /* "invert" button aspect */
            if ((ProfileSlotState[wParam - ID_PROFILE_SLOT_BUTTON] & 0x0f) == PSS_EMPTY) {
                lpdis->itemState = ODS_INVERT;
            }
            return DrawButton(lpdis, color, label, sizeof(label) - 1);
        }
        case ID_LOAD_BUTTON: return DrawButton(lpdis, MOTOR_COLOR_TEAL, "LOAD", sizeof("LOAD") - 1);
        case ID_SAVE_BUTTON: return DrawButton(lpdis, MOTOR_COLOR_TEAL, "SAVE", sizeof("SAVE") - 1);
        case ID_RESET_BUTTON: return DrawButton(lpdis, MOTOR_COLOR_PUMP, "RESET", sizeof("RESET") - 1);
        case ID_SEARCH_BUTTON:
        {
            if (AuthorizeSearch) {
                return DrawButton(lpdis, MOTOR_COLOR_TEAL, "ABORT", sizeof("ABORT") - 1);
            }
            else {
                return DrawButton(lpdis, MOTOR_COLOR_PUMP, "SEARCH", sizeof("SEARCH") - 1);
            }
        }
        case ID_VERSIONS_LIST:
        {
            u8 label[STRING_LENGTH_MAX] = "VERSION        ";
            u8 old_vers = PROFILE_Current.version;
            if (lpdis->itemID >= VERSIONS_MAX) {
                PROFILE_Current.version = 0; //will update the wild's list and prevents garbage data
            }
            else {
                PROFILE_Current.version = lpdis->itemID; //will update the wild's list normally
                memcpy(label, Versions[PROFILE_Current.version], STRING_LENGTH_MAX);
            }
            if (old_vers != PROFILE_Current.version) {
                SendMessageA(HWND_WildInput, CB_SETCURSEL, -1, 0); //solves edge case when selecting a wild before the version (wrong display in the edit control)
                //Note: this is being sent every time you click VERSION, so your selection of WILD is reset when you just peek at the VERSION options 
            }
            return DrawList(lpdis, label);
        }
        case ID_LANGUAGES_LIST:
        {
            u8* label = (lpdis->itemID >= LANGUAGES_ACT_MAX) ? "LANGUAGE       " : LanguagesActual[lpdis->itemID];
            return DrawList(lpdis, label);
        }
        case ID_WILDS_LIST:
        {
            u8* label = (lpdis->itemID >= OG_WILDS_MAX) ? "WILD           " : OgWilds[PROFILE_Current.version][lpdis->itemID];
            return DrawList(lpdis, label);
        }
        }
        return TRUE;
    }

    default:
        break;
    }
    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

static LRESULT WINAPI ResultsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    /* Handles messages sent to HWND_Results */
    switch (uMsg)
    {

    case WM_SETCURSOR:
    {
        if (LOWORD(lParam) == HTCLIENT) { //cursor is in the client area of a window
            HWND hwnd = (HWND)wParam;
            int menu = (int)GetMenu(hwnd);
            switch (menu)
            {
            case ID_RESULTS_HEADER:
            case ID_COPY_SEED_BUTTON:
                SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_HAND));
                break;
            default:
                SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
            }
        }
        return TRUE;
    }

    case WM_COMMAND:
    {
        if (AuthorizeSearch) { return 0; } //don't process commands when a search is running

        u16 notification = HIWORD(wParam);

        switch (notification)
        {
        case LBN_SELCHANGE:
        {
            int idx = SendMessageA(HWND_ResultsList, LB_GETCURSEL, 0, 0);
            if (idx == LB_ERR) { return 0; } //no item selected
            if (ResultsMode == MODE_FILES) {
                u8 path[PATH_REL_LENGTH_MAX] = { 0 };
                GetResultsPath(idx, path);
                LoadResultsFileDetails(path);
            }
            else if (ResultsMode == MODE_RESULTS) {
                LoadResultDetails(idx);
            }
            break;
        }
        case LBN_DBLCLK:
        {
            if (ResultsMode == MODE_RESULTS) { break; }
            int idx = SendMessageA(HWND_ResultsList, LB_GETCURSEL, 0, 0);
            if (idx == LB_ERR) { return 0; } //no item selected
            u8 path[PATH_REL_LENGTH_MAX] = { 0 };
            GetResultsPath(idx, path);
            ResultsMode = MODE_RESULTS;
            if (LoadResultsFromFile(path) != APP_RESUME) {
                MessageBoxA(HWND_AppMain, "Couln't load results file.", "Error", MB_OK | MB_ICONERROR);
            }
            break;
        }
        case BN_CLICKED:
        {
            switch (wParam)
            {
            case ID_RESULTS_HEADER:
                ResultsMode = MODE_FILES;
                ListResultsFiles();
                break;
            case ID_COPY_SEED_BUTTON:
            {
                int idx = SendMessageA(HWND_ResultsList, LB_GETCURSEL, 0, 0);
                if (idx == LB_ERR) { break; } //no item selected
                EditCopy();
                break;
            }
            }
        }
        }


        return 0;
    }

    case WM_MEASUREITEM:
    {
        /* Set the height of each item */
        LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
        lpmis->itemHeight = COMBOBOX_HEIGHT;
        return TRUE;
    }

    case WM_CTLCOLORLISTBOX:
    {
        return (LRESULT)HBRUSH_Dark; //need to return handle to a brush
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, MOTOR_COLOR_DARK);
        SetTextColor(hdc, MOTOR_COLOR_TEAL_H);
        SelectObject(hdc, myFont);
        return (LRESULT)HBRUSH_Dark; //need to return handle to a brush
    }

    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
        switch (wParam)
        {
        case ID_RESULTS_HEADER: return DrawButton(lpdis, MOTOR_COLOR_PUMP, "RESULTS", sizeof("RESULTS") - 1);
        case ID_COPY_SEED_BUTTON: return DrawButton(lpdis, MOTOR_COLOR_TEAL, "COPY SEED TO CLIPBOARD", sizeof("COPY SEED TO CLIPBOARD") - 1);
        case ID_RESULTS_LIST: return DrawResultsList(lpdis);
        }
        return TRUE;
    }

    case WM_PAINT:
    {
        { //HWND_Results esketit
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(HWND_Results, &ps);
            SelectObject(hdc, HPEN_Teal);
            SelectObject(hdc, HBRUSH_Dark);
            RoundRect(hdc, 0, 0, SEARCH_RESULTS_WIDTH, SEARCH_RESULTS_HEIGHT, APP_BORDER_RADIUS_M, APP_BORDER_RADIUS_M);
            EndPaint(HWND_Results, &ps);
        }
        { //HWND_ResultDetails esketit
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(HWND_ResultDetails, &ps);
            SelectObject(hdc, HPEN_Teal);
            SelectObject(hdc, HBRUSH_Dark);
            RoundRect(hdc, 0, 0, DETAILS_WIDTH, DETAILS_HEIGHT, APP_BORDER_RADIUS_M, APP_BORDER_RADIUS_M);
            EndPaint(HWND_ResultDetails, &ps);
        }
        return 0;
    }

    default:
        break;
    }
    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

static LRESULT WINAPI AppMainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    /* Handles messages sent to HWND_AppMain */
    switch (uMsg)
    {

        //TODO: uncomment in final build?
        //only when a search is running
        //case WM_CLOSE:
        //{
        //    if (AuthorizeSearch) {
        //        PauseSearch = TRUE;
        //        //doesn't work, need to wait for ConfirmAbortSearch
        //        if (!AbortSearch) { return 0; }
        //    }
        //    DestroyWindow(HWND_AppMain);
        //    return 0;
        //}

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }

    }

    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

/* Entry point */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    Esketit();

    WC_AppMain.cbSize = sizeof(WNDCLASSEXA);
    WC_AppMain.style = CS_OWNDC;
    WC_AppMain.lpfnWndProc = &AppMainProc;
    WC_AppMain.hInstance = hInstance;
    WC_AppMain.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(101)); //loads Motor_logo.ico
    WC_AppMain.hbrBackground = HBRUSH_Dark;
    WC_AppMain.lpszClassName = "WC_AppMain";

    if (!RegisterClassExA(&WC_AppMain)) { return GetLastError(); }

    HWND_AppMain = CreateWindowA(
        WC_AppMain.lpszClassName,
        "Motor v2.0.0",
        WS_VISIBLE | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        APP_WINDOW_WIDTH,
        APP_WINDOW_HEIGHT,
        0,
        0,
        hInstance,
        0
    );

    /* Handle to instance of the Main Window, referenced by its children */
    HINSTANCE HINSTANCE_AppMain = (HINSTANCE)GetWindowLongPtrA(HWND_AppMain, GWLP_HINSTANCE);

    HWND_ProgressBar = CreateWindowA(
        PROGRESS_CLASSA,
        NULL,
        WS_VISIBLE | WS_CHILD | PBS_SMOOTH,
        APP_WINDOW_PADDING_M,
        SEARCH_RESULTS_HEIGHT + 2 * APP_WINDOW_PADDING_S + 4,
        SEARCH_PARAMS_WIDTH,
        8,
        HWND_AppMain,
        (HMENU)ID_SEARCH_PROGRESS_BAR,
        HINSTANCE_AppMain,
        0
    );

    if (!HWND_ProgressBar) { return GetLastError(); }

    { //Progress bar style
        LONG style = GetWindowLongA(HWND_ProgressBar, GWL_EXSTYLE) ^ WS_EX_STATICEDGE; //removes border
        SetWindowLongA(HWND_ProgressBar, GWL_EXSTYLE, style);
    }

    WC_Results.cbSize = sizeof(WNDCLASSEXA);
    WC_Results.style = CS_OWNDC; //already inherit from parent?
    WC_Results.lpfnWndProc = &ResultsProc;
    WC_Results.hInstance = HINSTANCE_AppMain;
    WC_Results.lpszClassName = "WC_Results";

    if (!RegisterClassExA(&WC_Results)) { return GetLastError(); }

    HWND_Results = CreateWindowA(
        WC_Results.lpszClassName, //system class
        NULL,
        WS_VISIBLE | WS_CHILD,
        SEARCH_RESULTS_X,
        SEARCH_RESULTS_Y,
        SEARCH_RESULTS_WIDTH,
        SEARCH_RESULTS_HEIGHT,
        HWND_AppMain,
        NULL,
        HINSTANCE_AppMain,
        NULL
    );

    /* Handle to instance of the Search Results Sub Window, referenced by its children */
    HINSTANCE HINSTANCE_ResultsWindow = (HINSTANCE)GetWindowLongPtrA(HWND_Results, GWLP_HINSTANCE);

    HWND_ResultsHeader = CreateWindowA(
        "BUTTON",//WC_ResultsHeader.lpszClassName,
        "RESULTS",
        WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        0,
        0,
        SEARCH_RESULTS_WIDTH,
        BUTTON_WINDOW_HEIGHT,
        HWND_Results,
        (HMENU)ID_RESULTS_HEADER,
        HINSTANCE_ResultsWindow,
        NULL
    );

    HWND_ResultsList = CreateWindowA(
        "LISTBOX",
        NULL,
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_HASSTRINGS | LBS_NOTIFY | LBS_OWNERDRAWFIXED,
        APP_WINDOW_PADDING_S,
        BUTTON_WINDOW_HEIGHT + APP_WINDOW_PADDING_S,
        SEARCH_RESULTS_WIDTH - 2 * APP_WINDOW_PADDING_S,
        SEARCH_RESULTS_HEIGHT - BUTTON_WINDOW_HEIGHT,
        HWND_Results,
        (HMENU)ID_RESULTS_LIST,
        HINSTANCE_ResultsWindow,
        NULL
    );

    WC_ResultDetails.cbSize = sizeof(WNDCLASSEXA);
    WC_ResultDetails.style = CS_OWNDC;
    WC_ResultDetails.lpfnWndProc = &ResultsProc;
    WC_ResultDetails.hInstance = HINSTANCE_AppMain;
    //WC_ResultDetails.hbrBackground = HBRUSH_Pump;
    WC_ResultDetails.lpszClassName = "WC_ResultsDetails";

    if (!RegisterClassExA(&WC_ResultDetails)) { return GetLastError(); }

    HWND_ResultDetails = CreateWindowA(
        WC_ResultDetails.lpszClassName, //"STATIC",
        NULL,
        WS_VISIBLE | WS_CHILD,
        DETAILS_X,
        DETAILS_Y,
        DETAILS_WIDTH,
        DETAILS_HEIGHT,
        HWND_AppMain,
        NULL,
        HINSTANCE_AppMain,
        NULL
    );

    /* Handle to instance of the Details Window, referenced by its children */
    HINSTANCE HINSTANCE_DetailsWindow = (HINSTANCE)GetWindowLongPtrA(HWND_ResultDetails, GWLP_HINSTANCE);

    HWND_CopyButton = CreateWindowA(
        "BUTTON", //system class
        "COPY SEED TO CLIPBOARD",
        WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        0,
        DETAILS_HEIGHT - BUTTON_WINDOW_HEIGHT,
        DETAILS_WIDTH,
        BUTTON_WINDOW_HEIGHT,
        HWND_ResultDetails,
        (HMENU)ID_COPY_SEED_BUTTON,
        HINSTANCE_DetailsWindow,
        NULL
    );

    HWND_DetailsList = CreateWindowA(
        "STATIC", //system class
        "",
        WS_VISIBLE | WS_CHILD,// | SS_OWNERDRAW,
        APP_WINDOW_PADDING_S,
        APP_WINDOW_PADDING_S,
        DETAILS_WIDTH - 2 * APP_WINDOW_PADDING_S,
        DETAILS_HEIGHT - BUTTON_WINDOW_HEIGHT - 2 * APP_WINDOW_PADDING_S,
        HWND_ResultDetails,
        (HMENU)ID_DETAILS,
        HINSTANCE_DetailsWindow,
        NULL
    );

    WC_SearchParameters.cbSize = sizeof(WNDCLASSEXA);
    WC_SearchParameters.style = CS_OWNDC;
    WC_SearchParameters.lpfnWndProc = &SearchParametersProc;
    WC_SearchParameters.hInstance = HINSTANCE_AppMain;
    WC_SearchParameters.lpszClassName = "WC_SearchParameters";

    if (!RegisterClassExA(&WC_SearchParameters)) { return GetLastError(); }

    HWND_SearchParameters = CreateWindowA(
        WC_SearchParameters.lpszClassName,
        NULL,
        WS_VISIBLE | WS_CHILD,
        SEARCH_PARAMS_X,
        SEARCH_PARAMS_Y,
        SEARCH_PARAMS_WIDTH,
        SEARCH_PARAMS_HEIGHT,
        HWND_AppMain,
        (HMENU)ID_SEARCH_PARAMETERS,
        HINSTANCE_AppMain,
        NULL
    );

    /* Handle to instance of the Search Parameter Window, referenced by its children */
    HINSTANCE HINSTANCE_SearchWindow = (HINSTANCE)GetWindowLongPtrA(HWND_SearchParameters, GWLP_HINSTANCE);

    HWND_SearchButton = CreateWindowA(
        "BUTTON", //system class
        "SEARCH",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        SEARCH_BUTTON_X,
        SEARCH_BUTTON_Y,
        SEARCH_BUTTON_WIDTH,
        SEARCH_BUTTON_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_SEARCH_BUTTON,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_TidInput = CreateWindowA(
        "EDIT", //system class
        "TID",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_CENTER,
        APP_WINDOW_PADDING_S,
        APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        TEXT_INPUT_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_SEARCH_INPUT,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_SidInput = CreateWindowA(
        "EDIT", //system class
        "SID",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_CENTER,
        APP_WINDOW_PADDING_S,
        TEXT_INPUT_HEIGHT + 2 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        TEXT_INPUT_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_SEARCH_INPUT,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_VersionInput = CreateWindowA(
        "COMBOBOX", //system class
        "VERSION",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_UPPERCASE | CBS_HASSTRINGS | CBS_OWNERDRAWFIXED,
        TEXT_INPUT_WIDTH + 2 * APP_WINDOW_PADDING_S,
        APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        COMBOBOX_HEIGHT * (VERSIONS_MAX + 2), //why +2?
        HWND_SearchParameters,
        (HMENU)ID_VERSIONS_LIST,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_LanguageInput = CreateWindowA(
        "COMBOBOX", //system class
        "LANGUAGE",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_UPPERCASE | CBS_HASSTRINGS | CBS_OWNERDRAWFIXED,
        TEXT_INPUT_WIDTH + 2 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_HEIGHT + 3 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        COMBOBOX_HEIGHT * (LANGUAGES_ACT_MAX + 2),
        HWND_SearchParameters,
        (HMENU)ID_LANGUAGES_LIST,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_WildInput = CreateWindowA(
        "COMBOBOX", //system class
        "WILD",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_UPPERCASE | CBS_HASSTRINGS | CBS_OWNERDRAWFIXED,
        TEXT_INPUT_WIDTH + 2 * APP_WINDOW_PADDING_S,
        2 * TEXT_INPUT_HEIGHT + 5 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        COMBOBOX_HEIGHT * (OG_WILDS_MAX + 2),
        HWND_SearchParameters,
        (HMENU)ID_WILDS_LIST,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_AslrInput = CreateWindowA(
        "EDIT", //system class
        "ASLR",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_CENTER,
        APP_WINDOW_PADDING_S,
        2 * TEXT_INPUT_HEIGHT + 3 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        TEXT_INPUT_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_SEARCH_INPUT,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_SeedInput = CreateWindowA(
        "EDIT", //system class
        "SEED",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_UPPERCASE | ES_CENTER,
        APP_WINDOW_PADDING_S,
        3 * TEXT_INPUT_HEIGHT + 4 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        TEXT_INPUT_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_SEARCH_INPUT,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_FramesInput = CreateWindowA(
        "EDIT", //system class
        "FRAMES",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_CENTER,
        APP_WINDOW_PADDING_S,
        4 * TEXT_INPUT_HEIGHT + 5 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        TEXT_INPUT_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_SEARCH_INPUT,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_SpeciesFilterInput = CreateWindowA(
        "EDIT", //system class
        "SPECIES",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_CENTER,
        2 * TEXT_INPUT_WIDTH + 3 * APP_WINDOW_PADDING_S,
        APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        TEXT_INPUT_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_SPECIES_FILTER,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_ItemFilterInput = CreateWindowA(
        "EDIT", //system class
        "ITEM",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_CENTER,
        2 * TEXT_INPUT_WIDTH + 3 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_HEIGHT + 2 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        TEXT_INPUT_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_ITEM_FILTER,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_MoveFilterInput = CreateWindowA(
        "EDIT", //system class
        "MOVE",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_CENTER,
        2 * TEXT_INPUT_WIDTH + 3 * APP_WINDOW_PADDING_S,
        2 * TEXT_INPUT_HEIGHT + 3 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        TEXT_INPUT_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_MOVE_FILTER,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_ResetSearchParams = CreateWindowA(
        "BUTTON", //system class
        "RESET",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        TEXT_INPUT_WIDTH + 2 * APP_WINDOW_PADDING_S,
        4 * TEXT_INPUT_HEIGHT + 5 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        RESET_BUTTON_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_RESET_BUTTON,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_LoadSearchParams = CreateWindowA(
        "BUTTON", //system class
        "LOAD",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        APP_WINDOW_PADDING_S,
        5 * TEXT_INPUT_HEIGHT + 6 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        LOAD_BUTTON_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_LOAD_BUTTON,
        HINSTANCE_SearchWindow,
        NULL
    );

    HWND_SaveSearchParams = CreateWindowA(
        "BUTTON", //system class
        "SAVE",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        TEXT_INPUT_WIDTH + 2 * APP_WINDOW_PADDING_S,
        5 * TEXT_INPUT_HEIGHT + 6 * APP_WINDOW_PADDING_S,
        TEXT_INPUT_WIDTH,
        SAVE_BUTTON_HEIGHT,
        HWND_SearchParameters,
        (HMENU)ID_SAVE_BUTTON,
        HINSTANCE_SearchWindow,
        NULL
    );

    /* Profile slot buttons */
    for (u8 i = 0; i < PROFILE_SLOTS_MAX; i++) {
        HWND_ProfileSlotButton[i] = CreateWindowA(
            "BUTTON",
            "P1",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            APP_WINDOW_PADDING_S + (PROFILE_SLOT_BUTTON_WIDTH + APP_WINDOW_PADDING_S) * i,
            SAVE_BUTTON_HEIGHT + 5 * TEXT_INPUT_HEIGHT + 7 * APP_WINDOW_PADDING_S,
            PROFILE_SLOT_BUTTON_WIDTH,
            PROFILE_SLOT_BUTTON_WIDTH,
            HWND_SearchParameters,
            (HMENU)(ID_PROFILE_SLOT_BUTTON + i),
            HINSTANCE_SearchWindow,
            NULL
        );
    }

    if (!HWND_AppMain) { return APP_ERR_MAIN_CREATE; } //abort if failed to create main window

    /* Show the main window and its children windows */
    ShowWindow(HWND_AppMain, nCmdShow);
    UpdateWindow(HWND_AppMain);

    /* Prevent user from writing more characters than needed */
    SendMessageA(HWND_TidInput, EM_SETLIMITTEXT, U16_DIGITS_DEC_MAX, 0);
    SendMessageA(HWND_SidInput, EM_SETLIMITTEXT, U16_DIGITS_DEC_MAX, 0);
    SendMessageA(HWND_SpeciesFilterInput, EM_SETLIMITTEXT, 3, 0);
    SendMessageA(HWND_ItemFilterInput, EM_SETLIMITTEXT, 3, 0);
    SendMessageA(HWND_MoveFilterInput, EM_SETLIMITTEXT, 3, 0);
    SendMessageA(HWND_SeedInput, EM_SETLIMITTEXT, U32_DIGITS_HEX_MAX, 0);
    SendMessageA(HWND_FramesInput, EM_SETLIMITTEXT, U32_DIGITS_DEC_MAX, 0);
    SendMessageA(HWND_AslrInput, EM_SETLIMITTEXT, ASLR_DIGITS_DEC_MAX, 0);

    /* Progress bar init */
    SendMessageA(HWND_ProgressBar, PBM_SETBARCOLOR, 0, MOTOR_COLOR_TEAL);
    SendMessageA(HWND_ProgressBar, PBM_SETBKCOLOR, 0, MOTOR_COLOR_DARK);
    //SendMessageA(HWND_ProgressBar, PBM_SETPOS, 50, 0); //debug

    /* Populate combo boxes */
    for (u8 i = 0; i < LANGUAGES_ACT_MAX; i++) { SendMessageA(HWND_LanguageInput, CB_ADDSTRING, 0, (LPARAM)LanguagesActual[i]); }
    for (u8 i = 0; i < VERSIONS_MAX; i++) { SendMessageA(HWND_VersionInput, CB_ADDSTRING, 0, (LPARAM)Versions[i]); }
    for (u8 i = 0; i < OG_WILDS_MAX; i++) { SendMessageA(HWND_WildInput, CB_ADDSTRING, 0, (LPARAM)OgWilds[VERSION_DIAMOND][i]); }

    /* Create .results and .profiles directories if they don't already exist */
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
    CreateDirectoryA(ResultsDirectory, &sa);
    CreateDirectoryA(ProfilesDirectory, &sa);
    SetFileAttributesA(ProfilesDirectory, FILE_ATTRIBUTE_HIDDEN); //.profiles is hidden
    /* Check if the PROFILES file exists, if not zero-initialize it */
    CheckProfileFileSize();
    for (u8 i = PROFILE_SLOTS_MAX; i > 0; i--) { SetProfileSlotState(i - 1); } //init slots draw, slot 0 is active

    /* Tab */
    RegisterHotKey(HWND_SearchParameters, VK_TAB, MOD_NOREPEAT, VK_TAB);

    /* Create exclusive thread for the search */
    SYSTEM_INFO SystemInfo = { 0 };
    GetSystemInfo(&SystemInfo);
    SearchThreadsMax = (u8)SystemInfo.dwNumberOfProcessors / 2;
    if (SearchThreadsMax == 0) { SearchThreadsMax = 1; }
    if (SearchThreadsMax > SEARCH_THREADS_MAX) { SearchThreadsMax = SEARCH_THREADS_MAX; }

    /* Master search thread operates and joins the loops */
    DWORD MainSearchThreadID;
    HANDLE MainSearchThread = CreateThread(NULL, 0, &MotorThreadProc, NULL, 0, &MainSearchThreadID);
    if (MainSearchThread == NULL) { return GetLastError(); }

    /* Search threads */
    for (u8 i = 0; i < SearchThreadsMax; i++) {
        SearchThreadIndex[i] = i;
        SearchThread[i] = CreateThread(NULL, 0, &MotorSearchLoopThreadProc, &SearchThreadIndex[i], 0, &SearchThreadID[i]);
        if (SearchThread[i] == NULL) { return GetLastError(); }
    }

    MSG msg;
    /* Main processing loop, terminates the program on exit */
    while (GetMessageA(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    };

    FreeEsketit(); //useless?

    return APP_EXIT;
};
