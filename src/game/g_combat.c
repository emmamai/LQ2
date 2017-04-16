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
 * Combat code like damage, death and so on.
 *
 * =======================================================================
 */

#include "header/local.h"

/*
 * targ			entity that is being damaged
 * inflictor	entity that is causing the damage
 * attacker		entity that caused the inflictor to damage targ
 *      example: targ=monster, inflictor=rocket, attacker=player
 *
 * dir			direction of the attack
 * point		point at which the damage is being inflicted
 * normal		normal vector from that point
 * damage		amount of damage being inflicted
 * knockback	force to be applied against targ as a result of the damage
 *
 * dflags -> these flags are used to control how T_Damage works
 *      DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
 *      DAMAGE_NO_ARMOR			armor does not protect from this damage
 *      DAMAGE_ENERGY			damage is from an energy based weapon
 *      DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
 *      DAMAGE_BULLET			damage is from a bullet (used for ricochets)
 *      DAMAGE_NO_PROTECTION	kills godmode, armor, everything
 */

void
T_Damage(edict_t *targ, edict_t *inflictor, edict_t *attacker,
		vec3_t dir, vec3_t point, vec3_t normal, int damage,
		int knockback, int dflags, int mod)
{
	gclient_t *client;
	int take;

	if (!targ || !inflictor || !attacker)
	{
		return;
	}

	if (!targ->takedamage)
	{
		return;
	}

	meansOfDeath = mod;

	client = targ->client;

	VectorNormalize(dir);

	if (targ->flags & FL_NO_KNOCKBACK)
	{
		knockback = 0;
	}

	/* figure momentum add */
	if (!(dflags & DAMAGE_NO_KNOCKBACK))
	{
		if ((knockback) && (targ->movetype != MOVETYPE_NONE) &&
			(targ->movetype != MOVETYPE_BOUNCE) &&
			(targ->movetype != MOVETYPE_PUSH) &&
			(targ->movetype != MOVETYPE_STOP))
		{
			vec3_t kvel;
			float mass;

			if (targ->mass < 50)
			{
				mass = 50;
			}
			else
			{
				mass = targ->mass;
			}

			if (targ->client && (attacker == targ))
			{
				/* This allows rocket jumps */
				VectorScale(dir, 1600.0 * (float)knockback / mass, kvel);
			}
			else
			{
				VectorScale(dir, 500.0 * (float)knockback / mass, kvel);
			}

			VectorAdd(targ->velocity, kvel, targ->velocity);
		}
	}

	take = damage;

	/* check for godmode */
	if ((targ->flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION))
	{
		take = 0;
	}

	/* check for invincibility */
	if ((client && (client->invincible_framenum > level.framenum)) &&
		!(dflags & DAMAGE_NO_PROTECTION))
	{
		if (targ->pain_debounce_time < level.time)
		{
			gi.sound(targ, CHAN_ITEM, gi.soundindex(
						"items/protect4.wav"), 1, ATTN_NORM, 0);
			targ->pain_debounce_time = level.time + 2;
		}

		take = 0;
	}

	/* do the damage */
	if (take)
	{
		targ->health = targ->health - take;

		if (targ->health <= 0)
		{
			if (client)
			{
				targ->flags |= FL_NO_KNOCKBACK;
			}

			if (targ->health < -999)
			{
				targ->health = -999;
			}

			targ->enemy = attacker;

			targ->die(targ, inflictor, attacker, damage, point);
			return;
		}
	}
	
	if (client)
	{
		if (!(targ->flags & FL_GODMODE) && (take))
		{
			targ->pain(targ, attacker, knockback, take);
		}
	}
	else if (take)
	{
		if (targ->pain)
		{
			targ->pain(targ, attacker, knockback, take);
		}
	}

	/* add to the damage inflicted on a player this frame
	   the total will be turned into screen blends and view
	   angle kicks at the end of the frame */
	if (client)
	{
		client->damage_blood += take;
		client->damage_knockback += knockback;
		VectorCopy(point, client->damage_from);
	}
}