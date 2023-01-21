// motor.h

#pragma once
#include "common.h"
#include "aslr.h"

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
#define ITEMS_MAX           (468)   // Number of items (465 in Diamond and Pearl but it doesn't make a difference)
#define OWN_MOVES_MAX       (4)     // Maximum number of moves a PKMN can know at the same time
#define STACK_OFFSET        (4)     // Misalignment between wild and seven
#define KOREAN_OFFSET       (0x44)  // Korean RAM quirk
#define HEAPID_MAX          (0x5C)  // Hardcoded game constant. Apparently 0x5B would work too?
#define BALL_ID_MAX         (20)    // Hardcoded? game constant
#define SEED_MAX_B          (24)    // Hours in a day
#define SEED_OFF_C          (3600)  // Delay, 1 minute of leeway
#define MIN_DELAY_DPPT      (700)   // Minimum delay you can get in DPPT from a Save&Quit
#define MAX_DELAY_DPPT      (MIN_DELAY_DPPT + SEED_OFF_C)
#define TYPES_MAX           (18)    // Total number of types, including ???
#define NAME_CHARS_MAX      (11)

// Seven's PID is static (0x00005544), so its block order is always ACBD
#define SEVEN_BLOCK_A (0)
#define SEVEN_BLOCK_C (1)
#define SEVEN_BLOCK_B (2)
#define SEVEN_BLOCK_D (3)

#define NAME_ROTOM          {0x013C, 0x0139, 0x013E, 0x0139, 0x0137, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_ROTOM_JP       {0x013C, 0x0139, 0x013E, 0x0139, 0x0137, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_ROTOM_KO       {0x06C0, 0x0BFA, 0x0759, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_ROTOM_FR       {0x0137, 0x0139, 0x013E, 0x0133, 0x013D, 0x0137, 0x012B, 0xFFFF, 0x0000, 0x0000, 0x0000}
#define NAME_DIALGA         {0x012E, 0x0133, 0x012B, 0x0136, 0x0131, 0x012B, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_DIALGA_JP      {0x0078, 0x0054, 0x0053, 0x009C, 0x005D, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_DIALGA_KO      {0x0626, 0x094A, 0x06D9, 0x0401, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_PALKIA         {0x013A, 0x012B, 0x0136, 0x0135, 0x0133, 0x012B, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_PALKIA_JP      {0x0082, 0x009C, 0x005E, 0x0053, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_PALKIA_KO      {0x0C52, 0x04A0, 0x094A, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_GIRATINA       {0x0131, 0x0133, 0x013C, 0x012B, 0x013E, 0x0133, 0x0138, 0x012B, 0xFFFF, 0x0000, 0x0000}
#define NAME_GIRATINA_JP    {0x005F, 0x009A, 0x0077, 0x0054, 0x007B, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_GIRATINA_KO    {0x04A0, 0x0687, 0x0C30, 0x0524, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_ARCEUS         {0x012B, 0x013C, 0x012D, 0x012F, 0x013F, 0x013D, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_ARCEUS_JP      {0x0053, 0x009C, 0x006C, 0x0057, 0x006A, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_ARCEUS_KO      {0x094A, 0x06F3, 0x0884, 0x09CE, 0x08E0, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_DARKRAI        {0x012E, 0x012B, 0x013C, 0x0135, 0x013C, 0x012B, 0x0133, 0xFFFF, 0x0000, 0x0000, 0x0000}
#define NAME_DARKRAI_JP     {0x0071, 0x00F1, 0x0060, 0x009A, 0x0055, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_DARKRAI_KO     {0x05B1, 0x0BBF, 0x0687, 0x0A0C, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_SHAYMIN        {0x013D, 0x0132, 0x012B, 0x0143, 0x0137, 0x0133, 0x0138, 0xFFFF, 0x0000, 0x0000, 0x0000}
#define NAME_SHAYMIN_JP     {0x0068, 0x0058, 0x0055, 0x0090, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_SHAYMIN_KO     {0x08CC, 0x0A0C, 0x077A, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_UXIE           {0x013F, 0x0142, 0x0133, 0x012F, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_UXIE_FR        {0x013F, 0x0142, 0x0133, 0x012F, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_UXIE_GE        {0x013D, 0x012F, 0x0136, 0x0130, 0x012F, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_UXIE_JP        {0x0097, 0x0060, 0x0068, 0x00F1, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_UXIE_KO        {0x09EF, 0x0BBF, 0x08E9, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_AZELF          {0x012B, 0x0144, 0x012F, 0x0136, 0x0130, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_AZELF_FR       {0x012D, 0x013C, 0x012F, 0x0130, 0x012B, 0x012E, 0x012F, 0x013E, 0xFFFF, 0x0000, 0x0000}
#define NAME_AZELF_GE       {0x013E, 0x0139, 0x012C, 0x013F, 0x013E, 0x0144, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_AZELF_JP       {0x0053, 0x0061, 0x007F, 0x0091, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_AZELF_KO       {0x094A, 0x0495, 0x056A, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_REGIGIGAS      {0x013C, 0x012F, 0x0131, 0x0133, 0x0131, 0x0133, 0x0131, 0x012B, 0x013D, 0xFFFF, 0x0000}
#define NAME_REGIGIGAS_JP   {0x009D, 0x0069, 0x005F, 0x005D, 0x006A, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_REGIGIGAS_KO   {0x06AB, 0x0A94, 0x04A0, 0x0401, 0x08E0, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_HEATRAN        {0x0132, 0x012F, 0x012B, 0x013E, 0x013C, 0x012B, 0x0138, 0xFFFF, 0x0000, 0x0000, 0x0000}
#define NAME_HEATRAN_JP     {0x0083, 0x00F1, 0x007A, 0x009A, 0x00A1, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
#define NAME_HEATRAN_KO     {0x0D27, 0x061B, 0x06A3, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}

#define MOVES_GIRATINA_DP   {0x01D3, 0x0179, 0x019E, 0x00A3}
#define MOVES_ARCEUS_DP     {0x011F, 0x00F8, 0x0069, 0x003F}
#define MOVES_SHAYMIN_DP    {0x006F, 0x0159, 0x0049, 0x00EB}
#define MOVES_DARKRAI_DP    {0x0062, 0x005F, 0x00E4, 0x00AB}
#define MOVES_UXIE_DP       {0x005D, 0x0119, 0x00F8, 0x0085}
#define MOVES_AZELF_DP      {0x005D, 0x00FD, 0x00F8, 0x01A1}
#define MOVES_DIALGA_DP     {0x00E8, 0x00F6, 0x0151, 0x01CB}
#define MOVES_PALKIA_DP     {0x0160, 0x00F6, 0x0151, 0x01CC}
#define MOVES_HEATRAN_DP    {0x00B8, 0x01B4, 0x0053, 0x01BA}
#define MOVES_REGIGIGAS_DP  {0x006D, 0x0017, 0x0114, 0x01AC}

#define MOVES_GIRATINA_PT   {0x01D2, 0x00F6, 0x0151, 0x01D3}
#define MOVES_UXIE_PT       {0x0081, 0x0119, 0x00F8, 0x0085}
#define MOVES_AZELF_PT      {0x0081, 0x00FD, 0x00F8, 0x01A1}
#define MOVES_DIALGA_PT     {0x01CB, 0x0179, 0x019E, 0x00A3}
#define MOVES_PALKIA_PT     {0x01CC, 0x0179, 0x019E, 0x00A3}
#define MOVES_HEATRAN_PT    {0x013F, 0x00F2, 0x00B8, 0x01B4}
#define MOVES_REGIGIGAS_PT  {0x0092, 0x011A, 0x006D, 0x00C1}

#define MOVES_ROTOM         {0x0054, 0x006D, 0x00FD, 0x0068} //DPPT

#define GFX_GIRATINA_DP     {0x3377, 0x1463, 0x9631, 0x7779, 0x3377, 0x1463, 0x7605, 0x7777}
#define GFX_ARCEUS_DP       {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7205, 0x6565}
#define GFX_SHAYMIN_DP      {0x6872, 0x6E61, 0x0061, 0x0000, 0x0000, 0x0000, 0x0005, 0x0000}
#define GFX_DARKRAI_DP      {0x0000, 0x0000, 0x0000, 0x0000, 0x3364, 0x735F, 0x3105, 0x625F}
#define GFX_CAVERN_DP       {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} //Uxie & Azelf
#define GFX_ROTOM_DP        {0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0100, 0x0005, 0x002C}
#define GFX_PILLARS_DP      {0x10E4, 0x0000, 0x0217, 0x4300, 0x4652, 0x0000, 0x9D05, 0x0000} //Dialga & Palkia
#define GFX_HEATRAN_DP      {0x30B2, 0x3030, 0x3030, 0x3030, 0x30B2, 0x3030, 0x3005, 0x3030}
#define GFX_REGIGIGAS_DP    {0x0290, 0x0000, 0x0290, 0x0000, 0x0000, 0x0000, 0x0005, 0x0000}

#define GFX_DISTORTION_PT   {0x0FFC, 0x1B99, 0x1A06, 0x0000, 0xFFA3, 0x0133, 0x2705, 0x2122} //Giratina-o
#define GFX_TURNBACK_PT     {0x0290, 0x0000, 0x0000, 0x0000, 0x0004, 0x0000, 0x0005, 0x0000} //Giratina-a
#define GFX_PILLARS_PT      {0x7570, 0x6464, 0x656C, 0x0070, 0x0000, 0x0000, 0x0005, 0x0000} //Dialga & Palkia
#define GFX_CAVERN_PT       {0x4444, 0x4444, 0x4444, 0x4444, 0x2234, 0x2222, 0x2205, 0x4322} //Uxie & Azelf
#define GFX_ROTOM_PT        {0x0000, 0x0005, 0xE000, 0xFA00, 0xFC00, 0x4000, 0x3A05, 0x0800}
#define GFX_REGIGIGAS_PT    {0x2022, 0x2226, 0x00A0, 0x0080, 0x5650, 0x0000, 0x0005, 0x0B7A}
#define GFX_HEATRAN_PT      {0x0004, 0x0008, 0x007C, 0x0101, 0x696B, 0x676E, 0x7405, 0x0061}

/* Indices of each version */
enum { VERSION_DIAMOND, VERSION_PEARL, VERSION_PLATINUM };

