#ifndef FARM_BRIDGE_H
#define FARM_BRIDGE_H

#include "Farm.h"
#include "WorldOpenWorld.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#endif

#include <string>
#include <vector>

class FarmBridge {
public:
    static FarmBridge& getInstance() {
        static FarmBridge instance;
        return instance;
    }

    Farm& getFarm() { return farm; }
    WorldOpenWorld& getWorld() { return WorldOpenWorld::getInstance(); }

    void init();
    void initOpenWorld();

    // Core farming (existing)
    bool plantSeed(int x, int y, const std::string& cropName);
    bool harvestCrop(int x, int y);
    bool clearWitheredCrop(int x, int y);
    bool waterCrop(int x, int y);
    void updateWorld();
    std::string getStateJson() const;
    std::string getInventoryJson() const;
    std::string getCropAt(int x, int y) const;
    std::vector<std::string> getAvailableCrops() const;
    int getTotalTicks() const;

    // Open World APIs (new - Slice 1)
    std::string getWorldMapJson() const;
    std::string getMinimapDataJson() const;
    bool travelToBiome(int gridX, int gridY);
    bool setWaypoint(int gridX, int gridY, int localX, int localY);
    void clearWaypoint();
    std::string getWeatherStateJson() const;
    std::string getCurrentBiomeJson() const;
    std::string getWorldSnapshotJson() const;
    
    // Farm Outposts (Slice 2)
    bool establishOutpost(int biomeGridX, int biomeGridY, int localX, int localY);
    bool removeOutpost(int biomeGridX, int biomeGridY, int localX, int localY);
    std::string getOutpostPositionsJson() const;

private:
    Farm farm;
    bool initialized = false;
    bool openWorldInitialized = false;

    FarmBridge() {}
};

#endif
