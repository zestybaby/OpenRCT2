/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/LargeScenery.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "FootpathRemoveAction.hpp"
#include "GameAction.h"
#include "LargeSceneryRemoveAction.hpp"
#include "SmallSceneryRemoveAction.hpp"
#include "WallRemoveAction.hpp"

#include <algorithm>

using namespace OpenRCT2;

using ClearableItems = uint8_t;

namespace CLEARABLE_ITEMS
{
    constexpr ClearableItems SCENERY_SMALL = 1 << 0;
    constexpr ClearableItems SCENERY_LARGE = 1 << 1;
    constexpr ClearableItems SCENERY_FOOTPATH = 1 << 2;
} // namespace CLEARABLE_ITEMS

DEFINE_GAME_ACTION(ClearAction, GAME_COMMAND_CLEAR_SCENERY, GameActionResult)
{
private:
    MapRange _range;
    ClearableItems _itemsToClear;

public:
    ClearAction()
    {
    }
    ClearAction(MapRange range, ClearableItems itemsToClear)
        : _range(range)
        , _itemsToClear(itemsToClear)
    {
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_range) << DS_TAG(_itemsToClear);
    }

    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
    GameActionResult::Ptr CreateResult() const
    {
        auto result = MakeResult();
        result->ErrorTitle = STR_UNABLE_TO_REMOVE_ALL_SCENERY_FROM_HERE;
        result->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

        auto x = (_range.GetLeft() + _range.GetRight()) / 2 + 16;
        auto y = (_range.GetTop() + _range.GetBottom()) / 2 + 16;
        auto z = tile_element_height(x, y);
        result->Position = CoordsXYZ(x, y, z);

        return result;
    }

    GameActionResult::Ptr QueryExecute(bool executing) const
    {
        auto result = CreateResult();

        auto noValidTiles = true;
        auto error = GA_ERROR::OK;
        rct_string_id errorMessage = STR_NONE;
        money32 totalCost = 0;

        auto x0 = std::max(_range.GetLeft(), 32);
        auto y0 = std::max(_range.GetTop(), 32);
        auto x1 = std::min(_range.GetRight(), (int32_t)gMapSizeMaxXY);
        auto y1 = std::min(_range.GetBottom(), (int32_t)gMapSizeMaxXY);

        for (int32_t y = y0; y <= y1; y += 32)
        {
            for (int32_t x = x0; x <= x1; x += 32)
            {
                if (MapCanClearAt(x, y))
                {
                    auto cost = ClearSceneryFromTile(x / 32, y / 32, executing);
                    if (cost != MONEY32_UNDEFINED)
                    {
                        noValidTiles = false;
                        totalCost += cost;
                    }
                }
                else
                {
                    error = GA_ERROR::NOT_OWNED;
                    errorMessage = STR_LAND_NOT_OWNED_BY_PARK;
                }
            }
        }

        if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_LARGE)
        {
            ResetClearLargeSceneryFlag();
        }

        if (noValidTiles)
        {
            result->Error = error;
            result->ErrorMessage = errorMessage;
        }

        result->Cost = totalCost;
        return result;
    }

    money32 ClearSceneryFromTile(int32_t x, int32_t y, bool executing) const
    {
        // Pass down all flags.
        TileElement* tileElement = nullptr;
        money32 totalCost = 0;
        bool tileEdited;
        do
        {
            tileEdited = false;
            tileElement = map_get_first_element_at(x, y);
            do
            {
                auto type = tileElement->GetType();
                switch (type)
                {
                    case TILE_ELEMENT_TYPE_PATH:
                        if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_FOOTPATH)
                        {
                            auto footpathRemoveAction = FootpathRemoveAction(x * 32, y * 32, tileElement->base_height);
                            footpathRemoveAction.SetFlags(GetFlags());

                            auto res = executing ? GameActions::ExecuteNested(&footpathRemoveAction)
                                                 : GameActions::QueryNested(&footpathRemoveAction);

                            if (res->Error == GA_ERROR::OK)
                            {
                                totalCost += res->Cost;
                                tileEdited = executing;
                            }
                        }
                        break;
                    case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                        if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_SMALL)
                        {
                            auto removeSceneryAction = SmallSceneryRemoveAction(
                                x * 32, y * 32, tileElement->base_height, tileElement->AsSmallScenery()->GetSceneryQuadrant(),
                                tileElement->AsSmallScenery()->GetEntryIndex());
                            removeSceneryAction.SetFlags(GetFlags());

                            auto res = executing ? GameActions::ExecuteNested(&removeSceneryAction)
                                                 : GameActions::QueryNested(&removeSceneryAction);

                            if (res->Error == GA_ERROR::OK)
                            {
                                totalCost += res->Cost;
                                tileEdited = executing;
                            }
                        }
                        break;
                    case TILE_ELEMENT_TYPE_WALL:
                        if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_SMALL)
                        {
                            TileCoordsXYZD wallLocation = { x, y, tileElement->base_height, tileElement->GetDirection() };
                            auto wallRemoveAction = WallRemoveAction(wallLocation);
                            wallRemoveAction.SetFlags(GetFlags());

                            auto res = executing ? GameActions::ExecuteNested(&wallRemoveAction)
                                                 : GameActions::QueryNested(&wallRemoveAction);

                            if (res->Error == GA_ERROR::OK)
                            {
                                totalCost += res->Cost;
                                tileEdited = executing;
                            }
                        }
                        break;
                    case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                        if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_LARGE)
                        {
                            auto removeSceneryAction = LargeSceneryRemoveAction(
                                x * 32, y * 32, tileElement->base_height, tileElement->GetDirection(),
                                tileElement->AsLargeScenery()->GetSequenceIndex());
                            removeSceneryAction.SetFlags(GetFlags() | GAME_COMMAND_FLAG_PATH_SCENERY);

                            auto res = executing ? GameActions::ExecuteNested(&removeSceneryAction)
                                                 : GameActions::QueryNested(&removeSceneryAction);

                            if (res->Error == GA_ERROR::OK)
                            {
                                totalCost += res->Cost;
                                tileEdited = executing;
                            }
                        }
                        break;
                }
            } while (!tileEdited && !(tileElement++)->IsLastForTile());
        } while (tileEdited);

        return totalCost;
    }

    /**
     * Function to clear the flag that is set to prevent cost duplication
     * when using the clear scenery tool with large scenery.
     */
    static void ResetClearLargeSceneryFlag()
    {
        // TODO: Improve efficiency of this
        for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
        {
            for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
            {
                auto tileElement = map_get_first_element_at(x, y);
                do
                {
                    if (tileElement->GetType() == TILE_ELEMENT_TYPE_LARGE_SCENERY)
                    {
                        tileElement->AsLargeScenery()->SetIsAccounted(false);
                    }
                } while (!(tileElement++)->IsLastForTile());
            }
        }
    }

    static bool MapCanClearAt(int32_t x, int32_t y)
    {
        return (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode || map_is_location_owned_or_has_rights(x, y);
    }
};