/* OGWILD wilds, Diamond */
enum { OGW_DP_GIRATINA, OGW_DP_ARCEUS, OGW_DP_DIALGA, OGW_DP_PALKIA = OGW_DP_DIALGA, OGW_DP_SHAYMIN, OGW_DP_DARKRAI, OGW_DP_UXIE, OGW_DP_AZELF, OGW_DP_ROTOM, OGW_DP_HEATRAN, OGW_DP_REGIGIGAS };

/* OGWILD wilds, Platinum */
enum { OGW_PT_GIRATINA_O, OGW_PT_GIRATINA_A, OGW_PT_DIALGA, OGW_PT_PALKIA, OGW_PT_UXIE, OGW_PT_AZELF, OGW_PT_ROTOM, OGW_PT_HEATRAN, OGW_PT_REGIGIGAS, OGW_PT_9, OGW_PT_10 };

/* Types */
enum {
    TYPE_NORMAL,
    TYPE_FIGHTING,
    TYPE_FLYING,
    TYPE_POISON,
    TYPE_GROUND,
    TYPE_ROCK,
    TYPE_BUG,
    TYPE_GHOST,
    TYPE_STEEL,
    TYPE_UNKNOWN, //??? type
    TYPE_FIRE,
    TYPE_WATER,
    TYPE_GRASS,
    TYPE_ELECTRIC,
    TYPE_PSYCHIC,
    TYPE_ICE,
    TYPE_DRAGON,
    TYPE_DARK
};

const u8 Types[TYPES_MAX][STRING_LENGTH_MAX] = { "Normal", "Fighting", "Flying", "Poison", "Ground", "Rock", "Bug", "Ghost", "Steel", "???", "Fire", "Water", "Grass", "Electric", "Psychic", "Ice", "Dragon", "Dark" };

/* Strings of the available game languages by index, 0 and 6 are dummies */
const u8 Languages[LANGUAGES_MAX][STRING_LENGTH_MAX] = { "LANG_0", "Japanese", "English", "French", "Italian", "German", "LANG_6", "Spanish", "Korean" };
const u8 Languages_short[LANGUAGES_MAX][3] = { "L0", "JP", "EN", "FR", "IT", "GE", "L6", "SP", "KO" };

/* Caution: need to translate from one index to the other */
const u8 LanguagesActual[LANGUAGES_ACT_MAX][STRING_LENGTH_MAX] = { "Japanese", "English", "French", "Italian", "German", "Spanish", "Korean" };

/* Strings of the game versions */
const u8 Versions[VERSIONS_MAX][STRING_LENGTH_MAX] = { "Diamond", "Pearl", "Platinum" };
const u8 Versions_short[VERSIONS_MAX][3] = { "D", "P", "Pt" };

/* Offset from the base pointer */
const u32 OppPartyOffBeg[VERSIONS_MAX - 1] = { 0x4C7B0, 0x4B884 }; //DP, Pt
const u32 OppPartyOffEnd[VERSIONS_MAX - 1] = { 0x4D310, 0x4C3E4 }; //DP, Pt

/* The 24 ABCD Block permutations with the first 8 repeated at the end for optimization purposes */
const u32 Perms[32] = { 0x03020100, 0x02030100, 0x03010200, 0x02010300, 0x01030200, 0x01020300, 0x03020001, 0x02030001, 0x03010002, 0x02010003, 0x01030002, 0x01020003, 0x03000201, 0x02000301, 0x03000102, 0x02000103, 0x01000302, 0x01000203, 0x00030201, 0x00020301, 0x00030102, 0x00020103, 0x00010302, 0x00010203, 0x03020100, 0x02030100, 0x03010200, 0x02010300, 0x01030200, 0x01020300, 0x03020001, 0x02030001 };

/* Table for bonus/malus in each stat depending on Nature, HP omitted */
const s8 NatureStatModifiers[NATURES_MAX][STATS_MAX - 1] = { {0, 0, 0, 0, 0}, {1, -1, 0, 0, 0}, {1, 0, -1, 0, 0}, {1, 0, 0, -1, 0}, {1, 0, 0, 0, -1}, {-1, 1, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 1, -1, 0, 0}, {0, 1, 0, -1, 0}, {0, 1, 0, 0, -1}, {-1, 0, 1, 0, 0}, {0, -1, 1, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 1, -1, 0}, {0, 0, 1, 0, -1}, {-1, 0, 0, 1, 0}, {0, -1, 0, 1, 0}, {0, 0, -1, 1, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 1, -1}, {-1, 0, 0, 0, 1}, {0, -1, 0, 0, 1}, {0, 0, -1, 0, 1}, {0, 0, 0, -1, 1}, {0, 0, 0, 0, 0} };

const u8 Natures[NATURES_MAX][8] = { "Hardy", "Lonely", "Brave", "Adamant", "Naughty", "Bold", "Docile", "Relaxed", "Impish", "Lax", "Timid", "Hasty", "Serious", "Jolly", "Naive", "Modest", "Mild", "Quiet", "Bashful", "Rash", "Calm", "Gentle", "Sassy", "Careful", "Quirky" };

/* Ordered strings of all abilities */
const u8 Abilities[ABILITIES_MAX][STRING_LENGTH_MAX] = { "Anything", "Stench", "Drizzle", "Speed Boost", "Battle Armor", "Sturdy", "Damp", "Limber", "Sand Veil", "Static", "Volt Absorb", "Water Absorb", "Oblivious", "Cloud Nine", "Compound Eyes", "Insomnia", "Color Change", "Immunity", "Flash Fire", "Shield Dust", "Own Tempo", "Suction Cups", "Intimidate", "Shadow Tag", "Rough Skin", "Wonder Guard", "Levitate", "Effect Spore", "Synchronize", "Clear Body", "Natural Cure", "Lightning Rod", "Serene Grace", "Swift Swim", "Chlorophyll", "Illuminate", "Trace", "Huge Power", "Poison Point", "Inner Focus", "Magma Armor", "Water Veil", "Magnet Pull", "Soundproof", "Rain Dish", "Sand Stream", "Pressure", "Thick Fat", "Early Bird", "Flame Body", "Run Away", "Keen Eye", "Hyper Cutter", "Pickup", "Truant", "Hustle", "Cute Charm", "Plus", "Minus", "Forecast", "Sticky Hold", "Shed Skin", "Guts", "Marvel Scale", "Liquid Ooze", "Overgrow", "Blaze", "Torrent", "Swarm", "Rock Head", "Drought", "Arena Trap", "Vital Spirit", "White Smoke", "Pure Power", "Shell Armor", "Air Lock", "Tangled Feet", "Motor Drive", "Rivalry", "Steadfast", "Snow Cloak", "Gluttony", "Anger Point", "Unburden", "Heatproof", "Simple", "Dry Skin", "Download", "Iron Fist", "Poison Heal", "Adaptability", "Skill Link", "Hydration", "Solar Power", "Quick Feet", "Normalize", "Sniper", "Magic Guard", "No Guard", "Stall", "Technician", "Leaf Guard", "Klutz", "Mold Breaker", "Super Luck", "Aftermath", "Anticipation", "Forewarn", "Unaware", "Tinted Lens", "Filter", "Slow Start", "Scrappy", "Storm Drain", "Ice Body", "Solid Rock", "Snow Warning", "Honey Gather", "Frisk", "Reckless", "Multitype", "Flower Gift", "Bad Dreams" };

