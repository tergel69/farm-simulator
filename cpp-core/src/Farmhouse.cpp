#include "Farmhouse.h"
#include <sstream>
#include <algorithm>
#include <iostream>

// ============================================================================
// FarmhouseRoom Implementation
// ============================================================================

FarmhouseRoom::FarmhouseRoom() 
    : type(RoomType::BEDROOM), name("Room"), level(1), 
      isBuiltState(false), width(4), height(4), decorationValue(0),
      wallpaperStyle(DecorStyle::RUSTIC), flooringStyle(DecorStyle::RUSTIC) {
}

FarmhouseRoom::FarmhouseRoom(RoomType roomType, const std::string& customName)
    : type(roomType), level(1), isBuiltState(false), decorationValue(0),
      wallpaperStyle(DecorStyle::RUSTIC), flooringStyle(DecorStyle::RUSTIC) {
    
    const auto& def = getRoomDefinitions().at(type);
    width = def.width;
    height = def.height;
    
    if (!customName.empty()) {
        name = customName;
    } else {
        name = def.name;
    }
}

FarmhouseRoom::~FarmhouseRoom() {}

void FarmhouseRoom::update() {
    // Update decorations
    for (auto& rowPair : decorations) {
        for (auto& colPair : rowPair.second) {
            // Decoration logic can be expanded here
        }
    }
}

bool FarmhouseRoom::build() {
    if (isBuiltState) return false;
    
    isBuiltState = true;
    emitEvent("roomBuilt", "{\"room\":\"" + escapeJson(name) + "\",\"type\":" + 
              std::to_string(static_cast<int>(type)) + "}");
    return true;
}

bool FarmhouseRoom::canUpgrade() const {
    if (!isBuiltState) return false;
    return level < 5; // Max 5 levels
}

bool FarmhouseRoom::upgrade() {
    if (!canUpgrade()) return false;
    
    level++;
    decorationValue += 50;
    
    emitEvent("roomUpgraded", "{\"room\":\"" + escapeJson(name) + 
              "\",\"newLevel\":" + std::to_string(level) + "}");
    return true;
}

bool FarmhouseRoom::addDecoration(const std::string& itemId, Position pos, int quantity) {
    if (!isBuiltState) return false;
    
    decorations[pos.y][pos.x].itemId = itemId;
    decorations[pos.y][pos.x].quantity = quantity;
    decorations[pos.y][pos.x].position = pos;
    decorations[pos.y][pos.x].isPlaced = true;
    
    decorationValue += quantity * 5;
    
    emitEvent("decorationAdded", "{\"item\":\"" + escapeJson(itemId) + 
              "\",\"position\":{\"x\":" + std::to_string(pos.x) + 
              ",\"y\":" + std::to_string(pos.y) + "}}");
    return true;
}

bool FarmhouseRoom::removeDecoration(Position pos) {
    auto yIt = decorations.find(pos.y);
    if (yIt == decorations.end()) return false;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return false;
    
    decorationValue -= xIt->second.quantity * 5;
    yIt->second.erase(xIt);
    
    if (yIt->second.empty()) {
        decorations.erase(yIt);
    }
    
    return true;
}

DecorationSlot* FarmhouseRoom::getDecorationAt(Position pos) {
    auto yIt = decorations.find(pos.y);
    if (yIt == decorations.end()) return nullptr;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return nullptr;
    
    return &xIt->second;
}

std::vector<DecorationSlot*> FarmhouseRoom::getAllDecorations() {
    std::vector<DecorationSlot*> result;
    for (auto& rowPair : decorations) {
        for (auto& colPair : rowPair.second) {
            result.push_back(&colPair.second);
        }
    }
    return result;
}

bool FarmhouseRoom::setWallpaper(DecorStyle style) {
    if (!isBuiltState) return false;
    wallpaperStyle = style;
    emitEvent("wallpaperChanged", "{\"style\":" + std::to_string(static_cast<int>(style)) + "}");
    return true;
}

bool FarmhouseRoom::setFlooring(DecorStyle style) {
    if (!isBuiltState) return false;
    flooringStyle = style;
    emitEvent("flooringChanged", "{\"style\":" + std::to_string(static_cast<int>(style)) + "}");
    return true;
}

