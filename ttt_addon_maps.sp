#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] 地图功能兼容",
    author = "小豆(xiaodo)",
    description = "TTT插件",
    version = "1.0",
    url = "QQ群725354886"
}

char HookEntity[5][32] = {
    "trigger_multiple",
    "trigger_hurt",
    "trigger_once",
    "func_button",
    "logic_relay"
};

char HookTargetName[13][32] = {
    "OnStartTouch",
    "OnStartTouchAll",
    "OnEndTouch",
    "OnEndTouchAll",
    "OnTouching",
    "OnNotTouching",
    "OnHurt",
    "OnHurtPlayer",
    "OnTrigger",
    "OnDamaged",
    "OnPressed",
    "OnIn",
    "OnOut"
};

public OnEntityCreated(int entity, const char[] classname)
{
    for(int i = 0;i < sizeof(HookEntity);i++)
    {
        if(strcmp(classname, HookEntity[i]) == 0)
        {
            RequestFrame(Hook_TargetName, entity);
        }
    }
}

public Hook_TargetName(int entity)
{
    if(!IsValidEntity(entity)) return;
    new String:Name[64], String:Buffer[64];
    GetEntityTargetName(entity, Name, sizeof(Name));
    if(StrContains(Name, "TTT_Credits_Give") > -1)
    {
        HookSingleEntityOutput(entity, "OnUser1", Hook_Credits_Give);
        return;
    }
    if(StrContains(Name, "TTT_Credits_Take") > -1)
    {
        HookSingleEntityOutput(entity, "OnUser4", Hook_Credits_Take);
        return;
    }
    if(StrContains(Name, "TTT_Win_Innocents") > -1)
    {
        HookSingleEntityOutput(entity, "OnUser1", Hook_EndGame_Innocents);
        return;
    }
    if(StrContains(Name, "TTT_Win_Traitors") > -1)
    {
        HookSingleEntityOutput(entity, "OnUser1", Hook_EndGame_Traitors);
        return;
    }
    if(StrContains(Name, "TTT_GetRole_Logic") > -1)
    {
        HookSingleEntityOutput(entity, "OnTrigger", Hook_GetClientRole);
        return;
    }
    for(int i = 0;i < sizeof(HookTargetName);i++)
    {
        Format(Buffer, sizeof(Buffer), "TTT_GetRole_%s", HookTargetName[i]);
        if(StrContains(Name, Buffer) > -1)
        {
            Format(Buffer, sizeof(Buffer), "On%s", HookTargetName[i]);
            HookSingleEntityOutput(entity, Buffer, Hook_GetClientRole);
            return;
        }
    }
}

public Hook_GetClientRole(const char[] output, int caller, int activator, float delay)
{
    if(!IsClientValid(activator) || !IsPlayerAlive(activator)) return;
    if(TTT_GetClientRole(activator) == TTT_ROLE_TRAITOR)
    {
        PrintToChat(activator, "2");
        AcceptEntityInput(caller, "FireUser2", activator, caller);
    }
    else if(TTT_GetClientRole(activator) == TTT_ROLE_DETECTIVE)
    {
        PrintToChat(activator, "3");
        AcceptEntityInput(caller, "FireUser3", activator, caller);
    }
    else
    {
        PrintToChat(activator, "1");
        AcceptEntityInput(caller, "FireUser1", activator, caller);
    }
}

public Hook_Credits_Give(const char[] output, int caller, int activator, float delay)
{
    if(!IsClientValid(activator) || !IsPlayerAlive(activator)) return;
    TTT_SetClientCredits(activator, TTT_GetClientCredits(activator) + 1);
}

public Hook_Credits_Take(const char[] output, int caller, int activator, float delay)
{
    if(!IsClientValid(activator) || !IsPlayerAlive(activator)) return;
    if(TTT_GetClientCredits(activator) > 0)
    {
        AcceptEntityInput(caller, "FireUser1", activator, caller);
        TTT_SetClientCredits(activator, TTT_GetClientCredits(activator) - 1);
    }
    else
    {
        AcceptEntityInput(caller, "FireUser2", activator, caller);
    }
}

public Hook_EndGame_Innocents(const char[] output, int caller, int activator, float delay)
{
    TTT_ForceEndRound(TTT_TEAM_INNOCENTS);
}

public Hook_EndGame_Traitors(const char[] output, int caller, int activator, float delay)
{
    TTT_ForceEndRound(TTT_TEAM_TRAITORS);
}

stock bool GetEntityTargetName(int entity, char[] clsname, int maxlength)
{
	return !!GetEntPropString(entity, Prop_Data, "m_iName", clsname, maxlength);
}