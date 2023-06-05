#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <morecolors>
#include <cstrike>
#include <TroubleinTerroristTown>
#include <vphysics>

#define WEAPON_BASE "weapon_flashbang"
#define WEAPON_SLOT CS_SLOT_GRENADE

#define SND_HEAL "items/medshot4.wav"
#define SND_CANTHEAL "items/medshotno1.wav"
#define SND_WARNING "npc/strider/charging.wav"
#define SND_EXPLODE "weapons/hegrenade/explode5.wav"

#define BOX_MODEL "models/items/item_item_crate.mdl"

#define ModelsFolder "models/weapons/ttt"
#define V_MODEL "models/weapons/ttt/v_healthkit.mdl"
#define W_MODEL "models/items/healthkit.mdl"

int g_ItemID[4];

enum StationType {
    Station_None = -1,
    Station_Explode = 0,
    Station_Health,
    Station_Health_Plus
}

enum struct StationData  {
    int EntityRef;
    StationType Type;
    int Health;
    int HealthAmount;
}

enum struct PlayerData {
    StationType Type;
    float PressCoolDown;
}

int v_Model, w_Model;

PlayerData g_iPlayer[MAXPLAYERS + 1];
ArrayList g_aStation;

public Plugin:myinfo = {
    name = "[TTT] 生命恢复站",
    author = "小豆(xiaodo)",
    description = "TTT道具",
    version = "1.0",
    url = "QQ群725354886"
}

public OnPluginStart()
{
    g_aStation = new ArrayList(sizeof(StationData));
    HookEvent("round_start", Event_RoundStart);
}

public void OnMapStart()
{
    PrecacheSound(SND_HEAL);
    PrecacheSound(SND_CANTHEAL);
    PrecacheSound(SND_WARNING);
    PrecacheSound(SND_EXPLODE);
    PrecacheModel(BOX_MODEL);

    
    TTT_AddFilesToDownloadList(ModelsFolder, "v_healthkit");
    v_Model = PrecacheModel(V_MODEL);
    w_Model = PrecacheModel(W_MODEL);
    
    TTT_RegisterViewModel(WEAPON_BASE, v_Model);
    LoopValidClients(client)
    {
        OnClientPutInServer(client);
    }

}

public OnConfigsExecuted()
{
    g_ItemID[0] = TTT_RegisterShopItem("生命恢复站", ITEMTYPE_ActiveEquip, "置放后,可供所有人恢复生命值", 1, TTT_ROLE_INNOCENT, -1, -1, 0);
    g_ItemID[1] = TTT_RegisterShopItem("生命恢复站", ITEMTYPE_ActiveEquip, "置放后,可供所有人恢复生命值", 1, TTT_ROLE_TRAITOR, -1, -1, 0);
    g_ItemID[2] = TTT_RegisterShopItem("生命恢复站[强化]", ITEMTYPE_ActiveEquip, "置放后,可供所有人恢复生命值", 1, TTT_ROLE_DETECTIVE, -1, -1, 0);
    g_ItemID[3] = TTT_RegisterShopItem("爆炸生命恢复站", ITEMTYPE_ActiveEquip, "置放后,任何人对其按下 使用键 延迟1秒爆炸xD", 1, TTT_ROLE_TRAITOR, -1, -1, 0);
}

public OnPluginEnd()
{
    for(int i;i < sizeof(g_ItemID);i++)
    {
        if(g_ItemID[i] > 0) TTT_RemoveShopItem(g_ItemID[i]);
    }
}

public Action Event_RoundStart(Event event, const char[] name, bool dontBroadcast)
{
    CleanUpStation();
    return Plugin_Continue;
}

public OnClientPutInServer(int client)
{
    SDKHook(client, SDKHook_WeaponSwitchPost, OnWeaponSwitchPost);
    SDKHook(client, SDKHook_WeaponDropPost, OnWeaponDropPost);
    g_iPlayer[client].Type = Station_None;
    g_iPlayer[client].PressCoolDown = GetGameTime();
}

public OnClientDisconnect(int client)
{
    SDKUnhook(client, SDKHook_WeaponSwitchPost, OnWeaponSwitchPost);
    SDKUnhook(client, SDKHook_WeaponDropPost, OnWeaponDropPost);
    g_iPlayer[client].Type = Station_None;
    g_iPlayer[client].PressCoolDown = GetGameTime();
}

public OnWeaponSwitchPost(int client, int weapon)
{
    if(IsValidEntity(weapon))
    {
        char aWeapon[32];
        GetEntityClassname(weapon, aWeapon, sizeof(aWeapon));
        if(StrEqual(aWeapon, WEAPON_BASE))
        {
            //int vm = GetEntPropEnt(client, Prop_Data, "m_hViewModel");
            //SetEntProp(vm, Prop_Send, "m_nModelIndex", v_Model);
            SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", w_Model);
        }
    }
}

public OnWeaponDropPost(int client, int weapon)
{
    if(IsValidEntity(weapon))
    {
        char aWeapon[32];
        GetEntityClassname(weapon, aWeapon, sizeof(aWeapon));
        if(strcmp(aWeapon, WEAPON_BASE) == 0) AcceptEntityInput(weapon, "Kill");
    }
}

