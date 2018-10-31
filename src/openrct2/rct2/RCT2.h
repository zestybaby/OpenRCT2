/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../object/Object.h"
#include "../rct12/RCT12.h"
#include "../ride/RideRatings.h"
#include "../ride/Vehicle.h"
#include "../world/Banner.h"
#include "../world/Location.hpp"
#include "../world/MapAnimation.h"
#include "../world/Sprite.h"

#define RCT2_MAX_STAFF 200
#define RCT2_MAX_BANNERS_IN_PARK 250
#define RCT2_MAX_VEHICLES_PER_RIDE 32
#define RCT2_MAX_CARS_PER_TRAIN 32
#define RCT2_MAX_CATEGORIES_PER_RIDE 2
#define RCT2_MAX_RIDE_TYPES_PER_RIDE_ENTRY 3
#define RCT2_MAX_VEHICLES_PER_RIDE_ENTRY 4
#define RCT2_DOWNTIME_HISTORY_SIZE 8
#define RCT2_CUSTOMER_HISTORY_SIZE 10
#define RCT2_MAX_SPRITES 10000
#define RCT2_MAX_TILE_ELEMENTS 0x30000
#define RCT2_MAX_ANIMATED_OBJECTS 2000
#define RCT2_MAX_RESEARCHED_RIDE_TYPE_QUADS 8  // With 32 bits per uint32_t, this means there is room for 256 types.
#define RCT2_MAX_RESEARCHED_RIDE_ENTRY_QUADS 8 // With 32 bits per uint32_t, this means there is room for 256 entries.
#define RCT2_MAX_RESEARCHED_SCENERY_ITEM_QUADS 56
#define RCT2_MAX_RESEARCHED_SCENERY_ITEMS (RCT2_MAX_RESEARCHED_SCENERY_ITEM_QUADS * 32) // There are 32 bits per quad.
#define RCT2_MAX_RESEARCH_ITEMS 500
#define RCT2_RIDE_MEASUREMENT_MAX_ITEMS 4800

struct rct2_install_info
{
    uint32_t installLevel;
    char title[260];
    char path[260];
    uint32_t var_20C;
    uint8_t pad_210[256];
    char expansionPackNames[16][128];
    uint32_t activeExpansionPacks; // 0xB10
};

#pragma pack(push, 1)

/**
 * Ride measurement structure.
 * size: 0x04B0C
 */
struct rct2_ride_measurement
{
    uint8_t ride_index;                                // 0x0000
    uint8_t flags;                                     // 0x0001
    uint32_t last_use_tick;                            // 0x0002
    uint16_t num_items;                                // 0x0006
    uint16_t current_item;                             // 0x0008
    uint8_t vehicle_index;                             // 0x000A
    uint8_t current_station;                           // 0x000B
    int8_t vertical[RCT2_RIDE_MEASUREMENT_MAX_ITEMS];  // 0x000C
    int8_t lateral[RCT2_RIDE_MEASUREMENT_MAX_ITEMS];   // 0x12CC
    uint8_t velocity[RCT2_RIDE_MEASUREMENT_MAX_ITEMS]; // 0x258C
    uint8_t altitude[RCT2_RIDE_MEASUREMENT_MAX_ITEMS]; // 0x384C
};
assert_struct_size(rct2_ride_measurement, 0x4b0c);

struct rct2_research_item
{
    // Bit 16 (0: scenery entry, 1: ride entry)
    union
    {
        int32_t rawValue;
        struct
        {
            uint8_t entryIndex;
            uint8_t baseRideType;
            uint8_t type; // 0: scenery entry, 1: ride entry
            uint8_t flags;
        };
    };
    uint8_t category;

    bool IsInventedEndMarker() const;
    bool IsRandomEndMarker() const;
    bool IsUninventedEndMarker() const;
};
assert_struct_size(rct2_research_item, 5);

/**
 * Ride structure.
 * size: 0x0260
 */
