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
 * Miscellaneous entities, functs and functions.
 *
 * =======================================================================
 */

#include "header/local.h"

void
Use_Areaportal( edict_t *ent, edict_t *other /* unused */, edict_t *activator /* unused */ ) {
	if ( !ent ) {
		return;
	}

	ent->count ^= 1; /* toggle state */
	gi.SetAreaPortalState( ent->style, ent->count );
}

/*
 * QUAKED func_areaportal (0 0 0) ?
 *
 * This is a non-visible object that divides the world into
 * areas that are seperated when this portal is not activated.
 * Usually enclosed in the middle of a door.
 */
void
SP_func_areaportal( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->use = Use_Areaportal;
	ent->count = 0; /* always start closed; */
}

/* ===================================================== */

void
VelocityForDamage( int damage, vec3_t v ) {
	v[0] = 100.0 * crandom();
	v[1] = 100.0 * crandom();
	v[2] = 200.0 + 100.0 * random();

	if ( damage < 50 ) {
		VectorScale( v, 0.7, v );
	} else {
		VectorScale( v, 1.2, v );
	}
}

void
ThrowClientHead( edict_t *self, int damage ) {
	vec3_t vd;

	if ( !self ) {
		return;
	}

	self->s.origin[2] += 32;
	self->s.frame = 0;
	gi.setmodel( self, "" );
	VectorSet( self->mins, -16, -16, 0 );
	VectorSet( self->maxs, 16, 16, 16 );

	self->takedamage = DAMAGE_NO;
	self->solid = SOLID_BBOX;
	self->s.effects = 0;
	self->s.sound = 0;
	self->flags |= FL_NO_KNOCKBACK;

	self->movetype = MOVETYPE_BOUNCE;
	VelocityForDamage( damage, vd );
	VectorAdd( self->velocity, vd, self->velocity );

	if ( self->client ) { /* bodies in the queue don't have a client anymore */
		self->client->anim_priority = ANIM_DEATH;
		self->client->anim_end = self->s.frame;
	} else {
		self->think = NULL;
		self->nextthink = 0;
	}

	gi.linkentity( self );
}

/* ===================================================== */

/*
 * QUAKED path_corner (.5 .3 0) (-8 -8 -8) (8 8 8) TELEPORT
 * Target: next path corner
 * Pathtarget: gets used when an entity that has
 *             this path_corner targeted touches it
 */
void
path_corner_touch( edict_t *self, edict_t *other, cplane_t *plane /* unused */,
                   csurface_t *surf /* unused */ ) {
	vec3_t v;
	edict_t *next;

	if ( !self || !other ) {
		return;
	}

	if ( other->movetarget != self ) {
		return;
	}

	if ( other->enemy ) {
		return;
	}

	if ( self->pathtarget ) {
		char *savetarget;

		savetarget = self->target;
		self->target = self->pathtarget;
		G_UseTargets( self, other );
		self->target = savetarget;
	}

	if ( self->target ) {
		next = G_PickTarget( self->target );
	} else {
		next = NULL;
	}

	if ( ( next ) && ( next->spawnflags & 1 ) ) {
		VectorCopy( next->s.origin, v );
		v[2] += next->mins[2];
		v[2] -= other->mins[2];
		VectorCopy( v, other->s.origin );
		next = G_PickTarget( next->target );
		other->s.event = EV_OTHER_TELEPORT;
	}

	other->goalentity = other->movetarget = next;

	VectorSubtract( other->goalentity->s.origin, other->s.origin, v );
	other->ideal_yaw = vectoyaw( v );

}

void
SP_path_corner( edict_t *self ) {
	if ( !self ) {
		return;
	}

	if ( !self->targetname ) {
		gi.dprintf( "path_corner with no targetname at %s\n",
		            vtos( self->s.origin ) );
		G_FreeEdict( self );
		return;
	}

	self->solid = SOLID_TRIGGER;
	self->touch = path_corner_touch;
	VectorSet( self->mins, -8, -8, -8 );
	VectorSet( self->maxs, 8, 8, 8 );
	self->svflags |= SVF_NOCLIENT;
	gi.linkentity( self );
}

/* ===================================================== */

/*
 * QUAKED viewthing (0 .5 .8) (-8 -8 -8) (8 8 8)
 * Just for the debugging level.  Don't use
 */
void
TH_viewthing( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->s.frame = ( ent->s.frame + 1 ) % 7;
	ent->nextthink = level.time + FRAMETIME;
}

