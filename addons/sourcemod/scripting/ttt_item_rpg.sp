#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] RPG火箭筒",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}

#define WEAPON_BASE "weapon_famas"
#define WEAPON_SLOT CS_SLOT_PRIMARY
#define FIRE_SOUND "weapons/ttt/rpgfire1.mp3"

#define NadeSpeed 850.0
#define NadeDamage 270.0
#define NadeRadius 450.0

int g_ItemID, v_RPGModel, w_RPGModel, g_RPGMode[MAXPLAYERS + 1] = {0, ...};     //1为激光制导

new Float:SpinVel[3] = {0.0, 0.0, 200.0};
new Float:SmokeOrigin[3] = {-30.0,0.0,0.0};
new Float:SmokeAngle[3] = {0.0,-180.0,0.0};
new Float:MaxWorldLength;
char buffer[64];

public OnConfigsExecuted()
{
    g_ItemID = TTT_RegisterShopItem("RPG 火箭筒", ITEMTYPE_WEAPON, "能造成极高伤害的范围性武器", 1, TTT_ROLE_TRAITOR, -1, 1, 0);
    AddTempEntHook("Shotgun Shot", Hook_ShotgunShot);   //钩住玩家开火事件
}

public OnMapStart()
{
    new Float:WorldMinHull[3], Float:WorldMaxHull[3];
    GetEntPropVector(0, Prop_Send, "m_WorldMins", WorldMinHull);
    GetEntPropVector(0, Prop_Send, "m_WorldMaxs", WorldMaxHull);
    MaxWorldLength = GetVectorDistance(WorldMinHull, WorldMaxHull);
    PrecacheSound(FIRE_SOUND);
    Format(buffer, sizeof(buffer), "sound/%s", FIRE_SOUND);
    AddFileToDownloadsTable(buffer);
    PrecacheSound("weapons/rpg/rocket1.wav");
    PrecacheSound("weapons/hegrenade/explode5.wav");
    PrecacheModel("models/weapons/w_missile_closed.mdl");
    v_RPGModel = PrecacheModel("models/weapons/v_rpg.mdl");
    w_RPGModel = PrecacheModel("models/weapons/w_rocket_launcher.mdl");
    TTT_RegisterViewModel(WEAPON_BASE, v_RPGModel);

    LoopValidClients(client)
    {
        OnClientPutInServer(client);
    }
}

public OnPluginEnd()
{
    if(g_ItemID) TTT_RemoveShopItem(g_ItemID);
}

public OnClientPutInServer(int client)
{
    SDKHook(client, SDKHook_WeaponSwitchPost, OnWeaponSwitchPost);
    SDKHook(client, SDKHook_WeaponDropPost, OnWeaponDropPost);
}

public OnClientDisconnect(int client)
{
    SDKUnhook(client, SDKHook_WeaponSwitchPost, OnWeaponSwitchPost);
    SDKUnhook(client, SDKHook_WeaponDropPost, OnWeaponDropPost);
}

public OnWeaponSwitchPost(int client, int weapon)
{
    if(IsValidEntity(weapon))
    {
        char aWeapon[32];
        GetEntityClassname(weapon, aWeapon, sizeof(aWeapon));
        if(StrEqual(aWeapon, WEAPON_BASE))
        {
            //int vm = GetEntPropEnt(client, Prop_Data, "m_hViewModel");
            //SetEntProp(vm, Prop_Send, "m_nModelIndex", v_RPGModel);
            SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", w_RPGModel);
            TTT_SetPlayerAmmo(client, weapon, 0);
        }
    }
}

public OnWeaponDropPost(int client, int weapon)
{
    if(IsValidEntity(weapon))
    {
        char aWeapon[32];
        GetEntityClassname(weapon, aWeapon, sizeof(aWeapon));
        if(StrEqual(aWeapon, WEAPON_BASE)) RequestFrame(ChangeWepModel, weapon);
    }
}

public ChangeWepModel(int weapon)
{
    SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", w_RPGModel);
}

