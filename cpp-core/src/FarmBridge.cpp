#include "FarmBridge.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#endif

void FarmBridge::init() {
    if (initialized) {
        return;
    }
    initialized = true;

#ifdef __EMSCRIPTEN__
    farm.registerCallback("cropPlanted", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", type, data);
    });
    farm.registerCallback("cropHarvested", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", type, data);
    });
    farm.registerCallback("cropReady", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", type, data);
    });
    farm.registerCallback("cropWithered", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", type, data);
    });
    farm.registerCallback("cropCleared", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", type, data);
    });
    farm.registerCallback("inventoryUpdated", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", type, data);
    });
#endif
}

void FarmBridge::initOpenWorld() {
    if (openWorldInitialized) {
        return;
    }
    openWorldInitialized = true;
    
    WorldOpenWorld& world = WorldOpenWorld::getInstance();
    world.initializeOpenWorld();
    
#ifdef __EMSCRIPTEN__
    // Register open world event callbacks
    world.registerCallback("biomeDiscovered", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", "biome_" + type, data);
    });
    world.registerCallback("biomeEntered", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", "biome_" + type, data);
    });
    world.registerCallback("weatherChanged", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", "weather_" + type, data);
    });
    world.registerCallback("waypointSet", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", type, data);
    });
    world.registerCallback("streaming_biomeLoaded", [](const std::string& type, const std::string& data) {
        emscripten::val::global("window").call<void>("dispatchFarmEvent", type, data);
    });
#endif
}

bool FarmBridge::plantSeed(int x, int y, const std::string& cropName) {
    return farm.plantSeed(x, y, cropName);
}

bool FarmBridge::harvestCrop(int x, int y) {
    return farm.harvestCrop(x, y);
}

bool FarmBridge::clearWitheredCrop(int x, int y) {
    return farm.clearWitheredCrop(x, y);
}

bool FarmBridge::waterCrop(int x, int y) {
    return farm.waterCrop(x, y);
}

void FarmBridge::updateWorld() {
    farm.updateWorld();
    
    // Also update open world systems
    if (openWorldInitialized) {
        WorldOpenWorld& world = WorldOpenWorld::getInstance();
        world.updateTime(1);  // Advance time by 1 minute per tick
        world.updateWeather(1.0f);  // Update weather with dt=1
        world.updateStreaming();
    }
}

std::string FarmBridge::getStateJson() const {
    return farm.serializeState();
}

std::string FarmBridge::getInventoryJson() const {
    auto inv = farm.getInventory();
    std::string result = "{";
    bool first = true;
    for (const auto& pair : inv) {
        if (!first) result += ",";
        result += "\"" + pair.first + "\":" + std::to_string(pair.second);
        first = false;
    }
    result += "}";
    return result;
}

std::string FarmBridge::getCropAt(int x, int y) const {
    const Crop* crop = farm.getCropAt(x, y);
    if (crop == nullptr) return "null";
    return "{\"name\":\"" + crop->name + "\",\"growth\":" + std::to_string(crop->getGrowthPercent()) + ",\"stage\":\"" + crop->getStageName() + "\",\"withered\":" + (crop->isWithered ? "true" : "false") + "}";
}

std::vector<std::string> FarmBridge::getAvailableCrops() const {
    std::vector<std::string> crops;
    const auto& definitions = Farm::getCropDefinitions();
    for (const auto& pair : definitions) {
        crops.push_back(pair.first);
    }
    return crops;
}

int FarmBridge::getTotalTicks() const {
    return farm.getTotalTicks();
}

// ============================================================================
// Open World API Implementations (Slice 1)
// ============================================================================

std::string FarmBridge::getWorldMapJson() const {
    if (!openWorldInitialized) {
        return "{\"width\":8,\"height\":8,\"biomes\":[],\"error\":\"Open world not initialized\"}";
    }
    return WorldOpenWorld::getInstance().getWorldMapJson();
}

std::string FarmBridge::getMinimapDataJson() const {
    if (!openWorldInitialized) {
        return "{\"player\":{\"x\":0,\"y\":0},\"adjacent\":[]}";
    }
    return WorldOpenWorld::getInstance().getMinimapDataJson();
}

