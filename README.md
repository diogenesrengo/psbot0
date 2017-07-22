# psbot0
A little library to automate poker tables (Pokerstars only)

This code create a shared library with some functions to automate tables of 
texas's holdem poker, 2/6/9 seats, cash or tourney (PokerStars only). It can 
capture data from tables like cards, stacks, stage, position and more. It also 
permits clicking in buttons and write in box of chat and bet. 
See screenshot0.png for table's design needed by the library. See psbot0.h for 
types and variables and directory ./examples for examples. The library was 
tested in play money and open skill league from pokerstars software running
over wine in slackware and ubuntu distros; results were very good, humbly.

### Dependencies 
- some linux distribution						<br/>
- slackware 14.2: xdotool						<br/>
- ubuntu 16.04: xdotool libxdo-dev imagemagick libmagick++-dev		<br/>

### Installation  							
```
./configure
make && sudo make install
```
**For examples\ :** `make examples`					<br/>
**Your own code:** `gcc -g -O2 -Wall -o mybot mybot.c -lpsbot0`		<br/>

### Screenshot
<img src="https://github.com/diogenesrengo/psbot0/blob/master/screenshot0.png"/>

This code is for educational purposes only. Automated software are generally
prohibited by terms of service of your game provider ;)

