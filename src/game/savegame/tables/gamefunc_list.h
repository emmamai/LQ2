/*
 * Copyright (C) 1997-2001 Id Software, Inc.
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
 * Functionpointers to every function in the game.so.
 *
 * =======================================================================
 */

{"ReadLevel", (byte *)ReadLevel},
{"ReadLevelLocals", (byte *)ReadLevelLocals},
{"ReadEdict", (byte *)ReadEdict},
{"WriteLevel", (byte *)WriteLevel},
{"WriteLevelLocals", (byte *)WriteLevelLocals},
{"WriteEdict", (byte *)WriteEdict},
{"ReadGame", (byte *)ReadGame},
{"WriteGame", (byte *)WriteGame},
{"ReadClient", (byte *)ReadClient},
{"WriteClient", (byte *)WriteClient},
{"ReadField", (byte *)ReadField},
{"WriteField2", (byte *)WriteField2},
{"WriteField1", (byte *)WriteField1},
{"FindMmoveByName", (byte *)FindMmoveByName},
{"GetMmoveByAddress", (byte *)GetMmoveByAddress},
{"FindFunctionByName", (byte *)FindFunctionByName},
{"GetFunctionByAddress", (byte *)GetFunctionByAddress},
{"InitGame", (byte *)InitGame},
{"Info_SetValueForKey", (byte *)Info_SetValueForKey},
{"Info_Validate", (byte *)Info_Validate},
{"Info_RemoveKey", (byte *)Info_RemoveKey},
{"Info_ValueForKey", (byte *)Info_ValueForKey},
{"Q_strlcat", (byte *)Q_strlcat},
{"Q_strlcpy", (byte *)Q_strlcpy},
{"Q_strlwr", (byte *)Q_strlwr},
{"Com_sprintf", (byte *)Com_sprintf},
{"Q_strcasecmp", (byte *)Q_strcasecmp},
{"Q_strncasecmp", (byte *)Q_strncasecmp},
{"Q_stricmp", (byte *)Q_stricmp},
{"Com_PageInMemory", (byte *)Com_PageInMemory},
{"COM_Parse", (byte *)COM_Parse},
{"va", (byte *)va},
{"Swap_Init", (byte *)Swap_Init},
{"FloatNoSwap", (byte *)FloatNoSwap},
{"FloatSwap", (byte *)FloatSwap},
{"LongNoSwap", (byte *)LongNoSwap},
{"LongSwap", (byte *)LongSwap},
{"ShortNoSwap", (byte *)ShortNoSwap},
{"ShortSwap", (byte *)ShortSwap},
{"LittleFloat", (byte *)LittleFloat},
{"BigFloat", (byte *)BigFloat},
{"LittleLong", (byte *)LittleLong},
{"BigLong", (byte *)BigLong},
{"LittleShort", (byte *)LittleShort},
{"BigShort", (byte *)BigShort},
{"COM_DefaultExtension", (byte *)COM_DefaultExtension},
{"COM_FilePath", (byte *)COM_FilePath},
{"COM_FileBase", (byte *)COM_FileBase},
{"COM_FileExtension", (byte *)COM_FileExtension},
{"COM_StripExtension", (byte *)COM_StripExtension},
{"COM_SkipPath", (byte *)COM_SkipPath},
{"Q_log2", (byte *)Q_log2},
{"VectorScale", (byte *)VectorScale},
{"VectorInverse", (byte *)VectorInverse},
{"VectorLength", (byte *)VectorLength},
{"CrossProduct", (byte *)CrossProduct},
{"_VectorCopy", (byte *)_VectorCopy},
{"_VectorAdd", (byte *)_VectorAdd},
{"_VectorSubtract", (byte *)_VectorSubtract},
{"_DotProduct", (byte *)_DotProduct},
{"VectorMA", (byte *)VectorMA},
{"VectorNormalize2", (byte *)VectorNormalize2},
{"VectorNormalize", (byte *)VectorNormalize},
{"VectorCompare", (byte *)VectorCompare},
{"AddPointToBounds", (byte *)AddPointToBounds},
{"ClearBounds", (byte *)ClearBounds},
{"BoxOnPlaneSide2", (byte *)BoxOnPlaneSide2},
{"anglemod", (byte *)anglemod},
{"LerpAngle", (byte *)LerpAngle},
{"Q_fabs", (byte *)Q_fabs},
{"R_ConcatTransforms", (byte *)R_ConcatTransforms},
{"R_ConcatRotations", (byte *)R_ConcatRotations},
{"PerpendicularVector", (byte *)PerpendicularVector},
{"ProjectPointOnPlane", (byte *)ProjectPointOnPlane},
{"AngleVectors2", (byte *)AngleVectors2},
{"AngleVectors", (byte *)AngleVectors},
{"RotatePointAroundVector", (byte *)RotatePointAroundVector},
{"Weapon_BFG", (byte *)Weapon_BFG},
{"weapon_bfg_fire", (byte *)weapon_bfg_fire},
{"Weapon_Railgun", (byte *)Weapon_Railgun},
{"weapon_railgun_fire", (byte *)weapon_railgun_fire},
{"Weapon_SuperShotgun", (byte *)Weapon_SuperShotgun},
{"weapon_supershotgun_fire", (byte *)weapon_supershotgun_fire},
{"Weapon_Shotgun", (byte *)Weapon_Shotgun},
{"weapon_shotgun_fire", (byte *)weapon_shotgun_fire},
{"Weapon_Chaingun", (byte *)Weapon_Chaingun},
{"Chaingun_Fire", (byte *)Chaingun_Fire},
{"Weapon_Machinegun", (byte *)Weapon_Machinegun},
{"Machinegun_Fire", (byte *)Machinegun_Fire},
{"Weapon_HyperBlaster", (byte *)Weapon_HyperBlaster},
{"Weapon_HyperBlaster_Fire", (byte *)Weapon_HyperBlaster_Fire},
{"Weapon_Blaster", (byte *)Weapon_Blaster},
{"Weapon_Blaster_Fire", (byte *)Weapon_Blaster_Fire},
{"Blaster_Fire", (byte *)Blaster_Fire},
{"Weapon_RocketLauncher", (byte *)Weapon_RocketLauncher},
{"Weapon_RocketLauncher_Fire", (byte *)Weapon_RocketLauncher_Fire},
{"Weapon_GrenadeLauncher", (byte *)Weapon_GrenadeLauncher},
{"weapon_grenadelauncher_fire", (byte *)weapon_grenadelauncher_fire},
{"Weapon_Grenade", (byte *)Weapon_Grenade},
{"weapon_grenade_fire", (byte *)weapon_grenade_fire},
{"Weapon_Generic", (byte *)Weapon_Generic},
{"Drop_Weapon", (byte *)Drop_Weapon},
{"Use_Weapon", (byte *)Use_Weapon},
{"Think_Weapon", (byte *)Think_Weapon},
{"NoAmmoWeaponChange", (byte *)NoAmmoWeaponChange},
{"ChangeWeapon", (byte *)ChangeWeapon},
{"Pickup_Weapon", (byte *)Pickup_Weapon},
{"PlayerNoise", (byte *)PlayerNoise},
{"P_ProjectSource", (byte *)P_ProjectSource},
{"ClientEndServerFrame", (byte *)ClientEndServerFrame},
{"G_SetClientFrame", (byte *)G_SetClientFrame},
{"G_SetClientSound", (byte *)G_SetClientSound},
{"G_SetClientEvent", (byte *)G_SetClientEvent},
{"G_SetClientEffects", (byte *)G_SetClientEffects},
{"P_WorldEffects", (byte *)P_WorldEffects},
{"P_FallingDamage", (byte *)P_FallingDamage},
{"SV_CalcBlend", (byte *)SV_CalcBlend},
{"SV_AddBlend", (byte *)SV_AddBlend},
{"SV_CalcGunOffset", (byte *)SV_CalcGunOffset},
{"SV_CalcViewOffset", (byte *)SV_CalcViewOffset},
{"P_DamageFeedback", (byte *)P_DamageFeedback},
{"SV_CalcRoll", (byte *)SV_CalcRoll},
{"PlayerTrail_LastSpot", (byte *)PlayerTrail_LastSpot},
{"PlayerTrail_PickNext", (byte *)PlayerTrail_PickNext},
{"PlayerTrail_PickFirst", (byte *)PlayerTrail_PickFirst},
{"PlayerTrail_New", (byte *)PlayerTrail_New},
{"PlayerTrail_Add", (byte *)PlayerTrail_Add},
{"PlayerTrail_Init", (byte *)PlayerTrail_Init},
{"G_SetSpectatorStats", (byte *)G_SetSpectatorStats},
{"G_CheckChaseStats", (byte *)G_CheckChaseStats},
{"G_SetStats", (byte *)G_SetStats},
{"InventoryMessage", (byte *)InventoryMessage},
{"HelpComputerMessage", (byte *)HelpComputerMessage},
{"DeathmatchScoreboardMessage", (byte *)DeathmatchScoreboardMessage},
{"BeginIntermission", (byte *)BeginIntermission},
{"MoveClientToIntermission", (byte *)MoveClientToIntermission},
{"ClientBeginServerFrame", (byte *)ClientBeginServerFrame},
{"ClientThink", (byte *)ClientThink},
{"PrintPmove", (byte *)PrintPmove},
{"CheckBlock", (byte *)CheckBlock},
{"PM_trace", (byte *)PM_trace},
{"ClientDisconnect", (byte *)ClientDisconnect},
{"ClientConnect", (byte *)ClientConnect},
{"ClientUserinfoChanged", (byte *)ClientUserinfoChanged},
{"ClientBegin", (byte *)ClientBegin},
{"ClientBeginDeathmatch", (byte *)ClientBeginDeathmatch},
{"PutClientInServer", (byte *)PutClientInServer},
{"spectator_respawn", (byte *)spectator_respawn},
{"respawn", (byte *)respawn},
{"CopyToBodyQue", (byte *)CopyToBodyQue},
{"body_die", (byte *)body_die},
{"InitBodyQue", (byte *)InitBodyQue},
{"SelectSpawnPoint", (byte *)SelectSpawnPoint},
{"SelectCoopSpawnPoint", (byte *)SelectCoopSpawnPoint},
{"SelectDeathmatchSpawnPoint", (byte *)SelectDeathmatchSpawnPoint},
{"SelectFarthestDeathmatchSpawnPoint", (byte *)SelectFarthestDeathmatchSpawnPoint},
{"SelectRandomDeathmatchSpawnPoint", (byte *)SelectRandomDeathmatchSpawnPoint},
{"PlayersRangeFromSpot", (byte *)PlayersRangeFromSpot},
{"FetchClientEntData", (byte *)FetchClientEntData},
{"SaveClientData", (byte *)SaveClientData},
{"InitClientResp", (byte *)InitClientResp},
{"InitClientPersistant", (byte *)InitClientPersistant},
{"player_die", (byte *)player_die},
{"LookAtKiller", (byte *)LookAtKiller},
{"TossClientWeapon", (byte *)TossClientWeapon},
{"ClientObituary", (byte *)ClientObituary},
{"IsNeutral", (byte *)IsNeutral},
{"IsFemale", (byte *)IsFemale},
{"player_pain", (byte *)player_pain},
{"SP_info_player_intermission", (byte *)SP_info_player_intermission},
{"SP_info_player_coop", (byte *)SP_info_player_coop},
{"SP_info_player_deathmatch", (byte *)SP_info_player_deathmatch},
{"SP_info_player_start", (byte *)SP_info_player_start},
{"SP_CreateUnnamedSpawn", (byte *)SP_CreateUnnamedSpawn},
{"SP_CreateCoopSpots", (byte *)SP_CreateCoopSpots},
{"SP_FixCoopSpots", (byte *)SP_FixCoopSpots},
{"fire_bfg", (byte *)fire_bfg},
{"bfg_think", (byte *)bfg_think},
{"bfg_touch", (byte *)bfg_touch},
{"bfg_explode", (byte *)bfg_explode},
{"fire_rail", (byte *)fire_rail},
{"fire_rocket", (byte *)fire_rocket},
{"rocket_touch", (byte *)rocket_touch},
{"fire_grenade2", (byte *)fire_grenade2},
{"fire_grenade", (byte *)fire_grenade},
{"Grenade_Touch", (byte *)Grenade_Touch},
{"Grenade_Explode", (byte *)Grenade_Explode},
{"fire_blaster", (byte *)fire_blaster},
{"blaster_touch", (byte *)blaster_touch},
{"fire_shotgun", (byte *)fire_shotgun},
{"fire_bullet", (byte *)fire_bullet},
{"fire_lead", (byte *)fire_lead},
{"fire_hit", (byte *)fire_hit},
{"check_dodge", (byte *)check_dodge},
{"KillBox", (byte *)KillBox},
{"G_TouchSolids", (byte *)G_TouchSolids},
{"G_TouchTriggers", (byte *)G_TouchTriggers},
{"G_FreeEdict", (byte *)G_FreeEdict},
{"G_Spawn", (byte *)G_Spawn},
{"G_InitEdict", (byte *)G_InitEdict},
{"G_CopyString", (byte *)G_CopyString},
{"vectoangles", (byte *)vectoangles},
{"vectoyaw", (byte *)vectoyaw},
{"G_SetMovedir", (byte *)G_SetMovedir},
{"vtos", (byte *)vtos},
{"tv", (byte *)tv},
{"G_UseTargets", (byte *)G_UseTargets},
{"Think_Delay", (byte *)Think_Delay},
{"G_PickTarget", (byte *)G_PickTarget},
{"findradius", (byte *)findradius},
{"G_Find", (byte *)G_Find},
{"G_ProjectSource", (byte *)G_ProjectSource},
{"SP_trigger_monsterjump", (byte *)SP_trigger_monsterjump},
{"trigger_monsterjump_touch", (byte *)trigger_monsterjump_touch},
{"SP_trigger_gravity", (byte *)SP_trigger_gravity},
{"trigger_gravity_touch", (byte *)trigger_gravity_touch},
{"SP_trigger_hurt", (byte *)SP_trigger_hurt},
{"hurt_use", (byte *)hurt_use},
{"hurt_touch", (byte *)hurt_touch},
{"SP_trigger_push", (byte *)SP_trigger_push},
{"trigger_push_touch", (byte *)trigger_push_touch},
{"SP_trigger_always", (byte *)SP_trigger_always},
{"SP_trigger_counter", (byte *)SP_trigger_counter},
{"trigger_counter_use", (byte *)trigger_counter_use},
{"SP_trigger_key", (byte *)SP_trigger_key},
{"trigger_key_use", (byte *)trigger_key_use},
{"SP_trigger_relay", (byte *)SP_trigger_relay},
{"trigger_relay_use", (byte *)trigger_relay_use},
{"SP_trigger_once", (byte *)SP_trigger_once},
{"SP_trigger_multiple", (byte *)SP_trigger_multiple},
{"trigger_enable", (byte *)trigger_enable},
{"Touch_Multi", (byte *)Touch_Multi},
{"Use_Multi", (byte *)Use_Multi},
{"multi_trigger", (byte *)multi_trigger},
{"multi_wait", (byte *)multi_wait},
{"InitTrigger", (byte *)InitTrigger},
{"SP_target_earthquake", (byte *)SP_target_earthquake},
{"target_earthquake_use", (byte *)target_earthquake_use},
{"target_earthquake_think", (byte *)target_earthquake_think},
{"SP_target_lightramp", (byte *)SP_target_lightramp},
{"target_lightramp_use", (byte *)target_lightramp_use},
{"target_lightramp_think", (byte *)target_lightramp_think},
{"SP_target_laser", (byte *)SP_target_laser},
{"target_laser_start", (byte *)target_laser_start},
{"target_laser_use", (byte *)target_laser_use},
{"target_laser_off", (byte *)target_laser_off},
{"target_laser_on", (byte *)target_laser_on},
{"target_laser_think", (byte *)target_laser_think},
{"SP_target_crosslevel_target", (byte *)SP_target_crosslevel_target},
{"target_crosslevel_target_think", (byte *)target_crosslevel_target_think},
{"SP_target_crosslevel_trigger", (byte *)SP_target_crosslevel_trigger},
{"trigger_crosslevel_trigger_use", (byte *)trigger_crosslevel_trigger_use},
{"SP_target_blaster", (byte *)SP_target_blaster},
{"use_target_blaster", (byte *)use_target_blaster},
{"SP_target_spawner", (byte *)SP_target_spawner},
{"use_target_spawner", (byte *)use_target_spawner},
{"SP_target_splash", (byte *)SP_target_splash},
{"use_target_splash", (byte *)use_target_splash},
{"SP_target_changelevel", (byte *)SP_target_changelevel},
{"use_target_changelevel", (byte *)use_target_changelevel},
{"SP_target_explosion", (byte *)SP_target_explosion},
{"use_target_explosion", (byte *)use_target_explosion},
{"target_explosion_explode", (byte *)target_explosion_explode},
{"SP_target_goal", (byte *)SP_target_goal},
{"use_target_goal", (byte *)use_target_goal},
{"SP_target_secret", (byte *)SP_target_secret},
{"use_target_secret", (byte *)use_target_secret},
{"SP_target_help", (byte *)SP_target_help},
{"Use_Target_Help", (byte *)Use_Target_Help},
{"SP_target_speaker", (byte *)SP_target_speaker},
{"Use_Target_Speaker", (byte *)Use_Target_Speaker},
{"SP_target_temp_entity", (byte *)SP_target_temp_entity},
{"Use_Target_Tent", (byte *)Use_Target_Tent},
{"ServerCommand", (byte *)ServerCommand},
{"SVCmd_WriteIP_f", (byte *)SVCmd_WriteIP_f},
{"SVCmd_ListIP_f", (byte *)SVCmd_ListIP_f},
{"SVCmd_RemoveIP_f", (byte *)SVCmd_RemoveIP_f},
{"SVCmd_AddIP_f", (byte *)SVCmd_AddIP_f},
{"SV_FilterPacket", (byte *)SV_FilterPacket},
{"Svcmd_Test_f", (byte *)Svcmd_Test_f},
{"SP_worldspawn", (byte *)SP_worldspawn},
{"SpawnEntities", (byte *)SpawnEntities},
{"G_FindTeams", (byte *)G_FindTeams},
{"ED_ParseEdict", (byte *)ED_ParseEdict},
{"ED_ParseField", (byte *)ED_ParseField},
{"ED_NewString", (byte *)ED_NewString},
{"ED_CallSpawn", (byte *)ED_CallSpawn},
{"G_RunEntity", (byte *)G_RunEntity},
{"SV_Physics_Step", (byte *)SV_Physics_Step},
{"SV_AddRotationalFriction", (byte *)SV_AddRotationalFriction},
{"SV_Physics_Toss", (byte *)SV_Physics_Toss},
{"SV_Physics_Noclip", (byte *)SV_Physics_Noclip},
{"SV_Physics_None", (byte *)SV_Physics_None},
{"SV_Physics_Pusher", (byte *)SV_Physics_Pusher},
{"SV_Push", (byte *)SV_Push},
{"SV_PushEntity", (byte *)SV_PushEntity},
{"RealBoundingBox", (byte *)RealBoundingBox},
{"SV_AddGravity", (byte *)SV_AddGravity},
{"SV_FlyMove", (byte *)SV_FlyMove},
{"ClipVelocity", (byte *)ClipVelocity},
{"SV_Impact", (byte *)SV_Impact},
{"SV_RunThink", (byte *)SV_RunThink},
{"SV_CheckVelocity", (byte *)SV_CheckVelocity},
{"SV_TestEntityPosition", (byte *)SV_TestEntityPosition},
{"swimmonster_start", (byte *)swimmonster_start},
{"swimmonster_start_go", (byte *)swimmonster_start_go},
{"flymonster_start", (byte *)flymonster_start},
{"flymonster_start_go", (byte *)flymonster_start_go},
{"walkmonster_start", (byte *)walkmonster_start},
{"walkmonster_start_go", (byte *)walkmonster_start_go},
{"monster_start_go", (byte *)monster_start_go},
{"monster_start", (byte *)monster_start},
{"monster_death_use", (byte *)monster_death_use},
{"monster_triggered_start", (byte *)monster_triggered_start},
{"monster_triggered_spawn_use", (byte *)monster_triggered_spawn_use},
{"monster_triggered_spawn", (byte *)monster_triggered_spawn},
{"monster_use", (byte *)monster_use},
{"monster_think", (byte *)monster_think},
{"M_MoveFrame", (byte *)M_MoveFrame},
{"M_SetEffects", (byte *)M_SetEffects},
{"M_droptofloor", (byte *)M_droptofloor},
{"M_WorldEffects", (byte *)M_WorldEffects},
{"M_CatagorizePosition", (byte *)M_CatagorizePosition},
{"M_CheckGround", (byte *)M_CheckGround},
{"AttackFinished", (byte *)AttackFinished},
{"M_FlyCheck", (byte *)M_FlyCheck},
{"monster_fire_bfg", (byte *)monster_fire_bfg},
{"monster_fire_railgun", (byte *)monster_fire_railgun},
{"monster_fire_rocket", (byte *)monster_fire_rocket},
{"monster_fire_grenade", (byte *)monster_fire_grenade},
{"monster_fire_blaster", (byte *)monster_fire_blaster},
{"monster_fire_shotgun", (byte *)monster_fire_shotgun},
{"monster_fire_bullet", (byte *)monster_fire_bullet},
{"SP_misc_teleporter_dest", (byte *)SP_misc_teleporter_dest},
{"SP_misc_teleporter", (byte *)SP_misc_teleporter},
{"teleporter_touch", (byte *)teleporter_touch},
{"SP_func_clock", (byte *)SP_func_clock},
{"func_clock_use", (byte *)func_clock_use},
{"func_clock_think", (byte *)func_clock_think},
{"func_clock_format_countdown", (byte *)func_clock_format_countdown},
{"func_clock_reset", (byte *)func_clock_reset},
{"SP_target_string", (byte *)SP_target_string},
{"target_string_use", (byte *)target_string_use},
{"SP_target_character", (byte *)SP_target_character},
{"SP_misc_gib_head", (byte *)SP_misc_gib_head},
{"SP_misc_gib_leg", (byte *)SP_misc_gib_leg},
{"SP_misc_gib_arm", (byte *)SP_misc_gib_arm},
{"SP_light_mine2", (byte *)SP_light_mine2},
{"SP_light_mine1", (byte *)SP_light_mine1},
{"SP_misc_satellite_dish", (byte *)SP_misc_satellite_dish},
{"misc_satellite_dish_use", (byte *)misc_satellite_dish_use},
{"misc_satellite_dish_think", (byte *)misc_satellite_dish_think},
{"SP_misc_strogg_ship", (byte *)SP_misc_strogg_ship},
{"misc_strogg_ship_use", (byte *)misc_strogg_ship_use},
{"SP_misc_viper_bomb", (byte *)SP_misc_viper_bomb},
{"misc_viper_bomb_use", (byte *)misc_viper_bomb_use},
{"misc_viper_bomb_prethink", (byte *)misc_viper_bomb_prethink},
{"misc_viper_bomb_touch", (byte *)misc_viper_bomb_touch},
{"SP_misc_bigviper", (byte *)SP_misc_bigviper},
{"SP_misc_viper", (byte *)SP_misc_viper},
{"misc_viper_use", (byte *)misc_viper_use},
{"SP_misc_deadsoldier", (byte *)SP_misc_deadsoldier},
{"misc_deadsoldier_die", (byte *)misc_deadsoldier_die},
{"SP_misc_banner", (byte *)SP_misc_banner},
{"misc_banner_think", (byte *)misc_banner_think},
{"SP_monster_commander_body", (byte *)SP_monster_commander_body},
{"commander_body_drop", (byte *)commander_body_drop},
{"commander_body_use", (byte *)commander_body_use},
{"commander_body_think", (byte *)commander_body_think},
{"SP_misc_easterchick2", (byte *)SP_misc_easterchick2},
{"misc_easterchick2_think", (byte *)misc_easterchick2_think},
{"SP_misc_easterchick", (byte *)SP_misc_easterchick},
{"misc_easterchick_think", (byte *)misc_easterchick_think},
{"SP_misc_eastertank", (byte *)SP_misc_eastertank},
{"misc_eastertank_think", (byte *)misc_eastertank_think},
{"SP_misc_blackhole", (byte *)SP_misc_blackhole},
{"misc_blackhole_transparent", (byte *)misc_blackhole_transparent},
{"misc_blackhole_think", (byte *)misc_blackhole_think},
{"misc_blackhole_use", (byte *)misc_blackhole_use},
{"SP_misc_explobox", (byte *)SP_misc_explobox},
{"barrel_delay", (byte *)barrel_delay},
{"barrel_explode", (byte *)barrel_explode},
{"barrel_touch", (byte *)barrel_touch},
{"SP_func_explosive", (byte *)SP_func_explosive},
{"func_explosive_spawn", (byte *)func_explosive_spawn},
{"func_explosive_use", (byte *)func_explosive_use},
{"func_explosive_explode", (byte *)func_explosive_explode},
{"SP_func_object", (byte *)SP_func_object},
{"func_object_use", (byte *)func_object_use},
{"func_object_release", (byte *)func_object_release},
{"func_object_touch", (byte *)func_object_touch},
{"SP_func_wall", (byte *)SP_func_wall},
{"func_wall_use", (byte *)func_wall_use},
{"SP_light", (byte *)SP_light},
{"light_use", (byte *)light_use},
{"SP_info_notnull", (byte *)SP_info_notnull},
{"SP_info_null", (byte *)SP_info_null},
{"SP_viewthing", (byte *)SP_viewthing},
{"TH_viewthing", (byte *)TH_viewthing},
{"SP_point_combat", (byte *)SP_point_combat},
{"point_combat_touch", (byte *)point_combat_touch},
{"SP_path_corner", (byte *)SP_path_corner},
{"path_corner_touch", (byte *)path_corner_touch},
{"BecomeExplosion2", (byte *)BecomeExplosion2},
{"BecomeExplosion1", (byte *)BecomeExplosion1},
{"ThrowDebris", (byte *)ThrowDebris},
{"debris_die", (byte *)debris_die},
{"ThrowClientHead", (byte *)ThrowClientHead},
{"ThrowHead", (byte *)ThrowHead},
{"ThrowGib", (byte *)ThrowGib},
{"gib_die", (byte *)gib_die},
{"gib_touch", (byte *)gib_touch},
{"gib_think", (byte *)gib_think},
{"ClipGibVelocity", (byte *)ClipGibVelocity},
{"VelocityForDamage", (byte *)VelocityForDamage},
{"SP_func_areaportal", (byte *)SP_func_areaportal},
{"Use_Areaportal", (byte *)Use_Areaportal},
{"G_RunFrame", (byte *)G_RunFrame},
{"ExitLevel", (byte *)ExitLevel},
{"CheckDMRules", (byte *)CheckDMRules},
{"CheckNeedPass", (byte *)CheckNeedPass},
{"EndDMLevel", (byte *)EndDMLevel},
{"CreateTargetChangeLevel", (byte *)CreateTargetChangeLevel},
{"ClientEndServerFrames", (byte *)ClientEndServerFrames},
{"Com_Printf", (byte *)Com_Printf},
{"Sys_Error", (byte *)Sys_Error},
{"GetGameAPI", (byte *)GetGameAPI},
{"ShutdownGame", (byte *)ShutdownGame},
{"SetItemNames", (byte *)SetItemNames},
{"InitItems", (byte *)InitItems},
{"SP_item_health_mega", (byte *)SP_item_health_mega},
{"SP_item_health_large", (byte *)SP_item_health_large},
{"SP_item_health_small", (byte *)SP_item_health_small},
{"SP_item_health", (byte *)SP_item_health},
{"SpawnItem", (byte *)SpawnItem},
{"PrecacheItem", (byte *)PrecacheItem},
{"droptofloor", (byte *)droptofloor},
{"Use_Item", (byte *)Use_Item},
{"Drop_Item", (byte *)Drop_Item},
{"drop_make_touchable", (byte *)drop_make_touchable},
{"drop_temp_touch", (byte *)drop_temp_touch},
{"Touch_Item", (byte *)Touch_Item},
{"Drop_PowerArmor", (byte *)Drop_PowerArmor},
{"Pickup_PowerArmor", (byte *)Pickup_PowerArmor},
{"Use_PowerArmor", (byte *)Use_PowerArmor},
{"PowerArmorType", (byte *)PowerArmorType},
{"Pickup_Armor", (byte *)Pickup_Armor},
{"ArmorIndex", (byte *)ArmorIndex},
{"Pickup_Health", (byte *)Pickup_Health},
{"MegaHealth_think", (byte *)MegaHealth_think},
{"Drop_Ammo", (byte *)Drop_Ammo},
{"Pickup_Ammo", (byte *)Pickup_Ammo},
{"Add_Ammo", (byte *)Add_Ammo},
{"Pickup_Key", (byte *)Pickup_Key},
{"Use_Silencer", (byte *)Use_Silencer},
{"Use_Invulnerability", (byte *)Use_Invulnerability},
{"Use_Envirosuit", (byte *)Use_Envirosuit},
{"Use_Breather", (byte *)Use_Breather},
{"Use_Quad", (byte *)Use_Quad},
{"Pickup_Pack", (byte *)Pickup_Pack},
{"Pickup_Bandolier", (byte *)Pickup_Bandolier},
{"Pickup_AncientHead", (byte *)Pickup_AncientHead},
{"Pickup_Adrenaline", (byte *)Pickup_Adrenaline},
{"Drop_General", (byte *)Drop_General},
{"Pickup_Powerup", (byte *)Pickup_Powerup},
{"SetRespawn", (byte *)SetRespawn},
{"DoRespawn", (byte *)DoRespawn},
{"FindItem", (byte *)FindItem},
{"FindItemByClassname", (byte *)FindItemByClassname},
{"GetItemByIndex", (byte *)GetItemByIndex},
{"SP_func_killbox", (byte *)SP_func_killbox},
{"use_killbox", (byte *)use_killbox},
{"SP_func_door_secret", (byte *)SP_func_door_secret},
{"door_secret_die", (byte *)door_secret_die},
{"door_secret_blocked", (byte *)door_secret_blocked},
{"door_secret_done", (byte *)door_secret_done},
{"door_secret_move6", (byte *)door_secret_move6},
{"door_secret_move5", (byte *)door_secret_move5},
{"door_secret_move4", (byte *)door_secret_move4},
{"door_secret_move3", (byte *)door_secret_move3},
{"door_secret_move2", (byte *)door_secret_move2},
{"door_secret_move1", (byte *)door_secret_move1},
{"door_secret_use", (byte *)door_secret_use},
{"SP_func_conveyor", (byte *)SP_func_conveyor},
{"func_conveyor_use", (byte *)func_conveyor_use},
{"SP_func_timer", (byte *)SP_func_timer},
{"func_timer_use", (byte *)func_timer_use},
{"func_timer_think", (byte *)func_timer_think},
{"SP_trigger_elevator", (byte *)SP_trigger_elevator},
{"trigger_elevator_init", (byte *)trigger_elevator_init},
{"trigger_elevator_use", (byte *)trigger_elevator_use},
{"SP_func_train", (byte *)SP_func_train},
{"train_use", (byte *)train_use},
{"func_train_find", (byte *)func_train_find},
{"train_resume", (byte *)train_resume},
{"train_next", (byte *)train_next},
{"train_wait", (byte *)train_wait},
{"train_blocked", (byte *)train_blocked},
{"SP_func_water", (byte *)SP_func_water},
{"SP_func_door_rotating", (byte *)SP_func_door_rotating},
{"SP_func_door", (byte *)SP_func_door},
{"door_touch", (byte *)door_touch},
{"door_killed", (byte *)door_killed},
{"door_blocked", (byte *)door_blocked},
{"Think_SpawnDoorTrigger", (byte *)Think_SpawnDoorTrigger},
{"Think_CalcMoveSpeed", (byte *)Think_CalcMoveSpeed},
{"Touch_DoorTrigger", (byte *)Touch_DoorTrigger},
{"door_use", (byte *)door_use},
{"door_go_up", (byte *)door_go_up},
{"door_go_down", (byte *)door_go_down},
{"door_hit_bottom", (byte *)door_hit_bottom},
{"door_hit_top", (byte *)door_hit_top},
{"door_use_areaportals", (byte *)door_use_areaportals},
{"SP_func_button", (byte *)SP_func_button},
{"button_killed", (byte *)button_killed},
{"button_touch", (byte *)button_touch},
{"button_use", (byte *)button_use},
{"button_fire", (byte *)button_fire},
{"button_wait", (byte *)button_wait},
{"button_return", (byte *)button_return},
{"button_done", (byte *)button_done},
{"SP_func_rotating", (byte *)SP_func_rotating},
{"rotating_use", (byte *)rotating_use},
{"rotating_touch", (byte *)rotating_touch},
{"rotating_blocked", (byte *)rotating_blocked},
{"SP_func_plat", (byte *)SP_func_plat},
{"plat_spawn_inside_trigger", (byte *)plat_spawn_inside_trigger},
{"Touch_Plat_Center", (byte *)Touch_Plat_Center},
{"Use_Plat", (byte *)Use_Plat},
{"plat_blocked", (byte *)plat_blocked},
{"plat_go_up", (byte *)plat_go_up},
{"plat_go_down", (byte *)plat_go_down},
{"plat_hit_bottom", (byte *)plat_hit_bottom},
{"plat_hit_top", (byte *)plat_hit_top},
{"Think_AccelMove", (byte *)Think_AccelMove},
{"plat_Accelerate", (byte *)plat_Accelerate},
{"plat_CalcAcceleratedMove", (byte *)plat_CalcAcceleratedMove},
{"AngleMove_Calc", (byte *)AngleMove_Calc},
{"AngleMove_Begin", (byte *)AngleMove_Begin},
{"AngleMove_Final", (byte *)AngleMove_Final},
{"AngleMove_Done", (byte *)AngleMove_Done},
{"Move_Calc", (byte *)Move_Calc},
{"Move_Begin", (byte *)Move_Begin},
{"Move_Final", (byte *)Move_Final},
{"Move_Done", (byte *)Move_Done},
{"T_RadiusDamage", (byte *)T_RadiusDamage},
{"T_Damage", (byte *)T_Damage},
{"M_ReactToDamage", (byte *)M_ReactToDamage},
{"CheckArmor", (byte *)CheckArmor},
{"CheckPowerArmor", (byte *)CheckPowerArmor},
{"SpawnDamage", (byte *)SpawnDamage},
{"Killed", (byte *)Killed},
{"CanDamage", (byte *)CanDamage},
{"ClientCommand", (byte *)ClientCommand},
{"Cmd_PlayerList_f", (byte *)Cmd_PlayerList_f},
{"Cmd_Say_f", (byte *)Cmd_Say_f},
{"Cmd_Wave_f", (byte *)Cmd_Wave_f},
{"Cmd_Players_f", (byte *)Cmd_Players_f},
{"PlayerSort", (byte *)PlayerSort},
{"Cmd_PutAway_f", (byte *)Cmd_PutAway_f},
{"Cmd_Kill_f", (byte *)Cmd_Kill_f},
{"Cmd_InvDrop_f", (byte *)Cmd_InvDrop_f},
{"Cmd_WeapLast_f", (byte *)Cmd_WeapLast_f},
{"Cmd_WeapNext_f", (byte *)Cmd_WeapNext_f},
{"Cmd_WeapPrev_f", (byte *)Cmd_WeapPrev_f},
{"Cmd_InvUse_f", (byte *)Cmd_InvUse_f},
{"Cmd_Inven_f", (byte *)Cmd_Inven_f},
{"Cmd_Help_f", (byte *)Cmd_Help_f},
{"Cmd_Score_f", (byte *)Cmd_Score_f},
{"Cmd_Drop_f", (byte *)Cmd_Drop_f},
{"Cmd_Use_f", (byte *)Cmd_Use_f},
{"Cmd_Noclip_f", (byte *)Cmd_Noclip_f},
{"Cmd_Notarget_f", (byte *)Cmd_Notarget_f},
{"Cmd_God_f", (byte *)Cmd_God_f},
{"Cmd_Give_f", (byte *)Cmd_Give_f},
{"ValidateSelectedItem", (byte *)ValidateSelectedItem},
{"SelectPrevItem", (byte *)SelectPrevItem},
{"SelectNextItem", (byte *)SelectNextItem},
{"OnSameTeam", (byte *)OnSameTeam},
{"GetChaseTarget", (byte *)GetChaseTarget},
{"ChasePrev", (byte *)ChasePrev},
{"ChaseNext", (byte *)ChaseNext},
{"UpdateChaseCam", (byte *)UpdateChaseCam},
{"visible", (byte *)visible},
{0, 0}