bool FarmBridge::travelToBiome(int gridX, int gridY) {
    if (!openWorldInitialized) {
        initOpenWorld();
    }
    return WorldOpenWorld::getInstance().travelToBiome(gridX, gridY);
}

bool FarmBridge::setWaypoint(int gridX, int gridY, int localX, int localY) {
    if (!openWorldInitialized) {
        initOpenWorld();
    }
    return WorldOpenWorld::getInstance().setWaypoint(gridX, gridY, localX, localY);
}

void FarmBridge::clearWaypoint() {
    if (openWorldInitialized) {
        WorldOpenWorld::getInstance().clearWaypoint();
    }
}

std::string FarmBridge::getWeatherStateJson() const {
    if (!openWorldInitialized) {
        return "{\"current\":\"clear\",\"intensity\":0.5}";
    }
    return WorldOpenWorld::getInstance().getWeatherState().serialize();
}

std::string FarmBridge::getCurrentBiomeJson() const {
    if (!openWorldInitialized) {
        return "{\"type\":\"starter_valley\",\"name\":\"Sunny Meadow\"}";
    }
    const BiomeState* biome = WorldOpenWorld::getInstance().getCurrentBiome();
    if (!biome) {
        return "{\"type\":\"unknown\",\"name\":\"Unknown\"}";
    }
    
    std::string result = "{";
    result += "\"type\":\"" + biomeTypeToString(biome->type) + "\",";
    result += "\"name\":\"" + biome->name + "\",";
    result += "\"gridX\":" + std::to_string(biome->gridX) + ",";
    result += "\"gridY\":" + std::to_string(biome->gridY) + ",";
    result += "\"isUnlocked\":" + std::string(biome->isUnlocked ? "true" : "false");
    result += "}";
    return result;
}

std::string FarmBridge::getWorldSnapshotJson() const {
    if (!openWorldInitialized) {
        return "{\"error\":\"Open world not initialized\"}";
    }
    
    WorldOpenWorld& world = WorldOpenWorld::getInstance();
    std::string result = "{";
    result += "\"worldMap\":" + world.getWorldMapJson() + ",";
    result += "\"minimap\":" + world.getMinimapDataJson() + ",";
    result += "\"weather\":" + world.getWeatherState().serialize() + ",";
    result += "\"currentTime\":{\"hour\":" + std::to_string(world.getCurrentTime().hour) + 
              ",\"minute\":" + std::to_string(world.getCurrentTime().minute) + 
              ",\"day\":" + std::to_string(world.getCurrentTime().day) + "},";
    result += "\"season\":" + std::to_string(static_cast<int>(world.getCurrentSeason())) + ",";
    result += "\"loadedBiomes\":" + std::to_string(world.getLoadedBiomeCount()) + ",";
    result += "\"discoveredBiomes\":" + std::to_string(world.getTotalBiomeCount());
    result += "}";
    return result;
}

// ============================================================================
// Farm Outposts API Implementations (Slice 2)
// ============================================================================

bool FarmBridge::establishOutpost(int biomeGridX, int biomeGridY, int localX, int localY) {
    if (!openWorldInitialized) {
        initOpenWorld();
    }
    return WorldOpenWorld::getInstance().establishOutpost(biomeGridX, biomeGridY, localX, localY);
}

bool FarmBridge::removeOutpost(int biomeGridX, int biomeGridY, int localX, int localY) {
    if (!openWorldInitialized) {
        return false;
    }
    return WorldOpenWorld::getInstance().removeOutpost(biomeGridX, biomeGridY, localX, localY);
}

std::string FarmBridge::getOutpostPositionsJson() const {
    if (!openWorldInitialized) {
        return "[]";
    }
    
    auto positions = WorldOpenWorld::getInstance().getOutpostPositions();
    std::string result = "[";
    bool first = true;
    for (const auto& pos : positions) {
        if (!first) result += ",";
        result += pos.serialize();
        first = false;
    }
    result += "]";
    return result;
}

