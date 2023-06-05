#pragma semicolon 1
#include <sourcemod>
#include <cstrike>
#include <morecolors>
#include <sdkhooks>
#include <sdktools>
#include <vphysics>
#include <TroubleinTerroristTown>

#include "ttt/core/global"
#include "ttt/core/precache"
#include "ttt/core/commands"
#include "ttt/core/functions"
#include "ttt/core/natives"
#include "ttt/core/events"
#include "ttt/core/frags"
#include "ttt/core/karma"
#include "ttt/core/round"
#include "ttt/core/roles"
#include "ttt/core/timer"
#include "ttt/core/players"
#include "ttt/core/radar"
#include "ttt/core/weapons"
#include "ttt/core/damage"

#include "ttt/blockmsg"
#include "ttt/blockcmds"
#include "ttt/entites"
#include "ttt/hud"
#include "ttt/rules"
#include "ttt/sounds"
#include "ttt/icons"
#include "ttt/bodies"
#include "ttt/dnascanner"
#include "ttt/voice_chat"
#include "ttt/radio"
#include "ttt/shop"
#include "ttt/grabbermod"

public Plugin:myinfo = {
    name = "匪镇谍影",
    author = "小豆(xiaodo)",
    description = "匪镇谍影模式主插件",
    version = "1.0 Beta",
    url = "QQ群725354886"
}

#define DEBUG

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
    InitForwards();
    InitNatives();
    return APLRes_Success;
}

public OnPluginStart()
{
    InitDynamicArrays();
    InitEvents();
    InitCommands();
    InitCvars();
    InitSyncHUDChannels();
    InitChatMsg();
    InitMsgBlock();
    InitCmdBlock();
    CreateTimer(3.0, CheckGameCondition, _, TIMER_REPEAT);
    LoopValidClients(client)
    {
        FindClientViewModel(client);
    }
    #if defined DEBUG
        RegConsoleCmd("sm_become", Test);
    #endif
}

#if defined DEBUG
public Action:Test(int client, int args)
{
    if(args == 1) SetClientRole(client, GetCmdArgInt(1));
}
#endif

public void OnEntityCreated(int entity, const char[] classname)
{
    RequestFrame(InitEntities, entity);
    if (StrEqual(classname, "predicted_viewmodel"))
    {
        SDKHook(entity, SDKHook_Spawn, OnEntitySpawned);
    }
}

public OnMapInit()
{
    g_aShopItem.Clear();
}

public OnMapStart()
{
    InitResources();
    LoopValidClients(client)
    {
        OnClientPutInServer(client);
    }
    TTT_EndRound(TTT_ROLE_NONE);
}

public OnMapEnd()
{
    TTT_EndAllTimer();
}

public OnConfigsExecuted()
{
    ServerCommand("mp_friendlyfire 1");
    ServerCommand("mp_autoteambalance 0");
    ServerCommand("mp_limitteams 0");
    ServerCommand("mp_tkpunish 0");
    ServerCommand("mp_freezetime 1");
    ServerCommand("mp_autokick 0");
    ServerCommand("mp_teams_unbalance_limit 0");
    ServerCommand("mp_forcecamera 0");
    ServerCommand("mp_hostagepenalty 0");
    ServerCommand("mp_tkpunish 0");
    ServerCommand("ammo_338mag_max 0");
    ServerCommand("ammo_357sig_max 0");
    ServerCommand("ammo_45acp_max 0");
    ServerCommand("ammo_50AE_max 0");
    ServerCommand("ammo_556mm_box_max 0");
    ServerCommand("ammo_556mm_max 0");
    ServerCommand("ammo_57mm_max 100");
    ServerCommand("ammo_762mm_max 0");
    ServerCommand("ammo_9mm_max 0");
    ServerCommand("ammo_buckshot_max 0");
    ServerCommand("ammo_flashbang_max 1");
    ServerCommand("ammo_hegrenade_max 1");
    ServerCommand("ammo_smokegrenade_max 1");
}

public OnClientPutInServer(int client)
{
    CheckGameCondition(INVALID_HANDLE);
    InitSDKHooks(client);
    HudClientInit(client);
    ClearRoleIcon(client);
    ClearDNAIcon(INVALID_HANDLE, client);
    InitPlayerVars(client);
    BlockPlayerVoice(client);
    TTT_EndPlayerTimer(client);
    ClientCommand(client, "spec_menu");
}

public OnClientDisconnect(int client)
{
    UnHookSDKHooks(client);
    CheckGameCondition(INVALID_HANDLE);
    CheckWinConditions();
    HudOnClientDisconnect(client);
    ClearRoleIcon(client);
    ClearDNAIcon(INVALID_HANDLE, client);
    InitPlayerVars(client);
    TTT_EndPlayerTimer(client);
    Command_UnGrab(client);
}

public OnGameFrame()
{
    SetTeamScore(CS_TEAM_CT, InnocentWin);
    SetTeamScore(CS_TEAM_T, TraitorWin);
}

public Action:OnPlayerRunCmd(int client, int &buttons, int &impulse, float vel[3], float angles[3], int &weapon, int &subtype, int &cmdnum, int &tickcount, int &seed, int mouse[2])
{
    if (!IsClientValid(client)) return Plugin_Continue;
    if(impulse == 100)
    {
        if(buttons & IN_SPEED) return Plugin_Continue;
        OpenRadioMenu(client);
        impulse = 0;
        return Plugin_Changed;
    }
    if(buttons & IN_SCORE && !(GetClientButtons(client) & IN_SCORE) && g_Client[client].Role == TTT_ROLE_DETECTIVE)
    {
        OpenDNAMenu(client);
    }
    if(buttons & IN_USE && !(GetClientButtons(client) & IN_USE))
    {
        CheckBody(client, buttons);
    }
    if(buttons & IN_SCORE)
    {
        if(buttons & IN_USE && !(GetClientButtons(client) & IN_USE))
        {
            ChangeVoiceChannel(client);
        }
    }

    //--------------GrabberMod--------------//
    if (buttons & IN_JUMP)
    {
        int iEnt = GetEntPropEnt(client, Prop_Send, "m_hGroundEntity");

        if (IsValidEntity(iEnt))
        {
            char sName[128];
            GetEdictClassname(iEnt, sName, sizeof(sName));

            if (StrContains(sName, "prop_", false) == -1 || StrContains(sName, "door", false) != -1)
            {
                return Plugin_Continue;
            }
            else
            {
                if (StrEqual(sName, "prop_physics") || StrEqual(sName, "prop_physics_multiplayer") || StrEqual(sName, "func_physbox") || StrEqual(sName, "prop_physics_respawnable"))
                {
                    Command_UnGrab(client);
                }
            }
        }
    }
    GetClientWeapon(client, Buffer, sizeof(Buffer));
    if(StrEqual(Buffer, "weapon_c4"))
    {
        if (buttons & IN_ATTACK2 && !(GetClientButtons(client) & IN_ATTACK2) && GetGameTime() >= g_Client[client].PressTime)
        {
            GrabPhysicsEntity(client);
            g_Client[client].PressTime = GetGameTime() + 1.0;
        }
    }
    else
    {
        Command_UnGrab(client);
    }

    return Plugin_Changed;
}