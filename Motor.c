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

/* TYPEDEF */

typedef unsigned __int64 u64;
typedef unsigned __int32 u32;
typedef unsigned __int16 u16;
typedef unsigned __int8 u8;
typedef signed __int8 s8;

/* CONSTANTS */

#define BLOCKS (4) //number of blocks
#define BLOCK_PERM (24) //factorial of number of blocks
#define BLOCK_SIZE (16) //128 bytes (4*32), divided into 16-bit words
#define COND_SIZE (50) //100 bytes (1*100), divided into 16-bit words

#define LANGUAGES (9)
#define VERSIONS (3)
#define ABILITIES (124)
#define SPECIES (494)
#define MOVES (468)
#define ITEMS (468)
#define NATURES (25)
#define STATS (6)

#define LOC_BEG_OPP_PARTY_PL (0x58864) //Platinum
#define LOC_END_OPP_PARTY_PL (0x593C4) //Platinum
#define LOC_BEG_OPP_PARTY_DP (0x4C7B0) //Diamond and Pearl
#define LOC_END_OPP_PARTY_DP (0x4D310) //Diamond and Pearl

#define SEED_MAX_A (256) //month & day
#define SEED_MAX_B (24) //hour
#define SEED_MIN_C (0x0300) //delay min
#define SEED_MAX_C (0x1000) //delay max

#define RS_OFF (4) //misalignment between wild and seven
#define PIDS_MAX (1060) //calculated from a previous program that found the highest possible occidentary

u8 Languages[LANGUAGES][3] = {"_0", "jp", "en", "fr", "it", "ge", "_6", "sp", "ko"}; //languages by index
// enum { dummy_0=0, jp, en, fr, it, ge, dummy_6, sp, ko };

u8 Versions[VERSIONS][9] = {"Diamond", "Pearl", "Platinum"}; //versions
// enum { diamond=0, pearl, platinum };

u32 Aslrs[LANGUAGES][VERSIONS-1] = {{0, 0}, {0, 0}, {0x0226D300, 0x0227116C}, {0x0226D5FC, 0x02271460}, {0x0226D500, 0}, {0x0226D504, 0}, {0, 0}, {0x0226D604, 0}, {0, 0}}; //aslr to match with language and version

u8 Orders[BLOCK_PERM][BLOCKS+1] = {"ABCD", "ABDC", "ACBD", "ACDB", "ADBC", "ADCB", "BACD", "BADC", "BCAD", "BCDA", "BDAC", "BDCA", "CABD", "CADB", "CBAD", "CBDA", "CDAB", "CDBA", "DABC", "DACB", "DBAC", "DBCA", "DCAB", "DCBA"}; //all 24 block permutations

enum { hp=0, at, df, sp, sa, sd }; //enum for the indices of each stat

s8 NatureStatModifiers[NATURES][STATS-1] = {{0, 0, 0, 0, 0}, {1, -1, 0, 0, 0}, {1, 0, -1, 0, 0}, {1, 0, 0, -1, 0}, {1, 0, 0, 0, -1}, {-1, 1, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 1, -1, 0, 0}, {0, 1, 0, -1, 0}, {0, 1, 0, 0, -1}, {-1, 0, 1, 0, 0}, {0, -1, 1, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 1, -1, 0}, {0, 0, 1, 0, -1}, {-1, 0, 0, 1, 0}, {0, -1, 0, 1, 0}, {0, 0, -1, 1, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 1, -1}, {-1, 0, 0, 0, 1}, {0, -1, 0, 0, 1}, {0, 0, -1, 0, 1}, {0, 0, 0, -1, 1}, {0, 0, 0, 0, 0}};

u8 Abilities[ABILITIES][16] = {"None", "Stench", "Drizzle", "Speed Boost", "Battle Armor", "Sturdy", "Damp", "Limber", "Sand Veil", "Static", "Volt Absorb", "Water Absorb", "Oblivious", "Cloud Nine", "Compound Eyes", "Insomnia", "Color Change", "Immunity", "Flash Fire", "Shield Dust", "Own Tempo", "Suction Cups", "Intimidate", "Shadow Tag", "Rough Skin", "Wonder Guard", "Levitate", "Effect Spore", "Synchronize", "Clear Body", "Natural Cure", "Lightning Rod", "Serene Grace", "Swift Swim", "Chlorophyll", "Illuminate", "Trace", "Huge Power", "Poison Point", "Inner Focus", "Magma Armor", "Water Veil", "Magnet Pull", "Soundproof", "Rain Dish", "Sand Stream", "Pressure", "Thick Fat", "Early Bird", "Flame Body", "Run Away", "Keen Eye", "Hyper Cutter", "Pickup", "Truant", "Hustle", "Cute Charm", "Plus", "Minus", "Forecast", "Sticky Hold", "Shed Skin", "Guts", "Marvel Scale", "Liquid Ooze", "Overgrow", "Blaze", "Torrent", "Swarm", "Rock Head", "Drought", "Arena Trap", "Vital Spirit", "White Smoke", "Pure Power", "Shell Armor", "Air Lock", "Tangled Feet", "Motor Drive", "Rivalry", "Steadfast", "Snow Cloak", "Gluttony", "Anger Point", "Unburden", "Heatproof", "Simple", "Dry Skin", "Download", "Iron Fist", "Poison Heal", "Adaptability", "Skill Link", "Hydration", "Solar Power", "Quick Feet", "Normalize", "Sniper", "Magic Guard", "No Guard", "Stall", "Technician", "Leaf Guard", "Klutz", "Mold Breaker", "Super Luck", "Aftermath", "Anticipation", "Forewarn", "Unaware", "Tinted Lens", "Filter", "Slow Start", "Scrappy", "Storm Drain", "Ice Body", "Solid Rock", "Snow Warning", "Honey Gather", "Frisk", "Reckless", "Multitype", "Flower Gift", "Bad Dreams"};

