public Action Timer_RemoveHud(Handle timer, any client)
{
    if (IsClientValid(client) && !IsFakeClient(client))
    {
        SetEntPropFloat(client, Prop_Send, "m_flFlashDuration", GetEntPropFloat(client, Prop_Send, "m_flFlashDuration") + 7200.0);
        SetEntPropFloat(client, Prop_Send, "m_flFlashMaxAlpha", 0.5);
        //SetEntProp(client, Prop_Data, "m_iHideHUD", 1<<6);   //导致无法显示菜单
    }
    return Plugin_Continue;
}

public Action Event_PlayerBlind(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));

    if (IsClientValid(client) && !IsFakeClient(client) && GetClientTeam(client) > 1)
    {
        float fDuration = GetEntPropFloat(client, Prop_Send, "m_flFlashDuration");
        CreateTimer(fDuration, Timer_RemoveHud, client);
    }
    return Plugin_Continue;
}