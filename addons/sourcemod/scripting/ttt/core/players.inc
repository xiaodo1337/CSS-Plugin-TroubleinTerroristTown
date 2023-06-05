stock ResetPlayer(int client)
{
    InitPlayerRole(client);
    InitPlayerWeapon(client);
    RestoreKarma(client);
    SetEntProp(client, Prop_Send, "m_bHasHelmet", 0);
    SetEntProp(client, Prop_Send, "m_ArmorValue", 0);

    g_Client[client].Credits = 0;
    g_Client[client].CallTime = 3;
    g_Client[client].VoiceChannel = VOICE_CHANNEL_PUBLIC;
    g_Client[client].DNACalled = -1;
    g_Client[client].DNAProcess = 0;
    g_Client[client].Leak = false;
}

stock InitPlayerVars(int client)
{
    InitPlayerRole(client);
    g_Client[client].PressTime = 0.0;
    g_Client[client].Karma = ttt_karma_default.IntValue;

    g_Client[client].CallTime = 3;
    g_Client[client].VoiceChannel = VOICE_CHANNEL_PUBLIC;
    g_Client[client].DNACalled = -1;
    g_Client[client].DNAProcess = 0;
}

stock SetPlayerStatus(client)
{
    SetEntProp(client, Prop_Data, "m_iFrags", g_Client[client].Frags[0]);
    SetEntProp(client, Prop_Send, "m_iAccount", 0);
    SetEntProp(client, Prop_Send, "m_iAddonBits", 0);
}

stock MoveSpecPlayers()
{
    for(int client = 1;client <= MaxClients;client++)
    {
        if(IsClientValid(client) && (GetClientTeam(client) == CS_TEAM_NONE || GetClientTeam(client) == CS_TEAM_SPECTATOR))
        {
            CS_SwitchTeam(client, CS_TEAM_T);
            CS_RespawnPlayer(client);
            ResetPlayer(client);
        }
    }
}

public OnClientThinkPost(client)
{
    SetPlayerStatus(client);
    if(g_aViewModel.Length <= 0) return;
    static OldWeapon[MAXPLAYERS + 1];
    static OldSequence[MAXPLAYERS + 1];
    static Float:OldCycle[MAXPLAYERS + 1];
    decl String:ClassName[30];
    new WeaponIndex;

    if (!IsPlayerAlive(client))
    {
        new spec = GetEntPropEnt(client, Prop_Send, "m_hObserverTarget");
        if (spec != -1)
        {
            WeaponIndex = GetEntPropEnt(spec, Prop_Send, "m_hActiveWeapon");
            if(IsValidEdict(WeaponIndex))
            {
                GetEdictClassname(WeaponIndex, ClassName, sizeof(ClassName));
                ViewModel ViewModelArray;
                for(int i;i < g_aViewModel.Length;i++)
                {
                    g_aViewModel.GetArray(i, ViewModelArray, sizeof(ViewModelArray));
                    if (strcmp(ClassName, ViewModelArray.weapon_base, false) == 0)
                    {
                        SetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_nModelIndex", ViewModelArray.model_index);
                    }
                }
            }
        }
        return;
    }
    
    WeaponIndex = GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon");
    new Sequence = GetEntProp(g_Client[client].ClientVM[0], Prop_Send, "m_nSequence");
    new Float:Cycle = GetEntPropFloat(g_Client[client].ClientVM[0], Prop_Data, "m_flCycle");
    
    if (!IsValidEdict(WeaponIndex))
    {
        new EntEffects = GetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_fEffects");
        EntEffects |= EF_NODRAW;
        SetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_fEffects", EntEffects);
        g_Client[client].IsCustom = false;
        OldWeapon[client] = WeaponIndex;
        OldSequence[client] = Sequence;
        OldCycle[client] = Cycle;   
        return;
    }
    
    //just stuck the weapon switching in here aswell instead of a separate hook
    //只是把武器切换也放在这里，而不是一个单独的钩子
    if (WeaponIndex != OldWeapon[client])
    {
        GetEdictClassname(WeaponIndex, ClassName, sizeof(ClassName));
        bool match = false;
        ViewModel ViewModelArray;
        for(int i;i < g_aViewModel.Length;i++)
        {
            g_aViewModel.GetArray(i, ViewModelArray, sizeof(ViewModelArray));
            if (strcmp(ClassName, ViewModelArray.weapon_base, false) == 0)
            {
                //隐藏原ViewModel
                new EntEffects = GetEntProp(g_Client[client].ClientVM[0], Prop_Send, "m_fEffects");
                EntEffects |= EF_NODRAW;
                SetEntProp(g_Client[client].ClientVM[0], Prop_Send, "m_fEffects", EntEffects);
                //显示被隐藏的ViewModel
                EntEffects = GetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_fEffects");
                EntEffects &= ~EF_NODRAW;
                SetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_fEffects", EntEffects);
                //set model and copy over props from viewmodel to used viewmodel
                SetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_nModelIndex", ViewModelArray.model_index);
                SetEntPropEnt(g_Client[client].ClientVM[1], Prop_Send, "m_hWeapon", GetEntPropEnt(g_Client[client].ClientVM[0], Prop_Send, "m_hWeapon"));
                SetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_nSequence", GetEntProp(g_Client[client].ClientVM[0], Prop_Send, "m_nSequence"));
                SetEntPropFloat(g_Client[client].ClientVM[1], Prop_Send, "m_flPlaybackRate", GetEntPropFloat(g_Client[client].ClientVM[0], Prop_Send, "m_flPlaybackRate"));
                g_Client[client].IsCustom = true;
                match = true;
            }
        }
        if(!match)
        {
            //如果当前所持武器并不需要用到未使用的VM模型，则隐藏
            new EntEffects = GetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_fEffects");
            EntEffects |= EF_NODRAW;
            SetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_fEffects", EntEffects);
            
            g_Client[client].IsCustom = false;
        }
    }
    else
    {
        if (g_Client[client].IsCustom)
        {
            //copy the animation stuff from the viewmodel to the used one every frame
            SetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_nSequence", GetEntProp(g_Client[client].ClientVM[0], Prop_Send, "m_nSequence"));
            SetEntPropFloat(g_Client[client].ClientVM[1], Prop_Send, "m_flPlaybackRate", GetEntPropFloat(g_Client[client].ClientVM[0], Prop_Send, "m_flPlaybackRate"));
            
            if ((Cycle < OldCycle[client]) && (Sequence == OldSequence[client]))
            {
                SetEntProp(g_Client[client].ClientVM[1], Prop_Send, "m_nSequence", 0);
            }
        }
    }
    //hide viewmodel a frame after spawning
    if (g_Client[client].SpawnCheck)
    {
        g_Client[client].SpawnCheck = false;
        if (g_Client[client].IsCustom)
        {
            new EntEffects = GetEntProp(g_Client[client].ClientVM[0], Prop_Send, "m_fEffects");
            EntEffects |= EF_NODRAW;
            SetEntProp(g_Client[client].ClientVM[0], Prop_Send, "m_fEffects", EntEffects);
        }
    }
    
    OldWeapon[client] = WeaponIndex;
    OldSequence[client] = Sequence;
    OldCycle[client] = Cycle;
}