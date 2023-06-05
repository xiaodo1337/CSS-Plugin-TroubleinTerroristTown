stock Action ShopMenu(int client, int args)
{
    if(!IsClientValid(client) || !IsPlayerAlive(client) || !IsGameStart) return Plugin_Continue;
    Menu menu = new Menu(ShopMenu_CallBack);
    menu.SetTitle("[TTT 商店]");
    ShopItem Item;
    char buffer[MAX_MESSAGE_LENGTH], buffer2[16];
    int added;
    for(int i;i < g_aShopItem.Length;i++)
    {
        g_aShopItem.GetArray(i, Item, sizeof(Item));
        if(Item.ItemTeam == g_Client[client].Role)
        {
            added++;
            Format(buffer, sizeof(buffer), "%s - %s\n%s\n积分: %d", Item.ItemName, Item.ItemType == ITEMTYPE_WEAPON ? "武器" : Item.ItemType == ITEMTYPE_ActiveEquip ? "主动道具" : "被动道具", Item.ItemDesc, Item.ItemCost);
            if(Item.ItemTimeLimit > 0) Format(buffer, sizeof(buffer), "%s 剩余时间: %d秒", buffer, (Item.ItemTimeLimit > TimePassed) ? Item.ItemTimeLimit - TimePassed : 0);
            Format(buffer2, sizeof(buffer2), "%d", Item.ItemID);
            menu.AddItem(buffer2, buffer, (g_Client[client].Credits >= Item.ItemCost && g_Client[client].ItemBoughtTimes[Item.ItemID] > 0 && TeamCanBoughtTimes[g_Client[client].Role][Item.ItemID] > 0 && Item.ItemTimeLimit < TimePassed) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
            if(added % 4 != 0) menu.AddItem("", "", ITEMDRAW_SPACER);
        }
    }
    if(!added) menu.AddItem("-1", "无", ITEMDRAW_DISABLED);
    menu.Display(client, 30);
    return Plugin_Continue;
}

stock ShopMenu_CallBack(Menu menu, MenuAction action, int param1, int param2)
{
    if(action == MenuAction_Select)
    {
        char ItemID[32];
        bool found = GetMenuItem(menu, param2, ItemID, sizeof(ItemID));
        if(!found || StrEqual(ItemID, "-1") || StrEqual(ItemID, "")) return;
        if(!IsClientValid(param1) || !IsPlayerAlive(param1) || !IsGameStart) return;
        ShopItem Item;
        if(GetShopItemArrayByItemID(StringToInt(ItemID), Item))
        {
            if(Item.ItemTeam != g_Client[param1].Role)
            {
                GetRoleName(Item.ItemTeam, Buffer, sizeof(Buffer));
                CPrintToChat(param1, "%s该物品只有 \x07%X%s{default} 才能使用！", TTT_TAG, GetRoleColor(Item.ItemTeam), Buffer);
                return;
            }
            if(g_Client[param1].Credits < Item.ItemCost)
            {
                CPrintToChat(param1, "%s您的积分不足以购买 {yellow}%s{default} ！", TTT_TAG, Item.ItemName);
                return;
            }
            if(TeamCanBoughtTimes[g_Client[param1].Role][Item.ItemID] < 1 || g_Client[param1].ItemBoughtTimes[Item.ItemID] < 1)
            {
                CPrintToChat(param1, "%s购买次数超过限制！", TTT_TAG);
                return;
            }
            if(Item.ItemTimeLimit > TimePassed)
            {
                CPrintToChat(param1, "%s您需要等 %d 秒后才可购买该物品！", TTT_TAG, Item.ItemTimeLimit - TimePassed);
                return;
            }
            
            Action res = Plugin_Continue;
            Call_StartForward(g_fwOnItemPurchase);
            Call_PushCell(param1);
            Call_PushCell(StringToInt(ItemID));
            Call_Finish(res);

            if(res < Plugin_Handled)
            {
                g_Client[param1].Credits -= Item.ItemCost;
                TeamCanBoughtTimes[g_Client[param1].Role][Item.ItemID] --;
                g_Client[param1].ItemBoughtTimes[Item.ItemID] --;
                CPrintToChat(param1, "%s您花费 {yellow}%d{default} 积分购买了 {white}%s{default}！", TTT_TAG, Item.ItemCost, Item.ItemName);
                ClientCommand(param1, "playgamesound %s", shop_buy_sound);
            }
        }
        delete(menu);
    }
}

stock bool:GetShopItemArrayByItemID(int ItemID, ShopItem ItemArray)
{
    ShopItem Item;
    for(int i;i < g_aShopItem.Length;i++)
    {
        g_aShopItem.GetArray(i, Item, sizeof(Item));
        if(Item.ItemID == ItemID)
        {
            ItemArray = Item;
            return true;
        }
    }
    return false;
}

stock InitShopItemsBuyLimit()
{
    ShopItem Item;
    for(int i;i < g_aShopItem.Length;i++)
    {
        g_aShopItem.GetArray(i, Item, sizeof(Item));
        for(int j;j < sizeof(TeamCanBoughtTimes);j++) TeamCanBoughtTimes[j][Item.ItemID] = Item.ItemTeamBuyLimit == -1 ? 999999 : Item.ItemTeamBuyLimit;
        LoopValidClients(client)
        {
            g_Client[client].ItemBoughtTimes[Item.ItemID] = Item.ItemBuyLimit == -1 ? 999999 : Item.ItemBuyLimit;
        }
    }
}