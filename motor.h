// motor.h

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;

/* Constants */
#define BLOCKS              (4)     // Number of blocks: A, B, C and D
#define BLOCK_PERMS         (24)    // Factorial of the number of blocks
#define BLOCK_SIZE          (16)    // Number of 16-bit words in 128 bytes, the size of PKMN Block data
#define COND_SIZE           (50)    // Number of 16-bit words in 100 bytes, the size of PKMN Condition data
#define COND_SIZE_S         (25)    // Utility: stop earlier to avoid needless encryption
#define COND_SIZE_XS        (5)     // Utility: stop even earlier to avoid needless encryption
#define ABILITIES_MAX       (124)   // Number of abilities
#define SPECIES_MAX         (494)   // Number of species
#define MOVES_MAX           (468)   // Number of moves
#define VALID_MOVES_MAX     (MOVES_MAX + 2)   // Number of valid moves
#define ITEMS_MAX           (468)   // Number of items (465 in Diamond and Pearl but it doesn't make a difference)
#define OWN_MOVES_MAX       (4)     // Maximum number of moves a PKMN can know at the same time
#define STATS_MAX           (6)     // Number of stats: Hit Points, Attack, Defense, Speed, Special Attack and Special Defense
#define NATURES_MAX         (25)    // Number of natures
#define STACK_OFFSET        (4)     // Misalignment between wild and seven
#define HEAPID_MAX          (0x5C)  // Hardcoded game constant. Apparently 0x5B would work too?
#define BALL_ID_MAX         (20)    // Hardcoded? game constant
#define NAME_CHARS_MAX      (11)

// Seven's PID is static (0x00005544), so its block order is always ACBD
#define SEVEN_BLOCK_A (0)
#define SEVEN_BLOCK_C (1)
#define SEVEN_BLOCK_B (2)
#define SEVEN_BLOCK_D (3)

/* Offset from the base pointer */
#define OPP_PARTY_OFFSET_START (0x4B884)
#define OPP_PARTY_OFFSET_END   (0x4C3E4)

/* The 24 ABCD Block permutations with the first 8 repeated at the end for optimization purposes */
const u32 Perms[32] = { 0x03020100, 0x02030100, 0x03010200, 0x02010300, 0x01030200, 0x01020300, 0x03020001, 0x02030001, 0x03010002, 0x02010003, 0x01030002, 0x01020003, 0x03000201, 0x02000301, 0x03000102, 0x02000103, 0x01000302, 0x01000203, 0x00030201, 0x00020301, 0x00030102, 0x00020103, 0x00010302, 0x00010203, 0x03020100, 0x02030100, 0x03010200, 0x02010300, 0x01030200, 0x01020300, 0x03020001, 0x02030001 };

/* All valid English Platinum ASLRs */
/* 0x0227E140 seems to always update Seven's status and re-corrupt the wild, turning it into a Bad Egg */
/* 0x0227E140 is the only ASLR with a mirror, 0x0227E150 */
const u32 aslr_en_pt[4] = { 0x0227E140, 0x0227E144, 0x0227E148, 0x0227E14C };

typedef struct {
    u8 pos_a;
    u8 pos_b;
    u8 pos_c;
    u8 pos_d;
    u32 iv32;
    //Main pkmn data; ordered the same as in game to simulate the buffer overflow
    u32 pid;
    u16 bef;
    u16 checksum;
    u16 data[BLOCKS][BLOCK_SIZE];
    u16 cond[COND_SIZE_S]; // todo: see if can be replaced by COND_SIZE_XS (would save 40 bytes)
    // Size: 196 bytes
} PKMN;

/* Functions */

static u32 GetNatureId(u32 pid)
{
    /* Get the ID of the Nature (from 0 to 24), provided the PID. */
    // optimized to avoid div instruction (pid % 25)
    return pid - NATURES_MAX * ((0x51EB851FULL * pid) >> 35);
}

static void SetBlocks(PKMN* pkmn)
{
    /* Get the order of each block from the PID and set them in the correct permutation */
    *(u32*)(&pkmn->pos_a) = Perms[(pkmn->pid >> 13) & 31];
}

