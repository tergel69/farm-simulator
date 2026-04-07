#include "Biome.h"
#include <sstream>
#include <algorithm>

// ============================================================================
// BiomeType String Conversions
// ============================================================================

std::string biomeTypeToString(BiomeType type) {
    switch (type) {
        case BiomeType::STARTER_VALLEY: return "starter_valley";
        case BiomeType::VILLAGE_TOWN: return "village_town";
        case BiomeType::PINE_FOREST: return "pine_forest";
        case BiomeType::RIVERLANDS: return "riverlands";
        case BiomeType::COASTLINE_HARBOR: return "coastline_harbor";
        case BiomeType::HIGHLANDS: return "highlands";
        case BiomeType::CANYON_BADLANDS: return "canyon_badlands";
        case BiomeType::MIST_MARSH: return "mist_marsh";
        default: return "unknown";
    }
}

BiomeType stringToBiomeType(const std::string& str) {
    if (str == "starter_valley") return BiomeType::STARTER_VALLEY;
    if (str == "village_town") return BiomeType::VILLAGE_TOWN;
    if (str == "pine_forest") return BiomeType::PINE_FOREST;
    if (str == "riverlands") return BiomeType::RIVERLANDS;
    if (str == "coastline_harbor") return BiomeType::COASTLINE_HARBOR;
    if (str == "highlands") return BiomeType::HIGHLANDS;
    if (str == "canyon_badlands") return BiomeType::CANYON_BADLANDS;
    if (str == "mist_marsh") return BiomeType::MIST_MARSH;
    return BiomeType::STARTER_VALLEY;
}

// ============================================================================
// Microclimate Implementation
// ============================================================================

Microclimate::Microclimate()
    : temperature(0.5f), humidity(0.5f), windSpeed(0.3f), sunlight(0.7f),
      soilPH(6.5f), soilNutrients(0.7f), moisture(0.5f) {}

void Microclimate::update(float targetTemp, float targetHumidity, float dt) {
    // Smooth interpolation towards target values
    float lerpFactor = std::min(1.0f, dt * 0.1f);
    temperature = temperature + (targetTemp - temperature) * lerpFactor;
    humidity = humidity + (targetHumidity - humidity) * lerpFactor;
}

bool Microclimate::isSuitableForCrop(const std::string& cropId) const {
    // Simplified crop suitability check
    // In full implementation, this would reference a crop requirements table
    if (cropId == "cactus" || cropId == "heat_crop") {
        return temperature > 0.7f && moisture < 0.3f;
    }
    if (cropId == "rice" || cropId == "water_plant") {
        return moisture > 0.7f;
    }
    if (cropId == "mushroom" || cropId == "shade_crop") {
        return sunlight < 0.4f && humidity > 0.6f;
    }
    // Default crops prefer moderate conditions
    return temperature > 0.3f && temperature < 0.8f &&
           moisture > 0.3f && moisture < 0.8f;
}

std::string Microclimate::getStressFactor() const {
    if (temperature < 0.2f) return "too_cold";
    if (temperature > 0.9f) return "too_hot";
    if (moisture < 0.15f) return "drought";
    if (moisture > 0.9f) return "flooded";
    if (soilNutrients < 0.2f) return "nutrient_poor";
    if (soilPH < 5.5f || soilPH > 7.5f) return "wrong_ph";
    return "none";
}

// ============================================================================
// BiomeState Implementation
// ============================================================================

BiomeState::BiomeState()
    : type(BiomeType::STARTER_VALLEY), gridX(0), gridY(0),
      localWidth(50), localHeight(50), isUnlocked(false), isDiscovered(false),
      discoveryTimestamp(0), isLoaded(false), isActive(false), loadPriority(0.0f) {}

