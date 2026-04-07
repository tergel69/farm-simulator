#include "Building.h"
#include <sstream>
#include <algorithm>
#include <iostream>

// ============================================================================
// Building Implementation
// ============================================================================

Building::Building() 
    : type(BuildingType::CUSTOM), name("Custom Building"), level(1), 
      upgradeTier(UpgradeTier::BASIC), isUpgradingState(false), 
      upgradeProgress(0), maxUpgradeProgress(100) {
    position = {0, 0};
}

Building::Building(BuildingType buildingType, Position pos, const std::string& customName)
    : type(buildingType), position(pos), level(1), 
      upgradeTier(UpgradeTier::BASIC), isUpgradingState(false),
      upgradeProgress(0), maxUpgradeProgress(100) {
    
    if (!customName.empty()) {
        name = customName;
    } else {
        const auto& defs = BuildingManager::getBuildingDefinition(type);
        name = defs.name;
    }
}

Building::~Building() {}

void Building::update() {
    if (isUpgradingState) {
        upgradeProgress++;
        if (upgradeProgress >= maxUpgradeProgress) {
            completeUpgrade();
        }
    }
}

bool Building::canUpgrade() const {
    if (isUpgradingState) return false;
    if (upgradeTier == UpgradeTier::LEGENDARY) return false;
    return true;
}

bool Building::startUpgrade() {
    if (!canUpgrade()) return false;
    
    isUpgradingState = true;
    upgradeProgress = 0;
    
    // Calculate upgrade time based on tier
    switch (upgradeTier) {
        case UpgradeTier::BASIC: maxUpgradeProgress = 100; break;
        case UpgradeTier::IMPROVED: maxUpgradeProgress = 200; break;
        case UpgradeTier::SUPERIOR: maxUpgradeProgress = 300; break;
        case UpgradeTier::MASTERWORK: maxUpgradeProgress = 500; break;
        default: return false;
    }
    
    emitEvent("upgradeStarted", "{\"building\":\"" + escapeJson(name) + "\",\"tier\":" + 
              std::to_string(static_cast<int>(upgradeTier)) + "}");
    return true;
}

void Building::cancelUpgrade() {
    if (!isUpgradingState) return;
    
    isUpgradingState = false;
    upgradeProgress = 0;
    emitEvent("upgradeCancelled", "{\"building\":\"" + escapeJson(name) + "\"}");
}

bool Building::addUpgradeItem(const std::string& itemId, int quantity) {
    if (!isUpgradingState) return false;
    
    for (auto& slot : upgradeSlots) {
        if (!slot.isOccupied) {
            slot.isOccupied = true;
            slot.itemId = itemId;
            slot.itemQuantity = quantity;
            slot.upgradeLevel++;
            emitEvent("upgradeItemAdded", "{\"item\":\"" + escapeJson(itemId) + 
                      "\",\"quantity\":" + std::to_string(quantity) + "}");
            return true;
        }
    }
    return false;
}

bool Building::removeUpgradeItem(const std::string& itemId, int quantity) {
    for (auto& slot : upgradeSlots) {
        if (slot.isOccupied && slot.itemId == itemId) {
            slot.itemQuantity -= quantity;
            if (slot.itemQuantity <= 0) {
                slot.isOccupied = false;
                slot.itemId = "";
            }
            return true;
        }
    }
    return false;
}

void Building::completeUpgrade() {
    isUpgradingState = false;
    upgradeProgress = 0;
    level++;
    
    // Advance tier
    int currentTier = static_cast<int>(upgradeTier);
    if (currentTier < static_cast<int>(UpgradeTier::LEGENDARY)) {
        upgradeTier = static_cast<UpgradeTier>(currentTier + 1);
    }
    
    emitEvent("upgradeComplete", "{\"building\":\"" + escapeJson(name) + 
              "\",\"newLevel\":" + std::to_string(level) + 
              ",\"newTier\":" + std::to_string(static_cast<int>(upgradeTier)) + "}");
}

bool Building::interact() {
    emitEvent("buildingInteracted", "{\"building\":\"" + escapeJson(name) + 
              "\",\"type\":\"" + std::to_string(static_cast<int>(type)) + "\"}");
    return true;
}

std::string Building::getInfo() const {
    std::ostringstream oss;
    oss << "{\"name\":\"" << escapeJson(name) << "\",";
    oss << "\"type\":" << static_cast<int>(type) << ",";
    oss << "\"level\":" << level << ",";
    oss << "\"tier\":" << static_cast<int>(upgradeTier) << ",";
    oss << "\"isUpgradingState\":" << (isUpgradingState ? "true" : "false") << ",";
    oss << "\"upgradeProgress\":" << upgradeProgress << ",";
    oss << "\"maxUpgradeProgress\":" << maxUpgradeProgress << ",";
    oss << "\"position\":{\"x\":" << position.x << ",\"y\":" << position.y << "}}";
    return oss.str();
}

void Building::registerCallback(EventCallback callback) {
    callbacks.push_back(callback);
}

void Building::emitEvent(const std::string& type, const std::string& data) {
    for (auto& callback : callbacks) {
        callback(type, data);
    }
}

