InitMsgBlock()
{
    HookUserMessage(GetUserMessageId("TextMsg"), MsgHook_TextMsg, true);
    HookUserMessage(GetUserMessageId("HintText"), MsgHook_HintText, true);
}

public Action:MsgHook_TextMsg(UserMsg msg_id, Handle msg, const players[], playersNum, bool reliable, bool init)
{
    BfReadByte(msg);
    BfReadString(msg, Buffer, sizeof(Buffer));
    //PrintToServer(Buffer);
    if(StrEqual(Buffer, "#Game_teammate_attack") || StrEqual(Buffer, "#Killed_Teammate") || StrEqual(Buffer, "#C4_Plant_At_Bomb_Spot") || StrEqual(Buffer, "#Switch_To_BurstFire") || StrEqual(Buffer, "#Switch_To_FullAuto"))
        return Plugin_Handled;
    return Plugin_Continue;
}

public Action:MsgHook_HintText(UserMsg msg_id, Handle msg, const players[], playersNum, bool reliable, bool init)
{
    BfReadString(msg, Buffer, sizeof(Buffer));
    if(StrEqual(Buffer, "#Hint_careful_around_teammates") || StrEqual(Buffer, "#Hint_you_have_the_bomb"))
        return Plugin_Handled;
    return Plugin_Continue;
}