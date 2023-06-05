stock InitPlayerWeapon(client)
{
    if(IsClientValid(client) && IsPlayerAlive(client)) {
        int weapon_ent;
        weapon_ent = GetPlayerWeaponSlot(client, CS_SLOT_GRENADE);
        while (weapon_ent != -1)
        {
            TTT_RemoveWeapon(client, weapon_ent, CS_SLOT_GRENADE);
            weapon_ent = GetPlayerWeaponSlot(client, CS_SLOT_GRENADE);
        }
        for(new j;j <= CS_SLOT_C4;j++) {
            weapon_ent = GetPlayerWeaponSlot(client, j);
            TTT_RemoveWeapon(client, weapon_ent, j);
        }
        GivePlayerItem(client, "weapon_c4");
        GivePlayerItem(client, "weapon_fiveseven");
        GivePlayerItem(client, "weapon_knife");
    }
}

stock WeaponOnWeaponSwitchPost(int client, int weapon)
{
    GetEntityClassname(weapon, Buffer, sizeof(Buffer));
    if(StrEqual(Buffer, "weapon_c4"))
    {
        int vm = GetEntPropEnt(client, Prop_Data, "m_hViewModel");
        SetEntProp(vm, Prop_Send, "m_nModelIndex", V_PhysicsBar);
        SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", W_PhysicsBar);
    }
}

stock Action:WeaponOnWeaponDrop(int client, int weapon)
{
    if(IsValidEntity(weapon))
    {
        GetEntityClassname(weapon, Buffer, sizeof(Buffer));
        if(StrEqual(Buffer, "weapon_c4")) return Plugin_Handled;
        return Plugin_Continue;
    }
}

stock WeaponOnWeaponDropPost(int client, int weapon)
{
    if(IsValidEntity(weapon))
    {
        GetEntityClassname(weapon, Buffer, sizeof(Buffer));
        if(StrEqual(Buffer, "weapon_c4")) RequestFrame(ChnageWorldWeaponModel, weapon);
    }
}

stock ChnageWorldWeaponModel(int weapon)
{
    if(IsValidEntity(weapon)) SetEntProp(weapon, Prop_Send, "m_iWorldModelIndex", W_PhysicsBar);
}

stock InitWeaponsFireSpeed(int client)
{
    if(IsClientValid(client) && IsPlayerAlive(client))
    {
        int iWeapon = GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon");
        if(IsValidEntity(iWeapon))
        {
            GetEntityClassname(iWeapon, Buffer, sizeof(Buffer));
            if (StrEqual(Buffer, "weapon_fiveseven") || StrEqual(Buffer, "weapon_usp"))
            {
                SetEntPropFloat(iWeapon, Prop_Send, "m_flNextPrimaryAttack", GetGameTime() + 0.25);
            }
            if (StrEqual(Buffer, "weapon_elite"))
            {
                SetEntPropFloat(iWeapon, Prop_Send, "m_flNextPrimaryAttack", GetGameTime() + 0.18);
            }
            if (StrEqual(Buffer, "weapon_sg552"))
            {
                SetEntPropFloat(iWeapon, Prop_Send, "m_flNextPrimaryAttack", GetGameTime() + 0.12);
            }
        }
    }
}

stock FindClientViewModel(int client)
{
    g_Client[client].ClientVM[0] = GetEntPropEnt(client, Prop_Send, "m_hViewModel");
    new PVM = -1;
    while ((PVM = FindEntityByClassname(PVM, "predicted_viewmodel")) != -1)
    {
        if (GetEntPropEnt(PVM, Prop_Send, "m_hOwner") == client)
        {
            if (GetEntProp(PVM, Prop_Send, "m_nViewModelIndex") == 1)
            {
                g_Client[client].ClientVM[1] = PVM;
                break;
            }
        }
    }
}

stock HideHiddenViewModel(int client)
{
    new EntEffects = GetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_fEffects");
    EntEffects |= EF_NODRAW;
    SetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_fEffects", EntEffects);
}