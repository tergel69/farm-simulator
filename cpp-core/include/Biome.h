#ifndef BIOME_H
#define BIOME_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

// ============================================================================
// 8 Biome Types for Open World
// ============================================================================

enum class BiomeType {
    STARTER_VALLEY,      // Farm + Home base
    VILLAGE_TOWN,        // Shops, social, festivals
    PINE_FOREST,         // Forage + critters
    RIVERLANDS,          // Fishing + reeds + mills
    COASTLINE_HARBOR,    // Boats, rare fish, trader days
    HIGHLANDS,           // Animals, herbs, wind systems
    CANYON_BADLANDS,     // Ore, relics, heat crops
    MIST_MARSH           // Late-game magical ecology
};

std::string biomeTypeToString(BiomeType type);
BiomeType stringToBiomeType(const std::string& str);

// ============================================================================
// Microclimate and Soil Ecology
// ============================================================================

struct Microclimate {
    float temperature;      // 0.0 - 1.0 (cold to hot)
    float humidity;         // 0.0 - 1.0 (dry to wet)
    float windSpeed;        // 0.0 - 1.0 (calm to windy)
    float sunlight;         // 0.0 - 1.0 (shade to full sun)
    float soilPH;           // 4.5 - 8.5 (acidic to alkaline)
    float soilNutrients;    // 0.0 - 1.0 (depleted to rich)
    float moisture;         // 0.0 - 1.0 (dry to saturated)
    
    Microclimate();
    void update(float targetTemp, float targetHumidity, float dt);
    bool isSuitableForCrop(const std::string& cropId) const;
    std::string getStressFactor() const;
};

// ============================================================================
// Biome State - Serializable
// ============================================================================

struct BiomeState {
    BiomeType type;
    std::string name;
    int gridX;              // World grid position
    int gridY;
    int localWidth;         // Biome dimensions
    int localHeight;
    bool isUnlocked;
    bool isDiscovered;      // For fog-of-war exploration
    int discoveryTimestamp; // Game time when discovered
    
    // Biome-specific resources
    std::map<std::string, int> resourceNodes;  // e.g., {"ore_vein": 3, "herb_patch": 5}
    std::map<std::string, float> resourceRespawnTimers;
    
    // Ecology state
    Microclimate microclimate;
    std::string dominantFlora;
    std::vector<std::string> faunaList;
    
    // Weather overrides (biome-specific weather patterns)
    std::map<std::string, float> weatherProbabilities;  // {"clear": 0.4, "rainy": 0.3, ...}
    std::string currentBiomeWeather;
    
    // Streaming state
    bool isLoaded;          // Currently in memory
    bool isActive;          // Player is in this biome
    float loadPriority;     // For streaming decisions
    
    // Outposts (farm extensions)
    std::vector<int> outpostPositions;  // Encoded x,y positions for farm outposts
    
    BiomeState();
    BiomeState(BiomeType type, const std::string& name, int gx, int gy);
    
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    static BiomeState createDefault(BiomeType type);
};

// ============================================================================
// Weather State - Enhanced with microclimate support
// ============================================================================

enum class WeatherType {
    CLEAR,
    SUNNY,
    CLOUDY,
    RAINY,
    STORMY,
    THUNDERSTORM,
    SNOWY,
    BLIZZARD,
    FOGGY,
    WINDY,
    HOT,
    FREEZING,
    MAGICAL_SPARKLE,    // Mist Marsh special
    SANDSTORM           // Canyon special
};

std::string weatherTypeToString(WeatherType type);
WeatherType stringToWeatherType(const std::string& str);

struct WeatherState {
    WeatherType current;
    WeatherType previous;
    float intensity;            // 0.0 - 1.0
    float transitionProgress;   // 0.0 - 1.0 (for smooth transitions)
    int durationRemaining;      // Minutes remaining
    int timeStarted;            // Game time when weather started
    
    // Effects
    float visibilityModifier;   // 0.0 - 1.0
    float movementModifier;     // 0.8 - 1.2
    float cropGrowthModifier;   // 0.5 - 1.5
    float fishingBonus;         // -0.5 - 0.5
    float foragingBonus;        // -0.5 - 0.5
    
    // Audio states
    std::string ambientSound;
    std::string musicOverride;
    
    WeatherState();
    void transitionTo(WeatherType newWeather, float transitionTime = 60.0f);
    void update(float dt);
    
    std::string serialize() const;
    void deserialize(const std::string& json);
};

// ============================================================================
// World Position - Enhanced for biomes
// ============================================================================

struct BiomePosition {
    int biomeGridX;
    int biomeGridY;
    int localX;
    int localY;
    
    bool operator==(const BiomePosition& other) const;
    bool operator!=(const BiomePosition& other) const;
    
    std::string serialize() const;
    static BiomePosition deserialize(const std::string& json);
};

// ============================================================================
// Zone Streaming Configuration
// ============================================================================

struct StreamingConfig {
    int maxLoadedBiomes;          // Max biomes in memory
    float loadDistance;           // Distance threshold for loading
    float unloadDistance;         // Distance threshold for unloading
    float preloadDistance;        // Distance for preloading
    int priorityBoostRadius;      // Radius around player for priority loading
    
    StreamingConfig();
};

// ============================================================================
// Biome Data Definitions (Content)
// ============================================================================

struct BiomeDefinition {
    BiomeType type;
    std::string displayName;
    std::string description;
    std::string backgroundMusic;
    std::string ambientSound;
    std::vector<std::string> allowedActivities;
    std::map<std::string, float> baseResourceRates;
    std::map<std::string, std::vector<std::string>> lootTables;
    std::map<std::string, float> climateDefaults;  // temp, humidity, etc.
    std::vector<std::string> uniqueFeatures;
    int recommendedLevel;       // Minimum player level recommended
    bool hasDanger;             // Whether biome has threats
    
    BiomeDefinition();
    static BiomeDefinition loadForType(BiomeType type);
};

#endif // BIOME_H