static void SetChecksum(PKMN* pkmn)
{
    /* Sets the checksum of a PKMN by summing all of its Block data */
    u16* data = (u16*)&pkmn->data;
    u16 c = 0;
    for (u64 i = 0; i < BLOCKS * BLOCK_SIZE; i++)
    {
        c += data[i];
    }
    pkmn->checksum = c;
}

static void SetChecksumFastSeven(PKMN* pkmn)
{
    /* Checksum already initialized with Block A, sum the remaining blocks */
    u16* data = (u16*)&pkmn->data[1];
    u16 c = 0;
    for (u64 i = 0; i < (BLOCKS - 1) * BLOCK_SIZE; i++)
    {
        c += data[i];
    }
    pkmn->checksum += c;
}

static u32 RngNext(u32* state)
{
    /* General purpose LCRNG, advance and return state */
    *state = *state * 0x41C64E6D + 0x6073;
    return *state;
}

static void EncryptBlocks(PKMN* pkmn)
{
    /* LCRNG is seeded with the Checksum */
    /* Advance the LCRNG, XOR its 16 most significant bits with each 16-bit word of ABCD Block data */
    /* Non-intrinsic version (slower) */
    u32 state = pkmn->checksum;
    u64* data = (u64*)pkmn->data;
    for (u64 i = 0; i < 16; i++)
    {
        u32 state2 = state * 0xC2A29A69 + 0xE97E7B6A; //advance LCRNG by 2
        u32 state3 = state * 0x807DBCB5 + 0x52713895; //advance LCRNG by 3
        u32 state4 = state * 0xEE067F11 + 0x31B0DDE4; //advance LCRNG by 4
        state = state * 0x41C64E6D + 0x00006073; //advance LCRNG by 1
        u64 x = ((u64)(state >> 16)) | ((u64)(state2 & 0xffff0000)) | ((u64)(state3 >> 16) << 32) | ((u64)(state4 >> 16) << 48);
        data[i] ^= x;
        state = state4;
    }
}

static void EncryptBlocksChecksumZero(PKMN* pkmn)
{
    /* Fast encryption with precomputed RNG XOR mask (checksum == 0) */
    /* Block A (0) is encrypted in MotorInitPkmn */
    u64* d = (u64*)&pkmn->data[1];
    d[0] ^= 0x618d27a691785dd6;
    d[1] ^= 0x3080375dcfb81692;
    d[2] ^= 0xfee7321348fb407c;
    d[3] ^= 0x1d29639e3d69dfa3;
    d[4] ^= 0xa39792686296ea8d;
    d[5] ^= 0xaa8931aa6e031c49;
    d[6] ^= 0xe0c682d9c3ead3c5;
    d[7] ^= 0x24285a5f4e3b945c;
    d[8] ^= 0x007f7b8ebfe1fbb3;
    d[9] ^= 0x38b6bfd1c84840c4;
    d[10] ^= 0xbe347d23fb23903b;
    d[11] ^= 0xba84dfc5706ada00;
}

static void EncryptCondition(PKMN* pkmn)
{
    /* LCRNG is seeded with the PID */
    /* Advance the LCRNG, XOR its 16 most significant bits with each 16-bit word of Condition data */
    /* It is not needed to encrypt the whole 50 16-bit words of Condition data, I stop at 5 to include HP MAX */
    u32 state = pkmn->pid;
    for (u64 i = 0; i < COND_SIZE_XS; i++)
    {
        pkmn->cond[i] ^= (RngNext(&state) >> 16);
    }
}

static void MethodJSeedToPID(u32 state, PKMN* pkmn)
{
    /* Calculate PID, Nature and IVs according to Method J Stationary (no Synchronize / Cute Charm) from a given state */
    u32 nature = ((RngNext(&state) >> 17) * 25595) >> 25; // fast division by 0x0A3E0000 of the 32-bit state
    do {
        u32 state2 = state * 0xC2A29A69 + 0xE97E7B6A; //advance LCRNG by 2
        state = state * 0x41C64E6D + 0x00006073; //advance LCRNG by 1
        pkmn->pid = (state >> 16) | (state2 & 0xffff0000);
        state = state2;
    } while (GetNatureId(pkmn->pid) != nature);
    pkmn->iv32 = ((RngNext(&state) >> 16) & 0x00007fff) | ((RngNext(&state) >> 1) & 0x3fff8000);
}
