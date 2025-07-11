#pragma once

#include "CompanyAi/CompanyAi.h"
#include "Economy/Currency.h"
#include "Economy/Expenditures.h"
#include "Engine/Limits.h"
#include "Types.hpp"
#include <OpenLoco/Core/BitSet.hpp>
#include <OpenLoco/Core/EnumFlags.hpp>
#include <OpenLoco/Engine/World.hpp>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sfl/static_vector.hpp>
#include <vector>

namespace OpenLoco
{
    enum class CompanyFlags : uint32_t
    {
        none = 0U,
        unk0 = (1U << 0),                      // 0x01
        unk1 = (1U << 1),                      // 0x02
        unk2 = (1U << 2),                      // 0x04
        sorted = (1U << 3),                    // 0x08
        increasedPerformance = (1U << 4),      // 0x10
        decreasedPerformance = (1U << 5),      // 0x20
        challengeCompleted = (1U << 6),        // 0x40
        challengeFailed = (1U << 7),           // 0x80
        challengeBeatenByOpponent = (1U << 8), // 0x100
        bankrupt = (1U << 9),                  // 0x200
        autopayLoan = (1U << 31),              // 0x80000000 new for OpenLoco
    };
    OPENLOCO_ENABLE_ENUM_OPERATORS(CompanyFlags);

    enum class AiPlaystyleFlags : uint32_t
    {
        none = 0U,
        unk0 = (1U << 0),      // 0x01
        unk1 = (1U << 1),      // 0x02
        unk2 = (1U << 2),      // 0x04
        unk3 = (1U << 3),      // 0x08
        noAir = (1U << 4),     // 0x10
        noWater = (1U << 5),   // 0x20
        unk6 = (1U << 6),      // 0x40
        unk7 = (1U << 7),      // 0x80
        townIdSet = (1U << 8), // 0x100
    };
    OPENLOCO_ENABLE_ENUM_OPERATORS(AiPlaystyleFlags);

    enum class CorporateRating : uint8_t
    {
        platelayer,           // 0 - 9.9%
        engineer,             // 10 - 19.9%
        trafficManager,       // 20 - 29.9%
        transportCoordinator, // 30 - 39.9%
        routeSupervisor,      // 40 - 49.9%
        director,             // 50 - 59.9%
        chiefExecutive,       // 60 - 69.9%
        chairman,             // 70 - 79.9%
        president,            // 80 - 89.9%
        tycoon                // 90 - 100%
    };

    enum ObservationStatus : uint8_t
    {
        empty,
        buildingTrackRoad,
        buildingAirport,
        buildingDock,
        checkingServices,
        surveyingLandscape,
    };

    enum class Emotion : uint8_t
    {
        neutral,
        happy,
        worried,
        thinking,
        dejected,
        surprised,
        scared,
        angry,
        disgusted,
    };

    class OwnerStatus
    {
        int16_t data[2];

    public:
        OwnerStatus()
        {
            data[0] = -1;
            data[1] = 0;
        }
        OwnerStatus(EntityId id)
        {
            data[0] = -2;
            data[1] = enumValue(id);
        }
        OwnerStatus(const World::Pos2& pos)
        {
            data[0] = pos.x;
            data[1] = pos.y;
        }
        OwnerStatus(int16_t ax, int16_t cx)
        {
            data[0] = ax;
            data[1] = cx;
        }
        void getData(int16_t* res) const
        {
            res[0] = data[0];
            res[1] = data[1];
        }

        bool isEmpty() const { return data[0] == -1; }
        bool isEntity() const { return data[0] == -2; }
        EntityId getEntity() const
        {
            if (isEntity())
            {
                return static_cast<EntityId>(data[1]);
            }
            return EntityId::null;
        }
        World::Pos2 getPosition() const
        {
            if (isEntity())
            {
                return {};
            }
            return World::Pos2{ data[0], data[1] };
        }
    };

    void formatPerformanceIndex(const int16_t performanceIndex, FormatArguments& args);

    constexpr size_t kExpenditureHistoryCapacity = 16;

#pragma pack(push, 1)
    struct Company
    {
        struct Unk25C0HashTableEntry
        {
            uint16_t var_00; // x
            uint16_t var_02; // y + flags
            uint8_t var_04;  // z
            uint8_t var_05;  // trackId | (direction << 6)