BiomeState::BiomeState(BiomeType type, const std::string& name, int gx, int gy)
    : type(type), name(name), gridX(gx), gridY(gy),
      localWidth(50), localHeight(50), isUnlocked(true), isDiscovered(true),
      discoveryTimestamp(0), isLoaded(true), isActive(false), loadPriority(1.0f) {
    
    // Initialize with default definition
    BiomeDefinition def = BiomeDefinition::loadForType(type);
    dominantFlora = def.dominantFlora.empty() ? "grass" : def.dominantFlora[0];
    faunaList = def.lootTables.count("fauna") ? def.lootTables.at("fauna") : std::vector<std::string>();
    
    // Set climate defaults
    if (def.climateDefaults.count("temperature")) {
        microclimate.temperature = def.climateDefaults.at("temperature");
    }
    if (def.climateDefaults.count("humidity")) {
        microclimate.humidity = def.climateDefaults.at("humidity");
    }
}

std::string BiomeState::serialize() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"type\":\"" << biomeTypeToString(type) << "\",";
    oss << "\"name\":\"" << name << "\",";
    oss << "\"gridX\":" << gridX << ",";
    oss << "\"gridY\":" << gridY << ",";
    oss << "\"localWidth\":" << localWidth << ",";
    oss << "\"localHeight\":" << localHeight << ",";
    oss << "\"isUnlocked\":" << (isUnlocked ? "true" : "false") << ",";
    oss << "\"isDiscovered\":" << (isDiscovered ? "true" : "false") << ",";
    oss << "\"discoveryTimestamp\":" << discoveryTimestamp << ",";
    oss << "\"isLoaded\":" << (isLoaded ? "true" : "false") << ",";
    oss << "\"isActive\":" << (isActive ? "true" : "false") << ",";
    oss << "\"loadPriority\":" << loadPriority << ",";
    oss << "\"dominantFlora\":\"" << dominantFlora << "\",";
    oss << "\"currentBiomeWeather\":\"" << currentBiomeWeather << "\",";
    
    // Serialize resource nodes
    oss << "\"resourceNodes\":{";
    bool first = true;
    for (const auto& pair : resourceNodes) {
        if (!first) oss << ",";
        oss << "\"" << pair.first << "\":" << pair.second;
        first = false;
    }
    oss << "},";
    
    // Serialize outpost positions
    oss << "\"outpostPositions\":[";
    for (size_t i = 0; i < outpostPositions.size(); ++i) {
        if (i > 0) oss << ",";
        oss << outpostPositions[i];
    }
    oss << "]";
    
    oss << "}";
    return oss.str();
}

void BiomeState::deserialize(const std::string& json) {
    // Simplified deserialization - production would use proper JSON parser
    // This is a placeholder that sets reasonable defaults
    isUnlocked = true;
    isDiscovered = true;
    isLoaded = false;
    isActive = false;
}

BiomeState BiomeState::createDefault(BiomeType type) {
    std::string name = biomeTypeToString(type);
    return BiomeState(type, name, 0, 0);
}

// ============================================================================
// WeatherType String Conversions
// ============================================================================

std::string weatherTypeToString(WeatherType type) {
    switch (type) {
        case WeatherType::CLEAR: return "clear";
        case WeatherType::SUNNY: return "sunny";
        case WeatherType::CLOUDY: return "cloudy";
        case WeatherType::RAINY: return "rainy";
        case WeatherType::STORMY: return "stormy";
        case WeatherType::THUNDERSTORM: return "thunderstorm";
        case WeatherType::SNOWY: return "snowy";
        case WeatherType::BLIZZARD: return "blizzard";
        case WeatherType::FOGGY: return "foggy";
        case WeatherType::WINDY: return "windy";
        case WeatherType::HOT: return "hot";
        case WeatherType::FREEZING: return "freezing";
        case WeatherType::MAGICAL_SPARKLE: return "magical_sparkle";
        case WeatherType::SANDSTORM: return "sandstorm";
        default: return "clear";
    }
}

