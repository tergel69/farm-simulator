#include "Fishing.h"
#include <sstream>
#include <algorithm>
#include <ctime>

// FishSpecies Implementation
FishSpecies::FishSpecies() 
    : rarity(FishRarity::COMMON), sizeRange(FishSize::MEDIUM), minSize(10), maxSize(50),
      baseValue(20), catchDifficulty(0.5f), isBossFish(false) {}

std::string FishSpecies::getRarityName() const {
    switch (rarity) {
        case FishRarity::COMMON: return "Common";
        case FishRarity::UNCOMMON: return "Uncommon";
        case FishRarity::RARE: return "Rare";
        case FishRarity::VERY_RARE: return "Very Rare";
        case FishRarity::LEGENDARY: return "Legendary";
        default: return "Unknown";
    }
}

std::string FishSpecies::getSizeName(int size) const {
    if (size < minSize + (maxSize - minSize) * 0.2) return "Tiny";
    if (size < minSize + (maxSize - minSize) * 0.4) return "Small";
    if (size < minSize + (maxSize - minSize) * 0.6) return "Medium";
    if (size < minSize + (maxSize - minSize) * 0.8) return "Large";
    if (size < maxSize) return "Huge";
    return "Colossal";
}

int FishSpecies::calculateValue(int size) const {
    float sizeMultiplier = 1.0f + (static_cast<float>(size) / maxSize) * 0.5f;
    float rarityMultiplier = 1.0f;
    switch (rarity) {
        case FishRarity::UNCOMMON: rarityMultiplier = 1.5f; break;
        case FishRarity::RARE: rarityMultiplier = 2.5f; break;
        case FishRarity::VERY_RARE: rarityMultiplier = 4.0f; break;
        case FishRarity::LEGENDARY: rarityMultiplier = 10.0f; break;
        default: break;
    }
    return static_cast<int>(baseValue * sizeMultiplier * rarityMultiplier);
}

// CaughtFish Implementation
CaughtFish::CaughtFish() 
    : size(0), value(0), caughtHour(0), caughtDay(0), caughtSeason(0),
      location(FishingSpot::RIVER), isNewRecord(false) {}

CaughtFish::CaughtFish(const std::string& sp, int s, int v, FishingSpot loc)
    : species(sp), size(s), value(v), location(loc), isNewRecord(false) {}

std::string CaughtFish::toDisplayString() const {
    std::ostringstream oss;
    oss << size << "cm " << species << " (" << value << "g)";
    return oss.str();
}

// FishingQuest Implementation
FishingQuest::FishingQuest() 
    : requestedQuantity(1), requestedSize(0), rewardMoney(0), rewardFriendship(0),
      isCompleted(false), timeLimit(24) {}

// FishingTournament Implementation
FishingTournament::FishingTournament() 
    : season(0), day(0), duration(60), isActive(false), isCompleted(false) {}

void FishingTournament::addCatch(const std::string& player, const CaughtFish& fish) {
    playerScores[player] += fish.value;
}

std::string FishingTournament::getWinner() const {
    std::string winner;
    int maxScore = 0;
    for (const auto& pair : playerScores) {
        if (pair.second > maxScore) {
            maxScore = pair.second;
            winner = pair.first;
        }
    }
    return winner;
}

int FishingTournament::getPlayerRank(const std::string& player) const {
    int playerScore = playerScores.count(player) ? playerScores.at(player) : 0;
    int rank = 1;
    for (const auto& pair : playerScores) {
        if (pair.second > playerScore) rank++;
    }
    return rank;
}

// FishingRod Implementation
FishingRod::FishingRod() 
    : level(1), catchBonus(0.0f), tensionCapacity(100.0f), durability(100.0f),
      maxDurability(100.0f), equippedBait(BaitType::NONE), baitCount(0),
      hasReel(false), reelSpeed(1.0f) {}

