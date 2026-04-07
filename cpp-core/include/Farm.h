#ifndef FARM_H
#define FARM_H

#include "Crop.h"
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <memory>

struct PlotPosition {
    int x;
    int y;
};

struct FarmEvent {
    std::string type;
    std::string data;
};

struct CropDefinition {
    std::string name;
    int growthTime;
    std::string displayName;
};

class Farm {
public:
    static constexpr int GRID_WIDTH = 10;
    static constexpr int GRID_HEIGHT = 10;

    Farm();
    ~Farm();

    Farm(const Farm&) = delete;
    Farm& operator=(const Farm&) = delete;

    bool plantSeed(int x, int y, const std::string& cropName);
    bool harvestCrop(int x, int y);
    bool clearWitheredCrop(int x, int y);
    bool waterCrop(int x, int y);
    void updateWorld();
    const Crop* getCropAt(int x, int y) const;
    std::map<std::string, int> getInventory() const;
    void addToInventory(const std::string& itemName, int quantity = 1);
    void removeFromInventory(const std::string& itemName, int quantity = 1);

    static const std::map<std::string, CropDefinition>& getCropDefinitions();
    static bool isValidCrop(const std::string& cropName);

    using EventCallback = std::function<void(const std::string& type, const std::string& data)>;
    void registerCallback(const std::string& eventName, EventCallback callback);
    void emitEvent(const std::string& eventName, const std::string& data);

    std::string serializeState() const;
    void deserializeState(const std::string& jsonState);
    int getTotalTicks() const { return totalTicks; }

private:
    std::unique_ptr<Crop> grid[GRID_HEIGHT][GRID_WIDTH];
    std::map<std::string, int> inventory;
    std::map<std::string, std::vector<EventCallback>> callbacks;
    int totalTicks;

    void clearPlot(int x, int y);
    std::string escapeJson(const std::string& input) const;
};

#endif