/* Ordered strings of all species */
const u8 Pokelist[SPECIES_MAX][STRING_LENGTH_MAX] = { "Anything", "Bulbasaur", "Ivysaur", "Venusaur", "Charmander", "Charmeleon", "Charizard", "Squirtle", "Wartortle", "Blastoise", "Caterpie", "Metapod", "Butterfree", "Weedle", "Kakuna", "Beedrill", "Pidgey", "Pidgeotto", "Pidgeot", "Rattata", "Raticate", "Spearow", "Fearow", "Ekans", "Arbok", "Pikachu", "Raichu", "Sandshrew","Sandslash", "Nidoran-F", "Nidorina", "Nidoqueen", "Nidoran-M", "Nidorino", "Nidoking", "Clefairy", "Clefable", "Vulpix", "Ninetales", "Jigglypuff", "Wigglytuff", "Zubat", "Golbat", "Oddish", "Gloom", "Vileplume", "Paras", "Parasect", "Venonat", "Venomoth", "Diglett", "Dugtrio", "Meowth", "Persian", "Psyduck", "Golduck", "Mankey", "Primeape", "Growlithe", "Arcanine", "Poliwag", "Poliwhirl", "Poliwrath", "Abra", "Kadabra", "Alakazam", "Machop", "Machoke", "Machamp", "Bellsprout", "Weepinbell", "Victreebel", "Tentacool", "Tentacruel", "Geodude", "Graveler", "Golem", "Ponyta", "Rapidash", "Slowpoke", "Slowbro", "Magnemite", "Magneton", "Farfetch'd", "Doduo", "Dodrio", "Seel", "Dewgong", "Grimer", "Muk", "Shellder", "Cloyster", "Gastly", "Haunter", "Gengar", "Onix", "Drowzee", "Hypno", "Krabby", "Kingler", "Voltorb", "Electrode", "Exeggcute", "Exeggutor", "Cubone", "Marowak", "Hitmonlee", "Hitmonchan", "Lickitung", "Koffing", "Weezing", "Rhyhorn", "Rhydon", "Chansey", "Tangela", "Kangaskhan", "Horsea", "Seadra", "Goldeen", "Seaking", "Staryu", "Starmie", "Mr.Mime", "Scyther", "Jynx", "Electabuzz", "Magmar", "Pinsir", "Tauros", "Magikarp", "Gyarados", "Lapras", "Ditto", "Eevee", "Vaporeon", "Jolteon", "Flareon", "Porygon", "Omanyte", "Omastar", "Kabuto", "Kabutops", "Aerodactyl", "Snorlax", "Articuno", "Zapdos", "Moltres", "Dratini", "Dragonair", "Dragonite", "Mewtwo", "Mew", "Chikorita", "Bayleef", "Meganium", "Cyndaquil", "Quilava", "Typhlosion", "Totodile", "Croconaw", "Feraligatr", "Sentret", "Furret", "Hoothoot", "Noctowl", "Ledyba", "Ledian", "Spinarak", "Ariados", "Crobat", "Chinchou", "Lanturn", "Pichu", "Cleffa", "Igglybuff", "Togepi", "Togetic", "Natu", "Xatu", "Mareep", "Flaaffy", "Ampharos", "Bellossom", "Marill", "Azumarill", "Sudowoodo", "Politoed", "Hoppip", "Skiploom", "Jumpluff", "Aipom", "Sunkern", "Sunflora", "Yanma", "Wooper", "Quagsire", "Espeon", "Umbreon", "Murkrow", "Slowking", "Misdreavus", "Unown", "Wobbuffet", "Girafarig", "Pineco", "Forretress", "Dunsparce", "Gligar", "Steelix", "Snubbull", "Granbull", "Qwilfish", "Scizor", "Shuckle", "Heracross", "Sneasel", "Teddiursa", "Ursaring", "Slugma", "Magcargo", "Swinub", "Piloswine", "Corsola", "Remoraid", "Octillery", "Delibird", "Mantine", "Skarmory", "Houndour", "Houndoom", "Kingdra", "Phanpy", "Donphan", "Porygon-2", "Stantler", "Smeargle", "Tyrogue", "Hitmontop", "Smoochum", "Elekid", "Magby", "Miltank", "Blissey", "Raikou", "Entei", "Suicune", "Larvitar", "Pupitar", "Tyranitar", "Lugia", "Ho-Oh", "Celebi", "Treecko", "Grovyle", "Sceptile", "Torchic", "Combusken", "Blaziken", "Mudkip", "Marshtomp", "Swampert", "Poochyena", "Mightyena", "Zigzagoon", "Linoone", "Wurmple", "Silcoon", "Beautifly", "Cascoon", "Dustox", "Lotad", "Lombre", "Ludicolo", "Seedot", "Nuzleaf", "Shiftry", "Taillow", "Swellow", "Wingull", "Pelipper", "Ralts", "Kirlia", "Gardevoir", "Surskit", "Masquerain", "Shroomish", "Breloom", "Slakoth", "Vigoroth", "Slaking", "Nincada", "Ninjask", "Shedinja", "Whismur", "Loudred", "Exploud", "Makuhita", "Hariyama", "Azurill", "Nosepass", "Skitty", "Delcatty", "Sableye", "Mawile", "Aron", "Lairon", "Aggron", "Meditite", "Medicham", "Electrike", "Manectric", "Plusle", "Minun", "Volbeat", "Illumise", "Roselia", "Gulpin", "Swalot", "Carvanha", "Sharpedo", "Wailmer", "Wailord", "Numel", "Camerupt", "Torkoal", "Spoink", "Grumpig", "Spinda", "Trapinch", "Vibrava", "Flygon", "Cacnea", "Cacturne", "Swablu", "Altaria", "Zangoose", "Seviper", "Lunatone", "Solrock", "Barboach", "Whiscash", "Corphish", "Crawdaunt", "Baltoy", "Claydol", "Lileep", "Cradily", "Anorith", "Armaldo", "Feebas", "Milotic", "Castform", "Kecleon", "Shuppet", "Banette", "Duskull", "Dusclops", "Tropius", "Chimecho", "Absol", "Wynaut", "Snorunt", "Glalie", "Spheal", "Sealeo", "Walrein", "Clamperl", "Huntail", "Gorebyss", "Relicanth", "Luvdisc", "Bagon", "Shelgon", "Salamence", "Beldum", "Metang", "Metagross", "Regirock", "Regice", "Registeel", "Latias", "Latios", "Kyogre", "Groudon", "Rayquaza", "Jirachi", "Deoxys", "Turtwig", "Grotle", "Torterra", "Chimchar", "Monferno", "Infernape", "Piplup", "Prinplup", "Empoleon", "Starly", "Staravia", "Staraptor", "Bidoof", "Bibarel", "Kricketot", "Kricketune", "Shinx", "Luxio", "Luxray", "Budew", "Roserade", "Cranidos", "Rampardos", "Shieldon", "Bastiodon", "Burmy", "Wormadam", "Mothim", "Combee", "Vespiquen", "Pachirisu", "Buizel", "Floatzel", "Cherubi", "Cherrim", "Shellos", "Gastrodon", "Ambipom", "Drifloon", "Drifblim", "Buneary", "Lopunny", "Mismagius", "Honchkrow", "Glameow", "Purugly", "Chingling", "Stunky", "Skuntank", "Bronzor", "Bronzong", "Bonsly", "MimeJr.", "Happiny", "Chatot", "Spiritomb", "Gible", "Gabite", "Garchomp", "Munchlax", "Riolu", "Lucario", "Hippopotas", "Hippowdon", "Skorupi", "Drapion", "Croagunk", "Toxicroak", "Carnivine", "Finneon", "Lumineon", "Mantyke", "Snover", "Abomasnow", "Weavile", "Magnezone", "Lickilicky", "Rhyperior", "Tangrowth", "Electivire", "Magmortar", "Togekiss", "Yanmega", "Leafeon", "Glaceon", "Gliscor", "Mamoswine", "Porygon-Z", "Gallade", "Probopass", "Dusknoir", "Froslass", "Rotom", "Uxie", "Mesprit", "Azelf", "Dialga", "Palkia", "Heatran", "Regigigas", "Giratina", "Cresselia", "Phione", "Manaphy", "Darkrai", "Shaymin", "Arceus" };

