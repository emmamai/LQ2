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
 * The basic AI functions like enemy detection, attacking and so on.
 *
 * =======================================================================
 */

#include "header/local.h"

extern cvar_t *maxclients;

qboolean enemy_vis;
qboolean enemy_infront;
int enemy_range;
float enemy_yaw;

qboolean FindTarget(edict_t *self);
qboolean ai_checkattack(edict_t *self);

/*
 * Called once each frame to set level.sight_client
 * to the player to be checked for in findtarget.
 * If all clients are either dead or in notarget,
 * sight_client will be null.
 * In coop games, sight_client will cycle
 * between the clients.
 */
void
AI_SetSightClient(void)
{
	edict_t *ent;
	int start, check;

	if (level.sight_client == NULL)
	{
		start = 1;
	}
	else
	{
		start = level.sight_client - g_edicts;
	}

	check = start;

	while (1)
	{
		check++;

		if (check > game.maxclients)
		{
			check = 1;
		}

		ent = &g_edicts[check];

		if (ent->inuse &&
			(ent->health > 0) &&
			!(ent->flags & FL_NOTARGET))
		{
			level.sight_client = ent;
			return; /* got one */
		}

		if (check == start)
		{
			level.sight_client = NULL;
			return; /* nobody to see */
		}
	}
}

/*
 * Move the specified distance at current facing.
 */
void
ai_move(edict_t *self, float dist)
{

}

/*
 *
 * Used for standing around and looking
 * for players Distance is for slight
 * position adjustments needed by the
 * animations
 */
void
ai_stand(edict_t *self, float dist)
{
	
}

/*
 * The monster is walking it's beat
 */
void
ai_walk(edict_t *self, float dist)
{

}

/*
 * Turns towards target and advances
 * Use this call with a distance of 0
 * to replace ai_face
 */
void
ai_charge(edict_t *self, float dist)
{

}

/*
 * Don't move, but turn towards
 * ideal_yaw. Distance is for
 * slight position adjustments
 * needed by the animations
 */
void
ai_turn(edict_t *self, float dist)
{

}

/* ============================================================================ */

/*
 * .enemy
 * Will be world if not currently angry at anyone.
 *
 * .movetarget
 * The next path spot to walk toward.  If .enemy, ignore .movetarget.
 * When an enemy is killed, the monster will try to return to it's path.
 *
 * .hunt_time
 * Set to time + something when the player is in sight, but movement straight for
 * him is blocked.  This causes the monster to use wall following code for
 * movement direction instead of sighting on the player.
 *
 * .ideal_yaw
 * A yaw angle of the intended direction, which will be turned towards at up
 * to 45 deg / state.  If the enemy is in view and hunt_time is not active,
 * this will be the exact line towards the enemy.
 *
 * .pausetime
 * A monster will leave it's stand state and head towards it's .movetarget when
 * time > .pausetime.
 */

/* ============================================================================ */

/*
 * returns the range categorization of an entity relative to self
 * 0	melee range, will become hostile even if back is turned
 * 1	visibility and infront, or visibility and show hostile
 * 2	infront and show hostile
 * 3	only triggered by damage
 */
int
range(edict_t *self, edict_t *other)
{
	vec3_t v;
	float len;

 	if (!self || !other)
	{
		return 0;
	}

	VectorSubtract(self->s.origin, other->s.origin, v);
	len = VectorLength(v);

	if (len < MELEE_DISTANCE)
	{
		return RANGE_MELEE;
	}

	if (len < 500)
	{
		return RANGE_NEAR;
	}

	if (len < 1000)
	{
		return RANGE_MID;
	}

	return RANGE_FAR;
}

/*
 * returns 1 if the entity is visible
 * to self, even if not infront
 */
qboolean
visible(edict_t *self, edict_t *other)
{
	vec3_t spot1;
	vec3_t spot2;
	trace_t trace;

	if (!self || !other)
	{
		return false;
	}

	VectorCopy(self->s.origin, spot1);
	spot1[2] += self->viewheight;
	VectorCopy(other->s.origin, spot2);
	spot2[2] += other->viewheight;
	trace = gi.trace(spot1, vec3_origin, vec3_origin, spot2, self, MASK_OPAQUE);

	if (trace.fraction == 1.0)
	{
		return true;
	}

	return false;
}

