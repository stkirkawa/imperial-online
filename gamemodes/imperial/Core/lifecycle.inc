/*
 * IMPERIAL ROLE PLAY - Core\lifecycle.inc
 * Основные callback-функции мода и маршрутизация диалогов.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// ====================== �������� ������� ====================== //
// =============================================================== //
main()
{
    print("\n======================================");
    print("   GAMEMODE LOADED SUCCESSFULLY");
    print("======================================\n");
}

// =============================================================== //
// ====================== OnGameModeInit ======================== //
// =============================================================== //
hook OnGameModeInit()
{
    connection = mysql_connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_BASE);
    if(mysql_errno(connection) != 0) {
        print("[ERROR] Failed to connect to MySQL!");
        SendRconCommand("exit");
        return 1;
    }
    
    mysql_tquery(connection, "SET NAMES cp1251");
    mysql_tquery(connection, "SELECT * FROM `items`", "OnItemsLoaded");
    mysql_tquery(connection, "SELECT * FROM `settings` LIMIT 1", "LoadSettings", "");
    
    DisableInteriorEnterExits();
    SetGameModeText("My Server");
    AddPlayerClass(0, 1958.3783, 1343.1572, 15.3746, 269.1425, 0,0,0,0,0,0);
    
    print("[GAMEMODE] Initialized!");

    // ---- ������������� �������� ----
    for(new i = 0; i < MAX_ADMIN_REPORTS; i++) 
    {
        ReportInfo[i][report_is_tooked] = false;
        ReportInfo[i][rep_id] = i;
        ReportInfo[i][report_player_id] = -1;
        ReportInfo[i][report_target_id] = -1;
        ReportInfo[i][report_text][0] = EOS;
        ReportInfo[i][report_when_wroted_time] = -1;
        ReportInfo[i][report_when_wroted_text][0] = EOS;
    }

    SetTimer("AutoClearningReports", 60000*10, true); 
    SetTimer("UpdatePlayerTime", 1000, true);
    SetTimer("SaveTimePlayer", 30000, true);

    return 1;
}

// =============================================================== //
// ====================== OnGameModeExit ======================== //
// =============================================================== //
hook OnGameModeExit()
{
    if(connection) mysql_close(connection);
    return 1;
}

// =============================================================== //
// ====================== OnPlayerConnect ======================= //
// =============================================================== //
hook OnPlayerConnect(playerid)
{
    ResetPlayerData(playerid);
    
    if(SettingServer[server_status] == 1) {
        if(AdminInfo[playerid][aRank] < 7) {
            SendClientMessage(playerid, 0xFF0000FF, "������ ������ �� ����������� �������!");
            SetTimerEx("DelayedKick", 500, false, "i", playerid);
            return 0;
        }
    }
    
    SettingServer[player_online]++;
    SaveServerSettings();
    
    new name[MAX_PLAYER_NAME], ip[16];
    GetPlayerName(playerid, name, sizeof(name));
    GetPlayerIp(playerid, ip, sizeof(ip));
    
    format(pInfo[playerid][pName], 24, name);
    format(pInfo[playerid][pIP], 16, ip);
    format(pInfo[playerid][pLastIP], 16, ip);
    
    mysql_format(connection, query, sizeof(query),
        "SELECT u.*, b.ban_id, b.ban_admin, b.ban_reason, b.ban_date, b.ban_expire FROM `users` u "\
        "LEFT JOIN `banlist` b ON (b.user_id = u.id OR b.ip = '%e') "\
        "WHERE u.`name` = '%e' ORDER BY b.ban_id DESC LIMIT 1",
        pInfo[playerid][pIP], pInfo[playerid][pName]);
        
    mysql_tquery(connection, query, "OnPlayerCheck", "i", playerid);
    return 1;
}

// =============================================================== //
// ====================== OnPlayerDisconnect ==================== //
// =============================================================== //
hook OnPlayerDisconnect(playerid, reason)
{
    SettingServer[player_online]--;
    
    if(pInfo[playerid][pLogged]) {
        new current_ip[16];
        GetPlayerIp(playerid, current_ip, sizeof(current_ip));
        format(pInfo[playerid][pLastIP], 16, current_ip);
        
        SavePlayerData(playerid);
        SaveAdminData(playerid);
        SavePlayerInventory(playerid);
        SaveServerSettings();
    }
    
    ResetPlayerData(playerid);
    return 1;
}

// =============================================================== //
// ======================== OnPlayerSpawn ======================= //
// =============================================================== //
hook OnPlayerSpawn(playerid)
{
    if(!pInfo[playerid][pLogged]) return 1;
    
    SetMoney(playerid, pInfo[playerid][pMoney]);
    SetPlayerSkin(playerid, pInfo[playerid][pSkin]);
    SetPlayerScore(playerid, pInfo[playerid][pScore]);
    SetCameraBehindPlayer(playerid);
    TogglePlayerSpectating(playerid, false);
    
    pInfo[playerid][pSpawned] = true;
    AFKTime[playerid] = 0;
    IsAFK[playerid] = 0;
    return 1;
}

// =============================================================== //
// ====================== OnPlayerRequestClass ================== //
// =============================================================== //
hook OnPlayerRequestClass(playerid, classid)
{
    if(pInfo[playerid][pLogged] && pInfo[playerid][pSpawned]) {
        SetPlayerPos(playerid, 1958.3783, 1343.1572, 15.3746);
        SetPlayerCameraPos(playerid, 1958.3783, 1343.1572, 15.3746);
        SetPlayerCameraLookAt(playerid, 1958.3783, 1343.1572, 15.3746);
    }
    return 1;
}

// =============================================================== //
// ====================== OnDialogResponse ====================== //
// =============================================================== //
hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid) {
        // ---- ����������� / ����� ----
        case DIALOG_REGISTER:       return DialogRegister(playerid, response, inputtext);
        case DIALOG_LOGIN:          return DialogLogin(playerid, response, inputtext);
        case DIALOG_GENDER:         return DialogGender(playerid, response, listitem);
        case DIALOG_SKIN:           return DialogSkin(playerid, response, listitem);

        // ---- ��������� ----
        case DIALOG_INV_MAIN:       return DialogInvMain(playerid, response, listitem);
        case DIALOG_INV_ACTION:     return DialogInvAction(playerid, response, listitem);
        case DIALOG_INV_DROP:       return DialogInvDrop(playerid, response, inputtext);
        case DIALOG_INV_GIVE:       return DialogInvGive(playerid, response, inputtext);

        // ---- �����-������ ----
        case DIALOG_ADMIN_LOGIN:    return DialogAdminLogin(playerid, response, inputtext);
        case DIALOG_ADMIN_REGISTER: return DialogAdminRegister(playerid, response, inputtext);
        case DIALOG_ADMIN_PANEL_LIST: return DialogAdminPanel(playerid, response, listitem);
        case DIALOG_ADMIN_COMMANDS: return DialogAdminCommands(playerid, response, listitem);

        // ---- ��������� ������� ----
        case DIALOG_SETTING_SERVER: return DialogSettingsServer(playerid, response, listitem);
        case DIALOG_SET_MULTIPLIER: return DialogMultiplier(playerid, response, inputtext);
        case DIALOG_INPUT_DELITE_ACOUNT: return DialogDeliteAcount(playerid, response, inputtext);

        // ---- ������� ������� ----
        case DIALOG_REPORT_INPUT:   return DialogReportInput(playerid, response, inputtext);
        case DIALOG_AREPORT_LISTS:  return DialogAreportLists(playerid, response, listitem);
        case DIALOG_AREPORT_ANSWER: return DialogAnswerReport(playerid, response, inputtext);
    }
    return 0;
}
