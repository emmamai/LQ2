/*
 * Copyright (C) 1997-2001 Id Software, Inc.
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
 * Main header file for the game module.
 *
 * =======================================================================
 */

#ifndef GAME_LOCAL_H
#define GAME_LOCAL_H

#include "../../common/header/shared.h"

/* define GAME_INCLUDE so that game.h does not define the
   short, server-visible gclient_t and edict_t structures,
   because we define the full size ones in this file */
#define GAME_INCLUDE
#include "game.h"

/* the "gameversion" client command will print this plus compile date */
#define GAMEVERSION "baseq2"

/* protocol bytes that can be directly added to messages */
#define svc_muzzleflash 1
#define svc_muzzleflash2 2
#define svc_temp_entity 3
#define svc_layout 4
#define svc_inventory 5
#define svc_stufftext 11

/* ================================================================== */

/* view pitching times */
#define DAMAGE_TIME 0.5
#define FALL_TIME 0.3

#define FL_FLY 0x00000001
#define FL_SWIM 0x00000002 /* implied immunity to drowining */
#define FL_IMMUNE_LASER 0x00000004
#define FL_INWATER 0x00000008
#define FL_GODMODE 0x00000010
#define FL_NOTARGET 0x00000020
#define FL_IMMUNE_SLIME 0x00000040
#define FL_IMMUNE_LAVA 0x00000080
#define FL_PARTIALGROUND 0x00000100 /* not all corners are valid */
#define FL_WATERJUMP 0x00000200 /* player jumping out of water */
#define FL_TEAMSLAVE 0x00000400 /* not the first on the team */
#define FL_NO_KNOCKBACK 0x00000800

#define FRAMETIME 0.1

/* memory tags to allow dynamic memory to be cleaned up */
#define TAG_GAME 765 /* clear when unloading the dll */
#define TAG_LEVEL 766 /* clear when loading a new level */

typedef enum {
	DAMAGE_NO,
	DAMAGE_YES, /* will take damage if hit */
	DAMAGE_AIM /* auto targeting recognizes this */
} damage_t;

typedef enum {
	WEAPON_READY,
	WEAPON_ACTIVATING,
	WEAPON_DROPPING,
	WEAPON_FIRING
} weaponstate_t;

/* deadflag */
#define DEAD_NO 0
#define DEAD_DYING 1
#define DEAD_DEAD 2
#define DEAD_RESPAWNABLE 3

/* handedness values */
#define RIGHT_HANDED 0
#define LEFT_HANDED 1
#define CENTER_HANDED 2

/* game.serverflags values */
#define SFL_CROSS_TRIGGER_1 0x00000001
#define SFL_CROSS_TRIGGER_2 0x00000002
#define SFL_CROSS_TRIGGER_3 0x00000004
#define SFL_CROSS_TRIGGER_4 0x00000008
#define SFL_CROSS_TRIGGER_5 0x00000010
#define SFL_CROSS_TRIGGER_6 0x00000020
#define SFL_CROSS_TRIGGER_7 0x00000040
#define SFL_CROSS_TRIGGER_8 0x00000080
#define SFL_CROSS_TRIGGER_MASK 0x000000ff

/* edict->movetype values */
typedef enum {
	MOVETYPE_NONE, /* never moves */
	MOVETYPE_NOCLIP, /* origin and angles change with no interaction */
	MOVETYPE_PUSH, /* no clip to world, push on box contact */
	MOVETYPE_STOP, /* no clip to world, stops on box contact */

	MOVETYPE_WALK, /* gravity */
	MOVETYPE_FLY,
	MOVETYPE_TOSS, /* gravity */
	MOVETYPE_FLYMISSILE, /* extra size to monsters */
	MOVETYPE_BOUNCE
} movetype_t;

#define WEAP_SHOTGUN 1

/* this structure is left intact through an entire game
   it should be initialized at dll load time, and read/written to
   the server.ssv file for savegames */
typedef struct {
	gclient_t *clients; /* [maxclients] */

	/* store latched cvars here that we want to get at often */
	int maxclients;
	int maxentities;

	/* cross level triggers */
	int serverflags;

	qboolean autosaved;
} game_locals_t;

/* this structure is cleared as each map is entered
   it is read/written to the level.sav file for savegames */