/* Ordered strings of all items */
const u8 Items[ITEMS_MAX][STRING_LENGTH_MAX] = { "Anything", "Master Ball", "Ultra Ball", "Great Ball", "Poké Ball", "Safari Ball", "Net Ball", "Dive Ball", "Nest Ball", "Repeat Ball", "Timer Ball", "Luxury Ball", "Premier Ball", "Dusk Ball", "Heal Ball", "Quick Ball", "Cherish Ball", "Potion", "Antidote", "Burn Heal", "Ice Heal", "Awakening", "Paralyze Heal", "Full Restore", "Max Potion", "Hyper Potion", "Super Potion", "Full Heal", "Revive", "Max Revive", "Fresh Water", "Soda Pop", "Lemonade", "Moomoo Milk", "Energy Powder", "Energy Root", "Heal Powder", "Revival Herb", "Ether", "Max Ether", "Elixir", "Max Elixir", "Lava Cookie", "Berry Juice", "Sacred Ash", "HP Up", "Protein", "Iron", "Carbos", "Calcium", "Rare Candy", "PP Up", "Zinc", "PP Max", "Old Gateau", "Guard Spec", "Dire Hit", "X Attack", "X Defense", "X Speed", "X Accuracy", "X SpeAtk", "X SpeDef", "Poké Doll", "Fluffy Tail", "Blue Flute", "Yellow Flute", "Red Flute", "Black Flute", "White Flute", "Shoal Salt", "Shoal Shell", "Red Shard", "Blue Shard", "Yellow Shard", "Green Shard", "Super Repel", "Max Repel", "Escape Rope", "Repel", "Sun Stone", "Moon Stone", "Fire Stone", "Thunder Stone", "Water Stone", "Leaf Stone", "Tiny Mushroom", "Big Mushroom", "Pearl", "Big Pearl", "Stardust", "Star Piece", "Nugget", "Heart Scale", "Honey", "Growth Mulch", "Damp Mulch", "Stable Mulch", "Gooey Mulch", "Root Fossil", "Claw Fossil", "Helix Fossil", "Dome Fossil", "Old Amber", "Armor Fossil", "Skull Fossil", "Rare Bone", "Shiny Stone", "Dusk Stone", "Dawn Stone", "Oval Stone", "Odd Keystone", "Griseous Orb", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Adamant Orb", "Lustrous Orb", "Grass Mail", "Flame Mail", "Bubble Mail", "Bloom Mail", "Tunnel Mail", "Steel Mail", "Heart Mail", "Snow Mail", "Space Mail", "Air Mail", "Mosaic Mail", "Brick Mail", "Cheri Berry", "Chesto Berry", "Pecha Berry", "Rawst Berry", "Aspear Berry", "Leppa Berry", "Oran Berry", "Persim Berry", "Lum Berry", "Sitrus Berry", "Figy Berry", "Wiki Berry", "Mago Berry", "Aguav Berry", "Iapapa Berry", "Razz Berry", "Bluk Berry", "Nanab Berry", "Wepear Berry", "Pinap Berry", "Pomeg Berry", "Kelpsy Berry", "Qualot Berry", "Hondew Berry", "Grepa Berry", "Tamato Berry", "Cornn Berry", "Magost Berry", "Rabuta Berry", "Nomel Berry", "Spelon Berry", "Pamtre Berry", "Watmel Berry", "Durin Berry", "Belue Berry", "Occa Berry", "Passho Berry", "Wacan Berry", "Rindo Berry", "Yache Berry", "Chople Berry", "Kebia Berry", "Shuca Berry", "Coba Berry", "Payapa Berry", "Tanga Berry", "Charti Berry", "Kasib Berry", "Haban Berry", "Colbur Berry", "Babiri Berry", "Chilan Berry", "Liechi Berry", "Ganlon Berry", "Salac Berry", "Petaya Berry", "Apicot Berry", "Lansat Berry", "Starf Berry", "Enigma Berry", "Micle Berry", "Custap Berry", "Jaboca Berry", "Rowap Berry", "Bright Powder", "White Herb", "Macho Brace", "Exp Share", "Quick Claw", "Soothe Bell", "Mental Herb", "Choice Band", "King's Rock", "Silver Powder", "Amulet Coin", "Cleanse Tag", "Soul Dew", "Deep Sea Tooth", "Deep Sea Scale", "Smoke Ball", "Everstone", "Focus Band", "Lucky Egg", "Scope Lens", "Metal Coat", "Leftovers", "Dragon Scale", "Light Ball", "Soft Sand", "Hard Stone", "Miracle Seed", "Black Glasses", "Black Belt", "Magnet", "Mystic Water", "Sharp Beak", "Poison Barb", "Never-Melt Ice", "Spell Tag", "Twisted Spoon", "Charcoal", "Dragon Fang", "Silk Scarf", "Up-Grade", "Shell Bell", "Sea Incense", "Lax Incense", "Lucky Punch", "Metal Powder", "Thick Club", "Stick", "Red Scarf", "Blue Scarf", "Pink Scarf", "Green Scarf", "Yellow Scarf", "Wide Lens", "Muscle Band", "Wise Glasses", "Expert Belt", "Light Clay", "Life Orb", "Power Herb", "Toxic Orb", "Flame Orb", "Quick Powder", "Focus Sash", "Zoom Lens", "Metronome", "Iron Ball", "Lagging Tail", "Destiny Knot", "Black Sludge", "Icy Rock", "Smooth Rock", "Heat Rock", "Damp Rock", "Grip Claw", "Choice Scarf", "Sticky Barb", "Power Bracer", "Power Belt", "Power Lens", "Power Band", "Power Anklet", "Power Weight", "Shed Shell", "Big Root", "Choice Specs", "Flame Plate", "Splash Plate", "Zap Plate", "Meadow Plate", "Icicle Plate", "Fist Plate", "Toxic Plate", "Earth Plate", "Sky Plate", "Mind Plate", "Insect Plate", "Stone Plate", "Spooky Plate", "Draco Plate", "Dread Plate", "Iron Plate", "Odd Incense", "Rock Incense", "Full Incense", "Wave Incense", "Rose Incense", "Luck Incense", "Pure Incense", "Protector", "Electirizer", "Magmarizer", "Dubious Disc", "Reaper Cloth", "Razor Claw", "Razor Fang", "TM01", "TM02", "TM03", "TM04", "TM05", "TM06", "TM07", "TM08", "TM09", "TM10", "TM11", "TM12", "TM13", "TM14", "TM15", "TM16", "TM17", "TM18", "TM19", "TM20", "TM21", "TM22", "TM23", "TM24", "TM25", "TM26", "TM27", "TM28", "TM29", "TM30", "TM31", "TM32", "TM33", "TM34", "TM35", "TM36", "TM37", "TM38", "TM39", "TM40", "TM41", "TM42", "TM43", "TM44", "TM45", "TM46", "TM47", "TM48", "TM49", "TM50", "TM51", "TM52", "TM53", "TM54", "TM55", "TM56", "TM57", "TM58", "TM59", "TM60", "TM61", "TM62", "TM63", "TM64", "TM65", "TM66", "TM67", "TM68", "TM69", "TM70", "TM71", "TM72", "TM73", "TM74", "TM75", "TM76", "TM77", "TM78", "TM79", "TM80", "TM81", "TM82", "TM83", "TM84", "TM85", "TM86", "TM87", "TM88", "TM89", "TM90", "TM91", "TM92", "HM01", "HM02", "HM03", "HM04", "HM05", "HM06", "HM07", "HM08", "Explorer Kit", "Loot Sack", "Rule Book", "Poké Radar", "Point Card", "Journal", "Seal Case", "Fashion Case", "Seal", "Pal Pad", "Works Key", "Old Charm", "Galactic Key", "Red Chain", "Town Map", "VS Seeker", "Coin Case", "Old Rod", "Good Rod", "Super Rod", "Sprayduck", "Poffin Case", "Bicycle", "Suite Key", "Oak's Letter", "Lunar Wing", "Member Card", "Azure Flute", "S.S. Ticket", "Contest Pass", "Magma Stone", "Parcel", "Coupon 1", "Coupon 2", "Coupon 3", "Storage Key", "Secret Potion", "VS Recorder", "Gracidea", "Secret Key" };

/* Ordered strings of all moves */
const u8 Moves[MOVES_MAX][STRING_LENGTH_MAX] = { "Anything" , "Pound", "Karate Chop", "Double Slap", "Comet Punch", "Mega Punch", "Pay Day", "Fire Punch", "Ice Punch", "Thunder Punch", "Scratch", "Vice Grip", "Guillotine", "Razor Wind", "Swords Dance", "Cut", "Gust", "Wing Attack", "Whirlwind", "Fly", "Bind", "Slam", "Vine Whip", "Stomp", "Double Kick", "Mega Kick", "Jump Kick", "Rolling Kick", "Sand Attack", "Headbutt", "Horn Attack", "Fury Attack", "Horn Drill", "Tackle", "Body Slam", "Wrap", "Take Down", "Thrash", "Double-Edge", "Tail Whip", "Poison Sting", "Twineedle", "Pin Missile", "Leer", "Bite", "Growl", "Roar", "Sing", "Supersonic", "Sonic Boom", "Disable", "Acid", "Ember", "Flamethrower", "Mist", "Water Gun", "Hydro Pump", "Surf", "Ice Beam", "Blizzard", "Psybeam", "Bubble Beam", "Aurora Beam", "Hyper Beam", "Peck", "Drill Peck", "Submission", "Low Kick", "Counter", "Seismic Toss", "Strength", "Absorb", "Mega Drain", "Leech Seed", "Growth", "Razor Leaf", "Solar Beam", "Poison Powder", "Stun Spore", "Sleep Powder", "Petal Dance", "String Shot", "Dragon Rage", "Fire Spin", "Thunder Shock", "Thunderbolt", "Thunder Wave", "Thunder", "Rock Throw", "Earthquake", "Fissure", "Dig", "Toxic", "Confusion", "Psychic", "Hypnosis", "Meditate", "Agility", "Quick Attack", "Rage", "Teleport", "Night Shade", "Mimic", "Screech", "Double Team", "Recover", "Harden", "Minimize", "Smokescreen", "Confuse Ray", "Withdraw", "Defense Curl", "Barrier", "Light Screen", "Haze", "Reflect", "Focus Energy", "Bide", "Metronome", "Mirror Move", "Self-Destruct", "Egg Bomb", "Lick", "Smog", "Sludge", "Bone Club", "Fire Blast", "Waterfall", "Clamp", "Swift", "Skull Bash", "Spike Cannon", "Constrict", "Amnesia", "Kinesis", "Soft-Boiled", "High Jump Kick", "Glare", "Dream Eater", "Poison Gas", "Barrage", "Leech Life", "Lovely Kiss", "Sky Attack", "Transform", "Bubble", "Dizzy Punch", "Spore", "Flash", "Psywave", "Splash", "Acid Armor", "Crabhammer", "Explosion", "Fury Swipes", "Bonemerang", "Rest", "Rock Slide", "Hyper Fang", "Sharpen", "Conversion", "Tri Attack", "Super Fang", "Slash", "Substitute", "Struggle", "Sketch", "Triple Kick", "Thief", "Spider Web", "Mind Reader", "Nightmare", "Flame Wheel", "Snore", "Curse", "Flail", "Conversion 2", "Aeroblast", "Cotton Spore", "Reversal", "Spite", "Powder Snow", "Protect", "Mach Punch", "Scary Face", "Feint Attack", "Sweet Kiss", "Belly Drum", "Sludge Bomb", "Mud-Slap", "Octazooka", "Spikes", "Zap Cannon", "Foresight", "Destiny Bond", "Perish Song", "Icy Wind", "Detect", "Bone Rush", "Lock-On", "Outrage", "Sandstorm", "Giga Drain", "Endure", "Charm", "Rollout", "False Swipe", "Swagger", "Milk Drink", "Spark", "Fury Cutter", "Steel Wing", "Mean Look", "Attract", "Sleep Talk", "Heal Bell", "Return", "Present", "Frustration", "Safeguard", "Pain Split", "Sacred Fire", "Magnitude", "Dynamic Punch", "Megahorn", "Dragon Breath", "Baton Pass", "Encore", "Pursuit", "Rapid Spin", "Sweet Scent", "Iron Tail", "Metal Claw", "Vital Throw", "Morning Sun", "Synthesis", "Moonlight", "Hidden Power", "Cross Chop", "Twister", "Rain Dance", "Sunny Day", "Crunch", "Mirror Coat", "Psych Up", "Extreme Speed", "Ancient Power", "Shadow Ball", "Future Sight", "Rock Smash", "Whirlpool", "Beat Up", "Fake Out", "Uproar", "Stockpile", "Spit Up", "Swallow", "Heat Wave", "Hail", "Torment", "Flatter", "Will-O-Wisp", "Memento", "Facade", "Focus Punch", "Smelling Salts", "Follow Me", "Nature Power", "Charge", "Taunt", "Helping Hand", "Trick", "Role Play", "Wish", "Assist", "Ingrain", "Superpower", "Magic Coat", "Recycle", "Revenge", "Brick Break", "Yawn", "Knock Off", "Endeavor", "Eruption", "Skill Swap", "Imprison", "Refresh", "Grudge", "Snatch", "Secret Power", "Dive", "Arm Thrust", "Camouflage", "Tail Glow", "Luster Purge", "Mist Ball", "Feather Dance", "Teeter Dance", "Blaze Kick", "Mud Sport", "Ice Ball", "Needle Arm", "Slack Off", "Hyper Voice", "Poison Fang", "Crush Claw", "Blast Burn", "Hydro Cannon", "Meteor Mash", "Astonish", "Weather Ball", "Aromatherapy", "Fake Tears", "Air Cutter", "Overheat", "Odor Sleuth", "Rock Tomb", "Silver Wind", "Metal Sound", "Grass Whistle", "Tickle", "Cosmic Power", "Water Spout", "Signal Beam", "Shadow Punch", "Extrasensory", "Sky Uppercut", "Sand Tomb", "Sheer Cold", "Muddy Water", "Bullet Seed", "Aerial Ace", "Icicle Spear", "Iron Defense", "Block", "Howl", "Dragon Claw", "Frenzy Plant", "Bulk Up", "Bounce", "Mud Shot", "Poison Tail", "Covet", "Volt Tackle", "Magical Leaf", "Water Sport", "Calm Mind", "Leaf Blade", "Dragon Dance", "Rock Blast", "Shock Wave", "Water Pulse", "Doom Desire", "Psycho Boost", "Roost", "Gravity", "Miracle Eye", "Wake-Up Slap", "Hammer Arm", "Gyro Ball", "Healing Wish", "Brine", "Natural Gift", "Feint", "Pluck", "Tailwind", "Acupressure", "Metal Burst", "U-turn", "Close Combat", "Payback", "Assurance", "Embargo", "Fling", "Psycho Shift", "Trump Card", "Heal Block", "Wring Out", "Power Trick", "Gastro Acid", "Lucky Chant", "Me First", "Copycat", "Power Swap", "Guard Swap", "Punishment", "Last Resort", "Worry Seed", "Sucker Punch", "Toxic Spikes", "Heart Swap", "Aqua Ring", "Magnet Rise", "Flare Blitz", "Force Palm", "Aura Sphere", "Rock Polish", "Poison Jab", "Dark Pulse", "Night Slash", "Aqua Tail", "Seed Bomb", "Air Slash", "X-Scissor", "Bug Buzz", "Dragon Pulse", "Dragon Rush", "Power Gem", "Drain Punch", "Vacuum Wave", "Focus Blast", "Energy Ball", "Brave Bird", "Earth Power", "Switcheroo", "Giga Impact", "Nasty Plot", "Bullet Punch", "Avalanche", "Ice Shard", "Shadow Claw", "Thunder Fang", "Ice Fang", "Fire Fang", "Shadow Sneak", "Mud Bomb", "Psycho Cut", "Zen Headbutt", "Mirror Shot", "Flash Cannon", "Rock Climb", "Defog", "Trick Room", "Draco Meteor", "Discharge", "Lava Plume", "Leaf Storm", "Power Whip", "Rock Wrecker", "Cross Poison", "Gunk Shot", "Iron Head", "Magnet Bomb", "Stone Edge", "Captivate", "Stealth Rock", "Grass Knot", "Chatter", "Judgment", "Bug Bite", "Charge Beam", "Wood Hammer", "Aqua Jet", "Attack Order", "Defend Order", "Heal Order", "Head Smash", "Double Hit", "Roar of Time", "Spacial Rend", "Lunar Dance", "Crush Grip", "Magma Storm", "Dark Void", "Seed Flare", "Ominous Wind", "Shadow Force" };

/* Original wild Pokemon names */
const u8 OgWilds[VERSIONS_MAX][OG_WILDS_MAX][STRING_LENGTH_MAX] = {
    { "Giratina", "Arceus", "Dialga", "Shaymin", "Darkrai", "Uxie", "Azelf", "Rotom", "Heatran", "Regigigas" }, //Diamond
    { "Giratina", "Arceus", "Palkia", "Shaymin", "Darkrai", "Uxie", "Azelf", "Rotom", "Heatran", "Regigigas" }, //Pearl
    { "Giratina-O", "Giratina-A", "Dialga", "Palkia", "Uxie", "Azelf", "Rotom", "Heatran", "Regigigas", "OGW_PT_9", } //Platinum
};

typedef struct {
    u8 pos_a;
    u8 pos_b;
    u8 pos_c;
    u8 pos_d;
    u8 nature;
    u8 ivs[STATS_MAX];
    u32 iv32;
    //Main pkmn data; ordered the same as in game to simulate the buffer overflow
    u32 pid;
    u16 bef;
    u16 checksum;
    u16 data[BLOCKS][BLOCK_SIZE];
    u16 cond[COND_SIZE_S];
    //Size: 200 bytes
} PKMN;

/* The original wild PKMN's unique characteristics */
typedef struct {
    u16 species;
    u16 item;
    u16 level;
    u16 xp1;
    u16 xp2;
    u16 frab; //friendship and ability concatenated
    u16 moves[OWN_MOVES_MAX];
    u16 pp1and2;
    u16 pp3and4;
    u16 name[NAME_CHARS_MAX];
    u16 gfx[8]; //remains of graphical data from the current map
    //Size: 74 bytes
} OGWILD;

typedef struct {

    double time; //completion time
    u32 results; //number of results found
    u8 path[PATH_REL_LENGTH_MAX]; //relative path of the results file

    u32 seed[SEARCH_THREADS_MAX]; //1 seed per thread
    u32 frames[SEARCH_THREADS_MAX]; //number of frames searched per thread
    u32 progress[SEARCH_THREADS_MAX];

    u32 aslr;
    const OGWILD* pOgWild;
    u16 w_language;
    u16 w_version;
    u8 alt_form;
    u8 grouped_version;
    //Size: 192 bytes
} SEARCHDATA;

typedef struct {
    u32 seed; //original seed
    u32 reversed; //reversed seed
    u32 frames; //distance between seed and reversed
    //Size: 12 bytes
} REVERSEDSEED;

typedef struct {
    u8 type;
    u8 power;
    //Size: 2 bytes
} HIDDENPOWER;

/* Every static encounters */
const OGWILD dp_giratina = { 0x01E7, 0, 70, 0x8ACE, 0x0006, 0x2E00, MOVES_GIRATINA_DP, 0x0F05, 0x140A, NAME_GIRATINA, GFX_GIRATINA_DP };
const OGWILD dp_arceus = { 0x01ED, 0, 80, 0xC400, 0x0009, 0x7900, MOVES_ARCEUS_DP, 0x0F14, 0x050A, NAME_ARCEUS, GFX_ARCEUS_DP };
const OGWILD dp_shaymin = { 0x01EC, 0x009D, 30, 0x5500, 0x0000, 0x1E64, MOVES_SHAYMIN_DP, 0x1428, 0x050A, NAME_SHAYMIN, GFX_SHAYMIN_DP };
const OGWILD dp_darkrai = { 0x01EB, 0, 40, 0x3880, 0x0001, 0x7B00, MOVES_DARKRAI_DP, 0x141E, 0x0F14, NAME_DARKRAI, GFX_DARKRAI_DP };
const OGWILD dp_uxie = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_DP, 0x0A19, 0x140F, NAME_UXIE, GFX_CAVERN_DP };
const OGWILD dp_azelf = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_DP, 0x0A19, 0x140F, NAME_AZELF, GFX_CAVERN_DP };
const OGWILD dp_rotom = { 0x01DF, 0, 15, 0x0D2F, 0x0000, 0x1A46, MOVES_ROTOM, 0x0A1E, 0x0F0A, NAME_ROTOM, GFX_ROTOM_DP };
const OGWILD dp_uxie_fr = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_DP, 0x0A19, 0x140F, NAME_UXIE_FR, GFX_CAVERN_DP };
const OGWILD dp_azelf_fr = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_DP, 0x0A19, 0x140F, NAME_AZELF_FR, GFX_CAVERN_DP };
const OGWILD dp_motisma = { 0x01DF, 0, 15, 0x0D2F, 0x0000, 0x1A46, MOVES_ROTOM, 0x0A1E, 0x0F0A, NAME_ROTOM_FR, GFX_ROTOM_DP };
const OGWILD dp_uxie_ge = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_DP, 0x0A19, 0x140F, NAME_UXIE_GE, GFX_CAVERN_DP };
const OGWILD dp_azelf_ge = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_DP, 0x0A19, 0x140F, NAME_AZELF_GE, GFX_CAVERN_DP };
const OGWILD dp_giratina_jp = { 0x01E7, 0, 70, 0x8ACE, 0x0006, 0x2E00, MOVES_GIRATINA_DP, 0x0F05, 0x140A, NAME_GIRATINA_JP, GFX_GIRATINA_DP };
const OGWILD dp_arceus_jp = { 0x01ED, 0, 80, 0xC400, 0x0009, 0x7900, MOVES_ARCEUS_DP, 0x0F14, 0x050A, NAME_ARCEUS_JP, GFX_ARCEUS_DP };
const OGWILD dp_shaymin_jp = { 0x01EC, 0x009D, 30, 0x5500, 0x0000, 0x1E64, MOVES_SHAYMIN_DP, 0x1428, 0x050A, NAME_SHAYMIN_JP, GFX_SHAYMIN_DP };
const OGWILD dp_darkrai_jp = { 0x01EB, 0, 40, 0x3880, 0x0001, 0x7B00, MOVES_DARKRAI_DP, 0x141E, 0x0F14, NAME_DARKRAI_JP, GFX_DARKRAI_DP };
const OGWILD dp_uxie_jp = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_DP, 0x0A19, 0x140F, NAME_UXIE_JP, GFX_CAVERN_DP };
const OGWILD dp_azelf_jp = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_DP, 0x0A19, 0x140F, NAME_AZELF_JP, GFX_CAVERN_DP };
const OGWILD dp_rotom_jp = { 0x01DF, 0, 15, 0x0D2F, 0x0000, 0x1A46, MOVES_ROTOM, 0x0A1E, 0x0F0A, NAME_ROTOM_JP, GFX_ROTOM_DP };
const OGWILD dp_giratina_ko = { 0x01E7, 0, 70, 0x8ACE, 0x0006, 0x2E00, MOVES_GIRATINA_DP, 0x0F05, 0x140A, NAME_GIRATINA_KO, GFX_GIRATINA_DP };
const OGWILD dp_arceus_ko = { 0x01ED, 0, 80, 0xC400, 0x0009, 0x7900, MOVES_ARCEUS_DP, 0x0F14, 0x050A, NAME_ARCEUS_KO, GFX_ARCEUS_DP };
const OGWILD dp_shaymin_ko = { 0x01EC, 0x009D, 30, 0x5500, 0x0000, 0x1E64, MOVES_SHAYMIN_DP, 0x1428, 0x050A, NAME_SHAYMIN_KO, GFX_SHAYMIN_DP };
const OGWILD dp_darkrai_ko = { 0x01EB, 0, 40, 0x3880, 0x0001, 0x7B00, MOVES_DARKRAI_DP, 0x141E, 0x0F14, NAME_DARKRAI_KO, GFX_DARKRAI_DP };
const OGWILD dp_uxie_ko = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_DP, 0x0A19, 0x140F, NAME_UXIE_KO, GFX_CAVERN_DP };
const OGWILD dp_azelf_ko = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_DP, 0x0A19, 0x140F, NAME_AZELF_KO, GFX_CAVERN_DP };
const OGWILD dp_rotom_ko = { 0x01DF, 0, 15, 0x0D2F, 0x0000, 0x1A46, MOVES_ROTOM, 0x0A1E, 0x0F0A, NAME_ROTOM_KO, GFX_ROTOM_DP };
const OGWILD dp_dialga = { 0x01E3, 0, 47, 0xFAF2, 0x0001, 0x2E00, MOVES_DIALGA_DP, 0x0523, 0x050F, NAME_DIALGA, GFX_PILLARS_DP };
const OGWILD dp_palkia = { 0x01E4, 0, 47, 0xFAF2, 0x0001, 0x2E00, MOVES_PALKIA_DP, 0x0514, 0x050F, NAME_PALKIA, GFX_PILLARS_DP };
const OGWILD dp_dialga_jp = { 0x01E3, 0, 47, 0xFAF2, 0x0001, 0x2E00, MOVES_DIALGA_DP, 0x0523, 0x050F, NAME_DIALGA_JP, GFX_PILLARS_DP };
const OGWILD dp_palkia_jp = { 0x01E4, 0, 47, 0xFAF2, 0x0001, 0x2E00, MOVES_PALKIA_DP, 0x0514, 0x050F, NAME_PALKIA_JP, GFX_PILLARS_DP };
const OGWILD dp_dialga_ko = { 0x01E3, 0, 47, 0xFAF2, 0x0001, 0x2E00, MOVES_DIALGA_DP, 0x0523, 0x050F, NAME_DIALGA_KO, GFX_PILLARS_DP };
const OGWILD dp_palkia_ko = { 0x01E4, 0, 47, 0xFAF2, 0x0001, 0x2E00, MOVES_PALKIA_DP, 0x0514, 0x050F, NAME_PALKIA_KO, GFX_PILLARS_DP };
const OGWILD dp_heatran = { 0x01E5, 0, 70, 0x8ACE, 0x0006, 0x1264, MOVES_HEATRAN_DP, 0x0F0A, 0x0F0F, NAME_HEATRAN, GFX_HEATRAN_DP };
const OGWILD dp_heatran_jp = { 0x01E5, 0, 70, 0x8ACE, 0x0006, 0x1264, MOVES_HEATRAN_DP, 0x0F0A, 0x0F0F, NAME_HEATRAN_JP, GFX_HEATRAN_DP };
const OGWILD dp_heatran_ko = { 0x01E5, 0, 70, 0x8ACE, 0x0006, 0x1264, MOVES_HEATRAN_DP, 0x0F0A, 0x0F0F, NAME_HEATRAN_KO, GFX_HEATRAN_DP };
const OGWILD dp_regigigas = { 0x01E6, 0, 70, 0x8ACE, 0x0006, 0x7000, MOVES_REGIGIGAS_DP, 0x140A, 0x0F05, NAME_REGIGIGAS, GFX_REGIGIGAS_DP };
const OGWILD dp_regigigas_jp = { 0x01E6, 0, 70, 0x8ACE, 0x0006, 0x7000, MOVES_REGIGIGAS_DP, 0x140A, 0x0F05, NAME_REGIGIGAS_JP, GFX_REGIGIGAS_DP };
const OGWILD dp_regigigas_ko = { 0x01E6, 0, 70, 0x8ACE, 0x0006, 0x7000, MOVES_REGIGIGAS_DP, 0x140A, 0x0F05, NAME_REGIGIGAS_KO, GFX_REGIGIGAS_DP };

