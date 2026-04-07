#include "World.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <random>

// ============================================================================
// Region Implementation
// ============================================================================

Region::Region() 
    : type(RegionType::FARM), gridWidth(10), gridHeight(10), 
      isUnlocked(false) {}

Region::Region(const std::string& name, RegionType type, int width, int height)
    : name(name), type(type), gridWidth(width), gridHeight(height), 
      isUnlocked(true) {
    
    // Set default descriptions based on region type
    switch (type) {
        case RegionType::FARM:
            description = "Your home farm where you grow crops and raise animals.";
            backgroundMusic = "farm_theme";
            ambientSound = "birds_chirping";
            allowedActivities = {"farming", "watering", "harvesting", "animal_care"};
            break;
        case RegionType::VILLAGE:
            description = "A bustling village with shops, NPCs, and services.";
            backgroundMusic = "village_theme";
            ambientSound = "people_talking";
            allowedActivities = {"shopping", "talking", "questing", "trading"};
            break;
        case RegionType::MINE_ENTRANCE:
            description = "The entrance to the mysterious underground mines.";
            backgroundMusic = "mine_entrance_theme";
            ambientSound = "wind_blowing";
            allowedActivities = {"mining", "exploring", "combat"};
            break;
        case RegionType::FOREST:
            description = "A dense forest filled with forageables and wildlife.";
            backgroundMusic = "forest_theme";
            ambientSound = "forest_ambience";
            allowedActivities = {"foraging", "hunting", "exploring"};
            break;
        case RegionType::BEACH:
            description = "A sandy beach perfect for fishing and relaxation.";
            backgroundMusic = "beach_theme";
            ambientSound = "waves_crashing";
            allowedActivities = {"fishing", "foraging", "swimming"};
            break;
        case RegionType::MOUNTAIN:
            description = "Tall mountains with rare minerals and challenging terrain.";
            backgroundMusic = "mountain_theme";
            ambientSound = "wind_howling";
            allowedActivities = {"climbing", "mining", "foraging"};
            break;
        case RegionType::DESERT:
            description = "A hot desert with unique cacti and ancient ruins.";
            backgroundMusic = "desert_theme";
            ambientSound = "sand_storm";
            allowedActivities = {"exploring", "mining", "foraging"};
            break;
        case RegionType::CAVE:
            description = "Dark caves with dangerous monsters and valuable treasures.";
            backgroundMusic = "cave_theme";
            ambientSound = "dripping_water";
            allowedActivities = {"exploring", "combat", "treasure_hunting"};
            break;
        case RegionType::LAKE:
            description = "A serene lake with excellent fishing opportunities.";
            backgroundMusic = "lake_theme";
            ambientSound = "water_lapping";
            allowedActivities = {"fishing", "swimming", "boating"};
            break;
        case RegionType::SECRET_AREA:
            description = "A hidden area with mysterious secrets.";
            backgroundMusic = "mystery_theme";
            ambientSound = "whispers";
            allowedActivities = {"exploring", "discovering"};
            break;
    }
}

// ============================================================================
// PlayerStats Implementation
// ============================================================================

PlayerStats::PlayerStats() 
    : energy(100), maxEnergy(100), 
      health(100), maxHealth(100),
      experience(0), level(1),
      stamina(50), maxStamina(50) {}

void PlayerStats::resetDaily() {
    energy = maxEnergy;
    stamina = maxStamina;
    // Health doesn't reset automatically unless resting
}

// ============================================================================
// TimeOfDay Implementation
// ============================================================================

TimeOfDay::TimeOfDay() 
    : hour(6), minute(0), day(1), month(1), year(1), season(Season::SPRING) {}

void TimeOfDay::advanceTime(int minutes) {
    minute += minutes;
    while (minute >= 60) {
        minute -= 60;
        hour++;
        if (hour >= 24) {
            hour = 0;
            advanceDay();
        }
    }
}

void TimeOfDay::advanceDay() {
    day++;
    // Simple month system: 30 days per month, 12 months per year
    if (day > 30) {
        day = 1;
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }
    
    // Update season based on month
    if (month >= 3 && month <= 5) {
        season = Season::SPRING;
    } else if (month >= 6 && month <= 8) {
        season = Season::SUMMER;
    } else if (month >= 9 && month <= 11) {
        season = Season::FALL;
    } else {
        season = Season::WINTER;
    }
}

