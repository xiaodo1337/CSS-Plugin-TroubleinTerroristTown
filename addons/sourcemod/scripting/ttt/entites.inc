InitEntities(int entity)
{
    if(IsValidEntity(entity))
    {
        char classname[64];
        GetEntityClassname(entity, classname, sizeof(classname));
        if(StrEqual(classname, "func_buyzone"))
        {
            AcceptEntityInput(entity, "Kill");
        }
        if(StrEqual(classname, "hostage_entity"))
        {
            AcceptEntityInput(entity, "Kill");
        }
        if(StrEqual(classname, "func_hostage_rescue"))
        {
            AcceptEntityInput(entity, "Kill");
        }
        if(StrEqual(classname, "func_bomb_target"))
        {
            AcceptEntityInput(entity, "Kill");
        }
        if(StrEqual(classname, "info_player_counterterrorist"))
        {
            SetEntPropString(entity, Prop_Data, "m_iClassname", "info_player_terrorist");
        }
        if(StrEqual(classname, "prop_physics") || StrEqual(classname, "prop_physics_multiplayer") || StrEqual(classname, "func_physbox"))
        {
            SetEntProp(entity, Prop_Send, "m_CollisionGroup", COLLISION_GROUP_NONE);
        }
    }
}

public OnEntitySpawned(entity)
{
    new Owner = GetEntPropEnt(entity, Prop_Send, "m_hOwner");
    if (IsClientValid(Owner))
    {
        if (GetEntProp(entity, Prop_Send, "m_nViewModelIndex") == 0)
        {
            g_Client[Owner].ClientVM[0] = entity;
        }
        else if (GetEntProp(entity, Prop_Send, "m_nViewModelIndex") == 1)
        {
            g_Client[Owner].ClientVM[1] = entity;
        }
    }
}