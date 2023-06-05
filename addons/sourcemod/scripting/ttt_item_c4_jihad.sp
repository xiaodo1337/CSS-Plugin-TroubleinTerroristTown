#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] 清真C4",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}

#define WEAPON_BASE "weapon_hegrenade"
#define WEAPON_SLOT CS_SLOT_GRENADE

#define FIRE_SOUND "weapons/ttt/jihad.wav"

#define V_MODEL "models/weapons/ttt/v_jb.mdl"
#define W_MODEL "models/weapons/ttt/w_jb.mdl"
#define VModelMaterialsFolder "materials/models/weapons/v_models/jb_c4"
#define WModelMaterialsFolder "materials/models/weapons/w_models/jb_c4"
#define ModelsFolder "models/weapons/ttt"
#define SND_EXPLODE "weapons/hegrenade/explode5.wav"

int g_ItemID;
int v_Model;
int w_Model;

enum struct PlayerData
{
    Handle SuicideTimer;
}

PlayerData g_Client[MAXPLAYERS + 1];

public OnPluginStart()
{
    HookEvent("round_start", Event_RoundStart);
    HookEvent("player_death", Event_PlayerDeath, EventHookMode_Pre);
    RegConsoleCmd("drop", HookDropCommand);
}

public Action:Event_PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
    new victim = GetClientOfUserId(GetEventInt(event, "userid"));
    ResetPlayerVars(victim);
}

public Action:Event_RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
    LoopValidClients(client)
    {
        ResetPlayerVars(client)
    }
}

public OnConfigsExecuted()
{
    g_ItemID = TTT_RegisterShopItem("圣战C4", ITEMTYPE_ActiveEquip, "按下丢弃键发动自杀式袭击。", 1, TTT_ROLE_TRAITOR, -1, 1, 0);
}

public OnMapStart()
{
    PrecacheSound(SND_EXPLODE);
    char buffer[64];
    PrecacheSound(FIRE_SOUND);
    Format(buffer, sizeof(buffer), "sound/%s", FIRE_SOUND);
    AddFileToDownloadsTable(buffer);

    TTT_AddFilesToDownloadList(VModelMaterialsFolder);
    TTT_AddFilesToDownloadList(WModelMaterialsFolder);
    TTT_AddFilesToDownloadList(ModelsFolder, "_jb");
    v_Model = PrecacheModel(V_MODEL);
    w_Model = PrecacheModel(W_MODEL);
    TTT_RegisterViewModel(WEAPON_BASE, v_Model);
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
    ResetPlayerVars(client);
    SDKHook(client, SDKHook_WeaponCanSwitchTo, OnWeaponCanSwitchTo);
    SDKHook(client, SDKHook_WeaponSwitchPost, OnWeaponSwitchPost);
    SDKHook(client, SDKHook_WeaponDropPost, OnWeaponDropPost);
}

public OnClientDisconnect(int client)
{
    ResetPlayerVars(client);
    SDKUnhook(client, SDKHook_WeaponCanSwitchTo, OnWeaponCanSwitchTo);
    SDKUnhook(client, SDKHook_WeaponSwitchPost, OnWeaponSwitchPost);
    SDKUnhook(client, SDKHook_WeaponDropPost, OnWeaponDropPost);
}

public Action OnWeaponCanSwitchTo(int client, int weapon)
{
    char aWeapon[32];
    GetClientWeapon(client, aWeapon, sizeof(aWeapon));
    if(strcmp(aWeapon, WEAPON_BASE) == 0 && IsValidHandle(g_Client[client].SuicideTimer))
    {
        return Plugin_Handled;
    }
    return Plugin_Continue;
}

public OnWeaponSwitchPost(int client, int weapon)
{
    char aWeapon[32];
    GetEntityClassname(weapon, aWeapon, sizeof(aWeapon));
    if(strcmp(aWeapon, WEAPON_BASE) == 0)
    {
        //由于客户端预测，模型会闪烁，现已更换方法
        //int vm = GetEntPropEnt(client, Prop_Data, "m_hViewModel");
        //SetEntProp(vm, Prop_Send, "m_nModelIndex", v_AWPModel);
        SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", w_Model);
        TTT_SetViewModelSequence(client, 0);
    }
}

