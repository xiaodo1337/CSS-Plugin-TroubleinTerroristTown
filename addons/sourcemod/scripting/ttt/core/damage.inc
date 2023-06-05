stock Action DamageOnTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &weapon, float damageForce[3], float damagePosition[3])
{
    decl String:classname[64];
    GetEdictClassname(inflictor, classname, sizeof(classname));
    // If entity is a trigger, then allow damage. (Map is damaging client)
    if (StrContains(classname, "trigger") > -1 || !IsClientValid(attacker))
    {
        return Plugin_Continue;
    }
    
    Action res = Plugin_Continue;
    int attackerref = attacker;
    int inflictorref = inflictor;
    float damageref = damage;
    int damagetyperef = damagetype;
    int weaponref = weapon;
    Call_StartForward(g_fwOnTakeDamage);
    Call_PushCell(victim);
    Call_PushCellRef(attackerref);
    Call_PushCellRef(inflictorref);
    Call_PushFloatRef(damageref);
    Call_PushCellRef(damagetyperef);
    Call_PushCellRef(weaponref);
    Call_PushArray(damageForce, 3);
    Call_PushArray(damagePosition, 3);
    Call_Finish(res);
    if(res == Plugin_Handled)
    {
        return Plugin_Handled;
    }
    if(res == Plugin_Changed)
    {
        attacker = attackerref;
        inflictor = inflictorref;
        damage = damageref;
        damagetype = damagetyperef;
        weapon = weaponref;
        return Plugin_Changed;
    }
    
    return Plugin_Continue;
}

stock Action DamageOnTraceAttack(int victim, int &attacker, &inflictor, float &damage, int &damagetype, int &ammotype, int hitbox, int hitgroup)
{
    Action res = Plugin_Continue;
    int attackerref = attacker;
    int inflictorref = inflictor;
    float damageref = damage;
    int damagetyperef = damagetype;
    int ammotyperef = ammotype;
    Call_StartForward(g_fwOnTraceAttack);
    Call_PushCell(victim);
    Call_PushCellRef(attackerref);
    Call_PushCellRef(inflictorref);
    Call_PushFloatRef(damageref);
    Call_PushCellRef(damagetyperef);
    Call_PushCellRef(ammotyperef);
    Call_PushCell(hitbox);
    Call_PushCell(hitgroup);
    Call_Finish(res);

    if(res == Plugin_Handled && !(damagetype & DMG_BLAST))
    {
        return Plugin_Handled;
    }
    if(res == Plugin_Changed)
    {
        attacker = attackerref;
        inflictor = inflictorref;
        damage = damageref;
        damagetype = damagetyperef;
        ammotype = ammotyperef;
    }

    if((!IsGameStart && !IsGameEnd) || IsRequiredToPlay)
    	return Plugin_Handled;
    
    // Client was shot or knifed.
    if (damagetype & DMG_CSS_BULLET)
    {
        // If attacker isn't valid, then allow damage.
        if (!IsClientValid(attacker))
            return Plugin_Continue;
        
        if(g_Client[victim].Role == TTT_ROLE_DETECTIVE || g_Client[attacker].Role == TTT_ROLE_DETECTIVE)
        {
            damage *= 0.35;
            return Plugin_Changed;
        }
    }
    
    return Plugin_Changed;
}

AddTraitorTime(int victim, int attacker)
{
    if(g_Client[attacker].Role == TTT_ROLE_TRAITOR && g_Client[victim].Role != TTT_ROLE_TRAITOR && TraitorTime > 0 && IsGameStart)
    {
        TraitorTime += ttt_traitor_kill_time.IntValue;
    }
}