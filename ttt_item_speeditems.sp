#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] 疾行靴 & 肾上腺素",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}

#define ADRENO_GIVEN_HEALTH 50
#define ADRENO_HEALTH_REMOVE_PERSEC 3
#define ADRENO_SPEEDTIME 15.0

#define SND_ADRENO "player/breathe1.wav"

int g_ItemID[6];

enum struct PlayerData
{
    bool HasBoots;
    bool HasAdreno;
    int AdrenoHPAmount;
    Handle AdrenoSpeedTimer;
    Handle AdrenoHealthTimer;
}

PlayerData g_Client[MAXPLAYERS + 1];

public OnPluginStart()
{
    HookEvent("round_start", Event_RoundStart);
}

public OnMapStart()
{
    PrecacheSound(SND_ADRENO);
    LoopValidClients(client)
    {
        OnClientPutInServer(client);
    }
}

public OnConfigsExecuted()
{
    g_ItemID[0] = TTT_RegisterShopItem("疾行靴", ITEMTYPE_PassiveEquip, "购买后按下 静步键 可加快移动速度", 1, TTT_ROLE_DETECTIVE, -1, 1, 0);
    g_ItemID[1] = TTT_RegisterShopItem("疾行靴", ITEMTYPE_PassiveEquip, "购买后按下 静步键 可加快移动速度", 1, TTT_ROLE_TRAITOR, -1, 1, 0);
    g_ItemID[2] = TTT_RegisterShopItem("疾行靴", ITEMTYPE_PassiveEquip, "购买后按下 静步键 可加快移动速度", 1, TTT_ROLE_INNOCENT, -1, 1, 0);
    g_ItemID[3] = TTT_RegisterShopItem("肾上腺素", ITEMTYPE_PassiveEquip, "短时间内提升生命值与移动速度", 1, TTT_ROLE_DETECTIVE, -1, -1, 0);
    g_ItemID[4] = TTT_RegisterShopItem("肾上腺素", ITEMTYPE_PassiveEquip, "短时间内提升生命值与移动速度", 1, TTT_ROLE_TRAITOR, -1, -1, 0);
    g_ItemID[5] = TTT_RegisterShopItem("肾上腺素", ITEMTYPE_PassiveEquip, "短时间内提升生命值与移动速度", 1, TTT_ROLE_INNOCENT, -1, -1, 0);
}

public Action Event_RoundStart(Event event, const char[] name, bool dontBroadcast)
{
    LoopValidClients(client)
    {
        ResetPlayerVars(client);
    }
    return Plugin_Continue;
}

public OnClientPutInServer(int client)
{
    ResetPlayerVars(client);
}

public OnClientDisconnect(int client)
{
    ResetPlayerVars(client);
}

public OnPluginEnd()
{
    for(int i;i < sizeof(g_ItemID);i++)
    {
        if(g_ItemID[i]) TTT_RemoveShopItem(g_ItemID[i]);
    }
}

public Action TTT_OnItemPurchase(int client, int ItemID)
{
    if(ItemID == g_ItemID[0] || ItemID == g_ItemID[1] || ItemID == g_ItemID[2])
    {
        if(!g_Client[client].HasBoots) g_Client[client].HasBoots = true;
        else {
            CPrintToChat(client, "%s你已经购买过该道具了！", TTT_TAG);
            return Plugin_Handled;
        }
    }
    else if(ItemID == g_ItemID[3] || ItemID == g_ItemID[4] || ItemID == g_ItemID[5])
    {
        if(!g_Client[client].HasAdreno) {
            g_Client[client].HasAdreno = true;
            g_Client[client].AdrenoHPAmount = ADRENO_GIVEN_HEALTH;
            SetEntityHealth(client, GetClientHealth(client) + ADRENO_GIVEN_HEALTH);
            SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", 1.6);
            g_Client[client].AdrenoSpeedTimer = CreateTimer(ADRENO_SPEEDTIME, RemoveSpeedEffect, client);
            g_Client[client].AdrenoHealthTimer = CreateTimer(1.0, RemoveHealthEffect, client, TIMER_REPEAT);
            EmitSoundToClient(client, SND_ADRENO, client, SNDCHAN_STATIC);
        } else {
            CPrintToChat(client, "%s你正在使用该道具！请等待道具时效过后再购买！", TTT_TAG);
            return Plugin_Handled;
        }
    }
    return Plugin_Continue;
}