FishingRod::FishingRod(const std::string& n, int lvl)
    : name(n), level(lvl), catchBonus(lvl * 0.1f), tensionCapacity(100.0f + lvl * 10),
      durability(100.0f), maxDurability(100.0f + lvl * 20), equippedBait(BaitType::NONE),
      baitCount(0), hasReel(lvl >= 3), reelSpeed(1.0f + lvl * 0.05f) {}

std::string FishingRod::getRodName() const {
    return name.empty() ? "Basic Rod" : name;
}

bool FishingRod::use() {
    if (durability > 0) {
        durability -= 0.5f;
        return true;
    }
    return false;
}

void FishingRod::equipBait(BaitType bait, int count) {
    equippedBait = bait;
    baitCount = count;
}

void FishingRod::removeBait() {
    equippedBait = BaitType::NONE;
    baitCount = 0;
}

// FishingMiniGame Implementation
FishingMiniGame::FishingMiniGame() 
    : state(FishingState::IDLE), currentSpot(FishingSpot::RIVER), currentRod(nullptr),
      tension(0.0f), progress(0.0f), biteTimer(0), catchTimer(0), timeRemaining(60) {}

bool FishingMiniGame::startFishing(FishingSpot spot, FishingRod& rod, int season, int hour) {
    state = FishingState::IDLE;
    currentSpot = spot;
    currentRod = &rod;
    tension = 0.0f;
    progress = 0.0f;
    timeRemaining = 60;
    generatePotentialCatch();
    return true;
}

bool FishingMiniGame::castLine() {
    if (state != FishingState::IDLE) return false;
    state = FishingState::CASTING;
    biteTimer = 30 + (rand() % 60);
    return true;
}

bool FishingMiniGame::handleBite() {
    if (state != FishingState::WAITING && state != FishingState::BITING) return false;
    state = FishingState::REELING;
    catchTimer = 30;
    return true;
}

bool FishingMiniGame::reelIn(float tensionLevel) {
    if (state != FishingState::REELING) return false;
    
    tension = tensionLevel;
    if (tension > currentRod->tensionCapacity) {
        state = FishingState::ESCAPED;
        return false;
    }
    
    progress += 10.0f * currentRod->reelSpeed;
    if (progress >= 100.0f) {
        state = FishingState::CAUGHT;
        return true;
    }
    return false;
}

bool FishingMiniGame::giveUp() {
    state = FishingState::IDLE;
    return true;
}

FishingState FishingMiniGame::getState() const {
    return state;
}

float FishingMiniGame::getTension() const {
    return tension;
}

float FishingMiniGame::getProgress() const {
    return progress;
}

int FishingMiniGame::getTimeRemaining() const {
    return timeRemaining;
}

CaughtFish FishingMiniGame::getCurrentFish() const {
    return pendingFish;
}

float FishingMiniGame::calculateCatchChance() const {
    float baseChance = 0.7f;
    if (currentRod) {
        baseChance += currentRod->catchBonus;
    }
    return std::min(0.95f, baseChance);
}

void FishingMiniGame::generatePotentialCatch() {
    // Simplified - would use fish database in full implementation
    pendingFish = CaughtFish("Trout", 25, 50, currentSpot);
}

// FishingSystem Implementation
FishingSystem& FishingSystem::getInstance() {
    static FishingSystem instance;
    return instance;
}

FishingSystem::FishingSystem() : activeTournament(nullptr), totalFishCaught(0) {
    loadFishDatabase();
}

FishingSystem::~FishingSystem() {}

void FishingSystem::initialize() {
    loadFishDatabase();
}