const OGWILD pt_giratina_o = { 0x01E7, 0, 47, 0xFAF2, 0x0001, 0x1A00, MOVES_GIRATINA_PT, 0x0505, 0x050F, NAME_GIRATINA, GFX_DISTORTION_PT }; //origin
const OGWILD pt_giratina_a = { 0x01E7, 0, 47, 0xFAF2, 0x0001, 0x2E00, MOVES_GIRATINA_PT, 0x0505, 0x050F, NAME_GIRATINA, GFX_TURNBACK_PT }; //altered
const OGWILD pt_dialga = { 0x01E3, 0, 70, 0x8ACE, 0x0006, 0x2E00, MOVES_DIALGA_PT, 0x0F05, 0x140A, NAME_DIALGA, GFX_PILLARS_PT };
const OGWILD pt_palkia = { 0x01E4, 0, 70, 0x8ACE, 0x0006, 0x2E00, MOVES_PALKIA_PT, 0x0F05, 0x140A, NAME_PALKIA, GFX_PILLARS_PT };
const OGWILD pt_uxie = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_PT, 0x0A14, 0x140F, NAME_UXIE, GFX_CAVERN_PT };
const OGWILD pt_azelf = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_PT, 0x0A14, 0x140F, NAME_AZELF, GFX_CAVERN_PT };
const OGWILD pt_rotom = { 0x01DF, 0, 20, 0x1F40, 0x0000, 0x1A46, MOVES_ROTOM, 0x0A1E, 0x0F0A, NAME_ROTOM, GFX_ROTOM_PT };
const OGWILD pt_uxie_fr = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_PT, 0x0A14, 0x140F, NAME_UXIE_FR, GFX_CAVERN_PT };
const OGWILD pt_azelf_fr = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_PT, 0x0A14, 0x140F, NAME_AZELF_FR, GFX_CAVERN_PT };
const OGWILD pt_rotom_fr = { 0x01DF, 0, 20, 0x1F40, 0x0000, 0x1A46, MOVES_ROTOM, 0x0A1E, 0x0F0A, NAME_ROTOM_FR, GFX_ROTOM_PT };
const OGWILD pt_uxie_ge = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_PT, 0x0A14, 0x140F, NAME_UXIE_GE, GFX_CAVERN_PT };
const OGWILD pt_azelf_ge = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_PT, 0x0A14, 0x140F, NAME_AZELF_GE, GFX_CAVERN_PT };
const OGWILD pt_giratina_o_jp = { 0x01E7, 0, 47, 0xFAF2, 0x0001, 0x1A00, MOVES_GIRATINA_PT, 0x0505, 0x050F, NAME_GIRATINA_JP, GFX_DISTORTION_PT }; //origin
const OGWILD pt_giratina_a_jp = { 0x01E7, 0, 47, 0xFAF2, 0x0001, 0x1A00, MOVES_GIRATINA_PT, 0x0505, 0x050F, NAME_GIRATINA_JP, GFX_TURNBACK_PT }; //altered
const OGWILD pt_dialga_jp = { 0x01E3, 0, 70, 0x8ACE, 0x0006, 0x2E00, MOVES_DIALGA_PT, 0x0F05, 0x140A, NAME_DIALGA_JP, GFX_PILLARS_PT };
const OGWILD pt_palkia_jp = { 0x01E4, 0, 70, 0x8ACE, 0x0006, 0x2E00, MOVES_PALKIA_PT, 0x0F05, 0x140A, NAME_PALKIA_JP, GFX_PILLARS_PT };
const OGWILD pt_uxie_jp = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_PT, 0x0A14, 0x140F, NAME_UXIE_JP, GFX_CAVERN_PT };
const OGWILD pt_azelf_jp = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_PT, 0x0A14, 0x140F, NAME_AZELF_JP, GFX_CAVERN_PT };
const OGWILD pt_rotom_jp = { 0x01DF, 0, 20, 0x1F40, 0x0000, 0x1A46, MOVES_ROTOM, 0x0A1E, 0x0F0A, NAME_ROTOM_JP, GFX_ROTOM_PT };
const OGWILD pt_giratina_o_ko = { 0x01E7, 0, 47, 0xFAF2, 0x0001, 0x1A00, MOVES_GIRATINA_PT, 0x0505, 0x050F, NAME_GIRATINA_KO, GFX_DISTORTION_PT }; //origin
const OGWILD pt_giratina_a_ko = { 0x01E7, 0, 47, 0xFAF2, 0x0001, 0x1A00, MOVES_GIRATINA_PT, 0x0505, 0x050F, NAME_GIRATINA_KO, GFX_TURNBACK_PT }; //altered
const OGWILD pt_dialga_ko = { 0x01E3, 0, 70, 0x8ACE, 0x0006, 0x2E00, MOVES_DIALGA_PT, 0x0F05, 0x140A, NAME_DIALGA_KO, GFX_PILLARS_PT };
const OGWILD pt_palkia_ko = { 0x01E4, 0, 70, 0x8ACE, 0x0006, 0x2E00, MOVES_PALKIA_PT, 0x0F05, 0x140A, NAME_PALKIA_KO, GFX_PILLARS_PT };
const OGWILD pt_uxie_ko = { 0x01E0, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_UXIE_PT, 0x0A14, 0x140F, NAME_UXIE_KO, GFX_CAVERN_PT };
const OGWILD pt_azelf_ko = { 0x01E2, 0, 50, 0x625A, 0x0002, 0x1A8C, MOVES_AZELF_PT, 0x0A14, 0x140F, NAME_AZELF_KO, GFX_CAVERN_PT };
const OGWILD pt_rotom_ko = { 0x01DF, 0, 20, 0x1F40, 0x0000, 0x1A46, MOVES_ROTOM, 0x0A1E, 0x0F0A, NAME_ROTOM_KO, GFX_ROTOM_PT };
const OGWILD pt_heatran = { 0x01E5, 0, 50, 0x625A, 0x0002, 0x1264, MOVES_HEATRAN_PT, 0x0F28, 0x0F0A, NAME_HEATRAN, GFX_HEATRAN_PT };
const OGWILD pt_heatran_jp = { 0x01E5, 0, 50, 0x625A, 0x0002, 0x1264, MOVES_HEATRAN_PT, 0x0F28, 0x0F0A, NAME_HEATRAN_JP, GFX_HEATRAN_PT };
const OGWILD pt_heatran_ko = { 0x01E5, 0, 50, 0x625A, 0x0002, 0x1264, MOVES_HEATRAN_PT, 0x0F28, 0x0F0A, NAME_HEATRAN_KO, GFX_HEATRAN_PT };
const OGWILD pt_regigigas = { 0x01E6, 0, 1, 0x0000, 0x0000, 0x7000, MOVES_REGIGIGAS_PT, 0x140A, 0x280A, NAME_REGIGIGAS, GFX_REGIGIGAS_PT };
const OGWILD pt_regigigas_jp = { 0x01E6, 0, 1, 0x0000, 0x0000, 0x7000, MOVES_REGIGIGAS_PT, 0x140A, 0x280A, NAME_REGIGIGAS_JP, GFX_REGIGIGAS_PT };
const OGWILD pt_regigigas_ko = { 0x01E6, 0, 1, 0x0000, 0x0000, 0x7000, MOVES_REGIGIGAS_PT, 0x140A, 0x280A, NAME_REGIGIGAS_KO, GFX_REGIGIGAS_PT };


