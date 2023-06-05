stock InitResources()
{
    TTT_AddFilesToDownloadList("sound/ttt");
    TTT_AddFilesToDownloadList("sound/ttt/die");
    TTT_AddFilesToDownloadList("materials/ttt/icon");

    for(new i;i < sizeof(MusicList);i++)
    {
        PrecacheSound(MusicList[i].MUSIC_NAME);
        Format(Buffer, sizeof(Buffer), "sound/%s", MusicList[i].MUSIC_NAME);
    }
    for(new i = 1;i <= DeadSoundNums;i++)
    {
        Format(Buffer, sizeof(Buffer), "ttt/die/die%d.mp3", i);
        PrecacheSound(Buffer);
    }
    PrecacheSound(innocent_win_sound);
    PrecacheSound(traitor_win_sound);
    
    //Icon
    Format(Buffer, sizeof(Buffer), "materials/ttt/icon/%s.vmt", DetectiveIcon);
    PrecacheModel(Buffer);
    Format(Buffer, sizeof(Buffer), "materials/ttt/icon/%s.vmt", TraitorIcon);
    PrecacheModel(Buffer);
    Format(Buffer, sizeof(Buffer), "materials/ttt/icon/%s.vmt", CallIcon);
    PrecacheModel(Buffer);
    Format(Buffer, sizeof(Buffer), "materials/ttt/icon/%s.vmt", DNAIcon);
    PrecacheModel(Buffer);
    V_PhysicsBar = PrecacheModel("models/weapons/v_stunbaton.mdl");
    W_PhysicsBar = PrecacheModel("models/weapons/w_stunbaton.mdl");
}