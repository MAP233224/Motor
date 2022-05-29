//aslr.h
//functions relative to filtering out seeds for the RNG + ASLR manipulation

#pragma once
#include "common.h"

//MD5 implementation based on https://datatracker.ietf.org/doc/html/rfc1321
//SeedToTime implementation based on Pokefinder's "SeedtoTime4::generate" at https://github.com/Admiral-Fish/PokeFinder/blob/master/Source/Forms/Gen4/Tools/SeedtoTime4.cpp

#define BUFFER_SIZE     (8)
#define DIGEST_SIZE     (16)
#define OFFSETS_MAX     (256)
#define SETUPS_MAX      (8)
#define HOUR_MAX        (23)
#define MONTHS_MAX      (12)
#define WEEKDAYS_MAX    (7)
#define BOOT_TIME       (87) //frames of white screen: 86 mininum, 90 max, to be calibrated

enum { SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY }; //japan order

const u8 DaysInMonth[MONTHS_MAX + 1] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const u8 MagicMonth[MONTHS_MAX] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 }; //magic month array
const u32 VcountTicklo[VERSIONS_MAX - 1] = { 0x003B0007, 0x00000007 }; //to be confirmed
const u32 NDS_Keys[VERSIONS_MAX - 1] = { 0x000003FF, 0x00002FFF };

/* Smallest possible value found at the base sampling address */
const u32 AslrMin[LANGUAGES_MAX][VERSIONS_MAX - 1] = {
    { 0x00000000, 0x00000000 }, //_0
    { 0x02271940, 0x0227D4E0 }, //jp
    { 0x0226D260, 0x0227E0E0 }, //en
    { 0x0226D5E0, 0x0227E3C0 }, //fr
    { 0x0226D440, 0x0227E260 }, //it
    { 0x0226D4A0, 0x0227E2A0 }, //ge
    { 0x00000000, 0x00000000 }, //_6
    { 0x0226D600, 0x0227E3E0 }, //sp
    { 0x02274B00, 0x0227F000 }, //ko
};

