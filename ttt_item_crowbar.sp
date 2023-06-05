#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] 撬棍&秒杀刀",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}

#define V_CROWBARMODEL "models/weapons/ttt/v_crowbar.mdl"
#define W_CROWBARMODEL "models/weapons/ttt/w_crowbar.mdl"
#define HIT_SOUND "ttt/body_impact_hard.wav"
#define HITWALL_SOUND "ttt/crowbar_hitwall.wav"
#define ModelMaterialsFolderCrowbar "materials/models/weapons/v_models/crowbar"
#define ModelsFolder "models/weapons/ttt"

#define V_KNIFEMODEL "models/weapons/ttt/v_dragn_t_new.mdl"
#define W_KNIFEMODEL "models/weapons/ttt/w_dragn_t_new.mdl"
#define ModelMaterialsFolderKnife "materials/models/weapons/v_models/dragonlord"

#define WEAPON_BASE "weapon_knife"

int g_ItemID[4];
int v_CrowbarModel;
int w_CrowbarModel;
int v_KnifeModel;
int w_KnifeModel;

enum struct PlayerData {
    bool HasKnife;
    bool EnhanceCrowbar;
    int buttons;
}

PlayerData g_Client[MAXPLAYERS + 1];

public OnPluginStart()
{
    AddNormalSoundHook(HookKnifeSound);
    HookEvent("round_start", OnRoundStart);
    HookEvent("weapon_fire", OnWeaponFire);
}

public OnConfigsExecuted()
{
    g_ItemID[0] = TTT_RegisterShopItem("强化撬棍", ITEMTYPE_PassiveEquip, "增加撬棍的伤害及击退能力", 1, TTT_ROLE_TRAITOR, -1, 1, 0);
    
    g_ItemID[1] = TTT_RegisterShopItem("匕首", ITEMTYPE_WEAPON, "只能使用一次，但可以秒杀敌人", 1, TTT_ROLE_INNOCENT, -1, -1, 0);
    g_ItemID[2] = TTT_RegisterShopItem("匕首", ITEMTYPE_WEAPON, "只能使用一次，但可以秒杀敌人", 1, TTT_ROLE_DETECTIVE, -1, -1, 0);
    g_ItemID[3] = TTT_RegisterShopItem("匕首", ITEMTYPE_WEAPON, "只能使用一次，但可以秒杀敌人", 1, TTT_ROLE_TRAITOR, -1, -1, 0);
}

public OnMapStart()
{
    char buffer[64];
    Format(buffer, sizeof(buffer), "sound/%s", HIT_SOUND);
    AddFileToDownloadsTable(buffer);
    Format(buffer, sizeof(buffer), "sound/%s", HITWALL_SOUND);
    AddFileToDownloadsTable(buffer);
    TTT_AddFilesToDownloadList(ModelMaterialsFolderCrowbar);
    TTT_AddFilesToDownloadList(ModelsFolder, "crowbar");
    PrecacheSound(HIT_SOUND);
    PrecacheSound(HITWALL_SOUND);
    v_CrowbarModel = PrecacheModel(V_CROWBARMODEL);
    w_CrowbarModel = PrecacheModel(W_CROWBARMODEL);
    TTT_RegisterViewModel(WEAPON_BASE, v_CrowbarModel);

    TTT_AddFilesToDownloadList(ModelMaterialsFolderKnife);
    TTT_AddFilesToDownloadList(ModelsFolder, "dragn_t");
    v_KnifeModel = PrecacheModel(V_KNIFEMODEL);
    w_KnifeModel = PrecacheModel(W_KNIFEMODEL);

    LoopValidClients(client)
    {
        OnClientPutInServer(client);
    }
}

public OnPluginEnd()
{
    for(int i;i < sizeof(g_ItemID);i++)
    {
        if(g_ItemID[i]) TTT_RemoveShopItem(g_ItemID[i]);
    }
}

public Action OnRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    LoopValidClients(client)
    {
        g_Client[client].HasKnife = false;
        g_Client[client].EnhanceCrowbar = false;
    }
    return Plugin_Continue;
}

public OnClientPutInServer(int client)
{
    SDKHook(client, SDKHook_WeaponSwitchPost, OnWeaponSwitchPost);
    g_Client[client].HasKnife = false;
    g_Client[client].EnhanceCrowbar = false;
}

public OnClientDisconnect(int client)
{
    SDKUnhook(client, SDKHook_WeaponSwitchPost, OnWeaponSwitchPost);
    g_Client[client].HasKnife = false;
    g_Client[client].EnhanceCrowbar = false;
}

public Action TTT_OnItemPurchase(int client, int ItemID)
{
    if(ItemID == g_ItemID[0])
    {
        if(g_Client[client].EnhanceCrowbar)
        {
            CPrintToChat(client, "%s您已拥有该武器！请丢掉后再购买！", TTT_TAG);
            return Plugin_Handled;
        }
        else
        {
            g_Client[client].EnhanceCrowbar = true;
            return Plugin_Continue;
        }
    }
    if(ItemID == g_ItemID[1] || ItemID == g_ItemID[2] || ItemID == g_ItemID[3])
    {
        if(g_Client[client].HasKnife)
        {
            CPrintToChat(client, "%s您已拥有该武器！请丢掉后再购买！", TTT_TAG);
            return Plugin_Handled;
        }
        else
        {
            g_Client[client].HasKnife = true;
            OnWeaponSwitchPost(client, GetEntPropEnt(client, Prop_Data, "m_hActiveWeapon"));
            return Plugin_Continue;
        }
    }
    return Plugin_Continue;
}

