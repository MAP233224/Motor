// io.h
// Contains functions relative to file input/output

#pragma once
#include "common.h"

#define PROFILE_FILE_SIZE (PROFILE_SLOTS_MAX * sizeof(PROFILE))

/* Globals */

const u8 ResultsDirectory[] = ".results";
const u8 ProfilesDirectory[] = ".profiles";
const u8 ResultFileExtension[] = ".mtr"; //MoTorResult
const u8 ProfilesPath[] = ".profiles/PROFILES";
const u8 ResultsFileFilter[] = ".results/RESULTS_*.mtr";

/* Functions */

static int GetFileSize_mine(FILE* fp) {
    /* Return the size of an opened file */
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    return size;
}

static void InitProfilesFile(void) {
    /* Create the PROFILES file and set all its data to zero */
    FILE* fp = fopen(ProfilesPath, "wb+");
    u8 buffer[PROFILE_FILE_SIZE] = { 0 };
    fwrite(buffer, sizeof(buffer), 1, fp);
    fclose(fp);
}

static APPSTATUS CheckProfileFileSize(void) {
    /* Should always be PROFILE_SLOTS_MAX * sizeof(PROFILE) */
    FILE* fp = fopen(ProfilesPath, "rb");
    if (fp == NULL) { InitProfilesFile(); }
    else if (GetFileSize_mine(fp) != PROFILE_FILE_SIZE) {
        DLOG("The PROFILES file was not of the correct size.");
        fclose(fp);
        InitProfilesFile();
    }
    else {
        fclose(fp);
    }
    return APP_RESUME;
}

static APPSTATUS SaveProfileToSlot(u8 slot) {
    /**/
    FILE* fp = fopen(ProfilesPath, "rb+");
    if (fp == NULL) { return APP_ERR_OPEN_FILE; } //couldn't find file
    if (slot >= PROFILE_SLOTS_MAX) { return APP_ERR_PROFILE_SLOT; } //oob
    /* All good, save to slot */
    fseek(fp, slot * sizeof(PROFILE), SEEK_SET);
    fwrite(&PROFILE_Current, sizeof(PROFILE), 1, fp);
    fclose(fp);
    return APP_RESUME;
}

static int GetResultsCount(FILE* fp, BOOL hasProfileHeader) {
    /* Get the size of the file and divide it by the size of a RESULTDATA struct to get the number of results in the file */
    int count = GetFileSize_mine(fp);
    if (hasProfileHeader) { count -= sizeof(PROFILE); }
    count /= sizeof(RESULTDATA);
    return count;
}

static void GetProfileFromResultsFile(PROFILE* p, FILE* fp) {
    /* Only call when file is open at address 0 */
    fread(p, sizeof(PROFILE), 1, fp);
}