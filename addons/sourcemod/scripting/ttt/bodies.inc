stock BodiesOnRoundStart()
{
    if (g_aRagdoll != null)
    {
        g_aRagdoll.Clear();
    }
}

BodiesOnPlayerDeath(int victim, int attacker, char weapon[32], bool headshot)
{
    Ragdoll body;

    int iRagdoll = GetEntPropEnt(victim, Prop_Send, "m_hRagdoll");
    if (iRagdoll > 0)
    {
        AcceptEntityInput(iRagdoll, "Kill");
    }

    int iEntity = CreateRagdoll(victim);
    if (iEntity != -1) body.EntityRef = EntIndexToEntRef(iEntity);
    else body.EntityRef = -1;

    body.Victim = victim;
    body.VictimRole = g_Client[victim].Role;
    Format(body.VictimName, sizeof(body.VictimName), "%N", victim);
    if(IsClientValid(attacker))
    {
        Format(body.AttackerName, sizeof(body.AttackerName), "%N", attacker);
        body.AttackerRole = g_Client[attacker].Role;
    }
    else
    {
        Format(body.AttackerName, sizeof(body.AttackerName), "Unknown");
        attacker = 0;
    }
    body.Credits = g_Client[victim].Credits;
    body.CallIcon = -1;
    body.Attacker = attacker;
    body.GameTime = GetGameTime();
    body.HeadShot = headshot;
    Format(body.Weaponused, sizeof(body.Weaponused), weapon);
    GetClientModel(victim, body.Model, sizeof(body.Model));
    PushRagdollArray(body);
}

int CreateRagdoll(int client)
{
    char sModel[128];
    GetClientModel(client, sModel, sizeof(sModel));

    int iEntity = -1;
    iEntity = CreateEntityByName("prop_ragdoll");
    if (!IsValidEntity(iEntity))
    {
        LogError("Can't create \"prop_ragdoll\"! Invalid entity index.");
        return -1;
    }

    char sBuffer[12], sName[32];
    DispatchKeyValue(iEntity, "model", sModel);
    IntToString(view_as<int>(MOVETYPE_NONE), sBuffer, sizeof(sBuffer));
    DispatchKeyValue(iEntity, "movetype", sBuffer);
    Format(sName, sizeof(sName), "ragdoll_%d", client);
    DispatchKeyValue(iEntity, "targetname", sName);

    float fOrigin[3], fAngles[3], fVelocity[3];
    GetClientAbsOrigin(client, fOrigin);
    GetClientAbsAngles(client, fAngles);
    GetEntPropVector(client, Prop_Data, "m_vecAbsVelocity", fVelocity);
    // 我们没有必要在这里把它设置为NULL_VECTOR，所以只是把速度归一化，
    // 以符合我们事先检查的最大速度，也许也可以降低这个速度。
    float speed = GetVectorLength(fVelocity);
    if (speed >= 500.0)
    {
        NormalizeVector(fVelocity, fVelocity);
        ScaleVector(fVelocity, speed < 500.0 ? speed : 500.0);
    }
    if (DispatchSpawn(iEntity))
    {
        ActivateEntity(iEntity);
        SetEntProp(iEntity, Prop_Data, "m_CollisionGroup", COLLISION_GROUP_DEBRIS);
        SetEntProp(iEntity, Prop_Data, "m_nSolidType", SOLID_VPHYSICS);
        AcceptEntityInput(iEntity, "EnableMotion");
        TeleportEntity(iEntity, fOrigin, fAngles, fVelocity);
    }
    SetEntPropEnt(client, Prop_Send, "m_hRagdoll", iEntity);

   // PrintToChatAll("%d", GetEntProp(iEntity, Prop_Send, "m_iParentAttachment"))

    return iEntity;
}