u8 Pokelist[SPECIES][12] = {"anything", "Bulbasaur", "Ivysaur", "Venusaur", "Charmander", "Charmeleon", "Charizard", "Squirtle", "Wartortle", "Blastoise", "Caterpie", "Metapod", "Butterfree", "Weedle", "Kakuna", "Beedrill", "Pidgey", "Pidgeotto", "Pidgeot", "Rattata", "Raticate", "Spearow", "Fearow", "Ekans", "Arbok", "Pikachu", "Raichu", "Sandshrew","Sandslash", "Nidoran-F", "Nidorina", "Nidoqueen", "Nidoran-M", "Nidorino", "Nidoking", "Clefairy", "Clefable", "Vulpix", "Ninetales", "Jigglypuff", "Wigglytuff", "Zubat", "Golbat", "Oddish", "Gloom", "Vileplume", "Paras", "Parasect", "Venonat", "Venomoth", "Diglett", "Dugtrio", "Meowth", "Persian", "Psyduck", "Golduck", "Mankey", "Primeape", "Growlithe", "Arcanine", "Poliwag", "Poliwhirl", "Poliwrath", "Abra", "Kadabra", "Alakazam", "Machop", "Machoke", "Machamp", "Bellsprout", "Weepinbell", "Victreebel", "Tentacool", "Tentacruel", "Geodude", "Graveler", "Golem", "Ponyta", "Rapidash", "Slowpoke", "Slowbro", "Magnemite", "Magneton", "Farfetch'd", "Doduo", "Dodrio", "Seel", "Dewgong", "Grimer", "Muk", "Shellder", "Cloyster", "Gastly", "Haunter", "Gengar", "Onix", "Drowzee", "Hypno", "Krabby", "Kingler", "Voltorb", "Electrode", "Exeggcute", "Exeggutor", "Cubone", "Marowak", "Hitmonlee", "Hitmonchan", "Lickitung", "Koffing", "Weezing", "Rhyhorn", "Rhydon", "Chansey", "Tangela", "Kangaskhan", "Horsea", "Seadra", "Goldeen", "Seaking", "Staryu", "Starmie", "Mr.Mime", "Scyther", "Jynx", "Electabuzz", "Magmar", "Pinsir", "Tauros", "Magikarp", "Gyarados", "Lapras", "Ditto", "Eevee", "Vaporeon", "Jolteon", "Flareon", "Porygon", "Omanyte", "Omastar", "Kabuto", "Kabutops", "Aerodactyl", "Snorlax", "Articuno", "Zapdos", "Moltres", "Dratini", "Dragonair", "Dragonite", "Mewtwo", "Mew", "Chikorita", "Bayleef", "Meganium", "Cyndaquil", "Quilava", "Typhlosion", "Totodile", "Croconaw", "Feraligatr", "Sentret", "Furret", "Hoothoot", "Noctowl", "Ledyba", "Ledian", "Spinarak", "Ariados", "Crobat", "Chinchou", "Lanturn", "Pichu", "Cleffa", "Igglybuff", "Togepi", "Togetic", "Natu", "Xatu", "Mareep", "Flaaffy", "Ampharos", "Bellossom", "Marill", "Azumarill", "Sudowoodo", "Politoed", "Hoppip", "Skiploom", "Jumpluff", "Aipom", "Sunkern", "Sunflora", "Yanma", "Wooper", "Quagsire", "Espeon", "Umbreon", "Murkrow", "Slowking", "Misdreavus", "Unown", "Wobbuffet", "Girafarig", "Pineco", "Forretress", "Dunsparce", "Gligar", "Steelix", "Snubbull", "Granbull", "Qwilfish", "Scizor", "Shuckle", "Heracross", "Sneasel", "Teddiursa", "Ursaring", "Slugma", "Magcargo", "Swinub", "Piloswine", "Corsola", "Remoraid", "Octillery", "Delibird", "Mantine", "Skarmory", "Houndour", "Houndoom", "Kingdra", "Phanpy", "Donphan", "Porygon-2", "Stantler", "Smeargle", "Tyrogue", "Hitmontop", "Smoochum", "Elekid", "Magby", "Miltank", "Blissey", "Raikou", "Entei", "Suicune", "Larvitar", "Pupitar", "Tyranitar", "Lugia", "Ho-Oh", "Celebi", "Treecko", "Grovyle", "Sceptile", "Torchic", "Combusken", "Blaziken", "Mudkip", "Marshtomp", "Swampert", "Poochyena", "Mightyena", "Zigzagoon", "Linoone", "Wurmple", "Silcoon", "Beautifly", "Cascoon", "Dustox", "Lotad", "Lombre", "Ludicolo", "Seedot", "Nuzleaf", "Shiftry", "Taillow", "Swellow", "Wingull", "Pelipper", "Ralts", "Kirlia", "Gardevoir", "Surskit", "Masquerain", "Shroomish", "Breloom", "Slakoth", "Vigoroth", "Slaking", "Nincada", "Ninjask", "Shedinja", "Whismur", "Loudred", "Exploud", "Makuhita", "Hariyama", "Azurill", "Nosepass", "Skitty", "Delcatty", "Sableye", "Mawile", "Aron", "Lairon", "Aggron", "Meditite", "Medicham", "Electrike", "Manectric", "Plusle", "Minun", "Volbeat", "Illumise", "Roselia", "Gulpin", "Swalot", "Carvanha", "Sharpedo", "Wailmer", "Wailord", "Numel", "Camerupt", "Torkoal", "Spoink", "Grumpig", "Spinda", "Trapinch", "Vibrava", "Flygon", "Cacnea", "Cacturne", "Swablu", "Altaria", "Zangoose", "Seviper", "Lunatone", "Solrock", "Barboach", "Whiscash", "Corphish", "Crawdaunt", "Baltoy", "Claydol", "Lileep", "Cradily", "Anorith", "Armaldo", "Feebas", "Milotic", "Castform", "Kecleon", "Shuppet", "Banette", "Duskull", "Dusclops", "Tropius", "Chimecho", "Absol", "Wynaut", "Snorunt", "Glalie", "Spheal", "Sealeo", "Walrein", "Clamperl", "Huntail", "Gorebyss", "Relicanth", "Luvdisc", "Bagon", "Shelgon", "Salamence", "Beldum", "Metang", "Metagross", "Regirock", "Regice", "Registeel", "Latias", "Latios", "Kyogre", "Groudon", "Rayquaza", "Jirachi", "Deoxys", "Turtwig", "Grotle", "Torterra", "Chimchar", "Monferno", "Infernape", "Piplup", "Prinplup", "Empoleon", "Starly", "Staravia", "Staraptor", "Bidoof", "Bibarel", "Kricketot", "Kricketune", "Shinx", "Luxio", "Luxray", "Budew", "Roserade", "Cranidos", "Rampardos", "Shieldon", "Bastiodon", "Burmy", "Wormadam", "Mothim", "Combee", "Vespiquen", "Pachirisu", "Buizel", "Floatzel", "Cherubi", "Cherrim", "Shellos", "Gastrodon", "Ambipom", "Drifloon", "Drifblim", "Buneary", "Lopunny", "Mismagius", "Honchkrow", "Glameow", "Purugly", "Chingling", "Stunky", "Skuntank", "Bronzor", "Bronzong", "Bonsly", "MimeJr.", "Happiny", "Chatot", "Spiritomb", "Gible", "Gabite", "Garchomp", "Munchlax", "Riolu", "Lucario", "Hippopotas", "Hippowdon", "Skorupi", "Drapion", "Croagunk", "Toxicroak", "Carnivine", "Finneon", "Lumineon", "Mantyke", "Snover", "Abomasnow", "Weavile", "Magnezone", "Lickilicky", "Rhyperior", "Tangrowth", "Electivire", "Magmortar", "Togekiss", "Yanmega", "Leafeon", "Glaceon", "Gliscor", "Mamoswine", "Porygon-Z", "Gallade", "Probopass", "Dusknoir", "Froslass", "Rotom", "Uxie", "Mesprit", "Azelf", "Dialga", "Palkia", "Heatran", "Regigigas", "Giratina", "Cresselia", "Phione", "Manaphy", "Darkrai", "Shaymin", "Arceus"};

