/*
 * IMPERIAL ROLE PLAY - Systems\Inventory\inventory.inc
 * Callback-функции загрузки предметов и инвентарей игроков.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// ==================== ��������� ������� ======================= //
// =============================================================== //

// -------------------- �������� ��������� --------------------
forward OnItemsLoaded();
public OnItemsLoaded()
{
    TotalItems = cache_num_rows();
    if(TotalItems > MAX_ITEM_TYPES) TotalItems = MAX_ITEM_TYPES;
    
    for(new i = 0; i < TotalItems; i++) {
        cache_get_value_int(i, "id", Items[i][itemID]);
        cache_get_value(i, "name", Items[i][itemName], 32);
        cache_get_value_int(i, "type", Items[i][itemType]);
        cache_get_value_int(i, "max_stack", Items[i][itemMaxStack]);
    }
    return 1;
}

// -------------------- �������� ��������� ������ --------------------
forward OnPlayerInventoryLoaded(playerid);
public OnPlayerInventoryLoaded(playerid)
{
    new rows = cache_num_rows();
    if(rows) {
        new slot;
        for(new i = 0; i < rows; i++) {
            cache_get_value_int(i, "slot", slot);
            if(slot >= 0 && slot < MAX_INVENTORY_SLOTS) {
                cache_get_value_int(i, "item_id", PlayerInv[playerid][slot][invItemID]);
                cache_get_value_int(i, "amount", PlayerInv[playerid][slot][invAmount]);
                cache_get_value_int(i, "ammo", PlayerInv[playerid][slot][invAmmo]);
                PlayerInv[playerid][slot][invType] = 3;
            }
        }
    }
    return 1;
}

// =============================================================== //
