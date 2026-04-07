#include "Crop.h"
#include <algorithm>

CropVariety::CropVariety() 
    : growthTime(100), season(CropSeason::ALL), seedCost(10), harvestYield(25), 
      waterBonus(1.5f), isRare(false) {}

Crop::Crop() 
    : name(""), varietyName(""), growthTime(0), currentAge(0), stage(CropStage::SEED), 
      isWithered(false), witherThreshold(0), isWatered(false), waterTicksRemaining(0),
      quality(1), soilNutrientConsumption(1.0f), season(CropSeason::ALL),
      isCompanionBoosted(false), crossbreedGeneration(0) {}

Crop::Crop(std::string n, int totalGrowthTime)
    : name(n), varietyName(n), growthTime(totalGrowthTime), currentAge(0), stage(CropStage::SEED), 
      isWithered(false), witherThreshold(totalGrowthTime * 3), isWatered(false), waterTicksRemaining(0),
      quality(1), soilNutrientConsumption(1.0f), season(CropSeason::ALL),
      isCompanionBoosted(false), crossbreedGeneration(0) {
    updateStage();
}

Crop::Crop(std::string n, std::string variety, int totalGrowthTime)
    : name(n), varietyName(variety), growthTime(totalGrowthTime), currentAge(0), stage(CropStage::SEED), 
      isWithered(false), witherThreshold(totalGrowthTime * 3), isWatered(false), waterTicksRemaining(0),
      quality(1), soilNutrientConsumption(1.0f), season(CropSeason::ALL),
      isCompanionBoosted(false), crossbreedGeneration(1) {
    updateStage();
}

void Crop::grow(float soilMultiplier) {
    if (isWithered) return;
    if (isWatered) {
        growWithWaterBonus(soilMultiplier);
    } else {
        if (currentAge < growthTime) {
            currentAge += static_cast<int>(soilMultiplier);
            updateStage();
        } else if (currentAge < witherThreshold) {
            currentAge += static_cast<int>(soilMultiplier);
        } else {
            wither();
        }
    }
}

void Crop::growWithWaterBonus(float soilMultiplier) {
    if (isWithered) return;
    if (waterTicksRemaining > 0) {
        waterTicksRemaining--;
        if (waterTicksRemaining == 0) {
            isWatered = false;
        }
    }
    float growthAmount = 1.5f * soilMultiplier;
    if (isCompanionBoosted) {
        growthAmount *= 1.2f;
    }
    if (currentAge < growthTime) {
        currentAge += static_cast<int>(growthAmount);
        updateStage();
    } else if (currentAge < witherThreshold) {
        currentAge += static_cast<int>(growthAmount);
    } else {
        wither();
    }
}

void Crop::wither() {
    isWithered = true;
    stage = CropStage::WITHERED;
}

CropStage Crop::updateStage() {
    if (isWithered) return stage;
    if (currentAge == 0) {
        stage = CropStage::SEED;
    } else if (currentAge < growthTime * 0.25) {
        stage = CropStage::SPROUT;
    } else if (currentAge < growthTime * 0.5) {
        stage = CropStage::GROWING;
    } else if (currentAge < growthTime) {
        stage = CropStage::MATURE;
    } else {
        stage = CropStage::READY_TO_HARVEST;
    }
    return stage;
}

int Crop::getGrowthPercent() const {
    if (growthTime == 0) return 0;
    int percent = (currentAge * 100) / growthTime;
    return percent > 100 ? 100 : percent;
}

bool Crop::isReady() const {
    return currentAge >= growthTime && !isWithered;
}

std::string Crop::getStageName() const {
    switch (stage) {
        case CropStage::SEED: return "Seed";
        case CropStage::SPROUT: return "Sprout";
        case CropStage::GROWING: return "Growing";
        case CropStage::MATURE: return "Mature";
        case CropStage::READY_TO_HARVEST: return "Ready to Harvest";
        case CropStage::WITHERED: return "Withered";
        default: return "Unknown";
    }
}

int Crop::calculateYield() const {
    if (!isReady()) return 0;
    int baseYield = 1;
    float qualityMultiplier = 1.0f + (quality - 1) * 0.1f;
    float companionMultiplier = isCompanionBoosted ? 1.15f : 1.0f;
    return static_cast<int>(baseYield * qualityMultiplier * companionMultiplier);
}

void Crop::applyCompanionBoost() {
    isCompanionBoosted = true;
    quality = std::min(5, quality + 1);
}

bool Crop::tryCrossbreed(const Crop& other) {
    if (!isReady() || !other.isReady()) return false;
    if (name != other.name) return false;
    
    crossbreedGeneration = std::max(crossbreedGeneration, other.crossbreedGeneration) + 1;
    quality = std::min(5, std::max(quality, other.quality) + 1);
    return true;
}
