#ifndef SOIL_H
#define SOIL_H

#include <string>

enum class SoilType {
    NORMAL,
    FERTILE,
    SANDY,
    CLAY,
    LOAMY
};

struct SoilProperties {
    SoilType type;
    float growthMultiplier;
    float waterRetention;
    float nutrientLevel;
    int fertilizerTicks;
    
    SoilProperties();
    SoilProperties(SoilType t);
    
    void applyFertilizer(int amount);
    void tick();
    float getEffectiveGrowthMultiplier() const;
    std::string getTypeName() const;
    static SoilProperties fromTypeName(const std::string& name);
};

#endif