struct rct2_ride
{
    uint8_t type; // 0x000
    // pointer to static info. for example, wild mouse type is 0x36, subtype is
    // 0x4c.
    uint8_t subtype;                                             // 0x001
    uint16_t pad_002;                                            // 0x002
    uint8_t mode;                                                // 0x004
    uint8_t colour_scheme_type;                                  // 0x005
    rct_vehicle_colour vehicle_colours[RCT2_MAX_CARS_PER_TRAIN]; // 0x006
    uint8_t pad_046[0x03];                                       // 0x046, Used to be track colours in RCT1 without expansions
    // 0 = closed, 1 = open, 2 = test
    uint8_t status;     // 0x049
    rct_string_id name; // 0x04A
    union
    {
        uint32_t name_arguments; // 0x04C
        struct
        {
            rct_string_id name_arguments_type_name; // 0x04C
            uint16_t name_arguments_number;         // 0x04E
        };
    };
    LocationXY8 overall_view;                                // 0x050
    LocationXY8 station_starts[RCT12_MAX_STATIONS_PER_RIDE]; // 0x052
    uint8_t station_heights[RCT12_MAX_STATIONS_PER_RIDE];    // 0x05A
    uint8_t station_length[RCT12_MAX_STATIONS_PER_RIDE];     // 0x05E
    uint8_t station_depart[RCT12_MAX_STATIONS_PER_RIDE];     // 0x062
    // ride->vehicle index for current train waiting for passengers
    // at station
    uint8_t train_at_station[RCT12_MAX_STATIONS_PER_RIDE];    // 0x066
    LocationXY8 entrances[RCT12_MAX_STATIONS_PER_RIDE];       // 0x06A
    LocationXY8 exits[RCT12_MAX_STATIONS_PER_RIDE];           // 0x072
    uint16_t last_peep_in_queue[RCT12_MAX_STATIONS_PER_RIDE]; // 0x07A
    uint8_t pad_082[RCT12_MAX_STATIONS_PER_RIDE];  // 0x082, Used to be number of peeps in queue in RCT1, but this has moved.
    uint16_t vehicles[RCT2_MAX_VEHICLES_PER_RIDE]; // 0x086, Points to the first car in the train
    uint8_t depart_flags;                          // 0x0C6

    // Not sure if these should be uint or sint.
    uint8_t num_stations;                // 0x0C7
    uint8_t num_vehicles;                // 0x0C8
    uint8_t num_cars_per_train;          // 0x0C9
    uint8_t proposed_num_vehicles;       // 0x0CA
    uint8_t proposed_num_cars_per_train; // 0x0CB
    uint8_t max_trains;                  // 0x0CC
    uint8_t min_max_cars_per_train;      // 0x0CD
    uint8_t min_waiting_time;            // 0x0CE
    uint8_t max_waiting_time;            // 0x0CF
    union
    {
        uint8_t operation_option; // 0x0D0
        uint8_t time_limit;       // 0x0D0
        uint8_t num_laps;         // 0x0D0
        uint8_t launch_speed;     // 0x0D0
        uint8_t speed;            // 0x0D0
        uint8_t rotations;        // 0x0D0
    };

