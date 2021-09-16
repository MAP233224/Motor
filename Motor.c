/******************************/
/*       Motor.c by MAP       */
/******************************/

/* BUILD (WINDOWS 10) */
// >windres Motor.rc -O coff -o Motor.res< (to include the .ico)
// >gcc -O3 Motor.c -o Motor Motor.res< (optimized mode)

/* INCLUDE */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"


/* FUNCTIONS */

void ScanValue(u8 message[], u32* value, u8 format[], u64 max) {
    /* General purpose safe scan. Instruction message, value to change, string format and max value */
    do {
        printf("%s", message);
        u8 userInput[32];
        fgets(userInput, 16, stdin);
        if (strlen(userInput) == 0 || strlen(userInput) > 15) {
            continue;
        }
        if (sscanf(userInput, format, value) != 1) {
            *value = max + 1;
            continue;
        }
    } while (*value > max);
}

u8 GetNatureId(u32 pid) {
    /* Get the ID of the Nature (from 0 to 24), provided the PID. */
    return pid % 25;
}

u8 GetFormId(u8 form) {
    /* Get the form ID from the form byte */
    return form >> 3;
}

u8 BlockOrder(u32 pid) {
    /* Get the index of the block permutation of a given PID (from 0 to 23) */
    return ((pid & 0x3E000) >> 13) % BLOCK_PERM;
}

u8 PositionOfBlock(u8 perm, u8 block) {
    /* Return the positional index of a block (0, 1, 2 or 3) given the permutation index. */
    u8 i = 0;
    while (block != Orders[perm][i]) { i++; }
    return i;
}

u8 RngPosOfBlock(u8 pos) {
    /* Distance in RNG cycles from Checksum to the beginning of this Block (1, 17, 33 or 49) */
    return pos * BLOCK_SIZE + 1;
}

u16 StatNatureModifier(u8 nature, u8 stat_index, u16 stat_value) {
    /* Calculate and return the new value of a stat after the Nature modifier is applied.*/
    s8 m = NatureStatModifiers[nature][stat_index];
    return stat_value * (10 + m) / 10;
}

u16 IvToStat(Pkmn* pkmn, Original* wild, u8 stat) {
    /* Calculate the value of a stat based on the IV, Base Stat, Nature and Level. */
    u8 lv = wild->level;
    u16 val;
    if (stat == 0) { //hp
        val = (2 * (wild->bstats[stat]) + pkmn->ivs[stat]) * lv / 100 + lv + 10;
    }
    else { //others
        val = (2 * (wild->bstats[stat]) + pkmn->ivs[stat]) * lv / 100 + 5;
        val = StatNatureModifier(pkmn->nature, stat - 1, val); //ignore for hp (index 0), hence the -1
    }
    return val;
}

void SetCheckum(Pkmn* pkmn) {
    /* Return the checksum of a Pkmn by adding all of its block data into a 16-bit word. */
    u16 c = 0;
    for (u8 i = 0; i < BLOCK_SIZE; i++) {
        c += pkmn->data[pkmn->pos_a][i] + pkmn->data[pkmn->pos_b][i] + pkmn->data[pkmn->pos_c][i] + pkmn->data[pkmn->pos_d][i];
    }
    pkmn->checksum = c;
}

bool IsBadEgg(u16 badeggflag) {
    /* Check if the bad egg flag is set by looking at bit 2 of the "bad egg" 16-bit word. */
    return (bool)(badeggflag & 4);
}

bool IsEgg(u16 eggflag) {
    /* Check if the egg flag is set by looking at bit 30 of the "iv2" 16-bit word. */
    return (bool)(eggflag & 0x4000);
}

bool IsFatefulEncounter(u16 fateflag) {
    /* Check if the fateful encounter bit is set. */
    return (bool)(fateflag & 1);
}

bool SkippedCheckum(u16 badeggflag) {
    /* Check if the checksum was skipped by looking at bit 0 and 1 of the "bad egg" 16-bit word. */
    return (bool)((badeggflag & 1) && (badeggflag & 2));
}

bool IsShiny(u32 pid, u16 tid, u16 sid) {
    /* Check if a pkmn is shiny by xoring its pid (top and bottom 16 bits), tid and sid */
    return (bool)(((pid & 0xffff) ^ (pid >> 16) ^ tid ^ sid) < 8);
}

u32 RngNext(u32* state) {
    /* General purpose LCRNG, advance and return state */
    *state = *state * 0x41C64E6D + 0x6073;
    return *state;
}

