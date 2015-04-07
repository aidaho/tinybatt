#define VERSION "0.0.1"

#define BATTERY_PATH "/sys/class/power_supply/BAT0"
#define BATTERY_REMAINING_CAPACITY "energy_now"
#define BATTERY_LAST_CAPACITY "energy_full"
#define BATTERY_STATUS "status"
// We will be looking at these substrings to determine status:
#define CHARGING_STATUS "Charging"
#define DISCHARGING_STATUS "Discharging"

#define CHARGING_STRING "⚡"
#define WARNING_STRING "⚠"

#define CAPACITY_DIVIDER 1000
#define CAPACITY_UNIT "mWh"

#define RATE_DIVIDER 1000
#define RATE_UNIT "mW"

