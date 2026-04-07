#ifndef FARM_INFRASTRUCTURE_H
#define FARM_INFRASTRUCTURE_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "Building.h"

// Infrastructure types for farm improvements
enum class IrrigationType {
    SPRINKLER_BASIC,      // Waters 4 adjacent tiles
    SPRINKLER_ADVANCED,   // Waters 8 adjacent tiles
    SPRINKLER_QUALITY,    // Waters 24 tiles in radius
    DRIP_IRRIGATION,      // Water-efficient line system
    CANAL,                // Manual water channel
    RAIN_COLLECTOR,       // Collects rainwater automatically
    WELL_PUMP             // Unlimited water source
};

enum class FenceType {
    WOOD,
    STONE,
    IRON,
    HARDWOOD,
    GOLD,
    IRRADIATED,
    HEDGE
};

enum class PathType {
    DIRT,
    GRAVEL,
    STONE,
    BRICK,
    WOOD_PLANK,
    CRYSTAL,
    GRASS
};

enum class DecorationType {
    FLOWER_BED,
    FOUNTAIN,
    SCARECROW,
    GARDEN_GNOME,
    STATUE,
    LAMP_POST,
    BENCH,
    SIGNPOST,
    TORCH,
    TREE_DECORATIVE
};

struct IrrigationDefinition {
    std::string name;
    IrrigationType type;
    int waterRange;
    int waterCapacity;
    int cost;
    std::map<std::string, int> materials;
    std::string description;
    bool isAutomatic;
};

struct FenceDefinition {
    std::string name;
    FenceType type;
    int durability;
    int cost;
    std::map<std::string, int> materials;
    std::string description;
    float defenseBonus;
};

struct PathDefinition {
    std::string name;
    PathType type;
    int cost;
    std::map<std::string, int> materials;
    std::string description;
    float speedModifier;
};

struct DecorationDefinition {
    std::string name;
    DecorationType type;
    int cost;
    std::map<std::string, int> materials;
    std::string description;
    int decorationValue;
    bool providesBuff;
    std::string buffType;
    float buffRadius;
};

class IrrigationSystem {
public:
    IrrigationSystem();
    IrrigationSystem(IrrigationType type, Position pos);
    ~IrrigationSystem();
    
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Water management
    bool addWater(int amount);
    int removeWater(int amount);
    int getCurrentWater() const { return currentWater; }
    int getMaxWater() const { return maxWater; }
    
    // Range queries
    std::vector<Position> getWateredTiles() const;
    bool watersTile(Position tilePos) const;
    int getWaterRange() const { return waterRange; }
    
    // Getters
    IrrigationType getType() const { return type; }
    Position getPosition() const { return position; }
    std::string getName() const { return name; }
    bool isAutomatic() const { return isAutomaticState; }
    bool isActive() const { return isActiveState; }
    
    // Control
    void setActive(bool active) { isActiveState = active; }
    bool toggle();
    
    // Upgrade
    bool canUpgrade() const;
    bool upgrade();
    
private:
    IrrigationType type;
    std::string name;
    Position position;
    int currentWater;
    int maxWater;
    int waterRange;
    bool isAutomaticState;
    bool isActiveState;
    int level;
    
    std::string escapeJson(const std::string& input) const;
    static const std::map<IrrigationType, IrrigationDefinition>& getDefinitions();
};

class Fence {
public:
    Fence();
    Fence(FenceType type, Position pos);
    ~Fence();
    
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Damage system
    void takeDamage(int amount);
    void repair(int amount);
    int getDurability() const { return durability; }
    int getMaxDurability() const { return maxDurability; }
    bool isBroken() const { return durability <= 0; }
    
    // Getters
    FenceType getType() const { return type; }
    Position getPosition() const { return position; }
    std::string getName() const { return name; }
    float getDefenseBonus() const { return defenseBonus; }
    
    // Gate functionality
    bool isGate() const { return isGateState; }
    void setAsGate(bool isGate) { isGateState = isGate; }
    bool isOpen() const { return isOpenState; }
    void toggleGate();
    
private:
    FenceType type;
    std::string name;
    Position position;
    int durability;
    int maxDurability;
    float defenseBonus;
    bool isGateState;
    bool isOpenState;
    
    std::string escapeJson(const std::string& input) const;
    static const std::map<FenceType, FenceDefinition>& getDefinitions();
};

class Path {
public:
    Path();
    Path(PathType type, Position pos);
    ~Path();
    
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Wear and tear
    void addWear(int amount);
    void repair();
    int getCondition() const { return condition; }
    bool needsRepair() const { return condition < 30; }
    
    // Getters
    PathType getType() const { return type; }
    Position getPosition() const { return position; }
    std::string getName() const { return name; }
    float getSpeedModifier() const { return speedModifier; }
    
    // Upgrade
    bool canUpgrade() const;
    bool upgrade(PathType newType);
    
private:
    PathType type;
    std::string name;
    Position position;
    int condition;
    float speedModifier;
    
    std::string escapeJson(const std::string& input) const;
    static const std::map<PathType, PathDefinition>& getDefinitions();
};

class FarmDecoration {
public:
    FarmDecoration();
    FarmDecoration(DecorationType type, Position pos);
    ~FarmDecoration();
    
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Getters
    DecorationType getType() const { return type; }
    Position getPosition() const { return position; }
    std::string getName() const { return name; }
    int getDecorationValue() const { return decorationValue; }
    
    // Buff system (for scarecrows, etc.)
    bool providesBuff() const { return providesBuffState; }
    std::string getBuffType() const { return buffType; }
    float getBuffRadius() const { return buffRadius; }
    std::vector<Position> getAffectedTiles() const;
    
