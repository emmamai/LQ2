LQ2
========

## About

 LQ2 is a fork of the Quake II source code, with the intention of cleaning up the engine code and build system, and modifying the engine to run game code built using Lua rather than C. Note that the intention is not to create a game module that allows loading of Lua code, but rather to replace the loadable game module entirely.

 The scope of the project includes a full rewrite of the point release 3.19 game code in Lua, allowing the base game to be played using this engine. In the future, the CTF, rogue, and xatrix code may also be rewritten.

## Building ##

 ### Supported platforms ##
  LQ2 is written and tested under Linux, and has no current support for Windows. Windows support has not been pulled from the code, but there is currently no way to build it. A makefile target using the mingw32 compiler suite may be added later, as well as a Code::Blocks project.

 ### Prerequisites ###
  You need a reasonable C compiler and library.
  Requirements are not certain at this point, but have libx11 and libsdl1.2 installed at the very least.

 ### Compiling ###
  Just type make.

## License ##

 LQ2, like the original Quake II source code, is licensed under the GPL. Read the COPYING file for more information. The GPL does not cover the Quake II data files, so please ensure that you do not distribute the game data files alongside the source code or executable.