public Action TTT_OnItemPurchase(int client, int ItemID)
{
    if(ItemID == g_ItemID[0] || ItemID == g_ItemID[1] || ItemID == g_ItemID[2] || ItemID == g_ItemID[3])
    {
        if(GetPlayerWeaponSlot(client, CS_SLOT_GRENADE) == -1)
        {
            g_iPlayer[client].Type = (ItemID == g_ItemID[3]) ? Station_Explode : (ItemID == g_ItemID[2]) ? Station_Health_Plus : Station_Health;
            GivePlayerItem(client, WEAPON_BASE);
        }
        else
        {
            CPrintToChat(client, "%s您已拥有一件主动道具！请使用后再购买！", TTT_TAG);
            return Plugin_Handled;
        }
    }
    return Plugin_Continue;
}

void SpawnStation(int client, float Pos[3])
{
    if (!IsPlayerAlive(client) || g_iPlayer[client].Type == Station_None)
    {
        return;
    }

    int iStation = CreateEntityByName("prop_physics");
    if (iStation != -1)
    {
        StationData Station;
        Station.EntityRef = EntIndexToEntRef(iStation);
        Station.Type = g_iPlayer[client].Type;
        Station.Health = 5;
        Station.HealthAmount = Station.Type == Station_Health_Plus ? 200 : 100;
        g_aStation.PushArray(Station);

        SetEntProp(iStation, Prop_Send, "m_hOwnerEntity", client);
        DispatchKeyValue(iStation, "model", BOX_MODEL);
        DispatchKeyValue(iStation, "classname", "ttt_healthstation");
        if (DispatchSpawn(iStation))
        {
            SDKHook(iStation, SDKHook_OnTakeDamage, OnTakeDamageStation);
            TeleportEntity(iStation, Pos, NULL_VECTOR, NULL_VECTOR);
            Phys_SetMass(iStation, 50.0);
            SetEntProp(iStation, Prop_Data, "m_takedamage", 1, 1);
            /*
            float size[2][3];
            GetEntPropVector(iStation, Prop_Send, "m_vecMins", size[0]);
            GetEntPropVector(iStation, Prop_Send, "m_vecMaxs", size[1]);
            PrintToChatAll("Min: %.1f %.1f %.1f", size[0][0], size[0][1], size[0][2]);
            PrintToChatAll("Max: %.1f %.1f %.1f", size[1][0], size[1][1], size[1][2]);
            */
        }
    }
}

public Action OnTakeDamageStation(int iStation, int &iAttacker, int &inflictor, float &damage, int &damagetype)
{
    if (!IsValidEntity(iStation) || iStation == INVALID_ENT_REFERENCE || iStation <= MaxClients || !IsClientValid(iAttacker))
    {
        return Plugin_Continue;
    }
    if(iAttacker != GetEntProp(iStation, Prop_Send, "m_hOwnerEntity") && TTT_GetClientRole(iAttacker) != TTT_ROLE_DETECTIVE)
    {
        return Plugin_Continue;
    }
    StationData Station;
    for(new i = 0;i < g_aStation.Length;i++)
    {
        g_aStation.GetArray(i, Station);
        if(Station.EntityRef == EntIndexToEntRef(iStation))
        {
            if (Station.Health <= 0 || Station.HealthAmount <= 0)
            {
                AcceptEntityInput(iStation, "Break");
            }
            Station.Health --;
            g_aStation.SetArray(i, Station);
            return Plugin_Continue;
        }
    }
    return Plugin_Continue;
}

void OnUseStation(int client, int iStation)
{
    StationData Station;
    for(new i = 0;i < g_aStation.Length;i++)
    {
        g_aStation.GetArray(i, Station);
        if(Station.EntityRef == EntIndexToEntRef(iStation) && TTT_CanClientUseEntity(client, iStation))
        {
            if(Station.Type == Station_Health || Station.Type == Station_Health_Plus)
            {
                if(Station.HealthAmount <= 0)
                {
                    EmitSoundToAll(SND_CANTHEAL, iStation, 1, 90);
                    break;
                }
                int health = GetClientHealth(client);
                if(health < 100)
                {
                    int after_health = Min(health + Min(Station.HealthAmount, (Station.Type == Station_Health ? 5 : 10)), 100);
                    SetEntityHealth(client, after_health);
                    Station.HealthAmount -= after_health - health;
                    EmitSoundToAll(SND_HEAL, iStation, 1, 90);
                }
            }
            else if(Station.Type == Station_Explode)
            {
                EmitSoundToAll(SND_WARNING, iStation, SNDCHAN_WEAPON, _, SND_CHANGEPITCH, _, SNDPITCH_HIGH);
                SetEntityRenderColor(iStation, 255, 0, 0, 255);
                CreateTimer(1.0, ExplodeStation, EntIndexToEntRef(iStation));
            }
            g_aStation.SetArray(i, Station);
            break;
        }
    }
}