WeatherType stringToWeatherType(const std::string& str) {
    if (str == "clear") return WeatherType::CLEAR;
    if (str == "sunny") return WeatherType::SUNNY;
    if (str == "cloudy") return WeatherType::CLOUDY;
    if (str == "rainy") return WeatherType::RAINY;
    if (str == "stormy") return WeatherType::STORMY;
    if (str == "thunderstorm") return WeatherType::THUNDERSTORM;
    if (str == "snowy") return WeatherType::SNOWY;
    if (str == "blizzard") return WeatherType::BLIZZARD;
    if (str == "foggy") return WeatherType::FOGGY;
    if (str == "windy") return WeatherType::WINDY;
    if (str == "hot") return WeatherType::HOT;
    if (str == "freezing") return WeatherType::FREEZING;
    if (str == "magical_sparkle") return WeatherType::MAGICAL_SPARKLE;
    if (str == "sandstorm") return WeatherType::SANDSTORM;
    return WeatherType::CLEAR;
}

// ============================================================================
// WeatherState Implementation
// ============================================================================

WeatherState::WeatherState()
    : current(WeatherType::CLEAR), previous(WeatherType::CLEAR),
      intensity(0.5f), transitionProgress(1.0f), durationRemaining(120),
      timeStarted(0), visibilityModifier(1.0f), movementModifier(1.0f),
      cropGrowthModifier(1.0f), fishingBonus(0.0f), foragingBonus(0.0f),
      ambientSound("nature"), musicOverride("") {}

void WeatherState::transitionTo(WeatherType newWeather, float transitionTime) {
    if (newWeather == current) return;
    
    previous = current;
    current = newWeather;
    transitionProgress = 0.0f;
    durationRemaining = static_cast<int>(transitionTime);
    
    // Set effects based on weather type
    switch (newWeather) {
        case WeatherType::RAINY:
        case WeatherType::THUNDERSTORM:
            visibilityModifier = 0.7f;
            cropGrowthModifier = 1.2f;
            fishingBonus = 0.2f;
            ambientSound = "rain";
            break;
        case WeatherType::SNOWY:
        case WeatherType::BLIZZARD:
            visibilityModifier = 0.5f;
            movementModifier = 0.9f;
            cropGrowthModifier = 0.5f;
            ambientSound = "wind_snow";
            break;
        case WeatherType::FOGGY:
            visibilityModifier = 0.4f;
            foragingBonus = 0.1f;
            ambientSound = "quiet_fog";
            break;
        case WeatherType::WINDY:
            movementModifier = 0.95f;
            ambientSound = "strong_wind";
            break;
        case WeatherType::MAGICAL_SPARKLE:
            visibilityModifier = 1.1f;
            cropGrowthModifier = 1.3f;
            ambientSound = "magical_chimes";
            break;
        case WeatherType::SANDSTORM:
            visibilityModifier = 0.3f;
            movementModifier = 0.8f;
            foragingBonus = -0.3f;
            ambientSound = "sandstorm";
            break;
        default:
            visibilityModifier = 1.0f;
            movementModifier = 1.0f;
            cropGrowthModifier = 1.0f;
            fishingBonus = 0.0f;
            foragingBonus = 0.0f;
            ambientSound = "nature";
    }
}

void WeatherState::update(float dt) {
    if (transitionProgress < 1.0f) {
        transitionProgress += dt / 60.0f;  // Assume 60 second base transition
        if (transitionProgress > 1.0f) {
            transitionProgress = 1.0f;
        }
    }
    
    if (durationRemaining > 0) {
        durationRemaining -= static_cast<int>(dt);
        if (durationRemaining <= 0) {
            // Weather will naturally end - transition to clear
            transitionTo(WeatherType::CLEAR, 60.0f);
        }
    }
}

std::string WeatherState::serialize() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"current\":\"" << weatherTypeToString(current) << "\",";
    oss << "\"previous\":\"" << weatherTypeToString(previous) << "\",";
    oss << "\"intensity\":" << intensity << ",";
    oss << "\"transitionProgress\":" << transitionProgress << ",";
    oss << "\"durationRemaining\":" << durationRemaining << ",";
    oss << "\"timeStarted\":" << timeStarted << ",";
    oss << "\"visibilityModifier\":" << visibilityModifier << ",";
    oss << "\"movementModifier\":" << movementModifier << ",";
    oss << "\"cropGrowthModifier\":" << cropGrowthModifier << ",";
    oss << "\"ambientSound\":\"" << ambientSound << "\"";
    oss << "}";
    return oss.str();
}