void
SP_viewthing( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	gi.dprintf( "viewthing spawned\n" );

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->s.renderfx = RF_FRAMELERP;
	VectorSet( ent->mins, -16, -16, -24 );
	VectorSet( ent->maxs, 16, 16, 32 );
	ent->s.modelindex = gi.modelindex( "models/objects/banner/tris.md2" );
	gi.linkentity( ent );
	ent->nextthink = level.time + 0.5;
	ent->think = TH_viewthing;
	return;
}

/* ===================================================== */

/*
 * QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
 * Used as a positional target for spotlights, etc.
 */
void
SP_info_null( edict_t *self ) {
	if ( !self ) {
		return;
	}

	G_FreeEdict( self );
}

/*
 * QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
 * Used as a positional target for lightning.
 */
void
SP_info_notnull( edict_t *self ) {
	if ( !self ) {
		return;
	}

	VectorCopy( self->s.origin, self->absmin );
	VectorCopy( self->s.origin, self->absmax );
}

#define START_OFF 1

/* ===================================================== */

/*
 * QUAKED func_wall (0 .5 .8) ? TRIGGER_SPAWN TOGGLE START_ON ANIMATED ANIMATED_FAST
 * This is just a solid wall if not inhibited
 *
 * TRIGGER_SPAWN	the wall will not be present until triggered
 *                  it will then blink in to existance; it will
 *                  kill anything that was in it's way
 *
 * TOGGLE			only valid for TRIGGER_SPAWN walls
 *                  this allows the wall to be turned on and off
 *
 * START_ON		only valid for TRIGGER_SPAWN walls
 *              the wall will initially be present
 */
void
func_wall_use( edict_t *self, edict_t *other /* unused */, edict_t *activator /* unused */ ) {
	if ( !self ) {
		return;
	}

	if ( self->solid == SOLID_NOT ) {
		self->solid = SOLID_BSP;
		self->svflags &= ~SVF_NOCLIENT;
		KillBox( self );
	} else {
		self->solid = SOLID_NOT;
		self->svflags |= SVF_NOCLIENT;
	}

	gi.linkentity( self );

	if ( !( self->spawnflags & 2 ) ) {
		self->use = NULL;
	}
}

void
SP_func_wall( edict_t *self ) {
	if ( !self ) {
		return;
	}

	self->movetype = MOVETYPE_PUSH;
	gi.setmodel( self, self->model );

	if ( self->spawnflags & 8 ) {
		self->s.effects |= EF_ANIM_ALL;
	}

	if ( self->spawnflags & 16 ) {
		self->s.effects |= EF_ANIM_ALLFAST;
	}

	/* just a wall */
	if ( ( self->spawnflags & 7 ) == 0 ) {
		self->solid = SOLID_BSP;
		gi.linkentity( self );
		return;
	}

	/* it must be TRIGGER_SPAWN */
	if ( !( self->spawnflags & 1 ) ) {
		self->spawnflags |= 1;
	}

	/* yell if the spawnflags are odd */
	if ( self->spawnflags & 4 ) {
		if ( !( self->spawnflags & 2 ) ) {
			gi.dprintf( "func_wall START_ON without TOGGLE\n" );
			self->spawnflags |= 2;
		}
	}

	self->use = func_wall_use;

	if ( self->spawnflags & 4 ) {
		self->solid = SOLID_BSP;
	} else {
		self->solid = SOLID_NOT;
		self->svflags |= SVF_NOCLIENT;
	}

	gi.linkentity( self );
}

/* ===================================================== */

/*
 * QUAKED func_object (0 .5 .8) ? TRIGGER_SPAWN ANIMATED ANIMATED_FAST
 * This is solid bmodel that will fall if it's support it removed.
 */
void
func_object_touch( edict_t *self, edict_t *other, cplane_t *plane,
                   csurface_t *surf /* unused */ ) {
	if ( !self || !other || !plane ) {
		return;
	}

	/* only squash thing we fall on top of */
	if ( !plane ) {
		return;
	}

	if ( plane->normal[2] < 1.0 ) {
		return;
	}

	if ( other->takedamage == DAMAGE_NO ) {
		return;
	}

	T_Damage( other, self, self, vec3_origin, self->s.origin,
	          vec3_origin, self->dmg, 1, 0, MOD_CRUSH );
}

void
func_object_release( edict_t *self ) {
	if ( !self ) {
		return;
	}

	self->movetype = MOVETYPE_TOSS;
	self->touch = func_object_touch;
}

