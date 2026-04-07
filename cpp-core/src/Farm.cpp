#include "Farm.h"
#include <sstream>
#include <algorithm>

const std::map<std::string, CropDefinition>& Farm::getCropDefinitions() {
    static const std::map<std::string, CropDefinition> definitions = {
        {"Carrot", {"Carrot", 100, "Carrot"}},
        {"Tomato", {"Tomato", 150, "Tomato"}},
        {"Potato", {"Potato", 120, "Potato"}},
        {"Wheat", {"Wheat", 80, "Wheat"}},
        {"Corn", {"Corn", 130, "Corn"}},
        {"Strawberry", {"Strawberry", 180, "Strawberry"}},
        {"Pumpkin", {"Pumpkin", 250, "Pumpkin"}},
        {"Sunflower", {"Sunflower", 110, "Sunflower"}},
        {"Pepper", {"Pepper", 140, "Pepper"}},
        {"Blueberry", {"Blueberry", 200, "Blueberry"}}
    };
    return definitions;
}

bool Farm::isValidCrop(const std::string& cropName) {
    return getCropDefinitions().find(cropName) != getCropDefinitions().end();
}

Farm::Farm() : totalTicks(0) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = nullptr;
        }
    }
}

Farm::~Farm() {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] != nullptr) {
                grid[y][x].reset();
            }
        }
    }
}

bool Farm::plantSeed(int x, int y, const std::string& cropName) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return false;
    if (grid[y][x] != nullptr) return false;
    if (!isValidCrop(cropName)) return false;

    const auto& definitions = getCropDefinitions();
    auto it = definitions.find(cropName);
    if (it == definitions.end()) return false;

    grid[y][x] = std::make_unique<Crop>(cropName, it->second.growthTime);
    emitEvent("cropPlanted", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(cropName) + "\"}");
    return true;
}

bool Farm::harvestCrop(int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return false;
    if (grid[y][x] == nullptr) return false;
    if (!grid[y][x]->isReady()) return false;

    std::string cropName = grid[y][x]->name;
    addToInventory(cropName);
    clearPlot(x, y);
    emitEvent("cropHarvested", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(cropName) + "\"}");
    return true;
}

bool Farm::clearWitheredCrop(int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return false;
    if (grid[y][x] == nullptr) return false;
    if (!grid[y][x]->isWithered) return false;

    std::string cropName = grid[y][x]->name;
    clearPlot(x, y);
    emitEvent("cropCleared", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(cropName) + "\"}");
    return true;
}

bool Farm::waterCrop(int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return false;
    if (grid[y][x] == nullptr) return false;
    if (grid[y][x]->isWithered) return false;

    grid[y][x]->isWatered = true;
    grid[y][x]->waterTicksRemaining = 5;
    emitEvent("cropWatered", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(grid[y][x]->name) + "\"}");
    return true;
}

void Farm::updateWorld() {
    totalTicks++;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] != nullptr) {
                bool wasReady = grid[y][x]->isReady();
                bool wasWithered = grid[y][x]->isWithered;
                grid[y][x]->grow();
                if (!wasReady && grid[y][x]->isReady()) {
                    emitEvent("cropReady", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(grid[y][x]->name) + "\"}");
                }
                if (!wasWithered && grid[y][x]->isWithered) {
                    emitEvent("cropWithered", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(grid[y][x]->name) + "\"}");
                }
            }
        }
    }
}

const Crop* Farm::getCropAt(int x, int y) const {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return nullptr;
    return grid[y][x].get();
}

std::map<std::string, int> Farm::getInventory() const {
    return inventory;
}

void Farm::addToInventory(const std::string& itemName, int quantity) {
    if (quantity <= 0) return;
    inventory[itemName] += quantity;
    emitEvent("inventoryUpdated", serializeState());
}

void Farm::removeFromInventory(const std::string& itemName, int quantity) {
    if (quantity <= 0) return;
    auto it = inventory.find(itemName);
    if (it == inventory.end()) return;
    
    it->second -= quantity;
    if (it->second <= 0) {
        inventory.erase(it);
    }
    emitEvent("inventoryUpdated", serializeState());
}

void Farm::registerCallback(const std::string& eventName, EventCallback callback) {
    callbacks[eventName].push_back(callback);
}

void Farm::emitEvent(const std::string& eventName, const std::string& data) {
    if (callbacks.find(eventName) != callbacks.end()) {
        for (auto& cb : callbacks[eventName]) {
            cb(eventName, data);
        }
    }
}

void Farm::clearPlot(int x, int y) {
    if (grid[y][x] != nullptr) {
        grid[y][x].reset();
    }
}