            Unk25C0HashTableEntry() = default;
            Unk25C0HashTableEntry(World::Pos3 pos, uint8_t trackRoadId, uint8_t direction);

            constexpr World::Pos3 getPosition() const
            {
                return World::Pos3(var_00, var_02 & 0xFFFE, var_04 * 4 /*World::kSmallZStep*/);
            }
            constexpr uint8_t getTrackRoadId() const
            {
                return var_05 & 0x3F;
            }
            constexpr uint8_t getDirection() const
            {
                return (var_05 >> 6) & 0x03;
            }

            // When looking up the hash table entry if there is a hash collision check the next entries in
            // the table as well for matches until no hash collision or match is found
            constexpr bool hasHashCollision() const
            {
                return (var_02 & (1U << 0)) != 0;
            }
            constexpr uint16_t calculateHash() const;
        };

        StringId name;
        StringId ownerName;
        CompanyFlags challengeFlags;                                                    // 0x04
        currency48_t cash;                                                              // 0x08
        currency32_t currentLoan;                                                       // 0x0E
        uint32_t updateCounter;                                                         // 0x12
        int16_t performanceIndex;                                                       // 0x16
        uint8_t competitorId;                                                           // 0x18
        Emotion ownerEmotion;                                                           // 0x19
        ColourScheme mainColours;                                                       // 0x1A
        ColourScheme vehicleColours[10];                                                // 0x1C
        uint32_t customVehicleColoursSet;                                               // 0x30
        BitSet<224> unlockedVehicles;                                                   // 0x34
        uint16_t availableVehicles;                                                     // 0x50
        AiPlaystyleFlags aiPlaystyleFlags;                                              // 0x52
        uint8_t aiPlaystyleTownId;                                                      // 0x56
        uint8_t numExpenditureYears;                                                    // 0x57
        currency32_t expenditures[kExpenditureHistoryCapacity][ExpenditureType::Count]; // 0x58
        uint32_t startedDate;                                                           // 0x0498
        uint32_t var_49C;
        uint32_t var_4A0;
        AiThinkState var_4A4; // 0x04A4
        uint8_t var_4A5;
        AiPlaceVehicleState var_4A6;
        uint8_t var_4A7;
        AiThought aiThoughts[kMaxAiThoughts]; // 0x04A8
        uint8_t activeThoughtId;              // 0x2578
        World::SmallZ headquartersZ;          // 0x2579
        coord_t headquartersX;                // 0x257A -1 on no headquarter placed
        coord_t headquartersY;                // 0x257C
        union
        {
            currency32_t activeThoughtRevenueEstimate; // 0x257E Also used for thoughtState2AiStationIdx in sub_430CEC TODO: Don't do this
            uint32_t thoughtState2AiStationIdx;        // 0x257E Also used mostly for activeThoughtRevenueEstimate TODO: Don't do this
        };
        uint32_t var_2582;
        uint8_t pad_2586[0x2596 - 0x2586];
        uint32_t var_2596;
        uint8_t var_259A;
        uint8_t var_259B;
        uint8_t var_259C;
        uint8_t pad_259D;
        uint32_t aiPlaceVehicleIndex;
        uint8_t pad_25A2[0x25BE - 0x25A2];
        AiThoughtType var_25BE;
        CorporateRating currentRating;          // 0x25BF
        Unk25C0HashTableEntry var_25C0[0x1000]; // 0x25C0 Hash table entries
        uint16_t var_25C0_length;               // 0x85C0 Hash table length
        uint8_t var_85C2;
        uint8_t var_85C3;
        World::Pos2 var_85C4;
        World::SmallZ var_85C8;
        World::Pos2 var_85C9;
        World::SmallZ var_85CD;
        uint8_t var_85CE;
        uint8_t var_85CF;
        World::Pos2 var_85D0;
        World::SmallZ var_85D4;
        uint16_t var_85D5;
        World::Pos2 var_85D7;
        World::SmallZ var_85DB;
        uint16_t var_85DC;
        uint32_t var_85DE;
        uint32_t var_85E2;
        uint16_t var_85E6;
        uint16_t var_85E8;
        uint32_t var_85EA;
        uint8_t var_85EE;
        uint8_t var_85EF;
        uint16_t var_85F0;
        currency32_t var_85F2;
        uint16_t var_85F6;
        uint32_t cargoUnitsTotalDelivered;        // 0x85F8
        uint32_t cargoUnitsDeliveredHistory[120]; // 0x85FC
        int16_t performanceIndexHistory[120];     // 0x87DC
        uint16_t historySize;                     // 0x88CC
        currency48_t companyValueHistory[120];    // 0x88CE
        currency48_t vehicleProfit;               // 0x8B9E
        uint16_t transportTypeCount[6];           // 0x8BA4
        uint8_t activeEmotions[9];                // 0x8BB0 duration in days that emotion is active 0 == not active
        ObservationStatus observationStatus;      // 0x8BB9;
        TownId observationTownId;                 // 0x8BBA;
        EntityId observationEntity;               // 0x8BBC;
        int16_t observationX;                     // 0x8BBE;
        int16_t observationY;                     // 0x8BC0;
        uint16_t observationObject;               // 0x8BC2;
        uint16_t observationTimeout;              // 0x8BC4
        OwnerStatus ownerStatus;                  // 0x8BC6
        uint8_t pad_8BCA[0x8BCE - 0x8BCA];
        uint32_t cargoDelivered[32];             // 0x8BCE;
        uint8_t challengeProgress;               // 0x8C4E - percent completed on challenge
        uint8_t numMonthsInTheRed;               // 0x8C4F
        uint32_t cargoUnitsTotalDistance;        // 0x8C50
        uint32_t cargoUnitsDistanceHistory[120]; // 0x8C54
        uint16_t jailStatus;                     // 0x8E34
        uint8_t pad_8E36[0x8FA8 - 0x8E36];