typedef struct {
	int framenum;
	float time;

	char level_name[MAX_QPATH]; /* the descriptive name (Outer Base, etc) */
	char mapname[MAX_QPATH]; /* the server name (base1, etc) */
	char nextmap[MAX_QPATH]; /* go here when fraglimit is hit */

	/* intermission state */
	float intermissiontime; /* time the intermission was started */
	char *changemap;
	int exitintermission;
	vec3_t intermission_origin;
	vec3_t intermission_angle;

	edict_t *current_entity; /* entity running from G_RunFrame */
} level_locals_t;

/* spawn_temp_t is only used to hold entity field values that
   can be set from the editor, but aren't actualy present
   in edict_t during gameplay */
typedef struct {
	/* world vars */
	char *sky;
	float skyrotate;
	vec3_t skyaxis;
	char *nextmap;

	int lip;
	int distance;
	int height;
	char *noise;
	float pausetime;
	char *gravity;

	float minyaw;
	float maxyaw;
	float minpitch;
	float maxpitch;
} spawn_temp_t;

typedef struct {
	/* fixed data */
	vec3_t start_origin;
	vec3_t start_angles;
	vec3_t end_origin;
	vec3_t end_angles;

	int sound_start;
	int sound_middle;
	int sound_end;

	float accel;
	float speed;
	float decel;
	float distance;

	float wait;

	/* state data */
	int state;
	vec3_t dir;
	float current_speed;
	float move_speed;
	float next_speed;
	float remaining_distance;
	float decel_distance;
	void ( *endfunc )( edict_t * );
} moveinfo_t;

extern game_locals_t game;
extern level_locals_t level;
extern game_import_t gi;
extern game_export_t globals;
extern spawn_temp_t st;

extern int snd_fry;

/* means of death */
#define MOD_UNKNOWN 0
#define MOD_RAILGUN 11
#define MOD_WATER 17
#define MOD_SLIME 18
#define MOD_LAVA 19
#define MOD_CRUSH 20
#define MOD_TELEFRAG 21
#define MOD_FALLING 22
#define MOD_SUICIDE 23
#define MOD_EXPLOSIVE 25
#define MOD_BARREL 26
#define MOD_BOMB 27
#define MOD_EXIT 28
#define MOD_SPLASH 29
#define MOD_TARGET_LASER 30
#define MOD_TRIGGER_HURT 31
#define MOD_HIT 32
#define MOD_TARGET_BLASTER 33
#define MOD_FRIENDLY_FIRE 0x8000000

extern int meansOfDeath;

extern edict_t *g_edicts;

#define FOFS(x) (size_t)&(((edict_t *)NULL)->x)
#define STOFS(x) (size_t)&(((spawn_temp_t *)NULL)->x)
#define LLOFS(x) (size_t)&(((level_locals_t *)NULL)->x)
#define CLOFS(x) (size_t)&(((gclient_t *)NULL)->x)

#define random() ((randk() & 0x7fff) / ((float)0x7fff))
#define crandom() (2.0 * (random() - 0.5))

extern cvar_t *maxentities;
extern cvar_t *deathmatch;
extern cvar_t *dmflags;
extern cvar_t *skill;
extern cvar_t *g_select_empty;
extern cvar_t *dedicated;

extern cvar_t *sv_gravity;
extern cvar_t *sv_maxvelocity;

extern cvar_t *sv_rollspeed;
extern cvar_t *sv_rollangle;

extern cvar_t *run_pitch;
extern cvar_t *run_roll;
extern cvar_t *bob_up;
extern cvar_t *bob_pitch;
extern cvar_t *bob_roll;

extern cvar_t *sv_cheats;
extern cvar_t *maxclients;

extern cvar_t *sv_maplist;

#define world (&g_edicts[0])

/* item spawnflags */
#define ITEM_TRIGGER_SPAWN 0x00000001
#define ITEM_NO_TOUCH 0x00000002
/* 6 bits reserved for editor flags */
/* 8 bits used as power cube id bits for coop games */
#define DROPPED_ITEM 0x00010000
#define DROPPED_PLAYER_ITEM 0x00020000
#define ITEM_TARGETS_USED 0x00040000

/* fields are needed for spawning from the entity
   string and saving / loading games */
#define FFL_SPAWNTEMP 1
#define FFL_NOSPAWN 2