std::string Farm::escapeJson(const std::string& input) const {
    std::string output;
    output.reserve(input.length() + 2);
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

std::string Farm::serializeState() const {
    std::ostringstream oss;
    oss << "{\"inventory\":{";
    bool first = true;
    for (const auto& pair : inventory) {
        if (!first) oss << ",";
        oss << "\"" << escapeJson(pair.first) << "\":" << pair.second;
        first = false;
    }
    oss << "},\"grid\":[";
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (x > 0 || y > 0) oss << ",";
            if (grid[y][x] != nullptr) {
                oss << "{\"x\":" << x << ",\"y\":" << y
                    << ",\"crop\":\"" << escapeJson(grid[y][x]->name) << "\""
                    << ",\"growth\":" << grid[y][x]->getGrowthPercent()
                    << ",\"stage\":\"" << grid[y][x]->getStageName() << "\""
                    << ",\"withered\":" << (grid[y][x]->isWithered ? "true" : "false")
                    << ",\"watered\":" << (grid[y][x]->isWatered ? "true" : "false")
                    << "}";
            } else {
                oss << "null";
            }
        }
    }
    oss << "],\"ticks\":" << totalTicks << "}";
    return oss.str();
}
    oss << "},\"grid\":[";
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (x > 0 || y > 0) oss << ",";
            if (grid[y][x] != nullptr) {
                oss << "{\"x\":" << x << ",\"y\":" << y
                    << ",\"crop\":\"" << escapeJson(grid[y][x]->name) << "\""
                    << ",\"growth\":" << grid[y][x]->getGrowthPercent()
                    << ",\"stage\":\"" << grid[y][x]->getStageName() << "\""
                    << ",\"withered\":" << (grid[y][x]->isWithered ? "true" : "false")
                    << ",\"watered\":" << (grid[y][x]->isWatered ? "true" : "false")
                    << "}";
            } else {
                oss << "null";
            }
        }
    }
    oss << "],\"ticks\":" << totalTicks << "}";
    return oss.str();
}

