/*
 * IMPERIAL ROLE PLAY - Core\definitions.inc
 * Общие цветовые макросы и константы.
 * Код перенесён из исходного мода без удаления игровой логики.
 */

// =============================================================== //
// ====================== ������� ������ ======================== //
// =============================================================== //
#define COLOR_ERROR      0xFF6347FF
#define COLOR_ANSWER     0xFF0000FF
#define COLOR_ADMIN      0xFF0000FF
#define COLOR_PUNUSH     0xFF6347FF
#define COLOR_SYSTEM     0xFFA500FF
#define COLOR_SUCCESS    0x33CC33FF
#define COLOR_WHITE      0xFFFFFFFF
#define COLOR_GREY       0xA9A9A9FF
#define COLOR_INFO       0xFFFF00FF
#define COLOR_HOUSE      0x00FF00FF


// =============================================================== //
// ======================== ID �������� ========================== //
// =============================================================== //
enum {
    DIALOG_NONE,
    DIALOG_REGISTER,
    DIALOG_LOGIN,
    DIALOG_GENDER,
    DIALOG_SKIN,
    DIALOG_INV_MAIN,
    DIALOG_INV_ACTION,
    DIALOG_INV_DROP,
    DIALOG_INV_GIVE,
    DIALOG_ADMIN_REGISTER,
    DIALOG_ADMIN_LOGIN,
    DIALOG_ADMIN_PANEL_LIST,
    DIALOG_ADMIN_COMMANDS,
    DIALOG_REPORT_INPUT,
    DIALOG_AREPORT_LISTS,
    DIALOG_AREPORT_ANSWER,
    DIALOG_SETTING_SERVER,
    DIALOG_SET_MULTIPLIER,
    DIALOG_INPUT_DELITE_ACOUNT,
}

// =============================================================== //
// ====================== ������ ������ ========================== //
// =============================================================== //
enum p_info {
    pID,
    pName[24],
    pPassword[255],
    pGender,
    pSkin,
    pScore,
    pExp,
    pMoney,
    pRegister,
    pIP[16],
    pLastIP[16],
    bool:pLogged,
    bool:pSpawned,
    pMute,
    pBanID,
    pBanAdmin[24],
    pBanReason[64],
    pBanDate,
    pBanExpire
}
new pInfo[MAX_PLAYERS][p_info];

#define BCRYPT_COST 10

new const MaleSkins[]   = {78, 79, 134, 135, 137, 212, 230, 239};
new const FemaleSkins[] = {77};

// =============================================================== //
// ======================= ������� AFK ========================== //
// =============================================================== //
new AFKTime[MAX_PLAYERS];
new IsAFK[MAX_PLAYERS];
new LastTick[MAX_PLAYERS];

// =============================================================== //
// ====================== ������� ������� ======================= //
// =============================================================== //
const 
    MAX_ADMIN_REPORTS = 60,
    MAX_REPORT_TEXT_LENGTH = 120;

enum @ReportInfo
{
    rep_id,
    report_player_id,
    report_target_id,
    report_text[MAX_REPORT_TEXT_LENGTH],
    report_when_wroted_text[10],
    report_when_wroted_time,
    bool:report_is_tooked,
}
new ReportInfo[MAX_ADMIN_REPORTS][@ReportInfo];
new total_admin_reports = 0;

// =============================================================== //
// =================== ������� �������������� =================== //
// =============================================================== //
#define AdminNotLoginError(%0) \
    if(AdminInfo[%0][aLogged] == false) \
        return SendAdminNotify(playerid, "����������� {FFD700}/alogin{FFFFFF} ��� �����")

enum a_info {
    aID,
    aRank,
    aPassword[255],
    bool:aLogged,
    aPrefix[32],
    aAnsReport,
    aOnlineTime,        
    aWeekDays[7],      
    aLastResetWeek,        
}
new AdminInfo[MAX_PLAYERS][a_info];

new const AdminStandartPrefix[][] = {
    {"��� ����"},
    {"���������"},
    {"��. �������������"},
    {"�������������"},
    {"��. �������������"},
    {"��. �������������"},
    {"����������"},
    {"�����������"}
};

new bool:is_gm = false;

new bool:AdminVehicle[MAX_VEHICLES];
new AdminVehicleCount = 0;

