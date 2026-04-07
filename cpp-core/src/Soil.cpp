#include "Soil.h"

SoilProperties::SoilProperties() 
    : type(SoilType::NORMAL), growthMultiplier(1.0f), waterRetention(1.0f), nutrientLevel(1.0f), fertilizerTicks(0) {}

SoilProperties::SoilProperties(SoilType t) : type(t), fertilizerTicks(0) {
    switch (t) {
        case SoilType::FERTILE:
            growthMultiplier = 1.5f;
            waterRetention = 1.2f;
            nutrientLevel = 1.3f;
            break;
        case SoilType::SANDY:
            growthMultiplier = 0.8f;
            waterRetention = 0.6f;
            nutrientLevel = 0.7f;
            break;
        case SoilType::CLAY:
            growthMultiplier = 0.9f;
            waterRetention = 1.4f;
            nutrientLevel = 1.1f;
            break;
        case SoilType::LOAMY:
            growthMultiplier = 1.2f;
            waterRetention = 1.1f;
            nutrientLevel = 1.2f;
            break;
        default: // NORMAL
            growthMultiplier = 1.0f;
            waterRetention = 1.0f;
            nutrientLevel = 1.0f;
            break;
    }
}

void SoilProperties::applyFertilizer(int amount) {
    fertilizerTicks += amount;
    nutrientLevel = std::min(2.0f, nutrientLevel + 0.1f * amount);
}

void SoilProperties::tick() {
    if (fertilizerTicks > 0) {
        fertilizerTicks--;
        if (fertilizerTicks == 0) {
            nutrientLevel = std::max(0.5f, nutrientLevel - 0.05f);
        }
    }
}

float SoilProperties::getEffectiveGrowthMultiplier() const {
    float multiplier = growthMultiplier;
    if (fertilizerTicks > 0) {
        multiplier *= 1.2f;
    }
    return multiplier;
}

std::string SoilProperties::getTypeName() const {
    switch (type) {
        case SoilType::FERTILE: return "Fertile";
        case SoilType::SANDY: return "Sandy";
        case SoilType::CLAY: return "Clay";
        case SoilType::LOAMY: return "Loamy";
        default: return "Normal";
    }
}

SoilProperties SoilProperties::fromTypeName(const std::string& name) {
    if (name == "Fertile") return SoilProperties(SoilType::FERTILE);
    if (name == "Sandy") return SoilProperties(SoilType::SANDY);
    if (name == "Clay") return SoilProperties(SoilType::CLAY);
    if (name == "Loamy") return SoilProperties(SoilType::LOAMY);
    return SoilProperties(SoilType::NORMAL);
}
