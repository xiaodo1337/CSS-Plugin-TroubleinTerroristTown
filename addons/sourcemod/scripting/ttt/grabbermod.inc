void GrabPhysicsEntity(int client)
{
    //if (!IsGameStart) return;
    int vm = GetEntPropEnt(client, Prop_Data, "m_hViewModel");
    if(IsValidEntity(vm))
    {
        SetEntProp(vm, Prop_Data, "m_nSequence", 2);
        CreateTimer(0.67, ResetWeaponAnim, client);
    }
    if(IsGrabing(client))
    {
        Command_UnGrab(client);
        return;
    }
    int iEntity = -1;
    iEntity = GetObject(client, false);
    if (iEntity == -1) return;
    
    float fEntityPos[3], fClientPos[3];
    
    GetEntPropVector(iEntity, Prop_Send, "m_vecOrigin", fEntityPos);
    GetClientEyePosition(client, fClientPos);
    if (GetVectorDistance(fEntityPos, fClientPos, false) > ttt_grabbermod_grab_distance.FloatValue || Phys_GetMass(iEntity) > 50.0) return;

    LoopValidClients(i)
    {
        if (i != client && g_Client[client].Object == EntIndexToEntRef(iEntity))
        {
            return;
        }
    }

    char sName[128];
    GetEdictClassname(iEntity, sName, sizeof(sName));
    if (StrContains(sName, "door", false) != -1 || StrContains(sName, "button", false) != -1 || StrContains(sName, "_player", false) != -1)
    {
        return;
    }

    Action res;
    Call_StartForward(g_fwOnGrabEntity);
    Call_PushCell(client);
    Call_PushCell(iEntity);
    Call_Finish(res);
    if(res >= Plugin_Handled)
    {
        return;
    }

    if (GetEntityMoveType(iEntity) == MOVETYPE_PUSH)
    {
        return;
    }
    
    if (StrEqual(sName, "prop_physics") || StrEqual(sName, "prop_physics_multiplayer") || StrEqual(sName, "func_physbox") || StrEqual(sName, "prop_physics_respawnable"))
    {
        if (IsValidEntity(iEntity))
        {
            iEntity = ReplacePhysicsEntity(iEntity);

            SetEntPropEnt(iEntity, Prop_Data, "m_hPhysicsAttacker", client);
            SetEntPropFloat(iEntity, Prop_Data, "m_flLastPhysicsInfluenceTime", GetEngineTime());
        }
    }
    
    if (StrContains(sName, "ragdoll", false) != -1)
    {
        // This check should fix this crash bug: https://youtu.be/GbpkerPoO7g
        if (GetVectorDistance(fEntityPos, fClientPos, false) > 150.0)
        {
            return;
        }
        float fClientAngle[3];
        GetClientEyeAngles(client, fClientAngle);
        Handle trace = TR_TraceRayFilterEx(fClientPos, fClientAngle, MASK_SHOT, RayType_Infinite, TraceIgnoreMe, client);
        if(TR_DidHit(trace))
        {
            if(IsValidEntity(TR_GetEntityIndex(trace)))
            {
                float avec[2][3];
                TR_GetPlaneNormal(trace, avec[0]);
                TR_GetEndPosition(avec[1], trace);
                Phys_ApplyForceOffset(TR_GetEntityIndex(trace), avec[0], avec[1]);
            }
        }
    }

    if (GetEntityMoveType(iEntity) == MOVETYPE_NONE)
    {
        SetEntityMoveType(iEntity, MOVETYPE_VPHYSICS);
        Phys_EnableMotion(iEntity, true);
    }

    g_Client[client].Object = EntIndexToEntRef(iEntity);
    g_Client[client].Distance = GetVectorDistance(fEntityPos, fClientPos, false);
    //Phys_SetVelocity(iEntity, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0});      //VELOCITY清空
    TeleportEntity(iEntity, NULL_VECTOR, NULL_VECTOR, {0.0, 0.0, 0.0});
    SDKHook(client, SDKHook_PreThink, OnPreThink);
    //音效
}

int GetObject(int client, bool hitSelf=true)
{
    int iEntity = -1;
    if (IsClientValid(client))
    {
        if (IsGrabing(client))
        {
            iEntity = EntRefToEntIndex(g_Client[client].Object);
            return iEntity;
        }
        iEntity = TraceToEntity(client);
        if (IsValidEntity(iEntity))
        {
            char sName[64];
            GetEdictClassname(iEntity, sName, sizeof(sName));
            if (StrEqual(sName, "worldspawn"))
            {
                if (hitSelf) iEntity = client;
                else iEntity = -1;
            }
        }
        else iEntity = -1;
    }
    return iEntity;
}