stock CheckBody(int client, int button = 0)
{
    if (g_aRagdoll.Length == 0) return;
    int iEntity = TTT_GetClientAimTarget(client, false);
    if(IsValidEntity(iEntity))
    {
        if(CanClientUseEntity(client, iEntity) && IsEntityBody(iEntity))
        {
            Ragdoll body;
            if(!GetRagdollArrayByEntIndex(iEntity, body))
            {
                PrintToChat(client, "无法获取布娃娃数组");
                return;
            }
            int ent = EntRefToEntIndex(body.EntityRef);
            if (body.VictimRole == TTT_ROLE_NONE)
            {
                PrintToChat(client, "%s 无效尸体", TTT_TAG);
                RequestFrame(Frame_RemoveBody, body.EntityRef);
                RemoveRagdollArray(ent);
                return;
            }
            int victim = body.Victim;
            int attacker = body.Attacker;
            if (!body.Found)
            {
                bool bInWalk = ((button & IN_SPEED) > 0);
                if (!StrEqual(body.Weaponused, "Fake!", false))
                {
                    decl String:sBuffer[16];
                    GetRoleName(body.VictimRole, sBuffer, sizeof(sBuffer));
                    if (!bInWalk)
                    {
                        body.Found = true;
                        CPrintToChatAll("%s\x07%X%N{default} 发现了 \x07%X%s{default}({snow}%s{default}) 的尸体", TTT_TAG, GetRoleColor(g_Client[client].Role), client, GetRoleColor(body.VictimRole), body.VictimName, sBuffer);
                    }
                }
            }
            if(g_Client[client].Role == TTT_ROLE_DETECTIVE && !body.Checked)
            {
                body.Checked = true;
                ClearCallIcon(ent);
            }
            InspectBody(client, body.DNAPicked, body.Credits, victim, body.VictimRole, attacker, RoundToNearest(GetGameTime() - body.GameTime), body.HeadShot, body.Explode, body.Weaponused, body.VictimName);
            SetRagdollArray(body);
        }
    }
}

