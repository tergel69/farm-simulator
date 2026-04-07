#include "Crop.h"

Crop::Crop() : name(""), growthTime(0), currentAge(0), stage(CropStage::SEED), isWithered(false), witherThreshold(0), isWatered(false), waterTicksRemaining(0) {}

Crop::Crop(std::string n, int totalGrowthTime)
    : name(n), growthTime(totalGrowthTime), currentAge(0), stage(CropStage::SEED), isWithered(false), witherThreshold(totalGrowthTime * 3), isWatered(false), waterTicksRemaining(0) {
    updateStage();
}

void Crop::grow() {
    if (isWithered) return;
    if (isWatered) {
        growWithWaterBonus();
    } else {
        if (currentAge < growthTime) {
            currentAge++;
            updateStage();
        } else if (currentAge < witherThreshold) {
            currentAge++;
        } else {
            wither();
        }
    }
}

void Crop::growWithWaterBonus() {
    if (isWithered) return;
    if (waterTicksRemaining > 0) {
        waterTicksRemaining--;
        if (waterTicksRemaining == 0) {
            isWatered = false;
        }
    }
    if (currentAge < growthTime) {
        currentAge += 1.5;
        updateStage();
    } else if (currentAge < witherThreshold) {
        currentAge += 1.5;
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
