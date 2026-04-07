#include "FarmBridge.h"
#include <iostream>

#ifdef BUILD_FOR_WEB
#include <emscripten.h>

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void initGame() {
        FarmBridge::getInstance().init();
    }
    
    EMSCRIPTEN_KEEPALIVE
    void initOpenWorld() {
        FarmBridge::getInstance().initOpenWorld();
    }
    
    EMSCRIPTEN_KEEPALIVE
    void updateWorld() {
        FarmBridge::getInstance().updateWorld();
    }

    EMSCRIPTEN_KEEPALIVE
    int plantSeed(int x, int y, const char* cropName) {
        return FarmBridge::getInstance().plantSeed(x, y, cropName ? cropName : "") ? 1 : 0;
    }

    EMSCRIPTEN_KEEPALIVE
    int harvestCrop(int x, int y) {
        return FarmBridge::getInstance().harvestCrop(x, y) ? 1 : 0;
    }

    EMSCRIPTEN_KEEPALIVE
    int clearWitheredCrop(int x, int y) {
        return FarmBridge::getInstance().clearWitheredCrop(x, y) ? 1 : 0;
    }

    EMSCRIPTEN_KEEPALIVE
    int waterCrop(int x, int y) {
        return FarmBridge::getInstance().waterCrop(x, y) ? 1 : 0;
    }
    
    // Open World APIs (Slice 1)
    EMSCRIPTEN_KEEPALIVE
    int travelToBiome(int gridX, int gridY) {
        return FarmBridge::getInstance().travelToBiome(gridX, gridY) ? 1 : 0;
    }
    
    EMSCRIPTEN_KEEPALIVE
    int setWaypoint(int gridX, int gridY, int localX, int localY) {
        return FarmBridge::getInstance().setWaypoint(gridX, gridY, localX, localY) ? 1 : 0;
    }
    
    EMSCRIPTEN_KEEPALIVE
    void clearWaypoint() {
        FarmBridge::getInstance().clearWaypoint();
    }

    EMSCRIPTEN_KEEPALIVE
    const char* getGameStateJson() {
        static std::string state;
        state = FarmBridge::getInstance().getStateJson();
        return state.c_str();
    }
    
    EMSCRIPTEN_KEEPALIVE
    const char* getWorldMapJson() {
        static std::string mapData;
        mapData = FarmBridge::getInstance().getWorldMapJson();
        return mapData.c_str();
    }
    
    EMSCRIPTEN_KEEPALIVE
    const char* getMinimapDataJson() {
        static std::string minimapData;
        minimapData = FarmBridge::getInstance().getMinimapDataJson();
        return minimapData.c_str();
    }
    
    EMSCRIPTEN_KEEPALIVE
    const char* getWeatherStateJson() {
        static std::string weatherData;
        weatherData = FarmBridge::getInstance().getWeatherStateJson();
        return weatherData.c_str();
    }
    
    EMSCRIPTEN_KEEPALIVE
    const char* getCurrentBiomeJson() {
        static std::string biomeData;
        biomeData = FarmBridge::getInstance().getCurrentBiomeJson();
        return biomeData.c_str();
    }
    
    EMSCRIPTEN_KEEPALIVE
    const char* getWorldSnapshotJson() {
        static std::string snapshotData;
        snapshotData = FarmBridge::getInstance().getWorldSnapshotJson();
        return snapshotData.c_str();
    }
    
    // Farm Outposts (Slice 2)
    EMSCRIPTEN_KEEPALIVE
    int establishOutpost(int biomeGridX, int biomeGridY, int localX, int localY) {
        return FarmBridge::getInstance().establishOutpost(biomeGridX, biomeGridY, localX, localY) ? 1 : 0;
    }
    
    EMSCRIPTEN_KEEPALIVE
    int removeOutpost(int biomeGridX, int biomeGridY, int localX, int localY) {
        return FarmBridge::getInstance().removeOutpost(biomeGridX, biomeGridY, localX, localY) ? 1 : 0;
    }
    
    EMSCRIPTEN_KEEPALIVE
    const char* getOutpostPositionsJson() {
        static std::string outpostData;
        outpostData = FarmBridge::getInstance().getOutpostPositionsJson();
        return outpostData.c_str();
    }
}
#else
int main() {
    FarmBridge& bridge = FarmBridge::getInstance();
    bridge.init();
    bridge.initOpenWorld();
    
    std::cout << "Viby Farming Simulator - Open World Edition" << std::endl;
    std::cout << "Grid: " << Farm::BASE_GRID_WIDTH << "x" << Farm::BASE_GRID_HEIGHT << std::endl;
    std::cout << "8-Biome Open World Enabled" << std::endl;
    
    // Test open world initialization
    auto& world = bridge.getWorld();
    std::cout << "World Map: " << bridge.getWorldMapJson() << std::endl;
    std::cout << "Current Biome: " << bridge.getCurrentBiomeJson() << std::endl;
    std::cout << "Weather: " << bridge.getWeatherStateJson() << std::endl;
    
    // Test basic farming
    if (bridge.plantSeed(0, 0, "Carrot")) {
        std::cout << "Planted Carrot at 0,0" << std::endl;
    }
    if (bridge.plantSeed(1, 0, "Tomato")) {
        std::cout << "Planted Tomato at 1,0" << std::endl;
    }
    
    // Advance time
    for (int i = 0; i < 50; i++) {
        bridge.updateWorld();
    }
    
    std::cout << "State: " << bridge.getStateJson() << std::endl;
    std::cout << "World Snapshot: " << bridge.getWorldSnapshotJson() << std::endl;
    
    return 0;
}
#endif