u8 Items[ITEMS][16] = {"anything", "Master Ball", "Ultra Ball", "Great Ball", "Poké Ball", "Safari Ball", "Net Ball", "Dive Ball", "Nest Ball", "Repeat Ball", "Timer Ball", "Luxury Ball", "Premier Ball", "Dusk Ball", "Heal Ball", "Quick Ball", "Cherish Ball", "Potion", "Antidote", "Burn Heal", "Ice Heal", "Awakening", "Paralyze Heal", "Full Restore", "Max Potion", "Hyper Potion", "Super Potion", "Full Heal", "Revive", "Max Revive", "Fresh Water", "Soda Pop", "Lemonade", "Moomoo Milk", "Energy Powder", "Energy Root", "Heal Powder", "Revival Herb", "Ether", "Max Ether", "Elixir", "Max Elixir", "Lava Cookie", "Berry Juice", "Sacred Ash", "HP Up", "Protein", "Iron", "Carbos", "Calcium", "Rare Candy", "PP Up", "Zinc", "PP Max", "Old Gateau", "Guard Spec", "Dire Hit", "X Attack", "X Defense", "X Speed", "X Accuracy", "X SpeAtk", "X SpeDef", "Poké Doll", "Fluffy Tail", "Blue Flute", "Yellow Flute", "Red Flute", "Black Flute", "White Flute", "Shoal Salt", "Shoal Shell", "Red Shard", "Blue Shard", "Yellow Shard", "Green Shard", "Super Repel", "Max Repel", "Escape Rope", "Repel", "Sun Stone", "Moon Stone", "Fire Stone", "Thunder Stone", "Water Stone", "Leaf Stone", "Tiny Mushroom", "Big Mushroom", "Pearl", "Big Pearl", "Stardust", "Star Piece", "Nugget", "Heart Scale", "Honey", "Growth Mulch", "Damp Mulch", "Stable Mulch", "Gooey Mulch", "Root Fossil", "Claw Fossil", "Helix Fossil", "Dome Fossil", "Old Amber", "Armor Fossil", "Skull Fossil", "Rare Bone", "Shiny Stone", "Dusk Stone", "Dawn Stone", "Oval Stone", "Odd Keystone", "Griseous Orb", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Adamant Orb", "Lustrous Orb", "Grass Mail", "Flame Mail", "Bubble Mail", "Bloom Mail", "Tunnel Mail", "Steel Mail", "Heart Mail", "Snow Mail", "Space Mail", "Air Mail", "Mosaic Mail", "Brick Mail", "Cheri Berry", "Chesto Berry", "Pecha Berry", "Rawst Berry", "Aspear Berry", "Leppa Berry", "Oran Berry", "Persim Berry", "Lum Berry", "Sitrus Berry", "Figy Berry", "Wiki Berry", "Mago Berry", "Aguav Berry", "Iapapa Berry", "Razz Berry", "Bluk Berry", "Nanab Berry", "Wepear Berry", "Pinap Berry", "Pomeg Berry", "Kelpsy Berry", "Qualot Berry", "Hondew Berry", "Grepa Berry", "Tamato Berry", "Cornn Berry", "Magost Berry", "Rabuta Berry", "Nomel Berry", "Spelon Berry", "Pamtre Berry", "Watmel Berry", "Durin Berry", "Belue Berry", "Occa Berry", "Passho Berry", "Wacan Berry", "Rindo Berry", "Yache Berry", "Chople Berry", "Kebia Berry", "Shuca Berry", "Coba Berry", "Payapa Berry", "Tanga Berry", "Charti Berry", "Kasib Berry", "Haban Berry", "Colbur Berry", "Babiri Berry", "Chilan Berry", "Liechi Berry", "Ganlon Berry", "Salac Berry", "Petaya Berry", "Apicot Berry", "Lansat Berry", "Starf Berry", "Enigma Berry", "Micle Berry", "Custap Berry", "Jaboca Berry", "Rowap Berry", "Bright Powder", "White Herb", "Macho Brace", "Exp Share", "Quick Claw", "Soothe Bell", "Mental Herb", "Choice Band", "King's Rock", "Silver Powder", "Amulet Coin", "Cleanse Tag", "Soul Dew", "Deep Sea Tooth", "Deep Sea Scale", "Smoke Ball", "Everstone", "Focus Band", "Lucky Egg", "Scope Lens", "Metal Coat", "Leftovers", "Dragon Scale", "Light Ball", "Soft Sand", "Hard Stone", "Miracle Seed", "Black Glasses", "Black Belt", "Magnet", "Mystic Water", "Sharp Beak", "Poison Barb", "Never-Melt Ice", "Spell Tag", "Twisted Spoon", "Charcoal", "Dragon Fang", "Silk Scarf", "Up-Grade", "Shell Bell", "Sea Incense", "Lax Incense", "Lucky Punch", "Metal Powder", "Thick Club", "Stick", "Red Scarf", "Blue Scarf", "Pink Scarf", "Green Scarf", "Yellow Scarf", "Wide Lens", "Muscle Band", "Wise Glasses", "Expert Belt", "Light Clay", "Life Orb", "Power Herb", "Toxic Orb", "Flame Orb", "Quick Powder", "Focus Sash", "Zoom Lens", "Metronome", "Iron Ball", "Lagging Tail", "Destiny Knot", "Black Sludge", "Icy Rock", "Smooth Rock", "Heat Rock", "Damp Rock", "Grip Claw", "Choice Scarf", "Sticky Barb", "Power Bracer", "Power Belt", "Power Lens", "Power Band", "Power Anklet", "Power Weight", "Shed Shell", "Big Root", "Choice Specs", "Flame Plate", "Splash Plate", "Zap Plate", "Meadow Plate", "Icicle Plate", "Fist Plate", "Toxic Plate", "Earth Plate", "Sky Plate", "Mind Plate", "Insect Plate", "Stone Plate", "Spooky Plate", "Draco Plate", "Dread Plate", "Iron Plate", "Odd Incense", "Rock Incense", "Full Incense", "Wave Incense", "Rose Incense", "Luck Incense", "Pure Incense", "Protector", "Electirizer", "Magmarizer", "Dubious Disc", "Reaper Cloth", "Razor Claw", "Razor Fang", "TM01", "TM02", "TM03", "TM04", "TM05", "TM06", "TM07", "TM08", "TM09", "TM10", "TM11", "TM12", "TM13", "TM14", "TM15", "TM16", "TM17", "TM18", "TM19", "TM20", "TM21", "TM22", "TM23", "TM24", "TM25", "TM26", "TM27", "TM28", "TM29", "TM30", "TM31", "TM32", "TM33", "TM34", "TM35", "TM36", "TM37", "TM38", "TM39", "TM40", "TM41", "TM42", "TM43", "TM44", "TM45", "TM46", "TM47", "TM48", "TM49", "TM50", "TM51", "TM52", "TM53", "TM54", "TM55", "TM56", "TM57", "TM58", "TM59", "TM60", "TM61", "TM62", "TM63", "TM64", "TM65", "TM66", "TM67", "TM68", "TM69", "TM70", "TM71", "TM72", "TM73", "TM74", "TM75", "TM76", "TM77", "TM78", "TM79", "TM80", "TM81", "TM82", "TM83", "TM84", "TM85", "TM86", "TM87", "TM88", "TM89", "TM90", "TM91", "TM92", "HM01", "HM02", "HM03", "HM04", "HM05", "HM06", "HM07", "HM08", "Explorer Kit", "Loot Sack", "Rule Book", "Poké Radar", "Point Card", "Journal", "Seal Case", "Fashion Case", "Seal", "Pal Pad", "Works Key", "Old Charm", "Galactic Key", "Red Chain", "Town Map", "VS Seeker", "Coin Case", "Old Rod", "Good Rod", "Super Rod", "Sprayduck", "Poffin Case", "Bicycle", "Suite Key", "Oak's Letter", "Lunar Wing", "Member Card", "Azure Flute", "S.S. Ticket", "Contest Pass", "Magma Stone", "Parcel", "Coupon 1", "Coupon 2", "Coupon 3", "Storage Key", "Secret Potion", "VS Recorder", "Gracidea", "Secret Key"};

u8 Moves[MOVES][16] = { "None" , "Pound", "Karate Chop", "Double Slap", "Comet Punch", "Mega Punch", "Pay Day", "Fire Punch", "Ice Punch", "Thunder Punch", "Scratch", "Vice Grip", "Guillotine", "Razor Wind", "Swords Dance", "Cut", "Gust", "Wing Attack", "Whirlwind", "Fly", "Bind", "Slam", "Vine Whip", "Stomp", "Double Kick", "Mega Kick", "Jump Kick", "Rolling Kick", "Sand Attack", "Headbutt", "Horn Attack", "Fury Attack", "Horn Drill", "Tackle", "Body Slam", "Wrap", "Take Down", "Thrash", "Double-Edge", "Tail Whip", "Poison Sting", "Twineedle", "Pin Missile", "Leer", "Bite", "Growl", "Roar", "Sing", "Supersonic", "Sonic Boom", "Disable", "Acid", "Ember", "Flamethrower", "Mist", "Water Gun", "Hydro Pump", "Surf", "Ice Beam", "Blizzard", "Psybeam", "Bubble Beam", "Aurora Beam", "Hyper Beam", "Peck", "Drill Peck", "Submission", "Low Kick", "Counter", "Seismic Toss", "Strength", "Absorb", "Mega Drain", "Leech Seed", "Growth", "Razor Leaf", "Solar Beam", "Poison Powder", "Stun Spore", "Sleep Powder", "Petal Dance", "String Shot", "Dragon Rage", "Fire Spin", "Thunder Shock", "Thunderbolt", "Thunder Wave", "Thunder", "Rock Throw", "Earthquake", "Fissure", "Dig", "Toxic", "Confusion", "Psychic", "Hypnosis", "Meditate", "Agility", "Quick Attack", "Rage", "Teleport", "Night Shade", "Mimic", "Screech", "Double Team", "Recover", "Harden", "Minimize", "Smokescreen", "Confuse Ray", "Withdraw", "Defense Curl", "Barrier", "Light Screen", "Haze", "Reflect", "Focus Energy", "Bide", "Metronome", "Mirror Move", "Self-Destruct", "Egg Bomb", "Lick", "Smog", "Sludge", "Bone Club", "Fire Blast", "Waterfall", "Clamp", "Swift", "Skull Bash", "Spike Cannon", "Constrict", "Amnesia", "Kinesis", "Soft-Boiled", "High Jump Kick", "Glare", "Dream Eater", "Poison Gas", "Barrage", "Leech Life", "Lovely Kiss", "Sky Attack", "Transform", "Bubble", "Dizzy Punch", "Spore", "Flash", "Psywave", "Splash", "Acid Armor", "Crabhammer", "Explosion", "Fury Swipes", "Bonemerang", "Rest", "Rock Slide", "Hyper Fang", "Sharpen", "Conversion", "Tri Attack", "Super Fang", "Slash", "Substitute", "Struggle", "Sketch", "Triple Kick", "Thief", "Spider Web", "Mind Reader", "Nightmare", "Flame Wheel", "Snore", "Curse", "Flail", "Conversion 2", "Aeroblast", "Cotton Spore", "Reversal", "Spite", "Powder Snow", "Protect", "Mach Punch", "Scary Face", "Feint Attack", "Sweet Kiss", "Belly Drum", "Sludge Bomb", "Mud-Slap", "Octazooka", "Spikes", "Zap Cannon", "Foresight", "Destiny Bond", "Perish Song", "Icy Wind", "Detect", "Bone Rush", "Lock-On", "Outrage", "Sandstorm", "Giga Drain", "Endure", "Charm", "Rollout", "False Swipe", "Swagger", "Milk Drink", "Spark", "Fury Cutter", "Steel Wing", "Mean Look", "Attract", "Sleep Talk", "Heal Bell", "Return", "Present", "Frustration", "Safeguard", "Pain Split", "Sacred Fire", "Magnitude", "Dynamic Punch", "Megahorn", "Dragon Breath", "Baton Pass", "Encore", "Pursuit", "Rapid Spin", "Sweet Scent", "Iron Tail", "Metal Claw", "Vital Throw", "Morning Sun", "Synthesis", "Moonlight", "Hidden Power", "Cross Chop", "Twister", "Rain Dance", "Sunny Day", "Crunch", "Mirror Coat", "Psych Up", "Extreme Speed", "Ancient Power", "Shadow Ball", "Future Sight", "Rock Smash", "Whirlpool", "Beat Up", "Fake Out", "Uproar", "Stockpile", "Spit Up", "Swallow", "Heat Wave", "Hail", "Torment", "Flatter", "Will-O-Wisp", "Memento", "Facade", "Focus Punch", "Smelling Salts", "Follow Me", "Nature Power", "Charge", "Taunt", "Helping Hand", "Trick", "Role Play", "Wish", "Assist", "Ingrain", "Superpower", "Magic Coat", "Recycle", "Revenge", "Brick Break", "Yawn", "Knock Off", "Endeavor", "Eruption", "Skill Swap", "Imprison", "Refresh", "Grudge", "Snatch", "Secret Power", "Dive", "Arm Thrust", "Camouflage", "Tail Glow", "Luster Purge", "Mist Ball", "Feather Dance", "Teeter Dance", "Blaze Kick", "Mud Sport", "Ice Ball", "Needle Arm", "Slack Off", "Hyper Voice", "Poison Fang", "Crush Claw", "Blast Burn", "Hydro Cannon", "Meteor Mash", "Astonish", "Weather Ball", "Aromatherapy", "Fake Tears", "Air Cutter", "Overheat", "Odor Sleuth", "Rock Tomb", "Silver Wind", "Metal Sound", "Grass Whistle", "Tickle", "Cosmic Power", "Water Spout", "Signal Beam", "Shadow Punch", "Extrasensory", "Sky Uppercut", "Sand Tomb", "Sheer Cold", "Muddy Water", "Bullet Seed", "Aerial Ace", "Icicle Spear", "Iron Defense", "Block", "Howl", "Dragon Claw", "Frenzy Plant", "Bulk Up", "Bounce", "Mud Shot", "Poison Tail", "Covet", "Volt Tackle", "Magical Leaf", "Water Sport", "Calm Mind", "Leaf Blade", "Dragon Dance", "Rock Blast", "Shock Wave", "Water Pulse", "Doom Desire", "Psycho Boost", "Roost", "Gravity", "Miracle Eye", "Wake-Up Slap", "Hammer Arm", "Gyro Ball", "Healing Wish", "Brine", "Natural Gift", "Feint", "Pluck", "Tailwind", "Acupressure", "Metal Burst", "U-turn", "Close Combat", "Payback", "Assurance", "Embargo", "Fling", "Psycho Shift", "Trump Card", "Heal Block", "Wring Out", "Power Trick", "Gastro Acid", "Lucky Chant", "Me First", "Copycat", "Power Swap", "Guard Swap", "Punishment", "Last Resort", "Worry Seed", "Sucker Punch", "Toxic Spikes", "Heart Swap", "Aqua Ring", "Magnet Rise", "Flare Blitz", "Force Palm", "Aura Sphere", "Rock Polish", "Poison Jab", "Dark Pulse", "Night Slash", "Aqua Tail", "Seed Bomb", "Air Slash", "X-Scissor", "Bug Buzz", "Dragon Pulse", "Dragon Rush", "Power Gem", "Drain Punch", "Vacuum Wave", "Focus Blast", "Energy Ball", "Brave Bird", "Earth Power", "Switcheroo", "Giga Impact", "Nasty Plot", "Bullet Punch", "Avalanche", "Ice Shard", "Shadow Claw", "Thunder Fang", "Ice Fang", "Fire Fang", "Shadow Sneak", "Mud Bomb", "Psycho Cut", "Zen Headbutt", "Mirror Shot", "Flash Cannon", "Rock Climb", "Defog", "Trick Room", "Draco Meteor", "Discharge", "Lava Plume", "Leaf Storm", "Power Whip", "Rock Wrecker", "Cross Poison", "Gunk Shot", "Iron Head", "Magnet Bomb", "Stone Edge", "Captivate", "Stealth Rock", "Grass Knot", "Chatter", "Judgment", "Bug Bite", "Charge Beam", "Wood Hammer", "Aqua Jet", "Attack Order", "Defend Order", "Heal Order", "Head Smash", "Double Hit", "Roar of Time", "Spacial Rend", "Lunar Dance", "Crush Grip", "Magma Storm", "Dark Void", "Seed Flare", "Ominous Wind", "Shadow Force"};

/* STRUCTS */

typedef struct {
  u32 pid;
  u16 bef;
  u16 checksum;
  u8 order;
  u8 pos_a;
  u8 pos_b;
  u8 pos_c;
  u8 pos_d;
  u8 nature;
  u8 ivs[STATS];
  u16 iv1;
  u16 iv2;
  u8 bstats[STATS];
  u16 data[BLOCKS][BLOCK_SIZE];
  u16 cond[COND_SIZE];
} Pkmn;

typedef struct {
  u16 version;
  u16 language;
  u16 tid;
  u16 sid;
  u32 seed;
  u32 frames;
  u16 species;
  u16 item;
  u16 move;
  u32 aslr;
} User;

/* METHODS */

void ScanValue(u8 message[], u32 *value, u8 format[], u64 max) {
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

u8 GetNatureId(u32 pid){
  /* Get the ID of the Nature (from 0 to 24), provided the PID. */
  return pid%25;
}

u8 GetFormId(u8 form){
  /* Get the form ID from the form byte */
  return form >> 3;
}

u8 BlockOrder(u32 pid){
  /* Get the index of the block permutation of a given PID (from 0 to 23) */
  return ((pid & 0x3E000) >> 13) % BLOCK_PERM;
}

u8 PositionOfBlock(u8 block_id, u8 block_char){
  /* Return the positional index of a block given the index of the current block permutation. */
  u8 *str = Orders[block_id];
  u8 *ch = strchr(str, block_char);
  return ch - str;
}

u8 RngPosOfBlock(u8 pos){
  /* Distance in RNG cycles from Checksum to the beginning of this Block (1, 17, 33 or 49) */
  return pos*BLOCK_SIZE + 1;
}

u16 StatNatureModifier(u8 nature, u8 stat_index, u16 stat_value){
  /* Calculate and return the new value of a stat after the Nature modifier is applied.*/
  s8 m = NatureStatModifiers[nature][stat_index];
  return stat_value * (10 + m) / 10;
}

u16 IvToStat(Pkmn *pkmn, u8 stat) {
  /* Calculate the value of a stat based on the IV, Base Stat, Nature and Level. */
  u8 lv = pkmn->cond[2];
  u16 val;
  if (stat == 0) { //hp
    val = (2*(pkmn->bstats[stat]) + pkmn->ivs[stat])*lv/100 + lv + 10;
  }
  else { //others
    val = (2*(pkmn->bstats[stat]) + pkmn->ivs[stat])*lv/100 + 5;
    val = StatNatureModifier(pkmn->nature, stat-1, val); //ignore for hp (index 0), hence the -1
  }
  return val;
}

void SetCheckum(Pkmn *pkmn) {
  /* Return the checksum of a Pkmn by adding all of its block data into a 16-bit word. */
  u16 c = 0;
  for (u8 i = 0; i < BLOCK_SIZE; i++) {
    c += pkmn->data[pkmn->pos_a][i] + pkmn->data[pkmn->pos_b][i] + pkmn->data[pkmn->pos_c][i] + pkmn->data[pkmn->pos_d][i];
  }
  pkmn->checksum = c;
}

bool IsBadEgg(u16 badeggflag) {
  /* Check if the bad egg flag is set by looking at bit 2 of the "bad egg" 16-bit word. */
  return (bool)((badeggflag & 4) == 4);
}

bool IsEgg(u16 eggflag) {
  /* Check if the egg flag is set by looking at bit 30 of the "iv2" 16-bit word. */
  return (bool)((eggflag & 0x4000) == 0x4000);
}

bool IsFatefulEncounter(u16 fateflag) {
  /* Check if the fateful encounter bit is set. */
  return (bool)((fateflag & 1) == 1);
}

bool SkippedCheckum(u16 badeggflag) {
  /* Check if the checksum was skipped by looking at bit 0 and 1 of the "bad egg" 16-bit word. */
  return (bool)(((badeggflag & 1) == 1) && ((badeggflag & 2) == 2));
}

bool IsShiny(u32 pid, u16 tid, u16 sid) {
  /* Check if a pkmn is shiny by xoring its pid (top and bottom 16 bits), tid and sid */
  return (bool)(((pid & 0xffff) ^ (pid >> 16) ^ tid ^ sid) < 8);
}

u32 Rng_32(u32 seed, u16 iter) {
  /* General purpose LCRNG */
  u32 state = seed;
  for (u16 i = 0; i < iter; i++) {
    state = state * 0x41C64E6D + 0x6073;
  }
  return state;
}

u16 Rng_t16(u32 seed, u16 iter) {
  /* General purpose LCRNG, return only the 16 most significant bits */
  u32 state = seed;
  u16 top16 = 0;
  for (u16 i = 0; i < iter; i++) {
    state = state * 0x41C64E6D + 0x6073;
    top16 = state >> 16;
  }
  return top16;
}

void Encrypt(Pkmn *pkmn) {
  /* Encrypt with the XOR and RNG method each 16-bit word of Pkmn data. */
  /* First with the Checksum as the Seed/Key (for block data), then with the PID (for condition data). */
  /* Finally, assign each of the ABCD block in its corresponding place in Pkmn data. */
  pkmn->pos_a = PositionOfBlock(pkmn->order, 'A');
  pkmn->pos_b = PositionOfBlock(pkmn->order, 'B');
  pkmn->pos_c = PositionOfBlock(pkmn->order, 'C');
  pkmn->pos_d = PositionOfBlock(pkmn->order, 'D');
  u8 rng_a = RngPosOfBlock(pkmn->pos_a);
  u8 rng_b = RngPosOfBlock(pkmn->pos_b);
  u8 rng_c = RngPosOfBlock(pkmn->pos_c);
  u8 rng_d = RngPosOfBlock(pkmn->pos_d);
  for (u8 i = 0; i < BLOCK_SIZE; i++) {
    pkmn->data[pkmn->pos_a][i] ^= Rng_t16(pkmn->checksum, rng_a + i);
    pkmn->data[pkmn->pos_b][i] ^= Rng_t16(pkmn->checksum, rng_b + i);
    pkmn->data[pkmn->pos_c][i] ^= Rng_t16(pkmn->checksum, rng_c + i);
    pkmn->data[pkmn->pos_d][i] ^= Rng_t16(pkmn->checksum, rng_d + i);
  }
  for (u8 i = 0; i < COND_SIZE; i++) {
    pkmn->cond[i] ^= Rng_t16(pkmn->pid, i+1);
  }
}

void MethodJSeedToPID(u32 seed, Pkmn *pkmn) {
  /* Calculate PID, Nature and IVs according to Method J Stationary (no Synchronize) from a given Seed */
  u16 frame = 1;
  pkmn->nature = Rng_t16(seed, frame) / 0x0A3E;
  frame += 1;
  u32 pid = (Rng_t16(seed, frame)) | (Rng_t16(seed, frame+1) << 16);
  while (pid % 25 != pkmn->nature) { //until the 2 natures are the same, reroll PID
    frame += 2;
    pid = (Rng_t16(seed, frame)) | (Rng_t16(seed, frame+1) << 16);
  }
  pkmn->pid = pid;
  u16 iv1 = Rng_t16(seed, frame+2);
  u16 iv2 = Rng_t16(seed, frame+3);
  pkmn->ivs[hp] = iv1 & 31;
  pkmn->ivs[at] = (iv1 >> 5) & 31;
  pkmn->ivs[df] = (iv1 >> 10) & 31;
  pkmn->ivs[sp] = iv2 & 31;
  pkmn->ivs[sa] = (iv2 >> 5) & 31;
  pkmn->ivs[sd] = (iv2 >> 10) & 31;
  u32 ivsum = (pkmn->ivs[hp] << 0) | (pkmn->ivs[at] << 5) | (pkmn->ivs[df] << 10) | (pkmn->ivs[sp] << 15) | (pkmn->ivs[sa] << 20) | (pkmn->ivs[sd] << 25);
  pkmn->iv1 = ivsum & 0xffff;
  pkmn->iv2 = ivsum >> 16;
}

/* MAIN */

int main()
{

  User user = {0}; //0 init

  u8 answer;
  ScanValue("Use previous user settings (0=no, 1=yes): ", &answer, "%u", 1);

  if (answer == 1) { //use saved user data in Profile.txt
    FILE *profile;
    profile = fopen("Profile.txt", "r"); //read only
    //check for empty file before user scan!
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

    u8 save_user;
    ScanValue("Save those user settings? (0=no, 1=yes) ", &save_user, "%u", 1);

    if (save_user == 1) { //erase previous and save new user profile
      FILE *save_profile;
      save_profile = fopen("Profile.txt", "w+");
      fprintf(save_profile, "%u\n", user.version);
      fprintf(save_profile, "%u\n", user.language);
      fprintf(save_profile, "%u\n", user.tid);
      fprintf(save_profile, "%u\n", user.sid);
      fclose(save_profile);
    }
  }

  ScanValue("Search for a species (0=no, species_id=yes): ", &user.species, "%u", SPECIES+1);
  ScanValue("Search for an item (0=no, item_id=yes): ", &user.item, "%u", ITEMS+1);
  ScanValue("Search for a move (0=no, move_id=yes): ", &user.move, "%u", 0xffff);
  ScanValue("Enter your Seed (32 bit, hex): 0x", &user.seed, "%x", 0xffffffff);
  ScanValue("How many frames to search through (32 bit, dec): ", &user.frames, "%u", 0xffffffff);

  u8 *strlang = Languages[user.language];
  u8 *strvers = Versions[user.version];
  u8 *strspec = Pokelist[user.species];
  u8 *stritem = Items[user.item];

  u16 w_version = (user.version + 10) << 8; //convert for use in pkmn data
  u16 w_language = user.language << 8; //convert for use in pkmn data
  // user.aslr = 0x02271304;
  user.aslr = Aslrs[user.language][(u8)(user.version/2)]; //depends on language and version, only plat en and fr for now

  FILE *fp; //declare file object
  u8 *strfilename = "Results.txt"; //name of the file
  fp = fopen(strfilename, "w+"); //open/create file

  fprintf(fp, "> %s (%s)\n", strvers, strlang);
  fprintf(fp, "> TID = %u\n> SID = %u\n", user.tid, user.sid);
  fprintf(fp, "> Seed 0x%08X\n", user.seed);
  fprintf(fp, "> ASLR 0x%08X\n", user.aslr);
  fprintf(fp, "> Searched through %u frames for %s holding %s\n\n", user.frames, strspec, stritem);
  fprintf(fp, "Seed       | PID        | Level   | Species      | Form | Item           | Ability          | Hatch steps | Fateful | Shiny | IVs               | Moves\n");
  fprintf(fp, "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  printf("\n> %s (%s)\n", strvers, strlang);
  printf("> TID = %u\n> SID = %u\n", user.tid, user.sid);
  printf("> Seed 0x%08X\n", user.seed);
  printf("> ASLR 0x%08X\n", user.aslr);
  printf("> Searching through %u frames for %s holding %s...\n\n", user.frames, strspec, stritem);
  printf("Seed       | PID        | Level   | Species      | Form | Item           | Ability          | Hatch steps | Fateful | Shiny | IVs               | Moves\n");
  printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  u32 pid_list[PIDS_MAX] = {}; //0 init
  u32 results = 0;

  u32 seed = user.seed; //copy, advanced in the main loop

  clock_t begin = clock(); //timer starts

  /* Main search loop */
  for (u32 frame = 0; frame < user.frames; frame++){

    if (frame != 0) { //advance the RNG everytime, except on the 0th frame
      seed = Rng_32(seed, 1);
    }

    Pkmn wild = {0}; //0 init
    Pkmn seven = {0}; //0 init

    MethodJSeedToPID(seed, &wild);

    /* Checking for duplicate PIDs (is still letting some duplicates through?)*/
    bool duplicate = false;
    for (u16 i = 0; i < PIDS_MAX; i++) {
      if (pid_list[i] == wild.pid) {
        duplicate = true;
        break;
      } else if (pid_list[i] == 0) {
        pid_list[i] = wild.pid; //insert the new pid
        break;
      }
    }
    if (frame % PIDS_MAX == PIDS_MAX - 1) { //if filled, zero it out
      for (u16 i = 0; i < PIDS_MAX; i++) {
        pid_list[i] = 0;
      }
    }
    if (duplicate) { continue; }

    wild.order = BlockOrder(wild.pid);
    wild.pos_a = PositionOfBlock(wild.order, 'A');
    wild.pos_b = PositionOfBlock(wild.order, 'B');
    wild.pos_c = PositionOfBlock(wild.order, 'C');
    wild.pos_d = PositionOfBlock(wild.order, 'D');

    if (user.version == 2) { //for Platinum, init with Rotom
      wild.bstats[hp] = 50;
      wild.bstats[at] = 50;
      wild.bstats[df] = 77;
      wild.bstats[sp] = 91;
      wild.bstats[sa] = 95;
      wild.bstats[sd] = 77;

      wild.data[wild.pos_a][0] = 0x01DF; //species
      wild.data[wild.pos_a][2] = user.tid; //tid
      wild.data[wild.pos_a][3] = user.sid; //sid
      wild.data[wild.pos_a][4] = 0x1F40; //xp1 (depends also on version/level)
      wild.data[wild.pos_a][6] = 0x1A46; //ability and friendship concatenated
      wild.data[wild.pos_a][7] = w_language; //language

      wild.data[wild.pos_b][0] = 0x0054; //thundershock
      wild.data[wild.pos_b][1] = 0x006D; //confuse ray
      wild.data[wild.pos_b][2] = 0x00FD; //uproar
      wild.data[wild.pos_b][3] = 0x0068; //doubleteam
      wild.data[wild.pos_b][4] = 0x0A1E; //pp1and2
      wild.data[wild.pos_b][5] = 0x0F0A; //pp3and4
      wild.data[wild.pos_b][8] = wild.iv1;
      wild.data[wild.pos_b][9] = wild.iv2;
      wild.data[wild.pos_b][12] = 0x0004; //genderless

      if (user.language == 3) { //fr
        wild.data[wild.pos_c][0] = 0x0137; //M
        wild.data[wild.pos_c][1] = 0x0139; //O
        wild.data[wild.pos_c][2] = 0x013E; //T
        wild.data[wild.pos_c][3] = 0x0133; //I
        wild.data[wild.pos_c][4] = 0x013D; //S
        wild.data[wild.pos_c][5] = 0x0137; //M
        wild.data[wild.pos_c][6] = 0x012B; //A
        wild.data[wild.pos_c][7] = 0xffff; //terminator
      }
      else if (user.language == 1) { //jp
        wild.data[wild.pos_c][0] = 0x009E; //RO
        wild.data[wild.pos_c][1] = 0x0079; //TO
        wild.data[wild.pos_c][2] = 0x0091; //MU
        wild.data[wild.pos_c][3] = 0xffff; //terminator
      }
      else if (user.language == 8) { //ko
        wild.data[wild.pos_c][0] = 0x06C0; //RO
        wild.data[wild.pos_c][1] = 0x0BFA; //TO
        wild.data[wild.pos_c][2] = 0x0759; //MU
        wild.data[wild.pos_c][3] = 0xffff; //terminator
      }
      else { //en, it, ge, sp
        wild.data[wild.pos_c][0] = 0x013C; //R
        wild.data[wild.pos_c][1] = 0x0139; //O
        wild.data[wild.pos_c][2] = 0x013E; //T
        wild.data[wild.pos_c][3] = 0x0139; //O
        wild.data[wild.pos_c][4] = 0x0137; //M
        wild.data[wild.pos_c][5] = 0xffff; //terminator
      }

      wild.data[wild.pos_c][11] = w_version; //version

      wild.data[wild.pos_d][13] = 0x0400; //pokeball
      wild.data[wild.pos_d][14] = 0x0014; //level
    }
    else { //for Diamond and Pearl, init with Giratina
      wild.bstats[hp] = 150;
      wild.bstats[at] = 100;
      wild.bstats[df] = 120;
      wild.bstats[sp] = 90;
      wild.bstats[sa] = 100;
      wild.bstats[sd] = 120;

      wild.data[wild.pos_a][0] = 0x01E7; //species
      wild.data[wild.pos_a][2] = user.tid; //tid
      wild.data[wild.pos_a][3] = user.sid; //sid
      wild.data[wild.pos_a][4] = 0x8ACE; //xp1 (depends also on version/level)
      wild.data[wild.pos_a][5] = 0x0006; //xp1 (depends also on version/level)
      wild.data[wild.pos_a][6] = 0x2E00; //ability and friendship concatenated
      wild.data[wild.pos_a][7] = w_language; //language

      wild.data[wild.pos_b][0] = 0x01D3; //shadow force
      wild.data[wild.pos_b][1] = 0x0179; //heal block
      wild.data[wild.pos_b][2] = 0x019E; //earth power
      wild.data[wild.pos_b][3] = 0x00A3; //slash
      wild.data[wild.pos_b][4] = 0x0F05; //pp1and2
      wild.data[wild.pos_b][5] = 0x140A; //pp3and4
      wild.data[wild.pos_b][8] = wild.iv1;
      wild.data[wild.pos_b][9] = wild.iv2;
      wild.data[wild.pos_b][12] = 0x0004; //genderless

      if (user.language == 1) { //jp, wrong
        wild.data[wild.pos_c][0] = 0x009E; //RO
        wild.data[wild.pos_c][1] = 0x0079; //TO
        wild.data[wild.pos_c][2] = 0x0091; //MU
        wild.data[wild.pos_c][3] = 0xffff; //terminator
      }
      else if (user.language == 8) { //ko, wrong
        wild.data[wild.pos_c][0] = 0x06C0; //RO
        wild.data[wild.pos_c][1] = 0x0BFA; //TO
        wild.data[wild.pos_c][2] = 0x0759; //MU
        wild.data[wild.pos_c][3] = 0xffff; //terminator
      }
      else { //en, fr, it, ge, sp
        wild.data[wild.pos_c][0] = 0x0131; //G
        wild.data[wild.pos_c][1] = 0x0133; //I
        wild.data[wild.pos_c][2] = 0x013C; //R
        wild.data[wild.pos_c][3] = 0x012B; //A
        wild.data[wild.pos_c][4] = 0x013E; //T
        wild.data[wild.pos_c][5] = 0x0133; //I
        wild.data[wild.pos_c][6] = 0x0138; //N
        wild.data[wild.pos_c][7] = 0x012B; //A
        wild.data[wild.pos_c][8] = 0xffff; //terminator
      }

      wild.data[wild.pos_c][11] = w_version; //version

      wild.data[wild.pos_d][13] = 0x0400; //pokeball
      wild.data[wild.pos_d][14] = 0x0046; //level
    }

    wild.cond[2] = wild.data[wild.pos_d][14]; //level
    wild.cond[3] = IvToStat(&wild, hp);
    wild.cond[4] = wild.cond[3]; //current hp = max hp
    wild.cond[5] = IvToStat(&wild, at);
    wild.cond[6] = IvToStat(&wild, df);
    wild.cond[7] = IvToStat(&wild, sp);
    wild.cond[8] = IvToStat(&wild, sa);
    wild.cond[9] = IvToStat(&wild, sd);

    wild.cond[12] = w_language; //language again
    wild.cond[13] = 0xff00 + (w_version >> 8); //version variation
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
    seven.order = 2; //BlockOrder(seven.pid);
    seven.pos_a = 0; //PositionOfBlock(seven.order, 'A');
    seven.pos_b = 2; //PositionOfBlock(seven.order, 'B');
    seven.pos_c = 1; //PositionOfBlock(seven.order, 'C');
    seven.pos_d = 3; //PositionOfBlock(seven.order, 'D');

    /* Simulating the stack overflow */
    for (u8 i = 0; i < BLOCK_SIZE-RS_OFF; i++) {
      for (u8 j = 1; j < BLOCKS; j++) { //only need to start from j=1 bc block A is taken care of later.
        seven.data[j][i+RS_OFF] = wild.data[j-1][i]; //warning: negative index
      }
    }
    for (u8 i = 0; i < BLOCKS; i++) {
      for (u8 j = 0; j < BLOCKS-2; j++) {
        seven.data[j+2][i] = wild.data[j][BLOCK_SIZE-RS_OFF+i];
      }
    }
    for (u8 i = 0; i < COND_SIZE; i++) {
      if (i < RS_OFF) { seven.cond[i] = wild.data[2][BLOCK_SIZE-RS_OFF+i]; }
      else if (i < RS_OFF+BLOCK_SIZE) { seven.cond[i] = wild.data[3][i-RS_OFF]; }
      else { seven.cond[i] = wild.cond[i-RS_OFF-BLOCK_SIZE]; }
    }

    if (user.version == 2) { //platinum
      seven.data[seven.pos_a][0] = (u16)((user.aslr + LOC_BEG_OPP_PARTY_PL) & 0xffff);
      seven.data[seven.pos_a][1] = (u16)((user.aslr + LOC_BEG_OPP_PARTY_PL) >> 16);
      seven.data[seven.pos_a][2] = (u16)((user.aslr + LOC_END_OPP_PARTY_PL) & 0xffff);
      seven.data[seven.pos_a][3] = (u16)((user.aslr + LOC_END_OPP_PARTY_PL) >> 16);
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
      seven.data[seven.pos_a][0] = (u16)((user.aslr + LOC_BEG_OPP_PARTY_DP) & 0xffff);
      seven.data[seven.pos_a][1] = (u16)((user.aslr + LOC_BEG_OPP_PARTY_DP) >> 16);
      seven.data[seven.pos_a][2] = (u16)((user.aslr + LOC_END_OPP_PARTY_DP) & 0xffff);
      seven.data[seven.pos_a][3] = (u16)((user.aslr + LOC_END_OPP_PARTY_DP) >> 16);
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
    if ((ballid > 16) || (ballid == 0)) { continue; } //might be more complex than that

    SetCheckum(&seven);
    Encrypt(&seven);

    /* Is a bad egg or checksum was not skipped */
    wild.bef = seven.data[seven.pos_c][2];
    if (IsBadEgg(wild.bef)) { continue; }
    if (!SkippedCheckum(wild.bef)) { continue; }

    wild.pid = seven.data[seven.pos_c][0] | (seven.data[seven.pos_c][1] << 16); //don't actually need the top part I think
    wild.order = BlockOrder(wild.pid);
    u8 perm_a = PositionOfBlock(wild.order, 'A'); //for the species, item, ability and steps to hatch
    u8 perm_b = PositionOfBlock(wild.order, 'B'); //for the moves, IVs and fateful encounter flag

    /* Get final species, item, ability and steps to hatch */
    u16 f_species;
    u16 f_item;
    u8 f_ability;
    u16 f_steps;
    if (perm_a == 3) {
      f_species = seven.cond[RS_OFF];
      f_item = seven.cond[RS_OFF+1];
      f_ability = seven.cond[RS_OFF+6] >> 8;
      f_steps = seven.cond[RS_OFF+6] & 0xff;
    }
    else {
      f_species = seven.data[perm_a+1][RS_OFF];
      f_item = seven.data[perm_a+1][RS_OFF+1];
      f_ability = seven.data[perm_a+1][RS_OFF+6] >> 8;
      f_steps = seven.data[perm_a+1][RS_OFF+6] & 0xff;
    }

    /* Species check */
    if (user.species == 0) { //user didn't specify a species
      if (f_species >= SPECIES) { continue; } //any valid species
    }
    else if (f_species != user.species) { continue; } //match user.species

    /* Search for specific item */
    if (user.item != 0 && f_item != user.item) { continue; }

    /* Get final moveset, egg steps, form id and fateful encounter flag */
    u16 fate;
    u16 moves[4];
    if (perm_b == 3) {
      for (u8 i = 0; i < 4; i++) {
        moves[i] = seven.cond[RS_OFF+i];
      }
      fate = seven.cond[RS_OFF+12]; //16
      wild.iv1 = seven.cond[RS_OFF+8];
      wild.iv2 = seven.cond[RS_OFF+9];
    }
    else if (perm_b == 2) {
      for (u8 i = 0; i < 4; i++) {
        moves[i] = seven.cond[RS_OFF+i];
      }
      fate = seven.cond[0];
      wild.iv1 = seven.cond[RS_OFF+8];
      wild.iv2 = seven.cond[RS_OFF+9];
    }
    else {
      for (u8 i = 0; i < 4; i++) {
        moves[i] = seven.data[perm_b+1][RS_OFF+i];
      }
      fate = seven.data[perm_b+2][0];
      wild.iv1 = seven.data[perm_b+1][RS_OFF+8];
      wild.iv2 = seven.data[perm_b+1][RS_OFF+9];
    }

    /* Decompose IVs */
    wild.ivs[hp] = wild.iv1 & 31;
    wild.ivs[at] = (wild.iv1 >> 5) & 31;
    wild.ivs[df] = (wild.iv1 >> 10) & 31;
    wild.ivs[sp] = wild.iv2 & 31;
    wild.ivs[sa] = (wild.iv2 >> 5) & 31;
    wild.ivs[sd] = (wild.iv2 >> 10) & 31;

    /* Calculate steps to hatch if Egg, zero if not */
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
    u8 *fateful;
    if (IsFatefulEncounter(fate)) { fateful = "Fateful"; }
    else { fateful = "-------"; }

    /* Species string format */
    u8 *str_f_species;
    if (f_species > SPECIES) { str_f_species = "Glitchmon"; } //invalid index after Arceus
    else if (f_species == 0) { str_f_species = "DPBox"; } //the empty Pokémon
    else { str_f_species = Pokelist[f_species]; } //get Pokémon name from index

    /* Item string format */
    u8 *str_f_item;
    if (f_item > ITEMS) { str_f_item = "Glitch Item"; } //invalid index
    else if (f_item == 0) { str_f_item = "None"; } //no item if index is 0
    else { str_f_item = Items[f_item]; } //get item name from index

    /* Ability string format */
    u8 *str_f_abi;
    if (f_ability > ABILITIES) { str_f_abi = "Glitch Ability"; } //invalid index
    else if (f_ability == 0) { str_f_abi = "None"; } //no ability if index is 0
    else { str_f_abi = Abilities[f_ability]; } //get ability name from index

    /* Moves string format */
    u8 strmoves[4][14] = {};
    for (u8 i = 0; i < 4; i++) {
      if (moves[i] > MOVES) {
        u8 buffer[14];
        sprintf(buffer, "0x%04X", moves[i]);
        strcpy(strmoves[i], buffer);
      }
      else {
        u8 *buffer = Moves[moves[i]];
        strcpy(strmoves[i], buffer);
      }
    }

    /* Get shinyness */
    u8 *shiny;
    if (IsShiny(wild.pid, user.tid, user.sid)) { shiny = "Shiny"; }
    else { shiny = "-----"; }

    /* Print successful result */
    fprintf(fp, "0x%08X | 0x%08X | Lv. %-3d | %-12s | %-4d | %-14s | %-16s | %-5d steps | %s | %s | ", seed, wild.pid, f_level, str_f_species, form, str_f_item, str_f_abi, f_steps, fateful, shiny);
    fprintf(fp, "%02d/%02d/%02d/%02d/%02d/%02d | ", wild.ivs[hp], wild.ivs[at], wild.ivs[df], wild.ivs[sa], wild.ivs[sd], wild.ivs[sp]);
    fprintf(fp, "%s, %s, %s, %s\n", strmoves[0], strmoves[1], strmoves[2], strmoves[3]);

    printf("0x%08X | 0x%08X | Lv. %-3d | %-12s | %-4d | %-14s | %-16s | %-5d steps | %s | %s | ", seed, wild.pid, f_level, str_f_species, form, str_f_item, str_f_abi, f_steps, fateful, shiny);
    printf("%02d/%02d/%02d/%02d/%02d/%02d | ", wild.ivs[hp], wild.ivs[at], wild.ivs[df], wild.ivs[sa], wild.ivs[sd], wild.ivs[sp]);
    printf("%s, %s, %s, %s\n", strmoves[0], strmoves[1], strmoves[2], strmoves[3]);

    results++;
  }

  /* End of main loop */

  clock_t end = clock(); //end timer
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC; //calculate time elapsed since start of search, possible underflow?
  fprintf(fp, "\nFound %u results in %.1f seconds.\n", results, time_spent);
  printf("\nFound %u results in %.1f seconds.\n", results, time_spent);
  fclose(fp); //close file
  u8 exit;
  scanf("%s", &exit); //scan to halt execution
  return 0;
}
