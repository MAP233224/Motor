/******************************/
/*       Motor.c by MAP       */
/******************************/

/* INCLUDE */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


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

#define LOC_BEG_OPP_PARTY (0x58864) //Platinum
#define LOC_END_OPP_PARTY (0x593C4) //Platinum

#define SEED_MAX_A (256) //month & day
#define SEED_MAX_B (24) //hour
#define SEED_MIN_C (0x0300) //delay min
#define SEED_MAX_C (0x1000) //delay max

#define RS_OFF (4) //misalignment between rotom and seven
#define PIDS_MAX (1060) //calculated from a previous program that found the highest possible occidentary

char Languages[LANGUAGES][3] = {"_0", "jp", "en", "fr", "it", "ge", "_6", "sp", "ko"};

char Versions[VERSIONS][9] = {"Diamond", "Pearl", "Platinum"};

char Orders[BLOCK_PERM][BLOCKS+1] = {"ABCD", "ABDC", "ACBD", "ACDB", "ADBC", "ADCB", "BACD", "BADC", "BCAD", "BCDA", "BDAC", "BDCA", "CABD", "CADB", "CBAD", "CBDA", "CDAB", "CDBA", "DABC", "DACB", "DBAC", "DBCA", "DCAB", "DCBA"};

enum { hp=0, at, df, sp, sa, sd };

int NatureStatModifiers[NATURES][STATS-1] = {{0, 0, 0, 0, 0}, {1, -1, 0, 0, 0}, {1, 0, -1, 0, 0}, {1, 0, 0, -1, 0}, {1, 0, 0, 0, -1}, {-1, 1, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 1, -1, 0, 0}, {0, 1, 0, -1, 0}, {0, 1, 0, 0, -1}, {-1, 0, 1, 0, 0}, {0, -1, 1, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 1, -1, 0}, {0, 0, 1, 0, -1}, {-1, 0, 0, 1, 0}, {0, -1, 0, 1, 0}, {0, 0, -1, 1, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 1, -1}, {-1, 0, 0, 0, 1}, {0, -1, 0, 0, 1}, {0, 0, -1, 0, 1}, {0, 0, 0, -1, 1}, {0, 0, 0, 0, 0}};

// char Natures[NATURES][8] = {"Hardy", "Lonely", "Brave", "Adamant", "Naughty", "Bold", "Docile", "Relaxed", "Impish", "Lax", "Timid", "Hasty", "Serious", "Jolly", "Naive", "Modest", "Mild", "Quiet", "Bashful", "Rash", "Calm", "Gentle", "Sassy", "Careful", "Quirky"};

char Abilities[ABILITIES][16] = {"Cacophony", "Stench", "Drizzle", "Speed Boost", "Battle Armor", "Sturdy", "Damp", "Limber", "Sand Veil", "Static", "Volt Absorb", "Water Absorb", "Oblivious", "Cloud Nine", "Compound Eyes", "Insomnia", "Color Change", "Immunity", "Flash Fire", "Shield Dust", "Own Tempo", "Suction Cups", "Intimidate", "Shadow Tag", "Rough Skin", "Wonder Guard", "Levitate", "Effect Spore", "Synchronize", "Clear Body", "Natural Cure", "Lightning Rod", "Serene Grace", "Swift Swim", "Chlorophyll", "Illuminate", "Trace", "Huge Power", "Poison Point", "Inner Focus", "Magma Armor", "Water Veil", "Magnet Pull", "Soundproof", "Rain Dish", "Sand Stream", "Pressure", "Thick Fat", "Early Bird", "Flame Body", "Run Away", "Keen Eye", "Hyper Cutter", "Pickup", "Truant", "Hustle", "Cute Charm", "Plus", "Minus", "Forecast", "Sticky Hold", "Shed Skin", "Guts", "Marvel Scale", "Liquid Ooze", "Overgrow", "Blaze", "Torrent", "Swarm", "Rock Head", "Drought", "Arena Trap", "Vital Spirit", "White Smoke", "Pure Power", "Shell Armor", "Air Lock", "Tangled Feet", "Motor Drive", "Rivalry", "Steadfast", "Snow Cloak", "Gluttony", "Anger Point", "Unburden", "Heatproof", "Simple", "Dry Skin", "Download", "Iron Fist", "Poison Heal", "Adaptability", "Skill Link", "Hydration", "Solar Power", "Quick Feet", "Normalize", "Sniper", "Magic Guard", "No Guard", "Stall", "Technician", "Leaf Guard", "Klutz", "Mold Breaker", "Super Luck", "Aftermath", "Anticipation", "Forewarn", "Unaware", "Tinted Lens", "Filter", "Slow Start", "Scrappy", "Storm Drain", "Ice Body", "Solid Rock", "Snow Warning", "Honey Gather", "Frisk", "Reckless", "Multitype", "Flower Gift", "Bad Dreams"};

