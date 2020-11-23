# Motor
This application has been designed to help Pokémon Diamond, Pearl and Platinum players perform the Battle Corruption Glitch.
This glitch allows players to catch any desired species in the place of the static Rotom encounter in Old Chateau.

To build on Windows 10 with GCC, use the command ``gcc -O3 Motor.c -o Motor`` in Windows Shell.

Each user will need to provide several things to the application:

- their Trainer and Secret ID (TID and SID)
- their game's version (Diamond, Pearl or Platinum)
- their game's language (Japanese, English, French, Italian, German, Spanish, Korean)
- their target species (optional, from Bulbasaur to Arceus)
- how many seeds they want to go through (maximum 4,294,967,295)

*NB: only Pokémon Platinum for the English language is currently supported.* 

Once all of these parameters have been entered, the application will go through each seed and output every successful hit.
It will provide the seed on which it found the Pokémon on, alongside several other characteristics (level, item, ability, moves, ...).

...