bool FarmhouseRoom::interact() {
    emitEvent("roomInteracted", "{\"room\":\"" + escapeJson(name) + 
              "\",\"type\":" + std::to_string(static_cast<int>(type)) + "}");
    return true;
}

std::string FarmhouseRoom::getInfo() const {
    std::ostringstream oss;
    oss << "{\"name\":\"" << escapeJson(name) << "\",";
    oss << "\"type\":" << static_cast<int>(type) << ",";
    oss << "\"level\":" << level << ",";
    oss << "\"isBuilt\":" << (isBuiltState ? "true" : "false") << ",";
    oss << "\"width\":" << width << ",";
    oss << "\"height\":" << height << ",";
    oss << "\"decorationValue\":" << decorationValue << ",";
    oss << "\"wallpaper\":" << static_cast<int>(wallpaperStyle) << ",";
    oss << "\"flooring\":" << static_cast<int>(flooringStyle) << ",";
    oss << "\"decorationCount\":" << getAllDecorations().size() << "}";
    return oss.str();
}

void FarmhouseRoom::registerCallback(EventCallback callback) {
    callbacks.push_back(callback);
}

void FarmhouseRoom::emitEvent(const std::string& type, const std::string& data) {
    for (auto& callback : callbacks) {
        callback(type, data);
    }
}

std::string FarmhouseRoom::serialize() const {
    std::ostringstream oss;
    oss << "{\"type\":" << static_cast<int>(type) << ",";
    oss << "\"name\":\"" << escapeJson(name) << "\",";
    oss << "\"level\":" << level << ",";
    oss << "\"isBuilt\":" << (isBuiltState ? "true" : "false") << ",";
    oss << "\"width\":" << width << ",";
    oss << "\"height\":" << height << ",";
    oss << "\"decorationValue\":" << decorationValue << ",";
    oss << "\"wallpaper\":" << static_cast<int>(wallpaperStyle) << ",";
    oss << "\"flooring\":" << static_cast<int>(flooringStyle) << ",";
    oss << "\"decorations\":[";
    
    bool first = true;
    for (const auto& rowPair : decorations) {
        for (const auto& colPair : rowPair.second) {
            if (!first) oss << ",";
            oss << "{\"itemId\":\"" << escapeJson(colPair.second.itemId) << "\",";
            oss << "\"quantity\":" << colPair.second.quantity << ",";
            oss << "\"position\":{\"x\":" << colPair.second.position.x 
                << ",\"y\":" << colPair.second.position.y << "},";
            oss << "\"isPlaced\":" << (colPair.second.isPlaced ? "true" : "false") << "}";
            first = false;
        }
    }
    oss << "]}";
    
    return oss.str();
}

void FarmhouseRoom::deserialize(const std::string& json) {
    auto findKey = [&](const std::string& key) -> size_t {
        return json.find("\"" + key + "\":");
    };
    
    auto extractInt = [&](size_t pos) -> int {
        if (pos == std::string::npos) return 0;
        size_t colonPos = json.find(':', pos);
        if (colonPos == std::string::npos) return 0;
        size_t start = colonPos + 1;
        while (start < json.size() && (json[start] == ' ' || json[start] == '\t')) start++;
        std::string numStr;
        while (start < json.size() && (isdigit(json[start]) || json[start] == '-')) {
            numStr += json[start++];
        }
        return numStr.empty() ? 0 : std::stoi(numStr);
    };
    
    auto extractString = [&](size_t pos) -> std::string {
        if (pos == std::string::npos) return "";
        size_t quoteStart = json.find('"', pos);
        if (quoteStart == std::string::npos) return "";
        quoteStart++;
        size_t quoteEnd = json.find('"', quoteStart);
        if (quoteEnd == std::string::npos) return "";
        return json.substr(quoteStart, quoteEnd - quoteStart);
    };
    
    size_t typePos = findKey("type");
    if (typePos != std::string::npos) {
        type = static_cast<RoomType>(extractInt(typePos));
    }
    
    size_t namePos = findKey("name");
    if (namePos != std::string::npos) {
        name = extractString(namePos);
    }
    
    size_t levelPos = findKey("level");
    if (levelPos != std::string::npos) {
        level = extractInt(levelPos);
    }
    
    size_t builtPos = findKey("isBuilt");
    if (builtPos != std::string::npos) {
        isBuiltState = (json.find("true", builtPos) < json.find("false", builtPos));
    }
    
    size_t wallpaperPos = findKey("wallpaper");
    if (wallpaperPos != std::string::npos) {
        wallpaperStyle = static_cast<DecorStyle>(extractInt(wallpaperPos));
    }
    
    size_t flooringPos = findKey("flooring");
    if (flooringPos != std::string::npos) {
        flooringStyle = static_cast<DecorStyle>(extractInt(flooringPos));
    }
}