std::string TimeOfDay::getTimeString() const {
    std::ostringstream oss;
    oss << (hour < 10 ? "0" : "") << hour << ":" 
        << (minute < 10 ? "0" : "") << minute;
    return oss.str();
}

std::string TimeOfDay::getDateString() const {
    std::ostringstream oss;
    oss << "Year " << year << ", Month " << month << ", Day " << day;
    return oss.str();
}

bool TimeOfDay::isNight() const {
    return hour >= 20 || hour < 6;
}

bool TimeOfDay::isMorning() const {
    return hour >= 6 && hour < 12;
}

bool TimeOfDay::isAfternoon() const {
    return hour >= 12 && hour < 17;
}

bool TimeOfDay::isEvening() const {
    return hour >= 17 && hour < 20;
}

// ============================================================================
// World Implementation
// ============================================================================

World::World() 
    : hasWeather(true), currentWeather("clear") {
    farm = std::make_unique<Farm>();
    mine = std::make_unique<Mine>();
    fishingSystem = std::make_unique<FishingSystem>();
    npcManager = std::make_unique<NPCManager>();
}

World::~World() {}

World& World::getInstance() {
    static World instance;
    return instance;
}

void World::initialize() {
    initializeWorldGrid();
    initializeDefaultRegions();
    
    // Set player starting position at the farm
    playerPosition.regionX = 4;
    playerPosition.regionY = 4;
    playerPosition.localX = 5;
    playerPosition.localY = 5;
    
    emitEvent("worldInitialized", "{}");
}

void World::initializeWorldGrid() {
    // Create a 10x10 world grid
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int x = 0; x < WORLD_WIDTH; x++) {
            worldGrid[y][x] = Region();
            worldGrid[y][x].isUnlocked = false;
        }
    }
}

void World::initializeDefaultRegions() {
    // Place the farm in the center
    Region farmRegion("Sunny Farm", RegionType::FARM, 50, 50);
    namedRegions["Sunny Farm"] = farmRegion;
    worldGrid[4][4] = farmRegion;
    worldGrid[4][4].isUnlocked = true;
    
    // Village to the north
    Region villageRegion("Harvest Village", RegionType::VILLAGE, 40, 40);
    namedRegions["Harvest Village"] = villageRegion;
    worldGrid[3][4] = villageRegion;
    worldGrid[3][4].isUnlocked = true;
    
    // Mine entrance to the east
    Region mineRegion("Deep Mine Entrance", RegionType::MINE_ENTRANCE, 30, 30);
    namedRegions["Deep Mine"] = mineRegion;
    worldGrid[4][5] = mineRegion;
    worldGrid[4][5].isUnlocked = false; // Must be unlocked
    
    // Forest to the west
    Region forestRegion("Whispering Forest", RegionType::FOREST, 45, 45);
    namedRegions["Whispering Forest"] = forestRegion;
    worldGrid[4][3] = forestRegion;
    worldGrid[4][3].isUnlocked = false;
    
    // Beach to the south
    Region beachRegion("Coral Beach", RegionType::BEACH, 35, 50);
    namedRegions["Coral Beach"] = beachRegion;
    worldGrid[5][4] = beachRegion;
    worldGrid[5][4].isUnlocked = false;
    
    // Lake to the northeast
    Region lakeRegion("Crystal Lake", RegionType::LAKE, 30, 30);
    namedRegions["Crystal Lake"] = lakeRegion;
    worldGrid[3][5] = lakeRegion;
    worldGrid[3][5].isUnlocked = false;
    
    // Mountain to the southeast
    Region mountainRegion("Thunder Peak", RegionType::MOUNTAIN, 40, 40);
    namedRegions["Thunder Peak"] = mountainRegion;
    worldGrid[5][5] = mountainRegion;
    worldGrid[5][5].isUnlocked = false;
    
    // Desert to the southwest
    Region desertRegion("Golden Desert", RegionType::DESERT, 35, 35);
    namedRegions["Golden Desert"] = desertRegion;
    worldGrid[5][3] = desertRegion;
    worldGrid[5][3].isUnlocked = false;
    
    // Secret area (very hard to find)
    Region secretRegion("Ancient Ruins", RegionType::SECRET_AREA, 20, 20);
    namedRegions["Ancient Ruins"] = secretRegion;
    worldGrid[2][2] = secretRegion;
    worldGrid[2][2].isUnlocked = false;
}

