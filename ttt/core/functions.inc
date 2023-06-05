stock GetAliveClientCounts()
{
    int aliveclient;
    for(int client = 1;client <= MaxClients;client++)
    {
        if(IsClientInGame(client) && IsPlayerAlive(client))
            aliveclient++;
    }
    return aliveclient;
}

//将符合条件的人推入动态数组，返回动态数组大小
stock CreateClientList(&ArrayList:Array, int Role)
{
    Array = new ArrayList();
    
    for (new client = 1;client <= MaxClients;client++)
    {
        if (!IsClientInGame(client) || !IsPlayerAlive(client))
        {
            continue;
        }
        
        if (g_Client[client].Role == Role)
        {
            Array.Push(client);
        }
    }
    
    return Array.Length;
}

stock bool:IsPlayersInDistance(client, target)
{
    float origin[2][3], distance;
    GetClientAbsOrigin(client, origin[0]);
    GetClientAbsOrigin(target, origin[1]);
    distance = GetVectorDistance(origin[0], origin[1]);
    if(distance <= ttt_show_hud_distance.FloatValue) return true;
    return false;
}

stock bool CanClientUseEntity(int client, int iEntity)
{
    if(IsClientValid(client) && IsPlayerAlive(client) && IsValidEntity(iEntity))
    {
        static float fEntityPosition[3], fMyPosition[3];
        GetClientAbsOrigin(client, fMyPosition);
        GetEntPropVector(iEntity, Prop_Data, "m_vecAbsOrigin", fEntityPosition);
        if(GetVectorDistance(fMyPosition, fEntityPosition) <= ttt_entity_use_distance.FloatValue)
        {
            return true;
        }
    }
    return false
}

stock bool IsEntityBody(int iEntity)
{
    if(!IsValidEntity(iEntity)) return false;
    char classname[32];
    GetEntityClassname(iEntity, classname, sizeof(classname));
    return StrEqual(classname, "prop_ragdoll", false);
}