void FishingSystem::loadFishDatabase() {
    // Common fish
    FishSpecies trout;
    trout.name = "Trout";
    trout.displayName = "Rainbow Trout";
    trout.rarity = FishRarity::COMMON;
    trout.baseValue = 20;
    trout.minSize = 15;
    trout.maxSize = 40;
    trout.habitats = {FishingSpot::RIVER, FishingSpot::LAKE, FishingSpot::MOUNTAIN_STREAM};
    trout.activeSeasons = {0, 1, 2}; // Spring, Summer, Fall
    fishDatabase["Trout"] = trout;
    
    FishSpecies bass;
    bass.name = "Bass";
    bass.displayName = "Largemouth Bass";
    bass.rarity = FishRarity::COMMON;
    bass.baseValue = 25;
    bass.minSize = 20;
    bass.maxSize = 50;
    bass.habitats = {FishingSpot::LAKE, FishingSpot::POND};
    bass.activeSeasons = {1, 2};
    fishDatabase["Bass"] = bass;
    
    // Rare fish
    FishSpecies salmon;
    salmon.name = "Salmon";
    salmon.displayName = "King Salmon";
    salmon.rarity = FishRarity::UNCOMMON;
    salmon.baseValue = 50;
    salmon.minSize = 40;
    salmon.maxSize = 100;
    salmon.habitats = {FishingSpot::RIVER, FishingSpot::MOUNTAIN_STREAM};
    salmon.activeSeasons = {2, 3};
    fishDatabase["Salmon"] = salmon;
    
    // Legendary fish
    FishSpecies legend;
    legend.name = "LegendFish";
    legend.displayName = "The Ancient One";
    legend.rarity = FishRarity::LEGENDARY;
    legend.baseValue = 500;
    legend.minSize = 150;
    legend.maxSize = 300;
    legend.habitats = {FishingSpot::UNDERGROUND_CAVE};
    legend.activeSeasons = {0, 1, 2, 3};
    legend.isBossFish = true;
    fishDatabase["LegendFish"] = legend;
}

bool FishingSystem::fish(FishingSpot spot, FishingRod& rod, int season, int hour) {
    if (!rod.use()) return false;
    
    CaughtFish fish = catchFish(spot, rod.equippedBait == BaitType::NONE ? "" : "Bait", season, hour);
    if (fish.value > 0) {
        collection.push_back(fish);
        totalFishCaught++;
        
        // Check for new record
        if (speciesRecords.find(fish.species) == speciesRecords.end() || 
            fish.size > speciesRecords[fish.species]) {
            fish.isNewRecord = true;
            speciesRecords[fish.species] = fish.size;
        }
        
        // Add to aquarium if rare
        if (fish.value >= 100) {
            addToAquarium(fish);
        }
        
        return true;
    }
    return false;
}

CaughtFish FishingSystem::catchFish(FishingSpot spot, const std::string& bait, int season, int hour) {
    std::vector<FishSpecies> available = getFishAvailableAt(spot, season, hour);
    if (available.empty()) {
        return CaughtFish();
    }
    
    // Select random fish
    int idx = rand() % available.size();
    FishSpecies& species = available[idx];
    
    // Generate size
    int size = species.minSize + (rand() % (species.maxSize - species.minSize + 1));
    int value = species.calculateValue(size);
    
    return CaughtFish(species.name, size, value, spot);
}

bool FishingSystem::acceptQuest(const std::string& questId) {
    for (auto& quest : activeQuests) {
        if (quest.id == questId) return false;
    }
    // Would load quest from database
    return true;
}

bool FishingSystem::completeQuest(const std::string& questId, const CaughtFish& fish) {
    for (auto& quest : activeQuests) {
        if (quest.id == questId && quest.requestedFish == fish.species) {
            quest.isCompleted = true;
            completedQuests.push_back(quest);
            return true;
        }
    }
    return false;
}

std::vector<FishingQuest> FishingSystem::getActiveQuests() const {
    return activeQuests;
}

std::vector<FishingQuest> FishingSystem::getCompletedQuests() const {
    return completedQuests;
}

bool FishingSystem::startTournament(const std::string& tournamentId) {
    // Find and activate tournament
    return true;
}

bool FishingSystem::submitTournamentCatch(const std::string& tournamentId, const CaughtFish& fish) {
    if (activeTournament) {
        activeTournament->addCatch("Player", fish);
        return true;
    }
    return false;
}