std::string FarmhouseRoom::escapeJson(const std::string& input) const {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += c;
        }
    }
    return output;
}

const std::map<RoomType, RoomDefinition>& FarmhouseRoom::getRoomDefinitions() {
    static const std::map<RoomType, RoomDefinition> definitions = {
        {RoomType::KITCHEN, {"Kitchen", RoomType::KITCHEN, 5, 4, 500, {"Wood", "Stone"}, {{"Wood", 50}, {"Stone", 30}}, "Cooking area with stove and counter", false, ""}},
        {RoomType::BEDROOM, {"Bedroom", RoomType::BEDROOM, 4, 4, 300, {"Wood"}, {{"Wood", 40}}, "Sleeping quarters", false, ""}},
        {RoomType::CELLAR, {"Cellar", RoomType::CELLAR, 6, 5, 800, {"Stone", "Clay"}, {{"Stone", 100}, {"Clay", 50}}, "Underground storage for aging cheese and wine", false, "House Upgrade 2"}},
        {RoomType::GREENHOUSE_ROOM, {"Greenhouse Room", RoomType::GREENHOUSE_ROOM, 5, 4, 1000, {"Wood", "Glass"}, {{"Wood", 60}, {"Glass", 40}}, "Indoor growing space", false, "Complete Greenhouse Quest"}},
        {RoomType::NURSERY, {"Nursery", RoomType::NURSERY, 4, 3, 400, {"Wood", "Cloth"}, {{"Wood", 30}, {"Cloth", 20}}, "Room for children", false, "Marriage"}},
        {RoomType::LIBRARY, {"Library", RoomType::LIBRARY, 5, 4, 600, {"Wood", "Paper"}, {{"Wood", 50}, {"Paper", 30}}, "Collection of books and knowledge", false, "Donate 20 items to Museum"}},
        {RoomType::WORKSHOP, {"Workshop", RoomType::WORKSHOP, 5, 4, 700, {"Wood", "Iron"}, {{"Wood", 60}, {"Iron Bar", 20}}, "Crafting and repair area", false, ""}},
        {RoomType::STORAGE_ROOM, {"Storage Room", RoomType::STORAGE_ROOM, 4, 4, 350, {"Wood"}, {{"Wood", 45}}, "Extra storage space", false, ""}},
        {RoomType::BATHROOM, {"Bathroom", RoomType::BATHROOM, 3, 3, 450, {"Stone", "Copper"}, {{"Stone", 30}, {"Copper Bar", 10}}, "Bathing facility", false, "House Upgrade 1"}},
        {RoomType::DINING_ROOM, {"Dining Room", RoomType::DINING_ROOM, 5, 4, 550, {"Wood"}, {{"Wood", 55}}, "Area for meals and gatherings", false, ""}}
    };
    return definitions;
}

// ============================================================================
// PetArea Implementation
// ============================================================================

PetArea::PetArea()
    : type(PetAreaType::DOG_HOUSE), name("Pet Area"), comfortLevel(50) {
    position = {0, 0};
}

PetArea::PetArea(PetAreaType petAreaType, Position pos)
    : type(petAreaType), position(pos), comfortLevel(50) {
    
    const auto& def = getPetAreaDefinitions().at(type);
    name = def.name;
}

