#ifndef FARMHOUSE_H
#define FARMHOUSE_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "Building.h"

// Room types for farmhouse expansions
enum class RoomType {
    KITCHEN,
    BEDROOM,
    CELLAR,
    GREENHOUSE_ROOM,
    NURSERY,
    LIBRARY,
    WORKSHOP,
    STORAGE_ROOM,
    BATHROOM,
    DINING_ROOM
};

// Wallpaper and flooring styles
enum class DecorStyle {
    RUSTIC,
    MODERN,
    CLASSIC,
    COUNTRY,
    LUXURY,
    SEASONAL_SPRING,
    SEASONAL_SUMMER,
    SEASONAL_FALL,
    SEASONAL_WINTER
};

struct RoomDefinition {
    std::string name;
    RoomType type;
    int width;
    int height;
    int upgradeCost;
    std::vector<std::string> requiredMaterials;
    std::map<std::string, int> materialQuantities;
    std::string description;
    bool isUnlocked;
    std::string unlockCondition;
};

struct DecorationSlot {
    std::string itemId;
    int quantity;
    Position position;
    bool isPlaced;
    
    DecorationSlot() : quantity(0), isPlaced(false) {}
};

class FarmhouseRoom {
public:
    FarmhouseRoom();
    FarmhouseRoom(RoomType type, const std::string& customName = "");
    ~FarmhouseRoom();
    
    // Core functionality
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Getters
    RoomType getType() const { return type; }
    std::string getName() const { return name; }
    int getLevel() const { return level; }
    bool isBuilt() const { return isBuiltState; }
    int getDecorationValue() const { return decorationValue; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // Room management
    bool build();
    bool upgrade();
    bool canUpgrade() const;
    
    // Decoration system
    bool addDecoration(const std::string& itemId, Position pos, int quantity = 1);
    bool removeDecoration(Position pos);
    DecorationSlot* getDecorationAt(Position pos);
    std::vector<DecorationSlot*> getAllDecorations();
    
    // Style customization
    bool setWallpaper(DecorStyle style);
    bool setFlooring(DecorStyle style);
    DecorStyle getWallpaper() const { return wallpaperStyle; }
    DecorStyle getFlooring() const { return flooringStyle; }
    
    // Interaction
    bool interact();
    std::string getInfo() const;
    
    // Events
    using EventCallback = std::function<void(const std::string& type, const std::string& data)>;
    void registerCallback(EventCallback callback);
    
private:
    RoomType type;
    std::string name;
    int level;
    bool isBuiltState;
    int width;
    int height;
    int decorationValue;
    
    // Style
    DecorStyle wallpaperStyle;
    DecorStyle flooringStyle;
    
    // Decorations
    std::map<int, std::map<int, DecorationSlot>> decorations;
    
    std::vector<EventCallback> callbacks;
    
    void emitEvent(const std::string& type, const std::string& data);
    std::string escapeJson(const std::string& input) const;
    static const std::map<RoomType, RoomDefinition>& getRoomDefinitions();
};

// Pet area types
enum class PetAreaType {
    DOG_HOUSE,
    CAT_TREE,
    BIRD_CAGE,
    FISH_TANK,
    HAMSTER_CAGE,
    RABBIT_HUTCH
};

struct PetAreaDefinition {
    std::string name;
    PetAreaType type;
    int capacity;
    int cost;
    std::map<std::string, int> materials;
    std::string description;
    bool allowsMultiple;
};

class PetArea {
public:
    PetArea();
    PetArea(PetAreaType type, Position pos);
    ~PetArea();
    
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Pet management
    bool addPet(const std::string& petId, const std::string& petName);
    bool removePet(const std::string& petId);
    bool hasPet(const std::string& petId) const;
    int getPetCount() const;
    int getMaxCapacity() const;
    
    // Getters
    PetAreaType getType() const { return type; }
    Position getPosition() const { return position; }
    std::string getName() const { return name; }
    int getComfortLevel() const { return comfortLevel; }
    
