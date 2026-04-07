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
