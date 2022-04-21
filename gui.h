// gui.h
// Window dimensions, strings, colors, etc.

#pragma once
#include "common.h"

/* Dimensions - application main window */
#define BASE_WIDTH                      (800)
#define BASE_HEIGHT                     (720)
#define APP_BORDER_RADIUS_M             (8)
#define APP_BORDER_RADIUS_S             (4)
#define APP_WINDOW_PADDING_L            (64)
#define APP_WINDOW_PADDING_M            (16)
#define APP_WINDOW_PADDING_S            (8)
#define APP_WINDOW_WIDTH                (BASE_WIDTH + 16) //hack
#define APP_WINDOW_HEIGHT               (BASE_HEIGHT + 16 + 23) //hack
/* Dimensions - progress bar */
#define PROGRESS_WINDOW_WIDTH           (BASE_WIDTH/2)
#define PROGRESS_WINDOW_HEIGHT          (BASE_HEIGHT/2)
/* Dimensions - inputs */
#define BUTTON_WINDOW_HEIGHT            (24)
#define TEXT_INPUT_WIDTH                (128)
#define TEXT_INPUT_HEIGHT               (16)
#define SAVE_BUTTON_HEIGHT              (2 * TEXT_INPUT_HEIGHT)
#define LOAD_BUTTON_HEIGHT              (SAVE_BUTTON_HEIGHT)
#define RESET_BUTTON_HEIGHT             (SAVE_BUTTON_HEIGHT)//(TEXT_INPUT_HEIGHT)
#define COMBOBOX_HEIGHT                 (20)
/* Dimensions - Search Parameters sub window */
#define SEARCH_PARAMS_X                 (APP_WINDOW_PADDING_M)
#define SEARCH_PARAMS_Y                 (3 * BASE_HEIGHT / 5)
#define SEARCH_PARAMS_WIDTH             (BASE_WIDTH - 2 * APP_WINDOW_PADDING_M)
#define SEARCH_PARAMS_HEIGHT            (2 * BASE_HEIGHT / 5 - APP_WINDOW_PADDING_M)
/* Dimensions - Search Button */
#define SEARCH_BUTTON_X                 (4 * TEXT_INPUT_WIDTH + 5 * APP_WINDOW_PADDING_S)
#define SEARCH_BUTTON_Y                 (4 * SEARCH_PARAMS_HEIGHT / 5)
#define SEARCH_BUTTON_WIDTH             (SEARCH_PARAMS_WIDTH - SEARCH_BUTTON_X - APP_WINDOW_PADDING_S)
#define SEARCH_BUTTON_HEIGHT            (SEARCH_PARAMS_HEIGHT / 5 - APP_WINDOW_PADDING_S)
/* Dimensions - Search Results sub window */
#define SEARCH_RESULTS_X                (APP_WINDOW_PADDING_M)
#define SEARCH_RESULTS_Y                (APP_WINDOW_PADDING_M)
#define SEARCH_RESULTS_WIDTH            (2 * BASE_WIDTH / 5 - APP_WINDOW_PADDING_M)
#define SEARCH_RESULTS_HEIGHT           (3 * BASE_HEIGHT / 5 - 2 * APP_WINDOW_PADDING_M)
/* Dimensions - Result Details sub window */
#define DETAILS_WIDTH                   (3 * BASE_WIDTH / 5 - 2 * APP_WINDOW_PADDING_M)
#define DETAILS_HEIGHT                  (3 * BASE_HEIGHT / 5 - 2 * APP_WINDOW_PADDING_M)
#define DETAILS_X                       (SEARCH_RESULTS_X + SEARCH_RESULTS_WIDTH + APP_WINDOW_PADDING_M)
#define DETAILS_Y                       (APP_WINDOW_PADDING_M)
/* Dimensions - Seed to time button and year input field */
#define SEEDTIME_WIDTH                  (DETAILS_WIDTH - TEXT_INPUT_WIDTH - 2 * APP_WINDOW_PADDING_S)
/* Profile slot buttons */
#define PROFILE_SLOT_BUTTON_WIDTH       (26)

