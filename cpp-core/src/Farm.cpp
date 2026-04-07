#include "Farm.h"
#include <sstream>
#include <algorithm>

const std::map<std::string, CropDefinition>& Farm::getCropDefinitions() {
    static const std::map<std::string, CropDefinition> definitions = {
        {"Carrot", {"Carrot", 100, "Carrot", CropSeason::SPRING, 10, 25, 1.5f, false, {"Onion"}, {"Dill"}}},
        {"Tomato", {"Tomato", 150, "Tomato", CropSeason::SUMMER, 15, 30, 1.3f, false, {"Basil"}, {"Corn"}}},
        {"Potato", {"Potato", 120, "Potato", CropSeason::FALL, 8, 20, 1.2f, false, {"Beans"}, {"Pumpkin"}}},
        {"Wheat", {"Wheat", 80, "Wheat", CropSeason::ALL, 5, 15, 1.0f, false, {}, {}}},
        {"Corn", {"Corn", 130, "Corn", CropSeason::SUMMER, 12, 25, 1.1f, false, {"Beans", "Squash"}, {"Tomato"}}},
        {"Strawberry", {"Strawberry", 180, "Strawberry", CropSeason::SPRING, 20, 35, 1.4f, true, {}, {}}},
        {"Pumpkin", {"Pumpkin", 250, "Pumpkin", CropSeason::FALL, 25, 40, 1.2f, true, {"Corn"}, {"Potato"}}},
        {"Sunflower", {"Sunflower", 110, "Sunflower", CropSeason::SUMMER, 10, 20, 1.3f, false, {}, {}}},
        {"Pepper", {"Pepper", 140, "Pepper", CropSeason::SUMMER, 15, 28, 1.2f, false, {"Tomato"}, {}}},
        {"Blueberry", {"Blueberry", 200, "Blueberry", CropSeason::SPRING, 25, 45, 1.5f, true, {}, {}}},
        // Tier 2+ crops
        {"GoldenWheat", {"GoldenWheat", 300, "Golden Wheat", CropSeason::ALL, 50, 100, 2.0f, true, {}, {}}},
        {"CrystalBerry", {"CrystalBerry", 400, "Crystal Berry", CropSeason::WINTER, 75, 150, 2.5f, true, {}, {}}},
        {"AncientFruit", {"AncientFruit", 500, "Ancient Fruit", CropSeason::ALL, 100, 200, 3.0f, true, {}, {}}}
    };
    return definitions;
}

const std::map<std::string, AnimalDefinition>& Farm::getAnimalDefinitions() {
    static const std::map<std::string, AnimalDefinition> definitions = {
        {"Chicken", {"Chicken", "Chicken", "Egg", 24, 50, "Grain"}},
        {"Cow", {"Cow", "Cow", "Milk", 48, 150, "Grass"}},
        {"Sheep", {"Sheep", "Sheep", "Wool", 72, 200, "Grass"}},
        {"Pig", {"Pig", "Pig", "Truffle", 96, 300, "Vegetables"}},
        {"Bee", {"Bee", "Bee", "Honey", 168, 100, "Flowers"}}
    };
    return definitions;
}

bool Farm::isValidCrop(const std::string& cropName) {
    return getCropDefinitions().find(cropName) != getCropDefinitions().end();
}

bool Farm::isValidAnimal(const std::string& animalName) {
    return getAnimalDefinitions().find(animalName) != getAnimalDefinitions().end();
}

FarmPlot::FarmPlot() : isUnlocked(false), plotIndex(0) {}

FarmPlot::FarmPlot(SoilType soilType) : soil(soilType), isUnlocked(true), plotIndex(0) {}

Farm::Farm() : totalTicks(0), currentSeason(0), farmLevel(1), money(100) {
    initializeStartingPlots();
}

Farm::~Farm() {}

void Farm::initializeStartingPlots() {
    // Create initial 10x10 grid with normal soil
    for (int y = 0; y < BASE_GRID_HEIGHT; y++) {
        for (int x = 0; x < BASE_GRID_WIDTH; x++) {
            plots[y][x] = FarmPlot(SoilType::NORMAL);
            plots[y][x].isUnlocked = true;
            plots[y][x].plotIndex = y * BASE_GRID_WIDTH + x;
        }
    }
    
    // Initialize inventory with some starting items
    inventory["Parship"] = 10;
    inventory["Water Can"] = 1;
}

bool Farm::plantSeed(int x, int y, const std::string& cropName) {
    if (!isInBounds(x, y)) return false;
    if (!isPlotUnlocked(x, y)) return false;
    
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return false;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return false;
    
    if (plot->second.crop != nullptr) return false;
    if (!isValidCrop(cropName)) return false;

    const auto& definitions = getCropDefinitions();
    auto it = definitions.find(cropName);
    if (it == definitions.end()) return false;

    plot->second.crop = std::make_unique<Crop>(cropName, it->second.name, it->second.growthTime);
    emitEvent("cropPlanted", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(cropName) + "\"}");
    return true;
}

