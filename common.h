//common.h
//Used by every file

#pragma once

#ifdef UNICODE
#undef UNICODE
#endif

#include <time.h>
#include <stdio.h>
#include <string.h>
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

#define MOTOR_VERSION               ("Motor v2.2.1")
#define SEARCH_THREADS_MAX          (8)
#define PROFILE_SLOTS_MAX           (8)
#define STRING_LENGTH_MAX           (16)
#define PROFILE_CODE_OFFSET         (100)
#define U16_VALUE_MAX               (0xFFFF)
#define U16_DIGITS_DEC_MAX          (5)
#define U32_VALUE_MAX               (0xFFFFFFFF)
#define U32_DIGITS_HEX_MAX          (8)
#define U32_DIGITS_DEC_MAX          (10)
#define U64_VALUE_MAX               (0xFFFFFFFFFFFFFFFF)
#define U64_DIGITS_HEX_MAX          (16)
#define ASLR_VALUE_MAX              (11)
#define ASLR_DIGITS_DEC_MAX         (2)
#define MAC_VALUE_MAX               (0xFFFFFFFFFFFF)
#define MAC_DIGITS_HEX_MAX          (12)
#define YEAR_DIGITS_DEC_MAX         (4)
#define YEAR_VALUE_MIN              (2000)
#define YEAR_VALUE_MAX              (2099) //2059 for N3DS
#define FILTER_DIGITS_MAX           (3)
#define PATH_ABS_LENGTH_MAX         (256)
#define PATH_REL_LENGTH_MAX         (4 * STRING_LENGTH_MAX)
#define LANGUAGES_MAX               (9)  // Number of languages: _0, Japanese, English, French, Italian, German, _6, Spanish and Korean
#define LANGUAGES_ACT_MAX           (7)  // Actual number of languages: Japanese, English, French, Italian, German, Spanish and Korean
#define VERSIONS_MAX                (3)  // Number of versions: Diamond, Pearl and Platinum
#define ASLR_GROUPS_MAX             (12) // Number of ASLR groups (mirrors)
#define OG_WILDS_MAX                (10) // OG_WILDS_MAX
#define STATS_MAX                   (6)  // Number of stats: Hit Points, Attack, Defense, Speed, Special Attack and Special Defense
#define IV_VALUE_MAX                (31)
#define IV_DIGITS_MAX               (2)
#define NATURES_MAX                 (25) // Number of natures
#define NATURES_FILTER_MAX          (NATURES_MAX + 1) // Number of natures + 1 (the "none" filter)
#define NATURE_FILTER_NONE          (0xFF) //when you don't want to filter for a Nature, this should be the value in the PROFILE struct

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
    PROFILE_BAD_WILD,
    PROFILE_BAD_MAC,
    PROFILE_BAD_FILTER_ABILITY,
    PROFILE_BAD_FILTER_NATURE,
    PROFILE_BAD_FILTER_IVS,
};

/* Indices of each language */
enum { LANGUAGE__0, LANGUAGE_JP, LANGUAGE_EN, LANGUAGE_FR, LANGUAGE_IT, LANGUAGE_GE, LANGUAGE__6, LANGUAGE_SP, LANGUAGE_KO };

/* Indices of each stat */
enum { HP, AT, DF, SP, SA, SD };

const u8 PROFILE_ErrorCodes[][STRING_LENGTH_MAX] = {
    "OK",
    "FRAMES",
    "SEED",
    "SPECIES_FILTER",
    "MOVE_FILTER",
    "ITEM_FILTER",
    "TID",
    "SID",
    "ASLR",
    "LANGUAGE",
    "VERSION",
    "WILD",
    "MAC ADDRESS",
    "ABILITY_FILTER",
    "NATURE_FILTER",
    "IVS_FILTER",
};

//RESULTDATA should be in motor.h but it needs to be used by io.h
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
    u32 frames; //number of frames to search
    u32 seed; //starting search seed
    u16 filter_species; //index of species to filter
    u16 filter_move; //index of move to filter
    u16 filter_item; //index of item to filter
    u16 tid; //original trainer id
    u16 sid; //original trainer secret id
    u8 aslr; //aslr index
    u8 language; //game language index
    u8 version; //game version index
    u8 wild; //original wild target to be corrupted
    u8 mac[6]; //mac address
    u8 filter_ability;
    u8 filter_nature;
    u8 filter_ivs[STATS_MAX];
    //Size: 36 bytes
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

static BOOL IsValidIntString_hex(u8* b, u32 size) {
    /* Valid characters are 0~9 and A~F */
    for (u8 i = 0; i < size; i++) {
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

static inline u32 AsciiToInt_dec16(const u8* b) {
    /* Converts a buffer of chars to a u32 */
    return strtoul(b, NULL, 10);
}

static inline u64 AsciiToInt_dec32(const u8* b) {
    /* Converts a buffer of chars to a u64 */
    return strtoull(b, NULL, 10);
}

static inline u32 AsciiToInt_hex32(const u8* b) {
    /* Converts a buffer of chars in hex format to a u32 */
    return strtoul(b, NULL, 16);
}

static inline u64 AsciiToInt_hex64(const u8* b) {
    /* Converts a buffer of chars in hex format to a u64 */
    return strtoull(b, NULL, 16);
}

static u32 BCD(u32 val) {
    /* Binary Coded Decimal for values under 100 */
    //better optimized when u32 instead of u8
    return val + 6 * (val / 10);
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