        CompanyId id() const;
        bool empty() const;
        bool isVehicleIndexUnlocked(const uint8_t vehicleIndex) const;
        void recalculateTransportCounts();
        void clearOwnerStatusForDeletedVehicle(EntityId vehicleId);
        void updateDaily();
        void updateDailyLogic();
        void updateDailyPlayer();
        void evaluateChallengeProgress();
        void updateDailyControllingPlayer();
        void updateMonthlyHeadquarters();
        void updateMonthly1();
        void updateLoanAutorepay();
        void updateQuarterly();
        void updateVehicleColours();
        void updateHeadquartersColour();
        void updateOwnerEmotion();
        uint8_t getHeadquarterPerformanceVariation() const;

        bool hashTableContains(const Unk25C0HashTableEntry& entry) const;
        bool addHashTableEntry(const Unk25C0HashTableEntry& entry);

    private:
        void setHeadquartersVariation(const uint8_t variation);
        void setHeadquartersVariation(const uint8_t variation, const World::TilePos2& pos);

        uint8_t getNewChallengeProgress() const;
    };
#pragma pack(pop)

    static_assert(sizeof(Company) == 0x8FA8);
    static_assert(sizeof(Company::expenditures) == 0x440);
    static_assert(offsetof(Company, companyValueHistory[0]) == 0x88CE);
    static_assert(offsetof(Company, vehicleProfit) == 0x8B9E);
    static_assert(offsetof(Company, challengeProgress) == 0x8C4E);
    static_assert(offsetof(Company, activeEmotions) == 0x8BB0);

    StringId getCorporateRatingAsStringId(CorporateRating rating);
    constexpr CorporateRating performanceToRating(int16_t performanceIndex);
    void formatPerformanceIndex(const int16_t performanceIndex, FormatArguments& args);
    void companyEmotionEvent(CompanyId companyId, Emotion emotion);
    void companySetObservation(CompanyId id, ObservationStatus status, World::Pos2 pos, EntityId entity, uint16_t object);

    // This is kMaxRoadObjects + kMaxTrackObjects as tram tracks are roads but are tracks
    // and vice versa there was capabilities for some unknown track type to be classed as a road
    using AvailableTracksAndRoads = sfl::static_vector<uint8_t, Limits::kMaxRoadObjects + Limits::kMaxTrackObjects>;

    AvailableTracksAndRoads companyGetAvailableRailTracks(const CompanyId id);
    AvailableTracksAndRoads companyGetAvailableRoads(const CompanyId id);
    void updateYearly(Company& company);
    struct ProfitAndValue
    {
        currency48_t vehicleProfit;
        currency48_t companyValue;
    };

    // 0x00437D79
    ProfitAndValue calculateCompanyValue(const Company& company);
}