bool Farm::harvestCrop(int x, int y) {
    if (!isInBounds(x, y)) return false;
    
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return false;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return false;
    
    if (plot->second.crop == nullptr) return false;
    if (!plot->second.crop->isReady()) return false;

    std::string cropName = plot->second.crop->name;
    int yield = plot->second.crop->calculateYield();
    addToInventory(cropName, yield);
    clearPlot(x, y);
    emitEvent("cropHarvested", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(cropName) + "\",\"yield\":" + std::to_string(yield) + "}");
    return true;
}

bool Farm::clearWitheredCrop(int x, int y) {
    if (!isInBounds(x, y)) return false;
    
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return false;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return false;
    
    if (plot->second.crop == nullptr) return false;
    if (!plot->second.crop->isWithered) return false;

    clearPlot(x, y);
    emitEvent("cropCleared", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + "}");
    return true;
}

bool Farm::waterCrop(int x, int y) {
    if (!isInBounds(x, y)) return false;
    
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return false;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return false;
    
    if (plot->second.crop == nullptr) return false;
    if (plot->second.crop->isWithered) return false;

    plot->second.crop->isWatered = true;
    plot->second.crop->waterTicksRemaining = 5;
    emitEvent("cropWatered", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(plot->second.crop->name) + "\"}");
    return true;
}

bool Farm::fertilizePlot(int x, int y, int amount) {
    if (!isInBounds(x, y)) return false;
    
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return false;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return false;
    
    plot->second.soil.applyFertilizer(amount);
    emitEvent("plotFertilized", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"amount\":" + std::to_string(amount) + "}");
    return true;
}

void Farm::updateWorld() {
    totalTicks++;
    
    for (auto& rowPair : plots) {
        for (auto& plotPair : rowPair.second) {
            int x = plotPair.first;
            int y = rowPair.first;
            auto& plot = plotPair.second;
            if (plot.crop != nullptr) {
                bool wasReady = plot.crop->isReady();
                bool wasWithered = plot.crop->isWithered;
                
                float soilMultiplier = plot.soil.getEffectiveGrowthMultiplier();
                plot.crop->grow(soilMultiplier);
                
                if (!wasReady && plot.crop->isReady()) {
                    emitEvent("cropReady", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(plot.crop->name) + "\"}");
                }
                if (!wasWithered && plot.crop->isWithered) {
                    emitEvent("cropWithered", "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + ",\"crop\":\"" + escapeJson(plot.crop->name) + "\"}");
                }
            }
            
            // Tick soil
            plot.soil.tick();
        }
    }
    
    // Check companion planting bonuses
    checkCompanionPlanting();
}

const Crop* Farm::getCropAt(int x, int y) const {
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return nullptr;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return nullptr;
    
    return plot->second.crop.get();
}

const FarmPlot* Farm::getPlotAt(int x, int y) const {
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return nullptr;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return nullptr;
    
    return &plot->second;
}

std::map<std::string, int> Farm::getInventory() const {
    return inventory;
}

void Farm::addToInventory(const std::string& itemName, int quantity) {
    inventory[itemName] += quantity;
}

void Farm::removeFromInventory(const std::string& itemName, int quantity) {
    auto it = inventory.find(itemName);
    if (it != inventory.end()) {
        it->second -= quantity;
        if (it->second <= 0) {
            inventory.erase(it);
        }
    }
}

bool Farm::unlockAdjacentPlot(int fromX, int fromY, int direction) {
    if (!isInBounds(fromX, fromY)) return false;
    if (!isPlotUnlocked(fromX, fromY)) return false;
    
    int newX = fromX, newY = fromY;
    switch (direction) {
        case 0: newY--; break; // Up
        case 1: newX++; break; // Right
        case 2: newY++; break; // Down
        case 3: newX--; break; // Left
    }
    
    if (!isInBounds(newX, newY)) return false;
    if (isPlotUnlocked(newX, newY)) return false;
    
    auto plotIt = plots.find(newY);
    if (plotIt == plots.end()) {
        plots[newY] = std::map<int, FarmPlot>();
        plotIt = plots.find(newY);
    }
    
    plotIt->second[newX] = FarmPlot(SoilType::NORMAL);
    plotIt->second[newX].isUnlocked = true;
    plotIt->second[newX].plotIndex = newY * MAX_GRID_WIDTH + newX;
    
    emitEvent("plotUnlocked", "{\"x\":" + std::to_string(newX) + ",\"y\":" + std::to_string(newY) + "}");
    return true;
}

bool Farm::isPlotUnlocked(int x, int y) const {
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return false;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return false;
    
    return plot->second.isUnlocked;
}

SoilType Farm::getSoilTypeAt(int x, int y) const {
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return SoilType::NORMAL;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return SoilType::NORMAL;
    
    return plot->second.soil.type;
}