typedef enum {
	F_INT,
	F_FLOAT,
	F_LSTRING, /* string on disk, pointer in memory, TAG_LEVEL */
	F_GSTRING, /* string on disk, pointer in memory, TAG_GAME */
	F_VECTOR,
	F_ANGLEHACK,
	F_EDICT, /* index on disk, pointer in memory */
	F_ITEM, /* index on disk, pointer in memory */
	F_CLIENT, /* index on disk, pointer in memory */
	F_FUNCTION,
	F_MMOVE,
	F_IGNORE
} fieldtype_t;

typedef struct {
	char *name;
	int ofs;
	fieldtype_t type;
	int flags;
} field_t;

extern field_t fields[];

/* g_cmds.c */
void Cmd_Score_f( edict_t *ent );

void ChangeWeapon( edict_t *ent );
void Think_Weapon( edict_t *ent );

/* g_utils.c */
qboolean KillBox( edict_t *ent );
edict_t *G_Find( edict_t *from, int fieldofs, char *match );
edict_t *G_PickTarget( char *targetname );
void G_UseTargets( edict_t *ent, edict_t *activator );
void G_SetMovedir( vec3_t angles, vec3_t movedir );

void G_InitEdict( edict_t *e );
edict_t *G_Spawn( void );
void G_FreeEdict( edict_t *e );

void G_TouchTriggers( edict_t *ent );

char *vtos( vec3_t v );

float vectoyaw( vec3_t vec );

/* damage flags */
#define DAMAGE_NO_KNOCKBACK 0x00000001 /* do not affect velocity, just view angles */
#define DAMAGE_NO_PROTECTION 0x0000002 /* armor, shields, invulnerability, and godmode have no effect */

/* g_misc.c */
void ThrowClientHead( edict_t *self, int damage );

/* g_client.c */
void respawn( edict_t *ent );
void PutClientInServer( edict_t *ent );
void InitClientPersistant( gclient_t *client );
void InitClientResp( gclient_t *client );
void ClientBeginServerFrame( edict_t *ent );

/* g_player.c */
void player_pain( edict_t *self, edict_t *other, float kick, int damage );
void player_die( edict_t *self, edict_t *inflictor, edict_t *attacker,
                 int damage, vec3_t point );

/* g_svcmds.c */
void ServerCommand( void );

/* p_view.c */
void ClientEndServerFrame( edict_t *ent );

/* g_phys.c */
void G_RunEntity( edict_t *ent );

/* g_main.c */
void SaveClientData( void );
void FetchClientEntData( edict_t *ent );

/* ============================================================================ */

/* client_t->anim_priority */
#define ANIM_BASIC 0 /* stand / run */
#define ANIM_WAVE 1
#define ANIM_JUMP 2
#define ANIM_PAIN 3
#define ANIM_ATTACK 4
#define ANIM_DEATH 5
#define ANIM_REVERSE 6

/* client data that stays across multiple level loads */
typedef struct {
	char userinfo[MAX_INFO_STRING];
	char netname[16];
	int hand;

	qboolean connected; /* a loadgame will leave valid entities that
						   just don't have a connection yet */

	/* values saved and restored
	   from edicts when changing levels */
	int health;
	int max_health;
	int savedFlags;

	int selected_item;

	qboolean weapon;
} client_persistant_t;

/* client data that stays across deathmatch respawns */
typedef struct {
	client_persistant_t coop_respawn; /* what to set client->pers to on a respawn */
	int enterframe; /* level.framenum the client entered the game */
	int score; /* frags, etc */
	vec3_t cmd_angles; /* angles sent over in the last command */
} client_respawn_t;

/* this structure is cleared on each PutClientInServer(),
   except for 'client->pers' */
struct gclient_s {
	/* known to server */
	player_state_t ps; /* communicated by server to clients */
	int ping;

	/* private to game */
	client_persistant_t pers;
	client_respawn_t resp;
	pmove_state_t old_pmove; /* for detecting out-of-pmove changes */

	qboolean showscores; /* set layout stat */

	int buttons;
	int oldbuttons;
	int latched_buttons;

	qboolean weapon_thunk;
	qboolean newweapon;

	/* sum up damage over an entire frame, so
	   shotgun blasts give a single big kick */
	int damage_blood; /* damage taken out of health */
	int damage_knockback; /* impact damage */
	vec3_t damage_from; /* origin for vector calculation */

