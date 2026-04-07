#ifndef FARM_H
#define FARM_H

#include "Crop.h"
#include "Soil.h"
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <memory>

struct PlotPosition {
    int x;
    int y;
};

struct FarmPlot {
    std::unique_ptr<Crop> crop;
    SoilProperties soil;
    bool isUnlocked;
    int plotIndex;
    
    FarmPlot();
    FarmPlot(SoilType soilType);
};

struct FarmEvent {
    std::string type;
    std::string data;
};

struct CropDefinition {
    std::string name;
    int growthTime;
    std::string displayName;
    CropSeason season;
    int seedCost;
    int harvestYield;
    float waterBonus;
    bool isRare;
    std::vector<std::string> companionCrops;
    std::vector<std::string> avoidCrops;
};

struct AnimalDefinition {
    std::string name;
    std::string displayName;
    std::string product;
    int productionTime;
    int cost;
    std::string foodPreference;
};

class Farm {
public:
    static constexpr int BASE_GRID_WIDTH = 10;
    static constexpr int BASE_GRID_HEIGHT = 10;
    static constexpr int MAX_GRID_WIDTH = 30;
    static constexpr int MAX_GRID_HEIGHT = 30;
    static constexpr int MAX_PLOTS = MAX_GRID_WIDTH * MAX_GRID_HEIGHT;

    Farm();
    ~Farm();

    Farm(const Farm&) = delete;
    Farm& operator=(const Farm&) = delete;

    bool plantSeed(int x, int y, const std::string& cropName);
    bool harvestCrop(int x, int y);
    bool clearWitheredCrop(int x, int y);
    bool waterCrop(int x, int y);
    bool fertilizePlot(int x, int y, int amount);
    void updateWorld();
    const Crop* getCropAt(int x, int y) const;
    const FarmPlot* getPlotAt(int x, int y) const;
    std::map<std::string, int> getInventory() const;
    void addToInventory(const std::string& itemName, int quantity = 1);
    void removeFromInventory(const std::string& itemName, int quantity = 1);
    
    bool unlockAdjacentPlot(int fromX, int fromY, int direction);
    bool isPlotUnlocked(int x, int y) const;
    SoilType getSoilTypeAt(int x, int y) const;
    
    void checkCompanionPlanting();
    bool applyCropRotation(int x, int y, const std::string& newCrop);
    
    static const std::map<std::string, CropDefinition>& getCropDefinitions();
    static const std::map<std::string, AnimalDefinition>& getAnimalDefinitions();
    static bool isValidCrop(const std::string& cropName);
    static bool isValidAnimal(const std::string& animalName);

    using EventCallback = std::function<void(const std::string& type, const std::string& data)>;
    void registerCallback(const std::string& eventName, EventCallback callback);
    void emitEvent(const std::string& eventName, const std::string& data);

    std::string serializeState() const;
    void deserializeState(const std::string& jsonState);
    int getTotalTicks() const { return totalTicks; }
    int getCurrentSeason() const { return currentSeason; }
    void advanceSeason();

private:
    std::map<int, std::map<int, FarmPlot>> plots;
    std::map<std::string, int> inventory;
    std::map<std::string, std::vector<EventCallback>> callbacks;
    int totalTicks;
    int currentSeason;
    int farmLevel;
    int money;
    
    void clearPlot(int x, int y);
    std::string escapeJson(const std::string& input) const;
    bool isInBounds(int x, int y) const;
    void initializeStartingPlots();
};

#endif
