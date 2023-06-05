public Action:RulesMenu(int client, int args)
{
    Menu menu = new Menu(RulesMenu_Choose);
    menu.SetTitle("规则菜单");
    menu.AddItem("gamerule", "游戏介绍");
    menu.AddItem("innocent", "平民规则");
    menu.AddItem("detective", "侦探规则");
    menu.AddItem("traitor", "叛徒规则");
    menu.Display(client, 30);
}

stock RulesMenu_Choose(Menu menu, MenuAction action, int param1, int param2)
{
    if (action == MenuAction_Select)
	{
        char info[64];
        bool found = GetMenuItem(menu, param2, info, sizeof(info));
        if (!found) return;
        if(StrEqual(info, "gamerule"))
        {
            CPrintToChat(param1, "{yellow}一群匪徒当中出了一些叛徒，这些叛徒正磨刀霍霍，准备做掉所有和他们不是一伙的人。在游戏之中，一少部分玩家将被选为叛徒，他们的目标是干掉所有平民。");
            CPrintToChat(param1, "{yellow}余下的平民知道，这些叛徒一直藏身于人海之中；但他们却并不清楚：究竟谁是叛徒，谁又清白？“无辜者”需要做的就是活下去——在这个模式里，对于无辜者而言，知识即是力量：");
            CPrintToChat(param1, "{yellow}谁总是形迹可疑、鬼鬼祟祟？谁又与不时出现的被害人尸体有着千丝万缕、难以剪断的关系？甚至于，在某些极端情况下谁，还没有死，正在准备进行最后的血洗计划？");
        }
        if(StrEqual(info, "innocent"))
        {
            CPrintToChat(param1, "{yellow}平民要在游戏中配合侦探找出叛徒并将其击杀来获得游戏胜利");
            CPrintToChat(param1, "{yellow}1.禁止平民和侦探任何乱杀、猜测杀、仇杀和打黑枪");
            CPrintToChat(param1, "{yellow}2.平民及侦探可以击杀拿出叛徒武器的任何人.（叛徒武器：AWP、RPG、USP、圣战C4）");
            CPrintToChat(param1, "{yellow}3.平民看到任何袭击或杀死侦探的可疑人物，都可以直接击杀.");
            CPrintToChat(param1, "{yellow}4.你可以按 [使用键] 检查尸体死亡原因来判断谁是凶手.");
            CPrintToChat(param1, "{yellow}5.当你看到A杀死了B后,应该先去检查B的尸体查看其身份，才能对A做出回应.");
        }
        if(StrEqual(info, "detective"))
        {
            CPrintToChat(param1, "{yellow}侦探必须保护平民存活至游戏时间结束，或杀光所有叛徒");
            CPrintToChat(param1, "{yellow}1.禁止侦探和平民任何乱杀、猜测杀、仇杀和打黑枪");
            CPrintToChat(param1, "{yellow}2.平民及侦探可以击杀拿出叛徒武器的任何人.（叛徒武器：AWP、RPG、USP、圣战C4）");
            CPrintToChat(param1, "{yellow}3.侦探可以从平民或侦探尸体上查询DNA来获取凶手位置，更有利于找到叛徒的位置");
            CPrintToChat(param1, "{yellow}4.侦探沙鹰拥有一发子弹,打平民不扣血,秒杀叛徒");
        }
        if(StrEqual(info, "traitor"))
        {
            CPrintToChat(param1, "{yellow}叛徒要在指定时间内杀死所有平民和侦探");
            CPrintToChat(param1, "{yellow}1.叛徒可以用[夜视仪键]道具菜单来购买武器进行作案 (暂时未有以后会做)");
            CPrintToChat(param1, "{yellow}2.头顶有红色T字的人是你的队友，协同作案会让你更顺利");
            CPrintToChat(param1, "{yellow}3.RPG可能会炸伤自己，所以你要学会有效的控制距离");
            CPrintToChat(param1, "{yellow}4.当侦探切出沙鹰的时候你要小心，它可以一枪秒杀你");
            CPrintToChat(param1, "{yellow}5.游戏中你可以切出小刀来迷惑侦探和平民,故弄玄虚");
            CPrintToChat(param1, "{yellow}6.叛徒每杀死一人回合时间就会增加13s,要尽量的隐藏自己.");
        }
        CloseHandle(menu);
	}
}