void Farm::checkCompanionPlanting() {
    for (auto& rowPair : plots) {
        for (auto& plotPair : rowPair.second) {
            int x = plotPair.first;
            int y = rowPair.first;
            auto& plot = plotPair.second;
            
            if (plot.crop == nullptr || !isValidCrop(plot.crop->name)) continue;
            
            const auto& def = getCropDefinitions().at(plot.crop->name);
            bool hasCompanion = false;
            bool hasAvoid = false;
            
            // Check adjacent plots
            int dx[] = {-1, 1, 0, 0};
            int dy[] = {0, 0, -1, 1};
            
            for (int i = 0; i < 4; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                
                auto neighborIt = plots.find(ny);
                if (neighborIt == plots.end()) continue;
                
                auto neighbor = neighborIt->second.find(nx);
                if (neighbor == neighborIt->second.end() || neighbor->second.crop == nullptr) continue;
                
                const std::string& neighborCrop = neighbor->second.crop->name;
                
                if (std::find(def.companionCrops.begin(), def.companionCrops.end(), neighborCrop) != def.companionCrops.end()) {
                    hasCompanion = true;
                }
                if (std::find(def.avoidCrops.begin(), def.avoidCrops.end(), neighborCrop) != def.avoidCrops.end()) {
                    hasAvoid = true;
                }
            }
            
            if (hasCompanion && !hasAvoid) {
                plot.crop->applyCompanionBoost();
            }
        }
    }
}

bool Farm::applyCropRotation(int x, int y, const std::string& newCrop) {
    if (!isInBounds(x, y)) return false;
    if (!isPlotUnlocked(x, y)) return false;
    
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return false;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return false;
    
    // Check if previous crop was a legume (enriches soil)
    bool wasLegume = (plot->second.crop != nullptr && 
                     (plot->second.crop->name == "Beans" || plot->second.crop->name == "Peas"));
    
    if (wasLegume) {
        plot->second.soil.nutrientLevel = std::min(2.0f, plot->second.soil.nutrientLevel + 0.3f);
    }
    
    return plantSeed(x, y, newCrop);
}

void Farm::clearPlot(int x, int y) {
    auto plotIt = plots.find(y);
    if (plotIt == plots.end()) return;
    
    auto plot = plotIt->second.find(x);
    if (plot == plotIt->second.end()) return;
    
    plot->second.crop.reset();
}

bool Farm::isInBounds(int x, int y) const {
    return x >= 0 && x < MAX_GRID_WIDTH && y >= 0 && y < MAX_GRID_HEIGHT;
}

std::string Farm::escapeJson(const std::string& input) const {
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

void Farm::registerCallback(const std::string& eventName, EventCallback callback) {
    callbacks[eventName].push_back(callback);
}

void Farm::emitEvent(const std::string& eventName, const std::string& data) {
    auto it = callbacks.find(eventName);
    if (it != callbacks.end()) {
        for (auto& callback : it->second) {
            callback(eventName, data);
        }
    }
}

std::string Farm::serializeState() const {
    std::ostringstream oss;
    oss << "{\"ticks\":" << totalTicks << ",";
    oss << "\"season\":" << currentSeason << ",";
    oss << "\"farmLevel\":" << farmLevel << ",";
    oss << "\"money\":" << money << ",";
    oss << "\"plots\":{";
    
    bool firstPlot = true;
    for (const auto& rowPair : plots) {
        for (const auto& plotPair : rowPair.second) {
            if (!plotPair.second.isUnlocked) continue;
            if (!firstPlot) oss << ",";
            
            oss << "{\"x\":" << plotPair.first << ",\"y\":" << rowPair.first;
            if (plotPair.second.crop) {
                oss << ",\"crop\":\"" << escapeJson(plotPair.second.crop->name) << "\"";
                oss << ",\"age\":" << plotPair.second.crop->currentAge;
                oss << ",\"stage\":" << static_cast<int>(plotPair.second.crop->stage);
            }
            oss << ",\"soil\":\"" << plotPair.second.soil.getTypeName() << "\"";
            oss << "}";
            firstPlot = false;
        }
    }
    
    oss << "},\"inventory\":{";
    bool firstInv = true;
    for (const auto& pair : inventory) {
        if (!firstInv) oss << ",";
        oss << "\"" << escapeJson(pair.first) << "\":" << pair.second;
        firstInv = false;
    }
    oss << "}}";
    
    return oss.str();
}

void Farm::deserializeState(const std::string& jsonState) {
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
    currentSeason = extractNumber(findKey("season"));
    farmLevel = extractNumber(findKey("farmLevel"));
    money = extractNumber(findKey("money"));
}

void Farm::advanceSeason() {
    currentSeason = (currentSeason + 1) % 4;
    emitEvent("seasonChanged", "{\"season\":" + std::to_string(currentSeason) + "}");
}
