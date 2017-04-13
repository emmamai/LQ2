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
 * Returns true if the inflictor can
 * directly damage the target.  Used for
 * explosions and melee attacks.
 */
qboolean
CanDamage(edict_t *targ, edict_t *inflictor)
{
	vec3_t dest;
	trace_t trace;

	if (!targ || !inflictor)
	{
		return false;
	}

	/* bmodels need special checking because their origin is 0,0,0 */
	if (targ->movetype == MOVETYPE_PUSH)
	{
		VectorAdd(targ->absmin, targ->absmax, dest);
		VectorScale(dest, 0.5, dest);
		trace = gi.trace(inflictor->s.origin, vec3_origin, vec3_origin,
				dest, inflictor, MASK_SOLID);

		if (trace.fraction == 1.0)
		{
			return true;
		}

		if (trace.ent == targ)
		{
			return true;
		}

		return false;
	}

	trace = gi.trace(inflictor->s.origin, vec3_origin, vec3_origin,
			targ->s.origin, inflictor, MASK_SOLID);

	if (trace.fraction == 1.0)
	{
		return true;
	}

	VectorCopy(targ->s.origin, dest);
	dest[0] += 15.0;
	dest[1] += 15.0;
	trace = gi.trace(inflictor->s.origin, vec3_origin, vec3_origin,
			dest, inflictor, MASK_SOLID);

	if (trace.fraction == 1.0)
	{
		return true;
	}

	VectorCopy(targ->s.origin, dest);
	dest[0] += 15.0;
	dest[1] -= 15.0;
	trace = gi.trace(inflictor->s.origin, vec3_origin, vec3_origin,
			dest, inflictor, MASK_SOLID);

	if (trace.fraction == 1.0)
	{
		return true;
	}

	VectorCopy(targ->s.origin, dest);
	dest[0] -= 15.0;
	dest[1] += 15.0;
	trace = gi.trace(inflictor->s.origin, vec3_origin, vec3_origin,
			dest, inflictor, MASK_SOLID);

	if (trace.fraction == 1.0)
	{
		return true;
	}

	VectorCopy(targ->s.origin, dest);
	dest[0] -= 15.0;
	dest[1] -= 15.0;
	trace = gi.trace(inflictor->s.origin, vec3_origin, vec3_origin,
			dest, inflictor, MASK_SOLID);

	if (trace.fraction == 1.0)
	{
		return true;
	}

	return false;
}

void
Killed(edict_t *targ, edict_t *inflictor, edict_t *attacker,
		int damage, vec3_t point)
{
	if (!targ || !inflictor || !attacker)
	{
		return;
	}

	if (targ->health < -999)
	{
		targ->health = -999;
	}

	targ->enemy = attacker;

	if ((targ->svflags & SVF_MONSTER) && (targ->deadflag != DEAD_DEAD))
	{
		if (!(targ->monsterinfo.aiflags & AI_GOOD_GUY))
		{
			level.killed_monsters++;

			if (coop->value && attacker->client)
			{
				attacker->client->resp.score++;
			}

			/* medics won't heal monsters that they kill themselves */
			if (attacker && attacker->classname && strcmp(attacker->classname, "monster_medic") == 0)
			{
				targ->owner = attacker;
			}
		}
	}

	if ((targ->movetype == MOVETYPE_PUSH) ||
		(targ->movetype == MOVETYPE_STOP) ||
		(targ->movetype == MOVETYPE_NONE))
	{
		/* doors, triggers, etc */
		targ->die(targ, inflictor, attacker, damage, point);
		return;
	}

	targ->die(targ, inflictor, attacker, damage, point);
}

