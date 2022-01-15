# Motor
This application has been designed to help players of Pokémon Diamond, Pearl and Platinum perform the Battle Corruption Glitch.  
This glitch allows players to catch any desired species in the place of some static encounters like Rotom in the Old Chateau or Giratina in Turnback Cave. 

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
Performance wise, a full search of the 4,294,967,295 possible RNG states takes about 15 minutes to complete on my i5-11600K CPU.  

You can also perform Battle Corruption easily on Desmume 0.9.11 by editing Motor.lua and modifying your Seed, your TID and your SID.  
You'll be required to have a save file with a full party of 6 Pokémon, all knocked out, for the glitch to work.  
The most common way to get to this state is with the Pomeg Glitch.  

If you want to build the application yourself on Windows with GCC, use these commands in Windows Shell:  
``windres Motor.rc -O coff -o Motor.res`` (to create the .res file and include the .ico in the final build)  
``gcc -O3 Motor.c -o Motor Motor.res`` (optimized mode flag -O3 helps search through seeds 4 times faster than without)  

If you have any questions or issues regarding the application, direct them to me via Twitter @shinyhunter_map  
If you want to thank me with a little extra, you can donate at https://ko-fi.com/map233224 <3