/* Motor's Color Palette #GraphicDesignIsMyPassion */
//note: #RRGGBB -> 0xBBGGRR
#define MOTOR_COLOR_DARK                (0x1b1611)
#define MOTOR_COLOR_GRAY                (0xdddddd)
#define MOTOR_COLOR_TEAL                (0x907a00)
#define MOTOR_COLOR_TEAL_H              (0xbca616)
#define MOTOR_COLOR_PUMP                (0x214db1)
#define MOTOR_COLOR_PUMP_H              (0x3779dd)
#define MOTOR_COLOR_DEBUG               (0xff00ff) //magenta

#define ODS_INVERT                      (0x12345678) //custom owner drawn state

/* MessageBox labels */
#define MBL_CONFIRM                     ("Confirm")
#define MBL_ERROR                       ("Error")
#define MBL_NOTICE                      ("Notice")

/* HMENU Identifiers */
enum {
    ID_SEARCH_PARAMETERS = 100,
    ID_SEARCH_BUTTON,
    ID_SEARCH_INPUT,
    ID_SPECIES_FILTER,
    ID_ITEM_FILTER,
    ID_MOVE_FILTER,
    ID_SAVE_BUTTON,
    ID_LOAD_BUTTON,
    ID_RESET_BUTTON,
    ID_VERSIONS_LIST,
    ID_LANGUAGES_LIST,
    ID_WILDS_LIST,
    ID_PROFILE_SLOT_BUTTON = 0x1000, //for bitmask

    ID_RESULTS = 200,
    ID_RESULTS_HEADER,
    ID_RESULTS_LIST,
    ID_COPY_SEED_BUTTON,
    ID_YEAR_FILTER,
    ID_DETAILS,
    ID_SEARCH_PROGRESS_BAR
};

/* Preprocessor macros */
#define DeclareWindowAndClass(name) HWND HWND_##name; WNDCLASSEXA WC_##name;
#define DeclareBrushAndPen(color) HBRUSH HBRUSH_##color; HPEN HPEN_##color;

/* Globals */
HFONT myFont;
//TODO: dynamically allocate memory for this
//48 KB
u8 ResultsListStrings[1500][2 * STRING_LENGTH_MAX] = { 0 };

/* Windows and classes */
DeclareWindowAndClass(AppMain)

DeclareWindowAndClass(ProgressBar)

DeclareWindowAndClass(Results)
DeclareWindowAndClass(ResultsHeader)
DeclareWindowAndClass(ResultsList)
DeclareWindowAndClass(ResultDetails)
DeclareWindowAndClass(DetailsList)
DeclareWindowAndClass(CopyButton)
DeclareWindowAndClass(YearFilter)

DeclareWindowAndClass(ProfileSlotButton[PROFILE_SLOTS_MAX])
DeclareWindowAndClass(SearchParameters)
DeclareWindowAndClass(TidInput)
DeclareWindowAndClass(SidInput)
DeclareWindowAndClass(VersionInput)
DeclareWindowAndClass(LanguageInput)
DeclareWindowAndClass(AslrInput)
DeclareWindowAndClass(WildInput)
DeclareWindowAndClass(SeedInput)
DeclareWindowAndClass(FramesInput)
DeclareWindowAndClass(MacInput)
DeclareWindowAndClass(SpeciesFilterInput)
DeclareWindowAndClass(MoveFilterInput)
DeclareWindowAndClass(ItemFilterInput)
DeclareWindowAndClass(ResetSearchParams)
DeclareWindowAndClass(LoadSearchParams)
DeclareWindowAndClass(SaveSearchParams)
DeclareWindowAndClass(SearchButton)

