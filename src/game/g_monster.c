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
 * Monster utility functions.
 *
 * =======================================================================
 */

#include "header/local.h"

void monster_start_go(edict_t *self);

/* Monster weapons */

void
monster_fire_bullet(edict_t *self, vec3_t start, vec3_t dir, int damage,
		int kick, int hspread, int vspread, int flashtype)
{
	if (!self)
	{
		return;
	}

	fire_bullet(self, start, dir, damage, kick, hspread, vspread, MOD_UNKNOWN);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(flashtype);
	gi.multicast(start, MULTICAST_PVS);
}

void
monster_fire_shotgun(edict_t *self, vec3_t start, vec3_t aimdir, int damage,
		int kick, int hspread, int vspread, int count, int flashtype)
{
	if (!self)
	{
		return;
	}

	fire_shotgun(self, start, aimdir, damage, kick, hspread,
			vspread, count, MOD_UNKNOWN);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(flashtype);
	gi.multicast(start, MULTICAST_PVS);
}

void
monster_fire_blaster(edict_t *self, vec3_t start, vec3_t dir, int damage,
		int speed, int flashtype, int effect)
{
	if (!self)
	{
		return;
	}

	fire_blaster(self, start, dir, damage, speed, effect, false);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(flashtype);
	gi.multicast(start, MULTICAST_PVS);
}

void
monster_fire_grenade(edict_t *self, vec3_t start, vec3_t aimdir,
		int damage, int speed, int flashtype)
{
	if (!self)
	{
		return;
	}

	fire_grenade(self, start, aimdir, damage, speed, 2.5, damage + 40);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(flashtype);
	gi.multicast(start, MULTICAST_PVS);
}

void
monster_fire_rocket(edict_t *self, vec3_t start, vec3_t dir,
		int damage, int speed, int flashtype)
{
	if (!self)
	{
		return;
	}

	fire_rocket(self, start, dir, damage, speed, damage + 20, damage);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(flashtype);
	gi.multicast(start, MULTICAST_PVS);
}

void
monster_fire_railgun(edict_t *self, vec3_t start, vec3_t aimdir,
		int damage, int kick, int flashtype)
{
	if (!self)
	{
		return;
	}

	fire_rail(self, start, aimdir, damage, kick);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(flashtype);
	gi.multicast(start, MULTICAST_PVS);
}

void
monster_fire_bfg(edict_t *self, vec3_t start, vec3_t aimdir,
		int damage, int speed, int kick /* unused */, float damage_radius,
		int flashtype)
{
	if (!self)
	{
		return;
	}

	fire_bfg(self, start, aimdir, damage, speed, damage_radius);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(flashtype);
	gi.multicast(start, MULTICAST_PVS);
}

/* ================================================================== */

/* Monster utility functions */

void
M_FliesOff(edict_t *self)
{
	if (!self)
	{
		return;
	}

	self->s.effects &= ~EF_FLIES;
	self->s.sound = 0;
}

void
M_FliesOn(edict_t *self)
{
	if (!self)
	{
		return;
	}

	if (self->waterlevel)
	{
		return;
	}

	self->s.effects |= EF_FLIES;
	self->s.sound = gi.soundindex("infantry/inflies1.wav");
	self->think = M_FliesOff;
	self->nextthink = level.time + 60;
}

void
M_FlyCheck(edict_t *self)
{
	if (!self)
	{
		return;
	}

	if (self->waterlevel)
	{
		return;
	}

	if (random() > 0.5)
	{
		return;
	}

	self->think = M_FliesOn;
	self->nextthink = level.time + 5 + 10 * random();
}

void
AttackFinished(edict_t *self, float time)
{
	if (!self)
	{
		return;
	}

	self->monsterinfo.attack_finished = level.time + time;
}

void
M_CheckGround(edict_t *ent)
{
	vec3_t point;
	trace_t trace;

	if (!ent)
	{
		return;
	}

	if (ent->flags & (FL_SWIM | FL_FLY))
	{
		return;
	}

	if (ent->velocity[2] > 100)
	{
		ent->groundentity = NULL;
		return;
	}

	/* if the hull point one-quarter unit down
	   is solid the entity is on ground */
	point[0] = ent->s.origin[0];
	point[1] = ent->s.origin[1];
	point[2] = ent->s.origin[2] - 0.25;

	trace = gi.trace(ent->s.origin, ent->mins, ent->maxs, point,
			ent, MASK_MONSTERSOLID);

	/* check steepness */
	if ((trace.plane.normal[2] < 0.7) && !trace.startsolid)
	{
		ent->groundentity = NULL;
		return;
	}

	if (!trace.startsolid && !trace.allsolid)
	{
		VectorCopy(trace.endpos, ent->s.origin);
		ent->groundentity = trace.ent;
		ent->groundentity_linkcount = trace.ent->linkcount;
		ent->velocity[2] = trace.ent->velocity[2];
	}
}

