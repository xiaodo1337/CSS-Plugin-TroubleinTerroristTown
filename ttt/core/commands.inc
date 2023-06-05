ConVar ttt_gamestart_time;
ConVar ttt_gamestart_require;
ConVar ttt_round_time;
ConVar ttt_traitor_time;
ConVar ttt_traitor_kill_time;
ConVar ttt_entity_use_distance;
ConVar ttt_show_hud_distance;
ConVar ttt_dna_check_time;
ConVar ttt_dna_visible_time;
ConVar ttt_karma_default;
ConVar ttt_karma_max;
ConVar ttt_karma_min;
ConVar ttt_karma_innocent_kill_innocent;
ConVar ttt_karma_innocent_kill_traitor;
ConVar ttt_karma_innocent_kill_detective;
ConVar ttt_karma_traitor_kill_innocent;
ConVar ttt_karma_traitor_kill_traitor;
ConVar ttt_karma_traitor_kill_detective;
ConVar ttt_karma_detective_kill_innocent;
ConVar ttt_karma_detective_kill_traitor;
ConVar ttt_karma_detective_kill_detective;
ConVar ttt_frags_innocent_kill_innocent;
ConVar ttt_frags_innocent_kill_traitor;
ConVar ttt_frags_innocent_kill_detective;
ConVar ttt_frags_traitor_kill_innocent;
ConVar ttt_frags_traitor_kill_traitor;
ConVar ttt_frags_traitor_kill_detective;
ConVar ttt_frags_detective_kill_innocent;
ConVar ttt_frags_detective_kill_traitor;
ConVar ttt_frags_detective_kill_detective;
ConVar ttt_grabbermod_grab_distance;
ConVar ttt_grabbermod_grab_velocity_limit;

InitCommands()
{
    RegConsoleCmd("sm_rule", RulesMenu);
    RegConsoleCmd("nightvision", ShopMenu);
}