char Pokelist[SPECIES][12] = {"DPBox", "Bulbasaur", "Ivysaur", "Venusaur", "Charmander", "Charmeleon", "Charizard", "Squirtle", "Wartortle", "Blastoise", "Caterpie", "Metapod", "Butterfree", "Weedle", "Kakuna", "Beedrill", "Pidgey", "Pidgeotto", "Pidgeot", "Rattata", "Raticate", "Spearow", "Fearow", "Ekans", "Arbok", "Pikachu", "Raichu", "Sandshrew","Sandslash", "Nidoran-F", "Nidorina", "Nidoqueen", "Nidoran-M", "Nidorino", "Nidoking", "Clefairy", "Clefable", "Vulpix", "Ninetales", "Jigglypuff", "Wigglytuff", "Zubat", "Golbat", "Oddish", "Gloom", "Vileplume", "Paras", "Parasect", "Venonat", "Venomoth", "Diglett", "Dugtrio", "Meowth", "Persian", "Psyduck", "Golduck", "Mankey", "Primeape", "Growlithe", "Arcanine", "Poliwag", "Poliwhirl", "Poliwrath", "Abra", "Kadabra", "Alakazam", "Machop", "Machoke", "Machamp", "Bellsprout", "Weepinbell", "Victreebel", "Tentacool", "Tentacruel", "Geodude", "Graveler", "Golem", "Ponyta", "Rapidash", "Slowpoke", "Slowbro", "Magnemite", "Magneton", "Farfetch'd", "Doduo", "Dodrio", "Seel", "Dewgong", "Grimer", "Muk", "Shellder", "Cloyster", "Gastly", "Haunter", "Gengar", "Onix", "Drowzee", "Hypno", "Krabby", "Kingler", "Voltorb", "Electrode", "Exeggcute", "Exeggutor", "Cubone", "Marowak", "Hitmonlee", "Hitmonchan", "Lickitung", "Koffing", "Weezing", "Rhyhorn", "Rhydon", "Chansey", "Tangela", "Kangaskhan", "Horsea", "Seadra", "Goldeen", "Seaking", "Staryu", "Starmie", "Mr.Mime", "Scyther", "Jynx", "Electabuzz", "Magmar", "Pinsir", "Tauros", "Magikarp", "Gyarados", "Lapras", "Ditto", "Eevee", "Vaporeon", "Jolteon", "Flareon", "Porygon", "Omanyte", "Omastar", "Kabuto", "Kabutops", "Aerodactyl", "Snorlax", "Articuno", "Zapdos", "Moltres", "Dratini", "Dragonair", "Dragonite", "Mewtwo", "Mew", "Chikorita", "Bayleef", "Meganium", "Cyndaquil", "Quilava", "Typhlosion", "Totodile", "Croconaw", "Feraligatr", "Sentret", "Furret", "Hoothoot", "Noctowl", "Ledyba", "Ledian", "Spinarak", "Ariados", "Crobat", "Chinchou", "Lanturn", "Pichu", "Cleffa", "Igglybuff", "Togepi", "Togetic", "Natu", "Xatu", "Mareep", "Flaaffy", "Ampharos", "Bellossom", "Marill", "Azumarill", "Sudowoodo", "Politoed", "Hoppip", "Skiploom", "Jumpluff", "Aipom", "Sunkern", "Sunflora", "Yanma", "Wooper", "Quagsire", "Espeon", "Umbreon", "Murkrow", "Slowking", "Misdreavus", "Unown", "Wobbuffet", "Girafarig", "Pineco", "Forretress", "Dunsparce", "Gligar", "Steelix", "Snubbull", "Granbull", "Qwilfish", "Scizor", "Shuckle", "Heracross", "Sneasel", "Teddiursa", "Ursaring", "Slugma", "Magcargo", "Swinub", "Piloswine", "Corsola", "Remoraid", "Octillery", "Delibird", "Mantine", "Skarmory", "Houndour", "Houndoom", "Kingdra", "Phanpy", "Donphan", "Porygon-2", "Stantler", "Smeargle", "Tyrogue", "Hitmontop", "Smoochum", "Elekid", "Magby", "Miltank", "Blissey", "Raikou", "Entei", "Suicune", "Larvitar", "Pupitar", "Tyranitar", "Lugia", "Ho-Oh", "Celebi", "Treecko", "Grovyle", "Sceptile", "Torchic", "Combusken", "Blaziken", "Mudkip", "Marshtomp", "Swampert", "Poochyena", "Mightyena", "Zigzagoon", "Linoone", "Wurmple", "Silcoon", "Beautifly", "Cascoon", "Dustox", "Lotad", "Lombre", "Ludicolo", "Seedot", "Nuzleaf", "Shiftry", "Taillow", "Swellow", "Wingull", "Pelipper", "Ralts", "Kirlia", "Gardevoir", "Surskit", "Masquerain", "Shroomish", "Breloom", "Slakoth", "Vigoroth", "Slaking", "Nincada", "Ninjask", "Shedinja", "Whismur", "Loudred", "Exploud", "Makuhita", "Hariyama", "Azurill", "Nosepass", "Skitty", "Delcatty", "Sableye", "Mawile", "Aron", "Lairon", "Aggron", "Meditite", "Medicham", "Electrike", "Manectric", "Plusle", "Minun", "Volbeat", "Illumise", "Roselia", "Gulpin", "Swalot", "Carvanha", "Sharpedo", "Wailmer", "Wailord", "Numel", "Camerupt", "Torkoal", "Spoink", "Grumpig", "Spinda", "Trapinch", "Vibrava", "Flygon", "Cacnea", "Cacturne", "Swablu", "Altaria", "Zangoose", "Seviper", "Lunatone", "Solrock", "Barboach", "Whiscash", "Corphish", "Crawdaunt", "Baltoy", "Claydol", "Lileep", "Cradily", "Anorith", "Armaldo", "Feebas", "Milotic", "Castform", "Kecleon", "Shuppet", "Banette", "Duskull", "Dusclops", "Tropius", "Chimecho", "Absol", "Wynaut", "Snorunt", "Glalie", "Spheal", "Sealeo", "Walrein", "Clamperl", "Huntail", "Gorebyss", "Relicanth", "Luvdisc", "Bagon", "Shelgon", "Salamence", "Beldum", "Metang", "Metagross", "Regirock", "Regice", "Registeel", "Latias", "Latios", "Kyogre", "Groudon", "Rayquaza", "Jirachi", "Deoxys", "Turtwig", "Grotle", "Torterra", "Chimchar", "Monferno", "Infernape", "Piplup", "Prinplup", "Empoleon", "Starly", "Staravia", "Staraptor", "Bidoof", "Bibarel", "Kricketot", "Kricketune", "Shinx", "Luxio", "Luxray", "Budew", "Roserade", "Cranidos", "Rampardos", "Shieldon", "Bastiodon", "Burmy", "Wormadam", "Mothim", "Combee", "Vespiquen", "Pachirisu", "Buizel", "Floatzel", "Cherubi", "Cherrim", "Shellos", "Gastrodon", "Ambipom", "Drifloon", "Drifblim", "Buneary", "Lopunny", "Mismagius", "Honchkrow", "Glameow", "Purugly", "Chingling", "Stunky", "Skuntank", "Bronzor", "Bronzong", "Bonsly", "MimeJr.", "Happiny", "Chatot", "Spiritomb", "Gible", "Gabite", "Garchomp", "Munchlax", "Riolu", "Lucario", "Hippopotas", "Hippowdon", "Skorupi", "Drapion", "Croagunk", "Toxicroak", "Carnivine", "Finneon", "Lumineon", "Mantyke", "Snover", "Abomasnow", "Weavile", "Magnezone", "Lickilicky", "Rhyperior", "Tangrowth", "Electivire", "Magmortar", "Togekiss", "Yanmega", "Leafeon", "Glaceon", "Gliscor", "Mamoswine", "Porygon-Z", "Gallade", "Probopass", "Dusknoir", "Froslass", "Rotom", "Uxie", "Mesprit", "Azelf", "Dialga", "Palkia", "Heatran", "Regigigas", "Giratina", "Cresselia", "Phione", "Manaphy", "Darkrai", "Shaymin", "Arceus"};

