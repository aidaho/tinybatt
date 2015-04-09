#define MAX_BATTERY_COUNT 16  // ought to be enough for everybody
#define BATTERY_PATH "/sys/class/power_supply"  ///< path to battery subdirs
#define BATTERY_DIR_PREFIX "BAT"  // usually they look like 'BAT0', 'BAT1'...
#define BATTERY_REMAINING_CAPACITY "energy_now"
#define BATTERY_DISCHARGE_RATE "power_now"
#define BATTERY_LAST_CAPACITY "energy_full"
#define BATTERY_STATUS "status"
// We will be looking for these substrings to determine status:
#define CHARGING_STATUS "Charging"
#define DISCHARGING_STATUS "Discharging"

#define CHARGING_STRING "↻"
#define DISCHARGING_STRING "⚡"
#define WARNING_STRING "⚠"

#define CAPACITY_DIVIDER 1000
#define CAPACITY_UNIT "mWh"

#define RATE_DIVIDER 1000000
#define RATE_UNIT "mW"
#define RATE_DIVIDED_UNIT "W"

#define OUTPUT_BUFFER_LEN 32  ///< maximum lentgh of single abbrev
#define MAX_ABBREV_LEN 16  ///< maximum lentgh of single abbrev
// Ordered integer abbreviations separated by spaces
#define DRAW_ABBREVS "➀ ➁ ➂ ➃ ➄ ➅ ➆ ➇ ➈ ➉ ⑪ ⑫ ⑬ ⑭ ⑮ ⑯ ⑰ ⑱ ⑲ ⑳"
