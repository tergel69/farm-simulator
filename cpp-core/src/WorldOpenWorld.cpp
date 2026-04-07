#include "WorldOpenWorld.h"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iostream>

// ============================================================================
// SaveVersionInfo Implementation
// ============================================================================

SaveVersionInfo::SaveVersionInfo()
    : schemaVersion(SAVE_SCHEMA_VERSION), gameVersion("1.0.0"),
      saveTimestamp(0), hasBiomes(false), hasAutomation(false), 
      hasNPCSchedules(false) {}

std::string SaveVersionInfo::serialize() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"schemaVersion\":" << schemaVersion << ",";
    oss << "\"gameVersion\":\"" << gameVersion << "\",";
    oss << "\"saveTimestamp\":" << saveTimestamp << ",";
    oss << "\"hasBiomes\":" << (hasBiomes ? "true" : "false") << ",";
    oss << "\"hasAutomation\":" << (hasAutomation ? "true" : "false") << ",";
    oss << "\"hasNPCSchedules\":" << (hasNPCSchedules ? "true" : "false");
    oss << "}";
    return oss.str();
}

void SaveVersionInfo::deserialize(const std::string& json) {
    // Simplified - production would parse JSON properly
    schemaVersion = SAVE_SCHEMA_VERSION;
    hasBiomes = true;
}

// ============================================================================
// WorldOpenWorld Implementation
// ============================================================================

WorldOpenWorld::WorldOpenWorld() : hasActiveWaypoint(false) {
    saveVersion.schemaVersion = SAVE_SCHEMA_VERSION;
    saveVersion.hasBiomes = true;
}

WorldOpenWorld::~WorldOpenWorld() {}

WorldOpenWorld& WorldOpenWorld::getInstance() {
    static WorldOpenWorld instance;
    return instance;
}

void WorldOpenWorld::initializeOpenWorld() {
    // Call base initialization first
    World::initialize();
    
    initializeBiomeGrid();
    assignBiomeTypesToGrid();
    
    // Starter Valley is always unlocked at start
    discoverBiome(3, 3);  // Center of map
    
    emitEvent("openWorldInitialized", "{}");
    emitBiomeEvent("biomeDiscovered", "{\"biome\":\"starter_valley\",\"gridX\":3,\"gridY\":3}");
}

void WorldOpenWorld::initializeBiomeGrid() {
    // Create 8x8 biome grid
    for (int y = 0; y < BIOME_GRID_HEIGHT; y++) {
        for (int x = 0; x < BIOME_GRID_WIDTH; x++) {
            BiomeState defaultBiome;
            defaultBiome.gridX = x;
            defaultBiome.gridY = y;
            biomeGrid[y][x] = defaultBiome;
        }
    }
}