    uint8_t boat_hire_return_direction;    // 0x0D1
    LocationXY8 boat_hire_return_position; // 0x0D2
    uint8_t measurement_index;             // 0x0D4
    // bits 0 through 4 are the number of helix sections
    // bit 5: spinning tunnel, water splash, or rapids
    // bit 6: log reverser, waterfall
    // bit 7: whirlpool
    uint8_t special_track_elements; // 0x0D5
    uint8_t pad_0D6[2];             // 0x0D6
    // Divide this value by 29127 to get the human-readable max speed
    // (in RCT2, display_speed = (max_speed * 9) >> 18)
    int32_t max_speed;                           // 0x0D8
    int32_t average_speed;                       // 0x0DC
    uint8_t current_test_segment;                // 0x0E0
    uint8_t average_speed_test_timeout;          // 0x0E1
    uint8_t pad_0E2[0x2];                        // 0x0E2
    int32_t length[RCT12_MAX_STATIONS_PER_RIDE]; // 0x0E4
    uint16_t time[RCT12_MAX_STATIONS_PER_RIDE];  // 0x0F4
    fixed16_2dp max_positive_vertical_g;         // 0x0FC
    fixed16_2dp max_negative_vertical_g;         // 0x0FE
    fixed16_2dp max_lateral_g;                   // 0x100
    fixed16_2dp previous_vertical_g;             // 0x102
    fixed16_2dp previous_lateral_g;              // 0x104
    uint8_t pad_106[0x2];                        // 0x106
    uint32_t testing_flags;                      // 0x108
    // x y map location of the current track piece during a test
    // this is to prevent counting special tracks multiple times
    LocationXY8 cur_test_track_location; // 0x10C
    // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
    uint16_t turn_count_default; // 0x10E X = current turn count
    uint16_t turn_count_banked;  // 0x110
    uint16_t turn_count_sloped;  // 0x112 X = number turns > 3 elements
    union
    {
        uint8_t inversions; // 0x114 (???X XXXX)
        uint8_t holes;      // 0x114 (???X XXXX)
        // This is a very rough approximation of how much of the ride is undercover.
        // It reaches the maximum value of 7 at about 50% undercover and doesn't increase beyond that.
        uint8_t sheltered_eighths; // 0x114 (XXX?-????)
    };
    // Y is number of powered lifts, X is drops
    uint8_t drops;               // 0x115 (YYXX XXXX)
    uint8_t start_drop_height;   // 0x116
    uint8_t highest_drop_height; // 0x117
    int32_t sheltered_length;    // 0x118
    // Unused always 0? Should affect nausea
    uint16_t var_11C;               // 0x11C
    uint8_t num_sheltered_sections; // 0x11E (?abY YYYY)
    // see cur_test_track_location
    uint8_t cur_test_track_z; // 0x11F
    // Customer counter in the current 960 game tick (about 30 seconds) interval
    uint16_t cur_num_customers; // 0x120
    // Counts ticks to update customer intervals, resets each 960 game ticks.
    uint16_t num_customers_timeout; // 0x122
    // Customer count in the last 10 * 960 game ticks (sliding window)
    uint16_t num_customers[RCT2_CUSTOMER_HISTORY_SIZE]; // 0x124
    money16 price;                                      // 0x138
    LocationXY8 chairlift_bullwheel_location[2];        // 0x13A
    uint8_t chairlift_bullwheel_z[2];                   // 0x13E
    union
    {
        rating_tuple ratings; // 0x140
        struct
        {
            ride_rating excitement; // 0x140
            ride_rating intensity;  // 0x142
            ride_rating nausea;     // 0x144
        };
    };
    uint16_t value;                        // 0x146
    uint16_t chairlift_bullwheel_rotation; // 0x148
    uint8_t satisfaction;                  // 0x14A
    uint8_t satisfaction_time_out;         // 0x14B
    uint8_t satisfaction_next;             // 0x14C
    // Various flags stating whether a window needs to be refreshed
    uint8_t window_invalidate_flags; // 0x14D
    uint8_t pad_14E[0x02];           // 0x14E
    uint32_t total_customers;        // 0x150
    money32 total_profit;            // 0x154
    uint8_t popularity;              // 0x158
    uint8_t popularity_time_out;     // 0x159 Updated every purchase and ?possibly by time?
    uint8_t popularity_next;         // 0x15A When timeout reached this will be the next popularity
    uint8_t num_riders;              // 0x15B
    uint8_t music_tune_id;           // 0x15C
    uint8_t slide_in_use;            // 0x15D
    union
    {
        uint16_t slide_peep; // 0x15E
        uint16_t maze_tiles; // 0x15E
    };
    uint8_t pad_160[0xE];              // 0x160
    uint8_t slide_peep_t_shirt_colour; // 0x16E
    uint8_t pad_16F[0x7];              // 0x16F
    uint8_t spiral_slide_progress;     // 0x176
    uint8_t pad_177[0x9];              // 0x177
    int16_t build_date;                // 0x180
    money16 upkeep_cost;               // 0x182
    uint16_t race_winner;              // 0x184
    uint8_t pad_186[0x02];             // 0x186
    uint32_t music_position;           // 0x188
    uint8_t breakdown_reason_pending;  // 0x18C
    uint8_t mechanic_status;           // 0x18D
    uint16_t mechanic;                 // 0x18E
    uint8_t inspection_station;        // 0x190
    uint8_t broken_vehicle;            // 0x191
    uint8_t broken_car;                // 0x192
    uint8_t breakdown_reason;          // 0x193
    money16 price_secondary;           // 0x194
    union
    {
        struct
        {
            uint8_t reliability_subvalue;   // 0x196, 0 - 255, acts like the decimals for reliability_percentage
            uint8_t reliability_percentage; // 0x197, Starts at 100 and decreases from there.
        };
        uint16_t reliability; // 0x196
    };
    // Small constant used to increase the unreliability as the game continues,
    // making breakdowns more and more likely.
    uint8_t unreliability_factor; // 0x198
    // Range from [0, 100]
    uint8_t downtime;                                     // 0x199
    uint8_t inspection_interval;                          // 0x19A
    uint8_t last_inspection;                              // 0x19B
    uint8_t downtime_history[RCT2_DOWNTIME_HISTORY_SIZE]; // 0x19C
    uint32_t no_primary_items_sold;                       // 0x1A4
    uint32_t no_secondary_items_sold;                     // 0x1A8
    uint8_t breakdown_sound_modifier;                     // 0x1AC
    // Used to oscillate the sound when ride breaks down.
    // 0 = no change, 255 = max change
    uint8_t not_fixed_timeout;                                 // 0x1AD
    uint8_t last_crash_type;                                   // 0x1AE
    uint8_t connected_message_throttle;                        // 0x1AF
    money32 income_per_hour;                                   // 0x1B0
    money32 profit;                                            // 0x1B4
    uint8_t queue_time[RCT12_MAX_STATIONS_PER_RIDE];           // 0x1B8
    uint8_t track_colour_main[RCT12_NUM_COLOUR_SCHEMES];       // 0x1BC
    uint8_t track_colour_additional[RCT12_NUM_COLOUR_SCHEMES]; // 0x1C0
    uint8_t track_colour_supports[RCT12_NUM_COLOUR_SCHEMES];   // 0x1C4
    uint8_t music;                                             // 0x1C8
    uint8_t entrance_style;                                    // 0x1C9
    uint16_t vehicle_change_timeout;                           // 0x1CA
    uint8_t num_block_brakes;                                  // 0x1CC
    uint8_t lift_hill_speed;                                   // 0x1CD
    uint16_t guests_favourite;                                 // 0x1CE
    uint32_t lifecycle_flags;                                  // 0x1D0
    uint8_t vehicle_colours_extended[RCT2_MAX_CARS_PER_TRAIN]; // 0x1D4
    uint16_t total_air_time;                                   // 0x1F4
    uint8_t current_test_station;                              // 0x1F6
    uint8_t num_circuits;                                      // 0x1F7
    int16_t cable_lift_x;                                      // 0x1F8
    int16_t cable_lift_y;                                      // 0x1FA
    uint8_t cable_lift_z;                                      // 0x1FC
    uint8_t pad_1FD;                                           // 0x1FD
    uint16_t cable_lift;                                       // 0x1FE
    uint16_t queue_length[RCT12_MAX_STATIONS_PER_RIDE];        // 0x200
    uint8_t pad_208[0x58];                                     // 0x208
};
assert_struct_size(rct2_ride, 0x260);

