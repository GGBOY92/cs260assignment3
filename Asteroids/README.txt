Readme File for CS260 Assignment 4: Spaceships!
Date: 12/12/2010
Team: Steven Liss, Michael Travaglione

Instructions:
==============

Example config.ini file:

<username> Michael
<server UDP port> 8000
<server IP> 192.168.1.7

There is an example with the submitted project. The config file MUST be in the same directory as the 
executable. OR when running through Visual Studio, in the same directory as the .vcproj file.

In order to play in networked game, choose "Play with others" from main menu. This begins a series of rounds
between networked players.


Notes:
======
There is a #define called CLIENT_APP in main.h which must be toggled in order to run as the server or client.
You will need to change this define in order to build and generate the server executable.

You can also change the default round duration in main.h. It is defined as ROUND_TIME_SEC. THIS WILL ONLY CHANGE THE
ROUND TIME IF RUNNING AS THE SERVER.

Two users cannot have the same user name. The second player will simply be rejected. Please change the username field in the 
config file for each player.

The maxmimum amount of players allowed is 8. Additional players will not be allowed to join a game.

Game uses CS230 engine provided on distance.digipen.edu.

Particles are intentionally disabled in networked play.


Program follows guidelines given for assignment:
================================================
All required capabilities.

Extra Credit:
=============

We have attempted the following extra credit:

5 points- can play any number of players between 2 and 8
3 points- can join a game already in-progress
2 points- can play multiple rounds

=========================================================================================

Thanks for reading.