void WorldOpenWorld::assignBiomeTypesToGrid() {
    // Distribute 8 biome types across the grid in a logical pattern
    // Starter Valley in center, surrounded by easier biomes, harder ones at edges
    
    struct BiomePlacement {
        int x, y;
        BiomeType type;
        std::string name;
    };
    
    std::vector<BiomePlacement> placements = {
        // Center - Starter Valley (player starts here)
        {3, 3, BiomeType::STARTER_VALLEY, "Sunny Meadow"},
        {3, 4, BiomeType::STARTER_VALLEY, "Green Pastures"},
        {4, 3, BiomeType::STARTER_VALLEY, "Brook Crossing"},
        {4, 4, BiomeType::STARTER_VALLEY, "Home Valley"},
        
        // North - Village/Town Hub
        {2, 3, BiomeType::VILLAGE_TOWN, "Harbor Town"},
        {2, 4, BiomeType::VILLAGE_TOWN, "Market Square"},
        
        // West - Pine Forest
        {3, 2, BiomeType::PINE_FOREST, "Whispering Pines"},
        {4, 2, BiomeType::PINE_FOREST, "Deep Woods"},
        {3, 1, BiomeType::PINE_FOREST, "Ancient Grove"},
        
        // East - Riverlands
        {3, 5, BiomeType::RIVERLANDS, "Silver River"},
        {4, 5, BiomeType::RIVERLANDS, "Reed Marshes"},
        {3, 6, BiomeType::RIVERLANDS, "Mill Stream"},
        
        // South - Coastline Harbor
        {5, 3, BiomeType::COASTLINE_HARBOR, "Coral Bay"},
        {5, 4, BiomeType::COASTLINE_HARBOR, "Lighthouse Point"},
        
        // Northwest - Highlands
        {2, 2, BiomeType::HIGHLANDS, "Wind Swept Hills"},
        {1, 2, BiomeType::HIGHLANDS, "Eagle's Rest"},
        
        // Northeast - Canyon/Badlands
        {2, 5, BiomeType::CANYON_BADLANDS, "Red Rock Canyon"},
        {1, 5, BiomeType::CANYON_BADLANDS, "Scorched Earth"},
        
        // Southwest - Mist Marsh (late game)
        {5, 2, BiomeType::MIST_MARSH, "Veiled Mire"},
        {6, 2, BiomeType::MIST_MARSH, "Spirit Fen"},
        
        // Southeast - More content
        {5, 5, BiomeType::RIVERLANDS, "Delta Crossing"},
        {6, 4, BiomeType::PINE_FOREST, "Coastal Woods"},
    };
    
    for (const auto& placement : placements) {
        if (placement.x >= 0 && placement.x < BIOME_GRID_WIDTH &&
            placement.y >= 0 && placement.y < BIOME_GRID_HEIGHT) {
            
            BiomeState biome(placement.type, placement.name, placement.x, placement.y);
            
            // Starter biomes are discovered initially
            if (placement.type == BiomeType::STARTER_VALLEY || 
                placement.type == BiomeType::VILLAGE_TOWN) {
                biome.isDiscovered = true;
                biome.isUnlocked = true;
            } else {
                biome.isDiscovered = false;
                biome.isUnlocked = false;
            }
            
            biomeGrid[placement.y][placement.x] = biome;
        }
    }
}

void WorldOpenWorld::migrateFromLegacySave(const std::string& legacyJson) {
    // Initialize open world first
    initializeOpenWorld();
    
    // Load legacy farm data into the starter valley
    migrateV1ToV2();
    
    emitEvent("saveMigrated", "{\"fromVersion\":1,\"toVersion\":2}");
}

void WorldOpenWorld::migrateV1ToV2() {
    // Transfer farm state from base World to Starter Valley biome
    // This is called during migration from v1 saves
    saveVersion.schemaVersion = 2;
    saveVersion.hasBiomes = true;
}

const BiomeState* WorldOpenWorld::getBiomeAt(int gridX, int gridY) const {
    if (gridX < 0 || gridX >= BIOME_GRID_WIDTH ||
        gridY < 0 || gridY >= BIOME_GRID_HEIGHT) {
        return nullptr;
    }
    
    auto yIt = biomeGrid.find(gridY);
    if (yIt == biomeGrid.end()) return nullptr;
    
    auto xIt = yIt->second.find(gridX);
    if (xIt == yIt->second.end()) return nullptr;
    
    return &xIt->second;
}

BiomeState* WorldOpenWorld::getBiomeAt(int gridX, int gridY) {
    return const_cast<BiomeState*>(
        const_cast<const WorldOpenWorld*>(this)->getBiomeAt(gridX, gridY));
}

const BiomeState* WorldOpenWorld::getCurrentBiome() const {
    WorldPosition pos = getPlayerPosition();
    // Convert world position to biome grid position
    int biomeX = pos.regionX / 5;  // Scale factor
    int biomeY = pos.regionY / 5;
    return getBiomeAt(biomeX, biomeY);
}

bool WorldOpenWorld::travelToBiome(int gridX, int gridY) {
    const BiomeState* biome = getBiomeAt(gridX, gridY);
    if (!biome || !biome->isUnlocked) {
        return false;
    }
    
    // Load the target biome if not already loaded
    if (!isBiomeLoaded(gridX, gridY)) {
        loadBiome(gridX, gridY);
    }
    
    // Teleport player to this biome
    int worldX = gridX * 5 + 2;  // Center of biome in world coords
    int worldY = gridY * 5 + 2;
    
    teleportToWorldPosition(worldX, worldY);
    
    emitBiomeEvent("biomeEntered", 
        "{\"biome\":\"" + biomeTypeToString(biome->type) + 
        "\",\"gridX\":" + std::to_string(gridX) +
        ",\"gridY\":" + std::to_string(gridY) + "}");
    
    return true;
}

