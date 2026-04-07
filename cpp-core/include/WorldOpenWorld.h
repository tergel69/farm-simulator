#ifndef WORLD_OPEN_WORLD_H
#define WORLD_OPEN_WORLD_H

#include "World.h"
#include "Biome.h"
#include <memory>
#include <queue>
#include <set>

// ============================================================================
// Save Schema Versioning
// ============================================================================

constexpr int SAVE_SCHEMA_VERSION = 2;  // v1 = farm-only, v2 = open-world biomes

struct SaveVersionInfo {
    int schemaVersion;
    std::string gameVersion;
    int saveTimestamp;
    bool hasBiomes;
    bool hasAutomation;
    bool hasNPCSchedules;
    
    SaveVersionInfo();
    std::string serialize() const;
    void deserialize(const std::string& json);
};

// ============================================================================
// Open World State - Extends base World with biome system
// ============================================================================

class WorldOpenWorld : public World {
public:
    static constexpr int BIOME_GRID_WIDTH = 8;
    static constexpr int BIOME_GRID_HEIGHT = 8;
    
    WorldOpenWorld();
    ~WorldOpenWorld();
    
    static WorldOpenWorld& getInstance();
    
    // Initialization
    void initializeOpenWorld();
    void migrateFromLegacySave(const std::string& legacyJson);
    
    // Biome management
    const BiomeState* getBiomeAt(int gridX, int gridY) const;
    BiomeState* getBiomeAt(int gridX, int gridY);
    const BiomeState* getCurrentBiome() const;
    bool travelToBiome(int gridX, int gridY);
    bool discoverBiome(int gridX, int gridY);
    std::vector<BiomeState*> getLoadedBiomes();
    std::vector<BiomeState*> getAdjacentBiomes(int gridX, int gridY) const;
    
    // Biome streaming
    void updateStreaming();
    void loadBiome(int gridX, int gridY);
    void unloadBiome(int gridX, int gridY);
    bool isBiomeLoaded(int gridX, int gridY) const;
    void setStreamingConfig(const StreamingConfig& config);
    const StreamingConfig& getStreamingConfig() const;
    
    // Weather system (enhanced)
    const WeatherState& getWeatherState() const;
    void setWeatherState(const WeatherState& state);
    void updateWeather(float dt);
    std::string getWeatherDisplayName() const;
    
    // Microclimate access
    const Microclimate& getMicroclimateAt(int gridX, int gridY) const;
    
    // Farm outposts (distributed farming across biomes)
    bool establishOutpost(int biomeGridX, int biomeGridY, int localX, int localY);
    bool removeOutpost(int biomeGridX, int biomeGridY, int localX, int localY);
    std::vector<BiomePosition> getOutpostPositions() const;
    bool hasOutpostInBiome(int gridX, int gridY) const;
    
    // World map & navigation
    std::string getWorldMapJson() const;
    std::string getMinimapDataJson() const;
    bool setWaypoint(int gridX, int gridY, int localX, int localY);
    void clearWaypoint();
    BiomePosition getWaypoint() const;
    float getDistanceToWaypoint() const;
    
    // Time & day-night (enhanced for open world)
    void advanceDayOpenWorld();  // New method, doesn't override base
    float getTimeOfDayFloat() const;  // 0.0 - 1.0 for shader transitions
    std::string getDayNightState() const;
    
    // Serialization (v2 schema)
    std::string serializeStateOpenWorld() const;  // New method
    void deserializeStateOpenWorld(const std::string& jsonState);  // New method
    SaveVersionInfo getSaveVersionInfo() const;
    
    // Event emission (new event families)
    void emitBiomeEvent(const std::string& type, const std::string& data);
    void emitWeatherEvent(const std::string& type, const std::string& data);
    void emitStreamingEvent(const std::string& type, const std::string& data);
    
    // Content queries
    BiomeDefinition getBiomeDefinition(BiomeType type) const;
    std::vector<BiomeDefinition> getAllBiomeDefinitions() const;
    std::vector<std::string> getActivitiesForCurrentBiome() const;
    
    // Test/debug helpers
    void debugUnlockAllBiomes();
    void debugSetWeather(WeatherType type);
    int getLoadedBiomeCount() const;
    int getTotalBiomeCount() const;
    
private:
    // Biome grid (8x8 = 64 biomes max, but we use 8 types distributed)
    std::map<int, std::map<int, BiomeState>> biomeGrid;
    
    // Loaded biomes cache (for streaming)
    std::set<std::pair<int, int>> loadedBiomeSet;
    
    // Weather state
    WeatherState weatherState;
    
    // Streaming configuration
    StreamingConfig streamingConfig;
    
    // Waypoint system
    BiomePosition waypoint;
    bool hasActiveWaypoint;
    
    // Save version tracking
    SaveVersionInfo saveVersion;
    
    // Initialization helpers
    void initializeBiomeGrid();
    void assignBiomeTypesToGrid();
    void migrateV1ToV2();
    float calculateDistance(int x1, int y1, int x2, int y2) const;
    void recalculateLoadPriorities();
};

#endif // WORLD_OPEN_WORLD_H
