// main.c

#include "motor.h"

PKMN gWild = { 0 };
PKMN gSeven = { 0 };

static void Motor_Search_Loop(FILE* file)
{
    for (u64 seed = 0; seed <= 0xffffffff; seed++)
    {
        /* Init Wild */
        PKMN wild = { 0 };

        MethodJSeedToPID(seed, &wild); // todo: precompute? table of states just before PID generation
        SetBlocks(&wild);

        memcpy(wild.data[wild.pos_a], &gWild.data[0], BLOCK_SIZE * sizeof(wild.data[0][0]));
        memcpy(wild.data[wild.pos_b], &gWild.data[1], BLOCK_SIZE * sizeof(wild.data[0][0]));
        memcpy(wild.data[wild.pos_c], &gWild.data[2], BLOCK_SIZE * sizeof(wild.data[0][0]));
        memcpy(wild.data[wild.pos_d], &gWild.data[3], BLOCK_SIZE * sizeof(wild.data[0][0]));
        memcpy(&wild.data[wild.pos_b][8], &wild.iv32, sizeof(wild.iv32));
        wild.data[wild.pos_b][12] = 0x0004; // genderless
        wild.cond[2] = 0x0014; // level 20

        SetChecksum(&wild); // note: only variables at this point are PID and IVs
        EncryptBlocks(&wild);

        /* If the 1st move of Seven is invalid, the game will crash right before showing the battle menu (check with known encryption mask) */
        if ((wild.data[0][12] ^ 0xEA8D) > VALID_MOVES_MAX) { continue; }
        /* If the ball of Seven is invalid, the battle won't load (check with known encryption mask) */
        if (((wild.data[2][9] >> 8) ^ 0x70) > BALL_ID_MAX) { continue; }

        EncryptCondition(&wild); //only encrypts up to cond[COND_SIZE_XS]

        /* Init Seven */
        PKMN seven = { 0 }; //always ACBD (0x00020103)
        /* Block order up to Block A */
        memcpy(&seven, &gSeven, sizeof(seven) - sizeof(seven.cond) - 3 * sizeof(seven.data[0]));
        /* Simulate the buffer overflow */
        /* Block C, B, D and Condition data */
        memcpy(&seven.data[SEVEN_BLOCK_C], &wild.pid, 2 * (BLOCKS * BLOCK_SIZE + STACK_OFFSET + COND_SIZE_XS));

        EncryptBlocksChecksumZero(&seven);
        SetChecksumFastSeven(&seven);
        EncryptBlocks(&seven);

        /* If heap ID of Opponent 1 Party is valid, the game will crash when returning to the overworld */
        u8 heapid = seven.data[SEVEN_BLOCK_A][10] & 0xff;
        if (heapid < HEAPID_MAX) { continue; }
        /* If partycount of Opponent 1 Party is invalid, the game will crash right before showing the battle menu */
        s32 partycount = seven.data[SEVEN_BLOCK_A][14] | (seven.data[SEVEN_BLOCK_A][15] << 16);
        if (partycount > 54) { continue; }
        /* If the Bad Egg flag is set or the Fast Mode flags aren't set, the PKMN will become a Bad Egg */
        if ((seven.data[SEVEN_BLOCK_C][2] & 7) != 3) { continue; }

        /* Get the new PID of the wild and deduce its new block order */
        wild.pid = seven.data[SEVEN_BLOCK_C][0] | (seven.data[SEVEN_BLOCK_C][1] << 16);

        SetBlocks(&wild);

        /* Egg */
        if (seven.data[1 + wild.pos_b][STACK_OFFSET + 9] & 0x4000) { continue; }
        /* Species */
        if (seven.data[1 + wild.pos_a][STACK_OFFSET] >= SPECIES_MAX) { continue; }
        /* Moves */
        u32 valid_moves = 0;
        //u32 valid_moves = 1; // debug
        u16 move0 = seven.data[1 + wild.pos_b][STACK_OFFSET + 0];
        u16 move1 = seven.data[1 + wild.pos_b][STACK_OFFSET + 1];
        u16 move2 = seven.data[1 + wild.pos_b][STACK_OFFSET + 2];
        u16 move3 = seven.data[1 + wild.pos_b][STACK_OFFSET + 3];
        if (move0 <= VALID_MOVES_MAX) { valid_moves++; }
        if (move1 <= VALID_MOVES_MAX) { valid_moves++; }
        if (move2 <= VALID_MOVES_MAX) { valid_moves++; }
        if (move3 <= VALID_MOVES_MAX) { valid_moves++; }

        /* Print successful result */
        if (valid_moves)
        {
            printf("0x%08x,%u,0x%04x,0x%04x,0x%04x,0x%04x\n", (u32)seed, valid_moves, move0, move1, move2, move3);
            fprintf(file, "0x%08x,%u,0x%04x,0x%04x,0x%04x,0x%04x\n", (u32)seed, valid_moves, move0, move1, move2, move3);
        }
    }
}