/**
 * scores.dat file header.
 * size: 0x10
 */
struct rct_scores_header
{
    uint32_t var_0;
    uint32_t var_4;
    uint32_t var_8;
    uint32_t ScenarioCount;
};
assert_struct_size(rct_scores_header, 0x10);

/**
 * An entry of scores.dat
 * size: 0x02B0
 */
struct rct_scores_entry
{
    char Path[256];
    uint8_t Category;
    uint8_t pad_0101[0x1F];
    int8_t ObjectiveType;
    int8_t ObjectiveArg1;
    int32_t objectiveArg2;
    int16_t objectiveArg3;
    char Name[64];
    char Details[256];
    int32_t Flags;
    money32 CompanyValue;
    char CompletedBy[64];
};
assert_struct_size(rct_scores_entry, 0x02B0);

/**
 * SV6/SC6 header chunk
 * size: 0x20
 */
struct rct_s6_header
{
    uint8_t type;                // 0x00
    uint8_t classic_flag;        // 0x01
    uint16_t num_packed_objects; // 0x02
    uint32_t version;            // 0x04
    uint32_t magic_number;       // 0x08
    uint8_t pad_0C[0x14];
};
assert_struct_size(rct_s6_header, 0x20);

/**
 * SC6 information chunk
 * size: 0x198
 */
struct rct_s6_info
{
    uint8_t editor_step;
    uint8_t category;        // 0x01
    uint8_t objective_type;  // 0x02
    uint8_t objective_arg_1; // 0x03
    int32_t objective_arg_2; // 0x04
    int16_t objective_arg_3; // 0x08
    uint8_t pad_00A[0x3E];
    char name[64];          // 0x48
    char details[256];      // 0x88
    rct_object_entry entry; // 0x188
};
assert_struct_size(rct_s6_info, 0x198);

