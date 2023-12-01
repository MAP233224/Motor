# Motor (Windows GUI)
This application has been designed to help players of Pokémon Diamond, Pearl and Platinum perform the Battle Corruption Glitch.  
This glitch allows players to catch any desired species in the place of some static encounters like Rotom in the Old Chateau or Giratina in Turnback Cave. 

Each user will need to provide several things to the application:  

- their Trainer ID and Secret ID (TID and SID)
- their game's Version (Diamond, Pearl or Platinum)
- their game's Language (Japanese, English, French, Italian, German, Spanish, Korean)
- their corruption target (depends on Version)
- how many Frames they want to go through (maximum 4,294,967,295)
- a starting Seed (32-bit hexadecimal number)
- their desired ASLR (between 0 and 11, some languages are more restrictive)
- their desired Species ID (optional, from Bulbasaur to Arceus https://bulbapedia.bulbagarden.net/wiki/List_of_Pok%C3%A9mon_by_index_number_(Generation_IV))
- their desired Item ID (optional, from Master Ball to Secret Key https://bulbapedia.bulbagarden.net/wiki/List_of_items_by_index_number_(Generation_IV))
- their desired Move ID (optional, from Pound to Shadow Force https://bulbapedia.bulbagarden.net/wiki/List_of_moves)
- their desired Nature (optional)
- their desired minimum IVs (optional)

You can save/load these parameters to/from 1 of 8 possible profile slots.  
Upon launching the search, the application will go through each seed in the specified frame range and output every successful hit.  
It will provide the Seed on which it found the Pokémon on, alongside several other characteristics (Level, Item, Ability, Moves, ...).  
Performance wise, a full search of the 4,294,967,295 possible RNG states takes around 53 seconds to complete on an i5-11600K CPU running at 4100 MHz.  
  
You can then view each result in detail by clicking on them in the results list.  
The details give you the console-hittable seed and the number of frames advances needed for that result (previously done separately by the ReverseSeed application),  
which you can copy to your clipboard via the designated button and paste into your favorite RNG application like Pokéfinder or RNGReporter.  
You can load an existing ``.mtr`` results file from the application by clicking the "RESULTS" button.  
You can otherwise view a ``.txt`` version of that same file by opening the ``.results`` application directory.  

You can also perform Battle Corruption easily on Desmume 0.9.11 by editing Motor.lua and modifying your Seed, your TID and your SID.  
You'll be required to have a save file with a full party of 6 Pokémon, all knocked out, for the glitch to work.  
The most common way to get to this state is with the Pomeg Glitch.  

The executable was compiled for the x86-64 architecture with MSVC v19.29, assuming the user's CPU supports the SSE4 instruction set.  

If you have any questions or issues regarding the application, direct them to me via Twitter @shinyhunter_map  
If you want to thank me with a little extra, you can donate at https://ko-fi.com/map233224 <3