public TTT_OnEntityInfoHUDDisplay(int client, int iEntity)
{
    StationData Station;
    for(new i = 0;i < g_aStation.Length;i++)
    {
        g_aStation.GetArray(i, Station);
        if(Station.EntityRef == EntIndexToEntRef(iEntity))
        {
            SetHudTextParams(-1.0, 0.44, 0.35, 255, 255, 255, 255, 2, 0.0, 0.02, 0.2);
            ShowSyncHudText(client, TTT_GetHudChannel(HUD_CrossHair), "\n\n\n%s\n剩余补给量: %d", Station.Type == Station_Health_Plus ? "生命恢复站[强化]" : "生命恢复站", Station.HealthAmount);
        }
    }
}

public Action TTT_OnGrabEntity(int client, int iEntity)
{
    decl String:ClassName[32];
    GetEntityClassname(iEntity, ClassName, sizeof(ClassName));
    if(strcmp(ClassName, "ttt_healthstation") == 0) return Plugin_Handled;
    return Plugin_Continue;
}

public Action:OnPlayerRunCmd(int client, int &buttons, int &impulse, float vel[3], float angles[3], int &weapon, int &subtype, int &cmdnum, int &tickcount, int &seed, int mouse[2])
{
    if (!IsClientValid(client) || !IsPlayerAlive(client)) return Plugin_Continue;
    if(buttons & IN_USE && GetGameTime() >= g_iPlayer[client].PressCoolDown)
    {
        int target = TTT_GetClientAimTarget(client, false);
        if(IsValidEntity(target))
        {
            OnUseStation(client, target);
            g_iPlayer[client].PressCoolDown = GetGameTime() + 1.0;
        }
    }
    if(buttons & IN_ATTACK)
    {
        new String:ClassName[32];
        GetClientWeapon(client, ClassName, sizeof(ClassName));
        if(strcmp(ClassName, WEAPON_BASE, false) != 0) return Plugin_Continue;

        float vecDir[3], vecStartPos[3], vecEndPos[3], viewang[3];
        GetClientEyePosition(client, vecStartPos);
        GetClientEyePosition(client, vecEndPos);
        GetClientEyeAngles(client, viewang);
        GetAngleVectors(viewang, vecDir, NULL_VECTOR, NULL_VECTOR);
        vecEndPos[0] += vecDir[0] * 55.0;
        vecEndPos[1] += vecDir[1] * 55.0;
        vecEndPos[2] += vecDir[2] * 55.0;
        //Min: -15.2 -17.2 -0.2
        //Max: 17.2 15.3 24.2
        Handle trace = TR_TraceHullFilterEx(vecStartPos, vecEndPos, {-15.2, -17.2, -0.2}, {17.2, 15.3, 24.2}, MASK_SOLID, TraceRayDontHitSelf, client);
        if(TR_GetFraction(trace) == 1.0)
        {
            SpawnStation(client, vecEndPos);
            int weapon_ent = GetPlayerWeaponSlot(client, CS_SLOT_GRENADE);
            TTT_RemoveWeapon(client, weapon_ent, CS_SLOT_GRENADE);
        }
        buttons &= ~IN_ATTACK;
        return Plugin_Changed;
    }
    return Plugin_Continue;
}

public bool:TraceRayDontHitSelf(entity, mask, any:data)
{
	if(entity == data) // Check if the TraceRay hit the itself.
	{
		return false // Don't let the entity be hit
	}
	return true // It didn't hit itself
}

Action ExplodeStation(Handle Timer, int EntityRef)
{
    int iStation = EntRefToEntIndex(EntityRef);
    if(IsValidEntity(iStation))
    {
        new Float:StationPos[3];
        GetEntPropVector(iStation, Prop_Data, "m_vecAbsOrigin", StationPos);
        new StationOwner = GetEntProp(iStation, Prop_Send, "m_hOwnerEntity");
        new ExplosionIndex = CreateEntityByName("env_explosion");
        if (IsValidEntity(ExplosionIndex))
        {
            DispatchKeyValue(ExplosionIndex, "classname", "hegrenade_projectile");
            DispatchKeyValueVector(ExplosionIndex, "origin", StationPos); 
            DispatchKeyValueInt(ExplosionIndex, "spawnflags", 6146);
            SetEntPropEnt(ExplosionIndex, Prop_Send, "m_hOwnerEntity", StationOwner);
            DispatchSpawn(ExplosionIndex);
            DispatchKeyValueInt(ExplosionIndex, "iMagnitude", 250);
            DispatchKeyValueInt(ExplosionIndex, "iRadiusOverride", 550);
            ActivateEntity(ExplosionIndex);
            EmitSoundToAll(SND_EXPLODE, ExplosionIndex, 1, 90);
            AcceptEntityInput(ExplosionIndex, "Explode");
            DispatchKeyValue(ExplosionIndex, "classname", "env_explosion");
            AcceptEntityInput(ExplosionIndex, "Kill");
        }
        AcceptEntityInput(iStation, "Kill");
    }
    return Plugin_Continue;
}

void CleanUpStation()
{
    LoopValidClients(i)
    {
        g_iPlayer[i].Type = Station_None;
        g_iPlayer[i].PressCoolDown = GetGameTime();
    }
    g_aStation.Clear();
}