struct rct_s6_data
{
    // SC6[0]
    rct_s6_header header;

    // SC6[1]
    rct_s6_info info;

    // SC6[2]
    // packed objects

    // SC6[3]
    rct_object_entry objects[OBJECT_ENTRY_COUNT];

    // SC6[4]
    uint16_t elapsed_months;
    uint16_t current_day;
    uint32_t scenario_ticks;
    uint32_t scenario_srand_0;
    uint32_t scenario_srand_1;

    // SC6[5]
    RCT12TileElement tile_elements[RCT2_MAX_TILE_ELEMENTS];

    // SC6[6]
    uint32_t next_free_tile_element_pointer_index;
    rct_sprite sprites[RCT2_MAX_SPRITES];
    uint16_t sprite_lists_head[6];
    uint16_t sprite_lists_count[6];
    rct_string_id park_name;
    uint8_t pad_013573D6[2];
    uint32_t park_name_args;
    money32 initial_cash;
    money32 current_loan;
    uint32_t park_flags;
    money16 park_entrance_fee;
    uint16_t rct1_park_entrance_x;
    uint16_t rct1_park_entrance_y;
    uint8_t pad_013573EE[2];
    uint8_t rct1_park_entrance_z;
    uint8_t pad_013573F1;
    rct12_peep_spawn peep_spawns[RCT12_MAX_PEEP_SPAWNS];
    uint8_t guest_count_change_modifier;
    uint8_t current_research_level;
    uint8_t pad_01357400[4];
    uint32_t researched_ride_types[RCT2_MAX_RESEARCHED_RIDE_TYPE_QUADS];
    uint32_t researched_ride_entries[RCT2_MAX_RESEARCHED_RIDE_ENTRY_QUADS];
    uint32_t researched_track_types_a[128];
    uint32_t researched_track_types_b[128];

    // SC6[7]
    uint16_t guests_in_park;
    uint16_t guests_heading_for_park;

    // Ignored in scenario
    money32 expenditure_table[RCT12_EXPENDITURE_TABLE_MONTH_COUNT][RCT12_EXPENDITURE_TYPE_COUNT];

    // SC6[8]
    uint16_t last_guests_in_park;
    uint8_t pad_01357BCA[3];
    uint8_t handyman_colour;
    uint8_t mechanic_colour;
    uint8_t security_colour;

    // Ignored in scenario
    uint32_t researched_scenery_items[RCT2_MAX_RESEARCHED_SCENERY_ITEM_QUADS];

    // SC6[9]
    uint16_t park_rating;

    // Ignored in scenario
    uint8_t park_rating_history[32];
    uint8_t guests_in_park_history[32];

    // SC6[10]
    uint8_t active_research_types;
    uint8_t research_progress_stage;
    uint32_t last_researched_item_subject;
    uint8_t pad_01357CF8[1000];
    uint32_t next_research_item;
    uint16_t research_progress;
    uint8_t next_research_category;
    uint8_t next_research_expected_day;
    uint8_t next_research_expected_month;
    uint8_t guest_initial_happiness;
    uint16_t park_size;
    uint16_t guest_generation_probability;
    uint16_t total_ride_value_for_money;
    money32 maximum_loan;
    money16 guest_initial_cash;
    uint8_t guest_initial_hunger;
    uint8_t guest_initial_thirst;
    uint8_t objective_type;
    uint8_t objective_year;
    uint8_t pad_013580FA[2];
    money32 objective_currency;
    uint16_t objective_guests;
    uint8_t campaign_weeks_left[20];
    uint8_t campaign_ride_index[22];

    // Ignored in scenario
    money32 balance_history[RCT12_FINANCE_GRAPH_SIZE];

    // SC6[11]
    money32 current_expenditure;
    money32 current_profit;
    uint32_t weekly_profit_average_dividend;
    uint16_t weekly_profit_average_divisor;
    uint8_t pad_0135833A[2];

    // Ignored in scenario
    money32 weekly_profit_history[RCT12_FINANCE_GRAPH_SIZE];

    // SC6[12]
    money32 park_value;

    // Ignored in scenario
    money32 park_value_history[RCT12_FINANCE_GRAPH_SIZE];