char Items[ITEMS][14] = {"None", "Master Ball", "Ultra Ball", "Great Ball", "Poké Ball", "Safari Ball", "Net Ball", "Dive Ball", "Nest Ball", "Repeat Ball", "Timer Ball", "Luxury Ball", "Premier Ball", "Dusk Ball", "Heal Ball", "Quick Ball", "Cherish Ball", "Potion", "Antidote", "Burn Heal", "Ice Heal", "Awakening", "Paralyze Heal", "Full Restore", "Max Potion", "Hyper Potion", "Super Potion", "Full Heal", "Revive", "Max Revive", "Fresh Water", "Soda Pop", "Lemonade", "Moomoo Milk", "Energy Powder", "Energy Root", "Heal Powder", "Revival Herb", "Ether", "Max Ether", "Elixir", "Max Elixir", "Lava Cookie", "Berry Juice", "Sacred Ash", "HP Up", "Protein", "Iron", "Carbos", "Calcium", "Rare Candy", "PP Up", "Zinc", "PP Max", "Old Gateau", "Guard Spec", "Dire Hit", "X Attack", "X Defense", "X Speed", "X Accuracy", "X SpeAtk", "X SpeDef", "Poké Doll", "Fluffy Tail", "Blue Flute", "Yellow Flute", "Red Flute", "Black Flute", "White Flute", "Shoal Salt", "Shoal Shell", "Red Shard", "Blue Shard", "Yellow Shard", "Green Shard", "Super Repel", "Max Repel", "Escape Rope", "Repel", "Sun Stone", "Moon Stone", "Fire Stone", "Thunder Stone", "Water Stone", "Leaf Stone", "Tiny Mushroom", "Big Mushroom", "Pearl", "Big Pearl", "Stardust", "Star Piece", "Nugget", "Heart Scale", "Honey", "Growth Mulch", "Damp Mulch", "Stable Mulch", "Gooey Mulch", "Root Fossil", "Claw Fossil", "Helix Fossil", "Dome Fossil", "Old Amber", "Armor Fossil", "Skull Fossil", "Rare Bone", "Shiny Stone", "Dusk Stone", "Dawn Stone", "Oval Stone", "Odd Keystone", "Griseous Orb", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Adamant Orb", "Lustrous Orb", "Grass Mail", "Flame Mail", "Bubble Mail", "Bloom Mail", "Tunnel Mail", "Steel Mail", "Heart Mail", "Snow Mail", "Space Mail", "Air Mail", "Mosaic Mail", "Brick Mail", "Cheri Berry", "Chesto Berry", "Pecha Berry", "Rawst Berry", "Aspear Berry", "Leppa Berry", "Oran Berry", "Persim Berry", "Lum Berry", "Sitrus Berry", "Figy Berry", "Wiki Berry", "Mago Berry", "Aguav Berry", "Iapapa Berry", "Razz Berry", "Bluk Berry", "Nanab Berry", "Wepear Berry", "Pinap Berry", "Pomeg Berry", "Kelpsy Berry", "Qualot Berry", "Hondew Berry", "Grepa Berry", "Tamato Berry", "Cornn Berry", "Magost Berry", "Rabuta Berry", "Nomel Berry", "Spelon Berry", "Pamtre Berry", "Watmel Berry", "Durin Berry", "Belue Berry", "Occa Berry", "Passho Berry", "Wacan Berry", "Rindo Berry", "Yache Berry", "Chople Berry", "Kebia Berry", "Shuca Berry", "Coba Berry", "Payapa Berry", "Tanga Berry", "Charti Berry", "Kasib Berry", "Haban Berry", "Colbur Berry", "Babiri Berry", "Chilan Berry", "Liechi Berry", "Ganlon Berry", "Salac Berry", "Petaya Berry", "Apicot Berry", "Lansat Berry", "Starf Berry", "Enigma Berry", "Micle Berry", "Custap Berry", "Jaboca Berry", "Rowap Berry", "Bright Powder", "White Herb", "Macho Brace", "Exp Share", "Quick Claw", "Soothe Bell", "Mental Herb", "Choice Band", "King's Rock", "Silver Powder", "Amulet Coin", "Cleanse Tag", "Soul Dew", "Deep Sea Tooth", "Deep Sea Scale", "Smoke Ball", "Everstone", "Focus Band", "Lucky Egg", "Scope Lens", "Metal Coat", "Leftovers", "Dragon Scale", "Light Ball", "Soft Sand", "Hard Stone", "Miracle Seed", "Black Glasses", "Black Belt", "Magnet", "Mystic Water", "Sharp Beak", "Poison Barb", "Never-Melt Ice", "Spell Tag", "Twisted Spoon", "Charcoal", "Dragon Fang", "Silk Scarf", "Up-Grade", "Shell Bell", "Sea Incense", "Lax Incense", "Lucky Punch", "Metal Powder", "Thick Club", "Stick", "Red Scarf", "Blue Scarf", "Pink Scarf", "Green Scarf", "Yellow Scarf", "Wide Lens", "Muscle Band", "Wise Glasses", "Expert Belt", "Light Clay", "Life Orb", "Power Herb", "Toxic Orb", "Flame Orb", "Quick Powder", "Focus Sash", "Zoom Lens", "Metronome", "Iron Ball", "Lagging Tail", "Destiny Knot", "Black Sludge", "Icy Rock", "Smooth Rock", "Heat Rock", "Damp Rock", "Grip Claw", "Choice Scarf", "Sticky Barb", "Power Bracer", "Power Belt", "Power Lens", "Power Band", "Power Anklet", "Power Weight", "Shed Shell", "Big Root", "Choice Specs", "Flame Plate", "Splash Plate", "Zap Plate", "Meadow Plate", "Icicle Plate", "Fist Plate", "Toxic Plate", "Earth Plate", "Sky Plate", "Mind Plate", "Insect Plate", "Stone Plate", "Spooky Plate", "Draco Plate", "Dread Plate", "Iron Plate", "Odd Incense", "Rock Incense", "Full Incense", "Wave Incense", "Rose Incense", "Luck Incense", "Pure Incense", "Protector", "Electirizer", "Magmarizer", "Dubious Disc", "Reaper Cloth", "Razor Claw", "Razor Fang", "TM01", "TM02", "TM03", "TM04", "TM05", "TM06", "TM07", "TM08", "TM09", "TM10", "TM11", "TM12", "TM13", "TM14", "TM15", "TM16", "TM17", "TM18", "TM19", "TM20", "TM21", "TM22", "TM23", "TM24", "TM25", "TM26", "TM27", "TM28", "TM29", "TM30", "TM31", "TM32", "TM33", "TM34", "TM35", "TM36", "TM37", "TM38", "TM39", "TM40", "TM41", "TM42", "TM43", "TM44", "TM45", "TM46", "TM47", "TM48", "TM49", "TM50", "TM51", "TM52", "TM53", "TM54", "TM55", "TM56", "TM57", "TM58", "TM59", "TM60", "TM61", "TM62", "TM63", "TM64", "TM65", "TM66", "TM67", "TM68", "TM69", "TM70", "TM71", "TM72", "TM73", "TM74", "TM75", "TM76", "TM77", "TM78", "TM79", "TM80", "TM81", "TM82", "TM83", "TM84", "TM85", "TM86", "TM87", "TM88", "TM89", "TM90", "TM91", "TM92", "HM01", "HM02", "HM03", "HM04", "HM05", "HM06", "HM07", "HM08", "Explorer Kit", "Loot Sack", "Rule Book", "Poké Radar", "Point Card", "Journal", "Seal Case", "Fashion Case", "Seal", "Pal Pad", "Works Key", "Old Charm", "Galactic Key", "Red Chain", "Town Map", "VS Seeker", "Coin Case", "Old Rod", "Good Rod", "Super Rod", "Sprayduck", "Poffin Case", "Bicycle", "Suite Key", "Oak's Letter", "Lunar Wing", "Member Card", "Azure Flute", "S.S. Ticket", "Contest Pass", "Magma Stone", "Parcel", "Coupon 1", "Coupon 2", "Coupon 3", "Storage Key", "Secret Potion", "VS Recorder", "Gracidea", "Secret Key"};

char Moves[MOVES][14] = { "----" , "Pound", "Karate Chop", "Double Slap", "Comet Punch", "Mega Punch", "Pay Day", "Fire Punch", "Ice Punch", "Thunder Punch", "Scratch", "Vice Grip", "Guillotine", "Razor Wind", "Swords Dance", "Cut", "Gust", "Wing Attack", "Whirlwind", "Fly", "Bind", "Slam", "Vine Whip", "Stomp", "Double Kick", "Mega Kick", "Jump Kick", "Rolling Kick", "Sand Attack", "Headbutt", "Horn Attack", "Fury Attack", "Horn Drill", "Tackle", "Body Slam", "Wrap", "Take Down", "Thrash", "Double-Edge", "Tail Whip", "Poison Sting", "Twineedle", "Pin Missile", "Leer", "Bite", "Growl", "Roar", "Sing", "Supersonic", "Sonic Boom", "Disable", "Acid", "Ember", "Flamethrower", "Mist", "Water Gun", "Hydro Pump", "Surf", "Ice Beam", "Blizzard", "Psybeam", "Bubble Beam", "Aurora Beam", "Hyper Beam", "Peck", "Drill Peck", "Submission", "Low Kick", "Counter", "Seismic Toss", "Strength", "Absorb", "Mega Drain", "Leech Seed", "Growth", "Razor Leaf", "Solar Beam", "Poison Powder", "Stun Spore", "Sleep Powder", "Petal Dance", "String Shot", "Dragon Rage", "Fire Spin", "Thunder Shock", "Thunderbolt", "Thunder Wave", "Thunder", "Rock Throw", "Earthquake", "Fissure", "Dig", "Toxic", "Confusion", "Psychic", "Hypnosis", "Meditate", "Agility", "Quick Attack", "Rage", "Teleport", "Night Shade", "Mimic", "Screech", "Double Team", "Recover", "Harden", "Minimize", "Smokescreen", "Confuse Ray", "Withdraw", "Defense Curl", "Barrier", "Light Screen", "Haze", "Reflect", "Focus Energy", "Bide", "Metronome", "Mirror Move", "Self-Destruct", "Egg Bomb", "Lick", "Smog", "Sludge", "Bone Club", "Fire Blast", "Waterfall", "Clamp", "Swift", "Skull Bash", "Spike Cannon", "Constrict", "Amnesia", "Kinesis", "Soft-Boiled", "High Jump Kick", "Glare", "Dream Eater", "Poison Gas", "Barrage", "Leech Life", "Lovely Kiss", "Sky Attack", "Transform", "Bubble", "Dizzy Punch", "Spore", "Flash", "Psywave", "Splash", "Acid Armor", "Crabhammer", "Explosion", "Fury Swipes", "Bonemerang", "Rest", "Rock Slide", "Hyper Fang", "Sharpen", "Conversion", "Tri Attack", "Super Fang", "Slash", "Substitute", "Struggle", "Sketch", "Triple Kick", "Thief", "Spider Web", "Mind Reader", "Nightmare", "Flame Wheel", "Snore", "Curse", "Flail", "Conversion 2", "Aeroblast", "Cotton Spore", "Reversal", "Spite", "Powder Snow", "Protect", "Mach Punch", "Scary Face", "Feint Attack", "Sweet Kiss", "Belly Drum", "Sludge Bomb", "Mud-Slap", "Octazooka", "Spikes", "Zap Cannon", "Foresight", "Destiny Bond", "Perish Song", "Icy Wind", "Detect", "Bone Rush", "Lock-On", "Outrage", "Sandstorm", "Giga Drain", "Endure", "Charm", "Rollout", "False Swipe", "Swagger", "Milk Drink", "Spark", "Fury Cutter", "Steel Wing", "Mean Look", "Attract", "Sleep Talk", "Heal Bell", "Return", "Present", "Frustration", "Safeguard", "Pain Split", "Sacred Fire", "Magnitude", "Dynamic Punch", "Megahorn", "Dragon Breath", "Baton Pass", "Encore", "Pursuit", "Rapid Spin", "Sweet Scent", "Iron Tail", "Metal Claw", "Vital Throw", "Morning Sun", "Synthesis", "Moonlight", "Hidden Power", "Cross Chop", "Twister", "Rain Dance", "Sunny Day", "Crunch", "Mirror Coat", "Psych Up", "Extreme Speed", "Ancient Power", "Shadow Ball", "Future Sight", "Rock Smash", "Whirlpool", "Beat Up", "Fake Out", "Uproar", "Stockpile", "Spit Up", "Swallow", "Heat Wave", "Hail", "Torment", "Flatter", "Will-O-Wisp", "Memento", "Facade", "Focus Punch", "Smelling Salts", "Follow Me", "Nature Power", "Charge", "Taunt", "Helping Hand", "Trick", "Role Play", "Wish", "Assist", "Ingrain", "Superpower", "Magic Coat", "Recycle", "Revenge", "Brick Break", "Yawn", "Knock Off", "Endeavor", "Eruption", "Skill Swap", "Imprison", "Refresh", "Grudge", "Snatch", "Secret Power", "Dive", "Arm Thrust", "Camouflage", "Tail Glow", "Luster Purge", "Mist Ball", "Feather Dance", "Teeter Dance", "Blaze Kick", "Mud Sport", "Ice Ball", "Needle Arm", "Slack Off", "Hyper Voice", "Poison Fang", "Crush Claw", "Blast Burn", "Hydro Cannon", "Meteor Mash", "Astonish", "Weather Ball", "Aromatherapy", "Fake Tears", "Air Cutter", "Overheat", "Odor Sleuth", "Rock Tomb", "Silver Wind", "Metal Sound", "Grass Whistle", "Tickle", "Cosmic Power", "Water Spout", "Signal Beam", "Shadow Punch", "Extrasensory", "Sky Uppercut", "Sand Tomb", "Sheer Cold", "Muddy Water", "Bullet Seed", "Aerial Ace", "Icicle Spear", "Iron Defense", "Block", "Howl", "Dragon Claw", "Frenzy Plant", "Bulk Up", "Bounce", "Mud Shot", "Poison Tail", "Covet", "Volt Tackle", "Magical Leaf", "Water Sport", "Calm Mind", "Leaf Blade", "Dragon Dance", "Rock Blast", "Shock Wave", "Water Pulse", "Doom Desire", "Psycho Boost", "Roost", "Gravity", "Miracle Eye", "Wake-Up Slap", "Hammer Arm", "Gyro Ball", "Healing Wish", "Brine", "Natural Gift", "Feint", "Pluck", "Tailwind", "Acupressure", "Metal Burst", "U-turn", "Close Combat", "Payback", "Assurance", "Embargo", "Fling", "Psycho Shift", "Trump Card", "Heal Block", "Wring Out", "Power Trick", "Gastro Acid", "Lucky Chant", "Me First", "Copycat", "Power Swap", "Guard Swap", "Punishment", "Last Resort", "Worry Seed", "Sucker Punch", "Toxic Spikes", "Heart Swap", "Aqua Ring", "Magnet Rise", "Flare Blitz", "Force Palm", "Aura Sphere", "Rock Polish", "Poison Jab", "Dark Pulse", "Night Slash", "Aqua Tail", "Seed Bomb", "Air Slash", "X-Scissor", "Bug Buzz", "Dragon Pulse", "Dragon Rush", "Power Gem", "Drain Punch", "Vacuum Wave", "Focus Blast", "Energy Ball", "Brave Bird", "Earth Power", "Switcheroo", "Giga Impact", "Nasty Plot", "Bullet Punch", "Avalanche", "Ice Shard", "Shadow Claw", "Thunder Fang", "Ice Fang", "Fire Fang", "Shadow Sneak", "Mud Bomb", "Psycho Cut", "Zen Headbutt", "Mirror Shot", "Flash Cannon", "Rock Climb", "Defog", "Trick Room", "Draco Meteor", "Discharge", "Lava Plume", "Leaf Storm", "Power Whip", "Rock Wrecker", "Cross Poison", "Gunk Shot", "Iron Head", "Magnet Bomb", "Stone Edge", "Captivate", "Stealth Rock", "Grass Knot", "Chatter", "Judgment", "Bug Bite", "Charge Beam", "Wood Hammer", "Aqua Jet", "Attack Order", "Defend Order", "Heal Order", "Head Smash", "Double Hit", "Roar of Time", "Spacial Rend", "Lunar Dance", "Crush Grip", "Magma Storm", "Dark Void", "Seed Flare", "Ominous Wind", "Shadow Force"};


/* PKMN STRUCT */

typedef struct {
  unsigned __int32 pid;
  unsigned __int16 bef;
  unsigned __int16 checksum;
  int order;
  int pos_a;
  int pos_b;
  int pos_c;
  int pos_d;
  int nature;
  int ivs[STATS];
  unsigned __int16 iv1;
  unsigned __int16 iv2;
  int bstats[STATS];
  unsigned __int16 data[BLOCKS][BLOCK_SIZE];
  unsigned __int16 cond[COND_SIZE];
} Pkmn;


/* FUNCTIONS */

int GetNatureId(unsigned __int32 pid){
    /* Get the ID of the Nature, provided the PID. */
    return pid%25;
}

// char* GetNatureString(int nature_id){
//   return Natures[nature_id];
// }

int BlockOrder(unsigned __int32 pid){
  /* Get the index of the block permutation of a given PID (from 0 to 23) */
  return ((pid & 0x3E000) >> 13) % BLOCK_PERM;
}

int PositionOfBlock(int block_id, char block_char){
  /* Return the positional index of a block given the index of the current block permutation. */
  char *str = Orders[block_id];
  char *ch;
  int pos;
  ch = strchr(str, block_char);
  pos = (int)(ch - str);
  return pos;
}

int RngPosOfBlock(int pos){
  /* Distance in RNG cycles from Checksum to the beginning of this Block (1, 17, 33 or 49) */
  return pos*BLOCK_SIZE + 1;
}

int StatNatureModifier(int nature, int stat_index, int stat_value){
  /* Calculate and return the new value of a stat after the Nature modifier is applied.*/
  int m = NatureStatModifiers[nature][stat_index];
  if (m == 1) {
    stat_value = (stat_value*110)/100;
  }
  else if (m == -1){
    stat_value = (stat_value*90)/100;
  }
  else {
    stat_value = stat_value;
  }
  return stat_value;
}

unsigned __int16 IvToStat(Pkmn *pkmn, int stat) {
  /* Calculate the value of a stat based on the IV, Base Stat, Nature and Level. */
  int level = pkmn->data[pkmn->pos_d][14];
  int val;
  if (stat == 0) { //hp
    val = (2*(pkmn->bstats[stat]) + pkmn->ivs[stat])*level/100 + level + 10;
  }
  else { //others
    val = (2*(pkmn->bstats[stat]) + pkmn->ivs[stat])*level/100 + 5;
    val = StatNatureModifier(pkmn->nature, stat-1, val); //ignore for hp (index 0), hence the -1
  }
  return (unsigned __int16) val;
}

int SetCheckum(Pkmn *pkmn) {
  /* Return the checksum of a Pkmn by adding all of its block data into a 16-bit word. */
  unsigned __int16 c = 0;
  for (int i = 0; i < BLOCK_SIZE; i++) {
    c += pkmn->data[pkmn->pos_a][i] + pkmn->data[pkmn->pos_b][i] + pkmn->data[pkmn->pos_c][i] + pkmn->data[pkmn->pos_d][i];
  }
  pkmn->checksum = c;
  // printf("\nChecksum = %04X\n", c);
  return 0;
}

bool IsBadEgg(unsigned __int16 badeggflag) {
  /* Check if the bad egg flag is set by looking at bit 2 of the bad egg 16-bit word. */
  if ((badeggflag & 4) == 4) {
    return true;
  }
  else {
    return false;
  }
}

bool IsFatefulEncounter(unsigned __int16 fateflag) {
  /* Check if the fateful encounter bit is set. */
  if ((fateflag & 1) == 1) {
    return true;
  }
  else {
    return false;
  }
}

bool SkippedCheckum(unsigned __int16 badeggflag) {
  /* Check if the checksum was skipped by looking at bit 0 and 1 of the bad egg 16-bit word. */
  if (((badeggflag & 1) == 1) && ((badeggflag & 2) == 2)) {
    return true;
  }
  else {
    return false;
  }
}

unsigned __int32 ReverseSeed(unsigned __int32 seed) {
  /* Find the nearest console-hitable seed provided the current state of the RNG. Print the number of iterations. */
  int frame = 0;
  unsigned __int32 state = seed;
  unsigned __int8 a = (seed >> 24) & 0xff;
  unsigned __int8 b = (seed >> 16) & 0xff;
  unsigned __int16 c = seed & 0xffff;
  while (b > SEED_MAX_B || c < SEED_MIN_C || c > SEED_MAX_C) {
    state = (state * 0xEEB9EB65 + 0xA3561A1) & 0xffffffff;
    a = (state >> 24) & 0xff;
    b = (state >> 16) & 0xff;
    c = state & 0xffff;
    frame += 1;
  }
  printf("%d frames", frame);
  return state;
}

unsigned __int32 RandomSeed() {
  /* Generate a random console-hitable RNG seed */
  unsigned __int8 a = rand() % SEED_MAX_A;
  unsigned __int8 b = rand() % SEED_MAX_B;
  unsigned __int16 c = rand() % (SEED_MAX_C-SEED_MIN_C);
  return (a << 24) | (b << 16) | c;
}

unsigned __int32 Rng_32(unsigned __int32 seed, unsigned __int16 iter) {
  unsigned __int32 state = seed;
  for (unsigned __int16 i = 0; i < iter; i++) {
      state = state * 0x41C64E6D + 0x6073;
  }
  return state;
}

unsigned __int16 Rng_t16(unsigned __int32 seed, unsigned __int16 iter) {
  unsigned __int32 state = seed;
  unsigned __int16 top16 = 0;
  for (unsigned __int16 i = 0; i < iter; i++) {
      state = state * 0x41C64E6D + 0x6073;
      top16 = state >> 16;
  }
  return top16;
}

int Encrypt(Pkmn *pkmn) {
  /* Encrypt with the XOR and RNG method each 16-bit word of Pkmn data. */
  /* First with the Checksum as the Seed/Key (for block data), then with the PID (for condition data). */
  /* Finally, assign each of the ABCD block in its corresponding place in Pkmn data. */
  int pos_a = PositionOfBlock(pkmn->order, 'A');
  int pos_b = PositionOfBlock(pkmn->order, 'B');
  int pos_c = PositionOfBlock(pkmn->order, 'C');
  int pos_d = PositionOfBlock(pkmn->order, 'D');
  int rng_a = RngPosOfBlock(pos_a);
  int rng_b = RngPosOfBlock(pos_b);
  int rng_c = RngPosOfBlock(pos_c);
  int rng_d = RngPosOfBlock(pos_d);
  for (int i = 0; i < BLOCK_SIZE; i++) {
    pkmn->data[pos_a][i] = pkmn->data[pos_a][i] ^ Rng_t16(pkmn->checksum, rng_a + i);
    pkmn->data[pos_b][i] = pkmn->data[pos_b][i] ^ Rng_t16(pkmn->checksum, rng_b + i);
    pkmn->data[pos_c][i] = pkmn->data[pos_c][i] ^ Rng_t16(pkmn->checksum, rng_c + i);
    pkmn->data[pos_d][i] = pkmn->data[pos_d][i] ^ Rng_t16(pkmn->checksum, rng_d + i);
  }
  for (int i = 0; i < COND_SIZE; i++) {
    pkmn->cond[i] = pkmn->cond[i] ^ Rng_t16(pkmn->pid, i+1);
  }
  return 0;
}

int MethodJSeedToPID(unsigned __int32 seed, Pkmn *pkmn) {
  /* Calculate PID, Nature and IVs according to Method J Stationary (no Synchronize) from a given Seed */
  int frame = 1;
  pkmn->nature = Rng_t16(seed, frame) / 0x0A3E;
  int call_1 = frame + 1;
  int call_2 = frame + 2;
  unsigned __int32 pid = (Rng_t16(seed, call_1)) | (Rng_t16(seed, call_2) << 16);
  while (pid % 25 != pkmn->nature) { //until the 2 natures are the same, reroll PID
    call_1 += 2;
    call_2 += 2;
    pid = (Rng_t16(seed, call_1)) | (Rng_t16(seed, call_2) << 16);
  }
  pkmn->pid = pid;
  unsigned __int16 iv1 = Rng_t16(seed, call_1 + 2);
  unsigned __int16 iv2 = Rng_t16(seed, call_2 + 2);
  pkmn->ivs[hp] = iv1 & 31;
  pkmn->ivs[at] = (iv1 >> 5) & 31;
  pkmn->ivs[df] = (iv1 >> 10) & 31;
  pkmn->ivs[sp] = iv2 & 31;
  pkmn->ivs[sa] = (iv2 >> 5) & 31;
  pkmn->ivs[sd] = (iv2 >> 10) & 31;
  unsigned __int32 ivsum = (pkmn->ivs[hp] << 0) | (pkmn->ivs[at] << 5) | (pkmn->ivs[df] << 10) | (pkmn->ivs[sp] << 15) | (pkmn->ivs[sa] << 20) | (pkmn->ivs[sd] << 25);
  pkmn->iv1 = ivsum & 0xffff;
  pkmn->iv2 = ivsum >> 16;
  return 0;
}


/* MAIN */

int main()
{
  srand(time(NULL)); //init rand, call once

  /* User inputs */
  unsigned __int16 user_vers = 0xffff;
  unsigned __int16 user_lang = 0xffff;
  unsigned __int16 user_tid = 0xffff;
  unsigned __int16 user_sid = 0xffff;
  unsigned __int32 user_seed = 0xffffffff;
  unsigned __int32 user_frames = 0xffffffff;
  unsigned __int32 user_aslr = 0x0227116C; //depends on language and version, use 0x0227116C for english plat

  do {
    printf("Enter your Version (0=Diamond, 1=Pearl, 2=Platinum): ");
    scanf("%hd", &user_vers);
  } while (user_vers > 2);
  do {
    printf("Enter your Language (1=jp, 2=en, 3=fr, 4=it, 5=ge, 7=sp, 8=ko): ");
    scanf("%hd", &user_lang);
  } while (user_lang > 8);
  do {
    printf("Enter your TID (0 to 65536): ");
    scanf("%hd", &user_tid);
  } while (user_tid == 0xffff);
  do {
    printf("Enter your SID (0 to 65536): ");
    scanf("%hd", &user_sid);
  } while (user_sid == 0xffff);
  do {
    printf("Enter your Seed (32 bit, hex): ");
    scanf("%X", &user_seed);
  } while (user_seed == 0xffffffff);
  do {
    printf("Enter your scope (32 bit, dec): ");
    scanf("%u", &user_frames);
  } while (user_frames == 0xffffffff);

  char *strlang = Languages[user_lang];
  char *strvers = Versions[user_vers];

  user_vers = (user_vers + 10) << 8; //convert for use in pkmn data
  user_lang = user_lang << 8; //convert for use in pkmn data

  printf("\n> %s (%s)\n", strvers, strlang);
  printf("> TID = %u | SID = %u\n", user_tid, user_sid);
  printf("> Seed 0x%08X\n", user_seed);
  printf("> ASLR 0x%08X\n", user_aslr);
  printf("> Searching through %u frames...\n\n", user_frames);
  printf("Seed       | Level   | Species      | Item           | Ability          | Hatch steps | Fateful | Moves\n");
  printf("--------------------------------------------------------------------------------------------------------------------------------\n");

  unsigned __int16 p1 = (user_aslr + LOC_BEG_OPP_PARTY) & 0xffff;
  unsigned __int16 p2 = (user_aslr + LOC_END_OPP_PARTY) & 0xffff;

  unsigned __int32 pid_list[PIDS_MAX] = {}; //0 init
  unsigned __int32 results = 0;

  unsigned __int32 seed = user_seed;//RandomSeed();

  clock_t begin = clock(); //timer starts

  /* Main search loop */
  for (int frame = 0; frame < user_frames; frame++){

    if (frame != 0) { //advance the RNG everytime, except on the 0th frame
      seed = Rng_32(seed, 1);
    }

    Pkmn rotom = {0};
    Pkmn seven = {0};

    MethodJSeedToPID(seed, &rotom);

    /* Checking for duplicate PIDs (is still letting some duplicates through?)*/
    bool duplicate = false;
    for (int i = 0; i < PIDS_MAX; i++) {
      if (pid_list[i] == rotom.pid) {
        duplicate = true;
        break;
      } else if (pid_list[i] == 0) {
        pid_list[i] = rotom.pid; //insert the new pid
        break;
      }
    }
    if (frame % PIDS_MAX == PIDS_MAX - 1) { //if filled, zero it out
      for (int i = 0; i < PIDS_MAX; i++) {
        pid_list[i] = 0;
      }
    }
    if (duplicate == true) {
      continue;
    }

    /* Initialize Rotom */
    rotom.bstats[hp] = 50;
    rotom.bstats[at] = 50;
    rotom.bstats[df] = 77;
    rotom.bstats[sp] = 91;
    rotom.bstats[sa] = 95;
    rotom.bstats[sd] = 77;

    rotom.order = BlockOrder(rotom.pid);
    rotom.pos_a = PositionOfBlock(rotom.order, 'A');
    rotom.pos_b = PositionOfBlock(rotom.order, 'B');
    rotom.pos_c = PositionOfBlock(rotom.order, 'C');
    rotom.pos_d = PositionOfBlock(rotom.order, 'D');

    rotom.data[rotom.pos_a][0] = 0x01DF; //species
    rotom.data[rotom.pos_a][2] = user_tid; //tid
    rotom.data[rotom.pos_a][3] = user_sid; //sid
    rotom.data[rotom.pos_a][4] = 0x1F40; //xp1 (depends also on version/level)
    rotom.data[rotom.pos_a][6] = 0x1A46; //ability and friendship concatenated
    rotom.data[rotom.pos_a][7] = user_lang; //language

    rotom.data[rotom.pos_b][0] = 0x0054; //thundershock
    rotom.data[rotom.pos_b][1] = 0x006D; //confuse ray
    rotom.data[rotom.pos_b][2] = 0x00FD; //uproar
    rotom.data[rotom.pos_b][3] = 0x0068; //doubleteam
    rotom.data[rotom.pos_b][4] = 0x0A1E; //pp1and2
    rotom.data[rotom.pos_b][5] = 0x0F0A; //pp3and4
    rotom.data[rotom.pos_b][8] = rotom.iv1;
    rotom.data[rotom.pos_b][9] = rotom.iv2;
    rotom.data[rotom.pos_b][12] = 0x0004; //genderless

    if (user_lang == 3) { //fr
      rotom.data[rotom.pos_c][0] = 0x0137; //M
      rotom.data[rotom.pos_c][1] = 0x0139; //O
      rotom.data[rotom.pos_c][2] = 0x013E; //T
      rotom.data[rotom.pos_c][3] = 0x0133; //I
      rotom.data[rotom.pos_c][4] = 0x013D; //S
      rotom.data[rotom.pos_c][5] = 0x0137; //M
      rotom.data[rotom.pos_c][6] = 0x012B; //A
      rotom.data[rotom.pos_c][7] = 0xffff; //terminator
    }
    else if (user_lang == 1) { //jp
      rotom.data[rotom.pos_c][0] = 0x009E; //RO
      rotom.data[rotom.pos_c][1] = 0x0079; //TO
      rotom.data[rotom.pos_c][2] = 0x0091; //MU
      rotom.data[rotom.pos_c][3] = 0xffff; //terminator
    }
    else if (user_lang == 8) { //ko
      rotom.data[rotom.pos_c][0] = 0x06C0; //RO
      rotom.data[rotom.pos_c][1] = 0x0BFA; //TO
      rotom.data[rotom.pos_c][2] = 0x0759; //MU
      rotom.data[rotom.pos_c][3] = 0xffff; //terminator
    }
    else { //en, it, ge, sp
      rotom.data[rotom.pos_c][0] = 0x013C; //R
      rotom.data[rotom.pos_c][1] = 0x0139; //O
      rotom.data[rotom.pos_c][2] = 0x013E; //T
      rotom.data[rotom.pos_c][3] = 0x0139; //O
      rotom.data[rotom.pos_c][4] = 0x0137; //M
      rotom.data[rotom.pos_c][5] = 0xffff; //terminator
    }

    rotom.data[rotom.pos_c][11] = user_vers; //version

    rotom.data[rotom.pos_d][13] = 0x0400; //pokeball
    rotom.data[rotom.pos_d][14] = 0x0014; //level

    rotom.cond[2] = rotom.data[rotom.pos_d][14]; //level
    rotom.cond[3] = IvToStat(&rotom, hp);
    rotom.cond[4] = rotom.cond[3]; //current hp = max hp
    rotom.cond[5] = IvToStat(&rotom, at);
    rotom.cond[6] = IvToStat(&rotom, df);
    rotom.cond[7] = IvToStat(&rotom, sp);
    rotom.cond[8] = IvToStat(&rotom, sa);
    rotom.cond[9] = IvToStat(&rotom, sd);
    rotom.cond[12] = 0x0200;
    rotom.cond[13] = 0xFF0C;
    rotom.cond[14] = 0xffff;
    rotom.cond[15] = 0xffff;
    rotom.cond[16] = 0xffff;
    rotom.cond[17] = 0xffff;
    rotom.cond[18] = 0xffff;
    rotom.cond[19] = 0xffff;
    rotom.cond[20] = 0xffff;
    rotom.cond[21] = 0xffff;
    rotom.cond[22] = 0xffff;
    rotom.cond[23] = 0xffff;
    rotom.cond[24] = 0xffff;
    // rotom.cond[25] = 0x0000;
    rotom.cond[26] = 0xffff;
    // rotom.cond[27] = 0x8910; //0x0149; //not the correct value after encryption???
    rotom.cond[28] = 0xffff;
    rotom.cond[29] = 0xffff;

    SetCheckum(&rotom);
    Encrypt(&rotom);

    /* Initialize Seven */
    seven.pid = 0x00005544;
    seven.bef = 0x05a4; //after checksum check, changed to a badegg (bit 4: 0->1)
    seven.order = BlockOrder(seven.pid);
    seven.pos_a = PositionOfBlock(seven.order, 'A');
    seven.pos_b = PositionOfBlock(seven.order, 'B');
    seven.pos_c = PositionOfBlock(seven.order, 'C');
    seven.pos_d = PositionOfBlock(seven.order, 'D');

    /* Simulating the stack overflow */
    for (int i = 0; i < BLOCK_SIZE-RS_OFF; i++) {
      for (int j = 0; j < BLOCKS; j++) {
        seven.data[j][i+RS_OFF] = rotom.data[j-1][i];
      }
    }
    for (int i = 0; i < BLOCKS; i++) {
      for (int j = 0; j < BLOCKS-2; j++) {
        seven.data[j+2][i] = rotom.data[j][BLOCK_SIZE-RS_OFF+i];
      }
    }
    for (int i = 0; i < COND_SIZE; i++) {
      if (i < RS_OFF) {
        seven.cond[i] = rotom.data[2][BLOCK_SIZE-RS_OFF+i];
      }
      else if (i < RS_OFF+BLOCK_SIZE) {
        seven.cond[i] = rotom.data[3][i-RS_OFF];
      }
      else {
        seven.cond[i] = rotom.cond[i-RS_OFF-BLOCK_SIZE];
      }
    }

    seven.data[seven.pos_a][0] = p1;
    seven.data[seven.pos_a][1] = 0x022c;
    seven.data[seven.pos_a][2] = p2;
    seven.data[seven.pos_a][3] = 0x022c;
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

    seven.data[seven.pos_c][0] = rotom.pid % 65536;
    seven.data[seven.pos_c][1] = rotom.pid >> 16;
    seven.data[seven.pos_c][2] = rotom.bef;
    seven.data[seven.pos_c][3] = rotom.checksum;

    Encrypt(&seven);

    /* If the ball doesn't have a valid id the battle won't load */
    unsigned __int8 ballid = seven.data[seven.pos_d][13] >> 8;
    if ((ballid > 16) || (ballid == 0)) {
      continue;
    }

    SetCheckum(&seven);
    Encrypt(&seven);

    /* Is a bad egg or checksum was not skipped */
    rotom.bef = seven.data[seven.pos_c][2];
    if (IsBadEgg(rotom.bef) == true) {
      continue;
    }
    if (SkippedCheckum(rotom.bef) == false) {
      continue;
    }

    rotom.pid = seven.data[seven.pos_c][0] | (seven.data[seven.pos_c][1] << 16); //don't actually need the top part I think
    rotom.order = BlockOrder(rotom.pid);
    int perm_a = PositionOfBlock(rotom.order, 'A'); //for the species
    int perm_b = PositionOfBlock(rotom.order, 'B'); //for the moves and fateful encounter flag

    /* Get final species, item, ability and steps to hatch */
    unsigned __int16 f_species;
    unsigned __int16 f_item;
    unsigned __int8 f_ability;
    unsigned __int16 f_steps;
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

    if (f_species >= SPECIES) { //skip if invalid species
      continue;
    }

    /* Search for valid item (disabled by default) */
    // if (f_item >= ITEMS) {
    //   continue;
    // }

    f_steps = (f_steps + 1) * 0xff; //multiply by egg cycle of 255

    /* Get final moveset and fateful encounter flag */
    unsigned __int16 fate;
    unsigned __int16 moves[4];
    if (perm_b == 3) {
      for (int i = 0; i < 4; i++) {
        moves[i] = seven.cond[RS_OFF+i];
      }
      fate = seven.cond[16];
    }
    else if (perm_b == 2) {
      for (int i = 0; i < 4; i++) {
        moves[i] = seven.cond[RS_OFF+i];
      }
      fate = seven.cond[0];
    }
    else {
      for (int i = 0; i < 4; i++) {
        moves[i] = seven.data[perm_b+1][RS_OFF+i];
      }
      fate = seven.data[perm_b+2][0];
    }

    /* Check for at least 1 legal move (disabled by default) */
    // if ((moves[0] >= MOVES) && (moves[1] >= MOVES) && (moves[2] >= MOVES) && (moves[3] >= MOVES)) {
    //   continue;
    // }

    /* Check for 4 legal moves (disabled by default) */
    // if ((moves[0] >= MOVES) || (moves[1] >= MOVES) || (moves[2] >= MOVES) || (moves[3] >= MOVES)) {
    //   continue;
    // }

    /* Get final level */
    unsigned __int8 f_level = seven.cond[22] & 0xff;

    /* Fateful encounter string definition */
    char *fateful;
    if (IsFatefulEncounter(fate) == true) {
      fateful = "Fateful";
    }
    else {
      fateful = "-------";
    }

    /* Species string format */
    char *strspecies;
    if (f_species > SPECIES) {
      strspecies = "Glitchmon";
    }
    else {
      strspecies = Pokelist[f_species];
    }

    /* Item string format */
    char *stritem;
    if (f_item > ITEMS) {
      stritem = "Glitch Item";
    }
    else {
      stritem = Items[f_item];
    }

    /* Ability string format */
    char *strabi;
    if (f_ability > ABILITIES) {
      strabi = "Glitch Ability";
    }
    else {
      strabi = Abilities[f_ability];
    }

    /* Moves string format */
    char strmoves[4][14] = {};
    for (int i = 0; i < 4; i++) {
      if (moves[i] > MOVES) {
        char buffer[14];
        sprintf(buffer, "0x%04X", moves[i]);
        strcpy(strmoves[i], buffer);
      }
      else {
        char *buffer = Moves[moves[i]];
        strcpy(strmoves[i], buffer);
      }
    }

    /* Print successful result */
    printf("0x%08X | Lv. %-3d | %-12s | %-14s | %-16s | %-5d steps | %s | ", seed, f_level, strspecies, stritem, strabi, f_steps, fateful);
    printf("%s, %s, %s, %s\n", strmoves[0], strmoves[1], strmoves[2], strmoves[3]);
    results += 1;

  }

  /* End of main loop */

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("\nFound %u results in %.1f seconds.\n", results, time_spent);
  // printf("Press Enter to exit.");
  char exit;
  scanf("%s", &exit);
  // getchar(); //can't use that!!!
  return 0;
}