bool World::movePlayer(int dx, int dy) {
    int newX = playerPosition.regionX + dx;
    int newY = playerPosition.regionY + dy;
    
    if (!isValidMovement(newX, newY)) {
        return false;
    }
    
    playerPosition.regionX = newX;
    playerPosition.regionY = newY;
    
    // Check for region transitions
    checkRegionTransitions();
    
    emitEvent("playerMoved", 
        "{\"x\":" + std::to_string(playerPosition.regionX) + 
        ",\"y\":" + std::to_string(playerPosition.regionY) + "}");
    
    return true;
}

bool World::teleportToRegion(const std::string& regionName) {
    auto it = namedRegions.find(regionName);
    if (it == namedRegions.end()) {
        return false;
    }
    
    if (!it->second.isUnlocked) {
        return false;
    }
    
    // Find the region in the grid
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int x = 0; x < WORLD_WIDTH; x++) {
            if (worldGrid[y][x].name == regionName) {
                playerPosition.regionX = x;
                playerPosition.regionY = y;
                playerPosition.localX = 5; // Center of region
                playerPosition.localY = 5;
                
                emitEvent("teleported", "{\"region\":\"" + escapeJson(regionName) + "\"}");
                return true;
            }
        }
    }
    
    return false;
}

bool World::teleportToWorldPosition(int worldX, int worldY) {
    if (worldX < 0 || worldX >= WORLD_WIDTH || 
        worldY < 0 || worldY >= WORLD_HEIGHT) {
        return false;
    }
    
    if (!worldGrid[worldY][worldX].isUnlocked) {
        return false;
    }
    
    playerPosition.regionX = worldX;
    playerPosition.regionY = worldY;
    playerPosition.localX = 5;
    playerPosition.localY = 5;
    
    return true;
}

WorldPosition World::getPlayerPosition() const {
    return playerPosition;
}

std::string World::getCurrentRegionName() const {
    const Region* region = getCurrentRegion();
    return region ? region->name : "Unknown";
}

RegionType World::getCurrentRegionType() const {
    const Region* region = getCurrentRegion();
    return region ? region->type : RegionType::FARM;
}

const Region* World::getRegionAt(int worldX, int worldY) const {
    if (worldX < 0 || worldX >= WORLD_WIDTH || 
        worldY < 0 || worldY >= WORLD_HEIGHT) {
        return nullptr;
    }
    
    auto yIt = worldGrid.find(worldY);
    if (yIt == worldGrid.end()) {
        return nullptr;
    }
    
    auto xIt = yIt->second.find(worldX);
    if (xIt == yIt->second.end()) {
        return nullptr;
    }
    
    return &xIt->second;
}

const Region* World::getCurrentRegion() const {
    return getRegionAt(playerPosition.regionX, playerPosition.regionY);
}

std::vector<std::string> World::getAdjacentRegions() const {
    std::vector<std::string> adjacent;
    
    int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    for (auto& dir : dirs) {
        int newX = playerPosition.regionX + dir[0];
        int newY = playerPosition.regionY + dir[1];
        
        if (newX >= 0 && newX < WORLD_WIDTH && 
            newY >= 0 && newY < WORLD_HEIGHT) {
            const Region* region = getRegionAt(newX, newY);
            if (region && region->isUnlocked) {
                adjacent.push_back(region->name);
            }
        }
    }
    
    return adjacent;
}

bool World::unlockRegion(const std::string& regionName) {
    auto it = namedRegions.find(regionName);
    if (it == namedRegions.end()) {
        return false;
    }
    
    it->second.isUnlocked = true;
    
    // Also update in grid
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int x = 0; x < WORLD_WIDTH; x++) {
            if (worldGrid[y][x].name == regionName) {
                worldGrid[y][x].isUnlocked = true;
            }
        }
    }
    
    emitEvent("regionUnlocked", "{\"region\":\"" + escapeJson(regionName) + "\"}");
    return true;
}

bool World::isRegionUnlocked(const std::string& regionName) const {
    auto it = namedRegions.find(regionName);
    if (it == namedRegions.end()) {
        return false;
    }
    return it->second.isUnlocked;
}

