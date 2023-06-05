int CreateRoleIcon(int client, int role)
{
    ClearRoleIcon(client);

    if (role != TTT_ROLE_DETECTIVE && role != TTT_ROLE_TRAITOR)
    {
        return -1;
    }

    float origin[3];
    GetClientAbsOrigin(client, origin);
    origin[2] = origin[2] + 80.0;

    int ent = CreateEntityByName("env_sprite");
    if (!ent)
    {
        return -1;
    }

    char sBuffer[PLATFORM_MAX_PATH];

    if (role == TTT_ROLE_DETECTIVE)
    {
        Format(sBuffer, sizeof(sBuffer), "ttt/icon/%s.vmt", DetectiveIcon);
    }
    else if (role == TTT_ROLE_TRAITOR)
    {
        Format(sBuffer, sizeof(sBuffer), "ttt/icon/%s.vmt", TraitorIcon);
    }

    char iTarget[16];
    Format(iTarget, sizeof(iTarget), "client%d", client);
    DispatchKeyValue(client, "targetname", iTarget);
    SetEntProp(ent, Prop_Send, "m_hOwnerEntity", client);
    DispatchKeyValue(ent, "model", sBuffer);
    DispatchKeyValue(ent, "classname", "env_sprite");
    DispatchKeyValue(ent, "spawnflags", "1");
    DispatchKeyValue(ent, "scale", "0.08");
    DispatchKeyValue(ent, "rendermode", "1");
    DispatchKeyValue(ent, "rendercolor", "255 255 255");
    DispatchSpawn(ent);
    TeleportEntity(ent, origin, NULL_VECTOR, NULL_VECTOR);
    SetVariantString(iTarget);
    AcceptEntityInput(ent, "SetParent", ent, ent);

    if(role == TTT_ROLE_TRAITOR)
    {
        SDKHook(ent, SDKHook_SetTransmit, Hook_SetTransmitIconT);
    }
    
    return EntIndexToEntRef(ent);
}

ApplyRoleIcon(int client)
{
    if (IsClientValid(client) && IsPlayerAlive(client))
    {
        g_Client[client].RoleIcon = CreateRoleIcon(client, g_Client[client].Role);
    }
}

ClearRoleIcon(int client)
{
    if (g_Client[client].RoleIcon == 0)
    {
        g_Client[client].RoleIcon = -1;
    }

    int entity = EntRefToEntIndex(g_Client[client].RoleIcon);

    if (IsValidEdict(entity))
    {
        SDKUnhook(entity, SDKHook_SetTransmit, Hook_SetTransmitIconT);
        AcceptEntityInput(entity, "Kill");
    }

    g_Client[client].RoleIcon = -1;
}

int CreateCallIcon(int body)
{
    if(!IsValidEntity(body)) return -1;
    ClearCallIcon(body);
    float origin[3];
    GetEntPropVector(body, Prop_Data, "m_vecOrigin", origin);
    origin[2] = origin[2] + 15.0;

    int ent = CreateEntityByName("env_sprite");
    if (!ent) return -1;
    char sBuffer[PLATFORM_MAX_PATH];
    Format(sBuffer, sizeof(sBuffer), "ttt/icon/%s.vmt", CallIcon);
    
    char iTarget[16];
    Format(iTarget, sizeof(iTarget), "deadbody%d", body);
    DispatchKeyValue(body, "targetname", iTarget);

    DispatchKeyValue(ent, "model", sBuffer);
    DispatchKeyValue(ent, "classname", "env_sprite");
    DispatchKeyValue(ent, "spawnflags", "1");
    DispatchKeyValue(ent, "scale", "0.3");
    DispatchKeyValue(ent, "rendermode", "1");
    DispatchKeyValue(ent, "rendercolor", "255 255 255");
    DispatchSpawn(ent);
    TeleportEntity(ent, origin, NULL_VECTOR, NULL_VECTOR);
    SetVariantString(iTarget);
    AcceptEntityInput(ent, "SetParent", ent, ent);

    SDKHook(ent, SDKHook_SetTransmit, Hook_SetTransmitIconCall);
    
    return EntIndexToEntRef(ent);
}

