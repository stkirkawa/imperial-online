/*
 * IMPERIAL ROLE PLAY - Systems\Core\stocks.inc
 * Общие игровые вспомогательные функции и серверные утилиты.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// ======================== �������� ������� ==================== //
// =============================================================== //

// -------------------- ����������-��� --------------------
stock ProxDetector(playerid, Float:max_range, color, const string[], Float:max_ratio = 1.6)
{
    new Float:pos_x, Float:pos_y, Float:pos_z;
    if(!GetPlayerPos(playerid, pos_x, pos_y, pos_z)) return 0;
    
    new Float:color_r = float(color >> 24 & 0xFF);
    new Float:color_g = float(color >> 16 & 0xFF);
    new Float:color_b = float(color >> 8 & 0xFF);
    new Float:range_with_ratio = max_range * max_ratio;
    
    foreach(new i : Player) {
        if(!IsPlayerStreamedIn(playerid, i)) continue;
        
        new Float:range = GetPlayerDistanceFromPoint(i, pos_x, pos_y, pos_z);
        if(range > max_range) continue;
        
        new Float:range_ratio = (range_with_ratio - range) / range_with_ratio;
        new clr_r = floatround(range_ratio * color_r);
        new clr_g = floatround(range_ratio * color_g);
        new clr_b = floatround(range_ratio * color_b);
        
        SendClientMessage(i, (color & 0xFF) | (clr_b << 8) | (clr_g << 16) | (clr_r << 24), string);
    }
    SendClientMessage(playerid, color, string);
    return 1;
}

// -------------------- �������� ��������� --------------------
stock SendError(playerid, const text[])
{
    new string[128];
    format(string, sizeof(string), "������ | {FFFFFF}%s", text);
    SendClientMessage(playerid, COLOR_ERROR, string);
    return 1;
}

stock SendInfo(playerid, const text[])
{
    new string[128];
    format(string, sizeof(string), "���������� | {FFFFFF}%s", text);
    SendClientMessage(playerid, COLOR_INFO, string);
    return 1;
}

stock SendAdminNotify(playerid, const text[])
{
    new string[128];
    format(string, sizeof(string), "������� | {FFFFFF}%s", text);
    SendClientMessage(playerid, COLOR_ADMIN, string);
    return 1;
}

stock SendCommandInfo(playerid, const text[])
{
    new string[128];
    format(string, sizeof(string), "�����������: %s", text);
    SendClientMessage(playerid, COLOR_GREY, string);
    return 1;
}

// -------------------- ���������� ������ --------------------
forward UpdatePlayerTime();
public UpdatePlayerTime()
{
    new current_tick = GetTickCount();

    foreach(new i : Player) {
        if(current_tick - LastTick[i] > 3000) {
            AFKTime[i]++; 
            IsAFK[i] = 1;  

            new a[24];
            format(a, sizeof(a), "� AFK: %d ���", AFKTime[i]);
            SetPlayerChatBubble(i, a, 0xFFFF00FF, 15.0, 1100); 
        } else {
            if(AdminInfo[i][aLogged] && AdminInfo[i][aRank] > 0) {
                AdminInfo[i][aOnlineTime]++;
                
                new dayOfWeek = (4 + (gettime() / 86400)) % 7;
                if(dayOfWeek == 0) dayOfWeek = 6;
                else dayOfWeek--;
                
                AdminInfo[i][aWeekDays][dayOfWeek]++;
            }
        } 
        if(pInfo[i][pMute] > 0) {
            pInfo[i][pMute]--;
            if(pInfo[i][pMute] == 0) {
                SendInfo(i, "���� ���������� ���� �����������!");
            }
        } 
    }
    return 1;
}

forward SaveTimePlayer();
public SaveTimePlayer()
{
    foreach(new i : Player) {
        if(IsPlayerConnected(i) && pInfo[i][pLogged] && pInfo[i][pRegister]) {
            SavePlayerData(i);
            SaveAdminData(i);
            SavePlayerInventory(i);
            SaveServerSettings();
        }
    }
    return 1;
}

// ------------------------- ��������� ������� -------------------------
forward LoadSettings();
public LoadSettings()
{
    if(cache_num_rows() > 0) {
        cache_get_value_name_int(0, "multiplier", SettingServer[multiplier]);
        cache_get_value_name_int(0, "status", SettingServer[server_status]);
        cache_get_value_name_int(0, "player_online", SettingServer[player_online]);
    }
    return 1;
}

forward CloseServer();
public CloseServer()
{
    SendRconCommand("password tech123");
    foreach(new i : Player) {
        if(IsPlayerConnected(i) && pInfo[i][pLogged]) {
            SavePlayerData(i);
            SaveAdminData(i);
            SavePlayerInventory(i);
            SaveServerSettings();
        }
    }
    
    foreach(new i : Player) {
        if(IsPlayerConnected(i)) {
            if(AdminInfo[i][aRank] >= 7) {
                SendClientMessage(i, 0xFF0000FF, "�� ��������, ��� ��� � ��� 7+ �������!");
            } else {
                SendClientMessage(i, 0xFF0000FF, "������ ������ �� ����������� �������!");
                SetTimerEx("DelayedKick", 3000, false, "i", i);
            }
        }
    }
    
    SettingServer[server_status] = 1;
    SaveServerSettings();

    SetTimer("ShutdownServer", 5000, false);
    return 1;
}

forward ShutdownServer();
public ShutdownServer()
{
    SendRconCommand("exit");
    return 1;
}

forward OnCheckAccountDelete(adminid, const target_name[]);
public OnCheckAccountDelete(adminid, const target_name[])
{
    if(cache_num_rows() == 0)
        return SendError(adminid, "������� � ����� ��������� �� ������!");
    
    new target_id;
    cache_get_value_name_int(0, "id", target_id);
    
    mysql_format(connection, query, sizeof(query), 
        "DELETE FROM `users` WHERE `id` = '%d'", target_id);
    mysql_tquery(connection, query);
    
    mysql_format(connection, query, sizeof(query), 
        "DELETE FROM `admins` WHERE `player_id` = '%d'", target_id);
    mysql_tquery(connection, query);
    
    mysql_format(connection, query, sizeof(query), 
        "DELETE FROM `banlist` WHERE `user_id` = '%d'", target_id);
    mysql_tquery(connection, query);
    
    mysql_format(connection, query, sizeof(query), 
        "DELETE FROM `ip_bans` WHERE `account_id` = '%d'", target_id);
    mysql_tquery(connection, query);
    
    mysql_format(connection, query, sizeof(query), 
        "DELETE FROM `inventory` WHERE `player_id` = '%d'", target_id);
    mysql_tquery(connection, query);

    mysql_format(connection, query, sizeof(query), 
        "DELETE FROM `mutes` WHERE `user_id` = '%d'", target_id);
    mysql_tquery(connection, query);
    
    foreach(new i : Player) {
        if(IsPlayerConnected(i) && pInfo[i][pID] == target_id) {
            SendClientMessage(i, COLOR_PUNUSH, "��� ������� ��� �����!");
            SetTimerEx("DelayedKick", 500, false, "i", i);
        }
    }
    
    new string[128];
    format(string, sizeof(string), "������� ������ %s ��� ������� �����!", target_name);
    SendClientMessage(adminid, COLOR_PUNUSH, string);
    
    return 1;
}

stock SaveServerSettings()
{
    mysql_format(connection, query, sizeof(query), 
        "UPDATE `settings` SET `multiplier` = '%d', `status` = '%d', `player_online` = '%d' WHERE `id` = 1",
        SettingServer[multiplier],
        SettingServer[server_status],
        SettingServer[player_online]
    );
    mysql_tquery(connection, query);
}

// -------------------- �������� �� ������������ --------------------
stock IsValidAdminPassword(const str[])
{
    for(new i = 0; str[i] != EOS; i++) {
        new c = str[i];
        if(!('A' <= c <= 'Z') && !('a' <= c <= 'z') && !('0' <= c <= '9') &&
           c != '_' && c != '-' && c != '!' && c != '@' && c != '#' && c != '$' &&
           c != '%' && c != '^' && c != '&' && c != '*' && c != '.')
            return 0;
    }
    return 1;
}

// -------------------- ���������� --------------------
stock ConvertValueMinSecunds(seconds)
{
    new str[16];
    new hours = seconds / 3600;
    new minutes = (seconds % 3600) / 60;
    new secs = seconds % 60;
    
    format(str, sizeof(str), "%02d:%02d:%02d", hours, minutes, secs);
    return str;
}

stock FormatTime(seconds)
{
    new str[16];
    format(str, sizeof(str), "%02d:%02d:%02d", 
        seconds / 3600, 
        (seconds % 3600) / 60, 
        seconds % 60
    );
    return str;
}

stock format_timestamp(timestamp, dest[], size)
{
    new days = timestamp / 86400;
    new year = 1970 + (days / 365); 
    new month = ((days % 365) / 30) + 1;
    new day = ((days % 365) % 30) + 1;
    
    format(dest, size, "%02d.%02d.%04d", day, month, year);
    return 1;
}
// -------------------- ����������� / ����� --------------------
stock ShowGenderSelection(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_GENDER, DIALOG_STYLE_LIST,
        "{FF0000}����� ����",
        "{FFFF00}�������\n{FF66B2}�������\n\n{FFFFFF}�������� ���:",
        "�������", "�����");
}

stock ShowSkinSelection(playerid)
{
    new string[1024];
    strcat(string, "{FFFFFF}�������� ����:\n\n", sizeof(string));
    
    if(pInfo[playerid][pGender] == 1) {
        strcat(string, "{FFFF00}������� �����:\n", sizeof(string));
        for(new i = 0; i < sizeof(MaleSkins); i++) {
            format(string, sizeof(string), "%s{33CC33}%d. {FFFFFF}���� %d (ID: %d)\n",
                string, i+1, i+1, MaleSkins[i]);
        }
    } else {
        strcat(string, "{FF66B2}������� �����:\n", sizeof(string));
        for(new i = 0; i < sizeof(FemaleSkins); i++) {
            format(string, sizeof(string), "%s{33CC33}%d. {FFFFFF}���� %d (ID: %d)\n",
                string, i+1, i+1, FemaleSkins[i]);
        }
    }
    ShowPlayerDialog(playerid, DIALOG_SKIN, DIALOG_STYLE_LIST,
        "{FF0000}����� �����", string, "�������", "�����");
}

// -------------------- ���������� ������ ������ --------------------
stock SavePlayerData(playerid)
{
    if(pInfo[playerid][pID] == 0) return 0;

    new lastip[16];
    GetPlayerIp(playerid, lastip, sizeof(lastip));
    
    format(pInfo[playerid][pLastIP], 16, lastip);
    
    mysql_format(connection, query, sizeof(query),
        "UPDATE `users` SET `score` = %d, `exp` = %d, `money` = %d, `skin` = %d, `mute` = %d, `last_ip` = '%e' WHERE `id` = %d",
        pInfo[playerid][pScore], pInfo[playerid][pExp], pInfo[playerid][pMoney],
        pInfo[playerid][pSkin], pInfo[playerid][pMute], pInfo[playerid][pLastIP], pInfo[playerid][pID]);
    mysql_tquery(connection, query);
    return 1;
}

// -------------------- ��������� ������ --------------------
stock ResetPlayerData(playerid)
{
    pInfo[playerid][pID] = 0;
    pInfo[playerid][pName][0] = EOS;
    pInfo[playerid][pPassword][0] = EOS;
    pInfo[playerid][pGender] = 0;
    pInfo[playerid][pSkin] = 0;
    pInfo[playerid][pScore] = 0;
    pInfo[playerid][pExp] = 0;
    pInfo[playerid][pMoney] = 0;
    pInfo[playerid][pRegister] = 0;
    pInfo[playerid][pIP][0] = EOS;
    pInfo[playerid][pLastIP][0] = EOS;
    pInfo[playerid][pLogged] = false;
    pInfo[playerid][pSpawned] = false;
    pInfo[playerid][pMute] = 0;
    ResetPlayerMoney(playerid);
    SetPlayerScore(playerid, 0);
    
    // ---- ������� PVar ��� ������ ----
    DeletePVar(playerid, "Spectating");
    DeletePVar(playerid, "SpecTarget");
    DeletePVar(playerid, "SpecPosX");
    DeletePVar(playerid, "SpecPosY");
    DeletePVar(playerid, "SpecPosZ");
    DeletePVar(playerid, "SpecAngle");
    DeletePVar(playerid, "SpecInterior");
    DeletePVar(playerid, "SpecVirtualWorld");
    
    // ---- ������� �������� ----
    if(GetPVarInt(playerid, "report_id")) {
        new report_id = GetPVarInt(playerid, "report_id");
        if(report_id >= 0 && report_id < MAX_ADMIN_REPORTS) {
            ResetReport(report_id);
        }
        DeletePVar(playerid, "report_id");
    }
    
    // ---- ����� �����-������ ----
    AdminInfo[playerid][aRank] = 0;
    AdminInfo[playerid][aLogged] = false;
    AdminInfo[playerid][aPassword][0] = EOS;
    AdminInfo[playerid][aPrefix][0] = EOS;
    AdminInfo[playerid][aAnsReport] = 0;
    AdminInfo[playerid][aOnlineTime] = 0;
    AdminInfo[playerid][aID] = 0;
    AdminInfo[playerid][aLastResetWeek] = 0;
    for(new i = 0; i < 7; i++) {
        AdminInfo[playerid][aWeekDays][i] = 0;
    }

    // ---- ����� AFK ----
    IsAFK[playerid] = 0;
    AFKTime[playerid] = 0;
    LastTick[playerid] = 0;
    
    // ---- ����� ��������� ----
    ClearPlayerInventory(playerid);
    PlayerSelectedSlot[playerid] = -1;
    
    return 1;
}

// -------------------- ������ --------------------
stock GiveMoney(playerid, amount)
{
    if(!IsPlayerConnected(playerid)) return 0;
    pInfo[playerid][pMoney] += amount;
    GivePlayerMoney(playerid, amount);
    
    mysql_format(connection, query, sizeof(query),
        "UPDATE `users` SET `money` = %d WHERE `id` = %d",
        pInfo[playerid][pMoney], pInfo[playerid][pID]);
    mysql_tquery(connection, query);
    return 1;
}

stock ResetMoney(playerid, amount)
{
    if(!IsPlayerConnected(playerid)) return 0;
    pInfo[playerid][pMoney] -= amount;
    GivePlayerMoney(playerid, -amount);
    
    mysql_format(connection, query, sizeof(query),
        "UPDATE `users` SET `money` = %d WHERE `id` = %d",
        pInfo[playerid][pMoney], pInfo[playerid][pID]);
    mysql_tquery(connection, query);
    return 1;
}

stock SetMoney(playerid, amount)
{
    if(!IsPlayerConnected(playerid)) return 0;
    pInfo[playerid][pMoney] = amount;
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, amount);
    
    mysql_format(connection, query, sizeof(query),
        "UPDATE `users` SET `money` = %d WHERE `id` = %d",
        pInfo[playerid][pMoney], pInfo[playerid][pID]);
    mysql_tquery(connection, query);
    return 1;
}

// --------- ���� ���� ---------
forward DelayedKick(playerid);
public DelayedKick(playerid)
{
    if(IsPlayerConnected(playerid))
        Kick(playerid);
    return 1;
}

// -------------------- ��������� ����� --------------------
stock PlayerName(playerid)
{
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));
    return name;
}

//-------------------- ��������� ���������� ���� --------------------
stock ShowBanDialog(playerid)
{
    cache_get_value_name_int(0, "ban_id", pInfo[playerid][pBanID]);
    cache_get_value_name(0, "ban_admin", pInfo[playerid][pBanAdmin], 24);
    cache_get_value_name(0, "ban_reason", pInfo[playerid][pBanReason], 64);
    cache_get_value_name_int(0, "ban_date", pInfo[playerid][pBanDate]);
    cache_get_value_name_int(0, "ban_expire", pInfo[playerid][pBanExpire]);

    new seconds_left = pInfo[playerid][pBanExpire] - gettime();
    new days = seconds_left / 86400;
    new hours = (seconds_left % 86400) / 3600;
    new minutes = ((seconds_left % 86400) % 3600) / 60;

    new date_start[32], date_end[32];
    format_timestamp(pInfo[playerid][pBanDate], date_start, sizeof(date_start));
    format_timestamp(pInfo[playerid][pBanExpire], date_end, sizeof(date_end));

    new ban_str[600];
    format(ban_str, sizeof(ban_str), 
        "{FFFFFF}��� ������� {FF0000}������������{FFFFFF}!\n\n"\
        "{808080}ID ����������: {FFFF00}#%d\n"\
        "{808080}ID ��������: {FFFF00}%d\n"\
        "{808080}�������: {FFFF00}%s\n"\
        "{808080}������� �������������: {FFFFFF}%s\n"\
        "{808080}�������: {FF9900}%s\n\n"\
        "{808080}���� ������: {FFFFFF}%s\n"\
        "{808080}���� ���������: {FFFFFF}%s\n"\
        "{808080}�������� �� �������: {00FF00}%d ��. %d ���. %d ���.\n\n"\
        "{FFFFFF}���� �� �� �������� � ��������, ���������� �� �����.",
        pInfo[playerid][pBanID], pInfo[playerid][pID], pInfo[playerid][pName], 
        pInfo[playerid][pBanAdmin], pInfo[playerid][pBanReason], date_start, date_end, days, hours, minutes);
        
    ShowPlayerDialog(playerid, 9999, DIALOG_STYLE_MSGBOX, "{FF0000}����������", ban_str, "�������", "");
    SetTimerEx("DelayedKick", 500, false, "i", playerid);
    return 1;
}