PetArea::~PetArea() {}

void PetArea::update() {
    // Comfort decays slowly over time
    if (comfortLevel > 0) {
        comfortLevel--;
    }
}

std::string PetArea::serialize() const {
    std::ostringstream oss;
    oss << "{\"type\":" << static_cast<int>(type) << ",";
    oss << "\"name\":\"" << escapeJson(name) << "\",";
    oss << "\"position\":{\"x\":" << position.x << ",\"y\":" << position.y << "},";
    oss << "\"comfortLevel\":" << comfortLevel << ",";
    oss << "\"petCount\":" << pets.size() << ",";
    oss << "\"pets\":[";
    
    bool first = true;
    for (const auto& pet : pets) {
        if (!first) oss << ",";
        oss << "{\"id\":\"" << escapeJson(pet.first) << "\",";
        oss << "\"name\":\"" << escapeJson(pet.second) << "\"}";
        first = false;
    }
    oss << "]}";
    
    return oss.str();
}

void PetArea::deserialize(const std::string& json) {
    auto findKey = [&](const std::string& key) -> size_t {
        return json.find("\"" + key + "\":");
    };
    
    auto extractInt = [&](size_t pos) -> int {
        if (pos == std::string::npos) return 0;
        size_t colonPos = json.find(':', pos);
        if (colonPos == std::string::npos) return 0;
        size_t start = colonPos + 1;
        while (start < json.size() && (json[start] == ' ' || json[start] == '\t')) start++;
        std::string numStr;
        while (start < json.size() && (isdigit(json[start]) || json[start] == '-')) {
            numStr += json[start++];
        }
        return numStr.empty() ? 0 : std::stoi(numStr);
    };
    
    auto extractString = [&](size_t pos) -> std::string {
        if (pos == std::string::npos) return "";
        size_t quoteStart = json.find('"', pos);
        if (quoteStart == std::string::npos) return "";
        quoteStart++;
        size_t quoteEnd = json.find('"', quoteStart);
        if (quoteEnd == std::string::npos) return "";
        return json.substr(quoteStart, quoteEnd - quoteStart);
    };
    
    size_t typePos = findKey("type");
    if (typePos != std::string::npos) {
        type = static_cast<PetAreaType>(extractInt(typePos));
    }
    
    size_t namePos = findKey("name");
    if (namePos != std::string::npos) {
        name = extractString(namePos);
    }
    
    size_t comfortPos = findKey("comfortLevel");
    if (comfortPos != std::string::npos) {
        comfortLevel = extractInt(comfortPos);
    }
}

bool PetArea::addPet(const std::string& petId, const std::string& petName) {
    const auto& def = getPetAreaDefinitions().at(type);
    if (static_cast<int>(pets.size()) >= def.capacity) return false;
    if (hasPet(petId)) return false;
    
    pets.push_back({petId, petName});
    return true;
}

bool PetArea::removePet(const std::string& petId) {
    for (auto it = pets.begin(); it != pets.end(); ++it) {
        if (it->first == petId) {
            pets.erase(it);
            return true;
        }
    }
    return false;
}

bool PetArea::hasPet(const std::string& petId) const {
    for (const auto& pet : pets) {
        if (pet.first == petId) return true;
    }
    return false;
}

int PetArea::getPetCount() const {
    return static_cast<int>(pets.size());
}

int PetArea::getMaxCapacity() const {
    const auto& def = getPetAreaDefinitions().at(type);
    return def.capacity;
}

void PetArea::increaseComfort(int amount) {
    comfortLevel = std::min(100, comfortLevel + amount);
}

void PetArea::decreaseComfort(int amount) {
    comfortLevel = std::max(0, comfortLevel - amount);
}

bool PetArea::interact() {
    increaseComfort(5);
    return true;
}

std::string PetArea::escapeJson(const std::string& input) const {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += c;
        }
    }
    return output;
}