void World::updateTime(int minutes) {
    currentTime.advanceTime(minutes);
    
    // Update weather periodically
    if (currentTime.minute % 30 == 0) {
        updateWeather();
    }
    
    // Emit time event every hour
    if (currentTime.minute < minutes % 60) {
        emitEvent("timeAdvanced", 
            "{\"hour\":" + std::to_string(currentTime.hour) + 
            ",\"season\":\"" + std::to_string(static_cast<int>(currentTime.season)) + "\"}");
    }
}

void World::advanceDay() {
    currentTime.advanceDay();
    playerStats.resetDaily();
    
    // Update farm season based on world season
    // Note: Farm has its own season system, we just sync them
    int farmSeason = static_cast<int>(currentTime.season);
    
    emitEvent("dayAdvanced", 
        "{\"day\":" + std::to_string(currentTime.day) + 
        ",\"month\":" + std::to_string(currentTime.month) + 
        ",\"season\":" + std::to_string(static_cast<int>(currentTime.season)) + "}");
}

const TimeOfDay& World::getCurrentTime() const {
    return currentTime;
}

void World::setSeason(Season season) {
    currentTime.season = season;
}

Season World::getCurrentSeason() const {
    return currentTime.season;
}

const PlayerStats& World::getPlayerStats() const {
    return playerStats;
}

void World::modifyEnergy(int amount) {
    playerStats.energy = std::max(0, std::min(playerStats.maxEnergy, 
                                               playerStats.energy + amount));
}

void World::modifyHealth(int amount) {
    playerStats.health = std::max(0, std::min(playerStats.maxHealth, 
                                              playerStats.health + amount));
}

void World::addExperience(int amount) {
    playerStats.experience += amount;
    
    // Level up every 100 XP
    int newLevel = 1 + (playerStats.experience / 100);
    if (newLevel > playerStats.level) {
        playerStats.level = newLevel;
        emitEvent("levelUp", "{\"level\":" + std::to_string(playerStats.level) + "}");
    }
}

void World::levelUp() {
    playerStats.level++;
    playerStats.maxEnergy += 10;
    playerStats.maxHealth += 5;
    playerStats.maxStamina += 5;
    playerStats.energy = playerStats.maxEnergy;
    playerStats.health = playerStats.maxHealth;
    playerStats.stamina = playerStats.maxStamina;
    
    emitEvent("levelUp", "{\"level\":" + std::to_string(playerStats.level) + "}");
}

bool World::canPerformActivity(const std::string& activity) const {
    const Region* region = getCurrentRegion();
    if (!region || !region->isUnlocked) {
        return false;
    }
    
    if (!currentActivity.empty()) {
        return false; // Already doing an activity
    }
    
    auto it = std::find(region->allowedActivities.begin(), 
                       region->allowedActivities.end(), 
                       activity);
    return it != region->allowedActivities.end();
}

bool World::startActivity(const std::string& activity) {
    if (!canPerformActivity(activity)) {
        return false;
    }
    
    currentActivity = activity;
    emitEvent("activityStarted", "{\"activity\":\"" + escapeJson(activity) + "\"}");
    return true;
}

bool World::endActivity() {
    if (currentActivity.empty()) {
        return false;
    }
    
    std::string completedActivity = currentActivity;
    currentActivity.clear();
    
    emitEvent("activityEnded", "{\"activity\":\"" + escapeJson(completedActivity) + "\"}");
    return true;
}

std::string World::getCurrentActivity() const {
    return currentActivity;
}

Farm& World::getFarm() {
    return *farm;
}

const Farm& World::getFarm() const {
    return *farm;
}

Mine& World::getMine() {
    return *mine;
}

const Mine& World::getMine() const {
    return *mine;
}

FishingSystem& World::getFishingSystem() {
    return *fishingSystem;
}

const FishingSystem& World::getFishingSystem() const {
    return *fishingSystem;
}

NPCManager& World::getNPCManager() {
    return *npcManager;
}

const NPCManager& World::getNPCManager() const {
    return *npcManager;
}

void World::registerCallback(const std::string& eventName, EventCallback callback) {
    callbacks[eventName].push_back(callback);
}

void World::emitEvent(const std::string& eventName, const std::string& data) {
    auto it = callbacks.find(eventName);
    if (it != callbacks.end()) {
        for (auto& callback : it->second) {
            callback(eventName, data);
        }
    }
}