FishingTournament* FishingSystem::getActiveTournament() {
    return activeTournament;
}

std::vector<FishingTournament> FishingSystem::getUpcomingTournaments() const {
    return tournaments;
}

void FishingSystem::addToAquarium(const CaughtFish& fish) {
    aquarium.push_back(fish);
}

std::vector<CaughtFish> FishingSystem::getAquarium() const {
    return aquarium;
}

std::vector<CaughtFish> FishingSystem::getFishCollection() const {
    return collection;
}

bool FishingSystem::hasCaughtSpecies(const std::string& species) const {
    for (const auto& fish : collection) {
        if (fish.species == species) return true;
    }
    return false;
}

int FishingSystem::getSpeciesCount() const {
    return speciesRecords.size();
}

std::vector<FishSpecies> FishingSystem::getFishAvailableAt(FishingSpot spot, int season, int hour) const {
    std::vector<FishSpecies> available;
    for (const auto& pair : fishDatabase) {
        const FishSpecies& fish = pair.second;
        bool habitatMatch = std::find(fish.habitats.begin(), fish.habitats.end(), spot) != fish.habitats.end();
        bool seasonMatch = std::find(fish.activeSeasons.begin(), fish.activeSeasons.end(), season) != fish.activeSeasons.end();
        bool hourMatch = fish.activeHours.empty() || std::find(fish.activeHours.begin(), fish.activeHours.end(), hour) != fish.activeHours.end();
        
        if (habitatMatch && seasonMatch && hourMatch) {
            available.push_back(fish);
        }
    }
    return available;
}

const std::map<std::string, FishSpecies>& FishingSystem::getFishDatabase() {
    return getInstance().fishDatabase;
}

std::string FishingSystem::serializeState() const {
    std::ostringstream oss;
    oss << "{\"totalFishCaught\":" << totalFishCaught << ",";
    oss << "\"speciesCount\":" << speciesRecords.size() << ",";
    oss << "\"collection\":[";
    bool first = true;
    for (const auto& fish : collection) {
        if (!first) oss << ",";
        oss << "{\"species\":\"" << fish.species << "\",\"size\":" << fish.size << ",\"value\":" << fish.value << "}";
        first = false;
    }
    oss << "]}";
    return oss.str();
}

void FishingSystem::deserializeState(const std::string& jsonState) {
    auto findKey = [&](const std::string& key) -> size_t {
        return jsonState.find("\"" + key + "\":");
    };
    
    auto extractNumber = [&](size_t pos) -> int {
        if (pos == std::string::npos) return 0;
        size_t colonPos = jsonState.find(':', pos);
        if (colonPos == std::string::npos) return 0;
        size_t start = colonPos + 1;
        while (start < jsonState.size() && (jsonState[start] == ' ' || jsonState[start] == '\t')) start++;
        std::string numStr;
        while (start < jsonState.size() && (isdigit(jsonState[start]) || jsonState[start] == '-')) {
            numStr += jsonState[start++];
        }
        return numStr.empty() ? 0 : std::stoi(numStr);
    };
    
    totalFishCaught = extractNumber(findKey("totalFishCaught"));
}

int FishingSystem::getTotalFishCaught() const {
    return totalFishCaught;
}

int FishingSystem::getLargestFish(const std::string& species) const {
    auto it = speciesRecords.find(species);
    return it != speciesRecords.end() ? it->second : 0;
}

std::vector<CaughtFish> FishingSystem::getPersonalBests() const {
    std::vector<CaughtFish> bests;
    for (const auto& pair : speciesRecords) {
        CaughtFish fish(pair.first, pair.second, 0, FishingSpot::RIVER);
        bests.push_back(fish);
    }
    return bests;
}

std::string FishingSystem::escapeJson(const std::string& input) const {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\n': output += "\\n"; break;
            default: output += c;
        }
    }
    return output;
}
