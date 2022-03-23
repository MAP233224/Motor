//common.h
//Used by every file

#pragma once

#ifdef UNICODE
#undef UNICODE
#endif

#include <time.h>
#include <stdio.h>
#include <string.h>
//#include <stdbool.h>
#include <stdint.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int APPSTATUS;

#define MOTOR_VERSION               ("2.0.0")
#define SEARCH_THREADS_MAX          (8)
#define PROFILE_SLOTS_MAX           (8)
#define STRING_LENGTH_MAX           (16)
#define PROFILE_CODE_OFFSET         (100)
#define U16_VALUE_MAX               (0xFFFF)
#define U16_DIGITS_DEC_MAX          (5)
#define U32_VALUE_MAX               (0xFFFFFFFF)
#define U32_DIGITS_HEX_MAX          (8)
#define U32_DIGITS_DEC_MAX          (10)
#define ASLR_VALUE_MAX              (11) //Global and Japanese
#define ASLR_VALUE_MAX_KO           (4) //Korean
#define ASLR_VALUE_MAX_EN           (3) //English, French, Italian, German and Spanish
#define ASLR_DIGITS_DEC_MAX         (2)
#define FILTER_DIGITS_MAX           (3)
#define PATH_ABS_LENGTH_MAX         (256)
#define PATH_REL_LENGTH_MAX         (4 * STRING_LENGTH_MAX)
/* Profile slots states */
#define PSS_EMPTY                    (0x00)
#define PSS_TAKEN                    (0x01)
#define PSS_ACTIVE                   (0x10)

#define DLOG(...) { char s[256]; sprintf(s, __VA_ARGS__); OutputDebugStringA(s); }

enum {
    /* Application status */
    APP_EXIT,
    APP_RESUME,
    APP_ERR_REGISTER_CLASS,
    APP_ERR_MAIN_CREATE,
    APP_ERR_OPEN_FILE,
    APP_ERR_PROFILE_SLOT,
    APP_ERR_PROFILE_EMPTY,
    APP_ERR_NORESULTS,
    APP_ABORT_SEARCH,
    /* Results display modes */
    MODE_FILES,
    MODE_RESULTS,
    /* PROFILE error codes */
    PROFILE_OK = PROFILE_CODE_OFFSET,
    PROFILE_BAD_FRAMES,
    PROFILE_BAD_SEED,
    PROFILE_BAD_FILTER_SPECIES,
    PROFILE_BAD_FILTER_MOVE,
    PROFILE_BAD_FILTER_ITEM,
    PROFILE_BAD_TID,
    PROFILE_BAD_SID,
    PROFILE_BAD_ASLR,
    PROFILE_BAD_LANGUAGE,
    PROFILE_BAD_VERSION,
    PROFILE_BAD_WILD
};

/* TODO: replace this by a macro? */
const u8 PROFILE_ErrorCodes[12][STRING_LENGTH_MAX] = {
    "OK",
    "FRAMES",
    "SEED",
    "FILTER_SPECIES",
    "FILTER_MOVE",
    "FILTER_ITEM",
    "TID",
    "SID",
    "ASLR",
    "LANGUAGE",
    "VERSION",
    "WILD"
};

//TODO: RESULTDATA should be in motor.h but it needs to be used by io.h
typedef struct {
    u32 seed;
    u32 pid;
    u32 ivs;
    u16 species;
    u16 moves[4]; //OWN_MOVES_MAX
    u16 item;
    u16 fate;
    u8 ability;
    u8 level;
    u8 pokerus;
    u8 friendship; //and egg cycle
    u8 catch;
    //TODO: add something useful in the last 2 bytes?
    //Size: 31 -> 32 bytes (aligned)
} RESULTDATA;

typedef struct {
    u32 frames;
    u32 seed;
    u16 filter_species;
    u16 filter_move;
    u16 filter_item;
    u16 tid;
    u16 sid;
    u8 aslr;
    u8 language;
    u8 version;
    u8 wild;
    u8 padding[10];
    //there's space for an IV filter (4 bytes)
    //Size: 32 bytes, padded to be the same size as RESULTDATA on purpose
} PROFILE;