void WeatherState::deserialize(const std::string& json) {
    // Simplified - production would parse JSON properly
    current = WeatherType::CLEAR;
    previous = WeatherType::CLEAR;
    intensity = 0.5f;
    transitionProgress = 1.0f;
}

// ============================================================================
// BiomePosition Implementation
// ============================================================================

bool BiomePosition::operator==(const BiomePosition& other) const {
    return biomeGridX == other.biomeGridX &&
           biomeGridY == other.biomeGridY &&
           localX == other.localX &&
           localY == other.localY;
}

bool BiomePosition::operator!=(const BiomePosition& other) const {
    return !(*this == other);
}

std::string BiomePosition::serialize() const {
    std::ostringstream oss;
    oss << "{\"biomeGridX\":" << biomeGridX
        << ",\"biomeGridY\":" << biomeGridY
        << ",\"localX\":" << localX
        << ",\"localY\":" << localY << "}";
    return oss.str();
}

BiomePosition BiomePosition::deserialize(const std::string& json) {
    // Simplified - production would parse JSON properly
    BiomePosition pos;
    pos.biomeGridX = 0;
    pos.biomeGridY = 0;
    pos.localX = 0;
    pos.localY = 0;
    return pos;
}

// ============================================================================
// StreamingConfig Implementation
// ============================================================================

StreamingConfig::StreamingConfig()
    : maxLoadedBiomes(5), loadDistance(150.0f), unloadDistance(200.0f),
      preloadDistance(100.0f), priorityBoostRadius(50) {}

// ============================================================================
// BiomeDefinition Implementation
// ============================================================================

BiomeDefinition::BiomeDefinition()
    : type(BiomeType::STARTER_VALLEY), recommendedLevel(1), hasDanger(false) {}

