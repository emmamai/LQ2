/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 * Copyright (C) 2011 Knightmare
 * Copyright (C) 2011 Yamagi Burmeister
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * The savegame system.
 *
 * =======================================================================
 */

/*
 * This is the Quake 2 savegame system, fixed by Yamagi
 * based on an idea by Knightmare of kmquake2. This major
 * rewrite of the original g_save.c is much more robust
 * and portable since it doesn't use any function pointers.
 *
 * Inner workings:
 * When the game is saved all function pointers are
 * translated into human readable function definition strings.
 * The same way all mmove_t pointers are translated. This
 * human readable strings are then written into the file.
 * At game load the human readable strings are retranslated
 * into the actual function pointers and struct pointers. The
 * pointers are generated at each compilation / start of the
 * client, thus the pointers are always correct.
 *
 * Limitations:
 * While savegames survive recompilations of the game source
 * and bigger changes in the source, there are some limitation
 * which a nearly impossible to fix without a object orientated
 * rewrite of the game.
 *  - If functions or mmove_t structs that a referencenced
 *    inside savegames are added or removed (e.g. the files
 *    in tables/ are altered) the load functions cannot
 *    reconnect all pointers and thus not restore the game.
 *  - If the operating system is changed internal structures
 *    may change in an unrepairable way.
 *  - If the architecture is changed pointer length and
 *    other internal datastructures change in an
 *    incompatible way.
 *  - If the edict_t struct is changed, savegames
 *    will break.
 * This is not so bad as it looks since functions and
 * struct won't be added and edict_t won't be changed
 * if no big, sweeping changes are done. The operating
 * system and architecture are in the hands of the user.
 */

#include "../header/local.h"

/*
 * When ever the savegame version is changed, q2 will refuse to
 * load older savegames. This should be bumped if the files
 * in tables/ are changed, otherwise strange things may happen.
 */
#define SAVEGAMEVER "LQ2"

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__
#endif

/*
 * This macros are used to prohibit loading of savegames
 * created on other systems or architectures. This will
 * crash q2 in spectacular ways
 */
#ifndef YQ2OSTYPE
#error YQ2OSTYPE should be defined by the build system
#endif

#ifndef YQ2ARCH
#error YQ2ARCH should be defined by the build system
#endif

/*
 * Older operating systen and architecture detection
 * macros, implemented by savegame version YQ2-1.
 */
#if defined(__APPLE__)
#define OSTYPE_1 "MacOS X"
#elif defined(__FreeBSD__)
#define OSTYPE_1 "FreeBSD"
#elif defined(__OpenBSD__)
#define OSTYPE_1 "OpenBSD"
#elif defined(__linux__)
#define OSTYPE_1 "Linux"
#elif defined(_WIN32)
#define OSTYPE_1 "Windows"
#else
#define OSTYPE_1 "Unknown"
#endif

#if defined(__i386__)
#define ARCH_1 "i386"
#elif defined(__x86_64__)
#define ARCH_1 "amd64"
#elif defined(__sparc__)
#define ARCH_1 "sparc64"
#elif defined(__ia64__)
#define ARCH_1 "ia64"
#else
#define ARCH_1 "unknown"
#endif

/*
 * Fields to be saved
 */
field_t fields[] = {
#include "tables/fields.h"
};

/* ========================================================= */

/*
 * This will be called when the dll is first loaded,
 * which only happens when a new game is started or
 * a save game is loaded.
 */
void InitGame( void ) {
	gi.dprintf( "InitGame: %s built on %s.\n", GAMEVERSION, BUILD_DATE );

	gun_x = gi.cvar( "gun_x", "0", 0 );
	gun_y = gi.cvar( "gun_y", "0", 0 );
	gun_z = gi.cvar( "gun_z", "0", 0 );
	sv_rollspeed = gi.cvar( "sv_rollspeed", "200", 0 );
	sv_rollangle = gi.cvar( "sv_rollangle", "2", 0 );
	sv_maxvelocity = gi.cvar( "sv_maxvelocity", "2000", 0 );
	sv_gravity = gi.cvar( "sv_gravity", "800", 0 );

	/* noset vars */
	dedicated = gi.cvar( "dedicated", "0", CVAR_NOSET );

	/* latched vars */
	sv_cheats = gi.cvar( "cheats", "0", CVAR_SERVERINFO | CVAR_LATCH );
	gi.cvar( "gamename", GAMEVERSION, CVAR_SERVERINFO | CVAR_LATCH );
	gi.cvar( "gamedate", BUILD_DATE, CVAR_SERVERINFO | CVAR_LATCH );
	maxclients = gi.cvar( "maxclients", "4", CVAR_SERVERINFO | CVAR_LATCH );
	maxspectators = gi.cvar( "maxspectators", "4", CVAR_SERVERINFO );
	maxentities = gi.cvar( "maxentities", "1024", CVAR_LATCH );

	/* change anytime vars */
	dmflags = gi.cvar( "dmflags", "0", CVAR_SERVERINFO );
	fraglimit = gi.cvar( "fraglimit", "0", CVAR_SERVERINFO );
	timelimit = gi.cvar( "timelimit", "0", CVAR_SERVERINFO );
	password = gi.cvar( "password", "", CVAR_USERINFO );
	spectator_password = gi.cvar( "spectator_password", "", CVAR_USERINFO );
	needpass = gi.cvar( "needpass", "0", CVAR_SERVERINFO );
	g_select_empty = gi.cvar( "g_select_empty", "0", CVAR_ARCHIVE );
	run_pitch = gi.cvar( "run_pitch", "0.002", 0 );
	run_roll = gi.cvar( "run_roll", "0.005", 0 );
	bob_up = gi.cvar( "bob_up", "0.005", 0 );
	bob_pitch = gi.cvar( "bob_pitch", "0.002", 0 );
	bob_roll = gi.cvar( "bob_roll", "0.002", 0 );

	/* flood control */
	flood_msgs = gi.cvar( "flood_msgs", "4", 0 );
	flood_persecond = gi.cvar( "flood_persecond", "4", 0 );
	flood_waitdelay = gi.cvar( "flood_waitdelay", "10", 0 );

	/* dm map list */
	sv_maplist = gi.cvar( "sv_maplist", "", 0 );

	/* initialize all entities for this game */
	game.maxentities = maxentities->value;
	g_edicts = gi.TagMalloc( game.maxentities * sizeof( g_edicts[0] ), TAG_GAME );
	globals.edicts = g_edicts;
	globals.max_edicts = game.maxentities;

	/* initialize all clients for this game */
	game.maxclients = maxclients->value;
	game.clients = gi.TagMalloc( game.maxclients * sizeof( game.clients[0] ), TAG_GAME );
	globals.num_edicts = game.maxclients + 1;
}

void WriteGame( const char *filename, qboolean autosave ) {
	return;
}

void ReadGame( const char *filename ) {
	return;
}

void WriteLevel( const char *filename ) {
	return;
}

void ReadLevel( const char *filename ) {
	return;
}