/* Search parameters / profiles */
PROFILE PROFILE_Current; //used to launch searches
PROFILE PROFILE_Load; //used to load results
u8 ProfileSlotState[PROFILE_SLOTS_MAX] = { 0 }; //move to gui.h?

/* Functions */

static void SetString(u8* dest, u16 val, const u8 strarr[][STRING_LENGTH_MAX], u16 max, u8* zero, u8* format) {
    /* Set dest string accoring to val */
    //If val is greater than the length of array, format it and copy the new buffer string into dest string
    if (val >= max) {
        u8 buffer[8] = { 0 };
        sprintf(buffer, format, val);
        strcpy(dest, buffer);
        return;
    }
    //Else if val is 0, copy the zero string into dest string
    if (val == 0) {
        strcpy(dest, zero);
        return;
    }
    //Else, get the string at the index of val in array and copy it into dest string
    strcpy(dest, strarr[val]);
    return;
}

static BOOL IsValidIntString_hex(u8 b[U32_DIGITS_HEX_MAX]) {
    /* Valid characters are 0~9 and A~F */
    for (u8 i = 0; i < U32_DIGITS_HEX_MAX; i++) {
        if (b[i] < '0' || b[i] > 'F') { return FALSE; }
        if (b[i] > '9' && b[i] < 'A') { return FALSE; }
    }
    return TRUE;
}

static BOOL IsValidIntString_dec(u8* b, u8 size) {
    /* Valid characters are 0~9 */
    for (u8 i = 0; i < size; i++) {
        if (b[i] < '0' || b[i] > '9') { return FALSE; }
    }
    return TRUE;
}

static u32 AsciiToInt_dec16(u8* b, u8 digits) {
    /* Converts a buffer of 'digits' chars to an int */
    /* Assuming 'digits' not greater than 5 */
    u32 r = b[0] - 48;
    for (u8 i = 1; i < digits; i++) {
        r = r * 10 + b[i] - 48;
    }
    return r;
}

static u64 AsciiToInt_dec32(u8 b[U32_DIGITS_DEC_MAX]) {
    /* Converts a buffer of 10 chars to an int */
    u64 r = b[0];
    for (u8 i = 1; i < U32_DIGITS_DEC_MAX; i++) {
        r = r * 10 + b[i];
    }
    return r - 53333333328; //lemau
}

static u8 HexLetterToNumber(u8 n) {
    /* Virtually apends the capital letters right after 9 in the ASCII table */
    switch (n) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return n - 7;
    default:
        return n;
    }
}

static u32 AsciiToInt_hex32(u8 b[U32_DIGITS_HEX_MAX]) {
    /* Converts a buffer of 8 chars in hex format to an int */
    u32 r = HexLetterToNumber(b[0]);
    for (u8 i = 1; i < U32_DIGITS_HEX_MAX; i++) {
        r = r * 16 + HexLetterToNumber(b[i]);
    }
    return r - 858993456; //kek
}

static void ZeroLeftPadTextInputInt(u8* b, u8 size) {
    /* Insert '0' into the buffer from the left until there's no null terminator remaining */
    for (u8 i = 0; i < size; i++) {
        if (b[i] == 0) {
            for (u8 j = i; j > 0; j--) {
                b[j] = b[j - 1];
            }
            b[0] = '0';
        }
    }
}

static BOOL IsEmptyProfile(PROFILE* p) {
    /* Check if a PROFILE struct is full of 00 */
    u8* d = (u8*)p;
    for (u32 i = 0; i < sizeof(PROFILE); i++) {
        if (d[i]) { return FALSE; } //will return as soon as it finds a non-zero value
    }
    return TRUE;
}

static int GetIndexOfString(u8 str[STRING_LENGTH_MAX], const u8 arr[][STRING_LENGTH_MAX], u16 length) {
    /* Look for a specific string in an array of strings */
    for (int i = 0; i < length; i++) { //for each string
        int hits = 0;
        for (int j = 0; j < STRING_LENGTH_MAX; j++) { //for each char
            if (str[j] == arr[i][j]) { hits++; }
        }
        if (hits == STRING_LENGTH_MAX) { return i; } //found, return index
    }
    return -1; //not found
}