std::string World::serializeState() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"playerPosition\":{\"x\":" << playerPosition.regionX 
        << ",\"y\":" << playerPosition.regionY << "},";
    oss << "\"currentTime\":{\"hour\":" << currentTime.hour 
        << ",\"minute\":" << currentTime.minute
        << ",\"day\":" << currentTime.day
        << ",\"month\":" << currentTime.month
        << ",\"year\":" << currentTime.year
        << ",\"season\":" << static_cast<int>(currentTime.season) << "},";
    oss << "\"playerStats\":{\"energy\":" << playerStats.energy
        << ",\"health\":" << playerStats.health
        << ",\"experience\":" << playerStats.experience
        << ",\"level\":" << playerStats.level << "},";
    oss << "\"currentRegion\":\"" << escapeJson(getCurrentRegionName()) << "\",";
    oss << "\"weather\":\"" << escapeJson(currentWeather) << "\",";
    oss << "\"farm\":" << farm->serializeState();
    oss << "}";
    return oss.str();
}

void World::deserializeState(const std::string& jsonState) {
    // Simplified deserialization - would need proper JSON parser in production
    // This is a placeholder for demonstration
    emitEvent("stateLoaded", "{}");
}

int World::getTotalRegions() const {
    return namedRegions.size();
}

int World::getUnlockedRegions() const {
    int count = 0;
    for (const auto& pair : namedRegions) {
        if (pair.second.isUnlocked) {
            count++;
        }
    }
    return count;
}

std::vector<std::string> World::getAllRegionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : namedRegions) {
        names.push_back(pair.first);
    }
    return names;
}

std::string World::getRegionDescription(const std::string& regionName) const {
    auto it = namedRegions.find(regionName);
    if (it == namedRegions.end()) {
        return "";
    }
    return it->second.description;
}

void World::enableWeatherSystem(bool enabled) {
    hasWeather = enabled;
    if (!enabled) {
        currentWeather = "clear";
    }
}

std::string World::getCurrentWeather() const {
    return currentWeather;
}

bool World::isValidMovement(int newX, int newY) const {
    if (newX < 0 || newX >= WORLD_WIDTH || 
        newY < 0 || newY >= WORLD_HEIGHT) {
        return false;
    }
    
    auto yIt = worldGrid.find(newY);
    if (yIt == worldGrid.end()) {
        return false;
    }
    
    auto xIt = yIt->second.find(newX);
    if (xIt == yIt->second.end()) {
        return false;
    }
    
    return xIt->second.isUnlocked;
}

void World::checkRegionTransitions() {
    const Region* region = getCurrentRegion();
    if (region) {
        emitEvent("regionEntered", 
            "{\"region\":\"" + escapeJson(region->name) + 
            "\",\"type\":" + std::to_string(static_cast<int>(region->type)) + "}");
    }
}

void World::updateWeather() {
    if (!hasWeather) {
        currentWeather = "clear";
        return;
    }
    
    // Simple weather system based on season and randomness
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);
    
    int roll = dis(gen);
    
    switch (currentTime.season) {
        case Season::SPRING:
            if (roll < 40) currentWeather = "clear";
            else if (roll < 70) currentWeather = "cloudy";
            else if (roll < 90) currentWeather = "rainy";
            else currentWeather = "stormy";
            break;
        case Season::SUMMER:
            if (roll < 60) currentWeather = "clear";
            else if (roll < 80) currentWeather = "sunny";
            else if (roll < 95) currentWeather = "thunderstorm";
            else currentWeather = "hot";
            break;
        case Season::FALL:
            if (roll < 50) currentWeather = "clear";
            else if (roll < 75) currentWeather = "cloudy";
            else if (roll < 90) currentWeather = "windy";
            else currentWeather = "foggy";
            break;
        case Season::WINTER:
            if (roll < 30) currentWeather = "clear";
            else if (roll < 60) currentWeather = "snowy";
            else if (roll < 85) currentWeather = "blizzard";
            else currentWeather = "freezing";
            break;
    }
    
    emitEvent("weatherChanged", "{\"weather\":\"" + escapeJson(currentWeather) + "\"}");
}

std::string World::escapeJson(const std::string& input) const {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += c;
        }
    }
    return output;
}
