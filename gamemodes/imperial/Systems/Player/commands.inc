/*
 * IMPERIAL ROLE PLAY - Systems\Player\commands.inc
 * Команды, доступные игрокам.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// =============================================================== //
// ====================== ������� ������� ======================= //
// =============================================================== //
CMD:rep(playerid, params[])
{   
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");

    ShowReportDialog(playerid);
    return 1;
}

CMD:inv(playerid)
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    
    ShowPlayerInventory(playerid);
    return 1;
}

// =============================================================== //
