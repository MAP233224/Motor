// main.c

#include "motor.h"

PKMN gWild = { 0 };
PKMN gSeven = { 0 };

#define PIDIV32_MAX (2551446)
u32 gPIDIV32[3 * PIDIV32_MAX] = { 0 };

static void Motor_Search_Loop(FILE* file)
{
    for (u32 pid = 0; pid < 3 * (PIDIV32_MAX - 1); pid += 3)
    {
        /* Init Wild */
        PKMN wild = { 0 };

        u32 seed = gPIDIV32[pid];
        wild.pid = gPIDIV32[pid + 1];
        wild.iv32 = gPIDIV32[pid + 2];
        SetBlocks(&wild);

        memcpy(wild.data[wild.pos_a], &gWild.data[0], BLOCK_SIZE * sizeof(wild.data[0][0]));
        memcpy(wild.data[wild.pos_b], &gWild.data[1], BLOCK_SIZE * sizeof(wild.data[0][0]));
        memcpy(wild.data[wild.pos_c], &gWild.data[2], BLOCK_SIZE * sizeof(wild.data[0][0]));
        memcpy(wild.data[wild.pos_d], &gWild.data[3], BLOCK_SIZE * sizeof(wild.data[0][0]));
        memcpy(&wild.data[wild.pos_b][8], &wild.iv32, sizeof(wild.iv32));
        wild.data[wild.pos_b][12] = 0x0004; // genderless
        wild.cond[2] = 0x0014; // level 20

        SetChecksumFastWild(&wild);
        EncryptBlocks(&wild);

        /* Init Seven */
        PKMN seven = { 0 }; //always ACBD (0x00020103)
        /* Block order up to Block A */
        memcpy(&seven, &gSeven, sizeof(seven) - sizeof(seven.cond) - 3 * sizeof(seven.data[0]));
        /* Simulate the buffer overflow */
        /* Block C, B, D and Condition data */
        memcpy(&seven.data[SEVEN_BLOCK_C], &wild.pid, 2 * (BLOCKS * BLOCK_SIZE - 8)); // up to the wild's moves

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
        u16 move0 = seven.data[1 + wild.pos_b][STACK_OFFSET + 0];
        u16 move1 = seven.data[1 + wild.pos_b][STACK_OFFSET + 1];
        u16 move2 = seven.data[1 + wild.pos_b][STACK_OFFSET + 2];
        u16 move3 = seven.data[1 + wild.pos_b][STACK_OFFSET + 3];
        if (move0 <= VALID_MOVES_MAX) { valid_moves++; }
        if (move1 <= VALID_MOVES_MAX) { valid_moves++; }
        if (move2 <= VALID_MOVES_MAX) { valid_moves++; }
        if (move3 <= VALID_MOVES_MAX) { valid_moves++; }

        /* Print successful result */
        if (valid_moves > 1)
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
    gSeven.data[SEVEN_BLOCK_A][10] = 0x3A05; // low byte is heap id
    gSeven.data[SEVEN_BLOCK_A][11] = 0x0800;
    gSeven.data[SEVEN_BLOCK_A][12] = 0x0006;
    gSeven.data[SEVEN_BLOCK_A][13] = 0x0000;
    gSeven.data[SEVEN_BLOCK_A][14] = 0x0001; // party count low
    gSeven.data[SEVEN_BLOCK_A][15] = 0x0000; // party count high
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
    if ((argc < 2) || (argc > 3)) return 0; // motorathome <tid_min> [<tid_max>]

    u32 tid_min = strtoul(argv[1], NULL, 16);
    u32 tid_max = (argc == 3) ? strtoul(argv[2], NULL, 16) : tid_min;

    if (tid_min > tid_max) // swap
    {
        u32 tmp = tid_max;
        tid_max = tid_min;
        tid_min = tmp;
    }

    /* Load the PID/IVs pairs from file */
    FILE* fpidiv32 = fopen("seedpidiv32.bin", "rb");
    if (fpidiv32 == NULL) return 0;
    fread(&gPIDIV32, sizeof(gPIDIV32), 1, fpidiv32);
    fclose(fpidiv32);

    u8 filename[32] = { 0 };
    sprintf(filename, ".results/%08x_%08x.csv", tid_min, tid_max);
    FILE* fresults = fopen(filename, "w+");
    if (fresults == NULL) return 0;
    fclose(fresults);

    clock_t start = clock();
    printf("TID 0x%08x to 0x%08x search started.\n", tid_min, tid_max);

    for (u32 tid = tid_min; tid <= tid_max; tid++)
    {
        fresults = fopen(filename, "a");
        if (fresults == NULL) return 0;
        printf("TID 0x%08x search started.\n", tid);
        fprintf(fresults, "[0x%08x]\n", tid);
        for (u64 i = 0; i < 4; i++)
        {
            u32 aslr = aslr_en_pt[3 - i]; // do it in reverse because aslr 0 is prone to status changes
            //u32 aslr = aslr_en_pt[i]; // debug
            fprintf(fresults, "(0x%08x)\n", aslr);
            Motor_InitPkmn(tid, aslr);
            Motor_Search_Loop(fresults);
        }
        fclose(fresults);
    }

    printf("Complete search done in %u seconds.\n", (clock() - start) / CLOCKS_PER_SEC);
    return 0;
}