public OnWeaponDropPost(int client, int weapon)
{
    if(IsValidEntity(weapon))
    {
        char aWeapon[32];
        GetEntityClassname(weapon, aWeapon, sizeof(aWeapon));
        if(strcmp(aWeapon, WEAPON_BASE) == 0) RequestFrame(ChangeWepModel, weapon);
    }
}

public ChangeWepModel(int weapon)
{
    SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", w_Model);
}

public Action TTT_OnItemPurchase(int client, int ItemID)
{
    if(ItemID == g_ItemID)
    {
        if(GetPlayerWeaponSlot(client, WEAPON_SLOT) != -1)
        {
            CPrintToChat(client, "%s您已拥有一件主动道具！请使用后再购买！", TTT_TAG);
            return Plugin_Handled;
        }
        else
        {
            GivePlayerItem(client, WEAPON_BASE);
        }
    }
    return Plugin_Continue;
}

public Action HookDropCommand(int client, int args)
{
    if (!IsClientValid(client) || !IsPlayerAlive(client) || IsValidHandle(g_Client[client].SuicideTimer)) return Plugin_Continue;
    decl String:ClassName[32];
    GetClientWeapon(client, ClassName, sizeof(ClassName));
    if(strcmp(ClassName, WEAPON_BASE, false) != 0) return Plugin_Continue;
    SetEntPropFloat(client, Prop_Send, "m_flProgressBarStartTime", GetGameTime());
    SetEntProp(client, Prop_Send, "m_iProgressBarDuration", 3);
    g_Client[client].SuicideTimer = CreateTimer(3.0, MakeExplode, client);
    EmitSoundToAll(FIRE_SOUND, client);
    return Plugin_Continue;
}

public Action OnPlayerRunCmd(int client, int &buttons, int &impulse, float vel[3], float angles[3], int &weapon, int &subtype, int &cmdnum, int &tickcount, int &seed, int mouse[2])
{
    if (!IsClientValid(client) || !IsPlayerAlive(client)) return Plugin_Continue;
    if(buttons & IN_ATTACK)
    {
        decl String:ClassName[32];
        GetClientWeapon(client, ClassName, sizeof(ClassName));
        if(strcmp(ClassName, WEAPON_BASE, false) != 0) return Plugin_Continue;
        buttons &= ~IN_ATTACK;
        return Plugin_Changed;
    }
    return Plugin_Continue;
}

public Action MakeExplode(Handle Timer, int client)
{
    KillTimer(Timer);
    g_Client[client].SuicideTimer = INVALID_HANDLE;
    SetEntProp(client, Prop_Send, "m_iProgressBarDuration",0);
    new Float:Origin[3];
    GetClientAbsOrigin(client, Origin);
    int iWeapon = GetPlayerWeaponSlot(client, CS_SLOT_GRENADE);
    if(IsValidEntity(iWeapon))
    {
        RemovePlayerItem(client, iWeapon);
    }
    new ExplosionIndex = CreateEntityByName("env_explosion");
    if (IsValidEntity(ExplosionIndex))
    {
        DispatchKeyValue(ExplosionIndex, "classname", "hegrenade_projectile");
        DispatchKeyValueVector(ExplosionIndex, "origin", Origin); 
        DispatchKeyValueInt(ExplosionIndex, "spawnflags", 6146);
        SetEntPropEnt(ExplosionIndex, Prop_Send, "m_hOwnerEntity", client);
        DispatchSpawn(ExplosionIndex);
        DispatchKeyValueInt(ExplosionIndex, "iMagnitude", 220);
        DispatchKeyValueInt(ExplosionIndex, "iRadiusOverride", 650);
        ActivateEntity(ExplosionIndex);
        EmitSoundToAll(SND_EXPLODE, ExplosionIndex, 1, 90);
        AcceptEntityInput(ExplosionIndex, "Explode");
        DispatchKeyValue(ExplosionIndex, "classname", "env_explosion");
        AcceptEntityInput(ExplosionIndex, "Kill");
    }
    return Plugin_Continue;
}

public ResetPlayerVars(client)
{
    if(IsValidHandle(g_Client[client].SuicideTimer))
    {
        KillTimer(g_Client[client].SuicideTimer);
        g_Client[client].SuicideTimer = INVALID_HANDLE;
    }
    if(!IsClientValid(client)) return;
    if(GetEntProp(client, Prop_Send, "m_iProgressBarDuration") > 0)
    	SetEntProp(client, Prop_Send, "m_iProgressBarDuration",0);
}