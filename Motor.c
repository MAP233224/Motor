/******************************/
/*       Motor.c by MAP       */
/******************************/

#include "common.h"

void ScanValue(u8* message, u32* value, u8* format, u64 max) {
	/* General purpose safe scan. Instruction message, value to change, string format and max value */
	do {
		printf("%s", message);
		u8 userInput[STRING_LENGTH_MAX];
		fgets(userInput, STRING_LENGTH_MAX, stdin);
		if (strlen(userInput) == 0 || strlen(userInput) >= STRING_LENGTH_MAX) {
#ifdef DEBUG
			printf("DEBUG: Invalid strlen()\n");
#endif
			continue;
		}
		if (sscanf(userInput, format, value) != 1) {
#ifdef DEBUG
			printf("DEBUG: Invalid sscanf()\n");
#endif
			*value = max + 1; //doesn't work for seed, puts it at 0 and becomes valid
			continue;
		}
	} while (*value > max);
}

void CreateProfile(User* user) {
	/* Scan for user info and ask if you want to save this new profile */
	u32 save_profile;
	ScanValue("Enter your Version (0=Diamond, 1=Pearl, 2=Platinum): ", &user->version, "%u", 2);
	ScanValue("Enter your Language (1=jp, 2=en, 3=fr, 4=it, 5=ge, 7=sp, 8=ko): ", &user->language, "%u", 8);
	ScanValue("Enter your TID (0 to 65535): ", &user->tid, "%u", 0xffff);
	ScanValue("Enter your SID (0 to 65535): ", &user->sid, "%u", 0xffff);
	ScanValue("Save those user settings? (0=no, 1=yes) ", &save_profile, "%u", 1);
	printf("\n");
	if (save_profile) { //Save the new profile to "Profile.txt"
		FILE* new_profile;
		new_profile = fopen("Profile.txt", "w+");
		fprintf(new_profile, "%u\n", user->version);
		fprintf(new_profile, "%u\n", user->language);
		fprintf(new_profile, "%u\n", user->tid);
		fprintf(new_profile, "%u\n", user->sid);
		fclose(new_profile);
	}
}

u8 GetNatureId(u32 pid) {
	/* Get the ID of the Nature (from 0 to 24), provided the PID. */
	return pid % NATURES_MAX;
}

u8 GetFormId(u8 form) {
	/* Get the form ID from the form byte */
	return form >> 3;
}

u8 BlockOrder(u32 pid) {
	/* Get the index of the block permutation of a given PID (from 0 to 23) */
	return ((pid & 0x3E000) >> 13) % BLOCK_PERMS;
}

void SetBlocks(Pkmn* pkmn) {
	/* Get the order of each block from the PID and set them in the correct permutation */
	/* r/iamverysmart */
	pkmn->order = BlockOrder(pkmn->pid);
	pkmn->pos_a = (Perms[pkmn->order] & 0xf000) >> 12;
	pkmn->pos_b = (Perms[pkmn->order] & 0x0f00) >> 8;
	pkmn->pos_c = (Perms[pkmn->order] & 0x00f0) >> 4;
	pkmn->pos_d = Perms[pkmn->order] & 0x000f;
}

u16 StatNatureModifier(u8 nature, u8 stat_index, u16 stat_value) {
	/* Return the new value of a stat after the Nature modifier is applied.*/
	return stat_value * (10 + NatureStatModifiers[nature][stat_index]) / 10;
}

u16 IvToStat_HP(Pkmn* pkmn, Original* wild) {
	/* Return the value of the HP stat based on the IV, Base Stat and Level. */
	return (2 * (wild->bstats[hp]) + pkmn->ivs[hp]) * wild->level / 100 + wild->level + 10;
}

u16 IvToStat(Pkmn* pkmn, Original* wild, u8 stat) {
	/* Return the value of a stat based on the IV, Base Stat, Nature and Level. */
	/* HP (index 0) is ignored, hence why "stat - 1" is passed as the stat_index */
	return StatNatureModifier(pkmn->nature, stat - 1, (2 * (wild->bstats[stat]) + pkmn->ivs[stat]) * wild->level / 100 + 5);
}

