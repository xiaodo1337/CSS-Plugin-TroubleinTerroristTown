InitCmdBlock()
{
    RegConsoleCmd("kill", HookCommands);
    RegConsoleCmd("jointeam", HookCommands);
    RegConsoleCmd("joinclass", HookCommands);
    RegConsoleCmd("explode", HookCommands);
    RegConsoleCmd("spectate", HookCommands);
    for (int i = 0; i < sizeof(RadioCMDs); i++)
    {
        RegConsoleCmd(RadioCMDs[i], HookCommands);
    }
}

stock Action:HookCommands(int client, int args)
{
    return Plugin_Handled;
}