bool WorldOpenWorld::discoverBiome(int gridX, int gridY) {
    BiomeState* biome = getBiomeAt(gridX, gridY);
    if (!biome) return false;
    
    if (!biome->isDiscovered) {
        biome->isDiscovered = true;
        biome->isUnlocked = true;
        biome->discoveryTimestamp = static_cast<int>(getCurrentTime().hour * 60 + getCurrentTime().minute);
        
        emitBiomeEvent("biomeDiscovered",
            "{\"biome\":\"" + biomeTypeToString(biome->type) + 
            "\",\"name\":\"" + biome->name + "\"}");
    }
    
    return true;
}

std::vector<BiomeState*> WorldOpenWorld::getLoadedBiomes() {
    std::vector<BiomeState*> loaded;
    for (auto& pair : loadedBiomeSet) {
        BiomeState* biome = getBiomeAt(pair.first, pair.second);
        if (biome) {
            biome->isLoaded = true;
            loaded.push_back(biome);
        }
    }
    return loaded;
}

std::vector<BiomeState*> WorldOpenWorld::getAdjacentBiomes(int gridX, int gridY) const {
    std::vector<BiomeState*> adjacent;
    
    int dirs[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1}};
    
    for (auto& dir : dirs) {
        int newX = gridX + dir[0];
        int newY = gridY + dir[1];
        
        const BiomeState* biome = getBiomeAt(newX, newY);
        if (biome && biome->isDiscovered) {
            adjacent.push_back(const_cast<BiomeState*>(biome));
        }
    }
    
    return adjacent;
}

void WorldOpenWorld::updateStreaming() {
    WorldPosition playerPos = getPlayerPosition();
    int playerBiomeX = playerPos.regionX / 5;
    int playerBiomeY = playerPos.regionY / 5;
    
    recalculateLoadPriorities();
    
    // Unload distant biomes
    std::vector<std::pair<int, int>> toUnload;
    for (const auto& pair : loadedBiomeSet) {
        float dist = calculateDistance(playerBiomeX, playerBiomeY, pair.first, pair.second);
        if (dist > streamingConfig.unloadDistance / 50.0f) {
            toUnload.push_back(pair);
        }
    }
    
    for (const auto& pair : toUnload) {
        unloadBiome(pair.first, pair.second);
    }
    
    // Load nearby biomes
    for (int y = 0; y < BIOME_GRID_HEIGHT; y++) {
        for (int x = 0; x < BIOME_GRID_WIDTH; x++) {
            const BiomeState* biome = getBiomeAt(x, y);
            if (!biome || !biome->isDiscovered) continue;
            
            float dist = calculateDistance(playerBiomeX, playerBiomeY, x, y);
            if (dist < streamingConfig.loadDistance / 50.0f && !isBiomeLoaded(x, y)) {
                loadBiome(x, y);
            }
        }
    }
}

void WorldOpenWorld::loadBiome(int gridX, int gridY) {
    BiomeState* biome = getBiomeAt(gridX, gridY);
    if (!biome || biome->isLoaded) return;
    
    // Check if we're at max capacity
    if (static_cast<int>(loadedBiomeSet.size()) >= streamingConfig.maxLoadedBiomes) {
        // Unload lowest priority biome
        float lowestPriority = 999999.0f;
        int unloadX = -1, unloadY = -1;
        
        for (const auto& pair : loadedBiomeSet) {
            BiomeState* checkBiome = getBiomeAt(pair.first, pair.second);
            if (checkBiome && checkBiome->loadPriority < lowestPriority) {
                lowestPriority = checkBiome->loadPriority;
                unloadX = pair.first;
                unloadY = pair.second;
            }
        }
        
        if (unloadX >= 0) {
            unloadBiome(unloadX, unloadY);
        }
    }
    
    biome->isLoaded = true;
    loadedBiomeSet.insert({gridX, gridY});
    
    emitStreamingEvent("biomeLoaded",
        "{\"biome\":\"" + biomeTypeToString(biome->type) + 
        "\",\"gridX\":" + std::to_string(gridX) +
        ",\"gridY\":" + std::to_string(gridY) + "}");
}

