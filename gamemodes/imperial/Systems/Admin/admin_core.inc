/*
 * IMPERIAL ROLE PLAY - Systems\Admin\admin_core.inc
 * Авторизация администраторов, панель и диалоги справки по командам.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// -------------------- ����� ������ --------------------
DialogAdminLogin(playerid, response, inputtext[])
{
    if(!response) return 1;
    if(strlen(inputtext) < 1 || !IsValidAdminPassword(inputtext)) {
        ShowPlayerDialog(playerid, DIALOG_ADMIN_LOGIN, DIALOG_STYLE_PASSWORD,
            "{FF0000}���� � �����-������",
            "{FF6666}������! ������� ���������� ������!",
            "�����", "������");
        return 1;
    }
    
    bcrypt_check(inputtext, AdminInfo[playerid][aPassword], "OnAdminPasswordChecked", "d", playerid);
    return 1;
}

DialogAdminRegister(playerid, response, inputtext[])
{
    if(!response) return 1;
    if(strlen(inputtext) < 4 || !IsValidAdminPassword(inputtext)) {
        ShowPlayerDialog(playerid, DIALOG_ADMIN_REGISTER, DIALOG_STYLE_PASSWORD,
            "{FF0000}����������� ��������������",
            "{FF6666}������! ������ ������ ���� �� ����� 4 ��������!",
            "������������������", "������");
        return 1;
    }

    bcrypt_hash(inputtext, BCRYPT_COST, "OnAdminPasswordHashed", "d", playerid);
    return 1;
}

DialogAdminPanel(playerid, response, listitem) 
{
    if(!response) return 1;
    
    switch(listitem) 
    {
        case 0: {
            SendClientMessage(playerid, -1, "�����");
        }
        case 1: {
            ShowPlayerDialog(playerid, DIALOG_ADMIN_COMMANDS, DIALOG_STYLE_LIST, 
                "{FF0000}������� ��������������", 
                "{FFFFFF}1. ���������\n\
                2. ��. �������������\n\
                3. �������������\n\
                4. ��. �������������\n\
                5. ��. �������������\n\
                6. ����������\n\
                7. �����������",
                "�������", "�����"
            );
        } case 2: { 
            new dialog_str[2048];
            
            new total_week_seconds = 0;
            for(new d = 0; d < 7; d++) {
                total_week_seconds += AdminInfo[playerid][aWeekDays][d];
            }
            
            format(dialog_str, sizeof(dialog_str),
                "{FFFFFF}�������� �����: {FFD700}%d\n\n\
                {FFFFFF}���������� �� ������� �� ���� ������:\n\
                {FFFFFF}�����������: {FFD700}%s\n\
                {FFFFFF}�������: {FFD700}%s\n\
                {FFFFFF}�����: {FFD700}%s\n\
                {FFFFFF}�������: {FFD700}%s\n\
                {FFFFFF}�������: {FFD700}%s\n\
                {FFFFFF}�������: {FFD700}%s\n\
                {FFFFFF}�����������: {FFD700}%s\n\n\
                {FFFFFF}����� ����� �� ������: {FFD700}%s",
                AdminInfo[playerid][aAnsReport],
                FormatTime(AdminInfo[playerid][aWeekDays][0]),
                FormatTime(AdminInfo[playerid][aWeekDays][1]),
                FormatTime(AdminInfo[playerid][aWeekDays][2]),
                FormatTime(AdminInfo[playerid][aWeekDays][3]),
                FormatTime(AdminInfo[playerid][aWeekDays][4]),
                FormatTime(AdminInfo[playerid][aWeekDays][5]),
                FormatTime(AdminInfo[playerid][aWeekDays][6]),
                FormatTime(total_week_seconds)
            );
            
            ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_MSGBOX,
                "{FF0000}���������� ��������������", dialog_str, "��������", "�������");
        } case 3: {
            ShowPlayerDialog(playerid, DIALOG_SETTING_SERVER, DIALOG_STYLE_LIST, "{FF0000}��������� �������",
                "{FFFFFF}����������� ��������� ��������\n\
                ������� ���������� ����� �������\n\
                ������� ������ �� ����������� �������\n\
                �������� ������\n\
                ������� ������� �������\n\
                ������� ������� ������\n\
                �������������� ���� ������ �������������",
                "�������", "������"
            );
        }
    }
    return 1;
}

DialogAdminCommands(playerid, response, listitem)
{
    if(!response) return DialogAdminPanel(playerid, response, listitem);
    
    new string[1024], title[64];
    
    new level_names[8][64] = {
        "������� �����",
        "���������",
        "��. �������������",
        "�������������",
        "��. �������������",
        "��. �������������",
        "����������",
        "�����������"
    };
    new level = AdminInfo[playerid][aRank];
    format(title, sizeof(title), "{FF0000}������� %s ������", level_names[listitem + 1]);
    
    string[0] = EOS;
    
    if(listitem == 0) {
        if(level >= 1) {
            strcat(string, "{FFFF00}/a [���������] - {FFFFFF}��� �������������\n", sizeof(string));
            strcat(string, "{FFFF00}/tpc [X] [Y] [Z] - {FFFFFF}�������� �� �����������\n", sizeof(string));
            strcat(string, "{FFFF00}/kick [id] [�������] - {FFFFFF}��������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/sp [id] {FFFFFF}- ����� � ������\n", sizeof(string));
            strcat(string, "{FFFF00}/agm {FFFFFF}- ��������/��������� ��\n", sizeof(string));
            strcat(string, "{FFFF00}/spoff {FFFFFF}- ����� �� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/admins {FFFFFF}- ������ ������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/spawn [id] {FFFFFF}- ���������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/arep {FFFFFF}- ������ �����\n", sizeof(string));
            strcat(string, "{FFFF00}/slap [id] [���: 1 - �����/����]{FFFFFF}- ���������� ������\n", sizeof(string));
        } else {
            SendError(playerid, "������ ������� ������ ��� �� ��������!");
            return DialogAdminCommands(playerid, response, listitem);
        }
    } else if(listitem == 1) {
        if(level >= 2) {
            strcat(string, "{FFFF00}/fixcar [id] - {FFFFFF}�������� ��������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/flipcar [id] - {FFFFFF}����������� ���������\n", sizeof(string));
            strcat(string, "{FFFF00}/goto [id] - {FFFFFF}���������������� � ������\n", sizeof(string));
            strcat(string, "{FFFF00}/shp [id] [1-260] - {FFFFFF}������ �������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/jetpack - {FFFFFF}������ ���� �������\n", sizeof(string));
            strcat(string, "{FFFF00}/mute [id] [�����: 1-120] [�������] - {FFFFFF}������ ���������� ���� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/ans [id] [���������] - {FFFFFF}�������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/aveh [id ����] [���� 1] [���� 2] - {FFFFFF}������� ����� ���������\n", sizeof(string));
            strcat(string, "{FFFF00}/delaveh - {FFFFFF}������� ���� �����. ���������\n", sizeof(string));
            strcat(string, "{FFFF00}/setint [id ���������] - {FFFFFF}����������������� � ��������\n", sizeof(string));
            strcat(string, "{FFFF00}/setworld [id ������������ ����]- {FFFFFF}����������������� � ���������� ���\n", sizeof(string));
        } else {
            SendError(playerid, "������ ������� ������ ��� �� ��������!");
            return DialogAdminCommands(playerid, response, listitem);
        }
    } else if(listitem == 2) {
        if(level >= 3) {
            strcat(string, "{FFFF00}/getip [id] - {FFFFFF}�������� ip � ������\n", sizeof(string));
            strcat(string, "{FFFF00}/gethere [id] - {FFFFFF}��������������� ������ � ����\n", sizeof(string));
            strcat(string, "{FFFF00}/unmute [id] - {FFFFFF}����� ���������� ���� ������\n", sizeof(string));   
            strcat(string, "{FFFF00}/offmute [Nick_Name] [�����: 1-120] [�������] - {FFFFFF}������ ���������� ���� � ��������\n", sizeof(string));
            strcat(string, "{FFFF00}/unoffmute - {FFFFFF}����� ���������� ���� ������ � ��������\n", sizeof(string)); 
            strcat(string, "{FFFF00}/rcars [������: 1-350] - {FFFFFF}������� ��������� � �������\n", sizeof(string));
            strcat(string, "{FFFF00}/msg [���������] - {FFFFFF}�������� � ����� ���\n", sizeof(string));
            strcat(string, "{FFFF00}/cc - {FFFFFF}�������� ���\n", sizeof(string));
            strcat(string, "{FFFF00}/tempskin - {FFFFFF}������ ��������� ����\n", sizeof(string));
            strcat(string, "{FFFF00}/freeze - {FFFFFF}���������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/unfreeze - {FFFFFF}����������� ������\n", sizeof(string));
        } else {
            SendError(playerid, "������ ������� ������ ��� �� ��������!");
            return DialogAdminCommands(playerid, response, listitem);
        }
    } else if(listitem == 3) {
        if(level >= 4) {
            strcat(string, "{FFFF00}/achangename [Nick_Name] - {FFFFFF}���������� ���� ��������� ���-����\n", sizeof(string));
            strcat(string, "{FFFF00}/veh [id ������] [id ����] [���� 1] [���� 2] - {FFFFFF}������� ����� ��������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/delv [id ������] - {FFFFFF}������� ��������� ��������� � ������\n", sizeof(string));
            strcat(string, "{FFFF00}/setweather [id ������] - {FFFFFF}���������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/event - {FFFFFF}���� �����������\n", sizeof(string));
        } else {
            SendError(playerid, "������ ������� ������ ��� �� ��������!");
            return DialogAdminCommands(playerid, response, listitem);
        }
    } else if(listitem == 4) {
        if(level >= 5) {
            strcat(string, "{FFFF00}/ban [Nick_Name] [���] [�������] - {FFFFFF}�������� ������ (������/�������)\n", sizeof(string));
            strcat(string, "{FFFF00}/unban [Nick_Name] - {FFFFFF}��������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/astats [id ��������������] - {FFFFFF}���������� ���������� ��������������\n", sizeof(string));
            strcat(string, "{FFFF00}/banip [ip] [�������] - {FFFFFF}������������� IP\n", sizeof(string));
            strcat(string, "{FFFF00}/unbanip [ip] - {FFFFFF}�������������� IP\n", sizeof(string));
        } else {
            SendError(playerid, "������ ������� ������ ��� �� ��������!");
            return DialogAdminCommands(playerid, response, listitem);
        }
    } else if(listitem == 5) {
        if(level >= 6) {
            strcat(string, "{FFFF00}/setadmin [id] [1-5] - {FFFFFF}������ �������\n", sizeof(string));
        } else {
            SendError(playerid, "������ ������� ������ ��� �� ��������!");
            return DialogAdminCommands(playerid, response, listitem);
        }
    } else if(listitem == 6) {
        if(level >= 7) {
            strcat(string, "{FFFF00}/giveitem [id] [ID ��������] [����������] - {FFFFFF}������ ������� ������\n", sizeof(string));
            strcat(string, "{FFFF00}/setprefix [id] [�������] - {FFFFFF}������ ������� ��������������\n", sizeof(string));
            strcat(string, "{FFFF00}/givemoney [id] [���-��] - {FFFFFF}������ �����\n", sizeof(string));
        } else {
            SendError(playerid, "������ ������� ������ ��� �� ��������!");
            return DialogAdminCommands(playerid, response, listitem);
        }
    } else {
        strcat(string, "{808080}������� ��� ����� ������ � ����������...\n", sizeof(string));
        strcat(string, "{FFFFFF}����� �������� ����� �������!", sizeof(string));
    }
    
    ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_MSGBOX, 
        title, string, "�����", "");
    
    return 1;
}
