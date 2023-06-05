InitEvents()
{
    HookEvent("round_start", Event_RoundStart_Pre, EventHookMode_Pre);
    HookEvent("round_start", Event_RoundStart);
    HookEvent("round_freeze_end", Event_RoundFreezeEnd);
    HookEvent("round_end", Event_RoundEnd);
    HookEvent("player_spawn", Event_PlayerSpawn_Pre, EventHookMode_Pre);
    HookEvent("player_spawn", Event_PlayerSpawn);
    HookEvent("player_blind", Event_PlayerBlind, EventHookMode_Post);
    HookEvent("player_hurt", Event_PlayerHurt);
    HookEvent("player_death", Event_PlayerDeath_Pre, EventHookMode_Pre);
    HookEvent("player_jump", Event_PlayerJump);
    HookEvent("weapon_fire", Event_WeaponFire);
}

public Action Event_RoundStart_Pre(Event event, const char[] name, bool dontBroadcast)
{
    StopAllSound();
    InitGameParams();
    InitRoleNums();
    BodiesOnRoundStart();
    DnaListOnRoundStart();
    MoveSpecPlayers();
    return Plugin_Continue;
}

public Action Event_RoundStart(Event event, const char[] name, bool dontBroadcast)
{
    TTT_EndAllTimer();
    CheckGameCondition(INVALID_HANDLE);
    LoopValidClients(client)
    {
        if(IsPlayerAlive(client))
        {
            ResetPlayer(client);
        }
    }
    ResetPlayersVoice();
    InitShopItemsBuyLimit();
    return Plugin_Continue;
}

public Action Event_RoundFreezeEnd(Event event, const char[] name, bool dontBroadcast)
{
    InitGameStart();        //倒计时开局
    return Plugin_Continue;
}

public Action Event_RoundEnd(Event event, const char[] name, bool dontBroadcast)
{
    IsGameEnd = true;       //游戏结束
    ResetPlayersVoice();
    UpdatePlayersFrags();
    return Plugin_Continue;
}

public Action Event_PlayerSpawn_Pre(Event event, const char[] name, bool dontBroadcast)
{
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    if(IsClientValid(client))
    {
        CS_SwitchTeam(client, CS_TEAM_T);
        CreateTimer(0.0, Timer_RemoveHud, client);
    }
    return Plugin_Continue;
}

public Action Event_PlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    if(IsClientValid(client))
    {
        //延迟隐藏被隐藏的ViewModel
        g_Client[client].SpawnCheck = true;
    }
    return Plugin_Continue;
}

public Action Event_PlayerHurt(Event event, const char[] name, bool dontBroadcast)
{
    return Plugin_Continue;
}

public Action Event_PlayerDeath_Pre(Event event, const char[] name, bool dontBroadcast)
{
    char weapon[32];
    int victim = GetClientOfUserId(GetEventInt(event, "userid"));
    int attacker = GetClientOfUserId(GetEventInt(event, "attacker"));
    bool headshot = GetEventBool(event, "headshot");
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    //PrintToChatAll("攻击者: %N | 武器: %s", attacker, weapon);
    CheckWinConditions();
    AddTraitorTime(victim, attacker);
    PlayDeadSound(victim);
    ClearRoleIcon(victim);
    ClearDNAIcon(INVALID_HANDLE, victim);
    BodiesOnPlayerDeath(victim, attacker, weapon, headshot);
    BlockPlayerVoice(victim);
    TellVictimAttackerRole(victim, attacker);
    HideHiddenViewModel(victim);
    AddKillingFrags(victim, attacker);
    AddKillingKarma(victim, attacker);
    ClientCommand(victim, "spec_menu");
    return Plugin_Handled;
}

public Action Event_PlayerJump(Event event, const char[] name, bool dontBroadcast)
{
    return Plugin_Continue;
}

public Action Event_WeaponFire(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(GetEventInt(event, "userid"));
    RequestFrame(InitWeaponsFireSpeed, client);
    return Plugin_Continue;
}

stock InitSDKHooks(int client)
{
    SDKHook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
    SDKHook(client, SDKHook_TraceAttack, DamageOnTraceAttack);
    SDKHook(client, SDKHook_WeaponSwitchPost, WeaponOnWeaponSwitchPost);
    SDKHook(client, SDKHook_WeaponDrop, WeaponOnWeaponDrop);
    SDKHook(client, SDKHook_WeaponDropPost, WeaponOnWeaponDropPost);
    SDKHook(client, SDKHook_PostThinkPost, OnClientThinkPost);
}

stock UnHookSDKHooks(int client)
{
    SDKUnhook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
    SDKUnhook(client, SDKHook_TraceAttack, DamageOnTraceAttack);
    SDKUnhook(client, SDKHook_WeaponSwitchPost, WeaponOnWeaponSwitchPost);
    SDKUnhook(client, SDKHook_WeaponDrop, WeaponOnWeaponDrop);
    SDKUnhook(client, SDKHook_WeaponDropPost, WeaponOnWeaponDropPost);
    SDKUnhook(client, SDKHook_PostThinkPost, OnClientThinkPost);
}