/*
 * returns 1 if the entity is in
 * front (in sight) of self
 */
qboolean
infront(edict_t *self, edict_t *other)
{
	vec3_t vec;
	float dot;
	vec3_t forward;

	if (!self || !other)
	{
		return false;
	}

	AngleVectors(self->s.angles, forward, NULL, NULL);

	VectorSubtract(other->s.origin, self->s.origin, vec);
	VectorNormalize(vec);
	dot = DotProduct(vec, forward);

	if (dot > 0.3)
	{
		return true;
	}

	return false;
}


/* ============================================================================ */

void
HuntTarget(edict_t *self)
{

}

void
FoundTarget(edict_t *self)
{

}

/*
 * Self is currently not attacking anything,
 * so try to find a target
 *
 * Returns TRUE if an enemy was sighted
 *
 * When a player fires a missile, the point
 * of impact becomes a fakeplayer so that
 * monsters that see the impact will respond
 * as if they had seen the player.
 *
 * To avoid spending too much time, only
 * a single client (or fakeclient) is
 * checked each frame. This means multi
 * player games will have slightly
 * slower noticing monsters.
 */
qboolean
FindTarget(edict_t *self)
{

}

/* ============================================================================= */

qboolean
FacingIdeal(edict_t *self)
{
	float delta;

	if (!self)
	{
		return false;
	}

	delta = anglemod(self->s.angles[YAW] - self->ideal_yaw);

	if ((delta > 45) && (delta < 315))
	{
		return false;
	}

	return true;
}

/* ============================================================================= */

qboolean
M_CheckAttack(edict_t *self)
{
	vec3_t spot1, spot2;
	float chance;
	trace_t tr;

	if (!self || !self->enemy || !self->enemy->inuse)
	{
		return false;
	}

	if (self->enemy->health > 0)
	{
		/* see if any entities are in the way of the shot */
		VectorCopy(self->s.origin, spot1);
		spot1[2] += self->viewheight;
		VectorCopy(self->enemy->s.origin, spot2);
		spot2[2] += self->enemy->viewheight;

		tr = gi.trace(spot1, NULL, NULL, spot2, self,
				CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_SLIME |
				CONTENTS_LAVA | CONTENTS_WINDOW);

		/* do we have a clear shot? */
		if (tr.ent != self->enemy)
		{
			return false;
		}
	}

	/* melee attack */
	if (enemy_range == RANGE_MELEE)
	{
		/* don't always melee in easy mode */
		if ((skill->value == 0) && (randk() & 3))
		{
			return false;
		}

		if (self->monsterinfo.melee)
		{
			self->monsterinfo.attack_state = AS_MELEE;
		}
		else
		{
			self->monsterinfo.attack_state = AS_MISSILE;
		}

		return true;
	}

	/* missile attack */
	if (!self->monsterinfo.attack)
	{
		return false;
	}

	if (level.time < self->monsterinfo.attack_finished)
	{
		return false;
	}

	if (enemy_range == RANGE_FAR)
	{
		return false;
	}

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		chance = 0.4;
	}
	else if (enemy_range == RANGE_MELEE)
	{
		chance = 0.2;
	}
	else if (enemy_range == RANGE_NEAR)
	{
		chance = 0.1;
	}
	else if (enemy_range == RANGE_MID)
	{
		chance = 0.02;
	}
	else
	{
		return false;
	}

	if (skill->value == 0)
	{
		chance *= 0.5;
	}
	else if (skill->value >= 2)
	{
		chance *= 2;
	}

	if (random() < chance)
	{
		self->monsterinfo.attack_state = AS_MISSILE;
		self->monsterinfo.attack_finished = level.time + 2 * random();
		return true;
	}

	if (self->flags & FL_FLY)
	{
		if (random() < 0.3)
		{
			self->monsterinfo.attack_state = AS_SLIDING;
		}
		else
		{
			self->monsterinfo.attack_state = AS_STRAIGHT;
		}
	}

	return false;
}

/*
 * Turn and close until within an
 * angle to launch a melee attack
 */
void
ai_run_melee(edict_t *self)
{

}

/*
 * Turn in place until within an
 * angle to launch a missile attack
 */
void
ai_run_missile(edict_t *self)
{

}

