stock InitGameParams()
{
    TTT_EndAllTimer();
    IsGameEnd = false;
    IsGameStart = false;
}

public Action:StartGame(Handle Timer)
{
    tGameStart = INVALID_HANDLE;
    int MaxTraitor, MaxDetective, AlivePlayers;
    AlivePlayers = GetAliveClientCounts();

    MaxTraitor = (AlivePlayers < 10) ? 1 : AlivePlayers / 5;
    MaxDetective = (AlivePlayers < 10) ? 1 : AlivePlayers / 10;
        
    ArrayList PlayerList;
    
    for(int i;i < MaxTraitor;i++)
    {
        new ClientListSize = CreateClientList(PlayerList, TTT_ROLE_NONE);
        if(!ClientListSize)
        {
            CloseHandle(PlayerList);
            return;
        }
        int randindex = GetRandomInt(0, ClientListSize - 1);
        int randclient = GetArrayCell(PlayerList, randindex);
        SetClientRole(randclient, TTT_ROLE_TRAITOR);
    }
    
    for(int i;i < MaxDetective;i++)
    {
        new ClientListSize = CreateClientList(PlayerList, TTT_ROLE_NONE);
        if(!ClientListSize)
        {
            CloseHandle(PlayerList);
            return;
        }
        int randindex = GetRandomInt(0, ClientListSize - 1);
        int randclient = GetArrayCell(PlayerList, randindex);
        SetClientRole(randclient, TTT_ROLE_DETECTIVE);
    }

    for(int client = 1;client <= MaxClients;client++)
    {
        if(g_Client[client].Role == TTT_ROLE_NONE && IsClientValid(client) && IsPlayerAlive(client))
        {
            SetClientRole(client, TTT_ROLE_INNOCENT);
        }
    }

    RoleNumsUpdate();
    StartRoundTimeCountdown();
    StartTraitorTimeCountdown();
    StartPlayGameMusic();
    PrintCenterTextAll("游戏开始！");
    IsGameStart = true;
    CloseHandle(PlayerList);
}

stock TTT_EndRound(int team)
{
    if(!IsGameEnd)
    {
        StopAllSound();
        IsGameEnd = true;
        if(team == TTT_TEAM_INNOCENTS)
        {
            PrintCenterTextAll("平民 胜出！");
            CS_TerminateRound(16.0, CSRoundEnd_TargetSaved, true);
            PlaySoundToAll(innocent_win_sound);
            InnocentWin++;
            AddTeamFrags(TTT_TEAM_INNOCENTS, 1);
        }
        else if(team == TTT_TEAM_TRAITORS)
        {
            PrintCenterTextAll("叛徒 胜出！");
            CS_TerminateRound(16.0, CSRoundEnd_TargetBombed, true);
            PlaySoundToAll(traitor_win_sound);
            TraitorWin++;
            AddTeamFrags(TTT_TEAM_TRAITORS, 1);
        }
        else
        {
            CS_TerminateRound(0.0, CSRoundEnd_Draw, true);
            ClearTeamFrags(TTT_TEAM_ALL);
        }
    }
}

public Action:CS_OnTerminateRound(&Float:delay, &CSRoundEndReason:reason)
{
    if((reason == CSRoundEnd_GameStart && !IsGameStart))
    {
        return Plugin_Continue;
    }
    CheckWinConditions();
    return Plugin_Handled;
}

stock CheckWinConditions()
{
    if(IsGameStart && !IsGameEnd && !IsRequiredToPlay)
    {
        RoleNumsUpdate();
        if(!TraitorNums)
        {
            TTT_EndRound(TTT_TEAM_INNOCENTS);
        }
        else if(!DetectiveNums && !InnocentNums)
        {
            TTT_EndRound(TTT_TEAM_TRAITORS);
        }
    }
}

public Action:CheckGameCondition(Handle Timer)
{
    if(GetClientCount() < ttt_gamestart_require.IntValue)
    {
        for(int i = 1;i <= MaxClients;i++)
        {
            if(IsClientValid(i) && !IsPlayerAlive(i))
            {
                CS_SwitchTeam(i, CS_TEAM_T);
                CS_RespawnPlayer(i);
                ResetPlayer(i);
            }
            InitPlayerRole(i);
        }
        TTT_EndAllTimer();
        PrintHintTextToAll("等待玩家进入游戏");
        if(IsGameStart)
        {
            StopPlayGameMusic();
        }
        IsGameStart = false;
        IsGameEnd = false;
        IsRequiredToPlay = true;
    }
    else
    {
        if(IsRequiredToPlay)
        {
            TTT_EndRound(TTT_TEAM_ALL);
        }
        IsRequiredToPlay = false;
    }
}