/* Base pointers depending on ASLR, Language and Version, 1 per group (ignores mirrors)  */
const u32 aslr__0_dp[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //dummy
const u32 aslr__0_pt[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //dummy
const u32 aslr_jp_dp[ASLR_GROUPS_MAX] = { 0x02271940, 0x02271944, 0x02271948, 0x0227194C, 0x02271970, 0x02271974, 0x02271978, 0x0227197C, 0x022719F0, 0x022719F4, 0x022719F8, 0x022719FC };
const u32 aslr_jp_pt[ASLR_GROUPS_MAX] = { 0x0227D4F0, 0x0227D4F4, 0x0227D4F8, 0x0227D4FC, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //unavailable: nickname crash
const u32 aslr_en_dp[ASLR_GROUPS_MAX] = { 0x0226D2F8, 0x0226D2FC, 0x0226D300, 0x0226D304, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_en_pt[ASLR_GROUPS_MAX] = { 0x0227E140, 0x0227E144, 0x0227E148, 0x0227E14C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_fr_dp[ASLR_GROUPS_MAX] = { 0x0226D5F8, 0x0226D5FC, 0x0226D600, 0x0226D604, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_fr_pt[ASLR_GROUPS_MAX] = { 0x0227E440, 0x0227E444, 0x0227E448, 0x0227E44C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_it_dp[ASLR_GROUPS_MAX] = { 0x0226D500, 0x0226D504, 0x0226D508, 0x0226D50C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_it_pt[ASLR_GROUPS_MAX] = { 0x0227E348, 0x0227E34C, 0x0227E350, 0x0227E354, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_ge_dp[ASLR_GROUPS_MAX] = { 0x0226D500, 0x0226D504, 0x0226D508, 0x0226D50C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_ge_pt[ASLR_GROUPS_MAX] = { 0x0227E348, 0x0227E34C, 0x0227E350, 0x0227E354, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr__6_dp[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //dummy
const u32 aslr__6_pt[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //dummy
const u32 aslr_sp_dp[ASLR_GROUPS_MAX] = { 0x0226D600, 0x0226D604, 0x0226D608, 0x0226D60C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_sp_pt[ASLR_GROUPS_MAX] = { 0x0227E440, 0x0227E444, 0x0227E448, 0x0227E44C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_ko_dp[ASLR_GROUPS_MAX] = { 0x02274B9C, 0x02274BA0, 0x02274BA4, 0x02274BA8, 0x02274BAC, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const u32 aslr_ko_pt[ASLR_GROUPS_MAX] = { 0x0227F0E0, 0x0227F0E4, 0x0227F0E8, 0x0227F0EC, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

/* ASLR table using the grouped version format */
const u32* Aslrs[LANGUAGES_MAX][VERSIONS_MAX - 1] = {
  { aslr__0_dp, aslr__0_pt },
  { aslr_jp_dp, aslr_jp_pt },
  { aslr_en_dp, aslr_en_pt },
  { aslr_fr_dp, aslr_fr_pt },
  { aslr_it_dp, aslr_it_pt },
  { aslr_ge_dp, aslr_ge_pt },
  { aslr__6_dp, aslr__6_pt },
  { aslr_sp_dp, aslr_sp_pt },
  { aslr_ko_dp, aslr_ko_pt }
};

typedef struct {
    u8 year; //(2000+) 0 to 99
    u8 month; //1 to 12
    u8 day; //1 to 31
    u8 hour; //0 to 23
    u8 minute; //0 to 59
    u8 second; //0 to 59
    u16 delay; //0 to MAX_DELAY_DPPT
    //Size: 8 bytes
} DATETIME;

typedef struct {
    DATETIME datetime;
    u8 aslr_hour;
    u8 aslr_minute;
    u8 aslr_second;
    //Size: 12 bytes (aligned)
} DATETIME_EX;

typedef struct {

    u8 setups_count[OFFSETS_MAX / 4]; //counts how many setups have been added for this offset
    DATETIME_EX datetime_ex[OFFSETS_MAX / 4][SETUPS_MAX];

} DATETIME_BUF;

/* MD5 context. */
typedef struct {
    u32 state[4]; /* state (ABCD) */
    u32 count[2]; /* number of bits, modulo 2^64 (lsb first) */
    u8 buffer[64]; /* input buffer */
} MD5_CTX;

static u8 PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions. */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4. */
/* Rotation is separate from addition to prevent recomputation. */

#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (u32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (u32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (u32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (u32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }


static void MD5Init(MD5_CTX* context) {
    /* Begin an MD5 operation, writing a new context. */
    context->count[0] = context->count[1] = 0;
    /* Load magic initialization constants.*/
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

static void Decode(u32* output, u8* input, u32 len) {
    /* Decodes input (u8) into output (u32). */
    /* Assumes len is a multiple of 4. */
    u32 i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[i] = ((u32)input[j]) | (((u32)input[j + 1]) << 8) | (((u32)input[j + 2]) << 16) | (((u32)input[j + 3]) << 24);
    }
}

static void Encode(u8* output, u32* input, u32 len) {
    /* Encodes input (u32) into output (u8). */
    /* Assumes len is a multiple of 4. */
    u32 i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[j] = (u8)(input[i] & 0xff);
        output[j + 1] = (u8)((input[i] >> 8) & 0xff);
        output[j + 2] = (u8)((input[i] >> 16) & 0xff);
        output[j + 3] = (u8)((input[i] >> 24) & 0xff);
    }
}

static void MD5Transform(u32 state[4], u8 block[64]) {
    /* MD5 basic transformation. Transforms state based on block. */
    u32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    Decode(x, block, 64);

    /* Round 1 */
    FF(a, b, c, d, x[0], 7, 0xd76aa478); /* 1 */
    FF(d, a, b, c, x[1], 12, 0xe8c7b756); /* 2 */
    FF(c, d, a, b, x[2], 17, 0x242070db); /* 3 */
    FF(b, c, d, a, x[3], 22, 0xc1bdceee); /* 4 */
    FF(a, b, c, d, x[4], 7, 0xf57c0faf); /* 5 */
    FF(d, a, b, c, x[5], 12, 0x4787c62a); /* 6 */
    FF(c, d, a, b, x[6], 17, 0xa8304613); /* 7 */
    FF(b, c, d, a, x[7], 22, 0xfd469501); /* 8 */
    FF(a, b, c, d, x[8], 7, 0x698098d8); /* 9 */
    FF(d, a, b, c, x[9], 12, 0x8b44f7af); /* 10 */
    FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
    FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
    FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
    FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
    FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
    FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */

    /* Round 2 */
    GG(a, b, c, d, x[1], 5, 0xf61e2562); /* 17 */
    GG(d, a, b, c, x[6], 9, 0xc040b340); /* 18 */
    GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
    GG(b, c, d, a, x[0], 20, 0xe9b6c7aa); /* 20 */
    GG(a, b, c, d, x[5], 5, 0xd62f105d); /* 21 */
    GG(d, a, b, c, x[10], 9, 0x2441453); /* 22 */
    GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
    GG(b, c, d, a, x[4], 20, 0xe7d3fbc8); /* 24 */
    GG(a, b, c, d, x[9], 5, 0x21e1cde6); /* 25 */
    GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
    GG(c, d, a, b, x[3], 14, 0xf4d50d87); /* 27 */
    GG(b, c, d, a, x[8], 20, 0x455a14ed); /* 28 */
    GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
    GG(d, a, b, c, x[2], 9, 0xfcefa3f8); /* 30 */
    GG(c, d, a, b, x[7], 14, 0x676f02d9); /* 31 */
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH(a, b, c, d, x[5], 4, 0xfffa3942); /* 33 */
    HH(d, a, b, c, x[8], 11, 0x8771f681); /* 34 */
    HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
    HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
    HH(a, b, c, d, x[1], 4, 0xa4beea44); /* 37 */
    HH(d, a, b, c, x[4], 11, 0x4bdecfa9); /* 38 */
    HH(c, d, a, b, x[7], 16, 0xf6bb4b60); /* 39 */
    HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
    HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
    HH(d, a, b, c, x[0], 11, 0xeaa127fa); /* 42 */
    HH(c, d, a, b, x[3], 16, 0xd4ef3085); /* 43 */
    HH(b, c, d, a, x[6], 23, 0x4881d05); /* 44 */
    HH(a, b, c, d, x[9], 4, 0xd9d4d039); /* 45 */
    HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
    HH(b, c, d, a, x[2], 23, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II(a, b, c, d, x[0], 6, 0xf4292244); /* 49 */
    II(d, a, b, c, x[7], 10, 0x432aff97); /* 50 */
    II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
    II(b, c, d, a, x[5], 21, 0xfc93a039); /* 52 */
    II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
    II(d, a, b, c, x[3], 10, 0x8f0ccc92); /* 54 */
    II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
    II(b, c, d, a, x[1], 21, 0x85845dd1); /* 56 */
    II(a, b, c, d, x[8], 6, 0x6fa87e4f); /* 57 */
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
    II(c, d, a, b, x[6], 15, 0xa3014314); /* 59 */
    II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
    II(a, b, c, d, x[4], 6, 0xf7537e82); /* 61 */
    II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
    II(c, d, a, b, x[2], 15, 0x2ad7d2bb); /* 63 */
    II(b, c, d, a, x[9], 21, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    /* Zeroize sensitive information. */
    memset((u8*)x, 0, sizeof(x));
}

static void MD5Update(MD5_CTX* context, u8* input, u32 inputLen) {
    /* MD5 block update operation. */
    /* Continues an MD5 message-digest operation, processing another message block, and updating the context. */
    u32 i, index, partLen;

    /* Compute number of bytes mod 64 */
    index = (u32)((context->count[0] >> 3) & 0x3F);

    /* Update number of bits */
    if ((context->count[0] += ((u32)inputLen << 3)) < ((u32)inputLen << 3))
    {
        context->count[1]++;
    }

    context->count[1] += ((u32)inputLen >> 29);

    partLen = 64 - index;

    /* Transform as many times as possible. */
    if (inputLen >= partLen)
    {
        memcpy((u8*)&context->buffer[index], (u8*)input, partLen);
        MD5Transform(context->state, context->buffer);

        for (i = partLen; i + 63 < inputLen; i += 64)
        {
            MD5Transform(context->state, &input[i]);
        }

        index = 0;
    }
    else
    {
        i = 0;
    }

    /* Buffer remaining input */
    memcpy((u8*)&context->buffer[index], (u8*)&input[i], inputLen - i);
}

static void MD5Final(u8 digest[16], MD5_CTX* context) {
    /* Write the message digest and zero out the context. */
    u8 bits[8];
    u32 index, padLen;
    /* Save number of bits */
    Encode(bits, context->count, 8);
    /* Pad out to 56 mod 64. */
    index = (u32)((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    MD5Update(context, PADDING, padLen);
    /* Append length (before padding) */
    MD5Update(context, bits, 8);
    /* Store state in digest */
    Encode(digest, context->state, 16);
    /* Zeroize sensitive information. */
    memset((u8*)context, 0, sizeof(*context));
}

static u8 GetDayOfWeek(DATETIME* dt) {
    /* magic */
    u32 y = 2000 + dt->year - (dt->month < 3);
    return (y + y / 4 - y / 100 + y / 400 + MagicMonth[dt->month - 1] + dt->day) % WEEKDAYS_MAX;
}

static u32 GetRTCLow(DATETIME* dt) {
    /* Format year, month, day, weekday (0xWWDDMMYY) */
    return (GetDayOfWeek(dt) << 24) | (BCD(dt->day) << 16) | (BCD(dt->month) << 8) | (BCD(dt->year));
}

static u32 GetRTCHigh(DATETIME* dt) {
    /* Format hour, minute, second (0x00SSMMHH) */
    u8 hour = BCD(dt->hour) + 0x40 * (dt->hour > 11); //add 0x40 if PM
    return (BCD(dt->second) << 16) | (BCD(dt->minute) << 8) | hour;
}

static u32 MD5GetHeapOffset(PROFILE* pf, DATETIME* dt, u32 buffer[BUFFER_SIZE]) {
    /* From MAC address and date/time, calculate the MD5 of a buffer and the heap offset of the digest */

    //debug print buffer
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        DLOG("b[%u] = 0x%08X\n", i, buffer[i]);
    }

    /* Get MD5 digest of buffer */
    MD5_CTX ctx = { 0 };
    u8 digest[DIGEST_SIZE] = { 0 };
    MD5Init(&ctx);
    //MD5Update(&ctx, (u8*)buffer, sizeof(buffer));
    MD5Update(&ctx, (u8*)buffer, sizeof(buffer[0]) * BUFFER_SIZE);
    MD5Final(digest, &ctx);
    /* Get offset from digest */
    u32 offset = 0;
    for (u32 i = 0; i < DIGEST_SIZE; i++) { offset += digest[i]; }
    offset %= 256;
    while (offset % 4) { offset++; } //making sure it's a multiple of 4 for 32 bit alignment
    return offset;
}

static u8 GetDaysInMonth(u8 month, u8 year) {
    /* Return the number of days in a specific month, check year for february */
    /* Assuming year between (20)00 and (20)99 and month between 1 and 12 */
    if (month == 2)
    {
        if (year % 4) { return 28; }
        return 29;
    }
    return DaysInMonth[month];
}

static u8 GetAslrOffset(PROFILE* pf) {
    /* Return the difference between the user's ASLR and the minimum possible ASLR */
    return Aslrs[pf->language][pf->version >> 1][pf->aslr] - AslrMin[pf->language][pf->version >> 1];
}

static void CheckRollBack(DATETIME* dt, int minute, int second, int second_dif) {
    /* Check clock roll back for seconds and minutes */
    if (second - second_dif < 0)
    {
        dt->second = 60 + second - second_dif;
        dt->minute = minute ? minute - 1 : 59;
    }
    else
    {
        dt->second = second - second_dif;
        dt->minute = minute;
    }
}

static BOOL SendSetupToBuffer(DATETIME_BUF* dtbuf, DATETIME* dt, u8 offset, u8 hour, u8 minute, u8 second) {
    /* Add a date time setup to a buffer if the offs */
    if (dtbuf->setups_count[offset] >= SETUPS_MAX) { return FALSE; } //don't add if limit reached for this offset
    DATETIME_EX dtex = { .aslr_hour = hour, .aslr_minute = minute, .aslr_second = second };
    memcpy(&dtex.datetime, dt, sizeof(DATETIME)); //copy DATETIME to DATETIME_EX
    memcpy(&dtbuf->datetime_ex[offset][dtbuf->setups_count[offset]], &dtex, sizeof(DATETIME_EX)); //copy DATETIME_EX to DATETIME_BUF
    dtbuf->setups_count[offset]++;
    return TRUE; //successfully added to the buffer
}

static APPSTATUS SeedToTime_groups(u32 seed, PROFILE* pf, u8 year) {
    /* Get a list of all possible time/date setup depending on seed and mac address (rng + aslr manip) */
    //TODO: aslr groups!!!

    //Concept: instead of matching against 1 specific ASLR offset,
    //just print each setups for a given year, and spread them accross all possible aslr offsets


    u8 filepath[PATH_REL_LENGTH_MAX] = { 0 };
    sprintf(filepath, ".results/%08X_TIME.txt", seed);
    FILE* fp = fopen(filepath, "w+");
    if (fp == NULL) { return APP_ERR_OPEN_FILE; }

    fprintf(fp, "Seed: 0x%08X\n", seed);
    fprintf(fp, "Year: 20%02u\n", year);
    fprintf(fp, "ASLR: %02u (0x%08X)\n", pf->aslr, Aslrs[pf->language][pf->version >> 1][pf->aslr]);
    fprintf(fp, "MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n\n", pf->mac[0], pf->mac[1], pf->mac[2], pf->mac[3], pf->mac[4], pf->mac[5]);

    fprintf(fp, "DATE       ASLR time  SEED time\n");
    fprintf(fp, "dd/mm/yyyy (hh:mm:ss) hh:mm:ss delay\n");

    //Now, technically nothing matters in the buffer except date/time (others are all assumed constant)
    u32 buffer[BUFFER_SIZE] = {
        VcountTicklo[pf->version >> 1], //assuming hard reset / boot //+-1
        (*(u16*)(pf->mac + 4)) << 16, //last two u8 of MAC address in a u16, << 16
        0x86000001 ^ (*(u32*)pf->mac), //first 4 u8 of MAC address in a u32
        0x00000000, //rtc_low (date)
        0x00000000, //rtc_high (time)
        0x00000000, //constant
        0x00000000, //constant
        NDS_Keys[pf->version >> 1], //assumes the user isn't pressing any buttons during game boot
    };

    DATETIME dt = { 0 };
    DATETIME_BUF dtbuf = { 0 }; //max 8 per offset (Size = 256*8*12 = 24576 bytes)

    u32 results = 0;
    u8 hasPrintedOffset[OFFSETS_MAX] = { 0 }; //prevents printing the same offset more than once

    u8 ab = seed >> 24;
    u8 cd = (seed >> 16) & 0xFF;
    u32 efgh = seed & 0xFFFF;

    if (cd > HOUR_MAX)
    {
        dt.hour = HOUR_MAX;
        dt.delay = efgh - year + ((cd - HOUR_MAX) << 16);
    }
    else
    {
        dt.hour = cd;
        dt.delay = efgh - year;
    }

    dt.year = year;

    int second_dif = (dt.delay + BOOT_TIME) / 60; // difference between the time the game boots (ASLR manip) and the time you hit the seed (RNG manip)

    for (int month = 1; month <= 12; month++)
    {
        u8 maxDays = GetDaysInMonth(month, year);

        for (int day = 1; day <= maxDays; day++)
        {
            for (int minute = 0; minute < 60; minute++)
            {
                for (int second = 0; second < 60; second++)
                {
                    if (ab == ((month * day + minute + second) & 0xFF))
                    {
                        /* Found a matching seed, check rtc and prepare buffer */
                        CheckRollBack(&dt, minute, second, second_dif);
                        dt.month = month;
                        dt.day = day;

                        buffer[3] = GetRTCLow(&dt);
                        buffer[4] = GetRTCHigh(&dt);

                        u8 offset = MD5GetHeapOffset(pf, &dt, buffer); //currently ignoring offset 256

                        int hour_aslr = dt.hour - (dt.minute > minute); //checking underflow
                        if (hour_aslr >= 0)
                        {
                            if (SendSetupToBuffer(&dtbuf, &dt, offset / 4, hour_aslr, minute, second))
                            {
                                results++;
                            }
                        }
                    }
                }
            }
        }
    }

    /* Print DATETIME_EX buffer */
    for (u32 i = 0; i < OFFSETS_MAX / 4; i++)
    {
        fprintf(fp, "%02u\n", i); //offset/4
        for (u32 j = 0; j < SETUPS_MAX; j++)
        {
            DATETIME_EX* dtexp = &dtbuf.datetime_ex[i][j]; //get from buffer
            fprintf(fp, "%02u/%02u/20%02u (%02u:%02u:%02u) %02u:%02u:%02u %u\n",
                dtexp->datetime.day, dtexp->datetime.month, dtexp->datetime.year, dtexp->aslr_hour, dtexp->datetime.minute, dtexp->datetime.second, dtexp->datetime.hour, dtexp->aslr_minute, dtexp->aslr_second, dtexp->datetime.delay);
        }
    }

    fprintf(fp, "\nFound %u date/time setup(s).", results);
    fclose(fp);
    return APP_RESUME;
}

static APPSTATUS SeedToTime(u32 seed, PROFILE* pf, u8 year) {
    /* Get a list of all possible time/date setup depending on seed and mac address (rng + aslr manip) */

    u8 filepath[PATH_REL_LENGTH_MAX] = { 0 };
    sprintf(filepath, ".results/%08X_TIME.txt", seed);
    FILE* fp = fopen(filepath, "w+");
    if (fp == NULL) { return APP_ERR_OPEN_FILE; }

    fprintf(fp, "Seed: 0x%08X\n", seed);
    fprintf(fp, "Year: 20%02u\n", year);
    fprintf(fp, "ASLR: %02u (0x%08X)\n", pf->aslr, Aslrs[pf->language][pf->version >> 1][pf->aslr]);
    fprintf(fp, "MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n\n", pf->mac[0], pf->mac[1], pf->mac[2], pf->mac[3], pf->mac[4], pf->mac[5]);

    fprintf(fp, "DATE       ASLR time  SEED time\n");
    fprintf(fp, "dd/mm/yyyy (hh:mm:ss) hh:mm:ss delay\n");

    //todo: coin flips (mt rng) for current and nearby (pick a range) seeds

    //buffer[0] = (vcount << 16) | tick_low
    // 0x003B0007 (DP: EN, IT (R4, closest to retail) rarely: 0x003B0006, 0x003B0008)
    //vcount could be: 
    //0x0093 (bizhawk no firmware/bios, dp en, fr, sp)
    //0x00ED (bizhawk with firmware/bios, dp en)
    //0x0000 (bizhawk no firmware/bios, plat fr)
    //0x0083 (desmume, dp en, it)
    //0x0080 (desmume, dp jp)

    u32 buffer[BUFFER_SIZE] = {
        VcountTicklo[pf->version >> 1], //assuming hard reset / boot //+-1
        (*(u16*)(pf->mac + 4)) << 16, //last two u8 of MAC address in a u16, << 16
        0x86000001 ^ (*(u32*)pf->mac), //first 4 u8 of MAC address in a u32
        0x00000000, //rtc_low (date)
        0x00000000, //rtc_high (time)
        0x00000000, //constant
        0x00000000, //constant
        NDS_Keys[pf->version >> 1], //assumes the user isn't pressing any buttons during game boot
    };

    DATETIME dt = { 0 };

    u32 results = 0;

    u8 offset = GetAslrOffset(pf);

    u8 ab = seed >> 24;
    u8 cd = (seed >> 16) & 0xFF;
    u32 efgh = seed & 0xFFFF;

    if (cd > HOUR_MAX)
    {
        dt.hour = HOUR_MAX;
        dt.delay = efgh - year + ((cd - HOUR_MAX) << 16);
    }
    else
    {
        dt.hour = cd;
        dt.delay = efgh - year;
    }

    dt.year = year;

    int second_dif = (dt.delay + BOOT_TIME) / 60; // difference between the time the game boots (ASLR manip) and the time you hit the seed (RNG manip)

    for (int month = 1; month <= 12; month++)
    {
        u8 maxDays = GetDaysInMonth(month, year);

        for (int day = 1; day <= maxDays; day++)
        {
            for (int minute = 0; minute < 60; minute++)
            {
                for (int second = 0; second < 60; second++)
                {
                    if (ab == ((month * day + minute + second) & 0xFF))
                    {
                        /* Found a matching seed, check rtc and prepare buffer */
                        CheckRollBack(&dt, minute, second, second_dif);
                        dt.month = month;
                        dt.day = day;

                        buffer[3] = GetRTCLow(&dt);
                        buffer[4] = GetRTCHigh(&dt);

                        if (MD5GetHeapOffset(pf, &dt, buffer) == offset)
                        {
                            /* Offsets are matching, print rtc + delay to file */
                            int hour_aslr = dt.hour - (dt.minute > minute); //checking underflow
                            if (hour_aslr >= 0)
                            {
                                fprintf(fp, "%02u/%02u/20%02u (%02u:%02u:%02u) %02u:%02u:%02u %u\n",
                                    dt.day, dt.month, dt.year, hour_aslr, dt.minute, dt.second, dt.hour, minute, second, dt.delay);
                                results++;
                            }
                        }
                    }
                }
            }
        }
    }

    fprintf(fp, "\nFound %u date/time setup(s).", results);
    fclose(fp);
    return APP_RESUME;
}
