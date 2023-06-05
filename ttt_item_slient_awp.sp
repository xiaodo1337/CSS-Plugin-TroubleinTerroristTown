#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>

public Plugin:myinfo = {
    name = "[TTT] 消音AWP",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}

#define WEAPON_BASE "weapon_awp"
#define WEAPON_SLOT CS_SLOT_PRIMARY

#define V_AWPMODEL "models/weapons/ttt/v_slient_awp.mdl"
#define W_AWPMODEL "models/weapons/ttt/w_slient_awp.mdl"
#define FIRE_SOUND "weapons/ttt/silence_awp.wav"
#define VModelMaterialsFolderAWP "materials/models/weapons/v_models/slient_awp"
#define WModelMaterialsFolderAWP "materials/models/weapons/w_models/slient_awp"
#define ModelsFolder "models/weapons/ttt"

int g_ItemID;
int v_AWPModel;
int w_AWPModel;

public OnPluginStart()
{
    AddTempEntHook("Shotgun Shot", Hook_ShotgunShot);   //钩住玩家开火事件，达到消音效果
}

public OnConfigsExecuted()
{
    g_ItemID = TTT_RegisterShopItem("消音AWP", ITEMTYPE_WEAPON, "一把带有消音且威力强大的狙击枪! 仅5发子弹", 1, TTT_ROLE_TRAITOR, 6, 2, 0);
}

public OnMapStart()
{
    char buffer[64];
    Format(buffer, sizeof(buffer), "sound/%s", FIRE_SOUND);
    AddFileToDownloadsTable(buffer);
    PrecacheSound(FIRE_SOUND);

    TTT_AddFilesToDownloadList(VModelMaterialsFolderAWP);
    TTT_AddFilesToDownloadList(WModelMaterialsFolderAWP);
    TTT_AddFilesToDownloadList(ModelsFolder, "slient_awp");
    v_AWPModel = PrecacheModel(V_AWPMODEL);
    w_AWPModel = PrecacheModel(W_AWPMODEL);
    TTT_RegisterViewModel(WEAPON_BASE, v_AWPModel);
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
            //由于客户端预测，模型会闪烁，现已更换方法
            //int vm = GetEntPropEnt(client, Prop_Data, "m_hViewModel");
            //SetEntProp(vm, Prop_Send, "m_nModelIndex", v_AWPModel);
            SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", w_AWPModel);
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
    SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", w_AWPModel);
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
        TTT_SetWeaponClip(wepent, 5);
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
            damage *= 2.5;
            return Plugin_Changed;
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
        EmitSoundToAll(FIRE_SOUND, iWeapon, SNDCHAN_WEAPON, _, _, 0.1);
        return Plugin_Stop;
    }
    return Plugin_Continue;
}