public Action OnPlayerRunCmd(int client, int &buttons, int &impulse, float vel[3], float angles[3], int &weapon, int &subtype, int &cmdnum, int &tickcount, int &seed, int mouse[2])
{
    if (!IsClientValid(client) || !IsPlayerAlive(client)) return Plugin_Continue;
    if(buttons & IN_SPEED && g_Client[client].HasBoots)
    {
        if(!g_Client[client].HasAdreno) SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", 1.3);
        buttons &= ~IN_SPEED;
        return Plugin_Changed;
    }
    else if(!g_Client[client].HasAdreno)
    {
        SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", 1.0);
    }
    return Plugin_Continue;
}

public Action RemoveSpeedEffect(Handle Timer, int client)
{
    KillTimer(Timer);
    g_Client[client].AdrenoSpeedTimer = INVALID_HANDLE;
    if(!IsValidHandle(g_Client[client].AdrenoHealthTimer)) g_Client[client].HasAdreno = false;
    SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", 1.0);
    EmitSoundToClient(client, SND_ADRENO, client, SNDCHAN_STATIC, _, SND_STOPLOOPING);
    return Plugin_Continue;
}

public Action RemoveHealthEffect(Handle Timer, int client)
{
    //KillTimer(Timer);
    //g_Client[client].AdrenoHealthTimer = INVALID_HANDLE;
    if(!IsClientValid(client) || !IsPlayerAlive(client))
    {
        KillTimer(Timer);
        g_Client[client].AdrenoHealthTimer = INVALID_HANDLE;
        g_Client[client].HasAdreno = false;
        if(IsValidHandle(g_Client[client].AdrenoSpeedTimer))
        {
            KillTimer(g_Client[client].AdrenoSpeedTimer);
            g_Client[client].AdrenoSpeedTimer = INVALID_HANDLE;
        }
        EmitSoundToClient(client, SND_ADRENO, client, SNDCHAN_STATIC, _, SND_STOPLOOPING);
        return Plugin_Continue;
    }
    int after_health = Max(1, GetClientHealth(client) - Min(g_Client[client].AdrenoHPAmount, ADRENO_HEALTH_REMOVE_PERSEC));
    SetEntityHealth(client, after_health);
    if(after_health <= 1 || g_Client[client].AdrenoHPAmount <= ADRENO_HEALTH_REMOVE_PERSEC)
    {
        if(!IsValidHandle(g_Client[client].AdrenoSpeedTimer)) g_Client[client].HasAdreno = false;
        KillTimer(Timer);
        g_Client[client].AdrenoHealthTimer = INVALID_HANDLE;
        return Plugin_Continue;
    }
    g_Client[client].AdrenoHPAmount -= ADRENO_HEALTH_REMOVE_PERSEC;
    return Plugin_Continue;
}

public ResetPlayerVars(int client)
{
    g_Client[client].HasBoots = false;
    g_Client[client].HasAdreno = false;
    g_Client[client].HasBoots = false;
    if(IsValidHandle(g_Client[client].AdrenoSpeedTimer))
    {
        KillTimer(g_Client[client].AdrenoSpeedTimer);
        g_Client[client].AdrenoSpeedTimer = INVALID_HANDLE;
    }
    if(IsValidHandle(g_Client[client].AdrenoHealthTimer))
    {
        KillTimer(g_Client[client].AdrenoHealthTimer);
        g_Client[client].AdrenoHealthTimer = INVALID_HANDLE;
    }
    if(!IsClientValid(client)) return;
    EmitSoundToClient(client, SND_ADRENO, client, SNDCHAN_STATIC, _, SND_STOPLOOPING);
}