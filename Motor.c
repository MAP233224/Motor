/******************************/
/*       Motor.c by MAP       */
/******************************/

#include "common.h"

void ScanValue(u8* message, u32* value, u8* format, u64 max) {
	/* General purpose safe scan. Instruction message, value to change, string format and max value */
	do {
		printf("%s", message);
		u8 userInput[32];
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
			* value = max + 1; //doesn't work for seed, puts it at 0 and becomes valid
			continue;
		}
	} while (*value > max);
}

void CreateProfile(User* user) {
	/* Scan for user info and ask if you want to save this new profile */
	u32 save;
	ScanValue("Enter your Version (0=Diamond, 1=Pearl, 2=Platinum): ", &user->version, "%u", 2);
	ScanValue("Enter your Language (1=jp, 2=en, 3=fr, 4=it, 5=ge, 7=sp, 8=ko): ", &user->language, "%u", 8);
	ScanValue("Enter your TID (0 to 65535): ", &user->tid, "%u", 0xffff);
	ScanValue("Enter your SID (0 to 65535): ", &user->sid, "%u", 0xffff);
	ScanValue("Save those user settings? (0=no, 1=yes) ", &save, "%u", 1);
	if (save) {
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

u16 IvToStat(Pkmn* pkmn, Original* wild, u8 stat) {
	/* Return the value of a stat based on the IV, Base Stat, Nature and Level. */
	if (stat == 0) { return (2 * (wild->bstats[stat]) + pkmn->ivs[stat]) * wild->level / 100 + wild->level + 10; } //hp
	return StatNatureModifier(pkmn->nature, stat - 1, (2 * (wild->bstats[stat]) + pkmn->ivs[stat]) * wild->level / 100 + 5); //ignore for hp (index 0), hence the stat-1
}

void SetCheckum(Pkmn* pkmn) {
	/* Set the checksum of a Pkmn by summing all of its block data. */
	for (u8 i = 0; i < BLOCK_SIZE; i++) {
		pkmn->checksum += pkmn->data[pkmn->pos_a][i] + pkmn->data[pkmn->pos_b][i] + pkmn->data[pkmn->pos_c][i] + pkmn->data[pkmn->pos_d][i];
	}
}

bool IsBadEgg(u16 badegg) {
	/* Check if the bad egg flag is set by looking at bit 2 of the "bad egg" 16-bit word. */
	return (badegg & 4) == 4;
}

bool IsEgg(u16 egg) {
	/* Check if the egg flag is set by looking at bit 30 of the "iv2" 16-bit word. */
	return (egg & 0x4000) == 0x4000;
}

bool IsFatefulEncounter(u16 fate) {
	/* Check if the fateful encounter bit is set. */
	return fate & 1;
}

bool SkippedCheckum(u16 badegg) {
	/* Check if the checksum was skipped by looking at bit 0 and 1 of the "bad egg" 16-bit word. */
	return (badegg & 3) == 3;
}

bool IsShiny(u32 pid, u16 tid, u16 sid) {
	/* Check if a pkmn is shiny by xoring its pid (top and bottom 16 bits), tid and sid */
	return ((pid & 0xffff) ^ (pid >> 16) ^ tid ^ sid) < 8;
}

bool IsInvalidPartyCount(u32 count) {
	/* Check if the number of members in the opponent's party is invalid. Determines crash at battle menu. */
	return ((count > 0x00000036) && (count < 0x80000000));
}

u8* GetString(u16 val, u8 array[][STRING_LENGTH_MAX], u16 max, u8* zero, u8* oob) {
	/* Get string in array corresponding to val, bounds and zero check */
	if (val >= max) return oob;
	if (val == 0) return zero;
	return array[val];
}

u32 RngNext(u32* state) {
	/* General purpose LCRNG, advance and return state */
	*state = *state * 0x41C64E6D + 0x6073;
	return *state;
}

void Encrypt(Pkmn* pkmn) {
	/* Encrypt with XOR and LCRNG each 16-bit word of Pkmn data. */
	/* First with the Checksum as the Seed/Key (for block data), then with the PID (for condition data). */
	u32 pkmn_data_state = pkmn->checksum;
	u32 pkmn_cond_state = pkmn->pid;
	for (u8 i = 0; i < BLOCKS; i++) {
		for (u8 j = 0; j < BLOCK_SIZE; j++) {
			pkmn->data[i][j] ^= (RngNext(&pkmn_data_state) >> 16);
		}
	}
	for (u8 i = 0; i < COND_SIZE; i++) {
		pkmn->cond[i] ^= (RngNext(&pkmn_cond_state) >> 16);
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
	/* Calculate PID, Nature and IVs according to Method J Stationary (no Synchronize) from a given seed */
	pkmn->nature = (RngNext(&state) >> 16) / 0x0A3E;
	do { pkmn->pid = (RngNext(&state) >> 16) | (RngNext(&state) & 0xffff0000); } while (pkmn->pid % NATURES_MAX != pkmn->nature); //roll PID until the 2 natures are the same
	pkmn->iv1 = (RngNext(&state) >> 16) & 0x7FFF;
	pkmn->iv2 = (RngNext(&state) >> 16) & 0x7FFF;
	pkmn->iv1 |= (pkmn->iv2 & 1) << 15;
	pkmn->iv2 >>= 1;
}

void DebugPkmnData(Pkmn* pkmn) {
	/* Prints out the raw data of the chosen pkmn */
	#ifdef DEBUG
	printf("%04X\n", pkmn->checksum);
	for (int i = 0; i < BLOCKS; i++) {
		for (int j = 0; j < BLOCK_SIZE; j++) {
			printf("%04X ", pkmn->data[i][j]);
			if (j % 8 == 7) { printf("\n"); }
		}
	}
	for (int i = 0; i < COND_SIZE; i++) {
		printf("%04X ", pkmn->cond[i]);
		if (i % 8 == 7) { printf("\n"); }
	}
	printf("\n");
	#endif
}

int main() {

	User user = { 0 }; //0 init

	u32 use_saved;
	ScanValue("Use saved profile (0=no, 1=yes): ", &use_saved, "%u", 1);

	if (use_saved) { //use saved user profile from Profile.txt
		FILE* profile;
		profile = fopen("Profile.txt", "r"); //read only
		if (profile == NULL) {
			printf("Profile.txt not found in the program's directory. Please create a new profile:\n\n");
			CreateProfile(&user);
		}
		else { //read the existing profile info
			fscanf(profile, "%u", &user.version);
			fscanf(profile, "%u", &user.language);
			fscanf(profile, "%u", &user.tid);
			fscanf(profile, "%u", &user.sid);
			fclose(profile);
		}
	}
	else { CreateProfile(&user); }

	u8 grouped_version = user.version >> 1; //fuse Diamond and Pearl together

	u32 og;
	do {
		if (user.version == 2) { ScanValue("Static PKMN you want to corrupt (0=Giratina-O, 1=Giratina-A, 2=Dialga, 3=Palkia, 4=Uxie, 5=Azelf, 6=Rotom): ", &og, "%u", OG_WILDS_MAX - 1); } //platinum
		else { ScanValue("Static PKMN you want to corrupt (0=Giratina, 1=Arceus, 2=Dialga, 3=Palkia, 4=Shaymin, 5=Darkrai, 6=Uxie, 7=Azelf, 8=Rotom): ", &og, "%u", OG_WILDS_MAX - 1); } //dp
	} while (OGW_LangVers[user.language][grouped_version][og]==NULL);

	do {
		ScanValue("ASLR to use (0~11 for jp, 0~4 for ko, 0~3 otherwise): ", &user.aslr, "%u", ASLR_GROUPS_MAX - 1);
	} while (Aslrs[user.language][grouped_version][user.aslr]==0);

	ScanValue("Search for a species (0=no, species_id=yes): ", &user.species, "%u", SPECIES_MAX - 1);
	ScanValue("Search for an item (0=no, item_id=yes): ", &user.item, "%u", ITEMS_MAX - 1);
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
	else if (user.move > MOVES_MAX) {
		u8 buffer[8];
		sprintf(buffer, "0x%04X", user.move);
		strmove = buffer;
	}
	else { strmove = Moves[user.move]; }

	u16 w_version = (user.version + 10) << 8; //convert for use in pkmn data
	u16 w_language = user.language << 8; //convert for use in pkmn data
	Original ogwild = *OGW_LangVers[user.language][grouped_version][og];
	user.aslr = Aslrs[user.language][grouped_version][user.aslr]; //depends on language, version and user choice

	FILE* fp; //declare file object
	u8 filename[4 * STRING_LENGTH_MAX] = "Results_"; //Results file name, then append with profile info
#ifdef DEBUG
	strcat(filename, "DEBUG_");
#endif
	strcat(filename, OgWilds[grouped_version][og % OG_WILDS_MAX]); strcat(filename, "_");
	strcat(filename, strvers); strcat(filename, "_");
	strcat(filename, strlang); strcat(filename, "_");
	u8 strtidsid[STRING_LENGTH_MAX];
	sprintf(strtidsid, "%05u_%05u", user.tid, user.sid);
	strcat(filename, strtidsid);
	strcat(filename, ".txt");
	fp = fopen(filename, "w+"); //open/create file

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
	printf("> Searching through %u frames for %s holding %s knowing %s...\n", user.frames, strspec, stritem, strmove);

	u32 pid_list[PIDS_MAX] = { 0 }; //0 init
	u32 results = 0; //0 init
	u32 seed = user.seed; //copy to advance in the main loop
	if (user.language == 8) { user.aslr += KOREAN_OFFSET; } //RAM thing
	u8 alternate_form = 0;
	if (user.version == 2 && og == 0) { alternate_form = 8; } //Giratina Origin

	clock_t begin = clock(); //timer starts

	/* DEBUG: print unique aslr groups */
	// for (u32 aslr = 0x0227d4e0; aslr<=0x0227d5e0; aslr+=4){
	// user.aslr=aslr;
	// if (frame==0) {printf("%u\n", wild.pid&0xff); continue;}
	// else {continue;}
	// }

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

		for (u8 i = 0; i < OWN_MOVES_MAX; i++) { wild.data[wild.pos_b][i] = ogwild.moves[i]; }//4 moves
		wild.data[wild.pos_b][4] = ogwild.pp1and2; //pp1and2
		wild.data[wild.pos_b][5] = ogwild.pp3and4; //pp3and4
		wild.data[wild.pos_b][8] = wild.iv1;
		wild.data[wild.pos_b][9] = wild.iv2;
		wild.data[wild.pos_b][12] = 0x0004 + alternate_form; //genderless

		for (u8 i = 0; i < 11; i++) { wild.data[wild.pos_c][i] = ogwild.name[i]; } //11 characters for the name
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
		for (u8 i = 14; i < 25; i++) { wild.cond[i] = 0xffff; } //14 to 24 = 0xffff
		// wild.cond[25] = 0;
		wild.cond[26] = 0xffff;
		// wild.cond[27] = 0;
		wild.cond[28] = 0xffff;
		wild.cond[29] = 0xffff;

		SetCheckum(&wild);
		// DebugPkmnData(&wild);
		Encrypt(&wild);
		// DebugPkmnData(&wild);

		/* Initialize Seven */
		seven.pid = 0x00005544;
		seven.bef = 0x05a4; //after checksum check, changed to a Bad Egg (bit 4: 0->1)
		SetBlocks(&seven); //always ACBD (0x0213)

		/* Simulating the stack overflow */
		for (u8 i = 0; i < BLOCK_SIZE - STACK_OFFSET; i++) { //ABCD blocks part 1
			for (u8 j = 1; j < BLOCKS; j++) { //only need to start from j=1 bc block A is taken care of later.
				seven.data[j][i + STACK_OFFSET] = wild.data[j - 1][i]; //warning: negative index
			}
		}
		for (u8 i = 0; i < BLOCKS; i++) { //ABCD blocks part 2
			for (u8 j = 0; j < BLOCKS - 2; j++) {
				seven.data[j + 2][i] = wild.data[j][BLOCK_SIZE - STACK_OFFSET + i];
			}
		}
		for (u8 i = 0; i < COND_SIZE; i++) { //condition data
			if (i < STACK_OFFSET) { seven.cond[i] = wild.data[2][BLOCK_SIZE - STACK_OFFSET + i]; }
			else if (i < STACK_OFFSET + BLOCK_SIZE) { seven.cond[i] = wild.data[3][i - STACK_OFFSET]; }
			else { seven.cond[i] = wild.cond[i - STACK_OFFSET - BLOCK_SIZE]; }
		}

		seven.data[seven.pos_a][0] = (user.aslr + LocBegOppParty[grouped_version]) & 0xffff;
		seven.data[seven.pos_a][1] = (user.aslr + LocBegOppParty[grouped_version]) >> 16;
		seven.data[seven.pos_a][2] = (user.aslr + LocEndOppParty[grouped_version]) & 0xffff;
		seven.data[seven.pos_a][3] = (user.aslr + LocEndOppParty[grouped_version]) >> 16;
		for (u8 i = 0; i < 8; i++) { seven.data[seven.pos_a][i + 4] = ogwild.sv[i]; } //special values of the ogwild
		seven.data[seven.pos_a][12] = 0x0006;
		seven.data[seven.pos_a][13] = 0x0000;
		seven.data[seven.pos_a][14] = 0x0001;
		seven.data[seven.pos_a][15] = 0x0000;

		seven.data[seven.pos_c][0] = wild.pid % 65536;
		seven.data[seven.pos_c][1] = wild.pid >> 16;
		seven.data[seven.pos_c][2] = wild.bef;
		seven.data[seven.pos_c][3] = wild.checksum;

		Encrypt(&seven);
		// DebugPkmnData(&seven);

		if (seven.data[seven.pos_b][0] > MOVES_MAX + 2) { continue; } //menu crash if 1st move of Seven is invalid

		/* If the ball doesn't have a valid id the battle won't load */
		u8 ballid = seven.data[seven.pos_d][13] >> 8;
		if ((ballid > 16) || (ballid == 0)) { continue; } //might be more complex, some invalid Ball IDs load fine (on console? Need testing)

		SetCheckum(&seven);
		Encrypt(&seven);
		// DebugPkmnData(&seven);

		if ((seven.data[seven.pos_a][10]&0xff) < HEAPID_MAX) { continue; } //return to overworld crash

		u32 partycount = seven.data[seven.pos_a][14] | (seven.data[seven.pos_a][15] << 16);
		if (IsInvalidPartyCount(partycount)) { continue; } //battle menu crash

		/* If it's a Bad Egg or Checksum was not skipped, continue */
		wild.bef = seven.data[seven.pos_c][2];
		if (IsBadEgg(wild.bef)) { continue; }
		if (!SkippedCheckum(wild.bef)) { continue; }

		wild.pid = seven.data[seven.pos_c][0] | (seven.data[seven.pos_c][1] << 16); //don't actually need the top part I think
		SetBlocks(&wild);

		/* Get final species, item, ability and steps to hatch */
		u8 f_ability;
		u16 f_species, f_item, f_steps;
		switch (wild.pos_a) {
			case 0: //C
			f_species = seven.data[seven.pos_c][STACK_OFFSET];
			f_item = seven.data[seven.pos_c][STACK_OFFSET + 1];
			f_ability = seven.data[seven.pos_c][STACK_OFFSET + 6] >> 8;
			f_steps = seven.data[seven.pos_c][STACK_OFFSET + 6] & 0xff;
			break;
			case 1: //B
			f_species = seven.data[seven.pos_b][STACK_OFFSET];
			f_item = seven.data[seven.pos_b][STACK_OFFSET + 1];
			f_ability = seven.data[seven.pos_b][STACK_OFFSET + 6] >> 8;
			f_steps = seven.data[seven.pos_b][STACK_OFFSET + 6] & 0xff;
			break;
			case 2: //D
			f_species = seven.data[seven.pos_d][STACK_OFFSET];
			f_item = seven.data[seven.pos_d][STACK_OFFSET + 1];
			f_ability = seven.data[seven.pos_d][STACK_OFFSET + 6] >> 8;
			f_steps = seven.data[seven.pos_d][STACK_OFFSET + 6] & 0xff;
			break;
			case 3: //cond
			f_species = seven.cond[STACK_OFFSET];
			f_item = seven.cond[STACK_OFFSET + 1];
			f_ability = seven.cond[STACK_OFFSET + 6] >> 8;
			f_steps = seven.cond[STACK_OFFSET + 6] & 0xff;
			break;
		}

		/* Species filter */
		if (f_species >= SPECIES_MAX) { continue; } //if species isn't valid
		if (user.species != 0 && f_species != user.species) { continue; } //if user specified a species but it isn't the current one
		/* Item filter */
		if (user.item != 0 && f_item != user.item) { continue; } //if user specified an item but it isn't the current one
		//possibily that we're getting fucked here with SOME glitch items

		/* Get final moveset, egg steps, form id and fateful encounter flag */
		u16 fate;
		u16 moves[OWN_MOVES_MAX];
		switch (wild.pos_b) {
			case 0: //C
			for (u8 i = 0; i < OWN_MOVES_MAX; i++) { moves[i] = seven.data[seven.pos_c][STACK_OFFSET + i]; }
			wild.iv1 = seven.data[seven.pos_c][STACK_OFFSET + 8];
			wild.iv2 = seven.data[seven.pos_c][STACK_OFFSET + 9];
			fate = seven.data[seven.pos_d][0];
			break;
			case 1: //B
			for (u8 i = 0; i < OWN_MOVES_MAX; i++) { moves[i] = seven.data[seven.pos_b][STACK_OFFSET + i]; }
			wild.iv1 = seven.data[seven.pos_b][STACK_OFFSET + 8];
			wild.iv2 = seven.data[seven.pos_b][STACK_OFFSET + 9];
			fate = seven.cond[0];
			break;
			case 2: //D
			for (u8 i = 0; i < OWN_MOVES_MAX; i++) { moves[i] = seven.data[seven.pos_d][STACK_OFFSET + i]; }
			wild.iv1 = seven.data[seven.pos_d][STACK_OFFSET + 8];
			wild.iv2 = seven.data[seven.pos_d][STACK_OFFSET + 9];
			fate = seven.cond[BLOCK_SIZE];
			break;
			case 3: //cond
			for (u8 i = 0; i < OWN_MOVES_MAX; i++) { moves[i] = seven.cond[STACK_OFFSET + BLOCK_SIZE + i]; }
			wild.iv1 = seven.cond[STACK_OFFSET + 8];
			wild.iv2 = seven.cond[STACK_OFFSET + 9];
			fate = seven.cond[2*BLOCK_SIZE];
			break;
		}

		/* Filter for a specific move */
		if (user.move != 0) {
			if ((moves[0] != user.move) && (moves[1] != user.move) && (moves[2] != user.move) && (moves[3] != user.move)) { continue; } //if user specified a move but none of the 4 current ones match
		}

		GetIVs(&wild);

		f_steps = IsEgg(wild.iv2) * (f_steps + 1) * 255; // steps to hatch if Egg, zero if not an Egg
		u8 f_level = seven.cond[22] & 0xff; //final level
		u8 form = GetFormId((u8)fate); //final form id

		/* Fateful encounter string definition */
		u8* fateful;
		if (IsFatefulEncounter(fate)) { fateful = "Fateful"; }
		else { fateful = "-------"; }

		/* Species, item and ability string format */
		u8* str_f_species = GetString(f_species, Pokelist, SPECIES_MAX, "DPbox", "Glitchmon");
		u8* str_f_item = GetString(f_item, Items, ITEMS_MAX, "None", "Glitch Item");
		u8* str_f_abi = GetString(f_ability, Abilities, ABILITIES_MAX, "None", "Glitch Ability");

		/* Moves string format */
		u8 strmoves[OWN_MOVES_MAX][STRING_LENGTH_MAX] = { 0 };
		for (u8 i = 0; i < OWN_MOVES_MAX; i++) {
			if (moves[i] >= MOVES_MAX) {
				u8 buffer[STRING_LENGTH_MAX];
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

#ifdef DEBUG
		/* Print successful result to console */
		printf("0x%08X | 0x%08X | Lv. %-3d | %-12s | %-4d | %-14s | %-16s | %-5d steps | %s | %s | ", seed, wild.pid, f_level, str_f_species, form, str_f_item, str_f_abi, f_steps, fateful, shiny);
		printf("%02d/%02d/%02d/%02d/%02d/%02d | ", wild.ivs[hp], wild.ivs[at], wild.ivs[df], wild.ivs[sa], wild.ivs[sd], wild.ivs[sp]);
		printf("%s, %s, %s, %s\n", strmoves[0], strmoves[1], strmoves[2], strmoves[3]);
#endif
		/* Print successful result to file */
		fprintf(fp, "0x%08X | 0x%08X | Lv. %-3d | %-12s | %-4d | %-14s | %-16s | %-5d steps | %s | %s | ", seed, wild.pid, f_level, str_f_species, form, str_f_item, str_f_abi, f_steps, fateful, shiny);
		fprintf(fp, "%02d/%02d/%02d/%02d/%02d/%02d | ", wild.ivs[hp], wild.ivs[at], wild.ivs[df], wild.ivs[sa], wild.ivs[sd], wild.ivs[sp]);
		fprintf(fp, "%s, %s, %s, %s\n", strmoves[0], strmoves[1], strmoves[2], strmoves[3]);

		results++;
	}

	/* End of main loop */

	clock_t end = clock(); //end timer
	double time_spent = ((double)end - (double)begin) / CLOCKS_PER_SEC; //calculate time elapsed since start of search
	fprintf(fp, "\nFound %u results in %.2f seconds.\n", results, time_spent);
	printf("\n%u results compiled to %s in %.2f seconds.\n", results, filename, time_spent);
	fclose(fp); //close file
	u8 exit;
	scanf("%s", &exit); //scan to halt execution
	return 0;
}