/* Brushes and pens */
DeclareBrushAndPen(Dark)
DeclareBrushAndPen(Teal)
DeclareBrushAndPen(Teal_h)
DeclareBrushAndPen(Pump)
DeclareBrushAndPen(Pump_h)
DeclareBrushAndPen(Gray)
DeclareBrushAndPen(Debug)

/* Functions */

static void Esketit(void) {
    /* All the pretty things */

    //Use an old Windows System Font for highest compatibility
    myFont = CreateFontA(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier");

    HBRUSH_Dark = CreateSolidBrush(MOTOR_COLOR_DARK);
    HBRUSH_Teal = CreateSolidBrush(MOTOR_COLOR_TEAL);
    HBRUSH_Teal_h = CreateSolidBrush(MOTOR_COLOR_TEAL_H);
    HBRUSH_Pump = CreateSolidBrush(MOTOR_COLOR_PUMP);
    HBRUSH_Pump_h = CreateSolidBrush(MOTOR_COLOR_PUMP_H);
    HBRUSH_Gray = CreateSolidBrush(MOTOR_COLOR_GRAY);
    HBRUSH_Debug = CreateSolidBrush(MOTOR_COLOR_DEBUG);

    /* All pens are solid with a width of 1 pixel */
    HPEN_Dark = CreatePen(PS_SOLID, 0, MOTOR_COLOR_DARK);
    HPEN_Teal = CreatePen(PS_SOLID, 0, MOTOR_COLOR_TEAL);
    HPEN_Teal_h = CreatePen(PS_SOLID, 0, MOTOR_COLOR_TEAL_H);
    HPEN_Pump = CreatePen(PS_SOLID, 0, MOTOR_COLOR_PUMP);
    HPEN_Pump_h = CreatePen(PS_SOLID, 0, MOTOR_COLOR_PUMP_H);
    HPEN_Gray = CreatePen(PS_SOLID, 0, MOTOR_COLOR_GRAY);
    HPEN_Debug = CreatePen(PS_SOLID, 0, MOTOR_COLOR_DEBUG);
}

static void FreeEsketit(void) {
    /* Free resources */
    // Might not even be needed but whatever
    DeleteObject(HBRUSH_Dark);
    DeleteObject(HBRUSH_Teal);
    DeleteObject(HBRUSH_Teal_h);
    DeleteObject(HBRUSH_Pump);
    DeleteObject(HBRUSH_Pump_h);
    DeleteObject(HBRUSH_Gray);
    DeleteObject(HBRUSH_Debug);
    DeleteObject(HPEN_Dark);
    DeleteObject(HPEN_Teal);
    DeleteObject(HPEN_Teal_h);
    DeleteObject(HPEN_Pump);
    DeleteObject(HPEN_Pump_h);
    DeleteObject(HPEN_Gray);
    DeleteObject(HPEN_Debug);
}

static HBRUSH GetBrushFromColor(int color) {
    /* Return the HBRUSH corresponding to a MOTOR_COLOR */
    switch (color)
    {
    case MOTOR_COLOR_DARK: return HBRUSH_Dark;
    case MOTOR_COLOR_GRAY: return HBRUSH_Gray;
    case MOTOR_COLOR_TEAL: return HBRUSH_Teal;
    case MOTOR_COLOR_TEAL_H: return HBRUSH_Teal_h;
    case MOTOR_COLOR_PUMP: return HBRUSH_Pump;
    case MOTOR_COLOR_PUMP_H: return HBRUSH_Pump_h;
    }
    return HBRUSH_Debug;
}

static HPEN GetPenFromColor(int color) {
    /* Return the HPEN corresponding to a MOTOR_COLOR */
    switch (color)
    {
    case MOTOR_COLOR_DARK: return HPEN_Dark;
    case MOTOR_COLOR_GRAY: return HPEN_Gray;
    case MOTOR_COLOR_TEAL: return HPEN_Teal;
    case MOTOR_COLOR_TEAL_H: return HPEN_Teal_h;
    case MOTOR_COLOR_PUMP: return HPEN_Pump;
    case MOTOR_COLOR_PUMP_H: return HPEN_Pump_h;
    }
    return HPEN_Debug;
}

static BOOL DrawButton_hover(LPDRAWITEMSTRUCT lpdis, int color, u8* label, u8 length) {
    /* Draw button but hollow */
    SelectObject(lpdis->hDC, HBRUSH_Dark);
    Rectangle(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
    /* Draw the rounded button */
    SelectObject(lpdis->hDC, GetPenFromColor(color)); //stroke color
    SelectObject(lpdis->hDC, HBRUSH_Dark); //fill color
    RoundRect(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom, APP_BORDER_RADIUS_S, APP_BORDER_RADIUS_S);
    /* Draw the text of the button */
    SelectObject(lpdis->hDC, myFont);
    SetBkColor(lpdis->hDC, MOTOR_COLOR_DARK);
    SetTextColor(lpdis->hDC, color);
    DrawTextA(lpdis->hDC, label, length, &lpdis->rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
    return TRUE;
}

static BOOL DrawButton(LPDRAWITEMSTRUCT lpdis, int color, u8* label, u8 length) {
    /**/
    /* Draw the background rect the same color as main window */
    if (lpdis->itemState == (ODS_FOCUS | ODS_SELECTED) || lpdis->itemState == ODS_INVERT) {
        return DrawButton_hover(lpdis, color, label, length); //only when clicked
    }
    SelectObject(lpdis->hDC, HBRUSH_Dark);
    Rectangle(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
    /* Draw the rounded button */
    SelectObject(lpdis->hDC, GetPenFromColor(color)); //stroke color
    SelectObject(lpdis->hDC, GetBrushFromColor(color)); //fill color
    RoundRect(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom, APP_BORDER_RADIUS_S, APP_BORDER_RADIUS_S);
    /* Draw the text of the button */
    SelectObject(lpdis->hDC, myFont);
    SetBkColor(lpdis->hDC, color);
    SetTextColor(lpdis->hDC, MOTOR_COLOR_GRAY); //always
    DrawTextA(lpdis->hDC, label, length, &lpdis->rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
    return TRUE;
}

static BOOL DrawList(LPDRAWITEMSTRUCT lpdis, u8* label) {
    /* Draws a combobox control: drop down list and static text at the top */
    if (lpdis->itemState == (ODS_FOCUS | ODS_SELECTED)) { //mouse hover highlight
        SelectObject(lpdis->hDC, HBRUSH_Pump);
        SelectObject(lpdis->hDC, HPEN_Pump);
        Rectangle(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
        SetBkColor(lpdis->hDC, MOTOR_COLOR_PUMP);
        SetTextColor(lpdis->hDC, MOTOR_COLOR_GRAY);
    }
    else if (lpdis->itemState < 2) { //hack: avoids drawing Rectangle on the static text
        SelectObject(lpdis->hDC, HBRUSH_Dark);
        SelectObject(lpdis->hDC, HPEN_Teal_h);
        Rectangle(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
        SetBkColor(lpdis->hDC, MOTOR_COLOR_DARK);
        SetTextColor(lpdis->hDC, MOTOR_COLOR_TEAL_H);
    }
    else { //static text
        SetBkColor(lpdis->hDC, MOTOR_COLOR_DARK);
        SetTextColor(lpdis->hDC, MOTOR_COLOR_TEAL_H);
    }
    SelectObject(lpdis->hDC, myFont);
    DrawTextA(lpdis->hDC, label, STRING_LENGTH_MAX, &lpdis->rcItem, DT_SINGLELINE | DT_VCENTER);
    return TRUE;
}

static BOOL DrawResultsList(LPDRAWITEMSTRUCT lpdis) {
    /* Draw each item in the results list */
    if (lpdis->itemState == (ODS_FOCUS | ODS_SELECTED) || lpdis->itemState == ODS_SELECTED) { //selected
        SetBkColor(lpdis->hDC, MOTOR_COLOR_PUMP);
        SetTextColor(lpdis->hDC, MOTOR_COLOR_GRAY);
    }
    else {
        SetBkColor(lpdis->hDC, MOTOR_COLOR_DARK);
        SetTextColor(lpdis->hDC, MOTOR_COLOR_TEAL_H);
    }
    SelectObject(lpdis->hDC, myFont);
    DrawTextA(lpdis->hDC, (LPCSTR)ResultsListStrings[lpdis->itemID], 2 * STRING_LENGTH_MAX, &lpdis->rcItem, DT_SINGLELINE | DT_VCENTER);
    return TRUE;
}

static HWND GetNextSearchParamTabStop(HWND current) {
    /* There's probably a better way to do this */
    if (current == HWND_TidInput) { return HWND_SidInput; }
    if (current == HWND_SidInput) { return HWND_AslrInput; }
    if (current == HWND_AslrInput) { return HWND_SeedInput; }
    if (current == HWND_SeedInput) { return HWND_FramesInput; }
    if (current == HWND_FramesInput) { return HWND_VersionInput; }
    if (current == HWND_VersionInput) { return HWND_LanguageInput; }
    if (current == HWND_LanguageInput) { return HWND_WildInput; }
    if (current == HWND_WildInput) { return HWND_MacInput; }
    if (current == HWND_MacInput) { return HWND_SpeciesFilterInput; }
    if (current == HWND_SpeciesFilterInput) { return HWND_ItemFilterInput; }
    if (current == HWND_ItemFilterInput) { return HWND_MoveFilterInput; }
    if (current == HWND_MoveFilterInput) { return HWND_TidInput; }
    return HWND_TidInput;
}

static APPSTATUS ConfirmAbortSearch(void) {
    /* Message box */
    int answer = MessageBoxA(NULL, "Do you want to abort the search?", MBL_CONFIRM, MB_YESNOCANCEL | MB_ICONQUESTION);
    if (answer == IDYES) { return APP_ABORT_SEARCH; }
    return APP_RESUME;
}

static void ErrorMessageBox_BadProfile(APPSTATUS code) {
    /* Message box when a search parameter is incorrect */
    u8 str[3 * STRING_LENGTH_MAX] = { 0 };
    sprintf(str, "%s is invalid, please correct it.", PROFILE_ErrorCodes[code - PROFILE_OK]);
    MessageBoxA(NULL, str, MBL_ERROR, MB_OK | MB_ICONWARNING);
}

static void SetWindowTextFromInt(HWND hwnd, u8* format, u64 value) {
    /* Format the string with value and send it to the window */
    u8 str[STRING_LENGTH_MAX] = { 0 };
    sprintf(str, format, value);
    SetWindowTextA(hwnd, str);
}

static void SetFilterInput(HWND hInput, const u8 strarr[][STRING_LENGTH_MAX], u16 max, u16* pparam) {
    /* Filters */
    //Enter a digit, see the corresponding string show up (ex: 151 -> Mew)
    u8 b[STRING_LENGTH_MAX] = { 0 };
    GetWindowTextA(hInput, b, STRING_LENGTH_MAX);
    ZeroLeftPadTextInputInt(b, FILTER_DIGITS_MAX);
    if (!IsValidIntString_dec(b, FILTER_DIGITS_MAX)) { //String
        if (GetIndexOfString(b, strarr, max) == -1) {
            *pparam = 0;
            SetWindowTextA(hInput, "Anything"); //default
        }
    }
    else { //Number string
        u32 value = AsciiToInt_dec16(b, FILTER_DIGITS_MAX);
        if (value >= max) {
            *pparam = 0;
            SetWindowTextA(hInput, "Anything"); //default
        }
        else {
            *pparam = value;
            SetWindowTextA(hInput, strarr[value]); //fetch string from array
        }
    }
}

static void SetTextInput_dec16(HWND hInput, u16 min, u16 max, u8 digits) {
    /* TID, SID, Year  */
    u8 b[U16_DIGITS_DEC_MAX + 1] = { 0 }; //null terminator discarded when b is sent to AsciiToInt_dec16
    GetWindowTextA(hInput, b, digits + 1);
    ZeroLeftPadTextInputInt(b, digits);
    u32 value = AsciiToInt_dec16(b, digits);
    if (value < min || value > max || !IsValidIntString_dec(b, digits))
    {
        sprintf(b, "%u", max);
    }
    SetWindowTextA(hInput, b);
}

static void SetTextInput_dec32(HWND hInput) {
    /* Frames limit = 0xFFFFFFFF */
    u8 b[U32_DIGITS_DEC_MAX + 1] = { 0 }; //null terminator discarded when b is sent to AsciiToInt_dec32
    GetWindowTextA(hInput, b, U32_DIGITS_DEC_MAX + 1);
    ZeroLeftPadTextInputInt(b, U32_DIGITS_DEC_MAX);
    u64 value = AsciiToInt_dec32(b);
    if (value > U32_VALUE_MAX || !IsValidIntString_dec(b, U32_DIGITS_DEC_MAX)) {
        SetWindowTextA(hInput, "4294967295");
    }
    else {
        SetWindowTextA(hInput, b);
    }
}

static void SetTextInput_hex32(HWND hInput) {
    /* Seed */
    u8 b[U32_DIGITS_HEX_MAX + 1] = { 0 }; //null terminator discarded when b is sent to AsciiToInt_hex32
    GetWindowTextA(hInput, b, U32_DIGITS_HEX_MAX + 1);
    ZeroLeftPadTextInputInt(b, U32_DIGITS_HEX_MAX);
    u32 value = AsciiToInt_hex32(b);
    if (value > U32_VALUE_MAX || !IsValidIntString_hex(b, U32_DIGITS_HEX_MAX)) {
        SetWindowTextA(hInput, "FFFFFFFF");
    }
    else {
        sprintf(b, "%08X", value);
        SetWindowTextA(hInput, b);
    }
}

static void SetTextInput_aslr(HWND hInput) {
    /* ASLR */
    u8 b[ASLR_DIGITS_DEC_MAX + 1] = { 0 }; //null terminator discarded when b is sent to AsciiToInt_aslr
    GetWindowTextA(hInput, b, ASLR_DIGITS_DEC_MAX + 1);
    ZeroLeftPadTextInputInt(b, ASLR_DIGITS_DEC_MAX);
    u32 value = AsciiToInt_dec16(b, ASLR_DIGITS_DEC_MAX);
    if (value > ASLR_VALUE_MAX || !IsValidIntString_dec(b, ASLR_DIGITS_DEC_MAX)) {
        SetWindowTextA(hInput, "11"); //not always 11, depends on language and version
    }
    else {
        SetWindowTextA(hInput, b);
    }
}

static void SetTextInput_mac(HWND hInput) {
    /* MAC address (hex) */
    u8 b[MAC_DIGITS_HEX_MAX + 1] = { 0 }; //null terminator discarded when b is sent to AsciiToInt_hex32
    GetWindowTextA(hInput, b, MAC_DIGITS_HEX_MAX + 1);
    ZeroLeftPadTextInputInt(b, MAC_DIGITS_HEX_MAX);
    u64 value = AsciiToInt_hex64(b);
    if (value > MAC_VALUE_MAX || !IsValidIntString_hex(b, MAC_DIGITS_HEX_MAX)) {
        SetWindowTextA(hInput, "010203040506");
    }
    else {
        sprintf(b, "%012llX", value);
        SetWindowTextA(hInput, b);
    }
}