std::string Building::serialize() const {
    std::ostringstream oss;
    oss << "{\"type\":" << static_cast<int>(type) << ",";
    oss << "\"name\":\"" << escapeJson(name) << "\",";
    oss << "\"position\":{\"x\":" << position.x << ",\"y\":" << position.y << "},";
    oss << "\"level\":" << level << ",";
    oss << "\"tier\":" << static_cast<int>(upgradeTier) << ",";
    oss << "\"isUpgradingState\":" << (isUpgradingState ? "true" : "false") << ",";
    oss << "\"upgradeProgress\":" << upgradeProgress << ",";
    oss << "\"upgradeSlots\":[";
    
    bool first = true;
    for (const auto& slot : upgradeSlots) {
        if (!first) oss << ",";
        oss << "{\"occupied\":" << (slot.isOccupied ? "true" : "false");
        if (slot.isOccupied) {
            oss << ",\"item\":\"" << escapeJson(slot.itemId) << "\"";
            oss << ",\"quantity\":" << slot.itemQuantity;
            oss << ",\"level\":" << slot.upgradeLevel;
        }
        oss << "}";
        first = false;
    }
    oss << "]}";
    
    return oss.str();
}

void Building::deserialize(const std::string& json) {
    // Simple JSON parsing - in production, use a proper JSON library
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
        type = static_cast<BuildingType>(extractInt(typePos));
    }
    
    size_t namePos = findKey("name");
    if (namePos != std::string::npos) {
        name = extractString(namePos);
    }
    
    size_t levelPos = findKey("level");
    if (levelPos != std::string::npos) {
        level = extractInt(levelPos);
    }
    
    size_t tierPos = findKey("tier");
    if (tierPos != std::string::npos) {
        upgradeTier = static_cast<UpgradeTier>(extractInt(tierPos));
    }
    
    size_t upgradingPos = findKey("isUpgradingState");
    if (upgradingPos != std::string::npos) {
        isUpgradingState = (json.find("true", upgradingPos) < json.find("false", upgradingPos));
    }
    
    size_t progressPos = findKey("upgradeProgress");
    if (progressPos != std::string::npos) {
        upgradeProgress = extractInt(progressPos);
    }
}

