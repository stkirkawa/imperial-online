/*
 * IMPERIAL ROLE PLAY - Systems\Admin\stocks.inc
 * Вспомогательные функции администраторов и операции модерации.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// =============================================================== //
// ====================== ����� ����� =========================== //
// =============================================================== //

// ------------------ ����� ���������� ������� ------------------
stock CheckAndResetWeeklyStats(playerid)
{
    if(!IsPlayerConnected(playerid)) return 0;
    if(pInfo[playerid][pID] == 0) return 0;
    if(AdminInfo[playerid][aRank] == 0) return 0;
    
    new current_day = get_day_of_week();
    
    if(current_day == 0) {
        new current_time = gettime();
        new last_reset = AdminInfo[playerid][aLastResetWeek];
        
        new current_date = current_time / 86400;
        new last_reset_date = last_reset / 86400;
        
        if(current_date != last_reset_date) {
            for(new i = 0; i < 7; i++) {
                AdminInfo[playerid][aWeekDays][i] = 0;
            }
            AdminInfo[playerid][aLastResetWeek] = current_time;
            
            mysql_format(connection, query, sizeof(query),
                "UPDATE `admins` SET \
                `online_time` = 0, \
                `mon` = 0, `tue` = 0, `wed` = 0, \
                `thu` = 0, `fri` = 0, `sat` = 0, \
                `sun` = 0, \
                `last_reset_week` = %d \
                WHERE `player_id` = %d",
                current_time, pInfo[playerid][pID]);
            mysql_tquery(connection, query);
            
            if(IsPlayerConnected(playerid)) {
                SendAdminNotify(playerid, "���� ��������� ���������� ������� ��������!");
            }
        }
    }
    return 1;
}

stock get_day_of_week()
{
    new timestamp = gettime();
    new days = timestamp / 86400;
    return (days + 3) % 7;
}

// ---------------- ��� ������������� ----------------
stock SendAdminMessage(color, const message[])
{
    foreach(new i : Player) {
        if(IsPlayerConnected(i) && AdminInfo[i][aLogged]) {
            SendClientMessage(i, color, message);
        }
    }
    return 1;
}

stock GetAdminPrefix(playerid)
{
    new prefix[32];
    
    if(strlen(AdminInfo[playerid][aPrefix]) > 0) {
        format(prefix, sizeof(prefix), "%s", AdminInfo[playerid][aPrefix]);
        return prefix;
    }
    
    new rank = AdminInfo[playerid][aRank];
    if(rank >= 0 && rank < sizeof(AdminStandartPrefix)) {
        format(prefix, sizeof(prefix), "%s", AdminStandartPrefix[rank]);
    } else {
        format(prefix, sizeof(prefix), "�������������");
    }
    
    return prefix;
}

// ------------ ������� ������� ------------
stock ShowPlayer_Admin_Report_List(playerid)
{
    new string[2048];
    new count = 0; 
    
    format(string, sizeof(string), "ID\t���-����\n");
    
    for(new i = 0; i < MAX_ADMIN_REPORTS; i++) {
        if(ReportInfo[i][report_player_id] == -1) continue;
        
        format(string, sizeof(string), "%s%d\t%s\n", 
            string, i, PlayerName(ReportInfo[i][report_player_id]));
        count++; 
    }
    
    if(count == 0) {
        return SendError(playerid, "��� �������� ����� ��� ���������");
    }

    ShowPlayerDialog(playerid, DIALOG_AREPORT_LISTS, DIALOG_STYLE_TABLIST_HEADERS, 
        "{FF0000}������ �������� �����", string, "�������", "�������");
    return 1;
}

stock ShowPlayer_Admin_Report_Answer(playerid, report_id)
{
    SetPVarInt(playerid, "report_id_to_answer", report_id);
    ReportInfo[report_id][report_is_tooked] = true;

    new dialog_header[64], dialog_text[512];

    format(dialog_header, sizeof(dialog_header), "{FF0000}����� �� ������");
    format(dialog_text, sizeof(dialog_text), 
        "{FFFFFF}���: {FF6347}%s[%d]{FFFFFF}\n\
        {FFFFFF}����������: {FF6347}%s[%d]{FFFFFF}\n\
        {FFFFFF}������: {FFFFFF}%s\n\n\
        {FFFFFF}������� � ���� ���� ��� �����: \n\
        {FFFF00}�� - {FFFFFF}����������������� � ������, {FFFF00}�� - {FFFFFF}������� �� �����������\n\
        {FFFF00}�� ����� - {FFFFFF}��������� ������ ������ �� �����",
        PlayerName(ReportInfo[report_id][report_player_id]),
        ReportInfo[report_id][report_player_id],
        PlayerName(ReportInfo[report_id][report_target_id]),
        ReportInfo[report_id][report_target_id],
        ReportInfo[report_id][report_text]
    );

    ShowPlayerDialog(playerid, DIALOG_AREPORT_ANSWER, DIALOG_STYLE_INPUT, 
        dialog_header, dialog_text, "��������", "�����");
    return 1;
}

stock ShowReportDialog(playerid)
{
    new dialog_text[512];
    new dialog_text2[300];

    format(dialog_text, sizeof(dialog_text),
        "{FFFFFF}�� ����������� ��������� ������ ������������� �������\n\
        {FFFFFF}������� ���� ������, �������, ��� �� �� ������ ������������:\n\
        {FF4500}���������:\n\
        {FF0000}- {FFFFFF}�����������, ��� � ������\n\
        {FF0000}- {FFFFFF}������������� ���������, ������, ����� � �������\n\
        {FF0000}��������� �� ���������:\n"
    );

    format(dialog_text2, sizeof(dialog_text2),
        "{FF0000}- {FFFFFF}��� ������ {FF0000}(mute)\n\
        {FF0000}- {FFFFFF}�������������� {FF0000}(warn)\n\
        {FF0000}- {FFFFFF}���������� ���� {FF0000}(ban)\n\n\
        {FFFFFF}������� ID ������ � ����� ������ ����� ������:\n\
        {FF4500}������: {FFFFFF}5 ��"
    );

    strcat(dialog_text, dialog_text2, sizeof(dialog_text));
    
    ShowPlayerDialog(playerid, DIALOG_REPORT_INPUT, DIALOG_STYLE_INPUT, 
        "{FF0000}�������� ������", dialog_text, "���������", "������");
    return 1;
}

stock ResetReport(report_id)
{
    if(report_id < 0 || report_id >= MAX_ADMIN_REPORTS) return 0;
    ReportInfo[report_id][report_is_tooked] = false;
    ReportInfo[report_id][rep_id] = report_id;
    ReportInfo[report_id][report_player_id] = -1;
    ReportInfo[report_id][report_target_id] = -1;
    ReportInfo[report_id][report_text][0] = EOS;
    ReportInfo[report_id][report_when_wroted_time] = -1;
    ReportInfo[report_id][report_when_wroted_text][0] = EOS;
    return 1;
}

stock StartSpectate(playerid, targetid)
{
    new Float: reX, Float: reY, Float: reZ, Float: reA;
    GetPlayerPos(playerid, reX, reY, reZ);
    GetPlayerFacingAngle(playerid, reA);
    
    SetPVarFloat(playerid, "re_X", reX);
    SetPVarFloat(playerid, "re_Y", reY);
    SetPVarFloat(playerid, "re_Z", reZ);
    SetPVarFloat(playerid, "re_A", reA);
    SetPVarInt(playerid, "re_int", GetPlayerInterior(playerid));
    SetPVarInt(playerid, "re_virt", GetPlayerVirtualWorld(playerid));
    
    SetPVarInt(playerid, "Spectating", 1);
    TogglePlayerSpectating(playerid, true);
    SetPlayerInterior(playerid, GetPlayerInterior(targetid));
    SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));
    
    if(IsPlayerInAnyVehicle(targetid))
        PlayerSpectateVehicle(playerid, GetPlayerVehicleID(targetid));
    else
        PlayerSpectatePlayer(playerid, targetid);
    
    SetPVarInt(playerid, "SET_ADM_POS", 1);
    SetPVarInt(playerid, "follow_from_report", 1);
    
    new string[128];
    format(string, sizeof(string), "[A] ������������� %s ����� ������� �� ������� %s", 
        PlayerName(playerid), PlayerName(targetid));
    SendAdminMessage(COLOR_GREY, string);
    return 1;
}

stock TeleportToPlayer(playerid, targetid)
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    
    new target_virtualworld = GetPlayerVirtualWorld(targetid);
    new target_interior = GetPlayerInterior(targetid);
    
    SetPlayerPos(playerid, x, y, z);
    SetPlayerVirtualWorld(playerid, target_virtualworld);
    SetPlayerInterior(playerid, target_interior);

    new string[128];
    format(string, sizeof(string), "[A] ������������� %s[%d] ���������������� � ������ %s[%d]", 
        PlayerName(playerid), playerid, PlayerName(targetid), targetid);
    SendAdminMessage(COLOR_GREY, string);

    format(string, sizeof(string), "�� ����������������� � ������ %s[%d]", PlayerName(targetid), targetid);
    SendClientMessage(playerid, COLOR_GREY, string);
    return 1;
}

// ------------ ���������� ������� ------------
stock SaveAdminData(playerid) {
    if(pInfo[playerid][pID] == 0 || AdminInfo[playerid][aRank] == 0) return 0;
    
    mysql_format(connection, query, sizeof(query),
        "UPDATE `admins` SET \
        rank = %d, \
        password = '%e', \
        prefix = '%e', \
        ans_report = %d, \
        online_time = %d, \
        mon = %d, tue = %d, wed = %d, \
        thu = %d, fri = %d, sat = %d, \
        sun = %d, last_reset_week = %d \
        WHERE player_id = %d LIMIT 1",
        AdminInfo[playerid][aRank],
        AdminInfo[playerid][aPassword],
        AdminInfo[playerid][aPrefix],
        AdminInfo[playerid][aAnsReport],
        AdminInfo[playerid][aOnlineTime],
        AdminInfo[playerid][aWeekDays][0],
        AdminInfo[playerid][aWeekDays][1],
        AdminInfo[playerid][aWeekDays][2],
        AdminInfo[playerid][aWeekDays][3],
        AdminInfo[playerid][aWeekDays][4],
        AdminInfo[playerid][aWeekDays][5],
        AdminInfo[playerid][aWeekDays][6],
        AdminInfo[playerid][aLastResetWeek],
        pInfo[playerid][pID]);
    
    mysql_tquery(connection, query);
    return 1;
}

// ------------ ������ ��������� � �������� ------------
forward CheckMuteOnline(adminid, targetid, minutes, reason[64]);
public CheckMuteOnline(adminid, targetid, minutes, reason[64])
{
    if(cache_num_rows() > 0)
        return SendError(adminid, "� ����� ������ ��� ���� �������� ���!");

    new admin_name[MAX_PLAYER_NAME], t_name[MAX_PLAYER_NAME], t_ip[16], string[144];
    GetPlayerName(adminid, admin_name, sizeof(admin_name));
    GetPlayerName(targetid, t_name, sizeof(t_name));
    GetPlayerIp(targetid, t_ip, sizeof(t_ip));

    new time_expire = gettime() + (minutes * 60);

    mysql_format(connection, query, sizeof(query), 
        "INSERT INTO `mutes` (`user_id`, `name`, `ip`, `mute_admin`, `mute_reason`, `mute_time`) VALUES ('%d', '%e', '%e', '%e', '%e', '%d')", 
        pInfo[targetid][pID], t_name, t_ip, admin_name, reason, time_expire);
    mysql_tquery(connection, query);

    pInfo[targetid][pMute] = time_expire;

    format(string, sizeof(string), "������������� %s ������� ������ %s �� %d ���. �������: %s", admin_name, t_name, minutes, reason);
    SendClientMessageToAll(COLOR_PUNUSH, string);
    return 1;
}

forward CheckOfflineMute(adminid, const target_name[], minutes, const reason[]);
public CheckOfflineMute(adminid, const target_name[], minutes, const reason[])
{
    if(cache_num_rows() > 0)
        return SendError(adminid, "� ����� ������ ��� ���� �������� ���!");

    mysql_format(connection, query, sizeof(query), 
        "SELECT `id`, `ip` FROM `users` WHERE `name` = '%e' LIMIT 1", target_name);
    mysql_tquery(connection, query, "OnOfflineMute", "dsisi", adminid, target_name, minutes, reason, gettime());
    return 1;
}

forward OnOfflineMute(adminid, const target_name[], minutes, const reason[], time_now);
public OnOfflineMute(adminid, const target_name[], minutes, const reason[], time_now)
{
    if(cache_num_rows() == 0)
        return SendError(adminid, "������� � ����� ��������� �� ������!");

    new target_id_db, target_ip[16];
    cache_get_value_name_int(0, "id", target_id_db);
    cache_get_value_name(0, "ip", target_ip, sizeof(target_ip)); 

    new admin_name[MAX_PLAYER_NAME], string[144];
    GetPlayerName(adminid, admin_name, sizeof(admin_name));

    new time_expire = time_now + (minutes * 60);

    mysql_format(connection, query, sizeof(query), 
        "INSERT INTO `mutes` (`user_id`, `name`, `ip`, `mute_admin`, `mute_reason`, `mute_time`) VALUES ('%d', '%e', '%e', '%e', '%e', '%d')", 
        target_id_db, target_name, target_ip, admin_name, reason, time_expire);
    mysql_tquery(connection, query);

    format(string, sizeof(string), "������������� %s ������� ������ %s (�������) �� %d ���. �������: %s", admin_name, target_name, minutes, reason);
    SendClientMessageToAll(COLOR_PUNUSH, string);
    return 1;
}

forward CheckUnoffMute(adminid, const target_name[]);
public CheckUnoffMute(adminid, const target_name[])
{
    if(cache_num_rows() == 0)
        return SendError(adminid, "� ����� ������ ��� ��������� ����!");

    mysql_format(connection, query, sizeof(query), 
        "DELETE FROM `mutes` WHERE `name` = '%e' AND `mute_time` > '%d'", 
        target_name, gettime());
    mysql_tquery(connection, query);

    new admin_name[MAX_PLAYER_NAME], string[144];
    GetPlayerName(adminid, admin_name, sizeof(admin_name));

    format(string, sizeof(string), "������������� %s ���� ��� � ������ %s (�������)", admin_name, target_name);
    SendClientMessageToAll(COLOR_PUNUSH, string);
    return 1;
}

forward CheckAlreadyBannedOffline(adminid, const target_name[], days, const reason[]);
public CheckAlreadyBannedOffline(adminid, const target_name[], days, const reason[])
{
    if(cache_num_rows() > 0)
    {
        return SendError(adminid, "� ����� ������ ��� ���� �������� ���!");
    }

    mysql_format(connection, query, sizeof(query), 
        "SELECT `id`, `ip` FROM `users` WHERE `name` = '%e' LIMIT 1", target_name);
    
    mysql_tquery(connection, query, "OnOfflineBanCheck", "dsisi", adminid, target_name, days, reason, gettime());
    return 1;
}

forward OnOfflineBanCheck(adminid, const target_name[], days, const reason[], time_now);
public OnOfflineBanCheck(adminid, const target_name[], days, const reason[], time_now)
{
    if(cache_num_rows() == 0)
    {
        return SendError(adminid, "������� � ����� ��������� �� ������ � ���� ������!");
    }

    new target_id_db, target_ip[16];
    cache_get_value_name_int(0, "id", target_id_db);
    cache_get_value_name(0, "ip", target_ip, sizeof(target_ip)); 

    new admin_name[MAX_PLAYER_NAME], string[144];
    GetPlayerName(adminid, admin_name, sizeof(admin_name));

    new time_expire = time_now + (days * 86400);

    mysql_format(connection, query, sizeof(query), 
        "INSERT INTO `banlist` (`user_id`, `name`, `ip`, `ban_admin`, `ban_reason`, `ban_date`, `ban_expire`) VALUES ('%d', '%e', '%e', '%e', '%e', '%d', '%d')", 
        target_id_db, target_name, target_ip, admin_name, reason, time_now, time_expire);
    mysql_tquery(connection, query);

    format(string, sizeof(string), "������������� %s ������� ������ %s (� ��������) �� %d ��. �������: %s", admin_name, target_name, days, reason);
    SendClientMessageToAll(COLOR_PUNUSH, string);
    return 1;
}

forward OnUnbanCheck(adminid, const target_name[]);
public OnUnbanCheck(adminid, const target_name[])
{
    if(cache_num_rows() == 0)
    {
        return SendError(adminid, "����� � ����� ���-������ �� ������ � ������ ����������!");
    }

    new ban_expire;
    cache_get_value_name_int(0, "ban_expire", ban_expire);

    if(ban_expire > gettime())
    {
        new admin_name[MAX_PLAYER_NAME], string[144];
        GetPlayerName(adminid, admin_name, sizeof(admin_name));

        format(string, sizeof(string), "������������� %s �������� ������ %s", admin_name, target_name);
        SendClientMessageToAll(COLOR_PUNUSH, string); 

        mysql_format(connection, query, sizeof(query), 
            "DELETE FROM `banlist` WHERE `name` = '%e'", target_name);
        mysql_tquery(connection, query);
        
        return 1;
    }
    else
    {
        mysql_format(connection, query, sizeof(query), 
            "DELETE FROM `banlist` WHERE `name` = '%e'", target_name);
        mysql_tquery(connection, query);
        
        return SendError(adminid, "��� � ����� ������ ��� ����, ������ �������.");
    }
}

forward OnBanIPOffline(adminid, const ban_ip[], const reason[], time_now);
public OnBanIPOffline(adminid, const ban_ip[], const reason[], time_now)
{
    if(cache_num_rows() == 0) {
        new admin_name[MAX_PLAYER_NAME];
        GetPlayerName(adminid, admin_name, sizeof(admin_name));
        
        new string[256];
        format(string, sizeof(string), "������������� %s �������� ������� IP %s. �������: %s", admin_name, ban_ip, reason);
        SendClientMessageToAll(COLOR_PUNUSH, string);
        
        mysql_format(connection, query, sizeof(query), 
            "INSERT INTO `ip_bans` (`ip_address`, `account_id`, `player_name`, `nick_admin`, `date_ban`, `reason`) VALUES ('%e', '0', '����������', '%e', '%d', '%e')", 
            ban_ip, admin_name, time_now, reason);
        mysql_tquery(connection, query);
        
        return 1;
    }

    new target_id_db;
    cache_get_value_name_int(0, "id", target_id_db);
    
    new admin_name[MAX_PLAYER_NAME], string[256];
    GetPlayerName(adminid, admin_name, sizeof(admin_name));
    
    mysql_format(connection, query, sizeof(query), 
        "INSERT INTO `ip_bans` (`ip_address`, `account_id`, `player_name`, `nick_admin`, `date_ban`, `reason`) VALUES ('%e', '%d', '����������', '%e', '%d', '%e')", 
        ban_ip, target_id_db, admin_name, time_now, reason);
    mysql_tquery(connection, query);
    
    format(string, sizeof(string), "������������� %s �������� ������� IP %s. �������: %s", admin_name, ban_ip, reason);
    SendClientMessageToAll(COLOR_PUNUSH, string);
    return 1;
}

forward OnUnbanIPCheck(playerid, const ip[]);
public OnUnbanIPCheck(playerid, const ip[])
{
    if(cache_num_rows() == 0)
        return SendError(playerid, "IP �� ������ � ���� ���������� IP!");

    mysql_format(connection, query, sizeof(query), 
        "DELETE FROM `ip_bans` WHERE `ip_address` = '%e'", ip);
    mysql_tquery(connection, query);

    new admin_name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));

    new string[128];
    format(string, sizeof(string), "������������� %s �������� IP %s", admin_name, ip);
    SendClientMessageToAll(COLOR_PUNUSH, string);

    return 1;
}