public Action TTT_OnItemPurchase(int client, int ItemID)
{
    if(ItemID == g_ItemID)
    {
        int before_weapon = GetPlayerWeaponSlot(client, WEAPON_SLOT);
        if(before_weapon != -1)     //如果主武器槽位有武器
        {
            char classname[32];
            GetEntityClassname(before_weapon, classname, sizeof(classname));
            if (StrEqual(classname, WEAPON_BASE))
            {
                CPrintToChat(client, "%s您已拥有该武器！请丢掉后再购买！", TTT_TAG);
                return Plugin_Handled;
            }
            else
            {
                CS_DropWeapon(client, before_weapon, false);
            }
        }
        int wepent = GivePlayerItem(client, WEAPON_BASE);
        TTT_SetWeaponClip(wepent, 1);
        TTT_SetPlayerAmmo(client, wepent, 0);
    }
    return Plugin_Continue;
}

public Action TTT_OnTraceAttack(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &ammotype, int hitbox, int hitgroup)
{
    if(IsClientValid(attacker) && IsPlayerAlive(attacker))
    {
        int iWeapon = GetEntPropEnt(attacker, Prop_Send, "m_hActiveWeapon");
        if(IsValidEntity(iWeapon))
        {
            char classname[32];
            GetEntityClassname(iWeapon, classname, sizeof(classname));
            if(StrEqual(classname, WEAPON_BASE)) return Plugin_Handled;
        }
    }
    return Plugin_Continue;
}

public Action Hook_ShotgunShot(const char[] sample, const int[] Players, int numClients, float delay)
{
    int client = TE_ReadNum("m_iPlayer") + 1;
    if(!IsClientValid(client) || !IsPlayerAlive(client))
    {
        return Plugin_Continue;
    }
    char classname[32];
    int iWeapon = GetEntPropEnt(client, Prop_Data, "m_hActiveWeapon");
    GetEntityClassname(iWeapon, classname, sizeof(classname));
    if (StrEqual(classname, WEAPON_BASE))
    {
        CreateMissile(client);
        EmitSoundToAll(FIRE_SOUND, client, SNDCHAN_AUTO);
        return Plugin_Stop;
    }
    return Plugin_Continue;
}

public CreateMissile(int client)
{
    new entity = CreateEntityByName("hegrenade_projectile");
    if(IsValidEntity(entity))
    {
        Format(buffer, sizeof(buffer), "Missile_%d", entity);
        DispatchKeyValue(entity, "Name", buffer);
        DispatchSpawn(entity)
        SetEntPropEnt(entity, Prop_Send, "m_hOwnerEntity", client);
        SetEntPropEnt(entity, Prop_Send, "m_hThrower", client);
        float Pos[3], Viewang[3], Forward[3];
        GetClientEyePosition(client, Pos);
        GetClientEyeAngles(client, Viewang);
        GetAngleVectors(Viewang, Forward, NULL_VECTOR, NULL_VECTOR);
        Pos[0] += Forward[0] * 50.0;
        Pos[1] += Forward[1] * 50.0;
        Pos[2] += Forward[2] * 50.0;
        TeleportEntity(entity, Pos, Viewang, NULL_VECTOR);
        
        HookSingleEntityOutput(entity, "OnUser2", InitMissile, true);
        new String:OutputString[50] = "OnUser1 !self:FireUser2::0.0:1";
        SetVariantString(OutputString);
        AcceptEntityInput(entity, "AddOutput");
        AcceptEntityInput(entity, "FireUser1");
    }
}