u32 Rng_32(u32 state, u16 iter) {
    /* General purpose LCRNG, specify number of advances, return new state */
    for (u16 i = 0; i < iter; i++) {
        state = state * 0x41C64E6D + 0x6073;
    }
    return state;
}

void SetBlocks(Pkmn* pkmn) {
    /* Get the order of each block from the PID and set them in the correct permutation */
    pkmn->order = BlockOrder(pkmn->pid);
    pkmn->pos_a = PositionOfBlock(pkmn->order, 0);
    pkmn->pos_b = PositionOfBlock(pkmn->order, 1);
    pkmn->pos_c = PositionOfBlock(pkmn->order, 2);
    pkmn->pos_d = PositionOfBlock(pkmn->order, 3);
}

void Encrypt(Pkmn* pkmn) {
    /* Encrypt with XOR and LCRNG each 16-bit word of Pkmn data. */
    /* First with the Checksum as the Seed/Key (for block data), then with the PID (for condition data). */
    //TODO: rewrite this function without the need for Rng_32()
    u32 pkmn_cond_state = pkmn->pid; //seeded with pid
    u32 pkmn_a_state = Rng_32(pkmn->checksum, RngPosOfBlock(pkmn->pos_a)); //seeded with LCRNG
    u32 pkmn_b_state = Rng_32(pkmn->checksum, RngPosOfBlock(pkmn->pos_b));
    u32 pkmn_c_state = Rng_32(pkmn->checksum, RngPosOfBlock(pkmn->pos_c));
    u32 pkmn_d_state = Rng_32(pkmn->checksum, RngPosOfBlock(pkmn->pos_d));
    for (u8 i = 0; i < BLOCK_SIZE; i++) {
        pkmn->data[pkmn->pos_a][i] ^= (pkmn_a_state >> 16);
        pkmn->data[pkmn->pos_b][i] ^= (pkmn_b_state >> 16);
        pkmn->data[pkmn->pos_c][i] ^= (pkmn_c_state >> 16);
        pkmn->data[pkmn->pos_d][i] ^= (pkmn_d_state >> 16);
        RngNext(&pkmn_a_state);
        RngNext(&pkmn_b_state);
        RngNext(&pkmn_c_state);
        RngNext(&pkmn_d_state);
    }
    for (u8 i = 0; i < COND_SIZE; i++) {
        RngNext(&pkmn_cond_state);
        pkmn->cond[i] ^= (pkmn_cond_state >> 16);
    }
}

void GetIVs(Pkmn* pkmn) {
    /* Decompose IVs */
    pkmn->ivs[hp] = pkmn->iv1 & 31;
    pkmn->ivs[at] = (pkmn->iv1 >> 5) & 31;
    pkmn->ivs[df] = (pkmn->iv1 >> 10) & 31;
    pkmn->ivs[sp] = pkmn->iv2 & 31;
    pkmn->ivs[sa] = (pkmn->iv2 >> 5) & 31;
    pkmn->ivs[sd] = (pkmn->iv2 >> 10) & 31;
}

void MethodJSeedToPID(u32 seed, Pkmn* pkmn) {
    /* Calculate PID, Nature and IVs according to Method J Stationary (no Synchronize) from a given seed */
    u32 state = seed;
    pkmn->nature = (RngNext(&state)>>16) / 0x0A3E;
    do { pkmn->pid = (RngNext(&state) >> 16) | (RngNext(&state) & 0xffff0000); } while (pkmn->pid % 25 != pkmn->nature); //roll PID until the 2 natures are the same
    pkmn->iv1 = RngNext(&state) >> 16;
    pkmn->iv2 = RngNext(&state) >> 16;
    GetIVs(pkmn);
    u32 ivsum = (pkmn->ivs[hp] << 0) | (pkmn->ivs[at] << 5) | (pkmn->ivs[df] << 10) | (pkmn->ivs[sp] << 15) | (pkmn->ivs[sa] << 20) | (pkmn->ivs[sd] << 25);
    pkmn->iv1 = ivsum & 0xffff;
    pkmn->iv2 = ivsum >> 16;
}

