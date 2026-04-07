#ifndef BUILDING_H
#define BUILDING_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <queue>

struct Position {
    int x;
    int y;
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

enum class BuildingType {
    FARMHOUSE,
    BARN,
    COOP,
    GREENHOUSE,
    SHED,
    WELL,
    SILO,
    CUSTOM
};

enum class MachineType {
    SEED_MAKER,
    PRESERVES_JAR,
    CHEESE_PRESS,
    LOOM,
    MAYONNAISE_MACHINE,
    KEG,
    OIL_MAKER,
    FURNACE,
    CRAFTING_STATION,
    COMPOST_BIN
};

enum class FurnitureType {
    TABLE,
    CHAIR,
    BED,
    CHEST,
    SHELF,
    REFRIGERATOR,
    DECORATION,
    RUG,
    PAINTING,
    LAMP
};

enum class UpgradeTier {
    BASIC,
    IMPROVED,
    SUPERIOR,
    MASTERWORK,
    LEGENDARY
};

struct UpgradeSlot {
    std::string slotId;
    bool isOccupied;
    std::string itemId;
    int itemQuantity;
    int upgradeLevel;
    
    UpgradeSlot() : isOccupied(false), itemQuantity(0), upgradeLevel(0) {}
};

struct BuildingDefinition {
    std::string name;
    BuildingType type;
    int width;
    int height;
    int baseCost;
    std::vector<std::string> requiredMaterials;
    std::map<std::string, int> materialQuantities;
    std::vector<std::string> upgrades;
    std::string description;
    bool isUnlockable;
    std::string unlockCondition;
};

struct MachineRecipe {
    std::string inputItem;
    int inputQuantity;
    std::string outputItem;
    int outputQuantity;
    int processingTime; // in ticks
    float experienceGain;
    bool isUnlocked;
    std::string unlockRequirement;
};

struct MachineDefinition {
    std::string name;
    MachineType type;
    int capacity;
    int baseProcessingTime;
    float energyConsumption;
    std::vector<MachineRecipe> recipes;
    UpgradeTier maxUpgradeTier;
    std::string description;
};

struct FurnitureDefinition {
    std::string name;
    FurnitureType type;
    int width;
    int height;
    int cost;
    std::string craftRecipe;
    std::map<std::string, int> materials;
    bool isRotatable;
    std::string description;
};

class Building {
public:
    Building();
    Building(BuildingType type, Position pos, const std::string& customName = "");
    ~Building();
    
    // Core functionality
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Getters
    BuildingType getType() const { return type; }
    Position getPosition() const { return position; }
    std::string getName() const { return name; }
    int getLevel() const { return level; }
    UpgradeTier getUpgradeTier() const { return upgradeTier; }
    bool getIsUpgrading() const { return isUpgradingState; }
    int getUpgradeProgress() const { return upgradeProgress; }
    int getMaxUpgradeProgress() const { return maxUpgradeProgress; }
    const std::vector<UpgradeSlot>& getUpgradeSlots() const { return upgradeSlots; }
    
    // Setters
    void setName(const std::string& newName) { name = newName; }
    void setPosition(Position newPos) { position = newPos; }
    
    // Upgrade system
    bool canUpgrade() const;
    bool startUpgrade();
    void cancelUpgrade();
    bool addUpgradeItem(const std::string& itemId, int quantity);
    bool removeUpgradeItem(const std::string& itemId, int quantity);
    
    // Interaction
    bool interact();
    std::string getInfo() const;
    
    // Events
    using EventCallback = std::function<void(const std::string& type, const std::string& data)>;
    void registerCallback(EventCallback callback);
    
private:
    BuildingType type;
    std::string name;
    Position position;
    int level;
    UpgradeTier upgradeTier;
    bool isUpgradingState;
    int upgradeProgress;
    int maxUpgradeProgress;
    std::vector<UpgradeSlot> upgradeSlots;
    std::vector<EventCallback> callbacks;
    
    void emitEvent(const std::string& type, const std::string& data);
    void completeUpgrade();
    std::string escapeJson(const std::string& input) const;
};

class ProcessingMachine {
public:
    ProcessingMachine();
    ProcessingMachine(MachineType type, Position pos);
    ~ProcessingMachine();
    
    // Core processing
    void update();
    bool addItem(const std::string& itemId, int quantity);
    std::pair<std::string, int> collectOutput();
    void cancelProcessing();
    
    // State queries
    MachineType getType() const { return type; }
    Position getPosition() const { return position; }
    std::string getName() const { return name; }
    UpgradeTier getUpgradeTier() const { return upgradeTier; }
    int getLevel() const { return level; }
    bool isProcessing() const { return isProcessingState; }
    int getProcessingProgress() const { return processingProgress; }
    int getMaxProcessingTime() const { return currentMaxProcessingTime; }
    std::string getCurrentRecipe() const { return currentRecipeInput; }
    std::string getExpectedOutput() const { return expectedOutputItem; }
    int getExpectedOutputQuantity() const { return expectedOutputQuantity; }
    int getInputQueueSize() const { return static_cast<int>(inputQueue.size()); }
    int getCapacity() const { return capacity; }
    