    // SC6[13]
    money32 completed_company_value;
    uint32_t total_admissions;
    money32 income_from_admissions;
    money32 company_value;
    uint8_t peep_warning_throttle[16];
    rct12_award awards[RCT12_MAX_AWARDS];
    money16 land_price;
    money16 construction_rights_price;
    uint16_t word_01358774;
    uint8_t pad_01358776[2];
    uint32_t cd_key;
    uint8_t pad_0135877C[64];
    uint32_t game_version_number;
    money32 completed_company_value_record;
    uint32_t loan_hash;
    uint16_t ride_count;
    uint8_t pad_013587CA[6];
    money32 historical_profit;
    uint8_t pad_013587D4[4];
    char scenario_completed_name[32];
    money32 cash;
    uint8_t pad_013587FC[50];
    uint16_t park_rating_casualty_penalty;
    uint16_t map_size_units;
    uint16_t map_size_minus_2;
    uint16_t map_size;
    uint16_t map_max_xy;
    uint32_t same_price_throughout;
    uint16_t suggested_max_guests;
    uint16_t park_rating_warning_days;
    uint8_t last_entrance_style;
    uint8_t rct1_water_colour;
    uint8_t pad_01358842[2];
    rct2_research_item research_items[RCT2_MAX_RESEARCH_ITEMS];
    uint16_t map_base_z;
    char scenario_name[64];
    char scenario_description[256];
    uint8_t current_interest_rate;
    uint8_t pad_0135934B;
    uint32_t same_price_throughout_extended;
    int16_t park_entrance_x[RCT12_MAX_PARK_ENTRANCES];
    int16_t park_entrance_y[RCT12_MAX_PARK_ENTRANCES];
    int16_t park_entrance_z[RCT12_MAX_PARK_ENTRANCES];
    uint8_t park_entrance_direction[RCT12_MAX_PARK_ENTRANCES];
    char scenario_filename[256];
    uint8_t saved_expansion_pack_names[3256];
    rct_banner banners[RCT2_MAX_BANNERS_IN_PARK];
    char custom_strings[RCT12_MAX_USER_STRINGS][RCT12_USER_STRING_MAX_LENGTH];
    uint32_t game_ticks_1;
    rct2_ride rides[RCT12_MAX_RIDES_IN_PARK];
    uint16_t saved_age;
    uint16_t saved_view_x;
    uint16_t saved_view_y;
    uint8_t saved_view_zoom;
    uint8_t saved_view_rotation;
    rct_map_animation map_animations[RCT2_MAX_ANIMATED_OBJECTS];
    uint16_t num_map_animations;
    uint8_t pad_0138B582[2];
    rct_ride_rating_calc_data ride_ratings_calc_data;
    uint8_t pad_0138B5D0[60];
    rct2_ride_measurement ride_measurements[8];
    uint32_t next_guest_index;
    uint16_t grass_and_scenery_tilepos;
    uint32_t patrol_areas[(RCT2_MAX_STAFF + RCT12_STAFF_TYPE_COUNT) * RCT12_PATROL_AREA_SIZE];
    uint8_t staff_modes[RCT2_MAX_STAFF + RCT12_STAFF_TYPE_COUNT];
    uint8_t pad_13CA73E;
    uint8_t pad_13CA73F;
    uint8_t byte_13CA740;
    uint8_t pad_13CA741;
    uint8_t byte_13CA742[4]; // unused
    uint8_t climate;
    uint8_t pad_013CA747;
    uint16_t climate_update_timer;
    uint8_t current_weather;
    uint8_t next_weather;
    uint8_t temperature;
    uint8_t next_temperature;
    uint8_t current_weather_effect;
    uint8_t next_weather_effect;
    uint8_t current_weather_gloom;
    uint8_t next_weather_gloom;
    uint8_t current_rain_level;
    uint8_t next_rain_level;
    rct12_news_item news_items[RCT12_MAX_NEWS_ITEMS];
    char rct1_scenario_name[62];       // Unused in RCT2
    uint16_t rct1_scenario_slot_index; // Unused in RCT2
    uint32_t rct1_scenario_flags;      // Unused in RCT2
    uint16_t wide_path_tile_loop_x;
    uint16_t wide_path_tile_loop_y;
    uint8_t pad_13CE778[434];
};
assert_struct_size(rct_s6_data, 0x46b44a);

#pragma pack(pop)