void Method1SeedToPID(u32 seed, Pkmn* pkmn) {
    /* Calculate PID, Nature and IVs according to Method 1 from a given seed � UNUSED */
    u32 state = seed;
    pkmn->pid = (RngNext(&state) >> 16) | (RngNext(&state) & 0xffff0000);
    pkmn->nature = pkmn->pid % 25;
    pkmn->iv1 = RngNext(&state) >> 16;
    pkmn->iv2 = RngNext(&state) >> 16;
    GetIVs(pkmn);
    u32 ivsum = (pkmn->ivs[hp] << 0) | (pkmn->ivs[at] << 5) | (pkmn->ivs[df] << 10) | (pkmn->ivs[sp] << 15) | (pkmn->ivs[sa] << 20) | (pkmn->ivs[sd] << 25);
    pkmn->iv1 = ivsum & 0xffff;
    pkmn->iv2 = ivsum >> 16;
}

/* MAIN */

int main()
{

    User user = { 0 }; //0 init

    u32 use_saved;
    ScanValue("Use saved profile (0=no, 1=yes): ", &use_saved, "%u", 1);

    if (use_saved) { //use saved user profile from Profile.txt
        FILE* profile;
        profile = fopen("Profile.txt", "r"); //read only
        fscanf(profile, "%u", &user.version);
        fscanf(profile, "%u", &user.language);
        fscanf(profile, "%u", &user.tid);
        fscanf(profile, "%u", &user.sid);
        fclose(profile);
    }
    else { //console scan for version, language, tid and sid
        ScanValue("Enter your Version (0=Diamond, 1=Pearl, 2=Platinum): ", &user.version, "%u", 2);
        ScanValue("Enter your Language (1=jp, 2=en, 3=fr, 4=it, 5=ge, 7=sp, 8=ko): ", &user.language, "%u", 8);
        ScanValue("Enter your TID (0 to 65535): ", &user.tid, "%u", 0xffff);
        ScanValue("Enter your SID (0 to 65535): ", &user.sid, "%u", 0xffff);
        u32 save_user;
        ScanValue("Save those user settings? (0=no, 1=yes) ", &save_user, "%u", 1);
        if (save_user) { //Erases previous and saves new user profile
            FILE* save_profile;
            save_profile = fopen("Profile.txt", "w+");
            fprintf(save_profile, "%u\n", user.version);
            fprintf(save_profile, "%u\n", user.language);
            fprintf(save_profile, "%u\n", user.tid);
            fprintf(save_profile, "%u\n", user.sid);
            fclose(save_profile);
        }
    }

    /* PICK THE ORIGINAL WILD HERE (depending on version, language, pkmn) */
    u32 og;
    Original ogwild;
    if (user.version == 2) { //platinum
        ScanValue("Static PKMN you want to corrupt (0=Giratina, 1=Uxie, 2=Azelf, 3=Rotom): ", &og, "%u", 3);
        switch (og) {
            case 0: ogwild = pt_giratina; break;
            case 1: ogwild = pt_uxie; break;
            case 2: ogwild = pt_azelf; break;
            case 3: ogwild = pt_rotom; break;
            default: ogwild = pt_rotom;
        }
    }
    else { //dp
        ScanValue("Static PKMN you want to corrupt (0=Giratina, 1=Arceus, 2=Shaymin, 3=Darkrai, 4=Uxie, 5=Azelf, 6=Rotom): ", &og, "%u", 6);
        if (og > 3) {
            if (user.language == 3) { og += 7; } //french
            else if (user.language == 5) { og += 14; } //german
        }
        switch (og) {
            case 0: ogwild = dp_giratina; break;
            case 1: ogwild = dp_arceus; break;
            case 2: ogwild = dp_shaymin; break;
            case 3: ogwild = dp_darkrai; break;
            case 4: ogwild = dp_uxie; break;
            case 5: ogwild = dp_azelf; break;
            case 6: ogwild = dp_rotom; break;
            case 11: ogwild = dp_crehelf; break;
            case 12: ogwild = dp_crefadet; break;
            case 13: ogwild = dp_motisma; break;
            case 18: ogwild = dp_selfe; break;
            case 19: ogwild = dp_tobutz; break;
            case 20: ogwild = dp_rotom; break;
            default: ogwild = dp_giratina;
        }
    }

    ScanValue("Search for a species (0=no, species_id=yes): ", &user.species, "%u", SPECIES + 1);
    ScanValue("Search for an item (0=no, item_id=yes): ", &user.item, "%u", ITEMS + 1);
    ScanValue("Search for a move (0=no, move_id=yes): ", &user.move, "%u", 0xffff);
    ScanValue("Enter your Seed (32 bit, hex): 0x", &user.seed, "%x", 0xffffffff);
    ScanValue("How many frames to search through (32 bit, dec): ", &user.frames, "%u", 0xffffffff);
    ScanValue("Allow more seed options per result? (0=no, 1=yes): ", &user.dupe, "%u", 1);

    u8* strlang = Languages[user.language];
    u8* strvers = Versions[user.version];
    u8* strspec = Pokelist[user.species];
    u8* stritem = Items[user.item];
    u8* strmove;
    if (user.move == 0) { strmove = "anything"; }
    else if (user.move > MOVES) {
        u8 buffer[8];
        sprintf(buffer, "0x%04X", user.move);
        strmove = buffer;
    }
    else { strmove = Moves[user.move]; }

    u16 w_version = (user.version + 10) << 8; //convert for use in pkmn data
    u16 w_language = user.language << 8; //convert for use in pkmn data
    user.aslr = Aslrs[user.language][user.version>>1]; //depends on language and version. Right shift version by 1 because DP share the same value.

    FILE* fp; //declare file object
    u8* strfilename = "Results.txt"; //name of the file
    fp = fopen(strfilename, "w+"); //open/create file

    fprintf(fp, "> %s (%s)\n", strvers, strlang);
    fprintf(fp, "> TID = %u\n> SID = %u\n", user.tid, user.sid);
    fprintf(fp, "> Seed 0x%08X\n", user.seed);
    fprintf(fp, "> ASLR 0x%08X\n", user.aslr);
    fprintf(fp, "> Searched through %u frames for %s holding %s knowing %s\n\n", user.frames, strspec, stritem, strmove);
    fprintf(fp, "Seed       | PID        | Level   | Species      | Form | Item           | Ability          | Hatch steps | Fateful | Shiny | IVs               | Moves\n");
    fprintf(fp, "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    printf("\n> %s (%s)\n", strvers, strlang);
    printf("> TID = %u\n> SID = %u\n", user.tid, user.sid);
    printf("> Seed 0x%08X\n", user.seed);
    printf("> ASLR 0x%08X\n", user.aslr);
    printf("> Searching through %u frames for %s holding %s knowing %s...\n\n", user.frames, strspec, stritem, strmove);
    printf("Seed       | PID        | Level   | Species      | Form | Item           | Ability          | Hatch steps | Fateful | Shiny | IVs               | Moves\n");
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    u32 pid_list[PIDS_MAX] = {0}; //0 init
    u32 results = 0; //0 init
    u32 seed = user.seed; //copy to advance in the main loop

    clock_t begin = clock(); //timer starts

    /* Main search loop */
    for (u32 frame = 0; frame < user.frames; frame++) {

        if (frame != 0) { RngNext(&seed); } //advance the RNG everytime, except on the 0th frame

        Pkmn wild = { 0 }; //0 init
        Pkmn seven = { 0 }; //0 init

        MethodJSeedToPID(seed, &wild);

        /* Checking for duplicate PIDs if user specified it */
        if (user.dupe == 0) {
            bool duplicate = false;
            for (u16 i = 0; i < PIDS_MAX; i++) {
                if (pid_list[i] == wild.pid) {
                    duplicate = true;
                    break;
                }
                else if (pid_list[i] == 0) {
                    pid_list[i] = wild.pid; //insert the new pid
                    break;
                }
            }
            if (frame % PIDS_MAX == PIDS_MAX - 1) { memset(pid_list, 0, sizeof(pid_list)); } //if filled, zero it out
            if (duplicate) { continue; }
        }

        SetBlocks(&wild);

        wild.data[wild.pos_a][0] = ogwild.species; //species
        wild.data[wild.pos_a][1] = ogwild.item; //held item
        wild.data[wild.pos_a][2] = user.tid; //tid
        wild.data[wild.pos_a][3] = user.sid; //sid
        wild.data[wild.pos_a][4] = ogwild.xp1; //xp1
        wild.data[wild.pos_a][5] = ogwild.xp2; //xp2
        wild.data[wild.pos_a][6] = ogwild.frab; //ability and friendship concatenated
        wild.data[wild.pos_a][7] = w_language; //language

        for (u8 i = 0; i < MOVES_MAX; i++) wild.data[wild.pos_b][i] = ogwild.moves[i]; //4 moves
        wild.data[wild.pos_b][4] = ogwild.pp1and2; //pp1and2
        wild.data[wild.pos_b][5] = ogwild.pp3and4; //pp3and4
        wild.data[wild.pos_b][8] = wild.iv1;
        wild.data[wild.pos_b][9] = wild.iv2;
        wild.data[wild.pos_b][12] = 0x0004; //genderless

        for (u8 i = 0; i < 11; i++) wild.data[wild.pos_c][i] = ogwild.name[i]; //11 characters for the name
        wild.data[wild.pos_c][11] = w_version; //version

        wild.data[wild.pos_d][13] = 0x0400; //pokeball
        wild.data[wild.pos_d][14] = ogwild.level; //level

        wild.cond[2] = ogwild.level; //level again
        wild.cond[3] = IvToStat(&wild, &ogwild, hp);
        wild.cond[4] = wild.cond[3]; //current hp = max hp
        wild.cond[5] = IvToStat(&wild, &ogwild, at);
        wild.cond[6] = IvToStat(&wild, &ogwild, df);
        wild.cond[7] = IvToStat(&wild, &ogwild, sp);
        wild.cond[8] = IvToStat(&wild, &ogwild, sa);
        wild.cond[9] = IvToStat(&wild, &ogwild, sd);

        wild.cond[12] = w_language; //language again
        wild.cond[13] = 0xff00 | (w_version >> 8); //version variation
        wild.cond[14] = 0xffff;
        wild.cond[15] = 0xffff;
        wild.cond[16] = 0xffff;
        wild.cond[17] = 0xffff;
        wild.cond[18] = 0xffff;
        wild.cond[19] = 0xffff;
        wild.cond[20] = 0xffff;
        wild.cond[21] = 0xffff;
        wild.cond[22] = 0xffff;
        wild.cond[23] = 0xffff;
        wild.cond[24] = 0xffff;
        // wild.cond[25] = 0;
        wild.cond[26] = 0xffff;
        // wild.cond[27] = 0;
        wild.cond[28] = 0xffff;
        wild.cond[29] = 0xffff;

        SetCheckum(&wild);
        Encrypt(&wild);

        /* Initialize Seven */
        seven.pid = 0x00005544;
        seven.bef = 0x05a4; //after checksum check, changed to a Bad Egg (bit 4: 0->1)
        SetBlocks(&seven);

        /* Simulating the stack overflow */
        for (u8 i = 0; i < BLOCK_SIZE - RS_OFF; i++) { //ABCD blocks part 1
            for (u8 j = 1; j < BLOCKS; j++) { //only need to start from j=1 bc block A is taken care of later.
                seven.data[j][i + RS_OFF] = wild.data[j - 1][i]; //warning: negative index
            }
        }
        for (u8 i = 0; i < BLOCKS; i++) { //ABCD blocks part 2
            for (u8 j = 0; j < BLOCKS - 2; j++) {
                seven.data[j + 2][i] = wild.data[j][BLOCK_SIZE - RS_OFF + i];
            }
        }
        for (u8 i = 0; i < COND_SIZE; i++) { //condition data
            if (i < RS_OFF) { seven.cond[i] = wild.data[2][BLOCK_SIZE - RS_OFF + i]; }
            else if (i < RS_OFF + BLOCK_SIZE) { seven.cond[i] = wild.data[3][i - RS_OFF]; }
            else { seven.cond[i] = wild.cond[i - RS_OFF - BLOCK_SIZE]; }
        }

        if (user.version == 2) { //platinum
            seven.data[seven.pos_a][0] = (user.aslr + LOC_BEG_OPP_PARTY_PL) & 0xffff;
            seven.data[seven.pos_a][1] = (user.aslr + LOC_BEG_OPP_PARTY_PL) >> 16;
            seven.data[seven.pos_a][2] = (user.aslr + LOC_END_OPP_PARTY_PL) & 0xffff;
            seven.data[seven.pos_a][3] = (user.aslr + LOC_END_OPP_PARTY_PL) >> 16;
            seven.data[seven.pos_a][4] = 0;
            seven.data[seven.pos_a][5] = 0x0005;
            seven.data[seven.pos_a][6] = 0xe000;
            seven.data[seven.pos_a][7] = 0xfa00;
            seven.data[seven.pos_a][8] = 0xfc00;
            seven.data[seven.pos_a][9] = 0x4000;
            seven.data[seven.pos_a][10] = 0x3a05;
            seven.data[seven.pos_a][11] = 0x0800;
            seven.data[seven.pos_a][12] = 0x0006;
            seven.data[seven.pos_a][13] = 0;
            seven.data[seven.pos_a][14] = 0x0001;
            seven.data[seven.pos_a][15] = 0;
        }
        else { //dp
            seven.data[seven.pos_a][0] = (user.aslr + LOC_BEG_OPP_PARTY_DP) & 0xffff;
            seven.data[seven.pos_a][1] = (user.aslr + LOC_BEG_OPP_PARTY_DP) >> 16;
            seven.data[seven.pos_a][2] = (user.aslr + LOC_END_OPP_PARTY_DP) & 0xffff;
            seven.data[seven.pos_a][3] = (user.aslr + LOC_END_OPP_PARTY_DP) >> 16;
            seven.data[seven.pos_a][4] = 0x3377;
            seven.data[seven.pos_a][5] = 0x1463;
            seven.data[seven.pos_a][6] = 0x9631;
            seven.data[seven.pos_a][7] = 0x7779;
            seven.data[seven.pos_a][8] = 0x3377;
            seven.data[seven.pos_a][9] = 0x1463;
            seven.data[seven.pos_a][10] = 0x7605;
            seven.data[seven.pos_a][11] = 0x7777;
            seven.data[seven.pos_a][12] = 0x0006;
            seven.data[seven.pos_a][13] = 0;
            seven.data[seven.pos_a][14] = 0x0001;
            seven.data[seven.pos_a][15] = 0;
        }

        seven.data[seven.pos_c][0] = wild.pid % 65536;
        seven.data[seven.pos_c][1] = wild.pid >> 16;
        seven.data[seven.pos_c][2] = wild.bef;
        seven.data[seven.pos_c][3] = wild.checksum;

        Encrypt(&seven);

        /* If the ball doesn't have a valid id the battle won't load */
        u8 ballid = seven.data[seven.pos_d][13] >> 8;
        if ((ballid > 16) || (ballid == 0)) { continue; } //might be more complex than that, some invalid Ball IDs load fine

        SetBlocks(&seven);
        SetCheckum(&seven);
        Encrypt(&seven);

        /* If it's a Bad Egg or Checksum was not skipped, continue */
        wild.bef = seven.data[seven.pos_c][2];
        if (IsBadEgg(wild.bef)) { continue; }
        if (!SkippedCheckum(wild.bef)) { continue; }

        wild.pid = seven.data[seven.pos_c][0] | (seven.data[seven.pos_c][1] << 16); //don't actually need the top part I think
        SetBlocks(&wild);

        /* Get final species, item, ability and steps to hatch */
        u8 f_ability;
        u16 f_species, f_item, f_steps;
        if (wild.pos_a == 3) {
            f_species = seven.cond[RS_OFF];
            f_item = seven.cond[RS_OFF + 1];
            f_ability = seven.cond[RS_OFF + 6] >> 8;
            f_steps = seven.cond[RS_OFF + 6] & 0xff;
        }
        else {
            f_species = seven.data[wild.pos_a + 1][RS_OFF];
            f_item = seven.data[wild.pos_a + 1][RS_OFF + 1];
            f_ability = seven.data[wild.pos_a + 1][RS_OFF + 6] >> 8;
            f_steps = seven.data[wild.pos_a + 1][RS_OFF + 6] & 0xff;
        }

        /* Species check */
        if (user.species == 0) { //user didn't specify a species
            if (f_species >= SPECIES) { continue; } //any valid species
        }
        else if (f_species != user.species) { continue; } //match user.species

        /* Filter for specific item */
        if (user.item != 0 && f_item != user.item) { continue; }

        /* Get final moveset, egg steps, form id and fateful encounter flag */
        u16 fate;
        u16 moves[MOVES_MAX];

        if (wild.pos_b > 1) { //pos_b is either 2 or 3
            for (u8 i = 0; i < MOVES_MAX; i++) { moves[i] = seven.cond[RS_OFF + i]; }
            fate = seven.cond[16 * (wild.pos_b == 3)]; //index of 0 or 16 depending on if pos_b is 2 or 3
            wild.iv1 = seven.cond[RS_OFF + 8];
            wild.iv2 = seven.cond[RS_OFF + 9];
        }
        else { //pos_b is either 0 or 1
            for (u8 i = 0; i < MOVES_MAX; i++) { moves[i] = seven.data[wild.pos_b + 1][RS_OFF + i]; }
            fate = seven.data[wild.pos_b + 2][0];
            wild.iv1 = seven.data[wild.pos_b + 1][RS_OFF + 8];
            wild.iv2 = seven.data[wild.pos_b + 1][RS_OFF + 9];
        }

        GetIVs(&wild);

        /* Calculate steps to hatch if Egg, zero if not an Egg */
        f_steps = IsEgg(wild.iv2) * (f_steps + 1) * 255;

        /* Filter for a specific move */
        if (user.move != 0) {
            if ((moves[0] != user.move) && (moves[1] != user.move) && (moves[2] != user.move) && (moves[3] != user.move)) { continue; }
        }

        /* Get final level */
        u8 f_level = seven.cond[22] & 0xff;

        /* Get form id */
        u8 form = GetFormId((u8)fate);

        /* Fateful encounter string definition */
        u8* fateful;
        if (IsFatefulEncounter(fate)) { fateful = "Fateful"; }
        else { fateful = "-------"; }

        /* Species string format */
        u8* str_f_species;
        if (f_species >= SPECIES) { str_f_species = "Glitchmon"; } //invalid index after Arceus
        else if (f_species == 0) { str_f_species = "DPBox"; } //the empty Pok�mon
        else { str_f_species = Pokelist[f_species]; } //get Pok�mon name from index

        /* Item string format */
        u8* str_f_item;
        if (f_item >= ITEMS) { str_f_item = "Glitch Item"; } //invalid index
        else if (f_item == 0) { str_f_item = "None"; } //no item if index is 0
        else { str_f_item = Items[f_item]; } //get item name from index

        /* Ability string format */
        u8* str_f_abi;
        if (f_ability >= ABILITIES) { str_f_abi = "Glitch Ability"; } //invalid index
        else if (f_ability == 0) { str_f_abi = "None"; } //no ability if index is 0
        else { str_f_abi = Abilities[f_ability]; } //get ability name from index

        /* Moves string format */
        u8 strmoves[MOVES_MAX][16] = {0};
        for (u8 i = 0; i < MOVES_MAX; i++) {
            if (moves[i] >= MOVES) {
                u8 buffer[16];
                sprintf(buffer, "0x%04X", moves[i]);
                strcpy(strmoves[i], buffer);
            }
            else {
                u8* buffer = Moves[moves[i]];
                strcpy(strmoves[i], buffer);
            }
        }

        /* Get shinyness */
        u8* shiny;
        if (IsShiny(wild.pid, user.tid, user.sid)) { shiny = "Shiny"; }
        else { shiny = "-----"; }

        /* Print successful result to file */
        fprintf(fp, "0x%08X | 0x%08X | Lv. %-3d | %-12s | %-4d | %-14s | %-16s | %-5d steps | %s | %s | ", seed, wild.pid, f_level, str_f_species, form, str_f_item, str_f_abi, f_steps, fateful, shiny);
        fprintf(fp, "%02d/%02d/%02d/%02d/%02d/%02d | ", wild.ivs[hp], wild.ivs[at], wild.ivs[df], wild.ivs[sa], wild.ivs[sd], wild.ivs[sp]);
        fprintf(fp, "%s, %s, %s, %s\n", strmoves[0], strmoves[1], strmoves[2], strmoves[3]);
        /* And to console */
        printf("0x%08X | 0x%08X | Lv. %-3d | %-12s | %-4d | %-14s | %-16s | %-5d steps | %s | %s | ", seed, wild.pid, f_level, str_f_species, form, str_f_item, str_f_abi, f_steps, fateful, shiny);
        printf("%02d/%02d/%02d/%02d/%02d/%02d | ", wild.ivs[hp], wild.ivs[at], wild.ivs[df], wild.ivs[sa], wild.ivs[sd], wild.ivs[sp]);
        printf("%s, %s, %s, %s\n", strmoves[0], strmoves[1], strmoves[2], strmoves[3]);

        results++;
    }

    /* End of main loop */

    clock_t end = clock(); //end timer
    double time_spent = ((double)end - (double)begin) / CLOCKS_PER_SEC; //calculate time elapsed since start of search
    fprintf(fp, "\nFound %u results in %.1f seconds.\n", results, time_spent);
    printf("\nFound %u results in %.1f seconds.\n", results, time_spent);
    fclose(fp); //close file
    u8 exit;
    scanf("%s", &exit); //scan to halt execution
    return 0;
}