public InitMissile(const char[] output, int caller, int activator, float delay)
{
    new owner = GetEntPropEnt(caller, Prop_Send, "m_hThrower");
    if (owner == -1 || !IsClientValid(owner) || !IsPlayerAlive(owner))
    {
    	return;
    }
    SetEntityMoveType(caller, MOVETYPE_FLY);
    SetEntityModel(caller, "models/weapons/w_missile_closed.mdl");
    SetEntPropVector(caller, Prop_Data, "m_vecAngVelocity", SpinVel);
    SetEntPropFloat(caller, Prop_Send, "m_flElasticity", 0.0);
    SetEntPropVector(caller, Prop_Send, "m_vecMins", {-2.5, -2.5, -2.5});
    SetEntPropVector(caller, Prop_Send, "m_vecMaxs", {2.5, 2.5, 2.5});
    SetEntityRenderColor(caller, 255, 255, 255, 255);

    new SmokeIndex = CreateEntityByName("env_rockettrail");
    if (IsValidEntity(SmokeIndex))
    {
    	SetEntPropVector(SmokeIndex, Prop_Send, "m_StartColor", {0.65, 0.65, 0.65});
    	SetEntPropFloat(SmokeIndex, Prop_Send, "m_Opacity", 0.2);
    	SetEntPropFloat(SmokeIndex, Prop_Send, "m_SpawnRate", 100.0);
    	SetEntPropFloat(SmokeIndex, Prop_Send, "m_ParticleLifetime", 0.5);
    	SetEntPropFloat(SmokeIndex, Prop_Send, "m_StartSize", 5.0);
    	SetEntPropFloat(SmokeIndex, Prop_Send, "m_EndSize", 30.0);
    	SetEntPropFloat(SmokeIndex, Prop_Send, "m_SpawnRadius", 0.0);
    	SetEntPropFloat(SmokeIndex, Prop_Send, "m_MinSpeed", 0.0);
    	SetEntPropFloat(SmokeIndex, Prop_Send, "m_MaxSpeed", 10.0);
    	SetEntPropFloat(SmokeIndex, Prop_Send, "m_flFlareScale", 1.0);
    	
    	DispatchSpawn(SmokeIndex);
    	ActivateEntity(SmokeIndex);
    	
    	new String:NadeName[20];
    	Format(NadeName, sizeof(NadeName), "Missile_%i", caller);
    	DispatchKeyValue(caller, "targetname", NadeName);
    	SetVariantString(NadeName);
    	AcceptEntityInput(SmokeIndex, "SetParent");
    	TeleportEntity(SmokeIndex, SmokeOrigin, SmokeAngle, NULL_VECTOR);
    }
        
    new Float:NadePos[3];
    GetEntPropVector(caller, Prop_Send, "m_vecOrigin", NadePos);
    new Float:OwnerAng[3];
    GetClientEyeAngles(owner, OwnerAng);
    new Float:OwnerPos[3];
    GetClientEyePosition(owner, OwnerPos);
    TR_TraceRayFilter(OwnerPos, OwnerAng, MASK_SOLID, RayType_Infinite, DontHitOwnerOrNade, caller);
    new Float:InitialPos[3];
    TR_GetEndPosition(InitialPos);
    new Float:InitialVec[3];
    MakeVectorFromPoints(NadePos, InitialPos, InitialVec);
    NormalizeVector(InitialVec, InitialVec);
    ScaleVector(InitialVec, NadeSpeed);
    new Float:InitialAng[3];
    GetVectorAngles(InitialVec, InitialAng);
    TeleportEntity(caller, NULL_VECTOR, InitialAng, InitialVec);
        
    EmitSoundToAll("weapons/rpg/rocket1.wav", caller, 1, 90);
    HookSingleEntityOutput(caller, "OnUser2", MissileThink);
    new String:OutputString[50] = "OnUser1 !self:FireUser2::0.1:-1";
    SetVariantString(OutputString);
    AcceptEntityInput(caller, "AddOutput");
    AcceptEntityInput(caller, "FireUser1");
    SDKHook(caller, SDKHook_StartTouch, OnStartTouch);
}

public Action:MissileThink(const String:output[], caller, activator, Float:delay)
{
    // detonate any missiles that stopped for any reason but didn't detonate.
    decl Float:CheckVec[3];
    GetEntPropVector(caller, Prop_Send, "m_vecVelocity", CheckVec);
    if ((CheckVec[0] == 0.0) && (CheckVec[1] == 0.0) && (CheckVec[2] == 0.0))
    {
    	StopSound(caller, 1, "weapons/rpg/rocket1.wav");
    	CreateExplosion(caller);
    	return;
    }
    decl Float:NadePos[3];
    GetEntPropVector(caller, Prop_Send, "m_vecOrigin", NadePos);
    new Owner = GetEntPropEnt(caller, Prop_Send, "m_hThrower");
    if (g_RPGMode[Owner] > 0)
    {
    	new Float:ClosestDistance = MaxWorldLength;
    	decl Float:TargetVec[3];
    	
    	// make the missile go towards the coordinates the player is looking at.
    	if (g_RPGMode[Owner] == 1)
    	{
    		decl Float:OwnerAng[3];
    		GetClientEyeAngles(Owner, OwnerAng);
    		decl Float:OwnerPos[3];
    		GetClientEyePosition(Owner, OwnerPos);
    		TR_TraceRayFilter(OwnerPos, OwnerAng, MASK_SOLID, RayType_Infinite, DontHitOwnerOrNade, caller);
    		decl Float:TargetPos[3];
    		TR_GetEndPosition(TargetPos);
    		ClosestDistance = GetVectorDistance(NadePos, TargetPos);
    		MakeVectorFromPoints(NadePos, TargetPos, TargetVec);
    	}
    	
    	decl Float:CurrentVec[3];
    	GetEntPropVector(caller, Prop_Send, "m_vecVelocity", CurrentVec);
    	decl Float:FinalVec[3];
    	if (ClosestDistance > 100.0)
    	{
    		NormalizeVector(TargetVec, TargetVec);
    		NormalizeVector(CurrentVec, CurrentVec);
    		ScaleVector(TargetVec, NadeSpeed / 1000.0);
    		AddVectors(TargetVec, CurrentVec, FinalVec);
    	}
    	// ignore turning arc if the missile is close to the enemy to avoid it circling them.
    	else
    	{
    		FinalVec = TargetVec;
    	}
    	
    	NormalizeVector(FinalVec, FinalVec);
    	ScaleVector(FinalVec, NadeSpeed);
    	decl Float:FinalAng[3];
    	GetVectorAngles(FinalVec, FinalAng);
    	TeleportEntity(caller, NULL_VECTOR, FinalAng, FinalVec);
    }
        
    AcceptEntityInput(caller, "FireUser1");
}