std::string Building::escapeJson(const std::string& input) const {
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
// ProcessingMachine Implementation
// ============================================================================

ProcessingMachine::ProcessingMachine()
    : type(MachineType::SEED_MAKER), name("Machine"), level(1),
      upgradeTier(UpgradeTier::BASIC), capacity(5),
      isProcessingState(false), processingProgress(0),
      currentMaxProcessingTime(100), expectedOutputQuantity(0),
      currentQualityMultiplier(1.0f), outputQuantity(0) {
    position = {0, 0};
}

ProcessingMachine::ProcessingMachine(MachineType machineType, Position pos)
    : type(machineType), position(pos), level(1),
      upgradeTier(UpgradeTier::BASIC), isProcessingState(false),
      processingProgress(0), expectedOutputQuantity(0),
      currentQualityMultiplier(1.0f), outputQuantity(0) {
    
    const auto& def = MachineDefs::getDefinitions().at(type);
    name = def.name;
    capacity = def.capacity;
    currentMaxProcessingTime = def.baseProcessingTime;
}

ProcessingMachine::~ProcessingMachine() {}

void ProcessingMachine::update() {
    if (isProcessingState) {
        processTick();
    } else if (!inputQueue.empty()) {
        selectNextRecipe();
    }
}

void ProcessingMachine::processTick() {
    if (!isProcessingState) return;
    
    processingProgress++;
    
    if (processingProgress >= currentMaxProcessingTime) {
        completeProcessing();
    }
}

void ProcessingMachine::completeProcessing() {
    isProcessingState = false;
    processingProgress = 0;
    
    // Apply quality and yield bonuses
    float finalYield = expectedOutputQuantity * (1.0f + getYieldBonus());
    outputBuffer = expectedOutputItem;
    outputQuantity = static_cast<int>(finalYield);
    
    emitEvent("processingComplete", "{\"machine\":\"" + escapeJson(name) + 
              "\",\"output\":\"" + escapeJson(outputBuffer) + 
              "\",\"quantity\":" + std::to_string(outputQuantity) + "}");
    
    // Remove processed item from queue
    if (!inputQueue.empty()) {
        inputQueue.pop();
    }
}

void ProcessingMachine::selectNextRecipe() {
    if (inputQueue.empty()) return;
    
    const auto& queuedItem = inputQueue.front();
    
    if (hasRecipe(queuedItem.itemId)) {
        MachineRecipe recipe = getRecipeForInput(queuedItem.itemId);
        
        currentRecipeInput = recipe.inputItem;
        expectedOutputItem = recipe.outputItem;
        expectedOutputQuantity = recipe.outputQuantity;
        currentMaxProcessingTime = static_cast<int>(recipe.processingTime * (1.0f - getSpeedBonus()));
        currentQualityMultiplier = 1.0f + getQualityBonus();
        
        isProcessingState = true;
        processingProgress = 0;
        
        emitEvent("processingStarted", "{\"machine\":\"" + escapeJson(name) + 
                  "\",\"input\":\"" + escapeJson(currentRecipeInput) + 
                  "\",\"output\":\"" + escapeJson(expectedOutputItem) + "\"}");
    } else {
        // No recipe for this item, remove it
        inputQueue.pop();
    }
}

bool ProcessingMachine::addItem(const std::string& itemId, int quantity) {
    if (!hasRecipe(itemId)) return false;
    if (static_cast<int>(inputQueue.size()) >= capacity) return false;
    
    QueuedItem item;
    item.itemId = itemId;
    item.quantity = quantity;
    item.priority = 0;
    
    inputQueue.push(item);
    
    emitEvent("itemAdded", "{\"machine\":\"" + escapeJson(name) + 
              "\",\"item\":\"" + escapeJson(itemId) + 
              "\",\"quantity\":" + std::to_string(quantity) + "}");
    
    return true;
}

std::pair<std::string, int> ProcessingMachine::collectOutput() {
    if (outputBuffer.empty() || outputQuantity <= 0) {
        return {"", 0};
    }
    
    std::pair<std::string, int> result = {outputBuffer, outputQuantity};
    outputBuffer = "";
    outputQuantity = 0;
    
    return result;
}

void ProcessingMachine::cancelProcessing() {
    if (!isProcessingState) return;
    
    isProcessingState = false;
    processingProgress = 0;
    
    emitEvent("processingCancelled", "{\"machine\":\"" + escapeJson(name) + "\"}");
}

bool ProcessingMachine::canUpgrade() const {
    int currentTier = static_cast<int>(upgradeTier);
    const auto& def = MachineDefs::getDefinitions().at(type);
    int maxTier = static_cast<int>(def.maxUpgradeTier);
    return currentTier < maxTier;
}

bool ProcessingMachine::upgrade() {
    if (!canUpgrade()) return false;
    
    int currentTier = static_cast<int>(upgradeTier);
    upgradeTier = static_cast<UpgradeTier>(currentTier + 1);
    level++;
    
    // Recalculate processing time with new bonuses
    if (isProcessingState && !currentRecipeInput.empty()) {
        MachineRecipe recipe = getRecipeForInput(currentRecipeInput);
        currentMaxProcessingTime = static_cast<int>(recipe.processingTime * (1.0f - getSpeedBonus()));
    }
    
    emitEvent("machineUpgraded", "{\"machine\":\"" + escapeJson(name) + 
              "\",\"newLevel\":" + std::to_string(level) + 
              ",\"newTier\":" + std::to_string(static_cast<int>(upgradeTier)) + "}");
    
    return true;
}

int ProcessingMachine::getUpgradeCost() const {
    int baseCost = 100;
    int tier = static_cast<int>(upgradeTier);
    return baseCost * (tier + 1) * level;
}

std::vector<MachineRecipe> ProcessingMachine::getAvailableRecipes() const {
    const auto& def = MachineDefs::getDefinitions().at(type);
    std::vector<MachineRecipe> available;
    
    for (const auto& recipe : def.recipes) {
        if (recipe.isUnlocked) {
            available.push_back(recipe);
        }
    }
    
    return available;
}

bool ProcessingMachine::hasRecipe(const std::string& inputItem) const {
    const auto& def = MachineDefs::getDefinitions().at(type);
    for (const auto& recipe : def.recipes) {
        if (recipe.inputItem == inputItem && recipe.isUnlocked) {
            return true;
        }
    }
    return false;
}

MachineRecipe ProcessingMachine::getRecipeForInput(const std::string& inputItem) const {
    const auto& def = MachineDefs::getDefinitions().at(type);
    for (const auto& recipe : def.recipes) {
        if (recipe.inputItem == inputItem && recipe.isUnlocked) {
            return recipe;
        }
    }
    // Return empty recipe if not found
    return MachineRecipe{"", 0, "", 0, 0, 0.0f, false, ""};
}

float ProcessingMachine::getSpeedBonus() const {
    float bonus = 0.0f;
    switch (upgradeTier) {
        case UpgradeTier::BASIC: bonus = 0.0f; break;
        case UpgradeTier::IMPROVED: bonus = 0.1f; break;
        case UpgradeTier::SUPERIOR: bonus = 0.2f; break;
        case UpgradeTier::MASTERWORK: bonus = 0.3f; break;
        case UpgradeTier::LEGENDARY: bonus = 0.5f; break;
    }
    return bonus + (level * 0.01f);
}

float ProcessingMachine::getQualityBonus() const {
    float bonus = 0.0f;
    switch (upgradeTier) {
        case UpgradeTier::BASIC: bonus = 0.0f; break;
        case UpgradeTier::IMPROVED: bonus = 0.1f; break;
        case UpgradeTier::SUPERIOR: bonus = 0.25f; break;
        case UpgradeTier::MASTERWORK: bonus = 0.4f; break;
        case UpgradeTier::LEGENDARY: bonus = 0.6f; break;
    }
    return bonus + (level * 0.02f);
}

float ProcessingMachine::getYieldBonus() const {
    float bonus = 0.0f;
    switch (upgradeTier) {
        case UpgradeTier::BASIC: bonus = 0.0f; break;
        case UpgradeTier::IMPROVED: bonus = 0.1f; break;
        case UpgradeTier::SUPERIOR: bonus = 0.25f; break;
        case UpgradeTier::MASTERWORK: bonus = 0.4f; break;
        case UpgradeTier::LEGENDARY: bonus = 0.6f; break;
    }
    return bonus + (level * 0.02f);
}

std::string ProcessingMachine::serialize() const {
    std::ostringstream oss;
    oss << "{\"type\":" << static_cast<int>(type) << ",";
    oss << "\"name\":\"" << escapeJson(name) << "\",";
    oss << "\"position\":{\"x\":" << position.x << ",\"y\":" << position.y << "},";
    oss << "\"level\":" << level << ",";
    oss << "\"tier\":" << static_cast<int>(upgradeTier) << ",";
    oss << "\"capacity\":" << capacity << ",";
    oss << "\"isProcessing\":" << (isProcessingState ? "true" : "false") << ",";
    oss << "\"progress\":" << processingProgress << ",";
    oss << "\"maxProgress\":" << currentMaxProcessingTime << ",";
    oss << "\"queueSize\":" << inputQueue.size() << ",";
    oss << "\"outputBuffer\":\"" << escapeJson(outputBuffer) << "\",";
    oss << "\"outputQuantity\":" << outputQuantity << "}";
    
    return oss.str();
}

void ProcessingMachine::deserialize(const std::string& json) {
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
        type = static_cast<MachineType>(extractInt(typePos));
    }
    
    size_t namePos = findKey("name");
    if (namePos != std::string::npos) {
        name = extractString(namePos);
    }
    
    size_t levelPos = findKey("level");
    if (levelPos != std::string::npos) {
        level = extractInt(levelPos);
    }
    
    size_t tierPos = findKey("tier");
    if (tierPos != std::string::npos) {
        upgradeTier = static_cast<UpgradeTier>(extractInt(tierPos));
    }
}

