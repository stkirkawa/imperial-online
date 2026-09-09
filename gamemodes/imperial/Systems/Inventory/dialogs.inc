/*
 * IMPERIAL ROLE PLAY - Systems\Inventory\dialogs.inc
 * Обработчики диалогов инвентаря.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// -------------------- ������� ��������� --------------------
DialogInvMain(playerid, response, listitem)
{
    if(!response) return 1;
    
    new slot = GetInventorySlotByList(playerid, listitem);
    if(slot == -1) return ShowPlayerInventory(playerid);
    
    PlayerSelectedSlot[playerid] = slot;
    
    new item_name[32];
    GetItemName(PlayerInv[playerid][slot][invItemID], item_name, sizeof(item_name));
    
    new string[50];
    format(string, sizeof(string), "{FF0000}%s", item_name);
    
    ShowPlayerDialog(playerid, DIALOG_INV_ACTION, DIALOG_STYLE_LIST, string,
        "{FFFFFF}������������\n����������\n���������\n��������",
        "�������", "�����");
    return 1;
}

DialogInvAction(playerid, response, listitem)
{
    if(!response) return ShowPlayerInventory(playerid);
    
    new slot = PlayerSelectedSlot[playerid];
    if(slot < 0 || slot >= MAX_INVENTORY_SLOTS || PlayerInv[playerid][slot][invItemID] <= 0)
        return ShowPlayerInventory(playerid);
    
    new itemid = PlayerInv[playerid][slot][invItemID];
    
    switch(listitem) {
        case 0: {
            SendError(playerid, "�� �� ������ ������������ ���� �������");
            RemovePlayerItem(playerid, itemid, 1);
            ShowPlayerInventory(playerid);
        }
        case 1: {
            new item_name[32], str[256];
            GetItemName(itemid, item_name, sizeof(item_name));
            format(str, sizeof(str),
                "{FFFFFF}��������: {FFFF00}%s{FFFFFF}\n����������: {FFFF00}%d{FFFFFF}\n���: {FFFF00}%d{FFFFFF}\n����: {FFFF00}%d",
                item_name, PlayerInv[playerid][slot][invAmount], GetItemType(itemid), slot);
            ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_MSGBOX, "{FF8000}����������", str, "�������", "");
        }
        case 2: {
            ShowPlayerDialog(playerid, DIALOG_INV_DROP, DIALOG_STYLE_INPUT,
                "{FF0000}���������", "{FFFFFF}������� ����������:", "���������", "������");
        }
        case 3: {
            ShowPlayerDialog(playerid, DIALOG_INV_GIVE, DIALOG_STYLE_INPUT,
                "{FF0000}��������", "{FFFFFF}������� ID � ����������:\n{FFFF00}[ID] [����������]", "��������", "������");
        }
    }
    return 1;
}

DialogInvDrop(playerid, response, inputtext[])
{
    if(!response) return ShowPlayerInventory(playerid);
    
    new amount = strval(inputtext);
    if(amount <= 0) return ShowPlayerInventory(playerid);
    
    new slot = PlayerSelectedSlot[playerid];
    if(slot < 0) return ShowPlayerInventory(playerid);
    
    new itemid = PlayerInv[playerid][slot][invItemID];
    if(amount > PlayerInv[playerid][slot][invAmount])
        amount = PlayerInv[playerid][slot][invAmount];
    
    new item_name[32];
    GetItemName(itemid, item_name, sizeof(item_name));
    RemovePlayerItem(playerid, itemid, amount);
    
    new string[128];
    format(string, sizeof(string), "�� ��������� %s x%d", item_name, amount);
    SendClientMessage(playerid, 0xFF8000FF, string);
    
    ShowPlayerInventory(playerid);
    return 1;
}

DialogInvGive(playerid, response, inputtext[])
{
    if(!response) return ShowPlayerInventory(playerid);
    
    new targetid, amount;
    if(sscanf(inputtext, "ui", targetid, amount) || !IsPlayerConnected(targetid) || amount <= 0) {
        SendError(playerid, "������������ ������!");
        return ShowPlayerInventory(playerid);
    }
    
    new slot = PlayerSelectedSlot[playerid];
    if(slot < 0) return ShowPlayerInventory(playerid);
    
    new itemid = PlayerInv[playerid][slot][invItemID];
    if(amount > PlayerInv[playerid][slot][invAmount])
        amount = PlayerInv[playerid][slot][invAmount];
    
    if(!HasInventorySpace(targetid, itemid, amount)) {
        SendError(playerid, "� ������ ��� �����!");
        return ShowPlayerInventory(playerid);
    }
    
    new item_name[32];
    GetItemName(itemid, item_name, sizeof(item_name));
    
    RemovePlayerItem(playerid, itemid, amount);
    GivePlayerItem(targetid, itemid, amount);
    
    new string[128];
    format(string, sizeof(string), "�� �������� %s x%d ������ %s", item_name, amount, PlayerName(targetid));
    SendClientMessage(playerid, COLOR_SUCCESS, string);
    
    format(string, sizeof(string), "%s ������� ��� %s x%d", PlayerName(playerid), item_name, amount);
    SendClientMessage(targetid, COLOR_SUCCESS, string);
    
    ShowPlayerInventory(playerid);
    return 1;
}