void WorldOpenWorld::unloadBiome(int gridX, int gridY) {
    BiomeState* biome = getBiomeAt(gridX, gridY);
    if (!biome) return;
    
    biome->isLoaded = false;
    biome->isActive = false;
    loadedBiomeSet.erase({gridX, gridY});
    
    emitStreamingEvent("biomeUnloaded",
        "{\"gridX\":" + std::to_string(gridX) +
        ",\"gridY\":" + std::to_string(gridY) + "}");
}

bool WorldOpenWorld::isBiomeLoaded(int gridX, int gridY) const {
    return loadedBiomeSet.count({gridX, gridY}) > 0;
}

void WorldOpenWorld::setStreamingConfig(const StreamingConfig& config) {
    streamingConfig = config;
}

const StreamingConfig& WorldOpenWorld::getStreamingConfig() const {
    return streamingConfig;
}

const WeatherState& WorldOpenWorld::getWeatherState() const {
    return weatherState;
}

void WorldOpenWorld::setWeatherState(const WeatherState& state) {
    weatherState = state;
    emitWeatherEvent("weatherSet", weatherState.serialize());
}

void WorldOpenWorld::updateWeather(float dt) {
    weatherState.update(dt);
    
    // Update microclimates in loaded biomes
    for (const auto& pair : loadedBiomeSet) {
        BiomeState* biome = getBiomeAt(pair.first, pair.second);
        if (biome) {
            // Apply weather effects to microclimate
            float weatherHumidity = 0.5f;
            if (weatherState.current == WeatherType::RAINY || 
                weatherState.current == WeatherType::THUNDERSTORM) {
                weatherHumidity = 0.9f;
            } else if (weatherState.current == WeatherType::CLEAR ||
                       weatherState.current == WeatherType::SUNNY) {
                weatherHumidity = 0.3f;
            }
            
            biome->microclimate.update(biome->microclimate.temperature, weatherHumidity, dt);
        }
    }
    
    // Emit weather change events when weather transitions
    if (weatherState.transitionProgress < 0.1f && weatherState.previous != weatherState.current) {
        emitWeatherEvent("weatherChanged", weatherState.serialize());
    }
}

std::string WorldOpenWorld::getWeatherDisplayName() const {
    return weatherTypeToString(weatherState.current);
}

const Microclimate& WorldOpenWorld::getMicroclimateAt(int gridX, int gridY) const {
    const BiomeState* biome = getBiomeAt(gridX, gridY);
    static Microclimate defaultClimate;  // Fallback
    return biome ? biome->microclimate : defaultClimate;
}

bool WorldOpenWorld::establishOutpost(int biomeGridX, int biomeGridY, int localX, int localY) {
    BiomeState* biome = getBiomeAt(biomeGridX, biomeGridY);
    if (!biome || !biome->isUnlocked) return false;
    
    // Encode position as single integer (simple packing)
    int packedPos = (localX << 16) | localY;
    
    // Check if already exists
    for (int pos : biome->outpostPositions) {
        if (pos == packedPos) return false;  // Already established
    }
    
    biome->outpostPositions.push_back(packedPos);
    
    emitBiomeEvent("outpostEstablished",
        "{\"biome\":\"" + biomeTypeToString(biome->type) + 
        "\",\"gridX\":" + std::to_string(biomeGridX) +
        ",\"gridY\":" + std::to_string(biomeGridY) +
        ",\"localX\":" + std::to_string(localX) +
        ",\"localY\":" + std::to_string(localY) + "}");
    
    return true;
}