    // Upgrade system
    bool canUpgrade() const;
    bool upgrade();
    int getUpgradeCost() const;
    
    // Recipe management
    std::vector<MachineRecipe> getAvailableRecipes() const;
    bool hasRecipe(const std::string& inputItem) const;
    MachineRecipe getRecipeForInput(const std::string& inputItem) const;
    
    // Efficiency bonuses
    float getSpeedBonus() const;
    float getQualityBonus() const;
    float getYieldBonus() const;
    
    // Serialization
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Events
    using EventCallback = std::function<void(const std::string& type, const std::string& data)>;
    void registerCallback(EventCallback callback);
    
private:
    MachineType type;
    std::string name;
    Position position;
    int level;
    UpgradeTier upgradeTier;
    int capacity;
    
    // Processing state
    bool isProcessingState;
    int processingProgress;
    int currentMaxProcessingTime;
    std::string currentRecipeInput;
    std::string expectedOutputItem;
    int expectedOutputQuantity;
    float currentQualityMultiplier;
    
    // Queue system
    struct QueuedItem {
        std::string itemId;
        int quantity;
        int priority;
    };
    std::queue<QueuedItem> inputQueue;
    std::string outputBuffer;
    int outputQuantity;
    
    std::vector<EventCallback> callbacks;
    
    void emitEvent(const std::string& type, const std::string& data);
    void processTick();
    void completeProcessing();
    void selectNextRecipe();
    std::string escapeJson(const std::string& input) const;
    static const std::map<MachineType, MachineDefinition>& getMachineDefinitions();
};

// Make machine definitions accessible to BuildingManager
namespace MachineDefs {
    const std::map<MachineType, MachineDefinition>& getDefinitions();
}

class Furniture {
public:
    Furniture();
    Furniture(FurnitureType type, Position pos, bool isRotated = false);
    ~Furniture();
    
    // Basic properties
    FurnitureType getType() const { return type; }
    Position getPosition() const { return position; }
    bool isRotated() const { return rotated; }
    std::string getName() const { return name; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // Actions
    void rotate();
    bool interact();
    bool storeItem(const std::string& itemId, int quantity);
    std::pair<std::string, int> retrieveItem(const std::string& itemId, int quantity);
    
    // Storage (for chests, refrigerators, etc.)
    int getStorageCapacity() const { return storageCapacity; }
    int getCurrentStorage() const { return currentStorage; }
    std::map<std::string, int> getStoredItems() const { return storedItems; }
    
    // Decoration value
    int getDecorationValue() const { return decorationValue; }
    
    // Serialization
    std::string serialize() const;
    void deserialize(const std::string& json);
    
private:
    FurnitureType type;
    std::string name;
    Position position;
    bool rotated;
    int width;
    int height;
    int decorationValue;
    
    // Storage
    int storageCapacity;
    int currentStorage;
    std::map<std::string, int> storedItems;
    
    std::string escapeJson(const std::string& input) const;
};

// Manager classes for global state
class BuildingManager {
public:
    static BuildingManager& getInstance();
    
    // Building management
    bool placeBuilding(BuildingType type, Position pos, const std::string& customName = "");
    bool removeBuilding(Position pos);
    Building* getBuildingAt(Position pos);
    std::vector<Building*> getAllBuildings();
    std::vector<Building*> getBuildingsByType(BuildingType type);
    
    // Machine management
    bool placeMachine(MachineType type, Position pos);
    bool removeMachine(Position pos);
    ProcessingMachine* getMachineAt(Position pos);
    std::vector<ProcessingMachine*> getAllMachines();
    
    // Furniture management
    bool placeFurniture(FurnitureType type, Position pos, bool rotated = false);
    bool removeFurniture(Position pos);
    Furniture* getFurnitureAt(Position pos);
    std::vector<Furniture*> getAllFurniture();
    
    // Definitions
    static const BuildingDefinition& getBuildingDefinition(BuildingType type);
    static const MachineDefinition& getMachineDefinition(MachineType type);
    static const FurnitureDefinition& getFurnitureDefinition(FurnitureType type);
    bool isValidBuildingPosition(BuildingType type, Position pos) const;
    bool isValidMachinePosition(MachineType type, Position pos) const;
    bool isValidFurniturePosition(FurnitureType type, Position pos, bool rotated) const;
    
    // Global update
    void updateAll();
    
    // Serialization
    std::string serialize() const;
    void deserialize(const std::string& json);
    
private:
    BuildingManager();
    ~BuildingManager();
    BuildingManager(const BuildingManager&) = delete;
    BuildingManager& operator=(const BuildingManager&) = delete;
    
    std::map<int, std::map<int, std::unique_ptr<Building>>> buildings;
    std::map<int, std::map<int, std::unique_ptr<ProcessingMachine>>> machines;
    std::map<int, std::map<int, std::unique_ptr<Furniture>>> furniture;
    
    static const std::map<BuildingType, BuildingDefinition> buildingDefinitions;
    static const std::map<MachineType, MachineDefinition> machineDefinitions;
    static const std::map<FurnitureType, FurnitureDefinition> furnitureDefinitions;
};

#endif