	float killer_yaw; /* when dead, look at killer */

	weaponstate_t weaponstate;
	vec3_t kick_angles; /* weapon kicks */
	vec3_t kick_origin;
	float v_dmg_roll, v_dmg_pitch, v_dmg_time; /* damage kicks */
	float fall_time, fall_value; /* for view drop on fall */
	float damage_alpha;
	float bonus_alpha;
	vec3_t damage_blend;
	vec3_t v_angle; /* aiming direction */
	float bobtime; /* so off-ground doesn't change it */
	vec3_t oldviewangles;
	vec3_t oldvelocity;

	float next_drown_time;
	int old_waterlevel;

	/* animation vars */
	int anim_end;
	int anim_priority;
	qboolean anim_duck;
	qboolean anim_run;

	/* powerup timers */
	float invincible_framenum;

	float respawn_time; /* can respawn when time > this */
};

struct edict_s {
	entity_state_t s;
	struct gclient_s *client; /* NULL if not a player
							     the server expects the first part
							     of gclient_s to be a player_state_t
							     but the rest of it is opaque */

	qboolean inuse;
	int linkcount;

	link_t area; /* linked to a division node or leaf */

	int num_clusters; /* if -1, use headnode instead */
	int clusternums[MAX_ENT_CLUSTERS];
	int headnode; /* unused if num_clusters != -1 */
	int areanum, areanum2;

	/* ================================ */

	int svflags;
	vec3_t mins, maxs;
	vec3_t absmin, absmax, size;
	solid_t solid;
	int clipmask;
	edict_t *owner;

	/* DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER */
	/* EXPECTS THE FIELDS IN THAT ORDER! */

	/* ================================ */
	int movetype;
	int flags;

	char *model;
	float freetime; /* sv.time when the object was freed */

	/* only used locally in game, not by server */
	char *message;
	char *classname;
	int spawnflags;

	float timestamp;

	float angle; /* set in qe3, -1 = up, -2 = down */
	char *target;
	char *targetname;
	char *killtarget;
	char *team;
	char *pathtarget;
	edict_t *target_ent;

	float speed, accel, decel;
	vec3_t movedir;
	vec3_t pos1, pos2;

	vec3_t velocity;
	vec3_t avelocity;
	int mass;
	float air_finished;
	float gravity; /* per entity gravity multiplier (1.0 is normal)
				      use for lowgrav artifact, flares */

	edict_t *goalentity;
	edict_t *movetarget;
	float yaw_speed;
	float ideal_yaw;

	float nextthink;
	void ( *prethink )( edict_t *ent );
	void ( *think )( edict_t *self );
	void ( *blocked )( edict_t *self, edict_t *other );
	void ( *touch )( edict_t *self, edict_t *other, cplane_t *plane,
	                 csurface_t *surf );
	void ( *use )( edict_t *self, edict_t *other, edict_t *activator );
	void ( *pain )( edict_t *self, edict_t *other, float kick, int damage );
	void ( *die )( edict_t *self, edict_t *inflictor, edict_t *attacker,
	               int damage, vec3_t point );

	float touch_debounce_time;
	float pain_debounce_time;
	float damage_debounce_time;
	float fly_sound_debounce_time;

	int health;
	int max_health;
	int deadflag;

	char *map; /* target_changelevel */

	int viewheight; /* height above origin where eyesight is determined */
	int takedamage;
	int dmg;
	int sounds; /* make this a spawntemp var? */
	int count;

	edict_t *chain;
	edict_t *enemy;
	edict_t *oldenemy;
	edict_t *activator;
	edict_t *groundentity;
	int groundentity_linkcount;
	edict_t *teamchain;
	edict_t *teammaster;

	int noise_index;
	int noise_index2;
	float volume;
	float attenuation;

	/* timing variables */
	float wait;
	float delay; /* before firing targets */
	float random;

	float last_sound_time;

	int watertype;
	int waterlevel;

	vec3_t move_origin;
	vec3_t move_angles;

	/* move this to clientinfo? */
	int light_level;

	int style; /* also used as areaportal number */

	/* common data blocks */
	moveinfo_t moveinfo;
};

#endif /* GAME_LOCAL_H */
