# psbot0

This code create a shared library with some functions to automate tables of 
texas's holdem poker, 2/6/9 seats, cash or tourney (PokerStars only). It can 
capture data from tables like cards, stacks, stage, position and more. It also 
permits clicking in buttons and write in box of chat and bet. 
See screenshot0.png for table's design needed by the library. See psbot0.h for 
types and variables and directory ./examples for examples. The library was 
tested in play money and open skill league.

Dependencies: some linux distro and packages ...
slackware 14.2 : xdotool
ubuntu    16.04: xdotool libxdo-dev imagemagick libmagick++-dev

For install:   `./configure; sudo make && sudo make install`
For examples:  `make examples`
Your own code: `gcc -g -O2 -Wall -o mybot mybot.c -lpsbot0`

<img src="https://github.com/diogenesrengo/psbot0/blob/master/screenshot0.png"/>

This code is for educational purposes only. Automated software are generally
prohibited by terms of service of your game provider ;)