void SetCheckum(Pkmn* pkmn) {
	/* Set the checksum of a Pkmn by summing all of its Block data. */
	for (u8 i = 0; i < BLOCK_SIZE; i++) {
		pkmn->checksum += pkmn->data[pkmn->pos_a][i] + pkmn->data[pkmn->pos_b][i] + pkmn->data[pkmn->pos_c][i] + pkmn->data[pkmn->pos_d][i];
	}
}

bool IsEgg(u16 egg) {
	/* Check if the egg flag is set by looking at bit 30 of the "iv2" 16-bit word. */
	return (egg & 0x4000) == 0x4000;
}

bool IsFatefulEncounter(u16 fate) {
	/* Check if the fateful encounter bit is set. */
	return fate & 1;
}

bool IsShiny(u32 pid, u16 tid, u16 sid) {
	/* Check if a Pkmn is shiny by XORing its PID (top and bottom 16 bits), TID and SID */
	return ((pid & 0xffff) ^ (pid >> 16) ^ tid ^ sid) < 8;
}

bool IsInvalidPartyCount(u32 count) {
	/* Check if the number of members in the opponent's party is invalid. Determines crash at battle menu. */
	return ((count > 0x00000036) && (count < 0x80000000));
}

void SetString(u8* dest, u16 val, u8 array[][STRING_LENGTH_MAX], u16 max, u8* zero, u8* format) {
	/* Set dest string accoring to val */
	//If val is greater than the length of array, format it and copy the new buffer string into dest string
	if (val >= max) {
		u8 buffer[8];
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
	strcpy(dest, array[val]);
	return;
}

u32 RngNext(u32* state) {
	/* General purpose LCRNG, advance and return state */
	*state = *state * 0x41C64E6D + 0x6073;
	return *state;
}

void EncryptBlocks(Pkmn* pkmn) {
	/* LCRNG is seeded with the Checksum */
	/* Advance the LCRNG, XOR its 16 most significant bits with each 16-bit word of ABCD Block data */
	u32 state = pkmn->checksum;
	u16* data = (u16*)pkmn->data; //speed hack
	for (u8 i = 0; i < BLOCKS * BLOCK_SIZE; i++) {
		data[i] ^= (RngNext(&state) >> 16);
	}
}

void EncryptCondition(Pkmn* pkmn) {
	/* LCRNG is seeded with the PID */
	/* Advance the LCRNG, XOR its 16 most significant bits with each 16-bit word of Condition data */
	/* It is not needed to encrypt the whole 50 16-bit words of Condition data, I stop at 33 to include the Fateful encounter flag */
	u32 state = pkmn->pid;
	for (u8 i = 0; i < COND_SIZE_S; i++) {
		pkmn->cond[i] ^= (RngNext(&state) >> 16);
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

void MethodJSeedToPID(u32 state, Pkmn* pkmn) {
	/* Calculate PID, Nature and IVs according to Method J Stationary (no Synchronize) from a given state */
	pkmn->nature = (RngNext(&state) >> 16) / 0x0A3E;
	do { pkmn->pid = (RngNext(&state) >> 16) | (RngNext(&state) & 0xffff0000); } while (pkmn->pid % NATURES_MAX != pkmn->nature); //roll PID until the 2 natures are the same
	pkmn->iv1 = (RngNext(&state) >> 16) & 0x7FFF;
	pkmn->iv2 = (RngNext(&state) >> 16) & 0x7FFF;
	pkmn->iv1 |= (pkmn->iv2 & 1) << 15;
	pkmn->iv2 >>= 1;
}

void DebugPkmnData(Pkmn* pkmn) {
	/* Prints out the raw data of a Pkmn */
#ifdef DEBUG
	printf("PID: %08X\n", pkmn->pid);
	printf("Bad Egg flag: %04X\n", pkmn->bef);
	printf("Checksum: %04X\n", pkmn->checksum);
	u16* data = (u16*)pkmn->data;
	for (int i = 0; i < BLOCKS * BLOCK_SIZE + COND_SIZE; i++) {
		printf("%04X ", data[i]);
		if (i % 8 == 7) { printf("\n"); }
	}
	printf("\n");
#endif
}

/**************************************************************/

int main() {

	/* Display program name and version */
	printf("Motor v1.4.0");
#ifdef DEBUG
	printf(" (DEBUG)");
#endif
	printf("\n\n");

	User user = { 0 }; //Declare and initialize this session's User to 0

	u32 use_saved_profile;
	ScanValue("Use saved profile (0=no, 1=yes): ", &use_saved_profile, "%u", 1);
	if (use_saved_profile) { //Use saved user profile from "Profile.txt"
		FILE* profile_file_ptr;
		profile_file_ptr = fopen("Profile.txt", "r");
		if (profile_file_ptr == NULL) { //"Profile.txt" doesn't exist in the same directory as Motor.exe
			printf("Profile.txt not found in the program's directory. Please create a new profile:\n\n");
			CreateProfile(&user);
		}
		else { //Read the existing profile info from "Profile.txt"
			(void)fscanf(profile_file_ptr, "%u", &user.version);
			(void)fscanf(profile_file_ptr, "%u", &user.language);
			(void)fscanf(profile_file_ptr, "%u", &user.tid);
			(void)fscanf(profile_file_ptr, "%u", &user.sid);
			fclose(profile_file_ptr);
			printf("\n");
		}
	}
	else { CreateProfile(&user); }

	//Group Diamond and Pearl together
	u8 grouped_version = user.version >> 1;

	u32 og;
	do {
		if (user.version == 2) { //platinum
			ScanValue("Static PKMN you want to corrupt (0=Giratina-O, 1=Giratina-A, 2=Dialga, 3=Palkia, 4=Uxie, 5=Azelf, 6=Rotom): ", &og, "%u", OG_WILDS_MAX - 1);
		}
		else { //dp
			ScanValue("Static PKMN you want to corrupt (0=Giratina, 1=Arceus, 2=Dialga, 3=Palkia, 4=Shaymin, 5=Darkrai, 6=Uxie, 7=Azelf, 8=Rotom): ", &og, "%u", OG_WILDS_MAX - 1);
		}
	} while (OGW_LangVers[user.language][grouped_version][og] == NULL);

	do {
		ScanValue("ASLR to use (0~11 for jp, 0~4 for ko, 0~3 otherwise): ", &user.aslr, "%u", ASLR_GROUPS_MAX - 1);
	} while (Aslrs[user.language][grouped_version][user.aslr] == 0);

	ScanValue("Search for a species (0=no, species_id=yes): ", &user.species, "%u", SPECIES_MAX - 1);
	ScanValue("Search for an item (0=no, item_id=yes): ", &user.item, "%u", ITEMS_MAX - 1);
	ScanValue("Search for a move (0=no, move_id=yes): ", &user.move, "%u", 0xffff);
	ScanValue("Enter your Seed (32 bit, hex): 0x", &user.seed, "%x", 0xffffffff);
	ScanValue("How many frames to search through (32 bit, dec): ", &user.frames, "%u", 0xffffffff);
	ScanValue("Allow more seed options per result? (0=no, 1=yes): ", &user.dupe, "%u", 1);

	u8* str_lang = Languages[user.language];
	u8* str_vers = Versions[user.version];
	u8* str_species = Pokelist[user.species];
	u8* str_item = Items[user.item];
	u8* str_move;
	if (user.move == 0) { str_move = "anything"; }
	else if (user.move > MOVES_MAX) {
		u8 buffer[8];
		sprintf(buffer, "0x%04X", user.move);
		str_move = buffer;
	}
	else { str_move = Moves[user.move]; }

	u16 w_version = (user.version + 10) << 8; //convert for use in pkmn data
	u16 w_language = user.language << 8; //convert for use in pkmn data
	Original ogwild = *OGW_LangVers[user.language][grouped_version][og];

	FILE* fp; //declare file object
	u8 results_file_name[4 * STRING_LENGTH_MAX] = "Results_";
#ifdef DEBUG
	strcat(results_file_name, "DEBUG_");
#endif
	strcat(results_file_name, OgWilds[grouped_version][og % OG_WILDS_MAX]); strcat(results_file_name, "_"); //Original Wild
	u8 straslr[STRING_LENGTH_MAX]; sprintf(straslr, "%u", user.aslr); strcat(results_file_name, straslr); strcat(results_file_name, "_"); //ASLR
	strcat(results_file_name, str_vers); strcat(results_file_name, "_"); //Version
	strcat(results_file_name, str_lang); strcat(results_file_name, "_"); //Language
	u8 strtidsid[STRING_LENGTH_MAX]; sprintf(strtidsid, "%05u_%05u", user.tid, user.sid); strcat(results_file_name, strtidsid); strcat(results_file_name, ".txt"); //TID, SID
	fp = fopen(results_file_name, "w+"); //open/create file

	user.aslr = Aslrs[user.language][grouped_version][user.aslr]; //depends on language, version and user choice

	fprintf(fp, "> %s (%s)\n", str_vers, str_lang);
	fprintf(fp, "> TID = %u\n> SID = %u\n", user.tid, user.sid);
	fprintf(fp, "> Seed 0x%08X\n", user.seed);
	fprintf(fp, "> ASLR 0x%08X\n", user.aslr);
	fprintf(fp, "> Searched through %u frames for %s holding %s knowing %s\n\n", user.frames, str_species, str_item, str_move);
	fprintf(fp, "Seed       | PID        | Level   | Species      | Form | Item           | Ability          | Hatch steps | Fateful | Shiny | IVs               | Moves\n");
	fprintf(fp, "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\n> %s (%s)\n", str_vers, str_lang);
	printf("> TID = %u\n> SID = %u\n", user.tid, user.sid);
	printf("> Seed 0x%08X\n", user.seed);
	printf("> ASLR 0x%08X\n", user.aslr);
	printf("> Searching through %u frames for %s holding %s knowing %s...\n", user.frames, str_species, str_item, str_move);
#ifdef DEBUG
	printf("\nSeed       | PID        | Level   | Species      | Form | Item           | Ability          | Hatch steps | Fateful | Shiny | IVs               | Moves\n");
	printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
#endif

	u32 pid_list[PIDS_MAX] = { 0 };
	u32 results = 0;
	u32 seed = user.seed; //copy to advance in the main loop
	if (user.language == 8) { user.aslr += KOREAN_OFFSET; } //RAM thing
	u8 alternate_form = 0;
	if (user.version == 2 && og == 0) { alternate_form = 8; } //Giratina Origin

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

		/* Block A */
		wild.data[wild.pos_a][0] = ogwild.species; //species
		wild.data[wild.pos_a][1] = ogwild.item; //held item
		wild.data[wild.pos_a][2] = user.tid; //tid
		wild.data[wild.pos_a][3] = user.sid; //sid
		wild.data[wild.pos_a][4] = ogwild.xp1; //xp1
		wild.data[wild.pos_a][5] = ogwild.xp2; //xp2
		wild.data[wild.pos_a][6] = ogwild.frab; //ability and friendship concatenated
		wild.data[wild.pos_a][7] = w_language; //language
		/* Block B */
		for (u8 i = 0; i < OWN_MOVES_MAX; i++) { wild.data[wild.pos_b][i] = ogwild.moves[i]; }//4 moves
		wild.data[wild.pos_b][4] = ogwild.pp1and2; //pp1and2
		wild.data[wild.pos_b][5] = ogwild.pp3and4; //pp3and4
		wild.data[wild.pos_b][8] = wild.iv1;
		wild.data[wild.pos_b][9] = wild.iv2;
		wild.data[wild.pos_b][12] = 0x0004 | alternate_form; //0x0004 for genderless
		/* Block C */
		for (u8 i = 0; i < 11; i++) { wild.data[wild.pos_c][i] = ogwild.name[i]; } //11 characters for the name
		wild.data[wild.pos_c][11] = w_version; //version
		/* Block D */
		wild.data[wild.pos_d][13] = 0x0400; //pokeball
		wild.data[wild.pos_d][14] = ogwild.level; //level
		/* Condition data */
		wild.cond[2] = ogwild.level; //level again
		wild.cond[3] = IvToStat_HP(&wild, &ogwild);
		wild.cond[4] = wild.cond[3]; //current hp = max hp
		wild.cond[5] = IvToStat(&wild, &ogwild, at);
		wild.cond[6] = IvToStat(&wild, &ogwild, df);
		wild.cond[7] = IvToStat(&wild, &ogwild, sp);
		wild.cond[8] = IvToStat(&wild, &ogwild, sa);
		wild.cond[9] = IvToStat(&wild, &ogwild, sd);
		// wild.cond[10] = 0;
		// wild.cond[11] = 0;
		wild.cond[12] = w_language; //language again
		wild.cond[13] = 0xff00 | (w_version >> 8); //version variation
		for (u8 i = 14; i < 25; i++) { wild.cond[i] = 0xffff; } //14 to 24 = 0xffff
		// wild.cond[25] = 0;
		wild.cond[26] = 0xffff;
		// wild.cond[27] = 0;
		wild.cond[28] = 0xffff;
		wild.cond[29] = 0xffff;

		SetCheckum(&wild);
		EncryptBlocks(&wild);
		EncryptCondition(&wild);

		/* Initialize Seven */
		seven.pid = 0x00005544;
		seven.bef = 0x05a4; //after checksum check, changed to a Bad Egg (bit 4: 0->1)
		SetBlocks(&seven); //always ACBD (0x0213)

		/* Simulate the buffer overflow */
		/* Block A */
		seven.data[seven.pos_a][0] = (user.aslr + OppPartyOffBeg[grouped_version]) & 0xffff;
		seven.data[seven.pos_a][1] = (user.aslr + OppPartyOffBeg[grouped_version]) >> 16;
		seven.data[seven.pos_a][2] = (user.aslr + OppPartyOffEnd[grouped_version]) & 0xffff;
		seven.data[seven.pos_a][3] = (user.aslr + OppPartyOffEnd[grouped_version]) >> 16;
		for (u8 i = 0; i < 8; i++) { seven.data[seven.pos_a][i + 4] = ogwild.gfx[i]; }
		seven.data[seven.pos_a][12] = 0x0006;
		seven.data[seven.pos_a][13] = 0x0000;
		seven.data[seven.pos_a][14] = 0x0001;
		seven.data[seven.pos_a][15] = 0x0000;
		/* Block C, B, D and Condition data - array out of bounds method */
		u16* wild_data = (u16*)(&wild.pid);
		u16* seven_data = (u16*)(&seven.data[1]);
		for (u8 i = 0; i < (BLOCKS - 1) * BLOCK_SIZE + COND_SIZE_S + STACK_OFFSET; i++) { seven_data[i] = wild_data[i]; }

		EncryptBlocks(&seven);

		/* If the 1st move of Seven is invalid, the game will crash right before showing the battle menu */
		if (seven.data[seven.pos_b][0] > MOVES_MAX + 2) { continue; }

		/* If the ball doesn't have a valid ID the battle won't load */
		u8 ballid = seven.data[seven.pos_d][13] >> 8;
		if (ballid > BALL_ID_MAX) { continue; }

		SetCheckum(&seven);
		EncryptBlocks(&seven);

		/* If heap ID of Opponent 1 Party is valid, the game will crash when returning to the overworld */
		u8 heapid = seven.data[seven.pos_a][10] & 0xff;
		if (heapid < HEAPID_MAX) { continue; }

		/* If partycount of Opponent 1 Party is invalid, the game will crash right before showing the battle menu */
		u32 partycount = seven.data[seven.pos_a][14] | (seven.data[seven.pos_a][15] << 16);
		if (IsInvalidPartyCount(partycount)) { continue; }

		/* If the Bad Egg flag is set or the Fast Mode flags aren't set, the Pkmn will become a Bad Egg */
		if ((seven.data[seven.pos_c][2] & 7) != 3) { continue; }

		/* Get the new PID of the wild and deduce its new block order */
		wild.pid = seven.data[seven.pos_c][0] | (seven.data[seven.pos_c][1] << 16);
		SetBlocks(&wild);

		/* Get final species, item, ability and steps to hatch - array out of bounds method */
		u16 f_species = seven.data[1 + wild.pos_a][STACK_OFFSET];
		u16 f_item = seven.data[1 + wild.pos_a][STACK_OFFSET + 1];
		u16 f_steps = seven.data[1 + wild.pos_a][STACK_OFFSET + 6] & 0xff;
		u8 f_ability = seven.data[1 + wild.pos_a][STACK_OFFSET + 6] >> 8;

		/* Valid species, user-defined species and user-defined item filters */
		if (f_species >= SPECIES_MAX) { continue; }
		if (user.species != 0 && f_species != user.species) { continue; }
		if (user.item != 0 && f_item != user.item) { continue; }

		/* Get final moveset, IVs, Egg steps, Form ID and Fateful Encounter flag - array out of bounds method */
		u16 moves[OWN_MOVES_MAX];
		for (u8 i = 0; i < OWN_MOVES_MAX; i++) { moves[i] = seven.data[1 + wild.pos_b][STACK_OFFSET + i]; }
		wild.iv1 = seven.data[1 + wild.pos_b][STACK_OFFSET + 8];
		wild.iv2 = seven.data[1 + wild.pos_b][STACK_OFFSET + 9];
		u16 fate = seven.data[3 + wild.pos_b][0];

		/* Move filter */
		if (user.move != 0) {
			/* If none of the 4 current ones match the user's move, continue search */
			if ((moves[0] != user.move) && (moves[1] != user.move) && (moves[2] != user.move) && (moves[3] != user.move)) { continue; }
		}

		/* IVs, steps to hatch, level, form ID */
		GetIVs(&wild);
		f_steps = IsEgg(wild.iv2) * (f_steps + 1) * 255; //steps to hatch if Egg, else zero (0)
		u8 f_level = seven.cond[22] & 0xff;
		u8 form = GetFormId((u8)fate);

		/* Strings for a succesful result */
		u8* str_fateful;
		if (IsFatefulEncounter(fate)) { str_fateful = "Fateful"; }
		else { str_fateful = "-------"; }
		u8* str_shiny;
		if (IsShiny(wild.pid, user.tid, user.sid)) { str_shiny = "Shiny"; }
		else { str_shiny = "-----"; }
		u8 str_f_species[STRING_LENGTH_MAX];
		SetString(str_f_species, f_species, Pokelist, SPECIES_MAX, "DPbox", "0x%04X");
		u8 str_f_item[STRING_LENGTH_MAX];
		SetString(str_f_item, f_item, Items, ITEMS_MAX, "None", "0x%04X");
		u8 str_f_abi[STRING_LENGTH_MAX];
		SetString(str_f_abi, f_ability, Abilities, ABILITIES_MAX, "None", "0x%02X");
		u8 str_moves[OWN_MOVES_MAX][STRING_LENGTH_MAX];
		for (u8 i = 0; i < OWN_MOVES_MAX; i++) { SetString(str_moves[i], moves[i], Moves, MOVES_MAX, "None", "0x%04X"); }

#ifdef DEBUG
		/* Print successful result to console */
		printf("0x%08X | 0x%08X | Lv. %-3d | %-12s | %-4d | %-14s | %-16s | %-5d steps | %s | %s | ", seed, wild.pid, f_level, str_f_species, form, str_f_item, str_f_abi, f_steps, str_fateful, str_shiny);
		printf("%02d/%02d/%02d/%02d/%02d/%02d | ", wild.ivs[hp], wild.ivs[at], wild.ivs[df], wild.ivs[sa], wild.ivs[sd], wild.ivs[sp]);
		printf("%s, %s, %s, %s\n", str_moves[0], str_moves[1], str_moves[2], str_moves[3]);
#endif
		/* Print successful result to file */
		fprintf(fp, "0x%08X | 0x%08X | Lv. %-3d | %-12s | %-4d | %-14s | %-16s | %-5d steps | %s | %s | ", seed, wild.pid, f_level, str_f_species, form, str_f_item, str_f_abi, f_steps, str_fateful, str_shiny);
		fprintf(fp, "%02d/%02d/%02d/%02d/%02d/%02d | ", wild.ivs[hp], wild.ivs[at], wild.ivs[df], wild.ivs[sa], wild.ivs[sd], wild.ivs[sp]);
		fprintf(fp, "%s, %s, %s, %s\n", str_moves[0], str_moves[1], str_moves[2], str_moves[3]);

		results++;
	}

	/* End of search loop */
	clock_t end = clock(); //end timer
	double time_spent = ((double)end - (double)begin) / CLOCKS_PER_SEC; //calculate time elapsed since start of search
	fprintf(fp, "\nFound %u results in %.2f seconds.\n", results, time_spent); //print time to file
	printf("\n%u results compiled to %s in %.2f seconds.\n", results, results_file_name, time_spent); //print time to console
	fclose(fp); //close Results file
	u8 exit; (void)scanf("%s", &exit); //scan to halt execution
	return 0;
}
