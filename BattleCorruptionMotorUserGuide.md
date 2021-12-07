# Battle Corruption & Motor User Guide

Glitch available in Pokémon Diamond, Pearl and Platinum.

### [Glitch summary]

Launching a battle with a full party of fainted Pokémon (or eggs) against some static encounters allows you to catch any other Pokémon species. This corrupt Pokémon can potentially hold any item, have any ability, know any move, and much more.

Performing this glitch successfully cannot be done through sheer luck: this is why I created Motor, an application designed to help you find the Pokémon you're looking for.

### [In-game requirements]

In general:

*   A PKMN with Fly/Teleport
*   A Master Ball to catch the corrupt wild (the catch rate is botched because of the HP max sometimes being smaller than the HP current, so it is possible to get a 100% catch rate by just throwing a regular Poké Ball but I don't recommend it, and you only get 1 turn to catch the corrupt wild)

For the RNG manipulation:

*   The Coin Flip Poketch App to check your Seed
*   A Chatot with a learned saying via Chatter to advance the RNG
*   A Pokémon WITHOUT Synchronize or Cute Charm in the first slot of your Party

Pomeg Glitch (tutorial [here](https://youtu.be/ipg3J3COXHQ)):

*   Perform the Pomeg Glitch with a full Party, meaning the last PKMN alive is the one on which the glitch is performed
*   Reach your target (Giratina, Arceus, Rotom, etc.) and save in front of it

Do not use a Repel once you've done the Pomeg Glitch; you will crash every time you take a step in an area with encounters. Avoid areas with encounters in general, you will also crash if you trigger an encounter.

### [Using Motor]

You can download the latest release [here.](https://github.com/MAP233224/Motor/releases/tag/v1.3.5)

I am working on creating a cross-platform GUI version of this application, but for now it is only a Windows console application, compiled with GCC. It is written entirely in C, so you can compile it for Mac OS or Linux if you are capable.

Upon launching the app, you'll be prompted with a set of questions.

<div class="console">Use saved profile (0=no, 1=yes):</div>

Type _0_ or _1_ and press ENTER. Choosing 0 (no) will launch the Profile creation prompts. Choosing 1 (yes) will retrieve the User's Profile information in "Profile.txt" if it exists in the same directory as "Motor.exe". If you chose "no" or if "Profile.txt" was not found in the same directory as Motor.exe when chosing "yes", the profile creation prompts will show up.

<div class="console">Enter your Version (0=Diamond, 1=Pearl, 2=Platinum):</div>

Which game you're using.

<div class="console">Enter your Language (1=jp, 2=en, 3=fr, 4=it, 5=ge, 7=sp, 8=ko):</div>

The language you're playing in: Japanese, English, French, Italian, German, Spanish or Korean.

<div class="console">Enter your TID (0 to 65535):</div>

Found on your Trainer Card in game.

<div class="console">Enter your SID (0 to 65535):</div>

Found nowhere in game; see the available methods for finding it [here.](https://google.com/)

<div class="console">Save those user settings? (0=no, 1=yes)</div>

A file "Profile.txt" is created in the same directory as "Motor.exe". You can easily edit it or create it yourself.

<div class="console">Static PKMN you want to corrupt (*):</div>

The static Pokémon you will corrupt in game. Depends on whether you chose Diamond, Pearl or Platinum. As a side note, Arceus is infinitely re-usable thanks to the numerous infinite Void routes that exist for Diamond and Pearl – I suggest using [this RNG friendly route.](https://youtu.be/L7htZgroScI)

<div class="console">ASLR to use (0~11 for jp, 0~4 for ko, 0~3 otherwise):</div>

Different ASLRs yield different results. Some ASLRs have valid "mirrors", aka they have the same in-game result even if you don't hit the exact ASLR you're looking for. The more mirrors an ASLR has, the more likely you are to get your desired result.

In Diamond and Pearl:

*   ASLR 0, 1, 2, 3 in Japanese all have 4 valid mirrors.
*   ASLR 4, 5, 6, 7 in Japanese all have 8 valid mirrors.
*   ASLR 8, 9, 10, 11 in Japanese all have 4 valid mirrors.
*   ASLR 0 in English, French, Italian, German and Spanish has 1 valid mirror.
*   Korean has no mirror ASLR.

In Platinum:

*   ASLR 0 in English has 1 valid mirror.

<div class="console">Search for a species (0=no, species_id=yes):</div>

In decimal, the species ID you want to filter for. For example, Mew is _151_. A list of Pokémon by species ID can be found [here.](http://bulbapedia.bulbagarden.net/wiki/List_of_Pok%C3%A9mon_by_index_number_(Generation_IV))

<div class="console">Search for an item (0=no, item_id=yes):</div>

Filter for a specific item to be held by the new wild PKMN. A list of items by index number can be found [here.](https://bulbapedia.bulbagarden.net/wiki/List_of_items_by_index_number_(Generation_IV))

<div class="console">Search for a move (0=no, move_id=yes):</div>

Filter for a specific move known by the new wild PKMN. A list of moves by index number can be found [here.](https://bulbapedia.bulbagarden.net/wiki/List_of_moves)

<div class="console">Enter your Seed (32 bit, hex): 0x</div>

In hexadecimal, the seed of the RNG state. Motor needs a starting point, but it really doesn't matter what it is – I usually pick _0_ when doing full searches.

<div class="console">How many frames to search through (32 bit, dec):</div>

In decimal, the number of frames to search through. Pro tip: enter _-1_ to do a full search, as it is more convenient to type than _4294967295_. You get 1 result every 10 million frames on average.

<div class="console">Allow more seed options per result? (0=no, 1=yes):</div>

Allow for duplicate PIDs, which will give you more possible seeds to hit on console for the same PID (aka the same corrupt wild). For convenience and hopefully not having to do too many frame advances in game when RNG manipulating.

Then the search begins, with a recap of all the parameters you entered. A "Results.txt" file is created in the same directory that "Motor.exe" is in, with a name that matches the original fixed PKMN you're corrupting and your profile info (version_language_tid_sid). Once the search is done, pick a seed that you like and run it through [ReverseSeed](https://github.com/MAP233224/ReverseSeed/releases/tag/v1.0) to get the actual seed you'll need to hit on console via RNG manipulation and the amount of frame advances you'll need to do.

<div class="nb">

*   Platinum is only possible on the Japanese and English version because the Pomeg Glitch was patched in the other international releases.
*   Japanese Platinum has a unique crash (when the nickname prompt comes up at the end of the battle) that makes this glitch impossible to fully complete.
*   The number of unique results you'll get on a full search without duplicate PIDs is about 400\. The more filter you have, the less results you'll get, don't be surprised if you get 0 valid results when filtering by items or moves especially.
*   The completion time depends on your CPU, to complete a full search it takes about 30 minutes on a i5-7300HQ, 20 minutes on a i5-11600K as of v1.3.5.

</div>

### [Performing the Battle Corruption Glitch]

*   Pick a date/time setup (with the help of [PokéFinder](https://github.com/)) for the seed given by ReverseSeed.
*   Important: don't fiddle with the microphone, the touch screen or the buttons when launching the game (white loading screen) while doing the RNG manip, it'll make the ASLR inconsistent.
*   If you hit the correct seed (and do the subsequent frame advances) but don't get your result in battle, it means you got the wrong ASLR, so you need to use a different date/time setup.
*   If you get the correct Pokémon to show up, go straight into BAG -> POKé BALLS -> Master Ball.
*   Once caught, give it a nickname to set a terminator character in its name and make it valid.

The most common failure points are: infinite black loading screen, crash before battle menu, crash when resuming to the overworld, any status update on turn 0 that transforms the corrupt wild into a Bad Egg and crashes on dex entry.

### [Special Emulator mention]

Along with Motor's executable, you can download "Motor.lua" which contains several useful functions allowing you to perform the Battle Corruption Glitch a lot more reliably in the Desmume emulator.

### [Further reading: technical breakdown of the glitch]

If you want to know more :)

Data of the wild influenced by: ASLR, TID, SID, Version, Language, RNG, Original Wild.

"What is ASLR?" ASLR stands for Address Space Layout Randomization. In our case, it means that RAM is shifted into one of 65 different positions by 4 bytes (because the DS operates on a 32-bit CPU) every time the game is initialized. It is pseudo-randomly determined by your MAC address, your ABXY buttons, microphone and touch screen inputs in the white loading screen before the copyrights for Pokémon, Nintendo, Creatures Inc. and GAME FREAK inc. show up. It plays a role in the corrupt wild's data simply because the 7th Pokémon that gets created overwrites the header of the Opponent 1 Party structure, which includes a pointer to the previous Party block (the Player's Party) and a pointer to the next Party structure (the Ally's Party). Pointers are 32-bit values that hold memory addresses, and in this section of memory, addresses depend on ASLR.

A series of fortunate events:

*   The original wild Pokémon is generated with standard PRNG Method J (for static Pokémon)
*   A 7th Pokémon is "created" in the Player's Party, it inherits part of the original wild's data because of the stack overflow caused by its size and the immediate proximity of the Party structures
*   The Checksum of the Player's 7th Pokémon doesn't match its data, so its Bad Egg flag is turned on (this is why you always send out a Bad Egg when performing the glitch)
*   Its data is encrypted according to its current Checksum (0x0000)
*   Its data is re-encrypted according to its new calculated Checksum (sum of all the 16-bit words in its ABCD block data) and its PID (always equal to 0x00005544 because it's the header identifier of all data structures in the game) for Condition data

Dead full party, Seventh pkmn, Bad Egg flag, encrypt, stack overflow, corrupt wild, encrypt, checksum check, encrypt.

Hurdles (failure points): Bad Egg, Checksum check skipped, glitchmons, glitchballs, first move of Seven, ...

Effects: permanently decrypted, ...

Read the code in Motor.c if you want to know more!

The original overflow ("creating" a 7th pkmn):

Simplified version of the function "FightSystemBoot" in "fight.c" from the Diamond and Pearl source code.

<div class="console c"><span class="c-keyword">for</span> ( <span class="c-type">int</span> i <span class="c-operator">=</span> <span class="c-value">0</span>; i <span class="c-operator"><</span> partySize; i<span class="c-operator">++</span> ) {  
<span class="c-keyword">if</span> ( <span class="c-function">PkmnGet</span>(species) <span class="c-operator">&&</span> (<span class="c-function">PkmnGet</span>(egg_flag) <span class="c-operator">==</span> <span class="c-value">0</span>) <span class="c-operator">&&</span><span class="c-function">PkmnGet</span>(hp) ) {  
    <span class="c-flow">break</span>;  
  }  
}  
<span class="c-function">ServerDataPut</span>(sent_out_pkmn, i);  

<span class="c-comment">// partySize is 6 in our case. If species id is non-zero AND is not an egg AND hp is non-zero. Since all of our pkmn are dead/eggs, we never break out of this loop and exit with i equal to 6, which corresponds to a 7th index position//stores the index of the pkmn being sent out.</span></div>