public bool:DontHitOwnerOrNade(entity, contentsMask, any:data)
{
	new Owner = GetEntPropEnt(data, Prop_Send, "m_hThrower");
	return ((entity != data) && (entity != Owner));
}

public Action:OnStartTouch(entity, other) 
{
	if (other == 0)
	{
		StopSound(entity, 1, "weapons/rpg/rocket1.wav");
		CreateExplosion(entity);
	}
	// detonate if the missile hits something solid.
	else if((GetEntProp(other, Prop_Data, "m_nSolidType") != _:SOLID_NONE) && (!(GetEntProp(other, Prop_Data, "m_usSolidFlags") & 0x0004)))
	{
		StopSound(entity, 1, "weapons/rpg/rocket1.wav");
		CreateExplosion(entity);
	}
	return Plugin_Continue;
}

CreateExplosion(entity)
{
    UnhookSingleEntityOutput(entity, "OnUser2", MissileThink);
    new Float:MissilePos[3];
    GetEntPropVector(entity, Prop_Send, "m_vecOrigin", MissilePos);
    new MissileOwner = GetEntPropEnt(entity, Prop_Send, "m_hThrower");
    new MissileOwnerTeam = GetEntProp(entity, Prop_Send, "m_iTeamNum");
    new ExplosionIndex = CreateEntityByName("env_explosion");
    if (IsValidEntity(ExplosionIndex))
    {
        DispatchKeyValue(ExplosionIndex, "classname", "hegrenade_projectile");
        DispatchKeyValueVector(ExplosionIndex, "origin", MissilePos); 
        DispatchKeyValueInt(ExplosionIndex, "spawnflags", 6146);
        DispatchKeyValueFloat(ExplosionIndex, "iMagnitude", NadeDamage);
        DispatchKeyValueFloat(ExplosionIndex, "iRadiusOverride", NadeRadius);
        
        SetEntPropEnt(ExplosionIndex, Prop_Send, "m_hOwnerEntity", MissileOwner);
        SetEntProp(ExplosionIndex, Prop_Send, "m_iTeamNum", MissileOwnerTeam);
        
        DispatchSpawn(ExplosionIndex);
        ActivateEntity(ExplosionIndex);
        
        EmitSoundToAll("weapons/hegrenade/explode5.wav", ExplosionIndex, 1, 90);
        AcceptEntityInput(ExplosionIndex, "Explode");
        DispatchKeyValue(ExplosionIndex, "classname", "env_explosion");
        AcceptEntityInput(ExplosionIndex, "Kill");
    }
    AcceptEntityInput(entity, "Kill");
}

public OnPlayerRunCmdPost(int client, int buttons, int impulse, const float vel[3], const float angles[3], int weapon, int subtype, int cmdnum, int tickcount, int seed, const int mouse[2])
{
    if (!IsClientValid(client)) return;
    GetClientWeapon(client, buffer, sizeof(buffer));
    if(strcmp(buffer, WEAPON_BASE, false) == 0)
    {
        if (buttons & IN_ATTACK2 && !(GetClientButtons(client) & IN_ATTACK2))
        {
            g_RPGMode[client] = g_RPGMode[client] == 0 ? 1 : 0;
            PrintHintText(client, "已切换为%s模式", g_RPGMode[client] == 0 ? "普通" : "激光制导");
        }
    }
}