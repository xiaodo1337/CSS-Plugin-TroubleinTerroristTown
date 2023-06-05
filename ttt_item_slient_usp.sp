#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] USP",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}

int g_ItemID;

public OnConfigsExecuted()
{
    g_ItemID = TTT_RegisterShopItem("USP", ITEMTYPE_WEAPON, "能使用消音器并且威力大于普通手枪", 1, TTT_ROLE_TRAITOR, 99, 99, 0);
}

public OnPluginEnd()
{
    if(g_ItemID) TTT_RemoveShopItem(g_ItemID);
}

public Action TTT_OnItemPurchase(int client, int ItemID)
{
    if(ItemID == g_ItemID)
    {
        int before_weapon = GetPlayerWeaponSlot(client, CS_SLOT_SECONDARY);
        if(before_weapon != -1)     //如果主武器槽位有武器
        {
            char classname[32];
            GetEntityClassname(before_weapon, classname, sizeof(classname));
            if (StrEqual(classname, "weapon_usp"))
            {
                CPrintToChat(client, "%s您已拥有该武器！请丢掉后再购买！", TTT_TAG);
                return Plugin_Handled;
            }
            else
            {
                CS_DropWeapon(client, before_weapon, false);
            }
        }
        int wepent = GivePlayerItem(client, "weapon_usp");
        TTT_SetWeaponClip(wepent, 12);
        TTT_SetPlayerAmmo(client, wepent, 12);
    }
    return Plugin_Continue;
}

public Action TTT_OnTraceAttack(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &ammotype, int hitbox, int hitgroup)
{
    if(IsClientValid(attacker) && IsPlayerAlive(attacker) && victim != attacker)
    {
        char classname[32];
        GetClientWeapon(attacker, classname, sizeof(classname));
        if(StrEqual(classname, "weapon_usp"))
        
        {
            damage *= 2.0;
            return Plugin_Changed;
        }
    }
    return Plugin_Continue;
}