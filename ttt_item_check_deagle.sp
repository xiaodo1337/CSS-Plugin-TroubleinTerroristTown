#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] 侦探沙鹰",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}


#define WEAPON_BASE "weapon_deagle"
#define WEAPON_SLOT CS_SLOT_SECONDARY

#define V_DEAGLEMODEL "models/weapons/ttt/V_g_deagle.mdl"
#define W_DEAGLEMODEL "models/weapons/ttt/w_g_deagle.mdl"
#define VModelMaterialsFolderDeagle "materials/models/weapons/v_models/g_deagle"
#define WModelMaterialsFolderDeagle "materials/models/weapons/w_models/g_deagle"
#define ModelsFolder "models/weapons/ttt"

int g_ItemID;
int v_DeagleModel;
int w_DeagleModel;

public OnConfigsExecuted()
{
    g_ItemID = TTT_RegisterShopItem("侦探沙鹰", ITEMTYPE_WEAPON, "仅一发子弹! \n击中后显示身份并扣除叛徒50血量", 1, TTT_ROLE_DETECTIVE, -1, -1, 50);
}

public OnMapStart()
{
    TTT_AddFilesToDownloadList(VModelMaterialsFolderDeagle);
    TTT_AddFilesToDownloadList(WModelMaterialsFolderDeagle);
    TTT_AddFilesToDownloadList(ModelsFolder, "g_deagle");
    v_DeagleModel = PrecacheModel(V_DEAGLEMODEL);
    w_DeagleModel = PrecacheModel(W_DEAGLEMODEL);
    LoopValidClients(client)
    {
        OnClientPutInServer(client);
    }
    TTT_RegisterViewModel(WEAPON_BASE, v_DeagleModel);
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
            //由于客户端预测，模型会闪烁，现已更换方法
            //int vm = GetEntPropEnt(client, Prop_Data, "m_hViewModel");
            //SetEntProp(vm, Prop_Send, "m_nModelIndex", v_DeagleModel);
            SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", w_DeagleModel);
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
    SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", w_DeagleModel);
}

public Action TTT_OnItemPurchase(int client, int ItemID)
{
    if(ItemID == g_ItemID)
    {
        int before_weapon = GetPlayerWeaponSlot(client, WEAPON_SLOT);
        if(before_weapon != -1)     //如果副武器槽位有武器
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
    if(IsClientValid(attacker) && IsPlayerAlive(attacker) && victim != attacker)
    {
        char aWeapon[32];
        GetClientWeapon(attacker, aWeapon, sizeof(aWeapon));
        if(StrEqual(aWeapon, WEAPON_BASE))
        {
            if(TTT_GetClientRole(victim) == TTT_ROLE_TRAITOR)
            {
                TTT_SetClientLeak(victim, true);
                damage = 50.0 / 0.35;
                return Plugin_Changed;
            }
            else return Plugin_Handled;
        }
    }
    return Plugin_Continue;
}