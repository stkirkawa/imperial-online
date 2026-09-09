/*
 * IMPERIAL ROLE PLAY - Systems\Reports\dialogs.inc
 * Обработчики диалогов репортов игроков.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// -------------------- ������� ������� --------------------
DialogReportInput(playerid, response, inputtext[])
{
    if(!response) return 1;
    
    new targetid;
    new rep_text[128];
    
    if(sscanf(inputtext, "is[128]", targetid, rep_text)) {
        SendError(playerid, "������������ ������! �����������: ID �����");
        ShowReportDialog(playerid);
        return 1;
    } if(!IsPlayerConnected(targetid) || !pInfo[targetid][pLogged]) {
        SendError(playerid, "����� � ����� ID �� ������!");
        ShowReportDialog(playerid);
        return 1;
    } if(targetid == playerid) {
        SendError(playerid, "�� �� ������ ������������ �� ����!");
        ShowReportDialog(playerid);
        return 1;
    } if(strlen(rep_text) < 3) {
        SendError(playerid, "����� ������ ������� ��������! ������� 3 �������.");
        ShowReportDialog(playerid);
        return 1;
    }
    
    if(GetPVarInt(playerid, "report_id"))
        return SendError(playerid, "� ��� ��� ���� �������� ������. ��������� � ������������!");

    new bool:admin_online = false;
    foreach(new i : Player) {
        if(AdminInfo[i][aLogged]) { admin_online = true; break; }
    }
    if(!admin_online) return SendError(playerid, "������������� �� � ����!");
    
    new string[256], hour, minute, second;

    for(new i = 0; i < MAX_ADMIN_REPORTS; i++) {
        if(ReportInfo[i][report_player_id] != -1) continue;
        
        ReportInfo[i][report_is_tooked] = false;
        ReportInfo[i][rep_id] = i;
        ReportInfo[i][report_player_id] = playerid;
        ReportInfo[i][report_target_id] = targetid;
        format(ReportInfo[i][report_text], MAX_REPORT_TEXT_LENGTH, "%s", rep_text);
        
        ReportInfo[i][report_when_wroted_time] = gettime();
        gettime(hour, minute, second);
        format(string, sizeof(string), "%02d:%02d:%02d", hour, minute, second);
        strmid(ReportInfo[i][report_when_wroted_text], string, 0, 10, 10);

        total_admin_reports++;
        SetPVarInt(playerid, "report_id", i);
        break;
    }

    format(string, sizeof(string), "[������] ����� {FFFFFF}%s[%d] {FF6347}����������� �� {FFFFFF}%s[%d]", 
        PlayerName(playerid), playerid, PlayerName(targetid), targetid);
    SendAdminMessage(0xFF6347FF, string);
    
    format(string, sizeof(string), "{FFFFFF}�����: %s", rep_text);
    SendAdminMessage(0xFF6347FF, string);
    
    format(string, sizeof(string), "{FF6347}����� �����: {FFFFFF}%d", total_admin_reports);
    SendAdminMessage(0xFF6347FF, string);
    SendAdminMessage(0xFF6347FF, "����� ����� ������ ����������� {FFFFFF}/arep");

    format(string, sizeof(string),"[������] {FFFFFF}�� ������� ��������� ������ �� ������ {FF6347}%s[%d]", 
        PlayerName(targetid), targetid);
    SendClientMessage(playerid, 0xFF6347FF, string);

    format(string, sizeof(string), "[������] {FFFFFF}����� ����� ������: %s", rep_text);
    SendClientMessage(playerid, 0xFF6347FF, string);

    return 1;
}

DialogAreportLists(playerid, response, listitem)
{
    if(!response) return 1;
    if(listitem < 0 || listitem >= MAX_ADMIN_REPORTS) return 1;
    if(ReportInfo[listitem][report_player_id] == -1) return 1;
    
    if(ReportInfo[listitem][report_is_tooked] == false) {
        ShowPlayer_Admin_Report_Answer(playerid, listitem);
    } else {
        SendError(playerid, "���-�� ��� ������������� ��� ������, ��������� � ���������!");
    }
    return 1;
}

DialogAnswerReport(playerid, response, inputtext[])
{
    if(!response) {
        new report_id = GetPVarInt(playerid, "report_id_to_answer");
        if(report_id != -1) ReportInfo[report_id][report_is_tooked] = false;
        DeletePVar(playerid, "report_id_to_answer");
        ShowPlayer_Admin_Report_List(playerid);
    } else {
        new report_id = GetPVarInt(playerid, "report_id_to_answer"),
            target_id = ReportInfo[report_id][report_player_id];

        if(!strlen(inputtext)) {
            ShowPlayer_Admin_Report_Answer(playerid, report_id);
            return 1;
        }

        if(!IsPlayerConnected(target_id) || target_id == INVALID_PLAYER_ID) {
            SendError(playerid, "����� ������� ������ ��� �� ������!");
            ResetReport(report_id);
            DeletePVar(playerid, "report_id_to_answer");
            ShowPlayer_Admin_Report_List(playerid);
            return 1;
        } 
        if(strcmp(inputtext, "��", true) == 0) {
            if(AdminInfo[playerid][aRank] < 1) {
                SendError(playerid, "� ��� ��� ���� ��� ������!");
                ShowPlayer_Admin_Report_Answer(playerid, report_id);
                return 1;
            }
            StartSpectate(playerid, target_id);
            new string[128];
            format(string, sizeof(string), "[������] %s[%d]: {FFFFFF}��������� �����, ������� �������� ��� ����� �������!", PlayerName(playerid), playerid);
            SendClientMessage(target_id, COLOR_ANSWER, string);
            SendAdminMessage(COLOR_ANSWER, string);
            ResetReport(report_id);
            DeletePVar(playerid, "report_id_to_answer");
            total_admin_reports--;
            return 1;
        } else if(strcmp(inputtext, "��", true) == 0) {
            if(AdminInfo[playerid][aRank] < 1) {
                SendError(playerid, "� ��� ��� ���� ��� ������!");
                ShowPlayer_Admin_Report_Answer(playerid, report_id);
                return 1;
            }
            if(GetPVarInt(playerid, "Spectating") == 1) {
                SendError(playerid, "��� ������ ������� �� ������");
                ShowPlayer_Admin_Report_Answer(playerid, report_id);
                return 1;
            }
            TeleportToPlayer(playerid, target_id);
            ResetReport(report_id);
            DeletePVar(playerid, "report_id_to_answer");
            total_admin_reports--;
            return 1;
        }
        
        new string[256];
        format(string, sizeof(string), "[������] ������������� {FFFFFF}%s[%d] {FF6347}������� �� ������ {FFFFFF}%s[%d]: {FFFFFF}%s", 
            PlayerName(playerid), playerid, PlayerName(target_id), target_id, inputtext);
        SendAdminMessage(0xFF6347FF, string);

        format(string, sizeof(string), "[������] ������������� {FFFFFF}%s[%d] {FF6347}������� ���: {FFFFFF}%s", 
            PlayerName(playerid), playerid, inputtext);
        SendClientMessage(target_id, 0xFF6347FF, string);

        AdminInfo[playerid][aAnsReport]++;
        ResetReport(report_id);
        DeletePVar(target_id, "report_id");
        total_admin_reports--;
        DeletePVar(playerid, "report_id_to_answer");
        ShowPlayer_Admin_Report_List(playerid);
    }
    return 1;
}
