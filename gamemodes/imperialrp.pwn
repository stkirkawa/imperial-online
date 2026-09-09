/*
 * IMPERIAL ROLE PLAY - ТОЧКА ВХОДА МОДА
 * В этом файле находятся только зависимости компилятора и подключения модулей.
 * Объединение callback-функций выполняется через YSI y_hooks.
 */
#include <a_samp>
#include <a_mysql>
#include <bcrypt>
#include <sscanf2>
#include <foreach>
#include <Pawn.CMD>
#include <streamer>
#define AMX_OLD_CALL
#define __COMPILER_sNAMEMAX 31
#include <YSI/y_hooks>
#include "imperial/Connections/DB connect/database.inc"
#include "imperial/Core/definitions.inc"
#include "imperial/Core/lifecycle.inc"
#include "imperial/Database/callbacks.inc"
#include "imperial/Systems/Core/stocks.inc"
#include "imperial/Systems/Player/callbacks.inc"
#include "imperial/Systems/Player/commands.inc"
#include "imperial/Systems/Auth/dialogs.inc"
#include "imperial/Systems/Admin/admin_core.inc"
#include "imperial/Systems/Admin/callbacks.inc"
#include "imperial/Systems/Admin/commands.inc"
#include "imperial/Systems/Admin/stocks.inc"
#include "imperial/Systems/Inventory/dialogs.inc"
#include "imperial/Systems/Inventory/inventory.inc"
#include "imperial/Systems/Inventory/stocks.inc"
#include "imperial/Systems/Reports/dialogs.inc"
#include "imperial/Systems/Server/dialogs.inc"