ClearCallIcon(int body_entity)
{
    if(g_aRagdoll.Length == 0) return;
    Ragdoll body;
    for(int i;i < g_aRagdoll.Length;i++)
    {
        g_aRagdoll.GetArray(i, body, sizeof(body));
        int body_index = EntRefToEntIndex(body.EntityRef);
        if(!IsValidEntity(body_index))
        {
            g_aRagdoll.Erase(i);
            continue;
        }
        if(body_index == body_entity)
        {
            if(body.CallIcon == 0) body.CallIcon = -1; //防止服务器崩溃
            int call_entity = EntRefToEntIndex(body.CallIcon);
            if (IsValidEdict(call_entity))
            {
                SDKUnhook(call_entity, SDKHook_SetTransmit, Hook_SetTransmitIconCall);
                AcceptEntityInput(call_entity, "Kill");
            }
            body.CallIcon = -1;
            g_aRagdoll.SetArray(i, body, sizeof(body));
            return;
        }
    }
}

int CreateDNAIcon(int client)
{
    ClearDNAIcon(INVALID_HANDLE, client);
    if (!IsClientValid(client) || (IsClientValid(client) && !IsPlayerAlive(client)))
    {
        return -1;
    }
    float origin[3];
    GetClientAbsOrigin(client, origin);
    origin[2] = origin[2] + 30.0;

    int ent = CreateEntityByName("env_sprite");
    if (!ent) return -1;

    char sBuffer[PLATFORM_MAX_PATH];
    Format(sBuffer, sizeof(sBuffer), "ttt/icon/%s.vmt", DNAIcon);

    char iTarget[16];
    Format(iTarget, sizeof(iTarget), "killer%d", client);
    DispatchKeyValue(client, "targetname", iTarget);

    DispatchKeyValue(ent, "model", sBuffer);
    DispatchKeyValue(ent, "classname", "env_sprite");
    DispatchKeyValue(ent, "spawnflags", "1");
    DispatchKeyValue(ent, "scale", "0.15");
    DispatchKeyValue(ent, "rendermode", "1");
    DispatchKeyValue(ent, "rendercolor", "255 255 255");
    SetEntProp(ent, Prop_Send, "m_hOwnerEntity", client);
    DispatchSpawn(ent);
    TeleportEntity(ent, origin, NULL_VECTOR, NULL_VECTOR);
    SetVariantString(iTarget);
    AcceptEntityInput(ent, "SetParent", ent, ent);
    SDKHook(ent, SDKHook_SetTransmit, Hook_SetTransmitIconDNA);
    g_Client[client].DNAIconRemoveTimer = CreateTimer(ttt_dna_visible_time.FloatValue, ClearDNAIcon, client);
    return EntIndexToEntRef(ent);
}

stock Action:ClearDNAIcon(Handle Timer, int client)
{
    TTT_EndTimer(g_Client[client].DNAIconRemoveTimer);
    g_Client[client].DNAIconRemoveTimer = INVALID_HANDLE;
    if (g_Client[client].DNAIcon == 0)
    {
        g_Client[client].DNAIcon = -1;
    }
    int entity = EntRefToEntIndex(g_Client[client].DNAIcon);

    if (IsValidEdict(entity))
    {
        SDKUnhook(entity, SDKHook_SetTransmit, Hook_SetTransmitIconDNA);
        AcceptEntityInput(entity, "Kill");
    }

    g_Client[client].DNAIcon = -1;
}

stock Action:Hook_SetTransmitIconT(int entity, int client)
{
    if (IsClientValid(client))
    {
        if (!IsPlayerAlive(client) || g_Client[client].Role == TTT_ROLE_TRAITOR || (IsGameEnd && IsGameStart) || g_Client[client].IconLeak)
        {
            return Plugin_Continue;
        }
    }
    return Plugin_Handled;
}

stock Action:Hook_SetTransmitIconCall(int entity, int client)
{
    if (IsClientValid(client))
    {
        if(g_Client[client].Role == TTT_ROLE_DETECTIVE) return Plugin_Continue;
    }
    return Plugin_Handled;
}

stock Action:Hook_SetTransmitIconDNA(int entity, int client)
{
    if (IsClientValid(client) && IsPlayerAlive(client))
    {
        if(g_Client[client].Role == TTT_ROLE_DETECTIVE && g_Client[client].DNACalled == GetEntProp(entity, Prop_Send, "m_hOwnerEntity")) return Plugin_Continue;
    }
    return Plugin_Handled;
}