void ProcessingMachine::registerCallback(EventCallback callback) {
    callbacks.push_back(callback);
}

void ProcessingMachine::emitEvent(const std::string& type, const std::string& data) {
    for (auto& callback : callbacks) {
        callback(type, data);
    }
}

std::string ProcessingMachine::escapeJson(const std::string& input) const {
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

namespace MachineDefs {
    const std::map<MachineType, MachineDefinition>& getDefinitions() {
    static const std::map<MachineType, MachineDefinition> definitions = {
        {MachineType::SEED_MAKER, {
            "Seed Maker", MachineType::SEED_MAKER, 10, 200, 0.5f,
            {
                {"Carrot", 1, "Carrot Seeds", 3, 200, 5.0f, true, ""},
                {"Tomato", 1, "Tomato Seeds", 3, 200, 5.0f, true, ""},
                {"Potato", 1, "Potato Seeds", 3, 200, 5.0f, true, ""},
                {"Wheat", 1, "Wheat Seeds", 3, 200, 5.0f, true, ""},
                {"Corn", 1, "Corn Seeds", 3, 200, 5.0f, true, ""},
                {"Pumpkin", 1, "Pumpkin Seeds", 3, 200, 5.0f, true, ""},
                {"Strawberry", 1, "Strawberry Seeds", 3, 200, 5.0f, true, ""},
                {"Blueberry", 1, "Blueberry Seeds", 3, 200, 5.0f, true, ""},
                {"GoldenWheat", 1, "Golden Wheat Seeds", 3, 300, 10.0f, true, "Farming Level 5"},
                {"CrystalBerry", 1, "Crystal Berry Seeds", 3, 300, 10.0f, true, "Mining Level 5"},
                {"AncientFruit", 1, "Ancient Fruit Seeds", 3, 300, 10.0f, true, "Foraging Level 5"}
            },
            UpgradeTier::LEGENDARY,
            "Converts harvested crops into multiple seeds for replanting"
        }},
        {MachineType::PRESERVES_JAR, {
            "Preserves Jar", MachineType::PRESERVES_JAR, 8, 400, 0.3f,
            {
                {"Carrot", 1, "Pickled Carrot", 1, 400, 3.0f, true, ""},
                {"Tomato", 1, "Tomato Jam", 1, 400, 3.0f, true, ""},
                {"Strawberry", 1, "Strawberry Jam", 1, 400, 4.0f, true, ""},
                {"Blueberry", 1, "Blueberry Jam", 1, 400, 4.0f, true, ""},
                {"Pumpkin", 1, "Pumpkin Pickles", 1, 400, 3.5f, true, ""},
                {"Pepper", 1, "Pickled Pepper", 1, 400, 3.5f, true, ""},
                {"Cucumber", 1, "Pickles", 1, 400, 3.0f, true, ""},
                {"Beet", 1, "Pickled Beet", 1, 400, 3.0f, true, ""}
            },
            UpgradeTier::MASTERWORK,
            "Preserves fruits and vegetables as jams and pickles"
        }},
        {MachineType::CHEESE_PRESS, {
            "Cheese Press", MachineType::CHEESE_PRESS, 5, 200, 0.4f,
            {
                {"Milk", 1, "Cheese", 1, 200, 5.0f, true, ""},
                {"Large Milk", 1, "Large Cheese", 1, 200, 7.0f, true, ""},
                {"Goat Milk", 1, "Goat Cheese", 1, 200, 6.0f, true, "Own a Goat"},
                {"Buffalo Milk", 1, "Buffalo Cheese", 1, 200, 8.0f, true, "Own a Buffalo"}
            },
            UpgradeTier::SUPERIOR,
            "Converts milk into various types of cheese"
        }},
        {MachineType::LOOM, {
            "Loom", MachineType::LOOM, 5, 240, 0.3f,
            {
                {"Wool", 1, "Cloth", 1, 240, 5.0f, true, ""},
                {"Fine Wool", 1, "Fine Cloth", 1, 240, 7.0f, true, ""},
                {"Silk", 1, "Silk Fabric", 1, 240, 8.0f, true, "Raise Silkworms"},
                {"Cotton", 1, "Cotton Cloth", 1, 240, 4.0f, true, ""}
            },
            UpgradeTier::MASTERWORK,
            "Weaves wool and other fibers into cloth"
        }},
        {MachineType::MAYONNAISE_MACHINE, {
            "Mayonnaise Machine", MachineType::MAYONNAISE_MACHINE, 10, 180, 0.3f,
            {
                {"Egg", 1, "Mayonnaise", 1, 180, 4.0f, true, ""},
                {"Large Egg", 1, "Large Mayonnaise", 1, 180, 6.0f, true, ""},
                {"Duck Egg", 1, "Duck Mayonnaise", 1, 180, 7.0f, true, "Own a Duck"},
                {"Void Egg", 1, "Void Mayonnaise", 1, 180, 10.0f, true, "Find Void Egg"}
            },
            UpgradeTier::SUPERIOR,
            "Processes eggs into mayonnaise"
        }},
        {MachineType::KEG, {
            "Keg", MachineType::KEG, 10, 1000, 0.5f,
            {
                {"Wheat", 1, "Beer", 1, 1000, 5.0f, true, ""},
                {"Corn", 1, "Whiskey", 1, 1000, 6.0f, true, ""},
                {"Strawberry", 1, "Wine", 1, 1000, 8.0f, true, ""},
                {"Blueberry", 1, "Blueberry Wine", 1, 1000, 9.0f, true, ""},
                {"AncientFruit", 1, "Ancient Wine", 1, 1000, 15.0f, true, ""},
                {"CrystalBerry", 1, "Crystal Liqueur", 1, 1000, 12.0f, true, ""},
                {"Apple", 1, "Cider", 1, 1000, 6.0f, true, ""},
                {"Peach", 1, "Peach Wine", 1, 1000, 7.0f, true, ""}
            },
            UpgradeTier::LEGENDARY,
            "Ferments fruits and grains into alcoholic beverages"
        }},
        {MachineType::OIL_MAKER, {
            "Oil Maker", MachineType::OIL_MAKER, 5, 300, 0.4f,
            {
                {"Sunflower", 1, "Sunflower Oil", 1, 300, 5.0f, true, ""},
                {"Corn", 1, "Corn Oil", 1, 300, 4.0f, true, ""},
                {"Olive", 1, "Olive Oil", 1, 300, 8.0f, true, "Grow Olives"},
                {"Sesame", 1, "Sesame Oil", 1, 300, 7.0f, true, "Grow Sesame"}
            },
            UpgradeTier::SUPERIOR,
            "Extracts oil from oilseed crops"
        }},
        {MachineType::FURNACE, {
            "Furnace", MachineType::FURNACE, 5, 120, 1.0f,
            {
                {"Copper Ore", 5, "Copper Bar", 1, 120, 3.0f, true, ""},
                {"Iron Ore", 5, "Iron Bar", 1, 180, 4.0f, true, ""},
                {"Gold Ore", 5, "Gold Bar", 1, 300, 6.0f, true, ""},
                {"Silver Ore", 5, "Silver Bar", 1, 240, 5.0f, true, ""},
                {"Coal", 1, "Refined Coal", 1, 60, 2.0f, true, ""}
            },
            UpgradeTier::MASTERWORK,
            "Smelts ores into metal bars"
        }},
        {MachineType::CRAFTING_STATION, {
            "Crafting Station", MachineType::CRAFTING_STATION, 3, 100, 0.2f,
            {},
            UpgradeTier::IMPROVED,
            "Basic crafting station for simple recipes"
        }},
        {MachineType::COMPOST_BIN, {
            "Compost Bin", MachineType::COMPOST_BIN, 10, 500, 0.1f,
            {
                {"Crop Waste", 5, "Fertilizer", 1, 500, 2.0f, true, ""},
                {"Withered Crop", 1, "Quality Fertilizer", 1, 500, 3.0f, true, ""},
                {"Food Scraps", 3, "Basic Fertilizer", 1, 500, 1.5f, true, ""},
                {"Leaves", 5, "Mulch", 1, 500, 1.0f, true, ""}
            },
            UpgradeTier::SUPERIOR,
            "Converts organic waste into fertilizer"
        }}
    };
    return definitions;
}


// ============================================================================
// BuildingManager Implementation
// ============================================================================

BuildingManager& BuildingManager::getInstance() {
    static BuildingManager instance;
    return instance;
}

BuildingManager::BuildingManager() {}

BuildingManager::~BuildingManager() {}

bool BuildingManager::placeBuilding(BuildingType type, Position pos, const std::string& customName) {
    if (!isValidBuildingPosition(type, pos)) return false;
    
    buildings[pos.y][pos.x] = std::make_unique<Building>(type, pos, customName);
    return true;
}

bool BuildingManager::removeBuilding(Position pos) {
    auto yIt = buildings.find(pos.y);
    if (yIt == buildings.end()) return false;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return false;
    
    yIt->second.erase(xIt);
    if (yIt->second.empty()) {
        buildings.erase(yIt);
    }
    return true;
}

Building* BuildingManager::getBuildingAt(Position pos) {
    auto yIt = buildings.find(pos.y);
    if (yIt == buildings.end()) return nullptr;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return nullptr;
    
    return xIt->second.get();
}

std::vector<Building*> BuildingManager::getAllBuildings() {
    std::vector<Building*> result;
    for (auto& rowPair : buildings) {
        for (auto& colPair : rowPair.second) {
            result.push_back(colPair.second.get());
        }
    }
    return result;
}

std::vector<Building*> BuildingManager::getBuildingsByType(BuildingType type) {
    std::vector<Building*> result;
    for (auto& rowPair : buildings) {
        for (auto& colPair : rowPair.second) {
            if (colPair.second->getType() == type) {
                result.push_back(colPair.second.get());
            }
        }
    }
    return result;
}

bool BuildingManager::placeMachine(MachineType type, Position pos) {
    if (!isValidMachinePosition(type, pos)) return false;
    
    machines[pos.y][pos.x] = std::make_unique<ProcessingMachine>(type, pos);
    return true;
}

bool BuildingManager::removeMachine(Position pos) {
    auto yIt = machines.find(pos.y);
    if (yIt == machines.end()) return false;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return false;
    
    yIt->second.erase(xIt);
    if (yIt->second.empty()) {
        machines.erase(yIt);
    }
    return true;
}

ProcessingMachine* BuildingManager::getMachineAt(Position pos) {
    auto yIt = machines.find(pos.y);
    if (yIt == machines.end()) return nullptr;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return nullptr;
    
    return xIt->second.get();
}

std::vector<ProcessingMachine*> BuildingManager::getAllMachines() {
    std::vector<ProcessingMachine*> result;
    for (auto& rowPair : machines) {
        for (auto& colPair : rowPair.second) {
            result.push_back(colPair.second.get());
        }
    }
    return result;
}

bool BuildingManager::placeFurniture(FurnitureType type, Position pos, bool rotated) {
    if (!isValidFurniturePosition(type, pos, rotated)) return false;
    
    furniture[pos.y][pos.x] = std::make_unique<Furniture>(type, pos, rotated);
    return true;
}

bool BuildingManager::removeFurniture(Position pos) {
    auto yIt = furniture.find(pos.y);
    if (yIt == furniture.end()) return false;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return false;
    
    yIt->second.erase(xIt);
    if (yIt->second.empty()) {
        furniture.erase(yIt);
    }
    return true;
}

Furniture* BuildingManager::getFurnitureAt(Position pos) {
    auto yIt = furniture.find(pos.y);
    if (yIt == furniture.end()) return nullptr;
    
    auto xIt = yIt->second.find(pos.x);
    if (xIt == yIt->second.end()) return nullptr;
    
    return xIt->second.get();
}

std::vector<Furniture*> BuildingManager::getAllFurniture() {
    std::vector<Furniture*> result;
    for (auto& rowPair : furniture) {
        for (auto& colPair : rowPair.second) {
            result.push_back(colPair.second.get());
        }
    }
    return result;
}

const BuildingDefinition& BuildingManager::getBuildingDefinition(BuildingType type) {
    static const std::map<BuildingType, BuildingDefinition> defs = {
        {BuildingType::FARMHOUSE, {"Farmhouse", BuildingType::FARMHOUSE, 5, 4, 1000, {}, {}, {}, "Your main home", false, ""}},
        {BuildingType::BARN, {"Barn", BuildingType::BARN, 4, 3, 500, {"Wood", "Stone"}, {{"Wood", 100}, {"Stone", 50}}, {"Barn Upgrade"}, "House livestock", false, ""}},
        {BuildingType::COOP, {"Coop", BuildingType::COOP, 3, 2, 300, {"Wood"}, {{"Wood", 50}}, {"Coop Upgrade"}, "House chickens and birds", false, ""}},
        {BuildingType::GREENHOUSE, {"Greenhouse", BuildingType::GREENHOUSE, 6, 4, 2000, {"Wood", "Glass"}, {{"Wood", 200}, {"Glass", 100}}, {}, "Grow crops year-round", true, "Complete Greenhouse Quest"}},
        {BuildingType::SHED, {"Shed", BuildingType::SHED, 3, 2, 200, {"Wood"}, {{"Wood", 30}}, {}, "Basic storage building", false, ""}},
        {BuildingType::WELL, {"Well", BuildingType::WELL, 1, 1, 100, {"Stone"}, {{"Stone", 20}}, {}, "Provides water source", false, ""}},
        {BuildingType::SILO, {"Silo", BuildingType::SILO, 2, 2, 400, {"Stone", "Clay"}, {{"Stone", 50}, {"Clay", 20}}, {}, "Stores animal feed", false, ""}}
    };
    return defs.at(type);
}

const MachineDefinition& BuildingManager::getMachineDefinition(MachineType type) {
    return MachineDefs::getDefinitions().at(type);
}

const FurnitureDefinition& BuildingManager::getFurnitureDefinition(FurnitureType type) {
    static const std::map<FurnitureType, FurnitureDefinition> defs = {
        {FurnitureType::TABLE, {"Table", FurnitureType::TABLE, 2, 1, 50, "Wood", {{"Wood", 10}}, true, "Basic table"}},
        {FurnitureType::CHAIR, {"Chair", FurnitureType::CHAIR, 1, 1, 30, "Wood", {{"Wood", 5}}, true, "Basic chair"}},
        {FurnitureType::BED, {"Bed", FurnitureType::BED, 2, 1, 100, "Wood", {{"Wood", 20}, {"Cloth", 5}}, false, "Sleeping bed"}},
        {FurnitureType::CHEST, {"Chest", FurnitureType::CHEST, 1, 1, 75, "Wood", {{"Wood", 15}}, false, "Storage chest (36 slots)"}},
        {FurnitureType::SHELF, {"Shelf", FurnitureType::SHELF, 1, 1, 60, "Wood", {{"Wood", 12}}, false, "Display shelf"}},
        {FurnitureType::REFRIGERATOR, {"Refrigerator", FurnitureType::REFRIGERATOR, 1, 1, 200, "Iron", {{"Iron Bar", 10}}, false, "Preserves food (36 slots)"}},
        {FurnitureType::DECORATION, {"Decoration", FurnitureType::DECORATION, 1, 1, 25, "Various", {}, false, "Decorative item"}},
        {FurnitureType::RUG, {"Rug", FurnitureType::RUG, 2, 2, 40, "Cloth", {{"Cloth", 8}}, false, "Floor rug"}},
        {FurnitureType::PAINTING, {"Painting", FurnitureType::PAINTING, 2, 1, 80, "Canvas", {{"Canvas", 1}, {"Paint", 3}}, false, "Wall painting"}},
        {FurnitureType::LAMP, {"Lamp", FurnitureType::LAMP, 1, 1, 35, "Iron", {{"Iron Bar", 3}}, false, "Light source"}}
    };
    return defs.at(type);
}

bool BuildingManager::isValidBuildingPosition(BuildingType type, Position pos) const {
    const auto& def = getBuildingDefinition(type);
    
    // Check if position is occupied
    for (int dy = 0; dy < def.height; dy++) {
        for (int dx = 0; dx < def.width; dx++) {
            Position checkPos = {pos.x + dx, pos.y + dy};
            if (buildings.count(pos.y) && buildings.at(pos.y).count(checkPos.x)) return false;
            if (machines.count(pos.y) && machines.at(pos.y).count(checkPos.x)) return false;
            if (furniture.count(pos.y) && furniture.at(pos.y).count(checkPos.x)) return false;
        }
    }
    return true;
}

bool BuildingManager::isValidMachinePosition(MachineType type, Position pos) const {
    if (buildings.count(pos.y) && buildings.at(pos.y).count(pos.x)) return false;
    if (machines.count(pos.y) && machines.at(pos.y).count(pos.x)) return false;
    if (furniture.count(pos.y) && furniture.at(pos.y).count(pos.x)) return false;
    return true;
}

bool BuildingManager::isValidFurniturePosition(FurnitureType type, Position pos, bool rotated) const {
    const auto& def = getFurnitureDefinition(type);
    int width = rotated ? def.height : def.width;
    int height = rotated ? def.width : def.height;
    
    for (int dy = 0; dy < height; dy++) {
        for (int dx = 0; dx < width; dx++) {
            Position checkPos = {pos.x + dx, pos.y + dy};
            if (buildings.count(pos.y) && buildings.at(pos.y).count(checkPos.x)) return false;
            if (machines.count(pos.y) && machines.at(pos.y).count(checkPos.x)) return false;
            if (furniture.count(pos.y) && furniture.at(pos.y).count(checkPos.x)) return false;
        }
    }
    return true;
}

void BuildingManager::updateAll() {
    for (auto& rowPair : buildings) {
        for (auto& colPair : rowPair.second) {
            colPair.second->update();
        }
    }
    
    for (auto& rowPair : machines) {
        for (auto& colPair : rowPair.second) {
            colPair.second->update();
        }
    }
}

std::string BuildingManager::serialize() const {
    std::ostringstream oss;
    oss << "{\"buildings\":[";
    
    bool first = true;
    for (const auto& rowPair : buildings) {
        for (const auto& colPair : rowPair.second) {
            if (!first) oss << ",";
            oss << colPair.second->serialize();
            first = false;
        }
    }
    oss << "],\"machines\":[";
    
    first = true;
    for (const auto& rowPair : machines) {
        for (const auto& colPair : rowPair.second) {
            if (!first) oss << ",";
            oss << colPair.second->serialize();
            first = false;
        }
    }
    oss << "],\"furniture\":[";
    
    first = true;
    for (const auto& rowPair : furniture) {
        for (const auto& colPair : rowPair.second) {
            if (!first) oss << ",";
            oss << colPair.second->serialize();
            first = false;
        }
    }
    oss << "]}";
    
    return oss.str();
}

void BuildingManager::deserialize(const std::string& json) {
    // Simplified deserialization - would need full JSON parser for production
    // This is a placeholder for the actual implementation
}

// MachineDefs namespace implementation
namespace MachineDefs {
    const std::map<MachineType, MachineDefinition>& getDefinitions() {
        static const std::map<MachineType, MachineDefinition> definitions = {
            {MachineType::SEED_MAKER, {
                "Seed Maker", MachineType::SEED_MAKER, 10, 200, 0.5f,
                {
                    {"Carrot", 1, "Carrot Seeds", 3, 200, 5.0f, true, ""},
                    {"Tomato", 1, "Tomato Seeds", 3, 200, 5.0f, true, ""},
                    {"Potato", 1, "Potato Seeds", 3, 200, 5.0f, true, ""},
                    {"Wheat", 1, "Wheat Seeds", 3, 200, 5.0f, true, ""},
                    {"Corn", 1, "Corn Seeds", 3, 200, 5.0f, true, ""},
                    {"Pumpkin", 1, "Pumpkin Seeds", 3, 200, 5.0f, true, ""},
                    {"Strawberry", 1, "Strawberry Seeds", 3, 200, 5.0f, true, ""},
                    {"Blueberry", 1, "Blueberry Seeds", 3, 200, 5.0f, true, ""},
                    {"GoldenWheat", 1, "Golden Wheat Seeds", 3, 300, 10.0f, true, "Farming Level 5"},
                    {"CrystalBerry", 1, "Crystal Berry Seeds", 3, 300, 10.0f, true, "Mining Level 5"},
                    {"AncientFruit", 1, "Ancient Fruit Seeds", 3, 300, 10.0f, true, "Foraging Level 5"}
                },
                UpgradeTier::LEGENDARY,
                "Converts harvested crops into multiple seeds for replanting"
            }},
            {MachineType::PRESERVES_JAR, {
                "Preserves Jar", MachineType::PRESERVES_JAR, 8, 400, 0.3f,
                {
                    {"Carrot", 1, "Pickled Carrot", 1, 400, 3.0f, true, ""},
                    {"Tomato", 1, "Tomato Jam", 1, 400, 3.0f, true, ""},
                    {"Strawberry", 1, "Strawberry Jam", 1, 400, 4.0f, true, ""},
                    {"Blueberry", 1, "Blueberry Jam", 1, 400, 4.0f, true, ""},
                    {"Pumpkin", 1, "Pumpkin Pickles", 1, 400, 3.5f, true, ""},
                    {"Pepper", 1, "Pickled Pepper", 1, 400, 3.5f, true, ""}
                },
                UpgradeTier::MASTERWORK,
                "Preserves fruits and vegetables as jams and pickles"
            }},
            {MachineType::CHEESE_PRESS, {
                "Cheese Press", MachineType::CHEESE_PRESS, 5, 200, 0.4f,
                {
                    {"Milk", 1, "Cheese", 1, 200, 5.0f, true, ""},
                    {"Large Milk", 1, "Large Cheese", 1, 200, 7.0f, true, ""}
                },
                UpgradeTier::SUPERIOR,
                "Converts milk into various types of cheese"
            }},
            {MachineType::LOOM, {
                "Loom", MachineType::LOOM, 5, 240, 0.3f,
                {
                    {"Wool", 1, "Cloth", 1, 240, 5.0f, true, ""},
                    {"Fine Wool", 1, "Fine Cloth", 1, 240, 7.0f, true, ""}
                },
                UpgradeTier::MASTERWORK,
                "Weaves wool and other fibers into cloth"
            }},
            {MachineType::MAYONNAISE_MACHINE, {
                "Mayonnaise Machine", MachineType::MAYONNAISE_MACHINE, 10, 180, 0.3f,
                {
                    {"Egg", 1, "Mayonnaise", 1, 180, 4.0f, true, ""},
                    {"Large Egg", 1, "Large Mayonnaise", 1, 180, 6.0f, true, ""}
                },
                UpgradeTier::SUPERIOR,
                "Processes eggs into mayonnaise"
            }},
            {MachineType::KEG, {
                "Keg", MachineType::KEG, 10, 1000, 0.5f,
                {
                    {"Wheat", 1, "Beer", 1, 1000, 5.0f, true, ""},
                    {"Strawberry", 1, "Wine", 1, 1000, 8.0f, true, ""},
                    {"Blueberry", 1, "Blueberry Wine", 1, 1000, 9.0f, true, ""},
                    {"AncientFruit", 1, "Ancient Wine", 1, 1000, 15.0f, true, ""}
                },
                UpgradeTier::LEGENDARY,
                "Ferments fruits and grains into alcoholic beverages"
            }},
            {MachineType::OIL_MAKER, {
                "Oil Maker", MachineType::OIL_MAKER, 5, 300, 0.4f,
                {
                    {"Sunflower", 1, "Sunflower Oil", 1, 300, 5.0f, true, ""},
                    {"Corn", 1, "Corn Oil", 1, 300, 4.0f, true, ""}
                },
                UpgradeTier::SUPERIOR,
                "Extracts oil from oilseed crops"
            }},
            {MachineType::FURNACE, {
                "Furnace", MachineType::FURNACE, 5, 120, 1.0f,
                {
                    {"Copper Ore", 5, "Copper Bar", 1, 120, 3.0f, true, ""},
                    {"Iron Ore", 5, "Iron Bar", 1, 180, 4.0f, true, ""},
                    {"Gold Ore", 5, "Gold Bar", 1, 300, 6.0f, true, ""},
                    {"Silver Ore", 5, "Silver Bar", 1, 240, 5.0f, true, ""}
                },
                UpgradeTier::MASTERWORK,
                "Smelts ores into metal bars"
            }},
            {MachineType::CRAFTING_STATION, {
                "Crafting Station", MachineType::CRAFTING_STATION, 3, 100, 0.2f,
                {},
                UpgradeTier::IMPROVED,
                "Basic crafting station for simple recipes"
            }},
            {MachineType::COMPOST_BIN, {
                "Compost Bin", MachineType::COMPOST_BIN, 10, 500, 0.1f,
                {
                    {"Crop Waste", 5, "Fertilizer", 1, 500, 2.0f, true, ""},
                    {"Withered Crop", 1, "Quality Fertilizer", 1, 500, 3.0f, true, ""}
                },
                UpgradeTier::SUPERIOR,
                "Converts organic waste into fertilizer"
            }}
        };
        return definitions;
    }
}

// Initialize static members
const std::map<BuildingType, BuildingDefinition> BuildingManager::buildingDefinitions = {};
const std::map<MachineType, MachineDefinition> BuildingManager::machineDefinitions = {};
const std::map<FurnitureType, FurnitureDefinition> BuildingManager::furnitureDefinitions = {};
