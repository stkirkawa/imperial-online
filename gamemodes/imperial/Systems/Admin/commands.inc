/*
 * IMPERIAL ROLE PLAY - Systems\Admin\commands.inc
 * Административные команды, сгруппированные по уровням доступа.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// ======================= ����� ������� ======================== //
// =============================================================== //

// ====================== ������� 1+ ������ ===================== //
CMD:alogin(playerid)
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� �����-����!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    
    if(AdminInfo[playerid][aLogged]) {
        AdminInfo[playerid][aLogged] = false;
        SetPlayerColor(playerid, COLOR_WHITE);
        SendAdminNotify(playerid, "�� ����� �� �����-������!");
        return 1;
    }
    
    mysql_format(connection, query, sizeof(query),
        "SELECT `rank`, `password`, `prefix` FROM `admins` WHERE `player_id` = %d",
        pInfo[playerid][pID]);
    mysql_tquery(connection, query, "OnAdminLoginCheck", "d", playerid);
    return 1;
}

CMD:arep(playerid)
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    ShowPlayer_Admin_Report_List(playerid);
    return 1;
}

CMD:tpc(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    
    new Float:x, Float:y, Float:z;
    if(sscanf(params, "p<,>fff", x, y, z))
        return SendCommandInfo(playerid, "/tpc [x] [y] [z]");
    
    SetPlayerPos(playerid, x, y, z);
    new string[144];
    format(string, sizeof(string), "�� ����������������� �� ���������� {FFFF00}%f %f %f", x, y, z);
    SendClientMessage(playerid, COLOR_WHITE, string);
    return 1;
}

CMD:a(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    
    new text[144];
    if(sscanf(params, "s[144]", text))
        return SendCommandInfo(playerid, "/a [���������]");
    
    new string[256], prefix[32];
    format(prefix, sizeof(prefix), GetAdminPrefix(playerid));
    format(string, sizeof(string), "[A] %s %s[%d]: %s", prefix, PlayerName(playerid), playerid, text);
    SendAdminMessage(0x9ACD32FF, string);
    return 1;
}

CMD:ap(playerid)
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new string[256];
    if(AdminInfo[playerid][aRank] >= 7) {
        format(string, sizeof(string),
            "{FFFFFF}��������� ��������������\n\
            ������� ��������������\n\
            ���������� ��������������\n\
            ��������� �������"
        );
    } else {
        format(string, sizeof(string),
            "{FFFFFF}��������� ��������������\n\
            ������� ��������������\n\
            ���������� ��������������"
        );
    }

    ShowPlayerDialog(playerid, DIALOG_ADMIN_PANEL_LIST, DIALOG_STYLE_LIST,
        "{FF0000}������ ��������������", string, "�������", "������");
    return 1;
}

CMD:admins(playerid)
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new count = 0;
    new string[512];
    string[0] = EOS;
    
    foreach(new i : Player) {
        if(IsPlayerConnected(i) && AdminInfo[i][aLogged] && AdminInfo[i][aRank] > 0) {
            format(string, sizeof(string), "%s {00BFFF}%s {FFFFFF}%s[%d]\n",
                string, GetAdminPrefix(i), PlayerName(i), i);
            count++;
        }
    }
    
    if(count == 0) {
        format(string, sizeof(string), "��� ��������������� ������");
    }
    
    new title[64];
    format(title, sizeof(title), "{FF0000}�������������� ������");
    ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_MSGBOX, title, string, "�������", "");
    return 1;
}

CMD:kick(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid, reason[32];
    if(sscanf(params, "is", targetid, reason))
        return SendCommandInfo(playerid, "/kick [id] [�������]");

    if(targetid == playerid)
        return SendError(playerid, "�� �� ������ ���� �������!");
    if(AdminInfo[targetid][aRank] > AdminInfo[playerid][aRank])
        return SendError(playerid, "�� �� ������� �������������� ���� �� ������ �����.����!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");

    new str[144];
    format(str, sizeof(str), "������������� %s ������ ������ %s. �������: %s", 
        PlayerName(playerid), PlayerName(targetid), reason);
    SendClientMessageToAll(COLOR_PUNUSH, str);
    Kick(targetid);
    return 1;
}

CMD:sp(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid;
    if(sscanf(params, "i", targetid))
        return SendCommandInfo(playerid, "/sp [id]");
    if(targetid == playerid)
        return SendError(playerid, "�� �� ������ ������� �� �����!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");

    StartSpectate(playerid, targetid);
    return 1;
}

CMD:spoff(playerid) 
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    if(GetPVarInt(playerid, "Spectating") == 0) 
        return SendError(playerid, "�� �� � ������ ������!");

    TogglePlayerSpectating(playerid, 0);
    
    new Float:posX = GetPVarFloat(playerid, "SpecPosX");
    new Float:posY = GetPVarFloat(playerid, "SpecPosY");
    new Float:posZ = GetPVarFloat(playerid, "SpecPosZ");
    new Float:angle = GetPVarFloat(playerid, "SpecAngle");
    new interior = GetPVarInt(playerid, "SpecInterior");
    new virtualworld = GetPVarInt(playerid, "SpecVirtualWorld");

    SetPlayerPos(playerid, posX, posY, posZ);
    SetPlayerFacingAngle(playerid, angle);
    SetPlayerInterior(playerid, interior);
    SetPlayerVirtualWorld(playerid, virtualworld);

    DeletePVar(playerid, "Spectating");
    DeletePVar(playerid, "SpecTarget");
    DeletePVar(playerid, "SpecPosX");
    DeletePVar(playerid, "SpecPosY");
    DeletePVar(playerid, "SpecPosZ");
    DeletePVar(playerid, "SpecAngle");
    DeletePVar(playerid, "SpecInterior");
    DeletePVar(playerid, "SpecVirtualWorld");

    return 1;
}

CMD:agm(playerid)
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    if(!is_gm) {
        SetPlayerHealth(playerid, 9999999);
        SendClientMessage(playerid, 0xFF8C00FF, "�� �������� ��");
        is_gm = true;
    } else {
        SetPlayerHealth(playerid, 100);
        SendClientMessage(playerid, 0xFF8C00FF, "�� ��������� ��");
        is_gm = false;
    }
    return 1;
}

CMD:spawn(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid, str[128];
    if(sscanf(params, "i", targetid)) {
        format(str, sizeof(str), "[A] ������������� %s ��������� ����", PlayerName(playerid));
        SendAdminMessage(COLOR_GREY, str);
        SpawnPlayer(playerid);
        return 1;
    } else {
        if(!pInfo[targetid][pLogged])
            return SendError(playerid, "����� �� ������!");
        SpawnPlayer(targetid);
        format(str, sizeof(str), "[A] ������������� %s[%d] ��������� ������ %s[%d]", 
            PlayerName(playerid), playerid, PlayerName(targetid), targetid);
        SendAdminMessage(COLOR_GREY, str);
    }
    return 1; 
}

CMD:slap(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid, type;
    if(sscanf(params, "ii", targetid, type)) 
        return SendCommandInfo(playerid, "/slap [id] [���: 1 - �����/2 - ����]");
    
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);

    if(type == 1) 
        SetPlayerPos(targetid, x, y, z+2.0);
    else if(type == 2) 
        SetPlayerPos(targetid, x, y, z-2.0);
    else
        return SendError(playerid, "��� ������ ���� �����!");
    
    new string[128];
    format(string, sizeof(string), "[A] ������������� %s[%d] ��������� ������ %s[%d]", 
        PlayerName(playerid), playerid, PlayerName(targetid), targetid);
    SendAdminMessage(COLOR_GREY, string);

    format(string, sizeof(string), "��� ��������� ������������� %s", PlayerName(playerid));
    SendClientMessage(targetid, COLOR_GREY, string);

    return 1;
}

CMD:ans(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 1)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid, text[128];
    if(sscanf(params, "is[128]", targetid, text)) 
        return SendCommandInfo(playerid, "/ans [id] [���������]");

    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");

    new string[144], prefix[32];
    format(prefix, sizeof(prefix), GetAdminPrefix(playerid));
    format(string, sizeof(string), "%s %s[%d] ������ %s[%d]: %s", 
        prefix, PlayerName(playerid), playerid, PlayerName(targetid), targetid, text);
    SendAdminMessage(COLOR_ANSWER, string);

    format(string, sizeof(string), "%s %s[%d] ���: %s", prefix, PlayerName(playerid), playerid, text);
    SendClientMessage(targetid, COLOR_ANSWER, string);

    return 1;
}

// ====================== ������� 2+ ������ ===================== //
CMD:fixcar(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid;
    if(sscanf(params, "i", targetid)) 
        return SendCommandInfo(playerid, "/fixcar [id]");

    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");

    new vehicleid = GetPlayerVehicleID(targetid);
    if(vehicleid == INVALID_VEHICLE_ID)
        return SendError(playerid, "����� �� ��������� � ������������ ��������!");

    SetVehicleHealth(vehicleid, 1000.0);
    
    new admin_name[MAX_PLAYER_NAME], t_name[MAX_PLAYER_NAME], string[128];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));
    GetPlayerName(targetid, t_name, sizeof(t_name));

    format(string, sizeof(string), "������������� %s ������� ��������� ������ %s", admin_name, t_name);
    SendClientMessageToAll(COLOR_PUNUSH, string);
    
    return 1;
}

CMD:flipcar(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid == INVALID_VEHICLE_ID)
        return SendError(playerid, "�� �� ���������� � ������������ ��������!");

    new Float:x, Float:y, Float:z, Float:angle;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, angle);

    SetVehiclePos(vehicleid, x, y, z + 2.0);
    SetVehicleZAngle(vehicleid, angle);
    SetVehicleHealth(vehicleid, 1000.0);
    
    SendClientMessage(playerid, COLOR_PUNUSH, "�� ����������� ���� ���������!");
    return 1;
}

CMD:goto(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    
    new targetid;
    if(sscanf(params, "i", targetid)) 
        return SendCommandInfo(playerid, "/goto [id]");

    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");
    
    TeleportToPlayer(playerid, targetid);
    return 1;
}

CMD:shp(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    
    new targetid, Float:hp;
    if(sscanf(params, "if", targetid, hp)) 
        return SendCommandInfo(playerid, "/shp [id] [1-260]");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");
    if(hp > 260)
        return SendError(playerid, "������ ������ ������ ������ 260 ��������!");
    
    SetPlayerHealth(targetid, hp);

    new string[144];
    format(string, sizeof(string), "[A] ������������� %s[%d] ����� ������ %s[%d] �������� %d", 
        PlayerName(playerid), playerid, PlayerName(targetid), targetid, floatround(hp));
    SendAdminMessage(COLOR_GREY, string);

    format(string, sizeof(string), "������������� %s ����� ��� %d ��������", PlayerName(playerid), floatround(hp));
    SendClientMessage(targetid, COLOR_GREY, string);

    return 1;
}

CMD:jetpack(playerid)
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USEJETPACK);
    return 1;
}

CMD:mute(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid, minutes, reason[64];
    if(sscanf(params, "uis[64]", targetid, minutes, reason))
        return SendCommandInfo(playerid, "/mute [id] [������: 1-120] [�������]");

    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");
    if(AdminInfo[targetid][aRank] >= AdminInfo[playerid][aRank])
        return SendError(playerid, "������ �������� �������������� ���� ��� ������� ���!");

    if(minutes < 1 || minutes > 120)
        return SendError(playerid, "��� �� 1 �� 120 �����!");

    mysql_format(connection, query, sizeof(query), 
        "SELECT `mute_id` FROM `mutes` WHERE `user_id` = '%d' AND `mute_time` > '%d' LIMIT 1", 
        pInfo[targetid][pID], gettime());
    mysql_tquery(connection, query, "CheckMuteOnline", "iiis", playerid, targetid, minutes, reason);
    return 1;
}

CMD:aveh(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new vehicleid, car_color1, car_color2;
    if(sscanf(params, "iii", vehicleid, car_color1, car_color2))
        return SendCommandInfo(playerid, "/aveh [id ����] [���� 1] [���� 2]");
    
    if(vehicleid < 400 || vehicleid > 611)
        return SendError(playerid, "�������� ID ����������! (400-611)");
    
    if(car_color1 < 0 || car_color1 > 255 || car_color2 < 0 || car_color2 > 255)
        return SendError(playerid, "����� ������ ���� �� 0 �� 255!");
    
    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    new created_vehicle = CreateVehicle(vehicleid, x, y, z, a, car_color1, car_color2, -1, 0);
    
    if(created_vehicle == INVALID_VEHICLE_ID)
        return SendError(playerid, "�� ������� ������� ���������!");
    
    AdminVehicleCount++;
    
    new vehicle_name[32];
    GetVehicleName(vehicleid, vehicle_name, sizeof(vehicle_name));
    
    new string[256];
    format(string, sizeof(string), "[A] �� ������� {FFFF00}%s{FFFFFF} � ������� %d, %d", 
        vehicle_name, car_color1, car_color2);
    SendClientMessage(playerid, COLOR_GREY, string);
    
    PutPlayerInVehicle(playerid, vehicleid, 0);
    return 1;
}

CMD:delaveh(playerid)
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    if(AdminVehicleCount == 0)
        return SendError(playerid, "��� ��������� ��������� �����!");

    new string[144];
    format(string, sizeof(string), "[A] ������������� %s ������ ��� ��������� �� ������ (����: %d)", 
        PlayerName(playerid), AdminVehicleCount);
    
    for(new i = 1; i < MAX_VEHICLES; i++) {
        DestroyVehicle(i);
    }
    
    AdminVehicleCount = 0;
    
    SendAdminMessage(COLOR_GREY, string);
    return 1;
}

CMD:setint(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new interior;
    if(sscanf(params, "u", interior))
        return SendCommandInfo(playerid, "/setint [id ���������]");

    new str[70];
    format(str, sizeof(str), "�� ���������������� � �������� %d", interior);
    SendClientMessage(playerid, COLOR_GREY, str);
    SetPlayerInterior(playerid, interior);
    return 1;
}

CMD:setworld(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 2)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new world;
    if(sscanf(params, "u", world))
        return SendCommandInfo(playerid, "/setworld [id ������������ ����]");
        
    new str[70];
    format(str, sizeof(str), "�� ���������������� � ����������� ��� %d", world);
    SendClientMessage(playerid, COLOR_GREY, str);

    SetPlayerVirtualWorld(playerid, world);
    return 1;
}
// ====================== ������� 3+ ������ ===================== //
CMD:getip(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
        
    new targetid;
    if(sscanf(params, "u", targetid))
        return SendCommandInfo(playerid, "/getip [id ������]");

    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");

    new string[144];
    format(string, sizeof(string), "IP | {FFFFFF}�����: {7FFFD4}%s {FFFFFF} | LastIP: {7FFFD4}%s",
        pInfo[targetid][pIP],
        pInfo[targetid][pLastIP]
    );
    SendClientMessage(playerid, 0x5F9EA0FF, string);
    return 1;
}

CMD:gethere(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid;
    if(sscanf(params, "u", targetid))
        return SendCommandInfo(playerid, "/gethere [id ������]");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");

    new Float:posX, Float:posY, Float:posZ, Float:angle;
    GetPlayerPos(playerid, posX, posY, posZ);
    GetPlayerFacingAngle(playerid, angle);

    new vehicleid = GetPlayerVehicleID(targetid);
    if(vehicleid != 0) {
        RemovePlayerFromVehicle(targetid);
        SetPlayerPos(targetid, posX + 1, posY + 1, posZ);
    } else {
        SetPlayerPos(targetid, posX + 1, posY + 1, posZ);
    }
    SetPlayerInterior(targetid, GetPlayerInterior(playerid));
    SetPlayerVirtualWorld(targetid, GetPlayerVirtualWorld(playerid));
    SetPlayerFacingAngle(targetid, angle);
    new string[144];
    format(string, sizeof(string), "[A] ������������� %s[%d] �������������� ������ %s[%d] � ����", 
        PlayerName(playerid), playerid, PlayerName(targetid), targetid);
    SendAdminMessage(COLOR_GREY, string);

    format(string, sizeof(string), "������������� %s �������������� ��� � ����!", PlayerName(playerid));
    SendClientMessage(targetid, COLOR_GREY, string);

    format(string, sizeof(string), "�� ��������������� ������ %s � ����!", PlayerName(targetid));
    SendClientMessage(playerid, COLOR_GREY, string);

    GameTextForPlayer(targetid, "~g~��������~w~!", 1000, 3);
    PlayerPlaySound(targetid, 1138, posX, posY, posZ); 

    return 1;
}

CMD:unmute(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid;
    if(sscanf(params, "u", targetid))
        return SendCommandInfo(playerid, "/unmute [id]");

    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");

    pInfo[targetid][pMute] = 0;

    mysql_format(connection, query, sizeof(query), 
        "DELETE FROM `mutes` WHERE `user_id` = '%d' AND `mute_time` > '%d'", 
        pInfo[targetid][pID], gettime());
    mysql_tquery(connection, query);

    new admin_name[MAX_PLAYER_NAME], t_name[MAX_PLAYER_NAME], string[144];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));  
    GetPlayerName(targetid, t_name, sizeof(t_name));

    format(string, sizeof(string), "������������� %s ���� ��� � ������ %s", admin_name, t_name);
    SendClientMessageToAll(COLOR_PUNUSH, string);
    return 1;
}

CMD:unoffmute(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new target_name[MAX_PLAYER_NAME];
    if(sscanf(params, "s[24]", target_name))
        return SendCommandInfo(playerid, "/unoffmute [�������]");

    new target = INVALID_PLAYER_ID;
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(IsPlayerConnected(i))
        {
            new name[MAX_PLAYER_NAME];
            GetPlayerName(i, name, sizeof(name));
            if(!strcmp(name, target_name, true))
            {
                target = i;
                break;
            }
        }
    }

    if(target != INVALID_PLAYER_ID)
    {
        pInfo[target][pMute] = 0;
        
        mysql_format(connection, query, sizeof(query), 
            "DELETE FROM `mutes` WHERE `user_id` = '%d' AND `mute_time` > '%d'", 
            pInfo[target][pID], gettime());
        mysql_tquery(connection, query);

        new admin_name[MAX_PLAYER_NAME], t_name[MAX_PLAYER_NAME], string[144];
        GetPlayerName(playerid, admin_name, sizeof(admin_name));  
        GetPlayerName(target, t_name, sizeof(t_name));

        format(string, sizeof(string), "������������� %s ���� ��� � ������ %s", admin_name, t_name);
        SendClientMessageToAll(COLOR_PUNUSH, string);
        return 1;
    }
    else
    {
        mysql_format(connection, query, sizeof(query), 
            "SELECT `mute_id` FROM `mutes` WHERE `name` = '%e' AND `mute_time` > '%d' LIMIT 1", 
            target_name, gettime());
        mysql_tquery(connection, query, "CheckUnoffMute", "ds", playerid, target_name);
        return 1;
    }
}

CMD:rcars(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new radius;
    if(sscanf(params, "i", radius) || radius < 1 || radius > 350)
        return SendCommandInfo(playerid, "/rcars [������: 1-350]");

    new Float:posX, Float:posY, Float:posZ, count;
    GetPlayerPos(playerid, posX, posY, posZ);

    for(new i = 1; i < MAX_VEHICLES; i++) {
        new Float:x, Float:y, Float:z;
        GetVehiclePos(i, x, y, z);
        if(GetPlayerDistanceFromPoint(playerid, x, y, z) <= radius) {
            DestroyVehicle(i);
            count++;
        }
    }

    if(!count) return SendError(playerid, "� ���� ������� ��� �����!");

    new str[128];
    format(str, sizeof(str), "[A] ������������� %s ������ %d ����� � ������� %d �", PlayerName(playerid), count, radius);
    SendAdminMessage(COLOR_GREY, str);
    return 1;
}

CMD:msg(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new text[144];
    if(sscanf(params, "s[144]", text))
        return SendCommandInfo(playerid, "/msg [���������]");
    
    new string[144], prefix[32];
    format(prefix, sizeof(prefix), GetAdminPrefix(playerid));
    format(string, sizeof(string), "%s %s[%d]: {FFFFFF}%s", prefix, PlayerName(playerid), playerid, text);
    SendClientMessageToAll(0xFF0000FF, string);
    return 1;
}

CMD:cc(playerid,params[])
{
	if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new string[144];
    for(new i=0; i<60; i++) 
    {
        SendClientMessageToAll(COLOR_WHITE, " ");
    }
    format(string, sizeof(string),"[A] ������������� %s ������� ���", PlayerName(playerid));
    SendClientMessageToAll(COLOR_GREY, string);
    return 1;
}

CMD:tempskin(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid, skin;
    if(sscanf(params, "ui", targetid))
        return SendCommandInfo(playerid, "/tempskin [id] [id �����]");
    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");
    
    if(skin < 1 || skin > 311) 
        return SendError(playerid, "���� ������ ���� �� 1 �� 311 id!");
    
    SetPlayerSkin(targetid, skin);

    new string[144];
    format(string, sizeof(string), "[A] ������������� %s ����� ������ %s ��������� ���� %d", PlayerName(playerid), PlayerName(targetid), skin);
    SendAdminMessage(COLOR_GREY, string);

    format(string, sizeof(string), "������������� %s ����� ��� ��������� ���� %d", PlayerName(playerid), skin);
    SendClientMessage(targetid, COLOR_GREY, string);

    return 1;
}

CMD:freeze(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid;
    if(sscanf(params, "u", targetid))
        return SendCommandInfo(playerid, "/freeze [id]");
    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");

    TogglePlayerControllable(playerid, false);
    new string[144];
    format(string, sizeof(string), "[A] ������������� %s ��������� ������ %s", PlayerName(playerid), PlayerName(targetid));
    SendAdminMessage(COLOR_GREY, string);

    format(string, sizeof(string), "��� ��������� ������������� %s", PlayerName(playerid));
    SendClientMessage(targetid, COLOR_GREY, string);
    return 1;
}

CMD:unfreeze(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 3)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid;
    if(sscanf(params, "u", targetid))
        return SendCommandInfo(playerid, "/unfreeze [id]");
    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");

    TogglePlayerControllable(playerid, true);
    new string[144];
    format(string, sizeof(string), "[A] ������������� %s ���������� ������ %s", PlayerName(playerid), PlayerName(targetid));
    SendAdminMessage(COLOR_GREY, string);

    format(string, sizeof(string), "��� ���������� ������������� %s", PlayerName(playerid));
    SendClientMessage(targetid, COLOR_GREY, string);
    return 1;
}

// ====================== ������� 4+ ������ ===================== //
CMD:achangename(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 4)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new target_name[24];
    if(sscanf(params, "s[24]", target_name))
        return SendCommandInfo(playerid, "/achangename [�������]");

    if(strlen(target_name) < 3 || strlen(target_name) > 24)
        return SendError(playerid, "������� ������ ���� �� 3 �� 24 ��������!");
    
    SetPlayerName(playerid, target_name);
    
    new string[144];
    format(string, sizeof(string), "[A] ������������� %s ��������� ���� �������� ���-���� %s", PlayerName(playerid), target_name);
    SendAdminMessage(COLOR_GREY, string);
    return 1;
}

CMD:veh(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 4)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid, vehicleid, car_color1, car_color2;
    if(sscanf(params, "uiii", targetid, vehicleid, car_color1, car_color2))
        return SendCommandInfo(playerid, "/veh [id ������] [id ����] [���� 1] [���� 2]");

    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");

    if(vehicleid < 400 || vehicleid > 611)
        return SendError(playerid, "�������� ID ����������! (400-611)");
    
    if(car_color1 < 0 || car_color1 > 255 || car_color2 < 0 || car_color2 > 255)
        return SendError(playerid, "����� ������ ���� �� 0 �� 255!");
    
    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(targetid, x, y, z);
    GetPlayerFacingAngle(targetid, a);

    new created_vehicle = CreateVehicle(vehicleid, x, y, z, a, car_color1, car_color2, -1, 0);
    
    if(created_vehicle == INVALID_VEHICLE_ID)
        return SendError(playerid, "�� ������� ������� ���������!");
    
    AdminVehicle[created_vehicle] = true;
    AdminVehicleCount++;
    
    new vehicle_name[32];
    GetVehicleName(vehicleid, vehicle_name, sizeof(vehicle_name));
    
    new admin_name[MAX_PLAYER_NAME], t_name[MAX_PLAYER_NAME], string[256];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));
    GetPlayerName(targetid, t_name, sizeof(t_name));
    
    format(string, sizeof(string), "[A] ������������� %s ����� ������ %s[%d] ��������� %s", 
        admin_name, t_name, targetid, vehicle_name);
    SendAdminMessage(COLOR_GREY, string);
    
    PutPlayerInVehicle(targetid, created_vehicle, 0);

    return 1;
}

CMD:delv(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 4)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid;
    if(sscanf(params, "u", targetid))
        return SendCommandInfo(playerid, "/delv [id ������]");

    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");

    new vehicleid = GetPlayerVehicleID(targetid);
    if(vehicleid == INVALID_VEHICLE_ID)
        return SendError(playerid, "����� �� ��������� � ������������ ��������!");

    if(!AdminVehicle[vehicleid])
        return SendError(playerid, "��� �� ��������� ���������!");

    new admin_name[MAX_PLAYER_NAME], t_name[MAX_PLAYER_NAME], string[128];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));
    GetPlayerName(targetid, t_name, sizeof(t_name));

    DestroyVehicle(vehicleid);

    AdminVehicle[vehicleid] = false;
    AdminVehicleCount--;

    format(string, sizeof(string), "[A] ������������� %s ������ ��������� ������ %s[%d]", 
        admin_name, t_name, targetid);
    SendAdminMessage(COLOR_GREY, string);

    SendClientMessage(targetid, COLOR_PUNUSH, "��� ��������� ��������� ��� �����!");

    return 1;
}

CMD:setweather(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 4)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new weather;
    if(sscanf(params, "i", weather))
        return SendCommandInfo(playerid, "/setweather [id ������]");
    
    if(weather < 0 || weather > 255) 
        return SendError(playerid, "������ ������ ���� �� 0 �� 255!");
    
    SetWeather(weather);
    new string[144];
    format(string, sizeof(string), "[A] ������������� %s �������� ������ %d", PlayerName(playerid), weather);
    SendAdminMessage(COLOR_GREY, string);
    return 1;
}

CMD:event(playerid)
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 4)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new open_tp = false;
    new status[9];
    new str[254];

    if(!open_tp) format(status, sizeof(status), "�������");
    else format(status, sizeof(status), "�������");
    
    format(str, sizeof(str), "%s ��������\n������� � ���� ������\n������ ������\n������ ��������� ����\n������ ������ ��������\n������ �����", status);
    ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_LIST, "{FF0000}���� �����������", 
        str, "�������", "������"
    );
    return 1;
}
// ====================== ������� 5+ ������ ===================== //
CMD:ban(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 5)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new target_name[MAX_PLAYER_NAME], days, reason[64];
    if(sscanf(params, "s[24]is[64]", target_name, days, reason))
        return SendCommandInfo(playerid, "/ban [�������] [���-�� ����] [�������]");

    if(days <= 0 || days > 365) 
        return SendError(playerid, "���� ���� ������ ���� �� 1 �� 365 ����.");

    new target = INVALID_PLAYER_ID;
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(IsPlayerConnected(i))
        {
            new name[MAX_PLAYER_NAME];
            GetPlayerName(i, name, sizeof(name));
            if(!strcmp(name, target_name, true))
            {
                target = i;
                break;
            }
        }
    }

    if(target != INVALID_PLAYER_ID)
    {
        if(AdminInfo[target][aRank] >= AdminInfo[playerid][aRank])
            return SendError(playerid, "������ �������� �������������� ������ ��� ������� ��� �� ������!");

        mysql_format(connection, query, sizeof(query), 
            "SELECT `ban_id` FROM `banlist` WHERE `user_id` = '%d' AND `ban_expire` > '%d' LIMIT 1", 
            pInfo[target][pID], gettime());
        mysql_tquery(connection, query, "CheckAlreadyBanned", "iiis", playerid, target, days, reason);
        return 1;
    }
    else
    {
        mysql_format(connection, query, sizeof(query), 
            "SELECT `ban_id` FROM `banlist` WHERE `name` = '%e' AND `ban_expire` > '%d' LIMIT 1", 
            target_name, gettime());
        mysql_tquery(connection, query, "CheckAlreadyBannedOffline", "dsii", playerid, target_name, days, reason);
        return 1;
    }
}

CMD:unban(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 5) 
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new target_name[MAX_PLAYER_NAME]; 
    if(sscanf(params, "s[24]", target_name))
        return SendCommandInfo(playerid, "/unban [Nick_Name]");

    mysql_format(connection, query, sizeof(query), 
        "SELECT `name`, `ban_expire` FROM `banlist` WHERE `name` = '%e' LIMIT 1", target_name);
    
    mysql_tquery(connection, query, "OnUnbanCheck", "ds", playerid, target_name);
    return 1;
}

CMD:astats(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 5)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new targetid;
    if(sscanf(params, "u", targetid))
        return SendCommandInfo(playerid, "/astats [id ��������������]");

    if(!IsPlayerConnected(targetid))
        return SendError(playerid, "����� �� ������!");
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� �����������!");
    if(AdminInfo[targetid][aRank] < 1)
        return SendError(playerid, "���� ����� �� �������� ���������������!");

    new dialog_str[2048];
    
    new total_week_seconds = 0;
    for(new d = 0; d < 7; d++) {
        total_week_seconds += AdminInfo[targetid][aWeekDays][d];
    }
    
    new admin_name[MAX_PLAYER_NAME];
    GetPlayerName(targetid, admin_name, sizeof(admin_name));
    
    format(dialog_str, sizeof(dialog_str),
        "{FFFFFF}�������������: {FFD700}%s\n\n\
        {FFFFFF}�������� �����: {FFD700}%d\n\n\
        {FFFFFF}���������� �� ������� �� ���� ������:\n\
        {FFFFFF}�����������: {FFD700}%s\n\
        {FFFFFF}�������: {FFD700}%s\n\
        {FFFFFF}�����: {FFD700}%s\n\
        {FFFFFF}�������: {FFD700}%s\n\
        {FFFFFF}�������: {FFD700}%s\n\
        {FFFFFF}�������: {FFD700}%s\n\
        {FFFFFF}�����������: {FFD700}%s\n\n\
        {FFFFFF}����� ����� �� ������: {FFD700}%s",
        admin_name,
        AdminInfo[targetid][aAnsReport],
        FormatTime(AdminInfo[targetid][aWeekDays][0]),
        FormatTime(AdminInfo[targetid][aWeekDays][1]),
        FormatTime(AdminInfo[targetid][aWeekDays][2]),
        FormatTime(AdminInfo[targetid][aWeekDays][3]),
        FormatTime(AdminInfo[targetid][aWeekDays][4]),
        FormatTime(AdminInfo[targetid][aWeekDays][5]),
        FormatTime(AdminInfo[targetid][aWeekDays][6]),
        FormatTime(total_week_seconds)
    );
    
    ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_MSGBOX,
        "{FF0000}���������� ��������������", dialog_str, "��������", "�������");
        
    return 1;
}

CMD:banip(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 5)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new ban_ip[16], reason[64];
    if(sscanf(params, "s[16]s[64]", ban_ip, reason))
        return SendCommandInfo(playerid, "/banip [IP] [�������]");

    if(strlen(ban_ip) < 7 || strlen(ban_ip) > 16)
        return SendError(playerid, "������������ IP �����!");

    new found = 0;
    foreach(new i : Player) {
        if(IsPlayerConnected(i) && pInfo[i][pLogged]) {
            new player_ip[16];
            GetPlayerIp(i, player_ip, sizeof(player_ip));
            if(!strcmp(player_ip, ban_ip, false)) {
                found++;
                
                new admin_name[MAX_PLAYER_NAME], t_name[MAX_PLAYER_NAME], string[256];
                GetPlayerName(playerid, admin_name, sizeof(admin_name));
                GetPlayerName(i, t_name, sizeof(t_name));
                
                new time_now = gettime();
                mysql_format(connection, query, sizeof(query), 
                    "INSERT INTO `ip_bans` (`ip_address`, `account_id`, `player_name`, `nick_admin`, `date_ban`, `reason`) VALUES ('%e', '%d', '%e', '%e', '%d', '%e')", 
                    ban_ip, pInfo[i][pID], t_name, admin_name, time_now, reason);
                mysql_tquery(connection, query);
                
                format(string, sizeof(string), "������������� %s �������� ������� IP %s ������ %s. �������: %s", admin_name, ban_ip, t_name, reason);
                SendAdminMessage(COLOR_GREY, string);
                
                SendClientMessage(i, COLOR_PUNUSH, "��� IP ��� ������� ���������������!");
                
                SetTimerEx("DelayedKick", 500, false, "i", i);
            }
        }
    }

    if(found == 0) {
        mysql_format(connection, query, sizeof(query), 
            "SELECT `id` FROM `users` WHERE `ip` = '%e' LIMIT 1", ban_ip);
        mysql_tquery(connection, query, "OnBanIPOffline", "dsis", playerid, ban_ip, reason, gettime());
        
        return SendError(playerid, "����� � ����� IP �� ������ ������, �������� ��...");
    }

    return 1;
}

CMD:unbanip(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 5)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);

    new ip[16];
    if(sscanf(params, "s[16]", ip))
        return SendCommandInfo(playerid, "/unbanip [IP]");

    mysql_format(connection, query, sizeof(query), 
        "SELECT `ip_address` FROM `ip_bans` WHERE `ip_address` = '%e' LIMIT 1", 
        ip);
    mysql_tquery(connection, query, "OnUnbanIPCheck", "is", playerid, ip);
    return 1;
}

// ====================== ������� 6+ ������ ===================== //
CMD:setadmin(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 6)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    
    new targetid, arank;
    if(sscanf(params, "ud", targetid, arank))
        return SendCommandInfo(playerid, "/setadmin [id] [0-5]");
    
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");
    if(arank < 0 || arank > 5)
        return SendError(playerid, "������� ����� ���� ����� ���� �� 0 �� 5!");
    
    new string[144];
    
    if(arank == 0) {
        mysql_format(connection, query, sizeof(query),
            "DELETE FROM `admins` WHERE `player_id` = %d", pInfo[targetid][pID]);
        mysql_tquery(connection, query);
        
        AdminInfo[targetid][aRank] = 0;
        AdminInfo[targetid][aLogged] = false;
        AdminInfo[targetid][aPassword][0] = EOS;
        AdminInfo[targetid][aPrefix][0] = EOS;
        
        format(string, sizeof(string), "������� | {FFFFFF}�� ����� �����-����� � {FFFF00}%s", PlayerName(targetid));
        SendClientMessage(playerid, COLOR_ADMIN, string);
        SendClientMessage(targetid, COLOR_ADMIN, "������� | {FFFFFF}� ��� ����� �����-�����");
        SetPlayerColor(targetid, COLOR_WHITE);
    } else {
        if(AdminInfo[targetid][aRank] > 0) {
            SaveAdminData(targetid);
        } else {
            mysql_format(connection, query, sizeof(query),
                "INSERT INTO `admins` (`player_id`, `rank`, `password`, `prefix`) VALUES (%d, %d, '', '')",
                pInfo[targetid][pID], arank);
            mysql_tquery(connection, query);
        }
        
        AdminInfo[targetid][aRank] = arank;
        AdminInfo[targetid][aPrefix][0] = EOS;
        
        format(string, sizeof(string), "������� | {FFFFFF}������ {FFFF00}%s {FFFFFF}�����-����� {FFD700}%d ������",
            PlayerName(targetid), arank);
        SendClientMessage(playerid, COLOR_ADMIN, string);
        
        format(string, sizeof(string), "������� | {FFFFFF}��� ������ {FFD700}%d �������{FFFFFF} �����-����", arank);
        SendClientMessage(targetid, COLOR_ADMIN, string);
    }
    return 1;
}


// ====================== ������� 7+ ������ ===================== //
CMD:giveitem(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 7)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    
    new targetid, itemid, amount;
    if(sscanf(params, "uii", targetid, itemid, amount))
        return SendCommandInfo(playerid, "/giveitem [ID] [ID ��������] [����������]");
    
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");
    
    new bool:found = false;
    for(new i = 0; i < TotalItems; i++) {
        if(Items[i][itemID] == itemid) { found = true; break; }
    }
    if(!found) return SendError(playerid, "�������� ID ��������!");
    if(amount <= 0) return SendError(playerid, "������� ����������!");
    
    GivePlayerItem(targetid, itemid, amount);
    
    new item_name[32], string[128];
    GetItemName(itemid, item_name, sizeof(item_name));
    
    format(string, sizeof(string), "�� ������ %s x%d ������ %s", item_name, amount, PlayerName(targetid));
    SendClientMessage(playerid, 0x00FFFFFF, string);
    
    format(string, sizeof(string), "��� ����� %s x%d", item_name, amount);
    SendClientMessage(targetid, COLOR_SUCCESS, string);
    return 1;
}

CMD:setprefix(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 7)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    
    new targetid, new_prefix[32];
    if(sscanf(params, "us[32]", targetid, new_prefix))
        return SendCommandInfo(playerid, "/setprefix [ID] [����� �������]");
    
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");
    
    new len = strlen(new_prefix);
    if(len < 1 || len > 31)
        return SendError(playerid, "������� �� 1 �� 31 �������!");
    
    for(new i = 0; i < len; i++) {
        new c = new_prefix[i];
        if(!(192 <= c <= 255) && c != 168 && c != 184 && c != ' ' && c != '-') {
            SendError(playerid, "������ ������� �����, ������� � �����!");
            return 1;
        }
    }
    
    format(AdminInfo[targetid][aPrefix], 32, "%s", new_prefix);
    
    new string[128];
    format(string, sizeof(string), "������� | {FFFFFF}������ {FFFF00}%s {FFFFFF}���������� �������: {FFD700}%s",
        PlayerName(targetid), new_prefix);
    SendClientMessage(playerid, COLOR_ADMIN, string);
    
    format(string, sizeof(string), "������� | {FFFFFF}��� ���������� �������: {FFD700}%s", new_prefix);
    SendClientMessage(targetid, COLOR_ADMIN, string);
    return 1;
}

CMD:givemoney(playerid, params[])
{
    if(pInfo[playerid][pRegister] != 1)
        return SendError(playerid, "�� �� ����������������!");
    if(!pInfo[playerid][pLogged])
        return SendError(playerid, "�� �� ������������!");
    if(AdminInfo[playerid][aRank] < 7)
        return SendError(playerid, "� ��� ��� ����!");
    
    AdminNotLoginError(playerid);
    
    new targetid, money;
    if(sscanf(params, "ui", targetid, money))
        return SendCommandInfo(playerid, "/givemoney [id] [���-��]");
    
    if(!pInfo[targetid][pLogged])
        return SendError(playerid, "����� �� ������!");

    if(money < 0 || money > 250000000)
        return SendError(playerid, "�� �� ������ ������ ����� ������ 0$ � ����� 250.000.000$");
    
    GiveMoney(targetid, money);

    new string[144];
    format(string, sizeof(string), "[A] ������������� %s ����� ������ %s - %d ������", PlayerName(playerid), PlayerName(targetid), money);
    SendAdminMessage(COLOR_GREY, string);

    format(string, sizeof(string), "������������� %s ����� ��� %d ������", PlayerName(playerid), money);
    SendClientMessage(targetid, COLOR_GREY, string);

    return 1;
}