void Farm::deserializeState(const std::string& jsonState) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            clearPlot(x, y);
        }
    }
    inventory.clear();
    totalTicks = 0;

    auto findKey = [&](const std::string& key) -> size_t {
        return jsonState.find("\"" + key + "\":");
    };

    auto extractNumber = [&](size_t pos) -> int {
        if (pos == std::string::npos) return 0;
        size_t colonPos = jsonState.find(':', pos);
        if (colonPos == std::string::npos) return 0;
        size_t start = colonPos + 1;
        while (start < jsonState.size() && (jsonState[start] == ' ' || jsonState[start] == '\t')) start++;
        std::string numStr;
        while (start < jsonState.size() && (isdigit(jsonState[start]) || jsonState[start] == '-')) {
            numStr += jsonState[start++];
        }
        return numStr.empty() ? 0 : std::stoi(numStr);
    };

    auto extractString = [&](size_t pos) -> std::string {
        if (pos == std::string::npos) return "";
        size_t quoteStart = jsonState.find('"', pos + 1);
        if (quoteStart == std::string::npos) return "";
        size_t quoteEnd = jsonState.find('"', quoteStart + 1);
        if (quoteEnd == std::string::npos) return "";
        return jsonState.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
    };

    totalTicks = extractNumber(findKey("ticks"));

    size_t gridPos = findKey("grid");
    if (gridPos == std::string::npos) return;

    size_t arrayStart = jsonState.find('[', gridPos);
    if (arrayStart == std::string::npos) return;

    int bracketCount = 0;
    size_t i = arrayStart;
    while (i < jsonState.size()) {
        if (jsonState[i] == '[') bracketCount++;
        else if (jsonState[i] == ']') {
            bracketCount--;
            if (bracketCount == 0) break;
        }
        else if (jsonState[i] == '{' && bracketCount == 1) {
            size_t objEnd = jsonState.find('}', i);
            if (objEnd == std::string::npos) break;
            std::string obj = jsonState.substr(i, objEnd - i + 1);

            int x = extractNumber(obj.find("\"x\""));
            int y = extractNumber(obj.find("\"y\""));
            std::string cropName = extractString(obj.find("\"crop\""));

            if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT && !cropName.empty() && isValidCrop(cropName)) {
                const auto& definitions = getCropDefinitions();
                auto it = definitions.find(cropName);
                if (it != definitions.end()) {
                    grid[y][x] = std::make_unique<Crop>(cropName, it->second.growthTime);
                }
            }
            i = objEnd + 1;
            continue;
        }
        i++;
    }

    size_t invPos = findKey("inventory");
    if (invPos == std::string::npos) return;

    size_t invStart = jsonState.find('{', invPos);
    if (invStart == std::string::npos) return;

    size_t braceCount = 0;
    i = invStart;
    while (i < jsonState.size()) {
        if (jsonState[i] == '{') braceCount++;
        else if (jsonState[i] == '}') {
            braceCount--;
            if (braceCount == 0) break;
        }
        else if (jsonState[i] == '"' && braceCount == 1) {
            std::string itemName = extractString(i);
            size_t colonPos = jsonState.find(':', i);
            if (colonPos != std::string::npos) {
                int qty = extractNumber(colonPos);
                if (qty > 0 && !itemName.empty()) {
                    inventory[itemName] = qty;
                }
            }
            size_t nextComma = jsonState.find(',', i);
            size_t nextBrace = jsonState.find('}', i);
            if (nextComma != std::string::npos && (nextBrace == std::string::npos || nextComma < nextBrace)) {
                i = nextComma + 1;
            } else {
                break;
            }
            continue;
        }
        i++;
    }
}
    }
    inventory.clear();
    totalTicks = 0;

    auto findKey = [&](const std::string& key) -> size_t {
        return jsonState.find("\"" + key + "\":");
    };

    auto extractNumber = [&](size_t pos) -> int {
        if (pos == std::string::npos) return 0;
        size_t colonPos = jsonState.find(':', pos);
        if (colonPos == std::string::npos) return 0;
        size_t start = colonPos + 1;
        while (start < jsonState.size() && (jsonState[start] == ' ' || jsonState[start] == '\t')) start++;
        std::string numStr;
        while (start < jsonState.size() && (isdigit(jsonState[start]) || jsonState[start] == '-')) {
            numStr += jsonState[start++];
        }
        return numStr.empty() ? 0 : std::stoi(numStr);
    };

    auto extractString = [&](size_t pos) -> std::string {
        if (pos == std::string::npos) return "";
        size_t quoteStart = jsonState.find('"', pos + 1);
        if (quoteStart == std::string::npos) return "";
        size_t quoteEnd = jsonState.find('"', quoteStart + 1);
        if (quoteEnd == std::string::npos) return "";
        return jsonState.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
    };

    totalTicks = extractNumber(findKey("ticks"));

    size_t gridPos = findKey("grid");
    if (gridPos == std::string::npos) return;

    size_t arrayStart = jsonState.find('[', gridPos);
    if (arrayStart == std::string::npos) return;

    int bracketCount = 0;
    size_t i = arrayStart;
    while (i < jsonState.size()) {
        if (jsonState[i] == '[') bracketCount++;
        else if (jsonState[i] == ']') {
            bracketCount--;
            if (bracketCount == 0) break;
        }
        else if (jsonState[i] == '{' && bracketCount == 1) {
            size_t objEnd = jsonState.find('}', i);
            if (objEnd == std::string::npos) break;
            std::string obj = jsonState.substr(i, objEnd - i + 1);

            int x = extractNumber(obj.find("\"x\""));
            int y = extractNumber(obj.find("\"y\""));
            std::string cropName = extractString(obj.find("\"crop\""));

            if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT && !cropName.empty() && isValidCrop(cropName)) {
                const auto& definitions = getCropDefinitions();
                auto it = definitions.find(cropName);
                if (it != definitions.end()) {
                    grid[y][x] = std::make_unique<Crop>(cropName, it->second.growthTime);
                }
            }
            i = objEnd + 1;
            continue;
        }
        i++;
    }

    size_t invPos = findKey("inventory");
    if (invPos == std::string::npos) return;

    size_t invStart = jsonState.find('{', invPos);
    if (invStart == std::string::npos) return;

    size_t braceCount = 0;
    i = invStart;
    while (i < jsonState.size()) {
        if (jsonState[i] == '{') braceCount++;
        else if (jsonState[i] == '}') {
            braceCount--;
            if (braceCount == 0) break;
        }
        else if (jsonState[i] == '"' && braceCount == 1) {
            std::string itemName = extractString(i);
            size_t colonPos = jsonState.find(':', i);
            if (colonPos != std::string::npos) {
                int qty = extractNumber(colonPos);
                if (qty > 0 && !itemName.empty()) {
                    inventory[itemName] = qty;
                }
            }
            size_t nextComma = jsonState.find(',', i);
            size_t nextBrace = jsonState.find('}', i);
            if (nextComma != std::string::npos && (nextBrace == std::string::npos || nextComma < nextBrace)) {
                i = nextComma + 1;
            } else {
                break;
            }
            continue;
        }
        i++;
    }
}
