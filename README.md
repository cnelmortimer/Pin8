# Pin8
Pin8 is a native Chip-8 emulator for Haiku OS (which should also compile in Zeta and BeOS). It has been written in C++ for the BeOS/Haiku API and its main aim is to be simple and fun.

## Features
- Responsive interface
- Threaded user-interface with both virtual keyboard and physical one.
- Sound for games (beep-system configuration is required, though).
- Two run modes: standard and fast (540 Hz and 1000 Hz) (Up to my knowledge, Chip-8 does not define an explicit frequency... but these values have let me enjoy all games I have tried so far)

## Motivation
I just wanted to learn how native software for the BeOS family is created in C++ and I just decided to write this program as a first project. Besides, as I have never seen this kind of emulator in these systems, I thought that it was a good idea.

## License
This program has been written by Nicolás C. Cruz (UAL), (August, 2017) and it can be both used and modified by anyone interested at it for free. However, this initial autorship must be referenced. No commercial versions of the program can be created. This software is distributed with good intentions but without any kind of warranty and responsability.

## Acknowledgements
First, I would like to think all people who has documented the Chip-8 scpecification as well as created and shared their own emulators (see the comments in the core). Thank to their work, I could create my initial Chip-8 emulator in Java. Second, I wish to thank all kind users of BeShare and the Haiku IRC channel for helping me with some doubts regarding the BeAPI. Third and last, I would like to thank DarkWyrm (Jon Yoder) for his great book "Learning to program with Haiku"

## Additional information
Writing Chip-8 games is a very funny experience. If you are interested at learning the Chip-8 assembly and creating your own games, I highly recommend the online IDE Octo, by John Earnest.
