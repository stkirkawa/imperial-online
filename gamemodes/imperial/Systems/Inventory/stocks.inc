/*
 * IMPERIAL ROLE PLAY - Systems\Inventory\stocks.inc
 * Вспомогательные функции инвентаря.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// =============================================================== //
// =================== ��������� ����� ========================== //
// =============================================================== //
stock GetItemName(itemid, name[], size)
{
    for(new i = 0; i < TotalItems; i++) {
        if(Items[i][itemID] == itemid) {
            format(name, size, "%s", Items[i][itemName]);
            return 1;
        }
    }
    format(name, size, "����������");
    return 0;
}

stock GetItemType(itemid)
{
    for(new i = 0; i < TotalItems; i++) {
        if(Items[i][itemID] == itemid)
            return Items[i][itemType];
    }
    return 0;
}

stock GetItemMaxStack(itemid)
{
    for(new i = 0; i < TotalItems; i++) {
        if(Items[i][itemID] == itemid)
            return Items[i][itemMaxStack];
    }
    return 1;
}

stock IsItemStackable(itemid) { return GetItemMaxStack(itemid) > 1; }

stock FindFreeSlot(playerid)
{
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++) {
        if(PlayerInv[playerid][i][invItemID] == -1)
            return i;
    }
    return -1;
}

stock GetPlayerItemCount(playerid, itemid)
{
    new total = 0;
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++) {
        if(PlayerInv[playerid][i][invItemID] == itemid)
            total += PlayerInv[playerid][i][invAmount];
    }
    return total;
}

stock HasInventorySpace(playerid, itemid, amount)
{
    if(IsItemStackable(itemid)) {
        new max_stack = GetItemMaxStack(itemid);
        new needed = amount;
        
        for(new i = 0; i < MAX_INVENTORY_SLOTS; i++) {
            if(PlayerInv[playerid][i][invItemID] == itemid) {
                new space = max_stack - PlayerInv[playerid][i][invAmount];
                if(space >= needed) return 1;
                needed -= space;
            }
        }
        
        new free_slots = 0;
        for(new i = 0; i < MAX_INVENTORY_SLOTS; i++) {
            if(PlayerInv[playerid][i][invItemID] == -1)
                free_slots++;
        }
        
        new slots_needed = (needed / max_stack) + ((needed % max_stack) > 0 ? 1 : 0);
        return free_slots >= slots_needed;
    } else {
        new free_slots = 0;
        for(new i = 0; i < MAX_INVENTORY_SLOTS; i++) {
            if(PlayerInv[playerid][i][invItemID] == -1)
                free_slots++;
        }
        return free_slots >= amount;
    }
}

stock GivePlayerItem(playerid, itemid, amount)
{
    if(!IsPlayerConnected(playerid) || itemid <= 0 || amount <= 0) return 0;
    if(!HasInventorySpace(playerid, itemid, amount)) {
        SendError(playerid, "������������ �����!");
        return 0;
    }
    
    new max_stack = GetItemMaxStack(itemid);
    
    if(IsItemStackable(itemid)) {
        for(new i = 0; i < MAX_INVENTORY_SLOTS && amount > 0; i++) {
            if(PlayerInv[playerid][i][invItemID] == itemid) {
                new space = max_stack - PlayerInv[playerid][i][invAmount];
                if(space > 0) {
                    new to_add = (amount > space) ? space : amount;
                    PlayerInv[playerid][i][invAmount] += to_add;
                    amount -= to_add;
                }
            }
        }
        
        while(amount > 0) {
            new slot = FindFreeSlot(playerid);
            if(slot == -1) break;
            
            new to_add = (amount > max_stack) ? max_stack : amount;
            PlayerInv[playerid][slot][invItemID] = itemid;
            PlayerInv[playerid][slot][invType] = 3;
            PlayerInv[playerid][slot][invAmount] = to_add;
            amount -= to_add;
        }
    } else {
        for(new i = 0; i < amount; i++) {
            new slot = FindFreeSlot(playerid);
            if(slot == -1) break;
            
            PlayerInv[playerid][slot][invItemID] = itemid;
            PlayerInv[playerid][slot][invType] = 3;
            PlayerInv[playerid][slot][invAmount] = 1;
        }
    }
    
    SavePlayerInventory(playerid);
    return 1;
}

stock RemovePlayerItem(playerid, itemid, amount)
{
    if(!IsPlayerConnected(playerid) || amount <= 0) return 0;
    if(GetPlayerItemCount(playerid, itemid) < amount) return 0;
    
    for(new i = 0; i < MAX_INVENTORY_SLOTS && amount > 0; i++) {
        if(PlayerInv[playerid][i][invItemID] == itemid) {
            if(PlayerInv[playerid][i][invAmount] <= amount) {
                amount -= PlayerInv[playerid][i][invAmount];
                PlayerInv[playerid][i][invItemID] = -1;
                PlayerInv[playerid][i][invType] = 0;
                PlayerInv[playerid][i][invAmount] = 0;
                PlayerInv[playerid][i][invAmmo] = 0;
            } else {
                PlayerInv[playerid][i][invAmount] -= amount;
                amount = 0;
            }
        }
    }
    
    SavePlayerInventory(playerid);
    return 1;
}

stock ClearPlayerInventory(playerid)
{
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++) {
        PlayerInv[playerid][i][invItemID] = -1;
        PlayerInv[playerid][i][invType] = 0;
        PlayerInv[playerid][i][invAmount] = 0;
        PlayerInv[playerid][i][invAmmo] = 0;
    }
    return 1;
}

stock SavePlayerInventory(playerid)
{
    if(pInfo[playerid][pID] <= 0) return 0;
    
    mysql_format(connection, query, sizeof(query),
        "DELETE FROM `player_inventory` WHERE `player_id` = %d", pInfo[playerid][pID]);
    mysql_tquery(connection, query);
    
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++) {
        if(PlayerInv[playerid][i][invItemID] > 0 && PlayerInv[playerid][i][invAmount] > 0) {
            mysql_format(connection, query, sizeof(query),
                "INSERT INTO `player_inventory` (`player_id`, `slot`, `item_id`, `amount`, `ammo`) VALUES (%d, %d, %d, %d, %d)",
                pInfo[playerid][pID], i, PlayerInv[playerid][i][invItemID],
                PlayerInv[playerid][i][invAmount], PlayerInv[playerid][i][invAmmo]);
            mysql_tquery(connection, query);
        }
    }
    return 1;
}

stock LoadPlayerInventory(playerid)
{
    ClearPlayerInventory(playerid);
    mysql_format(connection, query, sizeof(query),
        "SELECT * FROM `player_inventory` WHERE `player_id` = %d ORDER BY `slot` ASC",
        pInfo[playerid][pID]);
    mysql_tquery(connection, query, "OnPlayerInventoryLoaded", "d", playerid);
    return 1;
}

stock ShowPlayerInventory(playerid)
{
    new string[2048], item_name[32], count = 0;
    
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++) {
        if(PlayerInv[playerid][i][invItemID] > 0 && PlayerInv[playerid][i][invAmount] > 0) {
            GetItemName(PlayerInv[playerid][i][invItemID], item_name, sizeof(item_name));
            format(string, sizeof(string), "%s{33CC33}[%d] {FFFFFF}%s x%d\n",
                string, ++count, item_name, PlayerInv[playerid][i][invAmount]);
        }
    }
    
    if(count == 0) {
        strcat(string, "{808080}��� ���������", sizeof(string));
        ShowPlayerDialog(playerid, DIALOG_INV_MAIN, DIALOG_STYLE_LIST,
            "{FF0000}���������", string, "�������", "");
    } else {
        strcat(string, "\n{FF8000}�������� �������:", sizeof(string));
        ShowPlayerDialog(playerid, DIALOG_INV_MAIN, DIALOG_STYLE_LIST,
            "{FF0000}���������", string, "�������", "�������");
    }
    return 1;
}

stock GetInventorySlotByList(playerid, listitem)
{
    new count = 0;
    for(new i = 0; i < MAX_INVENTORY_SLOTS; i++) {
        if(PlayerInv[playerid][i][invItemID] > 0 && PlayerInv[playerid][i][invAmount] > 0) {
            if(count == listitem) return i;
            count++;
        }
    }
    return -1;
}
