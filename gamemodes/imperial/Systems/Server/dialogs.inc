/*
 * IMPERIAL ROLE PLAY - Systems\Server\dialogs.inc
 * Обработчики диалогов настроек сервера.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// -------------------- ��������� ������� --------------------
DialogSettingsServer(playerid, response, listitem)
{
    if(!response) 
        return DialogAdminPanel(playerid, response, listitem);
    
    switch(listitem) {
        case 0: {
            if(AdminInfo[playerid][aRank] < 7) 
                return SendError(playerid, "� ��� ��� ����!");
            
            ShowPlayerDialog(playerid, DIALOG_SET_MULTIPLIER, DIALOG_STYLE_INPUT,
                "{FF0000}���������� ���������",
                "{FFFFFF}������� ��������� (1-3):",
                "����������", "������");
        }
        case 1: {
            if(AdminInfo[playerid][aRank] < 7) 
                return SendError(playerid, "� ��� ��� ����!");

            foreach(new i: Player) {
                if(IsPlayerConnected(i) && pInfo[i][pLogged]) {
                    SaveAdminData(i);
                    SavePlayerData(i);
                    SavePlayerInventory(i);
                    SaveServerSettings();
                }
            }

            SendClientMessage(playerid, COLOR_GREY, "�� ��������� ������ ����� �������!");
            
            DialogAdminPanel(playerid, 1, 3);
        }
        case 2: {
            if(AdminInfo[playerid][aRank] < 7) 
                return SendError(playerid, "� ��� ��� ����!");
            
            SendClientMessageToAll(0xFF0000FF, "[���������] ������ ����������� �� ���. ������� ����� 5 �����.");
            SendClientMessageToAll(0xFF0000FF, "������ ��� ������� ��������� � �����.");
            SetTimerEx("CloseServer", 300000, false, "i", playerid);
        }
        case 3: {
            if(AdminInfo[playerid][aRank] < 7) 
                return SendError(playerid, "� ��� ��� ����!");

            SendClientMessageToAll(0xFF0000FF, "������ ����� �������!");
            SendRconCommand("password 0");

            SettingServer[server_status] = 0;
            
            SaveServerSettings();
        }
        case 4: {
            if(AdminInfo[playerid][aRank] < 7) 
                return SendError(playerid, "� ��� ��� ����!");

            SendClientMessageToAll(0xFF0000FF, "������� �������, ������� ����� <3");

            SendRconCommand("gmx");
            
        }
        case 5: {
            if(AdminInfo[playerid][aRank] < 7) 
                return SendError(playerid, "� ��� ��� ����!");
            
            ShowPlayerDialog(playerid, DIALOG_INPUT_DELITE_ACOUNT, DIALOG_STYLE_INPUT, 
                "{FF0000}�������� �������� ������", 
                "{FFFFFF}������� ������� ������, ������� �������� ����� �������:",
                "�������", "������");
        }
        case 6: {
            if(AdminInfo[playerid][aRank] < 7) 
                return SendError(playerid, "� ��� ��� ����!");
            
            mysql_format(connection, query, sizeof(query), 
                "DELETE FROM `admins` WHERE `rank` < '7'");
            mysql_tquery(connection, query);
            
            foreach(new i : Player) {
                if(IsPlayerConnected(i) && AdminInfo[i][aRank] > 0 && AdminInfo[i][aRank] < 7) {
                    AdminInfo[i][aRank] = 0;
                    AdminInfo[i][aPassword][0] = EOS;
                    AdminInfo[i][aPrefix][0] = EOS;
                    AdminInfo[i][aAnsReport] = 0;
                    AdminInfo[i][aOnlineTime] = 0;
                    
                    for(new d = 0; d < 7; d++) {
                        AdminInfo[i][aWeekDays][d] = 0;
                    }
                    
                    AdminInfo[i][aLastResetWeek] = 0;
                    
                    SendClientMessage(i, COLOR_PUNUSH, "�� ���� ����� � ��������� ��������������!");
                }
            }
            SendClientMessage(playerid, COLOR_PUNUSH, "���� ������ ������������� (����� 7+ ������) �������������!");
        }

    }
    return 1;
}

DialogMultiplier(playerid, response, inputtext[])
{
    if(!response) 
        return 1;

    new multiplier_input = strval(inputtext); 
    
    if(multiplier_input < 1 || multiplier_input > 10)
        return SendError(playerid, "��������� �������� ������ ���� �� 1 �� 10!");
    
    SettingServer[multiplier] = multiplier_input;

    SaveServerSettings();

    
    new string[128];
    format(string, sizeof(string), "��������� �������� ���������� �� %dx", multiplier_input);
    SendClientMessage(playerid, COLOR_PUNUSH, string);
    
    return 1;
}

DialogDeliteAcount(playerid, response, inputtext[])
{
    if(!response) 
        return 1;

    new target_name[24];
    format(target_name, sizeof(target_name), "%s", inputtext);
    
    if(strlen(target_name) < 3 || strlen(target_name) > 24)
        return SendError(playerid, "������� ������ ���� �� 3 �� 24 ��������!");
    
    mysql_format(connection, query, sizeof(query), 
        "SELECT `id`, `name` FROM `users` WHERE `name` = '%e' LIMIT 1", target_name);
    mysql_tquery(connection, query, "OnCheckAccountDelete", "ds", playerid, target_name);
    
    return 1;
}