stock TraceToEntity(int client)
{
    float fEyePos[3], fEyeAngle[3];
    GetClientEyePosition(client, fEyePos);
    GetClientEyeAngles(client, fEyeAngle);
    TR_TraceRayFilter(fEyePos, fEyeAngle, MASK_PLAYERSOLID, RayType_Infinite, TraceASDF, client);
    if (TR_DidHit(null))
    {
        return TR_GetEntityIndex(null);
    }
    return -1;
}

stock bool TraceASDF(int entity, int mask)
{
    return entity >= MaxClients;
}

int ReplacePhysicsEntity(int iEntity)
{
    float fOrigin[3], fAngle[3];
    GetEntPropVector(iEntity, Prop_Send, "m_vecOrigin", fOrigin);
    GetEntPropVector(iEntity, Prop_Send, "m_angRotation", fAngle);

    Phys_Wake(iEntity);
    Phys_EnableMotion(iEntity, true);
    Phys_EnableDrag(iEntity, true);
    DispatchKeyValue(iEntity, "physdamagescale", "0.0");
    TeleportEntity(iEntity, fOrigin, fAngle, NULL_VECTOR);
    SetEntityMoveType(iEntity, MOVETYPE_VPHYSICS);

    return iEntity;
}

public void OnPreThink(int i)
{
    float vecDir[3], vecPos[3], vecVel[3], viewang[3];
    if (IsPlayerAlive(i))
    {
        if (IsGrabing(i))
        {
            GetClientEyeAngles(i, viewang);
            GetAngleVectors(viewang, vecDir, NULL_VECTOR, NULL_VECTOR);
            GetClientEyePosition(i, vecPos);
            vecPos[0] += vecDir[0] * g_Client[i].Distance;
            vecPos[1] += vecDir[1] * g_Client[i].Distance;
            vecPos[2] += vecDir[2] * g_Client[i].Distance;
            int iEntity = EntRefToEntIndex(g_Client[i].Object);
            GetEntPropVector(iEntity, Prop_Send, "m_vecOrigin", vecDir);
            SubtractVectors(vecPos, vecDir, vecVel);
            GetClientEyePosition(i, vecPos);
            decl String:classname[32];
            GetEntityClassname(iEntity, classname, sizeof(classname));
            if(StrContains(classname, "ragdoll", false) == -1)
            {
                if(GetVectorLength(vecVel) > ttt_grabbermod_grab_velocity_limit.FloatValue || GetVectorDistance(vecDir, vecPos, false) > ttt_grabbermod_grab_distance.FloatValue)
                {
                    Command_UnGrab(i);
                    Phys_SetVelocity(iEntity, vecVel, {0.0, 0.0, 0.0});
                    return;
                }
            }
            else if(GetVectorDistance(vecDir, vecPos, false) > ttt_grabbermod_grab_distance.FloatValue)
            {
                Command_UnGrab(i);
                Phys_SetVelocity(iEntity, vecVel, {0.0, 0.0, 0.0});
                return;
            }

            if(StrContains(classname, "ragdoll", false) > -1) ScaleVector(vecVel, 70.0);
            else ScaleVector(vecVel, 15.0);

            //TeleportEntity(iEntity, NULL_VECTOR, NULL_VECTOR, vecVel);   //导致布娃娃无法移动
            Phys_SetVelocity(iEntity, vecVel, {0.0, 0.0, 0.0});
        }
        else
        {
            Command_UnGrab(i);
        }
    }
}

stock bool TraceIgnoreMe(int entity, int mask, int client)
{
    return entity != client && entity > MAXPLAYERS;
}

void Command_UnGrab(int client)
{
    if(IsGrabing(client))
    {
        char sName[128];
        GetEdictClassname(EntRefToEntIndex(g_Client[client].Object), sName, sizeof(sName));
        if (StrEqual(sName, "prop_physics") || StrEqual(sName, "prop_physics_multiplayer") || StrEqual(sName, "func_physbox") || StrEqual(sName, "prop_physics"))
        {
            SetEntPropEnt(EntRefToEntIndex(g_Client[client].Object), Prop_Data, "m_hPhysicsAttacker", 0);
        }
        g_Client[client].Object = -1;
        SDKUnhook(client, SDKHook_PreThink, OnPreThink);
    }
}

stock Action:ResetWeaponAnim(Handle Timer, int client)
{
    GetClientWeapon(client, Buffer, sizeof(Buffer));
    if(StrEqual(Buffer, "weapon_c4"))
    {
        int vm = GetEntPropEnt(client, Prop_Data, "m_hViewModel");
        if(IsValidEntity(vm))
        {
            SetEntProp(vm, Prop_Data, "m_nSequence", 0);
        }
    }
    return Plugin_Continue;
}

bool IsGrabing(int client)
{
    int iObject = EntRefToEntIndex(g_Client[client].Object);
    if(iObject > 0 && IsValidEntity(iObject))
    {
        return true;
    }
    return false;
}