static void Motor_InitPkmn(u32 tidsid, u32 aslr)
{
    /* Initialize the global Wild and Seven */

    /* Wild */
    memset(&gWild, 0, sizeof(gWild)); //zero init
    /* Block A */
    gWild.data[0][0] = 0x01DF; // species
    gWild.data[0][1] = 0x0000; // item
    gWild.data[0][2] = tidsid & 0xffff; // tid
    gWild.data[0][3] = tidsid >> 16; // sid
    gWild.data[0][4] = 0x1F40; // xp low word
    gWild.data[0][5] = 0x0000; // xp high word
    gWild.data[0][6] = 0x1A46; // ability and friendship concatenated
    gWild.data[0][7] = 0x0200; // language: english
    /* Block B */
    gWild.data[1][0] = 0x0054; // move 1
    gWild.data[1][1] = 0x006D; // move 2
    gWild.data[1][2] = 0x00FD; // move 3
    gWild.data[1][3] = 0x0068; // move 4
    gWild.data[1][4] = 0x0A1E; // pp move 1 and 2
    gWild.data[1][5] = 0x0F0A; // pp move 3 and 4
    /* Block C */
    gWild.data[2][0] = 0x013C; // R
    gWild.data[2][1] = 0x0139; // O
    gWild.data[2][2] = 0x013E; // T
    gWild.data[2][3] = 0x0139; // O
    gWild.data[2][4] = 0x0137; // M
    gWild.data[2][5] = 0xFFFF; // terminator
    gWild.data[2][11] = 0x0c00; // version: platinum
    /* Block D */
    gWild.data[3][13] = 0x0400; // pokeball
    gWild.data[3][14] = 0x0014; // level

    /* Seven */
    memset(&gSeven, 0, sizeof(gSeven)); //zero init
    gSeven.pid = 0x00005544;
    gSeven.bef = 0x05a4; //after checksum check, changed to a Bad Egg (bit 4: 0->1)
    SetBlocks(&gSeven); //always ACBD (0x00020103)
    gSeven.data[SEVEN_BLOCK_A][0] = (aslr + OPP_PARTY_OFFSET_START) & 0xffff;
    gSeven.data[SEVEN_BLOCK_A][1] = (aslr + OPP_PARTY_OFFSET_START) >> 16;
    gSeven.data[SEVEN_BLOCK_A][2] = (aslr + OPP_PARTY_OFFSET_END) & 0xffff;
    gSeven.data[SEVEN_BLOCK_A][3] = (aslr + OPP_PARTY_OFFSET_END) >> 16;
    gSeven.data[SEVEN_BLOCK_A][4] = 0x0000;
    gSeven.data[SEVEN_BLOCK_A][5] = 0x0005;
    gSeven.data[SEVEN_BLOCK_A][6] = 0xE000;
    gSeven.data[SEVEN_BLOCK_A][7] = 0xFA00;
    gSeven.data[SEVEN_BLOCK_A][8] = 0xFC00;
    gSeven.data[SEVEN_BLOCK_A][9] = 0x4000;
    gSeven.data[SEVEN_BLOCK_A][10] = 0x3A05;
    gSeven.data[SEVEN_BLOCK_A][11] = 0x0800;
    gSeven.data[SEVEN_BLOCK_A][12] = 0x0006;
    gSeven.data[SEVEN_BLOCK_A][13] = 0x0000;
    gSeven.data[SEVEN_BLOCK_A][14] = 0x0001;
    gSeven.data[SEVEN_BLOCK_A][15] = 0x0000;
    u64* dataa = (u64*)gSeven.data;
    dataa[0] ^= 0x31b05271e97e0000;
    dataa[1] ^= 0x67dbafc5e2cc8e42;
    dataa[2] ^= 0xcac5fc5eef2cfc33;
    dataa[3] ^= 0xcba77abc993debd6;
    for (u64 i = 0; i < BLOCK_SIZE; i++)
    {
        gSeven.checksum += gSeven.data[SEVEN_BLOCK_A][i];
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) return 0;
    //u32 tidsid = 0x82B00B40; // debug
    u32 tidsid = strtoul(argv[1], NULL, 16);
    
    printf("TID 0x%08x\n", tidsid);

    u8 filename[32] = { 0 };
    sprintf(filename, "%08x.csv", tidsid);
    FILE* f = fopen(filename, "w+");
    if (f == NULL) return 0;

    clock_t start = clock();

    for (u64 i = 0; i < 4; i++)
    {
        u32 aslr = aslr_en_pt[3 - i]; // do it in reverse because aslr 0 is prone to status changes
        //u32 aslr = aslr_en_pt[i]; // debug
        fprintf(f, "0x%08x\n", aslr);
        printf("ASLR 0x%08x search started.\n", aslr);
        Motor_InitPkmn(tidsid, aslr);
        Motor_Search_Loop(f);
        printf("ASLR 0x%08x search done in %u seconds.\n", aslr, (clock() - start) / 1000);
    }

    fclose(f);
    printf("Complete search done in %u seconds.\n", (clock() - start) / 1000);
    return 0;
}
