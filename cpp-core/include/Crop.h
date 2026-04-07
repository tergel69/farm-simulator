#ifndef CROP_H
#define CROP_H

#include <string>
#include <vector>
#include "Soil.h"

enum class CropStage {
    SEED,
    SPROUT,
    GROWING,
    MATURE,
    READY_TO_HARVEST,
    WITHERED
};

enum class CropSeason {
    ALL,
    SPRING,
    SUMMER,
    FALL,
    WINTER
};

struct CropVariety {
    std::string name;
    std::string displayName;
    int growthTime;
    CropSeason season;
    int seedCost;
    int harvestYield;
    float waterBonus;
    bool isRare;
    std::vector<std::string> companionCrops;
    std::vector<std::string> avoidCrops;
    
    CropVariety();
};

class Crop {
public:
    std::string name;
    std::string varietyName;
    int growthTime;
    int currentAge;
    CropStage stage;
    bool isWithered;
    int witherThreshold;
    bool isWatered;
    int waterTicksRemaining;
    int quality;
    float soilNutrientConsumption;
    CropSeason season;
    bool isCompanionBoosted;
    int crossbreedGeneration;
    std::string parentVariety1;
    std::string parentVariety2;

    Crop();
    Crop(std::string n, int totalGrowthTime);
    Crop(std::string n, std::string variety, int totalGrowthTime);

    void grow(float soilMultiplier = 1.0f);
    void growWithWaterBonus(float soilMultiplier = 1.0f);
    void wither();
    CropStage updateStage();
    int getGrowthPercent() const;
    bool isReady() const;
    std::string getStageName() const;
    int calculateYield() const;
    void applyCompanionBoost();
    bool tryCrossbreed(const Crop& other);
};

#endif