void
SpawnDamage(int type, vec3_t origin, vec3_t normal)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(type);
	gi.WritePosition(origin);
	gi.WriteDir(normal);
	gi.multicast(origin, MULTICAST_PVS);
}

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
	int save;
	int asave;
	int psave;
	int te_sparks;

	if (!targ || !inflictor || !attacker)
	{
		return;
	}

	if (!targ->takedamage)
	{
		return;
	}

	/* friendly fire avoidance if enabled you
	   can't hurt teammates (but you can hurt
	   yourself) knockback still occurs */
	if ((targ != attacker) && ((deathmatch->value &&
		  ((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS))) ||
		 coop->value))
	{
		if (OnSameTeam(targ, attacker))
		{
			if ((int)(dmflags->value) & DF_NO_FRIENDLY_FIRE)
			{
				damage = 0;
			}
			else
			{
				mod |= MOD_FRIENDLY_FIRE;
			}
		}
	}

	meansOfDeath = mod;

	/* easy mode takes half damage */
	if ((skill->value == 0) && (deathmatch->value == 0) && targ->client)
	{
		damage *= 0.5;

		if (!damage)
		{
			damage = 1;
		}
	}

	client = targ->client;

	if (dflags & DAMAGE_BULLET)
	{
		te_sparks = TE_BULLET_SPARKS;
	}
	else
	{
		te_sparks = TE_SPARKS;
	}

	VectorNormalize(dir);

	/* bonus damage for suprising a monster */
	if (!(dflags & DAMAGE_RADIUS) && (targ->svflags & SVF_MONSTER) &&
		(attacker->client) && (!targ->enemy) && (targ->health > 0))
	{
		damage *= 2;
	}

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
	save = 0;

	/* check for godmode */
	if ((targ->flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION))
	{
		take = 0;
		save = damage;
		SpawnDamage(te_sparks, point, normal);
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
		save = damage;
	}

	/* treat cheat/powerup savings the same as armor */
	asave += save;

	/* team damage avoidance */
	if (!(dflags & DAMAGE_NO_PROTECTION) && false)
	{
		return;
	}

	/* do the damage */
	if (take)
	{
		if ((targ->svflags & SVF_MONSTER) || (client))
		{
			SpawnDamage(TE_BLOOD, point, normal);
		}
		else
		{
			SpawnDamage(te_sparks, point, normal);
		}

		targ->health = targ->health - take;

		if (targ->health <= 0)
		{
			if ((targ->svflags & SVF_MONSTER) || (client))
			{
				targ->flags |= FL_NO_KNOCKBACK;
			}

			Killed(targ, inflictor, attacker, take, point);
			return;
		}
	}

	if (targ->svflags & SVF_MONSTER)
	{

		if (!(targ->monsterinfo.aiflags & AI_DUCKED) && (take))
		{
			targ->pain(targ, attacker, knockback, take);

			/* nightmare mode monsters don't go into pain frames often */
			if (skill->value == 3)
			{
				targ->pain_debounce_time = level.time + 5;
			}
		}
	}
	else if (client)
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

void
T_RadiusDamage(edict_t *inflictor, edict_t *attacker, float damage,
		edict_t *ignore, float radius, int mod)
{
	float points;
	edict_t *ent = NULL;
	vec3_t v;
	vec3_t dir;

	if (!inflictor || !attacker)
	{
		return;
	}

	while ((ent = findradius(ent, inflictor->s.origin, radius)) != NULL)
	{
		if (ent == ignore)
		{
			continue;
		}

		if (!ent->takedamage)
		{
			continue;
		}

		VectorAdd(ent->mins, ent->maxs, v);
		VectorMA(ent->s.origin, 0.5, v, v);
		VectorSubtract(inflictor->s.origin, v, v);
		points = damage - 0.5 * VectorLength(v);

		if (ent == attacker)
		{
			points = points * 0.5;
		}

		if (points > 0)
		{
			if (CanDamage(ent, inflictor))
			{
				VectorSubtract(ent->s.origin, inflictor->s.origin, dir);
				T_Damage(ent, inflictor, attacker, dir, inflictor->s.origin,
						vec3_origin, (int)points, (int)points, DAMAGE_RADIUS,
						mod);
			}
		}
	}
}