public Action TTT_OnTraceAttack(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &ammotype, int hitbox, int hitgroup)
{
    if(IsClientValid(attacker) && IsPlayerAlive(attacker) && victim != attacker)
    {
        char aWeapon[32];
        GetClientWeapon(attacker, aWeapon, sizeof(aWeapon));
        if(StrEqual(aWeapon, "weapon_knife") && !(damagetype & DMG_BLAST))
        {
            if(!g_Client[attacker].HasKnife)
            {
                if(g_Client[attacker].buttons & IN_ATTACK)
                {
                    damage = 10.0 / 0.35;
                    if(g_Client[attacker].EnhanceCrowbar) damage = 45.0 / 0.35;
                    return Plugin_Changed;
                }
                else if(g_Client[attacker].buttons & IN_ATTACK2)
                {
                    float Origin[3][3], Velocity[3];
                    GetClientAbsOrigin(attacker, Origin[0]);
                    GetClientAbsOrigin(victim, Origin[1]);
                    MakeVectorFromPoints(Origin[0], Origin[1], Velocity);
                    NormalizeVector(Velocity, Velocity);
                    ScaleVector(Velocity, g_Client[attacker].EnhanceCrowbar ? 1000.0 : 200.0);
                    TeleportEntity(victim, NULL_VECTOR, NULL_VECTOR, Velocity);
                    return Plugin_Handled;
                }
            }
            else
            {
                damage = 99999.0 / 0.35;
                CreateTimer(0.0, RemoveKnife, attacker);
                return Plugin_Changed;
            }
        }
    }
    return Plugin_Continue;
}

public Action RemoveKnife(Handle Timer, attacker)
{
    int knife = GetPlayerWeaponSlot(attacker, CS_SLOT_KNIFE);
    if(IsClientValid(attacker) && IsPlayerAlive(attacker) && knife != -1)
    {
        g_Client[attacker].HasKnife = false;
        TTT_RemoveWeapon(attacker, knife, CS_SLOT_KNIFE);
        CreateTimer(0.1, GiveKnife, attacker);
    }
    return Plugin_Continue;
}

public Action GiveKnife(Handle Timer, attacker)
{
    if(IsClientValid(attacker) && IsPlayerAlive(attacker) && GetPlayerWeaponSlot(attacker, CS_SLOT_KNIFE) == -1)
    {
        GivePlayerItem(attacker, "weapon_knife");
    }
    return Plugin_Continue;
}

public OnWeaponSwitchPost(int client, int weapon)
{
    if(IsValidEntity(weapon))
    {
        char aWeapon[32];
        GetEntityClassname(weapon, aWeapon, sizeof(aWeapon));
        if(StrEqual(aWeapon, "weapon_knife"))
        {
            //int vm = GetEntPropEnt(client, Prop_Data, "m_hViewModel");
            //SetEntProp(vm, Prop_Send, "m_nModelIndex", g_Client[client].HasKnife ? v_KnifeModel : v_CrowbarModel);
            if(g_Client[client].HasKnife) TTT_SetViewModel(client, v_KnifeModel);
            SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", g_Client[client].HasKnife ? w_KnifeModel : w_CrowbarModel);
            RequestFrame(InitKnifeFireSpeed, client);
        }
    }
}

public Action HookKnifeSound(int clients[MAXPLAYERS], int &numClients, char sample[PLATFORM_MAX_PATH],
int &entity, int &channel, float &volume, int &level, int &pitch, int &flags,
char soundEntry[PLATFORM_MAX_PATH], int &seed)
{
    if(IsValidEntity(entity))
    {
        int owner = GetEntPropEnt(entity, Prop_Data, "m_hOwnerEntity");
        if(IsClientValid(owner) && !g_Client[owner].HasKnife)
        {
            if(StrContains(sample, "knife_hitwall") > -1)
            {
                EmitSoundToAll(HITWALL_SOUND, entity, SNDCHAN_WEAPON);
                return Plugin_Stop;
            }
            //if(StrContains(sample, "knife_hit") && (sample[23] == '1' || sample[23] == '2' || sample[23] == '3' || sample[23] == '4'))
            //{
            //    return Plugin_Stop;
            //}
            if(StrContains(sample, "knife_stab") > -1)
            {
                EmitSoundToAll(HIT_SOUND, entity, SNDCHAN_WEAPON);
                return Plugin_Stop;
            }
        }
    }
    return Plugin_Continue;
}

public Action OnWeaponFire(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(GetEventInt(event, "userid"));
    RequestFrame(InitKnifeFireSpeed, client);
    return Plugin_Continue;
}

stock InitKnifeFireSpeed(int client)
{
    if(IsClientValid(client) && IsPlayerAlive(client))
    {
        int iWeapon = GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon");
        if(IsValidEntity(iWeapon))
        {
            decl String:Buffer[32];
            GetEntityClassname(iWeapon, Buffer, sizeof(Buffer));
            if (StrEqual(Buffer, "weapon_knife"))
            {
                SetEntPropFloat(iWeapon, Prop_Send, "m_flNextPrimaryAttack", GetGameTime() + 1.0);
                SetEntPropFloat(iWeapon, Prop_Send, "m_flNextSecondaryAttack", GetGameTime() + 1.0);
            }
        }
    }
}

public OnPlayerRunCmdPre(int client, int buttons, int impulse, const float vel[3], const float angles[3], int weapon, int subtype, int cmdnum, int tickcount, int seed, const int mouse[2])
{
    if (!IsClientValid(client)) return;
    g_Client[client].buttons = buttons;
}