const std::map<PetAreaType, PetAreaDefinition>& PetArea::getPetAreaDefinitions() {
    static const std::map<PetAreaType, PetAreaDefinition> definitions = {
        {PetAreaType::DOG_HOUSE, {"Dog House", PetAreaType::DOG_HOUSE, 1, 200, {{"Wood", 30}}, "Cozy home for your dog", false}},
        {PetAreaType::CAT_TREE, {"Cat Tree", PetAreaType::CAT_TREE, 1, 150, {{"Wood", 20}, {"Cloth", 10}}, "Multi-level play area for cats", false}},
        {PetAreaType::BIRD_CAGE, {"Bird Cage", PetAreaType::BIRD_CAGE, 3, 100, {{"Wood", 15}, {"Iron Bar", 5}}, "Home for pet birds", true}},
        {PetAreaType::FISH_TANK, {"Fish Tank", PetAreaType::FISH_TANK, 5, 250, {{"Glass", 10}, {"Stone", 20}}, "Aquarium for fish", true}},
        {PetAreaType::HAMSTER_CAGE, {"Hamster Cage", PetAreaType::HAMSTER_CAGE, 2, 80, {{"Wood", 10}, {"Glass", 5}}, "Small habitat for hamsters", true}},
        {PetAreaType::RABBIT_HUTCH, {"Rabbit Hutch", PetAreaType::RABBIT_HUTCH, 2, 120, {{"Wood", 25}}, "Outdoor hutch for rabbits", false}}
    };
    return definitions;
}

// ============================================================================
// StorageUnit Implementation
// ============================================================================

StorageUnit::StorageUnit()
    : type(StorageType::CHEST_BASIC), maxSlots(36), 
      isPreservingState(false), isLockedState(false) {
    position = {0, 0};
}

StorageUnit::StorageUnit(StorageType storageType, Position pos)
    : type(storageType), position(pos), 
      isPreservingState(false), isLockedState(false) {
    maxSlots = getMaxSlotsForType(type);
    isPreservingState = (type == StorageType::REFRIGERATOR);
}

StorageUnit::~StorageUnit() {}

void StorageUnit::update() {
    // Preservation logic for refrigerator
    if (isPreservingState) {
        // Items stay fresh longer
    }
}

std::string StorageUnit::serialize() const {
    std::ostringstream oss;
    oss << "{\"type\":" << static_cast<int>(type) << ",";
    oss << "\"position\":{\"x\":" << position.x << ",\"y\":" << position.y << "},";
    oss << "\"maxSlots\":" << maxSlots << ",";
    oss << "\"isPreserving\":" << (isPreservingState ? "true" : "false") << ",";
    oss << "\"isLocked\":" << (isLockedState ? "true" : "false") << ",";
    oss << "\"items\":{";
    
    bool first = true;
    for (const auto& item : items) {
        if (!first) oss << ",";
        oss << "\"" << escapeJson(item.first) << "\":" << item.second;
        first = false;
    }
    oss << "}}";
    
    return oss.str();
}

void StorageUnit::deserialize(const std::string& json) {
    auto findKey = [&](const std::string& key) -> size_t {
        return json.find("\"" + key + "\":");
    };
    
    auto extractInt = [&](size_t pos) -> int {
        if (pos == std::string::npos) return 0;
        size_t colonPos = json.find(':', pos);
        if (colonPos == std::string::npos) return 0;
        size_t start = colonPos + 1;
        while (start < json.size() && (json[start] == ' ' || json[start] == '\t')) start++;
        std::string numStr;
        while (start < json.size() && (isdigit(json[start]) || json[start] == '-')) {
            numStr += json[start++];
        }
        return numStr.empty() ? 0 : std::stoi(numStr);
    };
    
    size_t typePos = findKey("type");
    if (typePos != std::string::npos) {
        type = static_cast<StorageType>(extractInt(typePos));
        maxSlots = getMaxSlotsForType(type);
    }
    
    size_t preservingPos = findKey("isPreserving");
    if (preservingPos != std::string::npos) {
        isPreservingState = (json.find("true", preservingPos) < json.find("false", preservingPos));
    }
    
    size_t lockedPos = findKey("isLocked");
    if (lockedPos != std::string::npos) {
        isLockedState = (json.find("true", lockedPos) < json.find("false", lockedPos));
    }
}

