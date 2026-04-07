#ifndef FISHING_H
#define FISHING_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>

enum class FishingSpot {
    RIVER,
    OCEAN,
    LAKE,
    POND,
    UNDERGROUND_CAVE,
    MOUNTAIN_STREAM,
    BEACH,
    DOCK,
    ICE_FISHING
};

enum class FishRarity {
    COMMON,
    UNCOMMON,
    RARE,
    VERY_RARE,
    LEGENDARY
};

enum class FishSize {
    TINY,
    SMALL,
    MEDIUM,
    LARGE,
    HUGE,
    COLOSSAL
};

struct FishSpecies {
    std::string name;
    std::string displayName;
    FishRarity rarity;
    FishSize sizeRange;
    int minSize;
    int maxSize;
    int baseValue;
    std::vector<FishingSpot> habitats;
    std::vector<int> activeSeasons; // 0=Spring, 1=Summer, 2=Fall, 3=Winter
    std::vector<int> activeHours; // 0-23
    std::string baitPreference;
    float catchDifficulty;
    bool isBossFish;
    
    FishSpecies();
    std::string getRarityName() const;
    std::string getSizeName(int size) const;
    int calculateValue(int size) const;
};

struct CaughtFish {
    std::string species;
    int size;
    int value;
    int caughtHour;
    int caughtDay;
    int caughtSeason;
    FishingSpot location;
    bool isNewRecord;
    std::string playerName;
    
    CaughtFish();
    CaughtFish(const std::string& sp, int s, int v, FishingSpot loc);
    std::string toDisplayString() const;
};

struct FishingQuest {
    std::string id;
    std::string title;
    std::string description;
    std::string requestedFish;
    int requestedQuantity;
    int requestedSize;
    std::map<std::string, int> rewardItems;
    int rewardMoney;
    int rewardFriendship;
    bool isCompleted;
    std::string questGiver;
    int timeLimit; // in game hours
    
    FishingQuest();
};

struct FishingTournament {
    std::string id;
    std::string name;
    int season;
    int day;
    int duration; // in minutes
    std::string targetFish;
    std::map<std::string, int> playerScores;
    std::vector<std::string> leaderboard;
    bool isActive;
    bool isCompleted;
    std::map<int, std::map<std::string, int>> prizes; // place -> items
    
    FishingTournament();
    void addCatch(const std::string& player, const CaughtFish& fish);
    std::string getWinner() const;
    int getPlayerRank(const std::string& player) const;
};

enum class BaitType {
    NONE,
    WORM,
    CRICKET,
    MINNOW,
    SHRIMP,
    SQUID,
    MAGIC_BAIT,
    TREASURE_BAIT
};

struct FishingRod {
    std::string name;
    int level;
    float catchBonus;
    float tensionCapacity;
    float durability;
    float maxDurability;
    BaitType equippedBait;
    int baitCount;
    bool hasReel;
    float reelSpeed;
    
    FishingRod();
    FishingRod(const std::string& n, int lvl);
    std::string getRodName() const;
    bool use();
    void equipBait(BaitType bait, int count);
    void removeBait();
};

enum class FishingState {
    IDLE,
    CASTING,
    WAITING,
    BITING,
    REELING,
    CAUGHT,
    ESCAPED
};

class FishingMiniGame {
public:
    FishingMiniGame();
    
    bool startFishing(FishingSpot spot, FishingRod& rod, int season, int hour);
    bool castLine();
    bool handleBite();
    bool reelIn(float tension);
    bool giveUp();
    
    FishingState getState() const;
    float getTension() const;
    float getProgress() const;
    int getTimeRemaining() const;
    CaughtFish getCurrentFish() const;
    
private:
    FishingState state;
    FishingSpot currentSpot;
    FishingRod* currentRod;
    float tension;
    float progress;
    int biteTimer;
    int catchTimer;
    int timeRemaining;
    CaughtFish pendingFish;
    std::mt19937 rng;
    
    float calculateCatchChance() const;
    void generatePotentialCatch();
};

class FishingSystem {
public:
    static FishingSystem& getInstance();
    
    void initialize();
    
    bool fish(FishingSpot spot, FishingRod& rod, int season, int hour);
    CaughtFish catchFish(FishingSpot spot, const std::string& bait, int season, int hour);
    
    bool acceptQuest(const std::string& questId);
    bool completeQuest(const std::string& questId, const CaughtFish& fish);
    std::vector<FishingQuest> getActiveQuests() const;
    std::vector<FishingQuest> getCompletedQuests() const;
    
    bool startTournament(const std::string& tournamentId);
    bool submitTournamentCatch(const std::string& tournamentId, const CaughtFish& fish);
    FishingTournament* getActiveTournament();
    std::vector<FishingTournament> getUpcomingTournaments() const;
    
    void addToAquarium(const CaughtFish& fish);
    std::vector<CaughtFish> getAquarium() const;
    std::vector<CaughtFish> getFishCollection() const;
    bool hasCaughtSpecies(const std::string& species) const;
    int getSpeciesCount() const;
    
    std::vector<FishSpecies> getFishAvailableAt(FishingSpot spot, int season, int hour) const;
    static const std::map<std::string, FishSpecies>& getFishDatabase();
    
    std::string serializeState() const;
    void deserializeState(const std::string& jsonState);
    
    int getTotalFishCaught() const;
    int getLargestFish(const std::string& species) const;
    std::vector<CaughtFish> getPersonalBests() const;
    
private:
    FishingSystem();
    ~FishingSystem();
    FishingSystem(const FishingSystem&) = delete;
    FishingSystem& operator=(const FishingSystem&) = delete;
    
    std::map<std::string, FishSpecies> fishDatabase;
    std::vector<CaughtFish> aquarium;
    std::vector<CaughtFish> collection;
    std::vector<FishingQuest> activeQuests;
    std::vector<FishingQuest> completedQuests;
    std::vector<FishingTournament> tournaments;
    FishingTournament* activeTournament;
    std::map<std::string, int> speciesRecords;
    int totalFishCaught;
    
    void loadFishDatabase();
    std::string escapeJson(const std::string& input) const;
};

#endif