/*
 * Strafe sideways, but stay at
 * approximately the same range
 */
void
ai_run_slide(edict_t *self, float distance)
{

}

/*
 * Decides if we're going to attack
 * or do something else used by
 * ai_run and ai_stand
 */
qboolean
ai_checkattack(edict_t *self)
{
	vec3_t temp;
	qboolean hesDeadJim;

	if (!self || !self->enemy || !self->enemy->inuse)
	{
		enemy_vis = false;

		return false;
	}

	/* this causes monsters to run blindly
	   to the combat point w/o firing */
	if (self->goalentity)
	{
		if (self->monsterinfo.aiflags & AI_COMBAT_POINT)
		{
			return false;
		}

		if ((self->monsterinfo.aiflags & AI_SOUND_TARGET) && !visible(self, self->goalentity))
		{
			if ((level.time - self->enemy->last_sound_time) > 5.0)
			{
				if (self->goalentity == self->enemy)
				{
					if (self->movetarget)
					{
						self->goalentity = self->movetarget;
					}
					else
					{
						self->goalentity = NULL;
					}
				}

				self->monsterinfo.aiflags &= ~AI_SOUND_TARGET;

				if (self->monsterinfo.aiflags & AI_TEMP_STAND_GROUND)
				{
					self->monsterinfo.aiflags &=
							~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				}
			}
			else
			{
				self->show_hostile = (int)level.time + 1;
				return false;
			}
		}
	}

	enemy_vis = false;

	/* see if the enemy is dead */
	hesDeadJim = false;

	if ((!self->enemy) || (!self->enemy->inuse))
	{
		hesDeadJim = true;
	}
	else if (self->monsterinfo.aiflags & AI_MEDIC)
	{
		if (self->enemy->health > 0)
		{
			hesDeadJim = true;
			self->monsterinfo.aiflags &= ~AI_MEDIC;
		}
	}
	else
	{
		if (self->monsterinfo.aiflags & AI_BRUTAL)
		{
			if (self->enemy->health <= -80)
			{
				hesDeadJim = true;
			}
		}
		else
		{
			if (self->enemy->health <= 0)
			{
				hesDeadJim = true;
			}
		}
	}

	if (hesDeadJim)
	{
		self->enemy = NULL;

		if (self->oldenemy && (self->oldenemy->health > 0))
		{
			self->enemy = self->oldenemy;
			self->oldenemy = NULL;
			HuntTarget(self);
		}
		else
		{
			if (self->movetarget)
			{
				self->goalentity = self->movetarget;
				self->monsterinfo.walk(self);
			}
			else
			{
				/* we need the pausetime otherwise the stand code
				   will just revert to walking with no target and
				   the monsters will wonder around aimlessly trying
				   to hunt the world entity */
				self->monsterinfo.pausetime = level.time + 100000000;
				self->monsterinfo.stand(self);
			}

			return true;
		}
	}

	/* wake up other monsters */
	self->show_hostile = (int)level.time + 1;

	/* check knowledge of enemy */
	enemy_vis = visible(self, self->enemy);

	if (enemy_vis)
	{
		self->monsterinfo.search_time = level.time + 5;
		VectorCopy(self->enemy->s.origin, self->monsterinfo.last_sighting);
	}

	/* look for other coop players here */
	if (coop->value && (self->monsterinfo.search_time < level.time))
	{
		if (FindTarget(self))
		{
			return true;
		}
	}

	if (self->enemy)
	{
		enemy_infront = infront(self, self->enemy);
		enemy_range = range(self, self->enemy);
		VectorSubtract(self->enemy->s.origin, self->s.origin, temp);
		enemy_yaw = vectoyaw(temp);
	}

	if (self->monsterinfo.attack_state == AS_MISSILE)
	{
		ai_run_missile(self);
		return true;
	}

	if (self->monsterinfo.attack_state == AS_MELEE)
	{
		ai_run_melee(self);
		return true;
	}

	/* if enemy is not currently visible,
	   we will never attack */
	if (!enemy_vis)
	{
		return false;
	}

	return self->monsterinfo.checkattack(self);
}

/*
 * The monster has an enemy
 * it is trying to kill
 */
void
ai_run(edict_t *self, float dist)
{

}
