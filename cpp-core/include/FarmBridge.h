#ifndef FARM_BRIDGE_H
#define FARM_BRIDGE_H

#include "Farm.h"

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

    void init();

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

private:
    Farm farm;
    bool initialized = false;

    FarmBridge() {}
};

#endif
