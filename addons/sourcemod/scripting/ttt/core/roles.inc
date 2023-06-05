stock InitPlayerRole(client)
{
    g_Client[client].Role = TTT_ROLE_NONE;
}

stock InitRoleNums()
{
    DetectiveNums = 0;
    InnocentNums = 0;
    TraitorNums = 0;
}

stock SetClientRole(int client, int Role)
{
    g_Client[client].Role = Role;
    if(Role == TTT_ROLE_DETECTIVE) {
        GivePlayerItem(client, "item_assaultsuit");
        SetEntProp(client, Prop_Send, "m_ArmorValue", 20);
        g_Client[client].Credits = 2;
    } else if(Role == TTT_ROLE_TRAITOR) {
        g_Client[client].Credits = 2;
    }
    /*
    new offset1 = FindSendPropInfo("CBaseCombatWeapon", "m_iClip1");
    new offset2 = FindSendPropInfo("CBasePlayer", "m_iAmmo");
    if(Role == TTT_ROLE_DETECTIVE)
    {
        new weapon = GivePlayerItem(client, "weapon_ump45");
        GivePlayerItem()
        PrintToChat(client, "1: %d | 2: %d", GetEntData(weapon, offset1), GetEntData(client, offset2))
        //SetEntProp(weapon, Prop_Send, "m_iClip1", 5);
        //SetEntData(weapon, offset, 50, _, true);
    }
    else if(Role == TTT_ROLE_TRAITOR)
    {
        GivePlayerItem(client, "weapon_hegrenade");
        //new weapon = GivePlayerItem(client, "weapon_awp");
        //SetEntProp(weapon, Prop_Send, "m_iClip1", 5);
        //SetEntData(weapon, offset, 5, _, true);
    }
    */
    CS_SwitchTeam(client, Role == TTT_ROLE_DETECTIVE ? CS_TEAM_CT : CS_TEAM_T);
    ApplyRoleIcon(client);
    //PrintToChat(client, "你已被选为%s", Role == TTT_ROLE_NONE ? "未选择" : Role == TTT_ROLE_INNOCENT ? "平民" : Role == TTT_ROLE_DETECTIVE ? "侦探" : "叛徒");
}

stock RoleNumsUpdate()
{
    DetectiveNums = 0;
    InnocentNums = 0;
    TraitorNums = 0;
    for(int client = 1;client <= MaxClients;client++)
    {
        if(!IsClientValid(client) || !IsPlayerAlive(client)) continue;
        if(g_Client[client].Role == TTT_ROLE_DETECTIVE) DetectiveNums ++;
        else if(g_Client[client].Role == TTT_ROLE_TRAITOR) TraitorNums ++;
        else if(g_Client[client].Role == TTT_ROLE_INNOCENT) InnocentNums ++;
    }
}

void GetRoleName(role, char[] name, int length = 16)
{
    if (role == TTT_ROLE_INNOCENT) Format(name, length, "平民");
    else if (role == TTT_ROLE_DETECTIVE) Format(name, length, "侦探");
    else if (role == TTT_ROLE_TRAITOR) Format(name, length, "叛徒");
    else Format(name, length, "未选择");
}

stock GetRoleColor(role)
{
    if(role == TTT_ROLE_DETECTIVE) return COLOR_BLUE;
    else return COLOR_RED;
}

stock TellVictimAttackerRole(victim, attacker)
{
    if(IsClientValid(attacker) && victim != attacker)
    {
        char RoleName[16];
        GetRoleName(g_Client[attacker].Role, RoleName, sizeof(RoleName));
        CPrintToChat(victim, "%s杀死你的玩家是名 \x07%X%s", TTT_TAG, GetRoleColor(g_Client[attacker].Role), RoleName);
    }
}