bool WorldOpenWorld::removeOutpost(int biomeGridX, int biomeGridY, int localX, int localY) {
    BiomeState* biome = getBiomeAt(biomeGridX, biomeGridY);
    if (!biome) return false;
    
    int packedPos = (localX << 16) | localY;
    
    auto it = std::find(biome->outpostPositions.begin(), 
                        biome->outpostPositions.end(), packedPos);
    if (it != biome->outpostPositions.end()) {
        biome->outpostPositions.erase(it);
        
        emitBiomeEvent("outpostRemoved",
            "{\"gridX\":" + std::to_string(biomeGridX) +
            ",\"gridY\":" + std::to_string(biomeGridY) + "}");
        
        return true;
    }
    
    return false;
}

std::vector<BiomePosition> WorldOpenWorld::getOutpostPositions() const {
    std::vector<BiomePosition> positions;
    
    for (int y = 0; y < BIOME_GRID_HEIGHT; y++) {
        for (int x = 0; x < BIOME_GRID_WIDTH; x++) {
            const BiomeState* biome = getBiomeAt(x, y);
            if (!biome) continue;
            
            for (int packed : biome->outpostPositions) {
                BiomePosition pos;
                pos.biomeGridX = x;
                pos.biomeGridY = y;
                pos.localX = (packed >> 16) & 0xFFFF;
                pos.localY = packed & 0xFFFF;
                positions.push_back(pos);
            }
        }
    }
    
    return positions;
}

bool WorldOpenWorld::hasOutpostInBiome(int gridX, int gridY) const {
    const BiomeState* biome = getBiomeAt(gridX, gridY);
    return biome && !biome->outpostPositions.empty();
}

std::string WorldOpenWorld::getWorldMapJson() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"width\":" << BIOME_GRID_WIDTH << ",";
    oss << "\"height\":" << BIOME_GRID_HEIGHT << ",";
    oss << "\"biomes\":[";
    
    bool first = true;
    for (int y = 0; y < BIOME_GRID_HEIGHT; y++) {
        for (int x = 0; x < BIOME_GRID_WIDTH; x++) {
            const BiomeState* biome = getBiomeAt(x, y);
            if (!biome) continue;
            
            if (!first) oss << ",";
            oss << "{";
            oss << "\"gridX\":" << x << ",";
            oss << "\"gridY\":" << y << ",";
            oss << "\"type\":\"" << biomeTypeToString(biome->type) << "\",";
            oss << "\"name\":\"" << biome->name << "\",";
            oss << "\"isDiscovered\":" << (biome->isDiscovered ? "true" : "false") << ",";
            oss << "\"isUnlocked\":" << (biome->isUnlocked ? "true" : "false") << ",";
            oss << "\"isLoaded\":" << (biome->isLoaded ? "true" : "false");
            oss << "}";
            first = false;
        }
    }
    
    oss << "]";
    oss << "}";
    return oss.str();
}

std::string WorldOpenWorld::getMinimapDataJson() const {
    std::ostringstream oss;
    oss << "{";
    
    // Player position
    WorldPosition pos = getPlayerPosition();
    oss << "\"player\":{\"x\":" << pos.regionX << ",\"y\":" << pos.regionY << "},";
    
    // Waypoint
    if (hasActiveWaypoint) {
        oss << "\"waypoint\":" << waypoint.serialize() << ",";
    }
    
    // Adjacent biomes
    int biomeX = pos.regionX / 5;
    int biomeY = pos.regionY / 5;
    
    oss << "\"adjacent\":[";
    auto adjacent = const_cast<WorldOpenWorld*>(this)->getAdjacentBiomes(biomeX, biomeY);
    bool first = true;
    for (const BiomeState* biome : adjacent) {
        if (!first) oss << ",";
        oss << "{\"type\":\"" << biomeTypeToString(biome->type) << 
               "\",\"name\":\"" << biome->name << "\"}";
        first = false;
    }
    oss << "]";
    
    oss << "}";
    return oss.str();
}

bool WorldOpenWorld::setWaypoint(int gridX, int gridY, int localX, int localY) {
    const BiomeState* biome = getBiomeAt(gridX, gridY);
    if (!biome || !biome->isDiscovered) return false;
    
    waypoint.biomeGridX = gridX;
    waypoint.biomeGridY = gridY;
    waypoint.localX = localX;
    waypoint.localY = localY;
    hasActiveWaypoint = true;
    
    emitEvent("waypointSet", waypoint.serialize());
    return true;
}