const OGWILD* OGW_LangVers[LANGUAGES_MAX][VERSIONS_MAX][OG_WILDS_MAX] = {
    { //_0
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
    { //jp
        {&dp_giratina_jp, &dp_arceus_jp, &dp_dialga_jp, &dp_shaymin_jp, &dp_darkrai_jp, &dp_uxie_jp, &dp_azelf_jp, &dp_rotom_jp, &dp_heatran_jp, &dp_regigigas_jp},
        {&dp_giratina_jp, &dp_arceus_jp, &dp_palkia_jp, &dp_shaymin_jp, &dp_darkrai_jp, &dp_uxie_jp, &dp_azelf_jp, &dp_rotom_jp, &dp_heatran_jp, &dp_regigigas_jp},
        {&pt_giratina_o_jp, &pt_giratina_a_jp, &pt_dialga_jp, &pt_palkia_jp, &pt_uxie_jp, &pt_azelf_jp, &pt_rotom_jp, &pt_heatran_jp, &pt_regigigas_jp, NULL}
    },
    { //en
        {&dp_giratina, &dp_arceus, &dp_dialga, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom, &dp_heatran, &dp_regigigas},
        {&dp_giratina, &dp_arceus, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom, &dp_heatran, &dp_regigigas},
        {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, &pt_heatran, &pt_regigigas, NULL }
    },
    { //fr
        {&dp_giratina, &dp_arceus, &dp_dialga, &dp_shaymin, &dp_darkrai, &dp_uxie_fr, &dp_azelf_fr, &dp_motisma, &dp_heatran, &dp_regigigas},
        {&dp_giratina, &dp_arceus, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie_fr, &dp_azelf_fr, &dp_motisma, &dp_heatran, &dp_regigigas},
        {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie_fr, &pt_azelf_fr, &pt_rotom_fr, &pt_heatran, &pt_regigigas, NULL}
    },
    { //it
        {&dp_giratina, &dp_arceus, &dp_dialga, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom, &dp_heatran, &dp_regigigas},
        {&dp_giratina, &dp_arceus, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom, &dp_heatran, &dp_regigigas},
        {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, &pt_heatran, &pt_regigigas, NULL}
    },
    { //ge
        {&dp_giratina, &dp_arceus, &dp_dialga, &dp_shaymin, &dp_darkrai, &dp_uxie_ge, &dp_azelf_ge, &dp_rotom, &dp_heatran, &dp_regigigas},
        {&dp_giratina, &dp_arceus, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie_ge, &dp_azelf_ge, &dp_rotom, &dp_heatran, &dp_regigigas},
        {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie_ge, &pt_azelf_ge, &pt_rotom, &pt_heatran, &pt_regigigas, NULL}
    },
    { //_6
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
    { //sp
        {&dp_giratina, &dp_arceus, &dp_dialga, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom, &dp_heatran, &dp_regigigas},
        {&dp_giratina, &dp_arceus, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom, &dp_heatran, &dp_regigigas},
        {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, &pt_heatran, &pt_regigigas, NULL}
    },
    { //ko
        {&dp_giratina_ko, &dp_arceus_ko, &dp_dialga_ko, &dp_shaymin_ko, &dp_darkrai_ko, &dp_uxie_ko, &dp_azelf_ko, &dp_rotom_ko, &dp_heatran_ko, &dp_regigigas_ko},
        {&dp_giratina_ko, &dp_arceus_ko, &dp_palkia_ko, &dp_shaymin_ko, &dp_darkrai_ko, &dp_uxie_ko, &dp_azelf_ko, &dp_rotom_ko, &dp_heatran_ko, &dp_regigigas_ko},
        {&pt_giratina_o_ko, &pt_giratina_a_ko, &pt_dialga_ko, &pt_palkia_ko, &pt_uxie_ko, &pt_azelf_ko, &pt_rotom_ko, &pt_heatran_ko, &pt_regigigas_ko, NULL}
    }
};

/* Functions */

static u32 GetNatureId(u32 pid) {
    /* Get the ID of the Nature (from 0 to 24), provided the PID. */
    //return pid % NATURES_MAX; //naive
    // optimized to avoid div instruction
    u64 x = 0x51EB851FULL;
    return pid - NATURES_MAX * ((x * pid) >> 35);
}

static u8 GetFormId(u16 fate) {
    /* Get the form ID from the fateful word */
    return (fate & 0xFF) >> 3;
}

static void SetBlocks(PKMN* pkmn) {
    /* Get the order of each block from the PID and set them in the correct permutation */
    *(u32*)(&pkmn->pos_a) = Perms[(pkmn->pid >> 13) & 31];
}

static void SetChecksum(PKMN* pkmn) {
    /* Set the checksum of a PKMN by summing all of its Block data */
    u16* data = (u16*)pkmn->data;
    u16 c = 0;
    for (u64 i = 0; i < BLOCKS * BLOCK_SIZE; i++)
    {
        c += data[i];
    }
    pkmn->checksum = c;
}

static void SetChecksumFastSeven(PKMN* s) {
    /* Checksum already initialized with Block A, sum the remaining blocks */
    u16* data = (u16*)&s->data[1];
    u16 c = 0;
    for (u64 i = 0; i < (BLOCKS - 1) * BLOCK_SIZE; i++)
    {
        c += data[i];
    }
    s->checksum += c;
}

static u16 GetGender(u32 pid, u16 species) {
    /* Retrieve gender from pid and species */
    if (species == 0x01E5) { //Heatran
        if ((pid & 0xff) < 127) { return 0x0001; }
        return 0x0000;
    }
    return 0x0004; //genderless
}

static BOOL IsEgg(u32 ivs) {
    /* Check if the egg flag is set by looking at bit 30 of the ivs.  */
    return (ivs >> 30) & 1;
}

static BOOL IsFatefulEncounter(u16 fate) {
    /* Check if the fateful encounter bit is set. */
    return fate & 1;
}

static BOOL IsShiny(u32 pid, u16 tid, u16 sid) {
    /* Check if a PKMN is shiny by XORing its PID (top and bottom 16 bits), TID and SID */
    return ((pid & 0xffff) ^ (pid >> 16) ^ tid ^ sid) < 8;
}

static BOOL HasPokerus(u8 pkrs) {
    /* Check if a PKMN's Pokerus byte is flagged as having Pokerus */
    return pkrs & 0x0f;
}

static u32 RngNext(u32* state) {
    /* General purpose LCRNG, advance and return state */
    *state = *state * 0x41C64E6D + 0x6073;
    return *state;
}

#include <intrin.h>

static void EncryptBlocks(PKMN* pkmn) {
    /* LCRNG is seeded with the Checksum */
    /* Advance the LCRNG, XOR its 16 most significant bits with each 16-bit word of ABCD Block data */
    /* Process in 64-bit blocks for less loops, advance LCRNG in parallel to avoid data dependencies (30+% faster) */
    //u32 state = pkmn->checksum;
    //u64* data = (u64*)pkmn->data;
    //for (u64 i = 0; i < 16; i++)
    //{
    //    u32 state2 = state * 0xC2A29A69 + 0xE97E7B6A; //advance LCRNG by 2
    //    u32 state3 = state * 0x807DBCB5 + 0x52713895; //advance LCRNG by 3
    //    u32 state4 = state * 0xEE067F11 + 0x31B0DDE4; //advance LCRNG by 4
    //    state = state * 0x41C64E6D + 0x00006073; //advance LCRNG by 1
    //    u64 x = ((u64)(state >> 16)) | ((u64)(state2 & 0xffff0000)) | ((u64)(state3 >> 16) << 32) | ((u64)(state4 >> 16) << 48);
    //    data[i] ^= x;
    //    state = state4;
    //}

    /* Instrinsic version */
    u32 state = pkmn->checksum;
    u64* data = (u64*)pkmn->data;
    // todo: see if vmul and vadd can be duplicated to advance even more and avoid data dependencies in the loop below
    __m128i vmul = _mm_set_epi32(0xEE067F11, 0x807DBCB5, 0xC2A29A69, 0x41C64E6D); // multiplier
    __m128i vadd = _mm_set_epi32(0x31B0DDE4, 0x52713895, 0xE97E7B6A, 0x00006073); // adder
    __m128i vsh0 = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x0f, 0x0e, 0x0b, 0x0a, 0x07, 0x06, 0x03, 0x02); // shuffler, upper 64-bits are 0
    __m128i vsh1 = _mm_set_epi8(0x0f, 0x0e, 0x0b, 0x0a, 0x07, 0x06, 0x03, 0x02, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80); // shuffler, lower 64-bits are 0

    for (u64 i = 0; i < 16; i += 4) // doing 4 per loop to not waste clock cycles
    {
        __m128i vstate0 = _mm_set1_epi32(state); // fill all 32-bit slots with state
        vstate0 = _mm_mullo_epi32(vstate0, vmul); // mul
        vstate0 = _mm_add_epi32(vstate0, vadd); // add
        __m128i vstate1 = _mm_shuffle_epi32(vstate0, 0xFF); // set state
        vstate1 = _mm_mullo_epi32(vstate1, vmul); // mul
        vstate1 = _mm_add_epi32(vstate1, vadd); // add
        __m128i vstate2 = _mm_shuffle_epi32(vstate1, 0xFF); // set state
        vstate2 = _mm_mullo_epi32(vstate2, vmul); // mul
        vstate2 = _mm_add_epi32(vstate2, vadd); // add
        __m128i vstate3 = _mm_shuffle_epi32(vstate2, 0xFF); // set state
        vstate3 = _mm_mullo_epi32(vstate3, vmul); // mul
        vstate3 = _mm_add_epi32(vstate3, vadd); // add
        vstate0 = _mm_shuffle_epi8(vstate0, vsh0); // shuffle low
        vstate1 = _mm_shuffle_epi8(vstate1, vsh1); // shuffle high
        vstate2 = _mm_shuffle_epi8(vstate2, vsh0); // shuffle low
        vstate3 = _mm_shuffle_epi8(vstate3, vsh1); // shuffle high
        vstate0 = _mm_or_si128(vstate0, vstate1); // or
        vstate2 = _mm_or_si128(vstate2, vstate3); // or
        vstate0 = _mm_xor_si128(vstate0, _mm_loadu_si128((__m128i*) & data[i])); // xor with pkmn data
        vstate2 = _mm_xor_si128(vstate2, _mm_loadu_si128((__m128i*) & data[i + 2])); // ditto
        _mm_storeu_si128((__m128i*) & data[i], vstate0); // store
        _mm_storeu_si128((__m128i*) & data[i + 2], vstate2); // store
        state = state * 0x5F748241 + 0xCBA72510; // advance by 16
    }
}

static void EncryptBlocksChecksumZero(PKMN* pkmn) {
    /* Fast encryption with precomputed RNG XOR mask (checksum == 0) */
    /* Block A (0) is encrypted in MotorInitPkmn */
    u64* d = (u64*)pkmn->data[1];
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

static void EncryptCondition(PKMN* pkmn) {
    /* LCRNG is seeded with the PID */
    /* Advance the LCRNG, XOR its 16 most significant bits with each 16-bit word of Condition data */
    /* It is not needed to encrypt the whole 50 16-bit words of Condition data, I stop at 5 to include HP MAX */
    u32 state = pkmn->pid;
    for (u64 i = 0; i < COND_SIZE_XS; i++) {
        pkmn->cond[i] ^= (RngNext(&state) >> 16);
    }
    //pkmn->cond[0] ^= (state * 0x41C64E6D + 0x00006073) >> 16;
    //pkmn->cond[1] ^= (state * 0xC2A29A69 + 0xE97E7B6A) >> 16;
    //pkmn->cond[2] ^= (state * 0x807DBCB5 + 0x52713895) >> 16;
    //pkmn->cond[3] ^= (state * 0xEE067F11 + 0x31B0DDE4) >> 16;
    //pkmn->cond[4] ^= (state * 0xEBA1483D + 0x8E425287) >> 16;
}

static void DecomposeIVs(u32 p, u8 ivs[STATS_MAX]) {
    /* Decompose 5-bit packed IVs into individual u8 buffers */
    ivs[HP] = (p >> 0) & IV_VALUE_MAX;
    ivs[AT] = (p >> 5) & IV_VALUE_MAX;
    ivs[DF] = (p >> 10) & IV_VALUE_MAX;
    ivs[SP] = (p >> 15) & IV_VALUE_MAX;
    ivs[SA] = (p >> 20) & IV_VALUE_MAX;
    ivs[SD] = (p >> 25) & IV_VALUE_MAX;
}

static u32 GetIVs(u8 ivs[STATS_MAX]) {
    /* From individual IVs to single 32-bit value */
    //currently unused, want to do something with faster filtering during the search
    return (ivs[HP] << 0) |
        (ivs[AT] << 5) |
        (ivs[DF] << 10) |
        (ivs[SP] << 15) |
        (ivs[SA] << 20) |
        (ivs[SD] << 25);
}

static HIDDENPOWER GetHiddenPower(u8 ivs[STATS_MAX]) {
    /* Calculate Hidden Power from IVs */

    u8 power = ((ivs[HP] & 2) >> 1) |
        ((ivs[AT] & 2) >> 0) |
        ((ivs[DF] & 2) << 1) |
        ((ivs[SP] & 2) << 2) |
        ((ivs[SA] & 2) << 3) |
        ((ivs[SD] & 2) << 4);

    u8 type = ((ivs[HP] & 1) >> 0) |
        ((ivs[AT] & 1) << 1) |
        ((ivs[DF] & 1) << 2) |
        ((ivs[SP] & 1) << 3) |
        ((ivs[SA] & 1) << 4) |
        ((ivs[SD] & 1) << 5);

    power = (power * 40 / 63) + 30;
    type = (type * 15 / 63) + 1;

    if (type >= TYPE_UNKNOWN) { type++; }

    return (HIDDENPOWER) { type, power };
}

static void MethodJSeedToPID(u32 state, PKMN* pkmn) {
    /* Calculate PID, Nature and IVs according to Method J Stationary (no Synchronize / Cute Charm) from a given state */
    pkmn->nature = (((u64)RngNext(&state) >> 17) * 3276101) >> 32; // fast nature computation (avoids div instruction with 0x0A3E0000)

    //do {
    //    pkmn->pid = (RngNext(&state) >> 16) | (RngNext(&state) & 0xffff0000);
    //} while (GetNatureId(pkmn->pid) != pkmn->nature); //roll PID until the 2 natures are the same

    // around 10% faster than above because the dependency chain is broken (good instruction level parallelism)
    do {
        u32 state2 = state * 0xC2A29A69 + 0xE97E7B6A; //advance LCRNG by 2
        state = state * 0x41C64E6D + 0x00006073; //advance LCRNG by 1
        pkmn->pid = (state >> 16) | (state2 & 0xffff0000);
        state = state2;
    } while (GetNatureId(pkmn->pid) != pkmn->nature);

    pkmn->iv32 = ((RngNext(&state) >> 16) & 0x00007fff) | ((RngNext(&state) >> 1) & 0x3fff8000);
}

static REVERSEDSEED ReverseSeed(u32 seed) {
    /* Find the nearest console-hitable seed provided the current state of the RNG */
    u32 state = seed;
    u16 frame = 0;
    u8 a = state >> 24;
    u8 b = state >> 16;
    u16 c = state & 0xffff;
    /* Search loop */
    while (b > SEED_MAX_B || c < MIN_DELAY_DPPT || c > MAX_DELAY_DPPT)
    {
        state = state * 0xEEB9EB65 + 0x0A3561A1; //reverse LCRNG
        a = state >> 24;
        b = state >> 16;
        c = state & 0xffff;
        frame++;
    }
    return (REVERSEDSEED) { seed, state, frame };
}

static APPSTATUS MotorSearchAslr(REVERSEDSEED* rs, PROFILE* pf) {
    /* Stripped down version of general search, only vary the ASLR */

    // todo: add valid mirrors
    // todo: use WildInit and SevenInit? 

    u8 filename[PATH_REL_LENGTH_MAX] = { 0 };
    sprintf(filename, ".results/%08X_ASLR.txt", rs->reversed);
    FILE* fp = fopen(filename, "wb+");
    if (fp == NULL) { return APP_ERR_OPEN_FILE; }

    fprintf(fp, "/4    ASLR        Wild       Species\n\n");

    SEARCHDATA sd = { 0 };
    sd.grouped_version = pf->version >> 1; //Group Diamond and Pearl together
    sd.w_version = (pf->version + 10) << 8; //convert for use in pkmn data
    sd.w_language = pf->language << 8; //convert for use in pkmn data
    sd.pOgWild = OGW_LangVers[pf->language][pf->version][pf->wild];
    sd.aslr = Aslrs[pf->language][sd.grouped_version][pf->aslr];
    sd.alt_form = (pf->version == VERSION_PLATINUM && pf->wild == OGW_PT_GIRATINA_O) ? 8 : 0; //Giratina Origin
    u8 korean_offset = (pf->language == LANGUAGE_KO) ? KOREAN_OFFSET : 0;
    u32 levelMult = sd.pOgWild->level / 100;

    for (u32 offset = 0; offset <= 256; offset += 4) {

        u32 aslr = AslrMin[pf->language][sd.grouped_version] + offset;

        PKMN wild = { 0 }; //0 init
        PKMN seven = { 0 }; //0 init

        MethodJSeedToPID(rs->seed, &wild);
        SetBlocks(&wild);

        /* Block A */
        wild.data[wild.pos_a][0] = sd.pOgWild->species; //species
        wild.data[wild.pos_a][1] = sd.pOgWild->item; //held item
        wild.data[wild.pos_a][2] = pf->tid; //tid
        wild.data[wild.pos_a][3] = pf->sid; //sid
        wild.data[wild.pos_a][4] = sd.pOgWild->xp1; //xp1
        wild.data[wild.pos_a][5] = sd.pOgWild->xp2; //xp2
        wild.data[wild.pos_a][6] = sd.pOgWild->frab; //ability and friendship concatenated
        wild.data[wild.pos_a][7] = sd.w_language; //language
        /* Block B */
        memcpy(&wild.data[wild.pos_b][0], sd.pOgWild->moves, sizeof(sd.pOgWild->moves)); //moves
        wild.data[wild.pos_b][4] = sd.pOgWild->pp1and2; //pp1and2
        wild.data[wild.pos_b][5] = sd.pOgWild->pp3and4; //pp3and4
        memcpy(&wild.data[wild.pos_b][8], &wild.iv32, 4);
        wild.data[wild.pos_b][12] = GetGender(wild.pid, sd.pOgWild->species) | sd.alt_form; //gender | alt_form
        /* Block C */
        memcpy(&wild.data[wild.pos_c][0], sd.pOgWild->name, sizeof(sd.pOgWild->name)); //name
        wild.data[wild.pos_c][11] = sd.w_version; //version
        /* Block D */
        wild.data[wild.pos_d][13] = 0x0400; //pokeball
        wild.data[wild.pos_d][14] = sd.pOgWild->level; //level
        /* Condition data */
        wild.cond[2] = sd.pOgWild->level; //level again

        SetChecksum(&wild);
        EncryptBlocks(&wild);
        EncryptCondition(&wild);

        /* Initialize Seven */
        seven.pid = 0x00005544;
        seven.bef = 0x05a4; //after checksum check, changed to a Bad Egg (bit 4: 0->1)
        SetBlocks(&seven); //always ACBD (0x00020103)

        /* Simulate the buffer overflow */
        /* Block A */
        seven.data[SEVEN_BLOCK_A][0] = (aslr + OppPartyOffBeg[sd.grouped_version] + korean_offset) & 0xffff;
        seven.data[SEVEN_BLOCK_A][1] = (aslr + OppPartyOffBeg[sd.grouped_version] + korean_offset) >> 16;
        seven.data[SEVEN_BLOCK_A][2] = (aslr + OppPartyOffEnd[sd.grouped_version] + korean_offset) & 0xffff;
        seven.data[SEVEN_BLOCK_A][3] = (aslr + OppPartyOffEnd[sd.grouped_version] + korean_offset) >> 16;
        memcpy(&seven.data[SEVEN_BLOCK_A][4], sd.pOgWild->gfx, sizeof(sd.pOgWild->gfx));
        seven.data[SEVEN_BLOCK_A][12] = 0x0006;
        seven.data[SEVEN_BLOCK_A][13] = 0x0000;
        seven.data[SEVEN_BLOCK_A][14] = 0x0001;
        seven.data[SEVEN_BLOCK_A][15] = 0x0000;
        /* Block C, B, D and Condition data */
        memcpy(&seven.data[SEVEN_BLOCK_C], &wild.pid, 2 * (BLOCKS * BLOCK_SIZE + STACK_OFFSET + COND_SIZE_XS));

        EncryptBlocksChecksumZero(&seven);

        fprintf(fp, "%02u    %08X    ", offset / 4, aslr);

        /* If the ball doesn't have a valid ID the battle won't load */
        u8 ballid = seven.data[SEVEN_BLOCK_D][13] >> 8;
        if (ballid > BALL_ID_MAX)
        {
            fprintf(fp, "Black screen\n");
            continue;
        }

        SetChecksum(&seven);
        EncryptBlocks(&seven);

        /* If the Bad Egg flag is set or the Fast Mode flags aren't set, the PKMN will become a Bad Egg */
        if ((seven.data[SEVEN_BLOCK_C][2] & 7) != 3)
        {
            fprintf(fp, "Bad Egg    ");
        }
        else
        {
            fprintf(fp, "Regular    ");
        }

        /* Get the new PID of the wild and deduce its new block order */
        wild.pid = seven.data[SEVEN_BLOCK_C][0] | (seven.data[SEVEN_BLOCK_C][1] << 16);
        SetBlocks(&wild);

        u16 species = seven.data[1 + wild.pos_a][STACK_OFFSET];
        u8 str_species[STRING_LENGTH_MAX] = { 0 };
        SetString(str_species, species, Pokelist, SPECIES_MAX, "DPBox", "0x%04X");

        if (aslr == sd.aslr)
        {
            fprintf(fp, "%s <<< your target\n", str_species);
        }
        else if (0) //todo: add condition for mirrors
        {
            fprintf(fp, "%s <<< valid mirror\n", str_species);
        }
        else
        {
            fprintf(fp, "%s\n", str_species);
        }
    }
    /* Search end */
    fclose(fp);
    return APP_RESUME;
}
