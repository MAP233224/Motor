# Motor
This application has been designed to help players of Pokémon Diamond, Pearl and Platinum perform the Battle Corruption Glitch.
This glitch allows players to catch any desired species in the place of the static Rotom encounter in Old Chateau (Platinum) or the static Giratina encounter in Turnback Cave (Diamond and Pearl).

***NB: The current version of this application only supports Pokémon Platinum for the English language.***

To build on Windows 10 with GCC, use the command ``gcc -O3 Motor.c -o Motor`` in Windows Shell.

Each user will need to provide several things to the application:

- their Trainer ID and Secret ID (TID and SID)
- their game's version (Diamond, Pearl or Platinum)
- their game's language (Japanese, English, French, Italian, German, Spanish, Korean)
- their target species (optional, from Bulbasaur to Arceus)
- their target item (optional, from Master Ball to Secret Key)
- how many seeds they want to go through (maximum 4,294,967,295)

Species IDs : https://bulbapedia.bulbagarden.net/wiki/List_of_Pok%C3%A9mon_by_index_number_(Generation_IV)
Item IDs : https://bulbapedia.bulbagarden.net/wiki/List_of_items_by_index_number_(Generation_IV)

Once all of these parameters have been entered, the application will go through each seed and output every successful hit.
It will provide the Seed on which it found the Pokémon on, alongside several other characteristics (Level, Item, Ability, Moves, ...).

This glitch is not guaranteed to work on console and is still being troubleshot on emulator (crashes sometimes before showing the battle menu, sometimes at the end of the battle).
You can however perform it on Desmume 0.9.11 by downloading Motor.lua and modifying your Seed, your TID and your SID.
You'll be required to have a save with a full party of 6 Pokémon, all knocked out (from the Pomeg Glitch usually).

