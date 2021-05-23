# Motor
This application has been designed to help players of Pokémon Diamond, Pearl and Platinum perform the Battle Corruption Glitch.  
This glitch allows players to catch any desired species in the place of the static Rotom encounter in Old Chateau (Platinum) or the static Giratina encounter in Turnback Cave (Diamond and Pearl).  

***NB: The current version of this application only supports Pokémon Platinum for the English language and Pokémon Diamond & Pearl for the English and Italian languages.***  
***Support for Pokémon Diamond & Pearl in all languages coming soon.***  

Each user will need to provide several things to the application:  

- their Trainer ID and Secret ID (TID and SID)  
- their game's Version (Diamond, Pearl or Platinum)  
- their game's Language (Japanese, English, French, Italian, German, Spanish, Korean)  
- their desired Species ID (optional, from Bulbasaur to Arceus https://bulbapedia.bulbagarden.net/wiki/List_of_Pok%C3%A9mon_by_index_number_(Generation_IV))  
- their desired Item ID (optional, from Master Ball to Secret Key https://bulbapedia.bulbagarden.net/wiki/List_of_items_by_index_number_(Generation_IV))  
- their desired Move ID (optional, from Pound to Shadow Force https://bulbapedia.bulbagarden.net/wiki/List_of_moves)  
- how many Seeds they want to go through (maximum 4,294,967,295)  

Once all of these parameters have been entered, the application will go through each seed and output every successful hit.  
It will provide the Seed on which it found the Pokémon on, alongside several other characteristics (Level, Item, Ability, Moves, ...).  

This glitch is not guaranteed to work on console and is still being troubleshot on emulator (crashes sometimes before showing the battle menu, sometimes at the end of the battle).  
You can however perform it on Desmume 0.9.11 by editing Motor.lua and modifying your Seed, your TID and your SID.  
You'll be required to have a save file with a full party of 6 Pokémon, all knocked out, for the glitch to work.  
The most common way to get to this state is with the Pomeg Glitch.  

If you want to build the application yourself on Windows 10 with GCC, use these commands in Windows Shell:  
``windres Motor.rc -O coff -o Motor.res`` (to create the .res file and include the .ico in the final build)  
``gcc -O3 Motor.c -o Motor Motor.res`` (optimized mode flag -O3 helps search through seeds 3 times faster than without)  

If you have any questions or issues regarding the application, direct them to me via Twitter @shinyhunter_map