void WorldOpenWorld::clearWaypoint() {
    hasActiveWaypoint = false;
    emitEvent("waypointCleared", "{}");
}

BiomePosition WorldOpenWorld::getWaypoint() const {
    return waypoint;
}

float WorldOpenWorld::getDistanceToWaypoint() const {
    if (!hasActiveWaypoint) return -1.0f;
    
    WorldPosition pos = getPlayerPosition();
    int playerBiomeX = pos.regionX / 5;
    int playerBiomeY = pos.regionY / 5;
    
    return calculateDistance(playerBiomeX, playerBiomeY, 
                            waypoint.biomeGridX, waypoint.biomeGridY);
}

void WorldOpenWorld::advanceDayOpenWorld() {
    World::advanceDay();
    
    // Update weather for new day
    updateWeather(0.0f);
    
    // Resource respawns in biomes
    for (auto& pair : biomeGrid) {
        for (auto& biomePair : pair.second) {
            BiomeState& biome = biomePair.second;
            if (biome.isUnlocked) {
                // Decrement respawn timers
                for (auto& timerPair : biome.resourceRespawnTimers) {
                    if (timerPair.second > 0) {
                        timerPair.second -= 1.0f;  // 1 day
                    }
                }
            }
        }
    }
}

float WorldOpenWorld::getTimeOfDayFloat() const {
    const TimeOfDay& time = getCurrentTime();
    return (time.hour * 60.0f + time.minute) / (24.0f * 60.0f);
}

std::string WorldOpenWorld::getDayNightState() const {
    float t = getTimeOfDayFloat();
    
    if (t < 0.25f) return "night";      // 0:00 - 6:00
    if (t < 0.35f) return "dawn";       // 6:00 - 8:24
    if (t < 0.5f) return "morning";     // 8:24 - 12:00
    if (t < 0.6f) return "afternoon";   // 12:00 - 14:24
    if (t < 0.75f) return "evening";    // 14:24 - 18:00
    if (t < 0.85f) return "dusk";       // 18:00 - 20:24
    return "night";                     // 20:24 - 24:00
}

std::string WorldOpenWorld::serializeStateOpenWorld() const {
    // V2 schema with biome support
    std::ostringstream oss;
    oss << "{";
    oss << "\"schemaVersion\":" << SAVE_SCHEMA_VERSION << ",";
    oss << "\"versionInfo\":" << saveVersion.serialize() << ",";
    
    // Base world state
    oss << "\"baseWorld\":" << World::serializeState() << ",";
    
    // Weather state
    oss << "\"weather\":" << weatherState.serialize() << ",";
    
    // Biome states (only discovered ones)
    oss << "\"biomes\":[";
    bool first = true;
    for (int y = 0; y < BIOME_GRID_HEIGHT; y++) {
        for (int x = 0; x < BIOME_GRID_WIDTH; x++) {
            const BiomeState* biome = getBiomeAt(x, y);
            if (!biome || !biome->isDiscovered) continue;
            
            if (!first) oss << ",";
            oss << biome->serialize();
            first = false;
        }
    }
    oss << "],";
    
    // Waypoint
    if (hasActiveWaypoint) {
        oss << "\"waypoint\":" << waypoint.serialize() << ",";
    }
    
    oss << "\"hasWaypoint\":" << (hasActiveWaypoint ? "true" : "false");
    
    oss << "}";
    return oss.str();
}

void WorldOpenWorld::deserializeStateOpenWorld(const std::string& jsonState) {
    // Production would use proper JSON parser
    // For now, initialize with defaults
    saveVersion.deserialize(jsonState);
    
    if (saveVersion.schemaVersion == 1) {
        // Migrate from v1
        migrateV1ToV2();
    }
    
    emitEvent("stateLoaded", "{}");
}

SaveVersionInfo WorldOpenWorld::getSaveVersionInfo() const {
    return saveVersion;
}

void WorldOpenWorld::emitBiomeEvent(const std::string& type, const std::string& data) {
    emitEvent("biome_" + type, data);
}

void WorldOpenWorld::emitWeatherEvent(const std::string& type, const std::string& data) {
    emitEvent("weather_" + type, data);
}