BiomeDefinition BiomeDefinition::loadForType(BiomeType type) {
    BiomeDefinition def;
    def.type = type;
    
    switch (type) {
        case BiomeType::STARTER_VALLEY:
            def.displayName = "Starter Valley";
            def.description = "A gentle valley perfect for beginning your farming journey.";
            def.backgroundMusic = "valley_peaceful";
            def.ambientSound = "birds_gentle_breeze";
            def.allowedActivities = {"farming", "watering", "harvesting", "animal_care", "home_decorating"};
            def.baseResourceRates = {"basic_seed", 1.0, "clay", 0.3};
            def.climateDefaults = {{"temperature", 0.5f}, {"humidity", 0.5f}, {"sunlight", 0.7f}};
            def.uniqueFeatures = {"home_plot", "starter_farm"};
            def.recommendedLevel = 1;
            def.hasDanger = false;
            break;
            
        case BiomeType::VILLAGE_TOWN:
            def.displayName = "Harbor Town";
            def.description = "A bustling town with shops, festivals, and friendly NPCs.";
            def.backgroundMusic = "town_lively";
            def.ambientSound = "people_marketplace";
            def.allowedActivities = {"shopping", "talking", "questing", "trading", "festival"};
            def.baseResourceRates = {"crafted_goods", 1.0, "rare_items", 0.1};
            def.climateDefaults = {{"temperature", 0.5f}, {"humidity", 0.4f}};
            def.uniqueFeatures = {"market_square", "town_hall", "shops"};
            def.recommendedLevel = 1;
            def.hasDanger = false;
            break;
            
        case BiomeType::PINE_FOREST:
            def.displayName = "Pine Forest";
            def.description = "A dense forest with forageables, critters, and ancient trees.";
            def.backgroundMusic = "forest_mysterious";
            def.ambientSound = "forest_creatures";
            def.allowedActivities = {"foraging", "hunting", "exploring", "mushroom_hunting"};
            def.baseResourceRates = {"wood", 0.8, "mushroom", 0.5, "berries", 0.6};
            def.climateDefaults = {{"temperature", 0.4f}, {"humidity", 0.7f}, {"sunlight", 0.4f}};
            def.uniqueFeatures = {"ancient_trees", "critter_den"};
            def.recommendedLevel = 3;
            def.hasDanger = false;
            break;
            
        case BiomeType::RIVERLANDS:
            def.displayName = "Riverlands";
            def.description = "Fertile riverbanks with excellent fishing and water mills.";
            def.backgroundMusic = "river_flow";
            def.ambientSound = "water_flowing";
            def.allowedActivities = {"fishing", "foraging", "mill_operation", "reed_harvest"};
            def.baseResourceRates = {"fish_common", 0.9, "reeds", 0.7, "water_plants", 0.5};
            def.climateDefaults = {{"temperature", 0.5f}, {"humidity", 0.8f}, {"moisture", 0.9f}};
            def.uniqueFeatures = {"water_mill", "fishing_spots"};
            def.recommendedLevel = 2;
            def.hasDanger = false;
            break;
            
        case BiomeType::COASTLINE_HARBOR:
            def.displayName = "Coastline Harbor";
            def.description = "A coastal area with boats, rare fish, and visiting traders.";
            def.backgroundMusic = "ocean_waves";
            def.ambientSound = "seagulls_waves";
            def.allowedActivities = {"fishing_rare", "boating", "trading", "treasure_hunting"};
            def.baseResourceRates = {"fish_rare", 0.4, "treasure", 0.1, "salt", 0.5};
            def.climateDefaults = {{"temperature", 0.6f}, {"humidity", 0.6f}, {"windSpeed", 0.7f}};
            def.uniqueFeatures = {"dock", "lighthouse", "trader_spawn"};
            def.recommendedLevel = 5;
            def.hasDanger = false;
            break;
            
        case BiomeType::HIGHLANDS:
            def.displayName = "Highlands";
            def.description = "Windy highlands with grazing animals and rare herbs.";
            def.backgroundMusic = "highland_winds";
            def.ambientSound = "strong_wind_grass";
            def.allowedActivities = {"animal_herding", "herb_gathering", "wind_energy", "exploring"};
            def.baseResourceRates = {"herbs_rare", 0.4, "wool", 0.6, "wind_essence", 0.3};
            def.climateDefaults = {{"temperature", 0.3f}, {"humidity", 0.4f}, {"windSpeed", 0.9f}};
            def.uniqueFeatures = {"windmills", "grazing_fields"};
            def.recommendedLevel = 4;
            def.hasDanger = false;
            break;
            
        case BiomeType::CANYON_BADLANDS:
            def.displayName = "Canyon Badlands";
            def.description = "Hot canyon lands with ore deposits, relics, and heat-resistant crops.";
            def.backgroundMusic = "desert_mystery";
            def.ambientSound = "hot_wind_echoes";
            def.allowedActivities = {"mining", "relic_hunting", "heat_crop_farming", "exploring"};
            def.baseResourceRates = {"ore_common", 0.7, "ore_rare", 0.2, "relics", 0.1, "cactus", 0.5};
            def.climateDefaults = {{"temperature", 0.9f}, {"humidity", 0.1f}, {"moisture", 0.1f}};
            def.uniqueFeatures = {"ore_veins", "ancient_ruins", "heat_springs"};
            def.recommendedLevel = 6;
            def.hasDanger = true;
            break;
            
        case BiomeType::MIST_MARSH:
            def.displayName = "Mist Marsh";
            def.description = "A mystical marsh with magical ecology and late-game secrets.";
            def.backgroundMusic = "marsh_ethereal";
            def.ambientSound = "mystical_fog";
            def.allowedActivities = {"magical_foraging", "spirit_encounters", "rare_herb_gathering"};
            def.baseResourceRates = {"magical_essence", 0.2, "spirit_herbs", 0.3, "glow_mushrooms", 0.4};
            def.climateDefaults = {{"temperature", 0.4f}, {"humidity", 0.9f}, {"sunlight", 0.2f}};
            def.uniqueFeatures = {"spirit_shrines", "glowing_flora", "teleportation_circles"};
            def.recommendedLevel = 8;
            def.hasDanger = false;
            break;
    }
    
    return def;
}
