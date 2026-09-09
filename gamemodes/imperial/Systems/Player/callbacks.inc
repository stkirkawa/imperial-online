/*
 * IMPERIAL ROLE PLAY - Systems\Player\callbacks.inc
 * Callback-функции игроков и транспорта.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// =============================================================== //
// ======================== ������� ============================= //
// =============================================================== //
hook OnPlayerText(playerid, text[]) {
    if(!pInfo[playerid][pLogged] || pInfo[playerid][pRegister] != 1) {
        return SendError(playerid, "�� �� ������������!");
    }
    
    if(pInfo[playerid][pMute] > 0) {
        new string[144];
        format(string, sizeof(string), 
            "������ | {FFFFFF}� ��� ���������� ����. ����������: %s", 
            ConvertValueMinSecunds(pInfo[playerid][pMute]));
        SendError(playerid, string);
        return 0;
    }
    
    if(strlen(text) < 1) return 1;
    
    SetPlayerChatBubble(playerid, text, 0xFFFFFFFF, 8.1, 4500);
    ApplyAnimation(playerid, "GANGS", "prtial_gngtlkA", 4.1, 0, 1, 1, 0, 0, 1);
    
    new string[144];
    format(string, sizeof(string), "%s[%d]: %s", PlayerName(playerid), playerid, text);
    ProxDetector(playerid, 20.0, 0xFFFFFFFF, string);
    
    return 0;
}

hook OnPlayerUpdate(playerid)
{   
    if(!pInfo[playerid][pLogged]) return 1; 

    LastTick[playerid] = GetTickCount();
    if(IsAFK[playerid]) {
        new str[120];
        format(str, sizeof(str), "AFK | {FFFFFF}�� ������� � AFK: {ffff00}%d {FFFFFF}������", AFKTime[playerid]);
        SendClientMessage(playerid, 0xFFFF00FF, str);
        IsAFK[playerid] = 0;
        AFKTime[playerid] = 0;
    }
    return 1;
}

// -------------------- OnPlayerKeyStateChange --------------------
hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    return 1;
}

// =============================================================== //
// ====================== ������ ������� ======================== //
// =============================================================== //
hook OnPlayerDeath(playerid, killerid, reason) { return 1; }
hook OnVehicleSpawn(vehicleid) { return 1; }
hook OnVehicleDeath(vehicleid, killerid) { return 1; }
hook OnPlayerCommandText(playerid, cmdtext[]) { return 0; }
hook OnPlayerEnterVehicle(playerid, vehicleid, ispassenger) { return 1; }
hook OnPlayerExitVehicle(playerid, vehicleid) { return 1; }
hook OnPlayerStateChange(playerid, newstate, oldstate) { return 1; }
hook OnPlayerEnterCheckpoint(playerid) { return 1; }
hook OnPlayerLeaveCheckpoint(playerid) { return 1; }
hook OnPlayerEnterRaceCheckpoint(playerid) { return 1; }
hook OnPlayerLeaveRaceCheckpoint(playerid) { return 1; }
hook OnRconCommand(cmd[]) { return 1; }
hook OnPlayerRequestSpawn(playerid) { return 1; }
hook OnObjectMoved(objectid) { return 1; }
hook OnPlayerObjectMoved(playerid, objectid) { return 1; }
hook OnPlayerPickUpPickup(playerid, pickupid) { return 1; }
hook OnVehicleMod(playerid, vehicleid, componentid) { return 1; }
hook OnVehiclePaintjob(playerid, vehicleid, paintjobid) { return 1; }
hook OnVehicleRespray(playerid, vehicleid, color1, color2) { return 1; }
hook OnPlayerSelectedMenuRow(playerid, row) { return 1; }
hook OnPlayerExitedMenu(playerid) { return 1; }
hook OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid) { return 1; }
hook OnRconLoginAttempt(ip[], password[], success) { return 1; }
hook OnPlayerStreamIn(playerid, forplayerid) { return 1; }
hook OnPlayerStreamOut(playerid, forplayerid) { return 1; }
hook OnVehicleStreamIn(vehicleid, forplayerid) { return 1; }
hook OnVehicleStreamOut(vehicleid, forplayerid) { return 1; }
hook OnPlayerClickPlayer(playerid, clickedplayerid, source) { return 1; }
hook OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ) { return 1; }
