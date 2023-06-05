#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] 雷达",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}

int g_ItemID[2];
int g_iBeam = -1;
int g_iHalo = -1;
new Float:RadarTime = 5.0;
Handle TimerHandle[MAXPLAYERS + 1];

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
    CreateNative("TTT_GiveClientRadar", Native_GiveClientRadar)
    return APLRes_Success;
}

public OnPluginStart()
{
    HookEvent("round_start", Event_RoundStart);
}

public void OnMapStart()
{
    g_iBeam = PrecacheModel("materials/sprites/bomb_planted_ring.vmt");
    g_iHalo = PrecacheModel("materials/sprites/halo.vtf");
}

public OnConfigsExecuted()
{
    g_ItemID[0] = TTT_RegisterShopItem("雷达", ITEMTYPE_PassiveEquip, "每隔一段时间显示所有人的位置", 1, TTT_ROLE_DETECTIVE, -1, 1, 0);
    g_ItemID[1] = TTT_RegisterShopItem("雷达", ITEMTYPE_PassiveEquip, "每隔一段时间显示所有人的位置", 1, TTT_ROLE_TRAITOR, -1, 1, 0);
}

public Action Event_RoundStart(Event event, const char[] name, bool dontBroadcast)
{
    LoopValidClients(client)
    {
        if(IsValidHandle(TimerHandle[client]))
        {
            KillTimer(TimerHandle[client]);
            TimerHandle[client] = INVALID_HANDLE;
        }
    }
    return Plugin_Continue;
}

public OnClientPutInServer(int client)
{
    if(IsValidHandle(TimerHandle[client]))
    {
        KillTimer(TimerHandle[client]);
        TimerHandle[client] = INVALID_HANDLE;
    }
}

public OnClientDisconnect(int client)
{
    if(IsValidHandle(TimerHandle[client]))
    {
        KillTimer(TimerHandle[client]);
        TimerHandle[client] = INVALID_HANDLE;
    }
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
    if(ItemID == g_ItemID[0] || ItemID == g_ItemID[1])
    {
        int role = TTT_GetClientRole(client);
        if(role == TTT_ROLE_DETECTIVE || role == TTT_ROLE_TRAITOR)
        {
            SetBeam(client);
        }
    }
    return Plugin_Continue;
}

void SetBeam(int client)
{
    if(IsValidHandle(TimerHandle[client])) return;
    int role = TTT_GetClientRole(client);
    float fPos[3];
    LoopValidClients(i)
    {
        if (IsPlayerAlive(i))
        {
            if (i == client) continue;
            GetClientAbsOrigin(i, fPos);
            int iColor[4];
            iColor[0] = 50;
            iColor[1] = 50;
            iColor[2] = 255;
            iColor[3] = 255;
            TE_SetupBeamRingPoint(fPos, 15.0, 30.0, g_iBeam, g_iHalo, 0, 15, (RadarTime - 1.5), 5.0, 0.0, iColor, 10, 0);
            TE_SendToClient(client);
        }
    }

    CPrintToChat(client, "%s雷达已更新", TTT_TAG);
    if (IsPlayerAlive(client) && (role == TTT_ROLE_TRAITOR || role == TTT_ROLE_DETECTIVE))
    {
        TimerHandle[client] = CreateTimer(RadarTime, Timer_UpdateRadar, client);
    }
}

public Action Timer_UpdateRadar(Handle timer, int client)
{
    KillTimer(timer);
    TimerHandle[client] = INVALID_HANDLE;
    if (IsClientValid(client) && IsPlayerAlive(client))
    {
        SetBeam(client);
    }
    return Plugin_Stop;
}

public Native_GiveClientRadar(Handle Plugin, int Params)
{
    int client = GetNativeCell(1);
    SetBeam(client);
}
