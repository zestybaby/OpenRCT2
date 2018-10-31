/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

// Structures shared between both RCT1 and RCT2.

#include "../common.h"
#include "../world/Location.hpp"

#define RCT12_MAX_RIDES_IN_PARK 255
#define RCT12_MAX_AWARDS 4
#define RCT12_MAX_NEWS_ITEMS 61
#define RCT12_MAX_STATIONS_PER_RIDE 4
#define RCT12_MAX_PEEP_SPAWNS 2
#define RCT12_MAX_PARK_ENTRANCES 4
// The number of elements in the patrol_areas array per staff member. Every bit in the array represents a 4x4 square.
// In RCT1, that's an 8-bit array. 8 * 128 = 1024 bits, which is also the number of 4x4 squares on a 128x128 map.
// For RCT2, it's a 32-bit array. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on a 256x256 map.
#define RCT12_PATROL_AREA_SIZE 128
#define RCT12_STAFF_TYPE_COUNT 4
#define RCT12_NUM_COLOUR_SCHEMES 4
#define RCT12_MAX_VEHICLES_PER_RIDE 32
#define RCT12_MAX_VEHICLE_COLOURS 32
#define RCT12_SOUND_ID_NULL 0xFF

#define RCT12_EXPENDITURE_TABLE_MONTH_COUNT 16
#define RCT12_EXPENDITURE_TYPE_COUNT 14
#define RCT12_FINANCE_GRAPH_SIZE 128

#define RCT12_MAX_USER_STRINGS 1024
#define RCT12_USER_STRING_MAX_LENGTH 32

#pragma pack(push, 1)

struct rct12_award
{
    uint16_t time;
    uint16_t type;
};
assert_struct_size(rct12_award, 4);

/**
 * A single news item / message.
 * size: 0x10C
 */
struct rct12_news_item
{
    uint8_t Type;
    uint8_t Flags;
    uint32_t Assoc;
    uint16_t Ticks;
    uint16_t MonthYear;
    uint8_t Day;
    uint8_t pad_0B;
    char Text[256];
};
assert_struct_size(rct12_news_item, 0x10C);

struct rct12_xyzd8
{
    uint8_t x, y, z, direction;
};
assert_struct_size(rct12_xyzd8, 4);

struct rct12_peep_spawn
{
    uint16_t x;
    uint16_t y;
    uint8_t z;
    uint8_t direction;
};
assert_struct_size(rct12_peep_spawn, 6);

enum class RCT12TileElementType : uint8_t
{
    Surface = (0 << 2),
    Path = (1 << 2),
    Track = (2 << 2),
    SmallScenery = (3 << 2),
    Entrance = (4 << 2),
    Wall = (5 << 2),
    LargeScenery = (6 << 2),
    Banner = (7 << 2),
    Corrupt = (8 << 2),
};

struct RCT12SurfaceElement;
struct RCT12PathElement;
struct RCT12TrackElement;
struct RCT12SmallSceneryElement;
struct RCT12LargeSceneryElement;
struct RCT12WallElement;
struct RCT12EntranceElement;
struct RCT12BannerElement;
struct RCT12CorruptElement;

struct RCT12TileElementBase
{
    uint8_t type;             // 0
    uint8_t flags;            // 1
    uint8_t base_height;      // 2
    uint8_t clearance_height; // 3

    uint8_t GetType() const;
    void SetType(uint8_t newType);
    uint8_t GetDirection() const;
    void SetDirection(uint8_t direction);
    uint8_t GetDirectionWithOffset(uint8_t offset) const;
    bool IsLastForTile() const;
    bool IsGhost() const;
    void Remove();
};

/**
 * Map element structure
 * size: 0x08
 */
struct RCT12TileElement : public RCT12TileElementBase
{
    uint8_t pad_04[4];

    template<typename TType, RCT12TileElementType TClass> TType* as() const
    {
        return (RCT12TileElementType)GetType() == TClass ? (TType*)this : nullptr;
    }

public:
    RCT12SurfaceElement* AsSurface() const
    {
        return as<RCT12SurfaceElement, RCT12TileElementType::Surface>();
    }
    RCT12PathElement* AsPath() const
    {
        return as<RCT12PathElement, RCT12TileElementType::Path>();
    }
    RCT12TrackElement* AsTrack() const
    {
        return as<RCT12TrackElement, RCT12TileElementType::Track>();
    }
    RCT12SmallSceneryElement* AsSmallScenery() const
    {
        return as<RCT12SmallSceneryElement, RCT12TileElementType::SmallScenery>();
    }
    RCT12LargeSceneryElement* AsLargeScenery() const
    {
        return as<RCT12LargeSceneryElement, RCT12TileElementType::LargeScenery>();
    }
    RCT12WallElement* AsWall() const
    {
        return as<RCT12WallElement, RCT12TileElementType::Wall>();
    }
    RCT12EntranceElement* AsEntrance() const
    {
        return as<RCT12EntranceElement, RCT12TileElementType::Entrance>();
    }
    RCT12BannerElement* AsBanner() const
    {
        return as<RCT12BannerElement, RCT12TileElementType::Banner>();
    }
    RCT12CorruptElement* AsCorrupt() const
    {
        return as<RCT12CorruptElement, RCT12TileElementType::Corrupt>();
    }

