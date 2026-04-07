#ifndef WORLD_H
#define WORLD_H

#include "Farm.h"
#include "NPC.h"
#include "Mine.h"
#include "Fishing.h"
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <functional>

enum class RegionType {
    FARM,
    VILLAGE,
    MINE_ENTRANCE,
    FOREST,
    BEACH,
    MOUNTAIN,
    DESERT,
    CAVE,
    LAKE,
    SECRET_AREA
};

enum class Season {
    SPRING,
    SUMMER,
    FALL,
    WINTER
};

struct WorldPosition {
    int regionX;
    int regionY;
    int localX;
    int localY;
    
    bool operator==(const WorldPosition& other) const {
        return regionX == other.regionX && 
               regionY == other.regionY && 
               localX == other.localX && 
               localY == other.localY;
    }
    
    bool operator!=(const WorldPosition& other) const {
        return !(*this == other);
    }
};

struct Region {
    std::string name;
    RegionType type;
    int gridWidth;
    int gridHeight;
    std::string description;
    std::vector<std::string> allowedActivities;
    std::map<std::string, int> spawnRates;
    bool isUnlocked;
    std::string backgroundMusic;
    std::string ambientSound;
    
    Region();
    Region(const std::string& name, RegionType type, int width, int height);
};

struct WorldEvent {
    std::string type;
    std::string data;
    int priority;
    double duration;
};

struct PlayerStats {
    int energy;
    int maxEnergy;
    int health;
    int maxHealth;
    int experience;
    int level;
    int stamina;
    int maxStamina;
    
    PlayerStats();
    void resetDaily();
};

struct TimeOfDay {
    int hour;
    int minute;
    int day;
    int month;
    int year;
    Season season;
    
    TimeOfDay();
    void advanceTime(int minutes);
    void advanceDay();
    std::string getTimeString() const;
    std::string getDateString() const;
    bool isNight() const;
    bool isMorning() const;
    bool isAfternoon() const;
    bool isEvening() const;
};

class World {
public:
    static constexpr int WORLD_WIDTH = 10;
    static constexpr int WORLD_HEIGHT = 10;
    static constexpr int REGION_SIZE = 50;
    
    World();
    ~World();
    
    World(const World&) = delete;
    World& operator=(const World&) = delete;
    
    // Singleton access
    static World& getInstance();
    
    // Initialization
    void initialize();
    
    // Player movement and positioning
    bool movePlayer(int dx, int dy);
    bool teleportToRegion(const std::string& regionName);
    bool teleportToWorldPosition(int worldX, int worldY);
    WorldPosition getPlayerPosition() const;
    std::string getCurrentRegionName() const;
    RegionType getCurrentRegionType() const;
    
    // Region management
    const Region* getRegionAt(int worldX, int worldY) const;
    const Region* getCurrentRegion() const;
    std::vector<std::string> getAdjacentRegions() const;
    bool unlockRegion(const std::string& regionName);
    bool isRegionUnlocked(const std::string& regionName) const;
    
    // Time system
    void updateTime(int minutes);
    void advanceDay();
    const TimeOfDay& getCurrentTime() const;
    void setSeason(Season season);
    Season getCurrentSeason() const;
    
    // Player stats
    const PlayerStats& getPlayerStats() const;
    void modifyEnergy(int amount);
    void modifyHealth(int amount);
    void addExperience(int amount);
    void levelUp();
    
    // Activities
    bool canPerformActivity(const std::string& activity) const;
    bool startActivity(const std::string& activity);
    bool endActivity();
    std::string getCurrentActivity() const;
    
    // Farm integration
    Farm& getFarm();
    const Farm& getFarm() const;
    
    // Mine integration
    Mine& getMine();
    const Mine& getMine() const;
    
    // Fishing integration
    FishingSystem& getFishingSystem();
    const FishingSystem& getFishingSystem() const;
    
    // NPC integration
    NPCManager& getNPCManager();
    const NPCManager& getNPCManager() const;
    
    // World events
    using EventCallback = std::function<void(const std::string& type, const std::string& data)>;
    void registerCallback(const std::string& eventName, EventCallback callback);
    void emitEvent(const std::string& eventName, const std::string& data);
    
    // Serialization
    std::string serializeState() const;
    void deserializeState(const std::string& jsonState);
    
    // World info
    int getTotalRegions() const;
    int getUnlockedRegions() const;
    std::vector<std::string> getAllRegionNames() const;
    std::string getRegionDescription(const std::string& regionName) const;
    
    // Special features
    bool hasWeatherSystem() const { return hasWeather; }
    void enableWeatherSystem(bool enabled);
    std::string getCurrentWeather() const;
    
private:
    std::map<int, std::map<int, Region>> worldGrid;
    std::map<std::string, Region> namedRegions;
    WorldPosition playerPosition;
    TimeOfDay currentTime;
    PlayerStats playerStats;
    std::string currentActivity;
    bool hasWeather;
    std::string currentWeather;
    
    std::unique_ptr<Farm> farm;
    std::unique_ptr<Mine> mine;
    std::unique_ptr<FishingSystem> fishingSystem;
    std::unique_ptr<NPCManager> npcManager;
    
    std::map<std::string, std::vector<EventCallback>> callbacks;
    
    void initializeWorldGrid();
    void initializeDefaultRegions();
    bool isValidMovement(int newX, int newY) const;
    void checkRegionTransitions();
    void updateWeather();
    std::string escapeJson(const std::string& input) const;
};

#endif // WORLD_H
