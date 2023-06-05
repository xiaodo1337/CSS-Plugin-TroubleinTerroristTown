stock OpenRadioMenu(client)
{
    if(!IsClientValid(client) || !IsPlayerAlive(client) || !IsGameStart || IsGameEnd) return;
    Menu menu = new Menu(RadioMenu_Callback);
    menu.SetTitle("------ 快捷消息 ------");
    new target = TTT_GetClientAimTarget(client);
    if(IsClientValid(target) && IsPlayerAlive(target) && IsPlayersInDistance(client, target))
    {
        char PlayerName[MAX_NAME_LENGTH], MenuName[64];
        GetClientName(target, PlayerName, sizeof(PlayerName));
        Format(MenuName, sizeof(MenuName), "我正在跟着 %s。", PlayerName);
        menu.AddItem("follow", MenuName);
        Format(MenuName, sizeof(MenuName), "%s 是叛徒！", PlayerName);
        menu.AddItem("istraitor", MenuName);
        Format(MenuName, sizeof(MenuName), "%s 的行为很可疑！", PlayerName);
        menu.AddItem("questionable", MenuName);
        if(g_Client[client].Role == TTT_ROLE_DETECTIVE)
        {
            Format(MenuName, sizeof(MenuName), "%s 通过。", PlayerName);
            menu.AddItem("pass", MenuName);
            Format(MenuName, sizeof(MenuName), "%s 请停下！", PlayerName);
            menu.AddItem("stop", MenuName);
        }
    }
    menu.AddItem("yes", "是的。");
    menu.AddItem("no", "不。");
    menu.AddItem("help", "救命！");
    menu.AddItem("anyalive", "还有人活着吗？");
    menu.Pagination = false;
    menu.ExitButton = true;
    menu.Display(client, 15);
}

stock RadioMenu_Callback(Menu menu, MenuAction action, int param1, int param2)
{
    if(action == MenuAction_Select)
    {
        char name[64], info[64];
        bool found = GetMenuItem(menu, param2, info, sizeof(info), _, name, sizeof(name));
        if(!found) return;
        if(IsPlayerAlive(param1) && IsGameStart && !IsGameEnd)
        {
            if((StrEqual(info, "pass") || StrEqual(info, "stop")) && g_Client[param1].Role != TTT_ROLE_DETECTIVE) return;
            CPrintToChatAll("{salmon}(快捷消息)\x07%X%N{yellow} : %s", GetRoleColor(g_Client[param1].Role), param1, name);
        }
        CloseHandle(menu);
    }
}