bool StorageUnit::storeItem(const std::string& itemId, int quantity) {
    if (getUsedSlots() >= maxSlots) return false;
    if (quantity <= 0) return false;
    
    items[itemId] += quantity;
    return true;
}

std::pair<std::string, int> StorageUnit::retrieveItem(const std::string& itemId, int quantity) {
    auto it = items.find(itemId);
    if (it == items.end()) return {"", 0};
    
    int actualQuantity = std::min(quantity, it->second);
    it->second -= actualQuantity;
    
    if (it->second <= 0) {
        items.erase(it);
    }
    
    return {itemId, actualQuantity};
}

int StorageUnit::getItemCount(const std::string& itemId) const {
    auto it = items.find(itemId);
    if (it == items.end()) return 0;
    return it->second;
}

int StorageUnit::getTotalItems() const {
    int total = 0;
    for (const auto& item : items) {
        total += item.second;
    }
    return total;
}

int StorageUnit::getUsedSlots() const {
    return static_cast<int>(items.size());
}

int StorageUnit::getAvailableSlots() const {
    return maxSlots - getUsedSlots();
}

int StorageUnit::getMaxSlots() const {
    return maxSlots;
}

bool StorageUnit::canUpgrade() const {
    switch (type) {
        case StorageType::CHEST_BASIC:
        case StorageType::CHEST_LARGE:
        case StorageType::WAREHOUSE:
            return true;
        default:
            return false;
    }
}

bool StorageUnit::upgrade() {
    if (!canUpgrade()) return false;
    
    switch (type) {
        case StorageType::CHEST_BASIC:
            type = StorageType::CHEST_LARGE;
            break;
        case StorageType::CHEST_LARGE:
            type = StorageType::WAREHOUSE;
            break;
        case StorageType::WAREHOUSE:
            type = StorageType::VAULT;
            break;
        default:
            return false;
    }
    
    maxSlots = getMaxSlotsForType(type);
    return true;
}

int StorageUnit::getMaxSlotsForType(StorageType storageType) {
    switch (storageType) {
        case StorageType::CHEST_BASIC: return 36;
        case StorageType::CHEST_LARGE: return 72;
        case StorageType::REFRIGERATOR: return 36;
        case StorageType::WAREHOUSE: return 144;
        case StorageType::VAULT: return 216;
        case StorageType::SPECIALTY_BOX: return 24;
        default: return 36;
    }
}

std::string StorageUnit::escapeJson(const std::string& input) const {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += c;
        }
    }
    return output;
}

// ============================================================================
// FarmhouseManager Implementation
// ============================================================================

FarmhouseManager& FarmhouseManager::getInstance() {
    static FarmhouseManager instance;
    return instance;
}

FarmhouseManager::FarmhouseManager() {}

FarmhouseManager::~FarmhouseManager() {}

bool FarmhouseManager::addRoom(RoomType type, const std::string& customName) {
    std::string roomId = "room_" + std::to_string(rooms.size());
    rooms[roomId] = std::make_unique<FarmhouseRoom>(type, customName);
    return true;
}

bool FarmhouseManager::removeRoom(const std::string& roomId) {
    auto it = rooms.find(roomId);
    if (it == rooms.end()) return false;
    
    rooms.erase(it);
    return true;
}

FarmhouseRoom* FarmhouseManager::getRoom(const std::string& roomId) {
    auto it = rooms.find(roomId);
    if (it == rooms.end()) return nullptr;
    
    return it->second.get();
}

std::vector<FarmhouseRoom*> FarmhouseManager::getAllRooms() {
    std::vector<FarmhouseRoom*> result;
    for (auto& pair : rooms) {
        result.push_back(pair.second.get());
    }
    return result;
}

