#ifndef CROP_H
#define CROP_H

#include <string>
#include <vector>

enum class CropStage {
    SEED,
    SPROUT,
    GROWING,
    MATURE,
    READY_TO_HARVEST,
    WITHERED
};

class Crop {
public:
    std::string name;
    int growthTime;
    int currentAge;
    CropStage stage;
    bool isWithered;
    int witherThreshold;
    bool isWatered;
    int waterTicksRemaining;

    Crop();
    Crop(std::string n, int totalGrowthTime);

    void grow();
    void growWithWaterBonus();
    void wither();
    CropStage updateStage();
    int getGrowthPercent() const;
    bool isReady() const;
    std::string getStageName() const;
};

#endif
