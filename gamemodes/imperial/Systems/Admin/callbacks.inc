/*
 * IMPERIAL ROLE PLAY - Systems\Admin\callbacks.inc
 * Callback-функции администраторов, репортов и модерации.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// =============================================================== //
// ====================== ������� ������� ====================== //
// =============================================================== //
forward OnAdminCheck(playerid);
public OnAdminCheck(playerid)
{
    if(cache_num_rows() == 0) {
        AdminInfo[playerid][aRank] = 0;
        AdminInfo[playerid][aLogged] = false;
        AdminInfo[playerid][aPassword][0] = EOS;
        AdminInfo[playerid][aPrefix][0] = EOS;
        AdminInfo[playerid][aOnlineTime] = 0;
        AdminInfo[playerid][aAnsReport] = 0;
        AdminInfo[playerid][aID] = 0;
        AdminInfo[playerid][aLastResetWeek] = 0;
        
        for(new i = 0; i < 7; i++) {
            AdminInfo[playerid][aWeekDays][i] = 0;
        }
        
        mysql_format(connection, query, sizeof(query),
            "INSERT INTO `admins` (`player_id`, `rank`, `password`, `prefix`, `last_reset_week`) VALUES (%d, 0, '', '', 0)",
            pInfo[playerid][pID]);
        mysql_tquery(connection, query, "OnAdminCreated", "d", playerid);
    } else {
        cache_get_value_int(0, "id", AdminInfo[playerid][aID]);
        cache_get_value_int(0, "rank", AdminInfo[playerid][aRank]);
        cache_get_value(0, "password", AdminInfo[playerid][aPassword], 255);
        cache_get_value(0, "prefix", AdminInfo[playerid][aPrefix], 32);
        cache_get_value_int(0, "ans_report", AdminInfo[playerid][aAnsReport]);
        cache_get_value_int(0, "online_time", AdminInfo[playerid][aOnlineTime]);
        cache_get_value_int(0, "mon", AdminInfo[playerid][aWeekDays][0]);
        cache_get_value_int(0, "tue", AdminInfo[playerid][aWeekDays][1]);
        cache_get_value_int(0, "wed", AdminInfo[playerid][aWeekDays][2]);
        cache_get_value_int(0, "thu", AdminInfo[playerid][aWeekDays][3]);
        cache_get_value_int(0, "fri", AdminInfo[playerid][aWeekDays][4]);
        cache_get_value_int(0, "sat", AdminInfo[playerid][aWeekDays][5]);
        cache_get_value_int(0, "sun", AdminInfo[playerid][aWeekDays][6]);
        cache_get_value_int(0, "last_reset_week", AdminInfo[playerid][aLastResetWeek]);
        
        CheckAndResetWeeklyStats(playerid);
        AdminInfo[playerid][aLogged] = false;
        
        if(AdminInfo[playerid][aRank] > 0) {
            new string[128];
            if(strlen(AdminInfo[playerid][aPassword]) == 0) {
                format(string, sizeof(string), "������� | {FFFFFF}� ��� ���� �����-����� {FFD700}%d ������{FFFFFF}, �� ��� ������!",
                    AdminInfo[playerid][aRank]);
                SendClientMessage(playerid, COLOR_ADMIN, string);
                SendAdminNotify(playerid, "����������� {FFD700}/alogin{FFFFFF} ��� ��������� ������");
            } else {
                format(string, sizeof(string), "������� | {FFFFFF}� ��� ���� �����-����� {FFD700}%d ������",
                    AdminInfo[playerid][aRank]);
                SendClientMessage(playerid, COLOR_ADMIN, string);
                SendAdminNotify(playerid, "����������� {FFD700}/alogin{FFFFFF} ��� �����");
            }
        }
    }
    return 1;
}

forward OnAdminCreated(playerid);
public OnAdminCreated(playerid)
{
    AdminInfo[playerid][aID] = cache_insert_id();
    return 1;
}

forward OnAdminLoginCheck(playerid);
public OnAdminLoginCheck(playerid)
{
    if(cache_num_rows() == 0) {
        ShowPlayerDialog(playerid, DIALOG_ADMIN_REGISTER, DIALOG_STYLE_PASSWORD,
            "{FF0000}����������� ��������������",
            "{FFFFFF}���������� ������ ��� ����� � �����-������:",
            "������������������", "������");
    } else {
        cache_get_value_int(0, "rank", AdminInfo[playerid][aRank]);
        cache_get_value(0, "password", AdminInfo[playerid][aPassword], 255);
        cache_get_value(0, "prefix", AdminInfo[playerid][aPrefix], 32);
        
        if(strlen(AdminInfo[playerid][aPassword]) == 0) {
            new str[254];
            format(str, sizeof(str), "{FFFFFF}� ��� ���� �����-����� {FFD700}%d ������{FFFFFF}, �� ��� ������!\n���������� ������:",
                AdminInfo[playerid][aRank]);
            ShowPlayerDialog(playerid, DIALOG_ADMIN_REGISTER, DIALOG_STYLE_PASSWORD,
                "{FF0000}����������� ��������������", str,
                "������������������", "������");
        } else {
            new str[256];
            if(strlen(AdminInfo[playerid][aPrefix]) > 0) {
                format(str, sizeof(str), "{FFFFFF}������� ������:\n{33CC33}��� �������: %s",
                    AdminInfo[playerid][aPrefix]);
            } else {
                format(str, sizeof(str), "{FFFFFF}������� ������ ��������������:");
            }
            ShowPlayerDialog(playerid, DIALOG_ADMIN_LOGIN, DIALOG_STYLE_PASSWORD,
                "{FF0000}���� � �����-������", str, "�����", "������");
        }
    }
    return 1;
}

forward OnAdminPasswordHashed(playerid);
public OnAdminPasswordHashed(playerid)
{
    new hash[255];
    bcrypt_get_hash(hash);
    
    new rank = 1;
    cache_get_value_int(0, "rank", rank);
    if(rank == 0) rank = 1;
    
    new std_prefix[32];
    format(std_prefix, sizeof(std_prefix), AdminStandartPrefix[rank]);
    
    mysql_format(connection, query, sizeof(query),
        "UPDATE `admins` SET `password` = '%e', `prefix` = '%e' WHERE `player_id` = %d",
        hash, std_prefix, pInfo[playerid][pID]);
    mysql_tquery(connection, query);
    
    AdminInfo[playerid][aRank] = rank;
    AdminInfo[playerid][aLogged] = true;
    format(AdminInfo[playerid][aPassword], 255, hash);
    format(AdminInfo[playerid][aPrefix], 32, std_prefix);
    SetPlayerColor(playerid, COLOR_ADMIN);
    
    new string[128];
    format(string, sizeof(string), "������� | {FFFFFF}������ ����������! ��� �������: {FFD700}%s", std_prefix);
    SendClientMessage(playerid, COLOR_ADMIN, string);
}

forward OnAdminPasswordChecked(playerid);
public OnAdminPasswordChecked(playerid)
{
    if(!bcrypt_is_equal()) {
        ShowPlayerDialog(playerid, DIALOG_ADMIN_LOGIN, DIALOG_STYLE_PASSWORD,
            "{FF0000}���� � �����-������",
            "{FF6666}�������� ������!\n\n���������� �����:",
            "�����", "������");
        return 1;
    }
    
    AdminInfo[playerid][aLogged] = true;

    new string[128];
    format(string, sizeof(string), "������� | {FFFFFF}�� ����� ��� {FFD700}%s",
        AdminStandartPrefix[AdminInfo[playerid][aRank]]);
    SendClientMessage(playerid, COLOR_ADMIN, string);
    SetPlayerColor(playerid, COLOR_ADMIN);
    return 1;
}

forward OnAdminRegister(playerid);
public OnAdminRegister(playerid)
{
    AdminInfo[playerid][aID] = cache_insert_id();
    SetPlayerColor(playerid, COLOR_ADMIN);
    SendAdminNotify(playerid, "�� ���������������� � �����-������.");
    return 1;
}

// -------------------- ������� ������� --------------------
forward AutoClearningReports();
public AutoClearningReports()
{
    new total_cleaned_reports = 0;

    for(new i = 0; i < MAX_ADMIN_REPORTS; i++) {
        if(ReportInfo[i][report_player_id] == -1) continue;

        if(gettime() - ReportInfo[i][report_when_wroted_time] >= 600) {
            if(IsPlayerConnected(ReportInfo[i][report_player_id])) {
                SendInfo(ReportInfo[i][report_player_id], "���� ������ ���� ������������� ������� �� ��������� �������!");
                DeletePVar(ReportInfo[i][report_player_id], "report_id");
            }
            ResetReport(i);
            total_cleaned_reports++;
        }
    }
    return 1;
}

// =============================================================== //