InitCvars()
{
    ttt_gamestart_time = CreateConVar("ttt_gamestart_time", "15.0", "游戏开始的时间(秒)", _, true, 0.0, true, 60.0);
    ttt_gamestart_require = CreateConVar("ttt_gamestart_require", "4.0", "开始游戏所需的人数", _, true, 3.0, true, 64.0);
    ttt_round_time = CreateConVar("ttt_round_time", "105.0", "每回合的时间(秒)", _, true, 0.0, true, 600.0);
    ttt_traitor_time = CreateConVar("ttt_traitor_time", "145.0", "叛徒的时间(秒)", _, true, 0.0, true, 600.0);
    ttt_traitor_kill_time = CreateConVar("ttt_traitor_kill_time", "13.0", "叛徒杀人所获得的时间(秒)", _, true, 0.0, true, 60.0);
    ttt_show_hud_distance = CreateConVar("ttt_show_hud_distance", "1200.0", "显示实体信息HUD的范围", _, true, 0.0, true, 5000.0);
    ttt_entity_use_distance = CreateConVar("ttt_entity_use_distance", "130.0", "使用实体的范围", _, true, 0.0, true, 1000.0);
    ttt_dna_check_time = CreateConVar("ttt_dna_check_time", "30.0", "DNA检测所需的时间", _, true, 0.0, true, 300.0);
    ttt_dna_visible_time = CreateConVar("ttt_dna_visible_time", "60.0", "DNA图标显示的时间", _, true, 0.0, true, 300.0);

    ttt_karma_default = CreateConVar("ttt_karma_default", "125.0", "玩家默认的Karma", _, true, 0.0, true, 1000.0);
    ttt_karma_max = CreateConVar("ttt_karma_max", "300.0", "玩家所能持有的最大Karma", _, true, 0.0, true, 1000.0);
    ttt_karma_min = CreateConVar("ttt_karma_min", "100.0", "低于该karma的玩家下局将恢复至该值", _, true, 0.0, true, 1000.0);
    ttt_karma_innocent_kill_innocent = CreateConVar("ttt_karma_innocent_kill_innocent", "25.0", "平民杀死平民所损失的Karma", _, true, 0.0, true, 100.0);
    ttt_karma_innocent_kill_traitor = CreateConVar("ttt_karma_innocent_kill_traitor", "50.0", "平民杀死叛徒所增加的Karma", _, true, 0.0, true, 100.0);
    ttt_karma_innocent_kill_detective = CreateConVar("ttt_karma_innocent_kill_detective", "100.0", "平民杀死侦探所损失的Karma", _, true, 0.0, true, 100.0);
    ttt_karma_traitor_kill_innocent = CreateConVar("ttt_karma_traitor_kill_innocent", "25.0", "叛徒杀死平民所增加的Karma", _, true, 0.0, true, 100.0);
    ttt_karma_traitor_kill_traitor = CreateConVar("ttt_karma_traitor_kill_traitor", "100.0", "叛徒杀死叛徒所减少的Karma", _, true, 0.0, true, 100.0);
    ttt_karma_traitor_kill_detective = CreateConVar("ttt_karma_traitor_kill_detective", "50.0", "叛徒杀死侦探所增加的Karma", _, true, 0.0, true, 100.0);
    ttt_karma_detective_kill_innocent = CreateConVar("ttt_karma_detective_kill_innocent", "25.0", "侦探杀死平民所损失的Karma", _, true, 0.0, true, 100.0);
    ttt_karma_detective_kill_traitor = CreateConVar("ttt_karma_detective_kill_traitor", "100.0", "侦探杀死叛徒所增加的Karma", _, true, 0.0, true, 100.0);
    ttt_karma_detective_kill_detective = CreateConVar("ttt_karma_detective_kill_detective", "100.0", "侦探杀死侦探所损失的Karma", _, true, 0.0, true, 100.0);
    ttt_frags_innocent_kill_innocent = CreateConVar("ttt_frags_innocent_kill_innocent", "1.0", "平民杀死平民所损失的分数", _, true, 0.0, true, 100.0);
    ttt_frags_innocent_kill_traitor = CreateConVar("ttt_frags_innocent_kill_traitor", "1.0", "平民杀死叛徒所增加的分数", _, true, 0.0, true, 100.0);
    ttt_frags_innocent_kill_detective = CreateConVar("ttt_frags_innocent_kill_detective", "5.0", "平民杀死侦探所损失的分数", _, true, 0.0, true, 100.0);
    ttt_frags_traitor_kill_innocent = CreateConVar("ttt_frags_traitor_kill_innocent", "1.0", "叛徒杀死平民所增加的分数", _, true, 0.0, true, 100.0);
    ttt_frags_traitor_kill_traitor = CreateConVar("ttt_frags_traitor_kill_traitor", "5.0", "叛徒杀死叛徒所减少的分数", _, true, 0.0, true, 100.0);
    ttt_frags_traitor_kill_detective = CreateConVar("ttt_frags_traitor_kill_detective", "2.0", "叛徒杀死侦探所增加的分数", _, true, 0.0, true, 100.0);
    ttt_frags_detective_kill_innocent = CreateConVar("ttt_frags_detective_kill_innocent", "1.0", "侦探杀死平民所损失的分数", _, true, 0.0, true, 100.0);
    ttt_frags_detective_kill_traitor = CreateConVar("ttt_frags_detective_kill_traitor", "2.0", "侦探杀死叛徒所增加的分数", _, true, 0.0, true, 100.0);
    ttt_frags_detective_kill_detective = CreateConVar("ttt_frags_detective_kill_detective", "5.0", "侦探杀死侦探所损失的分数", _, true, 0.0, true, 100.0);
    ttt_grabbermod_grab_distance = CreateConVar("ttt_grabbermod_grab_distance", "140.0", "玩家抓取物品所需的距离限制", _, true, 0.0, true, 300.0);
    ttt_grabbermod_grab_velocity_limit = CreateConVar("ttt_grabbermod_grab_velocity_limit", "35.0", "被抓取物品速度标量限制", _, true, 0.0, true, 300.0);
    AutoExecConfig(true, "TroubleInTerroristTown");
}