    void ClearAs(uint8_t newType);
};
assert_struct_size(RCT12TileElement, 8);

struct RCT12SurfaceElement : RCT12TileElementBase
{
private:
    uint8_t slope;        // 4 0xE0 Edge Style, 0x1F Slope
    uint8_t terrain;      // 5 0xE0 Terrain Style, 0x1F Water height
    uint8_t grass_length; // 6
    uint8_t ownership;    // 7
public:
    uint8_t GetSlope() const;
    void SetSlope(uint8_t newSlope);

    uint32_t GetSurfaceStyle() const;
    void SetSurfaceStyle(uint32_t newStyle);
    uint32_t GetEdgeStyle() const;
    void SetEdgeStyle(uint32_t newStyle);

    uint8_t GetGrassLength() const;
    void SetGrassLength(uint8_t newLength);
    void SetGrassLengthAndInvalidate(uint8_t newLength, CoordsXY coords);
    void UpdateGrassLength(CoordsXY coords);

    uint8_t GetOwnership() const;
    void SetOwnership(uint8_t newOwnership);

    uint32_t GetWaterHeight() const;
    void SetWaterHeight(uint32_t newWaterHeight);

    uint8_t GetParkFences() const;
    void SetParkFences(uint8_t newParkFences);

    bool HasTrackThatNeedsWater() const;
    void SetHasTrackThatNeedsWater(bool on);
};
assert_struct_size(RCT12SurfaceElement, 8);

struct RCT12PathElement : RCT12TileElementBase
{
private:
    uint8_t entryIndex; // 4, 0xF0 Path type, 0x08 Ride sign, 0x04 Set when path is sloped, 0x03 Rotation
    uint8_t additions;  // 5, 0bGSSSAAAA: G = Ghost, S = station index, A = addition (0 means no addition)
    uint8_t edges;      // 6
    union
    {
        uint8_t additionStatus; // 7
        uint8_t rideIndex;
    };

public:
    uint8_t GetEntryIndex() const;
    void SetEntryIndex(uint8_t newIndex);

    uint8_t GetQueueBannerDirection() const;
    void SetQueueBannerDirection(uint8_t direction);

    bool IsSloped() const;
    void SetSloped(bool isSloped);

    uint8_t GetSlopeDirection() const;
    void SetSlopeDirection(uint8_t newSlope);

    uint8_t GetRideIndex() const;
    void SetRideIndex(uint8_t newRideIndex);

    uint8_t GetStationIndex() const;
    void SetStationIndex(uint8_t newStationIndex);

    bool IsWide() const;
    void SetWide(bool isWide);

    bool IsQueue() const;
    void SetIsQueue(bool isQueue);
    bool HasQueueBanner() const;
    void SetHasQueueBanner(bool hasQueueBanner);

    uint8_t GetEdges() const;
    void SetEdges(uint8_t newEdges);
    uint8_t GetCorners() const;
    void SetCorners(uint8_t newCorners);
    uint8_t GetEdgesAndCorners() const;
    void SetEdgesAndCorners(uint8_t newEdgesAndCorners);

    bool HasAddition() const;
    uint8_t GetAddition() const;
    uint8_t GetAdditionEntryIndex() const;
    void SetAddition(uint8_t newAddition);

    bool AdditionIsGhost() const;
    void SetAdditionIsGhost(bool isGhost);

    uint8_t GetAdditionStatus() const;
    void SetAdditionStatus(uint8_t newStatus);

    uint8_t GetRCT1PathType() const;
};
assert_struct_size(RCT12PathElement, 8);

struct RCT12TrackElement : RCT12TileElementBase
{
    uint8_t trackType; // 4
    union
    {
        struct
        {
            // The lower 4 bits are the track sequence.
            // The upper 4 bits are either station bits or on-ride photo bits.
            //
            // Station bits:
            // - Bit 8 marks green light
            // - Bit 5-7 are station index.
            //
            // On-ride photo bits:
            // - Bits 7 and 8 are never set
            // - Bits 5 and 6 are set when a vehicle triggers the on-ride photo and act like a countdown from 3.
            // - If any of the bits 5-8 are set, the game counts it as a photo being taken.
            uint8_t sequence; // 5.
            uint8_t colour;   // 6
        };
        uint16_t mazeEntry; // 5
    };
    uint8_t rideIndex; // 7

public:
    uint8_t GetTrackType() const;
    void SetTrackType(uint8_t newEntryIndex);

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newSequenceIndex);

    uint8_t GetRideIndex() const;
    void SetRideIndex(uint8_t newRideIndex);

    uint8_t GetColourScheme() const;
    void SetColourScheme(uint8_t newColourScheme);

    uint8_t GetStationIndex() const;
    void SetStationIndex(uint8_t newStationIndex);

    bool HasChain() const;
    void SetHasChain(bool on);

    bool HasCableLift() const;
    void SetHasCableLift(bool on);

    bool IsInverted() const;
    void SetInverted(bool inverted);

    uint8_t GetBrakeBoosterSpeed() const;
    void SetBrakeBoosterSpeed(uint8_t speed);

    uint8_t HasGreenLight() const;
    void SetHasGreenLight(uint8_t greenLight);

    uint8_t GetSeatRotation() const;
    void SetSeatRotation(uint8_t newSeatRotation);

    uint16_t GetMazeEntry() const;
    void SetMazeEntry(uint16_t newMazeEntry);
    void MazeEntryAdd(uint16_t addVal);
    void MazeEntrySubtract(uint16_t subVal);

    bool IsTakingPhoto() const;
    void SetPhotoTimeout();
    void DecrementPhotoTimeout();

    bool IsHighlighted() const;
    void SetHighlight(bool on);

    // Used in RCT1, will be reintroduced at some point.
    // (See https://github.com/OpenRCT2/OpenRCT2/issues/7059)
    uint8_t GetDoorAState() const;
    uint8_t GetDoorBState() const;
};
assert_struct_size(RCT12TrackElement, 8);