    // Comfort system
    void increaseComfort(int amount);
    void decreaseComfort(int amount);
    bool isHighComfort() const { return comfortLevel >= 80; }
    
    // Interaction
    bool interact();
    
private:
    PetAreaType type;
    std::string name;
    Position position;
    int comfortLevel;
    std::vector<std::pair<std::string, std::string>> pets; // (petId, petName)
    
    std::string escapeJson(const std::string& input) const;
    static const std::map<PetAreaType, PetAreaDefinition>& getPetAreaDefinitions();
};

// Storage upgrade types
enum class StorageType {
    CHEST_BASIC,      // 36 slots
    CHEST_LARGE,      // 72 slots
    REFRIGERATOR,     // 36 slots, preserves food
    WAREHOUSE,        // 144 slots
    VAULT,            // 216 slots, secure storage
    SPECIALTY_BOX     // Type-specific storage (seeds, ores, etc.)
};

class StorageUnit {
public:
    StorageUnit();
    StorageUnit(StorageType type, Position pos);
    ~StorageUnit();
    
    void update();
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Storage operations
    bool storeItem(const std::string& itemId, int quantity);
    std::pair<std::string, int> retrieveItem(const std::string& itemId, int quantity);
    int getItemCount(const std::string& itemId) const;
    int getTotalItems() const;
    int getUsedSlots() const;
    int getAvailableSlots() const;
    int getMaxSlots() const;
    
    // Get item list
    std::map<std::string, int> getAllItems() const { return items; }
    
    // Upgrade
    bool canUpgrade() const;
    bool upgrade();
    StorageType getStorageType() const { return type; }
    
    // Position
    Position getPosition() const { return position; }
    void setPosition(Position pos) { position = pos; }
    
    // Special features
    bool isPreserving() const { return isPreservingState; }
    bool isLocked() const { return isLockedState; }
    void setLocked(bool locked) { isLockedState = locked; }
    
private:
    StorageType type;
    Position position;
    std::map<std::string, int> items;
    int maxSlots;
    bool isPreservingState;
    bool isLockedState;
    
    std::string escapeJson(const std::string& input) const;
    static int getMaxSlotsForType(StorageType type);
};

// Manager for all farmhouse components
class FarmhouseManager {
public:
    static FarmhouseManager& getInstance();
    
    // Room management
    bool addRoom(RoomType type, const std::string& customName = "");
    bool removeRoom(const std::string& roomId);
    FarmhouseRoom* getRoom(const std::string& roomId);
    std::vector<FarmhouseRoom*> getAllRooms();
    std::vector<FarmhouseRoom*> getRoomsByType(RoomType type);
    
    // Pet area management
    bool placePetArea(PetAreaType type, Position pos);
    bool removePetArea(Position pos);
    PetArea* getPetAreaAt(Position pos);
    std::vector<PetArea*> getAllPetAreas();
    
    // Storage management
    bool placeStorage(StorageType type, Position pos);
    bool removeStorage(Position pos);
    StorageUnit* getStorageAt(Position pos);
    std::vector<StorageUnit*> getAllStorage();
    
    // Global update
    void updateAll();
    
    // Serialization
    std::string serialize() const;
    void deserialize(const std::string& json);
    
    // Definitions
    static const RoomDefinition& getRoomDefinition(RoomType type);
    static const PetAreaDefinition& getPetAreaDefinition(PetAreaType type);
    
private:
    FarmhouseManager();
    ~FarmhouseManager();
    FarmhouseManager(const FarmhouseManager&) = delete;
    FarmhouseManager& operator=(const FarmhouseManager&) = delete;
    
    std::map<std::string, std::unique_ptr<FarmhouseRoom>> rooms;
    std::map<int, std::map<int, std::unique_ptr<PetArea>>> petAreas;
    std::map<int, std::map<int, std::unique_ptr<StorageUnit>>> storageUnits;
    
    static const std::map<RoomType, RoomDefinition> roomDefinitions;
    static const std::map<PetAreaType, PetAreaDefinition> petAreaDefinitions;
};

#endif
