/*
 * IMPERIAL ROLE PLAY - Systems\Auth\dialogs.inc
 * Обработчики диалогов регистрации и авторизации.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// =============================================================== //
// ================== ����������� �������� ====================== //
// =============================================================== //

// -------------------- ����������� / ����� --------------------
DialogRegister(playerid, response, inputtext[])
{
    if(!response) return Kick(playerid);
    if(strlen(inputtext) < 6) {
        ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD,
            "{FF0000}�����������",
            "{FF6666}������! ������ ������ ���� �� ����� 6 ��������!\n\n������� ������:",
            "������������������", "�����");
        return 1;
    }
    
    bcrypt_hash(inputtext, BCRYPT_COST, "OnPasswordHashed", "d", playerid);
    SendClientMessage(playerid, COLOR_SYSTEM, "������� | {FFFFFF}������ ���������...");
    return 1;
}

DialogLogin(playerid, response, inputtext[])
{
    if(!response) return Kick(playerid);
    if(strlen(inputtext) < 1) {
        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD,
            "{FF0000}�����������",
            "{FF6666}������! ������� ������!",
            "�����", "�����");
        return 1;
    }
    
    bcrypt_check(inputtext, pInfo[playerid][pPassword], "OnPasswordChecked", "d", playerid);
    SendClientMessage(playerid, COLOR_SYSTEM, "������� | {FFFFFF}�������� ������...");
    return 1;
}

DialogGender(playerid, response, listitem)
{
    if(!response) return Kick(playerid);
    pInfo[playerid][pGender] = listitem + 1;
    ShowSkinSelection(playerid);
    return 1;
}

DialogSkin(playerid, response, listitem)
{
    if(!response) return Kick(playerid);
    
    new skinid;
    if(pInfo[playerid][pGender] == 1) {
        skinid = (listitem >= 0 && listitem < sizeof(MaleSkins)) ? MaleSkins[listitem] : MaleSkins[0];
    } else {
        skinid = (listitem >= 0 && listitem < sizeof(FemaleSkins)) ? FemaleSkins[listitem] : FemaleSkins[0];
    }
    
    pInfo[playerid][pSkin] = skinid;
    
    mysql_format(connection, query, sizeof(query),
        "INSERT INTO `users` (`name`, `password`, `gender`, `skin`, `score`, `money`, `register`, `ip`, `last_ip`) VALUES ('%e', '%e', %d, %d, 1, 200, 1, '%e', %e)",
        pInfo[playerid][pName], pInfo[playerid][pPassword], pInfo[playerid][pGender], skinid, pInfo[playerid][pIP], pInfo[playerid][pLastIP]);
    mysql_tquery(connection, query, "OnPlayerRegistered", "d", playerid);
    
    SendClientMessage(playerid, COLOR_SYSTEM, "������� | {FFFFFF}�������� ��������...");
    return 1;
}