void
func_object_use( edict_t *self, edict_t *other /* unused */, edict_t *activator /* unused */ ) {
	if ( !self ) {
		return;
	}

	self->solid = SOLID_BSP;
	self->svflags &= ~SVF_NOCLIENT;
	self->use = NULL;
	KillBox( self );
	func_object_release( self );
}

void
SP_func_object( edict_t *self ) {
	if ( !self ) {
		return;
	}

	gi.setmodel( self, self->model );

	self->mins[0] += 1;
	self->mins[1] += 1;
	self->mins[2] += 1;
	self->maxs[0] -= 1;
	self->maxs[1] -= 1;
	self->maxs[2] -= 1;

	if ( !self->dmg ) {
		self->dmg = 100;
	}

	if ( self->spawnflags == 0 ) {
		self->solid = SOLID_BSP;
		self->movetype = MOVETYPE_PUSH;
		self->think = func_object_release;
		self->nextthink = level.time + 2 * FRAMETIME;
	} else {
		self->solid = SOLID_NOT;
		self->movetype = MOVETYPE_PUSH;
		self->use = func_object_use;
		self->svflags |= SVF_NOCLIENT;
	}

	if ( self->spawnflags & 2 ) {
		self->s.effects |= EF_ANIM_ALL;
	}

	if ( self->spawnflags & 4 ) {
		self->s.effects |= EF_ANIM_ALLFAST;
	}

	self->clipmask = MASK_MONSTERSOLID;

	gi.linkentity( self );
}

/* ===================================================== */

/*
 * QUAKED misc_banner (1 .5 0) (-4 -4 -4) (4 4 4)
 * The origin is the bottom of the banner.
 * The banner is 128 tall.
 */
void
misc_banner_think( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->s.frame = ( ent->s.frame + 1 ) % 16;
	ent->nextthink = level.time + FRAMETIME;
}

void
SP_misc_banner( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->s.modelindex = gi.modelindex( "models/objects/banner/tris.md2" );
	ent->s.frame = randk() % 16;
	gi.linkentity( ent );

	ent->think = misc_banner_think;
	ent->nextthink = level.time + FRAMETIME;
}

extern void train_use( edict_t *self, edict_t *other, edict_t *activator );
extern void func_train_find( edict_t *self );

/* ===================================================== */

/*
 * QUAKED light_mine1 (0 1 0) (-2 -2 -12) (2 2 12)
 */
void
SP_light_mine1( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->s.modelindex = gi.modelindex( "models/objects/minelite/light1/tris.md2" );
	gi.linkentity( ent );
}

/*
 * QUAKED light_mine2 (0 1 0) (-2 -2 -12) (2 2 12)
 */
void
SP_light_mine2( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->s.modelindex = gi.modelindex( "models/objects/minelite/light2/tris.md2" );
	gi.linkentity( ent );
}

/* ===================================================== */

/*
 * QUAKED target_character (0 0 1) ?
 * used with target_string (must be on same "team")
 * "count" is position in the string (starts at 1)
 */
void
SP_target_character( edict_t *self ) {
	if ( !self ) {
		return;
	}

	self->movetype = MOVETYPE_PUSH;
	gi.setmodel( self, self->model );
	self->solid = SOLID_BSP;
	self->s.frame = 12;
	gi.linkentity( self );
	return;
}

/* ===================================================== */

/*
 * QUAKED target_string (0 0 1) (-8 -8 -8) (8 8 8)
 */
void
target_string_use( edict_t *self, edict_t *other /* unused */, edict_t *activator /* unused */ ) {
	edict_t *e;
	int n, l;
	char c;

	if ( !self ) {
		return;
	}

	l = strlen( self->message );

	for ( e = self->teammaster; e; e = e->teamchain ) {
		if ( !e->count ) {
			continue;
		}

		n = e->count - 1;

		if ( n > l ) {
			e->s.frame = 12;
			continue;
		}

		c = self->message[n];

		if ( ( c >= '0' ) && ( c <= '9' ) ) {
			e->s.frame = c - '0';
		} else if ( c == '-' ) {
			e->s.frame = 10;
		} else if ( c == ':' ) {
			e->s.frame = 11;
		} else {
			e->s.frame = 12;
		}
	}
}

void
SP_target_string( edict_t *self ) {
	if ( !self->message ) {
		self->message = "";
	}

	self->use = target_string_use;
}

/* ===================================================== */

