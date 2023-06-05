#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] 基础武器",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}

int g_ItemID[6];

public OnConfigsExecuted()
{
    g_ItemID[0] = TTT_RegisterShopItem("双枪", ITEMTYPE_WEAPON, "更快的攻击速度", 1, TTT_ROLE_INNOCENT, 99, 99, 0);
    g_ItemID[1] = TTT_RegisterShopItem("双枪", ITEMTYPE_WEAPON, "更快的攻击速度", 1, TTT_ROLE_DETECTIVE, 99, 99, 0);
    g_ItemID[2] = TTT_RegisterShopItem("双枪", ITEMTYPE_WEAPON, "更快的攻击速度", 1, TTT_ROLE_TRAITOR, 99, 99, 0);

    g_ItemID[3] = TTT_RegisterShopItem("Sg552", ITEMTYPE_WEAPON, "带有瞄准镜的全自动武器", 1, TTT_ROLE_INNOCENT, 99, 99, 0);
    g_ItemID[4] = TTT_RegisterShopItem("Sg552", ITEMTYPE_WEAPON, "带有瞄准镜的全自动武器", 1, TTT_ROLE_DETECTIVE, 99, 99, 0);
    g_ItemID[5] = TTT_RegisterShopItem("Sg552", ITEMTYPE_WEAPON, "带有瞄准镜的全自动武器", 1, TTT_ROLE_TRAITOR, 99, 99, 0);
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
        int before_weapon = GetPlayerWeaponSlot(client, CS_SLOT_SECONDARY);
        if(before_weapon != -1)     //如果副武器槽位有武器
        {
            char classname[32];
            GetEntityClassname(before_weapon, classname, sizeof(classname));
            if (StrEqual(classname, "weapon_elite"))
            {
                CPrintToChat(client, "%s您已拥有该武器！请丢掉后再购买！", TTT_TAG);
                return Plugin_Handled;
            }
            else
            {
                CS_DropWeapon(client, before_weapon, false);
            }
        }
        int wepent = GivePlayerItem(client, "weapon_elite");
        TTT_SetWeaponClip(wepent, 30);
        TTT_SetPlayerAmmo(client, wepent, 90);
    }
    if(ItemID == g_ItemID[3] || ItemID == g_ItemID[4] || ItemID == g_ItemID[5])
    {
        int before_weapon = GetPlayerWeaponSlot(client, CS_SLOT_PRIMARY);
        if(before_weapon != -1)     //如果副武器槽位有武器
        {
            char classname[32];
            GetEntityClassname(before_weapon, classname, sizeof(classname));
            if (StrEqual(classname, "weapon_sg552"))
            {
                CPrintToChat(client, "%s您已拥有该武器！请丢掉后再购买！", TTT_TAG);
                return Plugin_Handled;
            }
            else
            {
                CS_DropWeapon(client, before_weapon, false);
            }
        }
        int wepent = GivePlayerItem(client, "weapon_sg552");
        TTT_SetWeaponClip(wepent, 30);
        TTT_SetPlayerAmmo(client, wepent, 90);
    }
    return Plugin_Continue;
}

public Action TTT_OnTraceAttack(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &ammotype, int hitbox, int hitgroup)
{
    if(IsClientValid(attacker) && IsPlayerAlive(attacker) && victim != attacker)
    {
        char aWeapon[32];
        GetClientWeapon(attacker, aWeapon, sizeof(aWeapon));
        if(StrEqual(aWeapon, "weapon_sg552"))
        {
            damage *= 0.85;
            return Plugin_Changed;
        }
        if(StrEqual(aWeapon, "weapon_elite"))
        {
            damage *= 0.75;
            return Plugin_Changed;
        }
    }
    return Plugin_Continue;
}