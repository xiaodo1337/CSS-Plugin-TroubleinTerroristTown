AddTeamFrags(int team, int frags)
{
    if(team == TTT_TEAM_ALL)
    {
        LoopValidClients(client) g_Client[client].Frags[1] += frags;
    }
    else if(team == TTT_TEAM_INNOCENTS)
    {
        LoopValidClients(client)
        {
            if(g_Client[client].Role == TTT_ROLE_INNOCENT || g_Client[client].Role == TTT_ROLE_DETECTIVE)
                g_Client[client].Frags[1] += frags;
        }
    }
    else if(team == TTT_TEAM_TRAITORS)
    {
        LoopValidClients(client)
        {
            if(g_Client[client].Role == TTT_ROLE_TRAITOR)
                g_Client[client].Frags[1] += frags;
        }
    }
}

ClearTeamFrags(int team)
{
    if(team == TTT_TEAM_ALL)
    {
        LoopValidClients(client) g_Client[client].Frags[1] = 0;
    }
    else if(team == TTT_TEAM_INNOCENTS)
    {
        LoopValidClients(client)
        {
            if(g_Client[client].Role == TTT_ROLE_INNOCENT || g_Client[client].Role == TTT_ROLE_DETECTIVE)
                g_Client[client].Frags[1] = 0;
        }
    }
    else if(team == TTT_TEAM_TRAITORS)
    {
        LoopValidClients(client)
        {
            if(g_Client[client].Role == TTT_ROLE_TRAITOR)
                g_Client[client].Frags[1] = 0;
        }
    }
}

UpdatePlayersFrags()
{
    LoopValidClients(client)
    {
        g_Client[client].Frags[0] = g_Client[client].Frags[1];
        SetEntProp(client, Prop_Data, "m_iFrags", g_Client[client].Frags[0]);
    }
}

AddKillingFrags(int victim, int attacker)
{
    if(attacker != victim)
    {
        if(g_Client[attacker].Role == TTT_ROLE_TRAITOR)
        {
            switch(g_Client[victim].Role)
            {
                case TTT_ROLE_TRAITOR : {
                    g_Client[attacker].Frags[1] -= ttt_frags_traitor_kill_traitor.IntValue;
                }
                case TTT_ROLE_DETECTIVE : {
                    g_Client[attacker].Frags[1] += ttt_frags_traitor_kill_detective.IntValue;
                }
                case TTT_ROLE_INNOCENT : {
                    g_Client[attacker].Frags[1] += ttt_frags_traitor_kill_innocent.IntValue;
                }
            }
        }
        if(g_Client[attacker].Role == TTT_ROLE_DETECTIVE)
        {
            switch(g_Client[victim].Role)
            {
                case TTT_ROLE_TRAITOR : {
                    g_Client[attacker].Frags[1] += ttt_frags_detective_kill_traitor.IntValue;
                }
                case TTT_ROLE_DETECTIVE : {
                    g_Client[attacker].Frags[1] -= ttt_frags_detective_kill_detective.IntValue;
                }
                case TTT_ROLE_INNOCENT : {
                    g_Client[attacker].Frags[1] -= ttt_frags_detective_kill_innocent.IntValue;
                }
            }
        }
        if(g_Client[attacker].Role == TTT_ROLE_INNOCENT)
        {
            switch(g_Client[victim].Role)
            {
                case TTT_ROLE_TRAITOR : {
                    g_Client[attacker].Frags[1] += ttt_frags_innocent_kill_traitor.IntValue;
                }
                case TTT_ROLE_DETECTIVE : {
                    g_Client[attacker].Frags[1] -= ttt_frags_innocent_kill_detective.IntValue;
                }
                case TTT_ROLE_INNOCENT : {
                    g_Client[attacker].Frags[1] -= ttt_frags_innocent_kill_innocent.IntValue;
                }
            }
        }
    }
}