/*
 * QUAKED func_clock (0 0 1) (-8 -8 -8) (8 8 8) TIMER_UP TIMER_DOWN START_OFF MULTI_USE
 * target a target_string with this
 *
 * The default is to be a time of day clock
 *
 * TIMER_UP and TIMER_DOWN run for "count" seconds and the fire "pathtarget"
 * If START_OFF, this entity must be used before it starts
 *
 * "style"	0 "xx"
 *          1 "xx:xx"
 *          2 "xx:xx:xx"
 */

#define CLOCK_MESSAGE_SIZE 16

/* don't let field width of any clock messages change, or it
   could cause an overwrite after a game load */

void
func_clock_reset( edict_t *self ) {
	if ( !self ) {
		return;
	}

	self->activator = NULL;

	if ( self->spawnflags & 1 ) {
		self->health = 0;
		self->wait = self->count;
	} else if ( self->spawnflags & 2 ) {
		self->health = self->count;
		self->wait = 0;
	}
}

/*
 * This is an evil hack to
 * prevent a rare crahs at
 * biggun exit. */
typedef struct zhead_s {
	struct zhead_s *prev, *next;
	short magic;
	short tag;
	int size;
} zhead_t;

void
func_clock_format_countdown( edict_t *self ) {
	if ( !self ) {
		return;
	}

	zhead_t *z = ( zhead_t * )self->message - 1;
	int size = z->size - sizeof ( zhead_t );

	if ( size < CLOCK_MESSAGE_SIZE ) {
		gi.TagFree ( self->message );
		self->message = gi.TagMalloc ( CLOCK_MESSAGE_SIZE, TAG_LEVEL );
	}

	if ( self->style == 0 ) {
		Com_sprintf( self->message, CLOCK_MESSAGE_SIZE, "%2i", self->health );
		return;
	}

	if ( self->style == 1 ) {
		Com_sprintf( self->message, CLOCK_MESSAGE_SIZE, "%2i:%2i",
		             self->health / 60, self->health % 60 );

		if ( self->message[3] == ' ' ) {
			self->message[3] = '0';
		}

		return;
	}

	if ( self->style == 2 ) {
		Com_sprintf( self->message, CLOCK_MESSAGE_SIZE, "%2i:%2i:%2i",
		             self->health / 3600,
		             ( self->health - ( self->health / 3600 ) * 3600 ) / 60,
		             self->health % 60 );

		if ( self->message[3] == ' ' ) {
			self->message[3] = '0';
		}

		if ( self->message[6] == ' ' ) {
			self->message[6] = '0';
		}

		return;
	}
}

void
func_clock_think( edict_t *self ) {
	if ( !self ) {
		return;
	}

	if ( !self->enemy ) {
		self->enemy = G_Find( NULL, FOFS( targetname ), self->target );

		if ( !self->enemy ) {
			return;
		}
	}

	if ( self->spawnflags & 1 ) {
		func_clock_format_countdown( self );
		self->health++;
	} else if ( self->spawnflags & 2 ) {
		func_clock_format_countdown( self );
		self->health--;
	} else {
		struct tm *ltime;
		time_t gmtime;

		time( &gmtime );
		ltime = localtime( &gmtime );
		Com_sprintf( self->message, CLOCK_MESSAGE_SIZE, "%2i:%2i:%2i",
		             ltime->tm_hour, ltime->tm_min, ltime->tm_sec );

		if ( self->message[3] == ' ' ) {
			self->message[3] = '0';
		}

		if ( self->message[6] == ' ' ) {
			self->message[6] = '0';
		}
	}

	self->enemy->message = self->message;
	self->enemy->use( self->enemy, self, self );

	if ( ( ( self->spawnflags & 1 ) && ( self->health > self->wait ) ) ||
	        ( ( self->spawnflags & 2 ) && ( self->health < self->wait ) ) ) {
		if ( self->pathtarget ) {
			char *savetarget;
			char *savemessage;

			savetarget = self->target;
			savemessage = self->message;
			self->target = self->pathtarget;
			self->message = NULL;
			G_UseTargets( self, self->activator );
			self->target = savetarget;
			self->message = savemessage;
		}

		if ( !( self->spawnflags & 8 ) ) {
			self->think = G_FreeEdict;
			self->nextthink = level.time + 1;
			return;
		}

		func_clock_reset( self );

		if ( self->spawnflags & 4 ) {
			return;
		}
	}

	self->nextthink = level.time + 1;
}

