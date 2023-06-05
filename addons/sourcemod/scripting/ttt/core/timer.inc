public InitGameStart()
{
    if(!IsRequiredToPlay)
    {
        new Float:GameStart_Time = ttt_gamestart_time.FloatValue;
        tGameStart = CreateTimer(GameStart_Time, StartGame, _, TIMER_FLAG_NO_MAPCHANGE);

        if (GameStart_Time > 1.0)
        {
            StopStartGameCountdown();   //删除之前存在的倒计时
            GameStartCountdownData = CreateDataPack();
            WritePackFloat(GameStartCountdownData, GameStart_Time);
            WritePackFloat(GameStartCountdownData, 0.0);
            //开始倒计时
            tGameStartCountdown = CreateTimer(1.0, GameStartCountdown, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
            GameStartCountdown(tGameStartCountdown);
        }
    }
}

public Action:GameStartCountdown(Handle:timer)
{
    ResetPack(GameStartCountdownData);
    new Float:length = ReadPackFloat(GameStartCountdownData);
    new Float:counter = ReadPackFloat(GameStartCountdownData);
    if (counter >= length)
    {
        StopStartGameCountdown();
        return Plugin_Stop;
    }
    GameStartTime = RoundToNearest(length - counter);
    PrintCenterTextAll("游戏将在 %d秒 后开始", GameStartTime);
    counter++;
    ResetPack(GameStartCountdownData);
    WritePackFloat(GameStartCountdownData, length);
    WritePackFloat(GameStartCountdownData, counter);
    
    LoopValidClients(i)
    {
        if((!IsGameStart && !IsGameEnd) && !IsPlayerAlive(i))
        {
            CS_SwitchTeam(i, CS_TEAM_T);
            CS_RespawnPlayer(i);
            ResetPlayer(i);
        }
    }
    return Plugin_Continue;
}

stock StopStartGameCountdown()
{
    TTT_EndTimer(tGameStartCountdown);
    if (GameStartCountdownData != INVALID_HANDLE)
    {
        CloseHandle(GameStartCountdownData);
        GameStartCountdownData = INVALID_HANDLE;
    }
}

//---------------------------------------------------------

stock StartRoundTimeCountdown()
{
    TTT_EndTimer(tRoundTime);
    RoundTime = ttt_round_time.IntValue;

    if (RoundTime > 1)
    {
        RoundTime --;
        tRoundTime = CreateTimer(1.0, RoundTimeCountdown, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
    }
}

stock Action:RoundTimeCountdown(Handle Timer)
{
    if (RoundTime <= 0)
    {
        LoopValidClients(client)
        {
            if(IsPlayerAlive(client))
            {
                g_Client[client].Credits ++;
                CPrintToChat(client, "%s你获得了 1 点积分", TTT_TAG);
            }
        }
        IsRoundTimeExtend = true;
        TTT_EndTimer(tRoundTime);
        return Plugin_Stop;
    }
    RoundTime --;
    return Plugin_Continue;
}

stock StartTraitorTimeCountdown()
{
    TTT_EndTimer(tTraitorTime);
    TraitorTime = ttt_traitor_time.IntValue;
    TimePassed = 1;

    if (TraitorTime > 1)
    {
        TraitorTime --;
        tTraitorTime = CreateTimer(1.0, TraitorTimeCountdown, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
    }
}

stock Action:TraitorTimeCountdown(Handle:Timer)
{
    if (TraitorTime <= 0)
    {
        TTT_EndTimer(tTraitorTime);
        //TTT_EndRound(TTT_TEAM_INNOCENTS);
        tEndGameTime = CreateTimer(60.0, EndGame);
        LoopValidClients(client)
        {
            if(IsPlayerAlive(client))
            {
                if(g_Client[client].Role == TTT_ROLE_TRAITOR)
                {
                    g_Client[client].Leak = true;
                }
                else
                {
                    g_Client[client].Credits ++;
                    CPrintToChat(client, "%s你获得了 1 点积分", TTT_TAG);
                }
                TTT_GiveClientRadar(client);
            }
        }
        CPrintToChatAll("%s叛徒们的时间到了！所有叛徒的身份都已暴露！", TTT_TAG);
        IsTraitorTimeExtend = true;
        return Plugin_Stop;
    }
    TraitorTime --;
    TimePassed ++;
    return Plugin_Continue;
}

stock Action EndGame(Handle Timer)
{
    TTT_EndTimer(tEndGameTime);
    TTT_EndRound(TTT_TEAM_INNOCENTS);
    return Plugin_Continue;
}

stock StartDNACheckTimeCountdown(int client)
{
    TTT_EndTimer(g_Client[client].DNACheckCountdownTimer);
    g_Client[client].DNAProcess = ttt_dna_check_time.IntValue;
    if (g_Client[client].DNAProcess > 1)
    {
        g_Client[client].DNACheckCountdownTimer = CreateTimer(1.0, DNACheckTimeCountdown, client);
    }
}

public Action:DNACheckTimeCountdown(Handle:Timer, int client)
{
    if (g_Client[client].DNAProcess <= 0)
    {
        TTT_EndTimer(g_Client[client].DNACheckCountdownTimer);
        return Plugin_Stop;
    }
    g_Client[client].DNAProcess --;
    g_Client[client].DNACheckCountdownTimer = CreateTimer(1.0, DNACheckTimeCountdown, client);
    return Plugin_Continue;
}

//---------------------------------------------------------

stock TTT_EndAllTimer()
{
    TTT_EndTimer(tGameMusic);
    TTT_EndTimer(tGameStart);
    TTT_EndTimer(tGameStartCountdown);
    TTT_EndTimer(tRoundTime);
    TTT_EndTimer(tTraitorTime);
    TTT_EndTimer(tEndGameTime);
    LoopValidClients(client)
    {
        TTT_EndTimer(g_Client[client].DNACheckTimer);
        TTT_EndTimer(g_Client[client].DNACheckCountdownTimer);
        TTT_EndTimer(g_Client[client].DNAIconRemoveTimer);
    }
    IsRoundTimeExtend = false;
    IsTraitorTimeExtend = false;
}

stock TTT_EndPlayerTimer(int client)
{
    TTT_EndTimer(g_Client[client].DNACheckTimer);
    TTT_EndTimer(g_Client[client].DNACheckCountdownTimer);
    TTT_EndTimer(g_Client[client].DNAIconRemoveTimer);
}

stock bool:TTT_IsTimerRunning(Handle:timer)
{
    return (timer != INVALID_HANDLE);
}

stock bool:TTT_EndTimer(&Handle:timer, bool:kill = true)
{
    if (TTT_IsTimerRunning(timer))
    {
        if (kill && IsValidHandle(timer))
        {
            KillTimer(timer);
        }
        timer = INVALID_HANDLE;
        return true;
    }
    
    timer = INVALID_HANDLE;
    return false;
}