void
M_CatagorizePosition(edict_t *ent)
{
	vec3_t point;
	int cont;

	if (!ent)
	{
		return;
	}

	/* get waterlevel */
	point[0] = (ent->absmax[0] + ent->absmin[0])/2;
	point[1] = (ent->absmax[1] + ent->absmin[1])/2;
	point[2] = ent->absmin[2] + 2;
	cont = gi.pointcontents(point);

	if (!(cont & MASK_WATER))
	{
		ent->waterlevel = 0;
		ent->watertype = 0;
		return;
	}

	ent->watertype = cont;
	ent->waterlevel = 1;
	point[2] += 26;
	cont = gi.pointcontents(point);

	if (!(cont & MASK_WATER))
	{
		return;
	}

	ent->waterlevel = 2;
	point[2] += 22;
	cont = gi.pointcontents(point);

	if (cont & MASK_WATER)
	{
		ent->waterlevel = 3;
	}
}

void
M_WorldEffects(edict_t *ent)
{
	int dmg;

	if (!ent)
	{
		return;
	}

	if (ent->health > 0)
	{
		if (!(ent->flags & FL_SWIM))
		{
			if (ent->waterlevel < 3)
			{
				ent->air_finished = level.time + 12;
			}
			else if (ent->air_finished < level.time)
			{
				/* drown! */
				if (ent->pain_debounce_time < level.time)
				{
					dmg = 2 + 2 * floor(level.time - ent->air_finished);

					if (dmg > 15)
					{
						dmg = 15;
					}

					T_Damage(ent, world, world, vec3_origin, ent->s.origin,
							vec3_origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					ent->pain_debounce_time = level.time + 1;
				}
			}
		}
		else
		{
			if (ent->waterlevel > 0)
			{
				ent->air_finished = level.time + 9;
			}
			else if (ent->air_finished < level.time)
			{
				/* suffocate! */
				if (ent->pain_debounce_time < level.time)
				{
					dmg = 2 + 2 * floor(level.time - ent->air_finished);

					if (dmg > 15)
					{
						dmg = 15;
					}

					T_Damage(ent, world, world, vec3_origin, ent->s.origin,
							vec3_origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					ent->pain_debounce_time = level.time + 1;
				}
			}
		}
	}

	if (ent->waterlevel == 0)
	{
		if (ent->flags & FL_INWATER)
		{
			gi.sound(ent, CHAN_BODY, gi.soundindex(
							"player/watr_out.wav"), 1, ATTN_NORM, 0);
			ent->flags &= ~FL_INWATER;
		}

		return;
	}

	if ((ent->watertype & CONTENTS_LAVA) && !(ent->flags & FL_IMMUNE_LAVA))
	{
		if (ent->damage_debounce_time < level.time)
		{
			ent->damage_debounce_time = level.time + 0.2;
			T_Damage(ent, world, world, vec3_origin, ent->s.origin,
					vec3_origin, 10 * ent->waterlevel, 0, 0, MOD_LAVA);
		}
	}

	if ((ent->watertype & CONTENTS_SLIME) && !(ent->flags & FL_IMMUNE_SLIME) && !(ent->svflags & SVF_DEADMONSTER))
	{
		if (ent->damage_debounce_time < level.time)
		{
			ent->damage_debounce_time = level.time + 1;
			T_Damage(ent, world, world, vec3_origin, ent->s.origin,
					vec3_origin, 4 * ent->waterlevel, 0, 0, MOD_SLIME);
		}
	}

	if (!(ent->flags & FL_INWATER))
	{
		if (!(ent->svflags & SVF_DEADMONSTER))
		{
			if (ent->watertype & CONTENTS_LAVA)
			{
				if (random() <= 0.5)
				{
					gi.sound(ent, CHAN_BODY, gi.soundindex(
									"player/lava1.wav"), 1, ATTN_NORM, 0);
				}
				else
				{
					gi.sound(ent, CHAN_BODY, gi.soundindex(
									"player/lava2.wav"), 1, ATTN_NORM, 0);
				}
			}
			else if (ent->watertype & CONTENTS_SLIME)
			{
				gi.sound(ent, CHAN_BODY, gi.soundindex(
								"player/watr_in.wav"), 1, ATTN_NORM, 0);
			}
			else if (ent->watertype & CONTENTS_WATER)
			{
				gi.sound(ent, CHAN_BODY, gi.soundindex(
								"player/watr_in.wav"), 1, ATTN_NORM, 0);
			}
		}

		ent->flags |= FL_INWATER;
		ent->damage_debounce_time = 0;
	}
}

void
M_droptofloor(edict_t *ent)
{
	vec3_t end;
	trace_t trace;

	if (!ent)
	{
		return;
	}

	ent->s.origin[2] += 1;
	VectorCopy(ent->s.origin, end);
	end[2] -= 256;

	trace = gi.trace(ent->s.origin, ent->mins, ent->maxs, end,
			ent, MASK_MONSTERSOLID);

	if ((trace.fraction == 1) || trace.allsolid)
	{
		return;
	}

	VectorCopy(trace.endpos, ent->s.origin);

	gi.linkentity(ent);
	M_CheckGround(ent);
	M_CatagorizePosition(ent);
}

void
M_SetEffects(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	ent->s.effects &= ~(EF_COLOR_SHELL | EF_POWERSCREEN);
	ent->s.renderfx &= ~(RF_SHELL_RED | RF_SHELL_GREEN | RF_SHELL_BLUE);

	if (ent->monsterinfo.aiflags & AI_RESURRECTING)
	{
		ent->s.effects |= EF_COLOR_SHELL;
		ent->s.renderfx |= RF_SHELL_RED;
	}

	if (ent->health <= 0)
	{
		return;
	}

	if (ent->powerarmor_time > level.time)
	{
		if (ent->monsterinfo.power_armor_type == POWER_ARMOR_SCREEN)
		{
			ent->s.effects |= EF_POWERSCREEN;
		}
		else if (ent->monsterinfo.power_armor_type == POWER_ARMOR_SHIELD)
		{
			ent->s.effects |= EF_COLOR_SHELL;
			ent->s.renderfx |= RF_SHELL_GREEN;
		}
	}
}

void
M_MoveFrame(edict_t *self)
{
	mmove_t *move;
	int index;

	if (!self)
	{
		return;
	}

	move = self->monsterinfo.currentmove;
	self->nextthink = level.time + FRAMETIME;

	if ((self->monsterinfo.nextframe) &&
		(self->monsterinfo.nextframe >= move->firstframe) &&
		(self->monsterinfo.nextframe <= move->lastframe))
	{
		self->s.frame = self->monsterinfo.nextframe;
		self->monsterinfo.nextframe = 0;
	}
	else
	{
		if (self->s.frame == move->lastframe)
		{
			if (move->endfunc)
			{
				move->endfunc(self);

				/* regrab move, endfunc is very likely to change it */
				move = self->monsterinfo.currentmove;

				/* check for death */
				if (self->svflags & SVF_DEADMONSTER)
				{
					return;
				}
			}
		}

		if ((self->s.frame < move->firstframe) ||
			(self->s.frame > move->lastframe))
		{
			self->monsterinfo.aiflags &= ~AI_HOLD_FRAME;
			self->s.frame = move->firstframe;
		}
		else
		{
			if (!(self->monsterinfo.aiflags & AI_HOLD_FRAME))
			{
				self->s.frame++;

				if (self->s.frame > move->lastframe)
				{
					self->s.frame = move->firstframe;
				}
			}
		}
	}

	index = self->s.frame - move->firstframe;

	if (move->frame[index].aifunc)
	{
		if (!(self->monsterinfo.aiflags & AI_HOLD_FRAME))
		{
			move->frame[index].aifunc(self,
					move->frame[index].dist * self->monsterinfo.scale);
		}
		else
		{
			move->frame[index].aifunc(self, 0);
		}
	}

	if (move->frame[index].thinkfunc)
	{
		move->frame[index].thinkfunc(self);
	}
}

void
monster_think(edict_t *self)
{

}

/*
 * Using a monster makes it angry
 * at the current activator
 */
void
monster_use(edict_t *self, edict_t *other /* unused */, edict_t *activator)
{

}

void
monster_triggered_spawn(edict_t *self)
{

}

void
monster_triggered_spawn_use(edict_t *self, edict_t *other /* unused */, edict_t *activator)
{

}

void
monster_triggered_start(edict_t *self)
{

}

/*
 * When a monster dies, it fires all of its targets
 * with the current enemy as activator.
 */
void
monster_death_use(edict_t *self)
{

}

/* ================================================================== */

qboolean
monster_start(edict_t *self)
{
	return false;
}

void
monster_start_go(edict_t *self)
{
}

void
walkmonster_start_go(edict_t *self)
{

}

void
walkmonster_start(edict_t *self)
{

}

void
flymonster_start_go(edict_t *self)
{
	
}

void
flymonster_start(edict_t *self)
{

}

void
swimmonster_start_go(edict_t *self)
{
	
}

void
swimmonster_start(edict_t *self)
{

}