    // Interaction
    bool interact();
    
private:
    DecorationType type;
    std::string name;
    Position position;
    int decorationValue;
    bool providesBuffState;
    std::string buffType;
    float buffRadius;
    int level;
    
    std::string escapeJson(const std::string& input) const;
    static const std::map<DecorationType, DecorationDefinition>& getDefinitions();
};

// Windmill and water wheel for power generation
enum class PowerGeneratorType {
    WINDMILL,
    WATER_WHEEL,
    SOLAR_PANEL,
    GEOTHERMAL
};

class PowerGenerator {
public:
    PowerGenerator();
    PowerGenerator(PowerGeneratorType type, Position pos);
    ~PowerGenerator();
    
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Power management
    int generatePower();
    int getCurrentOutput() const { return currentOutput; }
    int getMaxOutput() const { return maxOutput; }
    int getStoredEnergy() const { return storedEnergy; }
    int getMaxStoredEnergy() const { return maxStoredEnergy; }
    
    // Getters
    PowerGeneratorType getType() const { return type; }
    Position getPosition() const { return position; }
    std::string getName() const { return name; }
    bool isOperational() const { return isOperationalState; }
    
    // Efficiency based on conditions
    float getEfficiency() const;
    
private:
    PowerGeneratorType type;
    std::string name;
    Position position;
    int currentOutput;
    int maxOutput;
    int storedEnergy;
    int maxStoredEnergy;
    bool isOperationalState;
    int level;
    
    std::string escapeJson(const std::string& input) const;
};

// Compost bin for fertilizer production
class CompostBin {
public:
    CompostBin();
    CompostBin(Position pos);
    ~CompostBin();
    
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Composting
    bool addItem(const std::string& itemId, int quantity);
    bool isReady() const;
    std::pair<std::string, int> collectCompost();
    
    // Getters
    Position getPosition() const { return position; }
    int getProgress() const { return compostProgress; }
    int getMaxProgress() const { return maxCompostProgress; }
    std::string getCompostType() const { return compostType; }
    int getQuality() const { return quality; }
    
private:
    Position position;
    std::map<std::string, int> contents;
    int compostProgress;
    int maxCompostProgress;
    std::string compostType;
    int quality;
    bool isReadyState;
    
    std::string escapeJson(const std::string& input) const;
    void calculateCompostType();
};

// Manager for all farm infrastructure
class FarmInfrastructureManager {
public:
    static FarmInfrastructureManager& getInstance();
    
    // Irrigation management
    bool placeIrrigation(IrrigationType type, Position pos);
    bool removeIrrigation(Position pos);
    IrrigationSystem* getIrrigationAt(Position pos);
    std::vector<IrrigationSystem*> getAllIrrigation();
    std::vector<Position> getWateredTilesAt(Position pos) const;
    
    // Fence management
    bool placeFence(FenceType type, Position pos, bool isGate = false);
    bool removeFence(Position pos);
    Fence* getFenceAt(Position pos);
    std::vector<Fence*> getAllFences();
    
    // Path management
    bool placePath(PathType type, Position pos);
    bool removePath(Position pos);
    Path* getPathAt(Position pos);
    std::vector<Path*> getAllPaths();
    
    // Decoration management
    bool placeDecoration(DecorationType type, Position pos);
    bool removeDecoration(Position pos);
    FarmDecoration* getDecorationAt(Position pos);
    std::vector<FarmDecoration*> getAllDecorations();
    
    // Power management
    bool placePowerGenerator(PowerGeneratorType type, Position pos);
    bool removePowerGenerator(Position pos);
    PowerGenerator* getPowerGeneratorAt(Position pos);
    std::vector<PowerGenerator*> getAllPowerGenerators();
    int getTotalPowerOutput() const;
    
    // Compost management
    bool placeCompostBin(Position pos);
    bool removeCompostBin(Position pos);
    CompostBin* getCompostBinAt(Position pos);
    std::vector<CompostBin*> getAllCompostBins();
    
    // Global update
    void updateAll();
    
    // Serialization
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Definitions
    static const IrrigationDefinition& getIrrigationDefinition(IrrigationType type);
    static const FenceDefinition& getFenceDefinition(FenceType type);
    static const PathDefinition& getPathDefinition(PathType type);
    static const DecorationDefinition& getDecorationDefinition(DecorationType type);
    
private:
    FarmInfrastructureManager();
    ~FarmInfrastructureManager();
    FarmInfrastructureManager(const FarmInfrastructureManager&) = delete;
    FarmInfrastructureManager& operator=(const FarmInfrastructureManager&) = delete;
    
    std::map<int, std::map<int, std::unique_ptr<IrrigationSystem>>> irrigation;
    std::map<int, std::map<int, std::unique_ptr<Fence>>> fences;
    std::map<int, std::map<int, std::unique_ptr<Path>>> paths;
    std::map<int, std::map<int, std::unique_ptr<FarmDecoration>>> decorations;
    std::map<int, std::map<int, std::unique_ptr<PowerGenerator>> > powerGenerators;
    std::map<int, std::map<int, std::unique_ptr<CompostBin>>> compostBins;
    
    static const std::map<IrrigationType, IrrigationDefinition> irrigationDefinitions;
    static const std::map<FenceType, FenceDefinition> fenceDefinitions;
    static const std::map<PathType, PathDefinition> pathDefinitions;
    static const std::map<DecorationType, DecorationDefinition> decorationDefinitions;
};

#endif