stock InspectBody(int client, bool DNAPicked, int Credits, int victim, int victimRole, int attacker, int time, bool headshot, bool explode, const char[] weapon, const char[] victimName)
{
    char timefmt[8], rolename[16], title[256];
    FormatTime(timefmt, sizeof(timefmt), "%M:%S", time);
    GetRoleName(victimRole, rolename, sizeof(rolename));
    Format(title, sizeof(title), "=== TTT尸体检查结果 ===\n这具尸体是 %s\n此人是一位 %s\n大概死于 %s 之前.\n", victimName, rolename, timefmt);
    if(strlen(weapon) > 0 && attacker && victim != attacker)
    {
        if(StrContains(weapon, "prop_", false) > -1)
        {
            Format(title, sizeof(title), "%s身上有被重物砸伤的淤青\n", title);
        }
        else
        {
            if(StrEqual(weapon, "knife"))
            {
                Format(title, sizeof(title), "%s身上有被刀捅伤的痕迹\n", title);
            }
            else if(StrEqual(weapon, "hegrenade") || explode)
            {
                Format(title, sizeof(title), "%s一场猛烈的爆炸夺走了他的生命\n", title);
            }
            else if(!StrEqual(weapon, "flashbang") && !StrEqual(weapon, "smokegrenade"))
            {
                Format(title, sizeof(title), "%s身上有明显的弹孔\n凶器疑似: %s\n", title, weapon);
                if(headshot) Format(title, sizeof(title), "%s子弹穿透了他的脑袋\n", title);
            }
        }
    }
    else
    {
        Format(title, sizeof(title), "%s由于怨恨这个世界而死\n", title);
    }

    Menu BodyMenu = new Menu(BodyMenu_Callback);
    BodyMenu.SetTitle(title);
    BodyMenu.AddItem("", "关闭");
    BodyMenu.AddItem("call_detective", "呼叫侦探", (g_Client[client].Role != TTT_ROLE_DETECTIVE && g_Client[client].CallTime > 0) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
    BodyMenu.AddItem("pickup_dna", "采集DNA", (!DNAPicked && g_Client[client].Role == TTT_ROLE_DETECTIVE) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
    if(Credits > 0) BodyMenu.AddItem("pickup_credits", "您发现了他留下来的积分");
    BodyMenu.ExitButton = false;
    BodyMenu.Display(client, 30);
}

stock BodyMenu_Callback(Menu menu, MenuAction action, int param1, int param2)
{
    if(action == MenuAction_Select)
    {
        char info[64];
        bool found = GetMenuItem(menu, param2, info, sizeof(info));
        if (!found) return;
        if(IsPlayerAlive(param1))
        {
            if(StrEqual(info, "call_detective"))
            {
                CallDetective(param1);
            }
            else if(StrEqual(info, "pickup_dna") && g_Client[param1].Role == TTT_ROLE_DETECTIVE)
            {
                PickUpBodyDna(param1);
            }
            else if(StrEqual(info, "pickup_credits"))
            {
                PickUpBodyCredits(param1);
            }
        }
        CloseHandle(menu);
    }
}

stock Frame_RemoveBody(any ref)
{
    int iRagdoll = EntRefToEntIndex(ref);

    if (iRagdoll > 0)
    {
        AcceptEntityInput(iRagdoll, "Kill");
    }
}

stock CallDetective(client)
{
    if((IsGameStart && IsGameEnd) || (!IsGameStart && !IsGameEnd) || g_Client[client].Role == TTT_ROLE_DETECTIVE) return;
    if(g_Client[client].CallTime <= 0)
    {
        CPrintToChat(client, "%s本回合呼叫侦探次数已用尽！", TTT_TAG);
        return;
    }
    if(!DetectiveNums)
    {
        CPrintToChat(client, "%s没有可呼叫的侦探！", TTT_TAG);
        return;
    }
    int iEntity = TTT_GetClientAimTarget(client, false);
    if(IsValidEntity(iEntity))
    {
        if(CanClientUseEntity(client, iEntity) && IsEntityBody(iEntity))
        {
            if(g_aRagdoll.Length == 0) return;
            Ragdoll body;
            if(!GetRagdollArrayByEntIndex(iEntity, body))
            {
                PrintToChat(client, "无法获取布娃娃数组");
                return;
            }
            int ent = EntRefToEntIndex(body.EntityRef);
            if(!IsValidEntity(EntRefToEntIndex(body.CallIcon)) && !body.Checked)
            {
                body.CallIcon = CreateCallIcon(ent);
                CPrintToChatAll("%s{red}%N{default} 呼叫侦探检查 {red}%s{default} 的尸体", TTT_TAG, client, body.VictimName);
                g_Client[client].CallTime --;
            }
            else
            {
                CPrintToChat(client, "%s这具尸体已被呼叫过或被侦探检查过了!", TTT_TAG);
            }
            SetRagdollArray(body);
        }
        else
        {
            CPrintToChat(client, "%s请瞄准尸体后再呼叫!", TTT_TAG);
        }
    }
    else
    {
        CPrintToChat(client, "%s请瞄准尸体后再呼叫!", TTT_TAG);
    }
}

stock PickUpBodyCredits(int client)
{
    if((!IsGameStart && !IsGameEnd)) return;
    int iEntity = TTT_GetClientAimTarget(client, false);
    if(IsValidEntity(iEntity))
    {
        if(CanClientUseEntity(client, iEntity) && IsEntityBody(iEntity))
        {
            if(g_aRagdoll.Length == 0) return;
            Ragdoll body;
            if(!GetRagdollArrayByEntIndex(iEntity, body))
            {
                PrintToChat(client, "无法获取布娃娃数组");
                return;
            }
            if(body.Credits > 0)
            {
                CPrintToChat(client, "%s您拾取了 {red}%s {default}的 {yellow}%d {default}点积分！", TTT_TAG, body.VictimName, body.Credits);
                g_Client[client].Credits += body.Credits;
                body.Credits = 0;
            }
            else
            {
                CPrintToChat(client, "%s这具尸体的积分已经被他人拾取过了！", TTT_TAG);
            }
            SetRagdollArray(body);
        }
        else
        {
            CPrintToChat(client, "%s请瞄准尸体后再拾取！", TTT_TAG);
        }
    }
    else
    {
        CPrintToChat(client, "%s请瞄准尸体后再拾取！", TTT_TAG);
    }
}

stock bool GetRagdollArrayByPlayer(int client, Ragdoll body_array)
{
    if (IsClientValid(client))
    {
        Ragdoll body;
        for (int i = 0; i < g_aRagdoll.Length; ++i)
        {
            g_aRagdoll.GetArray(i, body, sizeof(body));
            if(!IsValidEntity(EntRefToEntIndex(body.EntityRef)))
            {
                g_aRagdoll.Erase(i);
                continue;
            }
            if (body.Victim == client)
            {
                body_array = body;
                return true;
            }
        }
    }
    return false;
}

stock bool GetRagdollArrayByEntIndex(int EntIndex, Ragdoll body_array)
{
    if (IsValidEntity(EntIndex))
    {
        Ragdoll body;
        for (int i = 0; i < g_aRagdoll.Length; ++i)
        {
            g_aRagdoll.GetArray(i, body, sizeof(body));
            if(!IsValidEntity(EntRefToEntIndex(body.EntityRef)))
            {
                g_aRagdoll.Erase(i);
                continue;
            }
            if (body.EntityRef == EntIndexToEntRef(EntIndex))
            {
                body_array = body;
                return true;
            }
        }
    }
    return false;
}

stock bool GetRagdollArrayByEntRef(int EntRef, Ragdoll body_array)
{
    if (IsValidEntity(EntRefToEntIndex(EntRef)))
    {
        Ragdoll body;
        for (int i = 0; i < g_aRagdoll.Length; ++i)
        {
            g_aRagdoll.GetArray(i, body, sizeof(body));
            if(!IsValidEntity(EntRefToEntIndex(body.EntityRef)))
            {
                g_aRagdoll.Erase(i);
                continue;
            }
            if (body.EntityRef == EntRef)
            {
                body_array = body;
                return true;
            }
        }
    }
    return false;
}

stock int GetClientByRagdollIndex(ragdoll_id)
{
    if (IsValidEntity(ragdoll_id))
    {
        Ragdoll body;
        for (int i = 0; i < g_aRagdoll.Length; ++i)
        {
            g_aRagdoll.GetArray(i, body, sizeof(body));
            if(!IsValidEntity(EntRefToEntIndex(body.EntityRef)))
            {
                g_aRagdoll.Erase(i);
                continue;
            }
            if (EntRefToEntIndex(body.EntityRef) == ragdoll_id)
            {
                return body.Victim;
            }
        }
    }
    return -1;
}

stock int GetRagdollIndexByPlayerIndex(int client)
{
    if (IsClientValid(client))
    {
        Ragdoll body;
        for (int i = 0; i < g_aRagdoll.Length; ++i)
        {
            g_aRagdoll.GetArray(i, body, sizeof(body));
            if(!IsValidEntity(EntRefToEntIndex(body.EntityRef)))
            {
                g_aRagdoll.Erase(i);
                continue;
            }
            if (body.Victim == client)
            {
                return EntRefToEntIndex(body.EntityRef);
            }
        }
    }
    return -1;
}

stock int SetRagdollArray(Ragdoll body)
{
    bool found = false;

    for (int i = 0; i < g_aRagdoll.Length; ++i)
    {
        Ragdoll tmp;
        g_aRagdoll.GetArray(i, tmp);
        if(!IsValidEntity(EntRefToEntIndex(tmp.EntityRef)))
        {
            g_aRagdoll.Erase(i);
            continue;
        }
        if (body.EntityRef == tmp.EntityRef)
        {
            g_aRagdoll.SetArray(i, body, sizeof(body));
            found = true;
            break;
        }
    }

    if (!found)
    {
        g_aRagdoll.PushArray(body, sizeof(body));
    }

    return found;
}

stock int PushRagdollArray(Ragdoll body)
{
    return g_aRagdoll.PushArray(body, sizeof(body));
}

stock bool RemoveRagdollArray(ragdoll_id)
{
    Ragdoll body;
    for (int i = 0; i < g_aRagdoll.Length; ++i)
    {
        g_aRagdoll.GetArray(i, body, sizeof(body));

        if (body.EntityRef == EntIndexToEntRef(ragdoll_id))
        {
            g_aRagdoll.Erase(i);
            return true;
        }
    }
    return false;
}