void WorldOpenWorld::emitStreamingEvent(const std::string& type, const std::string& data) {
    emitEvent("streaming_" + type, data);
}

BiomeDefinition WorldOpenWorld::getBiomeDefinition(BiomeType type) const {
    return BiomeDefinition::loadForType(type);
}

std::vector<BiomeDefinition> WorldOpenWorld::getAllBiomeDefinitions() const {
    std::vector<BiomeDefinition> definitions;
    
    definitions.push_back(BiomeDefinition::loadForType(BiomeType::STARTER_VALLEY));
    definitions.push_back(BiomeDefinition::loadForType(BiomeType::VILLAGE_TOWN));
    definitions.push_back(BiomeDefinition::loadForType(BiomeType::PINE_FOREST));
    definitions.push_back(BiomeDefinition::loadForType(BiomeType::RIVERLANDS));
    definitions.push_back(BiomeDefinition::loadForType(BiomeType::COASTLINE_HARBOR));
    definitions.push_back(BiomeDefinition::loadForType(BiomeType::HIGHLANDS));
    definitions.push_back(BiomeDefinition::loadForType(BiomeType::CANYON_BADLANDS));
    definitions.push_back(BiomeDefinition::loadForType(BiomeType::MIST_MARSH));
    
    return definitions;
}

std::vector<std::string> WorldOpenWorld::getActivitiesForCurrentBiome() const {
    const BiomeState* biome = getCurrentBiome();
    if (!biome) return {};
    
    BiomeDefinition def = BiomeDefinition::loadForType(biome->type);
    return def.allowedActivities;
}

void WorldOpenWorld::debugUnlockAllBiomes() {
    for (int y = 0; y < BIOME_GRID_HEIGHT; y++) {
        for (int x = 0; x < BIOME_GRID_WIDTH; x++) {
            BiomeState* biome = getBiomeAt(x, y);
            if (biome) {
                biome->isUnlocked = true;
                biome->isDiscovered = true;
            }
        }
    }
    emitEvent("debugAllBiomesUnlocked", "{}");
}

void WorldOpenWorld::debugSetWeather(WeatherType type) {
    weatherState.transitionTo(type, 5.0f);  // Fast transition for debug
    emitWeatherEvent("weatherDebugSet", weatherState.serialize());
}

int WorldOpenWorld::getLoadedBiomeCount() const {
    return static_cast<int>(loadedBiomeSet.size());
}

int WorldOpenWorld::getTotalBiomeCount() const {
    int count = 0;
    for (int y = 0; y < BIOME_GRID_HEIGHT; y++) {
        for (int x = 0; x < BIOME_GRID_WIDTH; x++) {
            const BiomeState* biome = getBiomeAt(x, y);
            if (biome && biome->isDiscovered) {
                count++;
            }
        }
    }
    return count;
}

float WorldOpenWorld::calculateDistance(int x1, int y1, int x2, int y2) const {
    float dx = static_cast<float>(x2 - x1);
    float dy = static_cast<float>(y2 - y1);
    return std::sqrt(dx * dx + dy * dy);
}

void WorldOpenWorld::recalculateLoadPriorities() {
    WorldPosition pos = getPlayerPosition();
    int playerBiomeX = pos.regionX / 5;
    int playerBiomeY = pos.regionY / 5;
    
    for (int y = 0; y < BIOME_GRID_HEIGHT; y++) {
        for (int x = 0; x < BIOME_GRID_WIDTH; x++) {
            BiomeState* biome = getBiomeAt(x, y);
            if (!biome) continue;
            
            float dist = calculateDistance(playerBiomeX, playerBiomeY, x, y);
            
            // Priority based on distance and activity
            biome->loadPriority = 1000.0f - dist * 10.0f;
            
            // Boost priority for biomes with outposts
            if (!biome->outpostPositions.empty()) {
                biome->loadPriority += 50.0f;
            }
            
            // Maximum boost for current biome
            if (x == playerBiomeX && y == playerBiomeY) {
                biome->loadPriority += 200.0f;
                biome->isActive = true;
            } else {
                biome->isActive = false;
            }
        }
    }
}
