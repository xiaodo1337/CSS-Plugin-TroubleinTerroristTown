stock DnaListOnRoundStart()
{
    for(int i = 1;i <= MaxClients;i++)
    {
        if (g_Client[i].DNAList != null)
        {
            g_Client[i].DNAList.Clear();
        }
    }
}

stock PickUpBodyDna(int client)
{
    if((IsGameStart && IsGameEnd) || (!IsGameStart && !IsGameEnd) || g_Client[client].Role != TTT_ROLE_DETECTIVE) return;
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
            if(body.DNAPicked)
            {
                CPrintToChat(client, "%s该尸体的DNA已被拾取!", TTT_TAG);
                return;
            }
            if(g_Client[client].DNAList.Length < 5)
            {
                CPrintToChat(client, "%s你拾取了{yellow}%s{default}的DNA，单击{yellow}计分板键{default}打开DNA检测菜单！", TTT_TAG, body.VictimName);
                g_Client[client].DNAList.PushArray(body, sizeof(body));
                body.DNAPicked = true;
                SetRagdollArray(body);
            }
            else
            {
                CPrintToChat(client, "%s你的DNA列表已满!", TTT_TAG)
            }
        }
        else
        {
            CPrintToChat(client, "%s请瞄准尸体后再拾取DNA!", TTT_TAG);
        }
    }
    else
    {
        CPrintToChat(client, "%s请瞄准尸体后再拾取DNA!", TTT_TAG);
    }
}

stock OpenDNAMenu(int client)
{
    char info[16], name[64];
    Menu menu = new Menu(DNAMenu_Callback);
    menu.SetTitle("DNA检测菜单");
    for(int i;i < 5;i++)
    {
        if(i < g_Client[client].DNAList.Length) {
            Ragdoll body;
            g_Client[client].DNAList.GetArray(i, body, sizeof(body));
            Format(info, sizeof(info), "%d", body.EntityRef);
            Format(name, sizeof(name), "%s的DNA",  body.VictimName);
            menu.AddItem(info, name);
        } else {
            menu.AddItem("", "-空-", ITEMDRAW_DISABLED);
        }
    }
    menu.AddItem("", "关闭");
    menu.ExitButton = false;
    menu.Display(client, 15);
}

stock DNAMenu_Callback(Menu menu, MenuAction action, int param1, int param2)
{
    if(action == MenuAction_Select)
    {
        char info[16];
        bool found = GetMenuItem(menu, param2, info, sizeof(info));
        if (!found) return;
        if(!StrEqual(info, ""))
        {
            TTT_EndTimer(g_Client[param1].DNACheckTimer);
            int EntRef = StringToInt(info);
            Ragdoll body;
            GetRagdollArrayByEntRef(EntRef, body);
            if(!body.Scanned)
            {
                DataPack DataPackBody = new DataPack();
                DataPackBody.WriteCell(param1);
                DataPackBody.WriteCell(body.EntityRef);
                g_Client[param1].DNACheckTimer = CreateTimer(ttt_dna_check_time.FloatValue, FindOutKiller, DataPackBody);
                StartDNACheckTimeCountdown(param1);
            }
            else
            {
                CPrintToChat(param1, "%s该尸体的DNA已被检测过了!", TTT_TAG);
            }
        }
    }
}

stock Action:FindOutKiller(Handle Timer, DataPack DataPackBody)
{
    DataPackBody.Reset();
    int client = DataPackBody.ReadCell();
    int EntityRef = DataPackBody.ReadCell();
    if(IsClientValid(client))
    {
        Ragdoll body;
        g_Client[client].DNACheckTimer = INVALID_HANDLE;
        TTT_EndTimer(g_Client[client].DNACheckCountdownTimer);
        GetRagdollArrayByEntRef(EntityRef, body);
        RemoveDNAList(client, body.Victim);
        if(IsClientValid(body.Attacker) && IsPlayerAlive(body.Attacker))
        {
            char RoleName[16];
            GetRoleName(g_Client[body.Attacker].Role, RoleName, sizeof(RoleName));
            CPrintToChat(client, "%s{red}%s{default}的DNA检测完毕，凶手是\x07%X%s{default}!\n凶手已被{blue}蓝色图标{default}标记出来 {yellow}(%d秒有效期)", TTT_TAG, body.VictimName, GetRoleColor(g_Client[body.Attacker].Role), RoleName, ttt_dna_visible_time.IntValue);
            g_Client[body.Attacker].DNAIcon = CreateDNAIcon(body.Attacker);
            g_Client[client].DNACalled = body.Attacker;
        }
        else
        {
            CPrintToChat(client, "%s{red}%s{default}的DNA检测已完成，凶手已死亡", TTT_TAG, body.VictimName);
        }
        body.Scanned = true;
        SetRagdollArray(body);
    }
}

stock RemoveDNAList(int client, int victim)
{
    for(int i;i < g_Client[client].DNAList.Length;i++)
    {
        if(i >= 5)
        {
            g_Client[client].DNAList.Erase(i);
            continue;
        }
        Ragdoll tmp;
        g_Client[client].DNAList.GetArray(i, tmp, sizeof(tmp));
        if(tmp.Victim == victim)
        {
            g_Client[client].DNAList.Erase(i);
            return;
        }
    }
}