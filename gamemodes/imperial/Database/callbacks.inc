/*
 * IMPERIAL ROLE PLAY - Database\callbacks.inc
 * Callback-функции результатов MySQL для аккаунтов игроков.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// =============================================================== //
// ======================== SQL ������� ========================= //
// =============================================================== //

forward OnPlayerCheck(playerid);
public OnPlayerCheck(playerid)
{
    if(cache_num_rows() == 0)
    {
        new check_ban_id = 0;
        cache_get_value_name_int(0, "ban_id", check_ban_id);
        
        if(check_ban_id > 0)
        {
            cache_get_value_name_int(0, "ban_expire", pInfo[playerid][pBanExpire]);
            if(pInfo[playerid][pBanExpire] > gettime()) 
            {
                return ShowBanDialog(playerid);
            }
        }

        new str[450];
        format(str, sizeof(str),
            "{FFFFFF}������� �� ������!\n\n{808080}���: {FFFF00}%s\n{808080}IP: {FFFF00}%s\n\n{FFFFFF}������� ������:",
            pInfo[playerid][pName], pInfo[playerid][pIP]);
        ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD,
            "{FF0000}�����������", str, "������������������", "�����");
    }
    else
    {
        cache_get_value_name_int(0, "ban_id", pInfo[playerid][pBanID]);

        if(pInfo[playerid][pBanID] > 0) 
        {
            cache_get_value_name_int(0, "ban_expire", pInfo[playerid][pBanExpire]);
            
            if(pInfo[playerid][pBanExpire] > gettime())
            {
                return ShowBanDialog(playerid);
            }
            else 
            {
                new delete_query[128];
                mysql_format(connection, delete_query, sizeof(delete_query), "DELETE FROM `banlist` WHERE `ban_id` = '%d'", pInfo[playerid][pBanID]);
                mysql_tquery(connection, delete_query);
                pInfo[playerid][pBanID] = 0;
            }
        }
        cache_get_value_name(0, "password", pInfo[playerid][pPassword], 255);
        cache_get_value_name_int(0, "id", pInfo[playerid][pID]);
        cache_get_value_name_int(0, "gender", pInfo[playerid][pGender]);
        cache_get_value_name_int(0, "skin", pInfo[playerid][pSkin]);
        cache_get_value_name_int(0, "score", pInfo[playerid][pScore]);
        cache_get_value_name_int(0, "exp", pInfo[playerid][pExp]);
        cache_get_value_name_int(0, "money", pInfo[playerid][pMoney]);
        cache_get_value_name_int(0, "register", pInfo[playerid][pRegister]);
        cache_get_value_name(0, "ip", pInfo[playerid][pIP], 16);
        cache_get_value_name(0, "last_ip", pInfo[playerid][pLastIP], 16);
        cache_get_value_name_int(0, "mute", pInfo[playerid][pMute]);

        new string[256];
        format(string, sizeof(string), "{FFFFFF}������������, {FFFF00}%s{FFFFFF}!\n\n������� ������:", pInfo[playerid][pName]);
        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "{FF0000}�����������", string, "�����", "�����");
    }
    return 1;
}


forward OnPlayerRegistered(playerid);
public OnPlayerRegistered(playerid)
{
    pInfo[playerid][pID] = cache_insert_id();
    pInfo[playerid][pRegister] = 1;
    pInfo[playerid][pLogged] = true;
    pInfo[playerid][pScore] = 1;
    pInfo[playerid][pMoney] = 200;
    
    SetPlayerSkin(playerid, pInfo[playerid][pSkin]);
    SetPlayerScore(playerid, 1);
    GivePlayerMoney(playerid, 200);
    ClearPlayerInventory(playerid);
    
    new gender_text[10];
    if(pInfo[playerid][pGender] == 1) format(gender_text, sizeof(gender_text), "�������");
    else format(gender_text, sizeof(gender_text), "�������");
    
    new string[256];
    format(string, sizeof(string),
        "����� ����������!\n\n{FFFF00}����������:\n{FFFFFF}� ���: {FFFF00}%s\n� ���: {FFFF00}%s\n� ����: {FFFF00}%d\n� �����: {FFFF00}200$",
        pInfo[playerid][pName], gender_text, pInfo[playerid][pSkin]);
    
    ShowPlayerDialog(playerid, DIALOG_STYLE_MSGBOX, DIALOG_NONE,
        "{FF0000}�������� �����������", string, "��", "");
    SendClientMessage(playerid, COLOR_SYSTEM, "������� | {FFFFFF}������� ������!");
    
    AFKTime[playerid] = 0;
    IsAFK[playerid] = 0;
    SpawnPlayer(playerid);
    SetCameraBehindPlayer(playerid);
    return 1;
}

forward OnPasswordHashed(playerid);
public OnPasswordHashed(playerid)
{
    new hash[255];
    bcrypt_get_hash(hash);
    format(pInfo[playerid][pPassword], 255, hash);
    ShowGenderSelection(playerid);
}

forward OnPasswordChecked(playerid);
public OnPasswordChecked(playerid)
{
    if(!bcrypt_is_equal()) {
        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD,
            "{FF0000}�����������", "{FF6666}�������� ������!", "�����", "�����");
        return 1;
    }
    
    pInfo[playerid][pLogged] = true;
    pInfo[playerid][pRegister] = 1;
    
    SetPlayerSkin(playerid, pInfo[playerid][pSkin]);
    SetPlayerScore(playerid, pInfo[playerid][pScore]);
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, pInfo[playerid][pMoney]);
    LoadPlayerInventory(playerid);

    new current_ip[16];
    GetPlayerIp(playerid, current_ip, sizeof(current_ip));
    
    if(strcmp(pInfo[playerid][pLastIP], current_ip, true) != 0) {
        format(pInfo[playerid][pLastIP], 16, current_ip);
        
        mysql_format(connection, query, sizeof(query), 
            "UPDATE `users` SET `last_ip` = '%e' WHERE `id` = '%d'", 
            pInfo[playerid][pLastIP], pInfo[playerid][pID]);
        mysql_tquery(connection, query);
        
        SendClientMessage(playerid, COLOR_SYSTEM, "������� | {FFFFFF}��� IP ���������! ��������� �����.");
    }

    mysql_format(connection, query, sizeof(query),
        "SELECT * FROM `admins` WHERE `player_id` = '%d'", pInfo[playerid][pID]);
    mysql_tquery(connection, query, "OnAdminCheck", "d", playerid);

    new string[128];
    format(string, sizeof(string), "������� | {FFFFFF}����� ����������, {FFFF00}%s{FFFFFF}!", pInfo[playerid][pName]);
    SendClientMessage(playerid, COLOR_SYSTEM, string);

    AFKTime[playerid] = 0;
    IsAFK[playerid] = 0;
    SpawnPlayer(playerid);
    return 1;
}