// =============================================================== //
// ====================== ������� ���������� ==================== //
// =============================================================== //
stock GetVehicleName(modelid, name[], size)
{
    new const VehicleNames[][] = {
        "Landstalker", "Bravura", "Buffalo", "Linerunner", "Perenniel",
        "Sentinel", "Dumper", "Firetruck", "Trashmaster", "Stretch",
        "Manana", "Infernus", "Voodoo", "Pony", "Mule", "Cheetah",
        "Ambulance", "Leviathan", "Moonbeam", "Esperanto", "Taxi",
        "Washington", "Bobcat", "Whoopee", "BF Injection", "Hunter",
        "Premier", "Enforcer", "Securicar", "Banshee", "Predator",
        "Bus", "Rhino", "Barracks", "Hotknife", "Trailer", "Previon",
        "Coach", "Cabbie", "Stallion", "Rumpo", "RC Bandit", "Romero",
        "Packer", "Monster", "Admiral", "Squalo", "Seasparrow", "Pizzaboy",
        "Tram", "Trailer", "Turismo", "Speeder", "Reefer", "Tropic",
        "Flatbed", "Yankee", "Caddy", "Solair", "Berkley's RC Van",
        "Skimmer", "PCJ-600", "Faggio", "Freeway", "RC Baron", "RC Raider",
        "Glendale", "Oceanic", "Sanchez", "Sparrow", "Patriot", "Quad",
        "Coastguard", "Dinghy", "Hermes", "Sabre", "Rustler", "ZR-350",
        "Walton", "Regina", "Comet", "BMX", "Burrito", "Camper",
        "Marquis", "Baggage", "Dozer", "Maverick", "News Chopper", "Rancher",
        "FBI Rancher", "Virgo", "Greenwood", "Jetmax", "Hotring", "Sandking",
        "Blista Compact", "Police Maverick", "Boxville", "Benson", "Mesa",
        "RC Goblin", "Hotring Racer A", "Hotring Racer B", "Bloodring Banger",
        "Rancher", "Super GT", "Elegant", "Journey", "Bike", "Mountain Bike",
        "Beagle", "Cropduster", "Stuntplane", "Petrol", "RC Tiger",
        "Tornado", "Fortune", "Cadrona", "FBI Truck", "Willard",
        "Forklift", "Tractor", "Combine", "Feltzer", "Remington",
        "Slamvan", "Blade", "Freight", "Streak", "Vortex", "Vincent",
        "Bullet", "Clover", "Sadler", "Firetruck", "Hustler",
        "Intruder", "Primo", "Cargobob", "Tampa", "Sunrise", "Merit",
        "Utility", "Nevada", "Yosemite", "Windsor", "Monster A", "Monster B",
        "Uranus", "Jester", "Sultan", "Stratum", "Elegy", "Raindance",
        "RC Tiger", "Flash", "Tahoma", "Savanna", "Bandito", "Freight",
        "Trailer", "Kart", "Mower", "Dune", "Sweeper", "Broadway",
        "Tornado", "AT-400", "DFT-30", "Huntley", "Stafford", "BF-400",
        "Newsvan", "Tug", "Trailer A", "Emperor", "Wayfarer", "Euros",
        "Hotdog", "Club", "Freight Trailer", "Trailer 3", "Andromada",
        "Dodo", "RC Cam", "Launch", "LSPD Car", "SFPD Car", "LVPD Car",
        "Police Ranger", "Picador", "S.W.A.T.", "Alpha", "Phoenix",
        "Glendale", "Sadler", "Luggage", "Luggage A", "Stairs", "Boxville",
        "Tiller", "Utility Trailer"
    };
    
    if(modelid >= 400 && modelid <= 611) {
        format(name, size, VehicleNames[modelid - 400]);
        return 1;
    }
    format(name, size, "����������");
    return 0;
}

// ------------------- ��������� ������� -------------------
enum e_ServerSettings {
    multiplier,           
    server_status,       
    player_online, 
}
new SettingServer[e_ServerSettings];

// =============================================================== //
// ====================== ������� ��������� ===================== //
// =============================================================== //
#define MAX_INVENTORY_SLOTS 50
#define MAX_ITEM_TYPES     100

enum ITEM_DATA {
    itemID,
    itemName[32],
    itemType,
    itemMaxStack
}

enum INV_SLOT {
    invItemID,
    invType,
    invAmount,
    invAmmo
}

new Items[MAX_ITEM_TYPES][ITEM_DATA];
new TotalItems = 0;
new PlayerInv[MAX_PLAYERS][MAX_INVENTORY_SLOTS][INV_SLOT];
new PlayerSelectedSlot[MAX_PLAYERS] = {-1, ...};

// =============================================================== //
