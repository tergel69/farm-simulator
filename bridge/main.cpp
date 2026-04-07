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

    EMSCRIPTEN_KEEPALIVE
    const char* getGameStateJson() {
        static std::string state;
        state = FarmBridge::getInstance().getStateJson();
        return state.c_str();
    }
}
#else
int main() {
    FarmBridge& bridge = FarmBridge::getInstance();
    bridge.init();
    
    std::cout << "Viby Farming Simulator - Desktop Mode" << std::endl;
    std::cout << "Grid: " << Farm::BASE_GRID_WIDTH << "x" << Farm::BASE_GRID_HEIGHT << std::endl;
    
    if (bridge.plantSeed(0, 0, "Carrot")) {
        std::cout << "Planted Carrot at 0,0" << std::endl;
    }
    if (bridge.plantSeed(1, 0, "Tomato")) {
        std::cout << "Planted Tomato at 1,0" << std::endl;
    }
    if (!bridge.plantSeed(0, 0, "Potato")) {
        std::cout << "Failed to plant Potato at 0,0 (plot occupied)" << std::endl;
    }
    if (!bridge.plantSeed(15, 15, "Carrot")) {
        std::cout << "Failed to plant Carrot at 15,15 (out of bounds)" << std::endl;
    }
    
    for (int i = 0; i < 50; i++) {
        bridge.updateWorld();
    }
    
    std::cout << "State: " << bridge.getStateJson() << std::endl;
    
    return 0;
}
#endif