std::vector<FarmhouseRoom*> FarmhouseManager::getRoomsByType(RoomType type) {
    std::vector<FarmhouseRoom*> result;
    for (auto& pair : rooms) {
        if (pair.second->getType() == type) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

bool FarmhouseManager::placePetArea(PetAreaType type, Position pos) {
    if (petAreas.count(pos.y) && petAreas.at(pos.y).count(pos.x)) return false;
    
    petAreas[pos.y][pos.x] = std::make_unique<PetArea>(type, pos);
    return true;
}

bool FarmhouseManager::removePetArea(Position pos) {
    auto yIt = petAreas.find(pos.y);
    if (yIt == petAreas.end()) return false;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return false;
    
    yIt->second.erase(xIt);
    if (yIt->second.empty()) {
        petAreas.erase(yIt);
    }
    return true;
}

PetArea* FarmhouseManager::getPetAreaAt(Position pos) {
    auto yIt = petAreas.find(pos.y);
    if (yIt == petAreas.end()) return nullptr;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return nullptr;
    
    return xIt->second.get();
}

std::vector<PetArea*> FarmhouseManager::getAllPetAreas() {
    std::vector<PetArea*> result;
    for (auto& rowPair : petAreas) {
        for (auto& colPair : rowPair.second) {
            result.push_back(colPair.second.get());
        }
    }
    return result;
}

bool FarmhouseManager::placeStorage(StorageType type, Position pos) {
    if (storageUnits.count(pos.y) && storageUnits.at(pos.y).count(pos.x)) return false;
    
    storageUnits[pos.y][pos.x] = std::make_unique<StorageUnit>(type, pos);
    return true;
}

bool FarmhouseManager::removeStorage(Position pos) {
    auto yIt = storageUnits.find(pos.y);
    if (yIt == storageUnits.end()) return false;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return false;
    
    yIt->second.erase(xIt);
    if (yIt->second.empty()) {
        storageUnits.erase(yIt);
    }
    return true;
}

StorageUnit* FarmhouseManager::getStorageAt(Position pos) {
    auto yIt = storageUnits.find(pos.y);
    if (yIt == storageUnits.end()) return nullptr;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return nullptr;
    
    return xIt->second.get();
}

std::vector<StorageUnit*> FarmhouseManager::getAllStorage() {
    std::vector<StorageUnit*> result;
    for (auto& rowPair : storageUnits) {
        for (auto& colPair : rowPair.second) {
            result.push_back(colPair.second.get());
        }
    }
    return result;
}

void FarmhouseManager::updateAll() {
    for (auto& pair : rooms) {
        pair.second->update();
    }
    
    for (auto& rowPair : petAreas) {
        for (auto& colPair : rowPair.second) {
            colPair->update();
        }
    }
    
    for (auto& rowPair : storageUnits) {
        for (auto& colPair : rowPair.second) {
            colPair->update();
        }
    }
}

std::string FarmhouseManager::serialize() const {
    std::ostringstream oss;
    oss << "{\"rooms\":[";
    
    bool first = true;
    for (const auto& pair : rooms) {
        if (!first) oss << ",";
        oss << "{\"id\":\"" << pair.first << "\",";
        oss << pair.second->serialize() << "}";
        first = false;
    }
    
    oss << "],\"petAreas\":[";
    first = true;
    for (const auto& rowPair : petAreas) {
        for (const auto& colPair : rowPair.second) {
            if (!first) oss << ",";
            oss << colPair->serialize();
            first = false;
        }
    }
    
    oss << "],\"storage\":[";
    first = true;
    for (const auto& rowPair : storageUnits) {
        for (const auto& colPair : rowPair.second) {
            if (!first) oss << ",";
            oss << colPair->serialize();
            first = false;
        }
    }
    
    oss << "]}";
    return oss.str();
}

void FarmhouseManager::deserialize(const std::string& json) {
    // Simplified deserialization - would need full JSON parser for production
}

const RoomDefinition& FarmhouseManager::getRoomDefinition(RoomType type) {
    return FarmhouseRoom::getRoomDefinitions().at(type);
}

const PetAreaDefinition& FarmhouseManager::getPetAreaDefinition(PetAreaType type) {
    return PetArea::getPetAreaDefinitions().at(type);
}

// Initialize static members
const std::map<RoomType, RoomDefinition> FarmhouseManager::roomDefinitions = {};
const std::map<PetAreaType, PetAreaDefinition> FarmhouseManager::petAreaDefinitions = {};