void
func_clock_use( edict_t *self, edict_t *other /* unused */, edict_t *activator ) {
	if ( !self || !activator ) {
		return;
	}

	if ( !( self->spawnflags & 8 ) ) {
		self->use = NULL;
	}

	if ( self->activator ) {
		return;
	}

	self->activator = activator;
	self->think( self );
}

void
SP_func_clock( edict_t *self ) {
	if ( !self ) {
		return;
	}

	if ( !self->target ) {
		gi.dprintf( "%s with no target at %s\n", self->classname,
		            vtos( self->s.origin ) );
		G_FreeEdict( self );
		return;
	}

	if ( ( self->spawnflags & 2 ) && ( !self->count ) ) {
		gi.dprintf( "%s with no count at %s\n", self->classname,
		            vtos( self->s.origin ) );
		G_FreeEdict( self );
		return;
	}

	if ( ( self->spawnflags & 1 ) && ( !self->count ) ) {
		self->count = 60 * 60;
	}

	func_clock_reset( self );

	self->message = gi.TagMalloc( CLOCK_MESSAGE_SIZE, TAG_LEVEL );

	self->think = func_clock_think;

	if ( self->spawnflags & 4 ) {
		self->use = func_clock_use;
	} else {
		self->nextthink = level.time + 1;
	}
}

/* ================================================================================= */

void
teleporter_touch( edict_t *self, edict_t *other, cplane_t *plane /* unused */,
                  csurface_t *surf /* unused */ ) {
	edict_t *dest;
	int i;

	if ( !self || !other ) {
		return;
	}

	if ( !other->client ) {
		return;
	}

	dest = G_Find( NULL, FOFS( targetname ), self->target );

	if ( !dest ) {
		gi.dprintf( "Couldn't find destination\n" );
		return;
	}

	/* unlink to make sure it can't possibly interfere with KillBox */
	gi.unlinkentity( other );

	VectorCopy( dest->s.origin, other->s.origin );
	VectorCopy( dest->s.origin, other->s.old_origin );
	other->s.origin[2] += 10;

	/* clear the velocity and hold them in place briefly */
	VectorClear( other->velocity );
	other->client->ps.pmove.pm_time = 160 >> 3;       /* hold time */
	other->client->ps.pmove.pm_flags |= PMF_TIME_TELEPORT;

	/* draw the teleport splash at source and on the player */
	self->owner->s.event = EV_PLAYER_TELEPORT;
	other->s.event = EV_PLAYER_TELEPORT;

	/* set angles */
	for ( i = 0; i < 3; i++ ) {
		other->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(
		            dest->s.angles[i] - other->client->resp.cmd_angles[i] );
	}

	VectorClear( other->s.angles );
	VectorClear( other->client->ps.viewangles );
	VectorClear( other->client->v_angle );

	/* kill anything at the destination */
	KillBox( other );

	gi.linkentity( other );
}

/*
 * QUAKED misc_teleporter (1 0 0) (-32 -32 -24) (32 32 -16)
 * Stepping onto this disc will teleport players to the targeted misc_teleporter_dest object.
 */
void
SP_misc_teleporter( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	edict_t *trig;

	if ( !ent->target ) {
		gi.dprintf( "teleporter without a target.\n" );
		G_FreeEdict( ent );
		return;
	}

	gi.setmodel( ent, "models/objects/dmspot/tris.md2" );
	ent->s.skinnum = 1;
	ent->s.effects = EF_TELEPORTER;
	ent->s.sound = gi.soundindex( "world/amb10.wav" );
	ent->solid = SOLID_BBOX;

	VectorSet( ent->mins, -32, -32, -24 );
	VectorSet( ent->maxs, 32, 32, -16 );
	gi.linkentity( ent );

	trig = G_Spawn();
	trig->touch = teleporter_touch;
	trig->solid = SOLID_TRIGGER;
	trig->target = ent->target;
	trig->owner = ent;
	VectorCopy( ent->s.origin, trig->s.origin );
	VectorSet( trig->mins, -8, -8, 8 );
	VectorSet( trig->maxs, 8, 8, 24 );
	gi.linkentity( trig );
}

/*
 * QUAKED misc_teleporter_dest (1 0 0) (-32 -32 -24) (32 32 -16)
 * Point teleporters at these.
 */
void
SP_misc_teleporter_dest( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	gi.setmodel( ent, "models/objects/dmspot/tris.md2" );
	ent->s.skinnum = 0;
	ent->solid = SOLID_BBOX;
	VectorSet( ent->mins, -32, -32, -24 );
	VectorSet( ent->maxs, 32, 32, -16 );
	gi.linkentity( ent );
}
