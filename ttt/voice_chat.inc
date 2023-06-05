InitChatMsg()
{
    RegConsoleCmd("say", Say_Message);
    RegConsoleCmd("say_team", Say_Message_Team);
}

stock Action:Say_Message(int client, int args)
{
    if (args && IsClientValid(client))
    {
        char message[MAX_MESSAGE_LENGTH];
        GetCmdArg(1, message, sizeof(message));
        PlayerTalk(client, message, false);
    }
    return Plugin_Handled;
}

stock Action:Say_Message_Team(int client, int args)
{
    if (args && IsClientValid(client))
    {
        if(g_Client[client].Role == TTT_ROLE_TRAITOR && IsPlayerAlive(client))
        {
            char message[MAX_MESSAGE_LENGTH];
            GetCmdArg(1, message, sizeof(message));
            PlayerTalk(client, message, true);
        }
        else
        {
            CPrintToChat(client, "%s只有活着并且是叛徒才可使用队内交流", TTT_TAG);
        }
    }
    return Plugin_Handled;
}

stock PlayerTalk(int client, char Message[MAX_MESSAGE_LENGTH], bool:TeamTalk)
{
    CReplaceColorCodes(Message, 0, true, sizeof(Message));
    if(TeamTalk)
    {
        for(int target = 1;target <= MaxClients;target++)
        {
            if(IsClientValid(target) && IsPlayerAlive(target) && g_Client[target].Role == TTT_ROLE_TRAITOR)
            {
                CPrintToChat(target, "{pink}[叛徒队内消息] \x07%X%N{default} : %s", GetRoleColor(g_Client[client].Role), client, Message);
            }
        }
    }
    else
    {
        if(IsPlayerAlive(client))
        {
            CPrintToChatAll("\x07%X%N{default} : %s", GetRoleColor(g_Client[client].Role), client, Message);
        }
        else
        {
            for(int target = 1;target <= MaxClients;target++)
            {
                if(IsClientValid(target))
                {
                    if(!IsPlayerAlive(target))
                    {
                        CPrintToChat(target, "{gray}%N{default} : %s", client, Message);
                    }
                }
            }
        }
    }
    decl String:RoleName[16];
    GetRoleName(g_Client[client].Role, RoleName, sizeof(RoleName));
    LogMessage("[%s聊天记录] %N(%s): %s", TeamTalk ? "队内" : "公共", client, RoleName, Message);
}

stock BlockPlayerVoice(int client)
{
    for(int receiver = 1;receiver <= MaxClients;receiver++)
    {
        if(IsClientValid(receiver))
        {
            if(IsPlayerAlive(receiver))
            {
                SetListenOverride(receiver, client, Listen_No);
            }
            else
            {
                SetListenOverride(receiver, client, Listen_Yes);
            }
            SetListenOverride(client, receiver, Listen_Yes);
        }
    }
}

stock ResetPlayersVoice()
{
    for(int receiver = 1;receiver <= MaxClients;receiver++)
    {
        for(int sender = 1;sender <= MaxClients;sender++)
        {
            if(IsClientValid(receiver) && IsClientValid(sender))
            {
                g_Client[sender].VoiceChannel = VOICE_CHANNEL_PUBLIC;
                SetListenOverride(receiver, sender, Listen_Yes);
            }
        }
    }
}

stock ChangeVoiceChannel(sender)
{
    if(IsPlayerAlive(sender) && g_Client[sender].Role == TTT_ROLE_TRAITOR)
    {
        if(g_Client[sender].VoiceChannel != VOICE_CHANNEL_TRAITOR)
        {
            g_Client[sender].VoiceChannel = VOICE_CHANNEL_TRAITOR;
            for(int receiver = 1;receiver <= MaxClients;receiver++)
            {
                if(IsClientValid(receiver))
                {
                    if(!IsPlayerAlive(receiver)) SetListenOverride(receiver, sender, Listen_Yes);
                    else
                    {
                        if(g_Client[receiver].Role == TTT_ROLE_TRAITOR) SetListenOverride(receiver, sender, Listen_Yes);
                        else SetListenOverride(receiver, sender, Listen_No);
                    }
                }
            }
        }
        else
        {
            g_Client[sender].VoiceChannel = VOICE_CHANNEL_PUBLIC;
            for(int receiver = 1;receiver <= MaxClients;receiver++)
            {
                if(IsClientValid(receiver))
                {
                    SetListenOverride(receiver, sender, Listen_Yes);
                }
            }
        }
    }
}