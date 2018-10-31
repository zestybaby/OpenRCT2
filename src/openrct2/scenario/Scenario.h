/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _SCENARIO_H_
#define _SCENARIO_H_

#include "../common.h"
#include "../management/Finance.h"
#include "../management/Research.h"
#include "../object/Object.h"
#include "../rct12/RCT12.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideRatings.h"
#include "../world/Banner.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Sprite.h"

struct ParkLoadResult;

#pragma pack(push, 1)

enum SCENARIO_SOURCE
{
    SCENARIO_SOURCE_RCT1,
    SCENARIO_SOURCE_RCT1_AA,
    SCENARIO_SOURCE_RCT1_LL,
    SCENARIO_SOURCE_RCT2,
    SCENARIO_SOURCE_RCT2_WW,
    SCENARIO_SOURCE_RCT2_TT,
    SCENARIO_SOURCE_REAL,
    SCENARIO_SOURCE_OTHER,
};

struct rct_stex_entry
{
    rct_string_id scenario_name; // 0x00
    rct_string_id park_name;     // 0x02
    rct_string_id details;       // 0x04
    uint8_t var_06;
};
assert_struct_size(rct_stex_entry, 7);


#pragma pack(pop)

enum
{
    SCENARIO_FLAGS_VISIBLE = (1 << 0),
    SCENARIO_FLAGS_COMPLETED = (1 << 1),
    SCENARIO_FLAGS_SIXFLAGS = (1 << 2)
};

enum
{
    S6_TYPE_SAVEDGAME,
    S6_TYPE_SCENARIO
};

#define S6_RCT2_VERSION 120001
#define S6_MAGIC_NUMBER 0x00031144

enum
{
    // RCT2 categories (keep order)
    SCENARIO_CATEGORY_BEGINNER,
    SCENARIO_CATEGORY_CHALLENGING,
    SCENARIO_CATEGORY_EXPERT,
    SCENARIO_CATEGORY_REAL,
    SCENARIO_CATEGORY_OTHER,

    // OpenRCT2 categories
    SCENARIO_CATEGORY_DLC,
    SCENARIO_CATEGORY_BUILD_YOUR_OWN,

    SCENARIO_CATEGORY_COUNT
};

enum
{
    OBJECTIVE_NONE,
    OBJECTIVE_GUESTS_BY,
    OBJECTIVE_PARK_VALUE_BY,
    OBJECTIVE_HAVE_FUN,
    OBJECTIVE_BUILD_THE_BEST,
    OBJECTIVE_10_ROLLERCOASTERS,
    OBJECTIVE_GUESTS_AND_RATING,
    OBJECTIVE_MONTHLY_RIDE_INCOME,
    OBJECTIVE_10_ROLLERCOASTERS_LENGTH,
    OBJECTIVE_FINISH_5_ROLLERCOASTERS,
    OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE,
    OBJECTIVE_MONTHLY_FOOD_INCOME
};

enum
{
    SCENARIO_SELECT_MODE_DIFFICULTY,
    SCENARIO_SELECT_MODE_ORIGIN,
};

enum
{
    AUTOSAVE_EVERY_MINUTE,
    AUTOSAVE_EVERY_5MINUTES,
    AUTOSAVE_EVERY_15MINUTES,
    AUTOSAVE_EVERY_30MINUTES,
    AUTOSAVE_EVERY_HOUR,
    AUTOSAVE_NEVER
};

#define AUTOSAVE_PAUSE 0

extern const rct_string_id ScenarioCategoryStringIds[SCENARIO_CATEGORY_COUNT];

extern uint32_t gScenarioTicks;
extern uint32_t gScenarioSrand0;
extern uint32_t gScenarioSrand1;

extern uint8_t gScenarioObjectiveType;
extern uint8_t gScenarioObjectiveYear;
extern uint16_t gScenarioObjectiveNumGuests;
extern money32 gScenarioObjectiveCurrency;

extern uint16_t gScenarioParkRatingWarningDays;
extern money32 gScenarioCompletedCompanyValue;
extern money32 gScenarioCompanyValueRecord;

extern rct_s6_info gS6Info;
extern char gScenarioName[64];
extern char gScenarioDetails[256];
extern char gScenarioCompletedBy[32];
extern char gScenarioSavePath[260];
extern char gScenarioExpansionPacks[3256];
extern bool gFirstTimeSaving;
extern uint16_t gSavedAge;
extern uint32_t gLastAutoSaveUpdate;

extern char gScenarioFileName[260];

void load_from_sc6(const char* path);
void scenario_begin();
void scenario_update();

#ifdef DEBUG_DESYNC
uint32_t dbg_scenario_rand(const char* file, const char* function, const uint32_t line, const void* data);
#    define scenario_rand() dbg_scenario_rand(__FILE__, __FUNCTION__, __LINE__, NULL)
#    define scenario_rand_data(data) dbg_scenario_rand(__FILE__, __FUNCTION__, __LINE__, data)
void dbg_report_desync(uint32_t tick, uint32_t srand0, uint32_t server_srand0, const char* clientHash, const char* serverHash);
#else
uint32_t scenario_rand();
#endif

uint32_t scenario_rand_max(uint32_t max);

bool scenario_prepare_for_save();
int32_t scenario_save(const utf8* path, int32_t flags);
void scenario_remove_trackless_rides(rct_s6_data* s6);
void scenario_fix_ghosts(rct_s6_data* s6);
void scenario_failure();
void scenario_success();
void scenario_success_submit_name(const char* name);
void scenario_autosave_check();

#endif