struct RCT12SmallSceneryElement : RCT12TileElementBase
{
private:
    uint8_t entryIndex; // 4
    uint8_t age;        // 5
    uint8_t colour_1;   // 6
    uint8_t colour_2;   // 7
public:
    uint8_t GetEntryIndex() const;
    void SetEntryIndex(uint8_t newIndex);
    uint8_t GetAge() const;
    void SetAge(uint8_t newAge);
    void IncreaseAge(int32_t x, int32_t y);
    uint8_t GetSceneryQuadrant() const;
    void SetSceneryQuadrant(uint8_t newQuadrant);
    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t colour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t colour);
    bool NeedsSupports() const;
    void SetNeedsSupports();
};
assert_struct_size(RCT12SmallSceneryElement, 8);

struct RCT12LargeSceneryElement : RCT12TileElementBase
{
private:
    uint16_t entryIndex; // 4
    uint8_t colour[2];   // 6
public:
    uint32_t GetEntryIndex() const;
    void SetEntryIndex(uint32_t newIndex);

    uint16_t GetSequenceIndex() const;
    void SetSequenceIndex(uint16_t newIndex);

    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t colour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t colour);

    BannerIndex GetBannerIndex() const;
    void SetBannerIndex(BannerIndex newIndex);
};
assert_struct_size(RCT12LargeSceneryElement, 8);

struct RCT12WallElement : RCT12TileElementBase
{
private:
    uint8_t entryIndex; // 4
    union
    {
        uint8_t colour_3;         // 5
        BannerIndex banner_index; // 5
    };
    uint8_t colour_1;  // 6 0b_2221_1111 2 = colour_2 (uses flags for rest of colour2), 1 = colour_1
    uint8_t animation; // 7 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)

public:
    uint8_t GetEntryIndex() const;
    void SetEntryIndex(uint8_t newIndex);

    uint8_t GetSlope() const;
    void SetSlope(uint8_t newslope);

    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t newColour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t newColour);
    colour_t GetTertiaryColour() const;
    void SetTertiaryColour(colour_t newColour);

    uint8_t GetAnimationFrame() const;
    void SetAnimationFrame(uint8_t frameNum);

    BannerIndex GetBannerIndex() const;
    void SetBannerIndex(BannerIndex newIndex);

    bool IsAcrossTrack() const;
    void SetAcrossTrack(bool acrossTrack);
    bool AnimationIsBackwards() const;
    void SetAnimationIsBackwards(bool isBackwards);

    int32_t GetRCT1WallType(int32_t edge) const;
    colour_t GetRCT1WallColour() const;
};
assert_struct_size(RCT12WallElement, 8);

struct RCT12EntranceElement : RCT12TileElementBase
{
private:
    uint8_t entranceType; // 4
    uint8_t index;        // 5. 0bUSSS????, S = station index.
    uint8_t pathType;     // 6
    uint8_t rideIndex;    // 7

public:
    uint8_t GetEntranceType() const;
    void SetEntranceType(uint8_t newType);

    uint8_t GetRideIndex() const;
    void SetRideIndex(uint8_t newRideIndex);

    uint8_t GetStationIndex() const;
    void SetStationIndex(uint8_t stationIndex);

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newSequenceIndex);

    uint8_t GetPathType() const;
    void SetPathType(uint8_t newPathType);
};
assert_struct_size(RCT12EntranceElement, 8);

struct RCT12BannerElement : RCT12TileElementBase
{
private:
    BannerIndex index; // 4
    uint8_t position;  // 5
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t flags;  // 6
    uint8_t unused; // 7
#pragma clang diagnostic pop
public:
    BannerIndex GetIndex() const;
    void SetIndex(BannerIndex newIndex);

    uint8_t GetPosition() const;
    void SetPosition(uint8_t newPosition);

    uint8_t GetAllowedEdges() const;
    void SetAllowedEdges(uint8_t newEdges);
    void ResetAllowedEdges();
};
assert_struct_size(RCT12BannerElement, 8);

struct RCT12CorruptElement : RCT12TileElementBase
{
    uint8_t pad[4];
};
assert_struct_size(RCT12CorruptElement, 8);

#pragma pack(pop)
