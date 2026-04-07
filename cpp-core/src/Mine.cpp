#include "Mine.h"
#include <sstream>
#include <algorithm>
#include <cmath>

// OreDeposit Implementation
OreDeposit::OreDeposit() 
    : type(OreType::COPPER), quantity(10), maxQuantity(10), isDepleted(false), x(0), y(0), z(0) {}

OreDeposit::OreDeposit(OreType t, int qty, int depth) 
    : type(t), quantity(qty), maxQuantity(qty), isDepleted(false), x(0), y(0), z(depth) {}

std::string OreDeposit::getTypeName() const {
    switch (type) {
        case OreType::COPPER: return "Copper Ore";
        case OreType::IRON: return "Iron Ore";
        case OreType::GOLD: return "Gold Ore";
        case OreType::SILVER: return "Silver Ore";
        case OreType::DIAMOND: return "Diamond";
        case OreType::EMERALD: return "Emerald";
        case OreType::RUBY: return "Ruby";
        case OreType::SAPPHIRE: return "Sapphire";
        case OreType::AMETHYST: return "Amethyst";
        case OreType::COAL: return "Coal";
        case OreType::MYTHRIL: return "Mythril";
        case OreType::ANCIENT_FRAGMENT: return "Ancient Fragment";
        default: return "Unknown Ore";
    }
}

int OreDeposit::getValue() const {
    static const std::map<OreType, int> values = {
        {OreType::COPPER, 15}, {OreType::IRON, 25}, {OreType::GOLD, 50},
        {OreType::SILVER, 35}, {OreType::DIAMOND, 150}, {OreType::EMERALD, 140},
        {OreType::RUBY, 130}, {OreType::SAPPHIRE, 125}, {OreType::AMETHYST, 80},
        {OreType::COAL, 10}, {OreType::MYTHRIL, 200}, {OreType::ANCIENT_FRAGMENT, 500}
    };
    auto it = values.find(type);
    return it != values.end() ? it->second : 0;
}

// GemNode Implementation
GemNode::GemNode() : type(GemType::NONE), isMined(false), x(0), y(0), z(0) {}

GemNode::GemNode(GemType t, int depthX, int depthY, int depthZ) 
    : type(t), isMined(false), x(depthX), y(depthY), z(depthZ) {}

std::string GemNode::getTypeName() const {
    switch (type) {
        case GemType::QUARTZ: return "Quartz";
        case GemType::TOPAZ: return "Topaz";
        case GemType::OPAL: return "Opal";
        case GemType::PERIDOT: return "Peridot";
        case GemType::GARNET: return "Garnet";
        case GemType::MOONSTONE: return "Moonstone";
        case GemType::SUNSTONE: return "Sunstone";
        default: return "Unknown Gem";
    }
}

int GemNode::getValue() const {
    static const std::map<GemType, int> values = {
        {GemType::QUARTZ, 50}, {GemType::TOPAZ, 75}, {GemType::OPAL, 100},
        {GemType::PERIDOT, 90}, {GemType::GARNET, 85}, {GemType::MOONSTONE, 120},
        {GemType::SUNSTONE, 130}
    };
    auto it = values.find(type);
    return it != values.end() ? it->second : 0;
}

// CaveMonster Implementation
CaveMonster::CaveMonster() : health(10), maxHealth(10), damage(2), isDefeated(false), x(0), y(0) {}

CaveMonster::CaveMonster(const std::string& n, int hp, int dmg, int posX, int posY)
    : name(n), health(hp), maxHealth(hp), damage(dmg), isDefeated(false), x(posX), y(posY) {}

std::vector<std::string> CaveMonster::defeat() {
    isDefeated = true;
    std::vector<std::string> dropsList;
    for (const auto& drop : drops) {
        float roll = static_cast<float>(rand()) / RAND_MAX;
        if (roll <= drop.dropChance) {
            int qty = drop.minQuantity + (rand() % (drop.maxQuantity - drop.minQuantity + 1));
            for (int i = 0; i < qty; i++) {
                dropsList.push_back(drop.itemName);
            }
        }
    }
    return dropsList;
}

// TreasureChest Implementation
TreasureChest::TreasureChest() : isOpened(false), x(0), y(0), rarity("common") {}

TreasureChest::TreasureChest(int posX, int posY, const std::string& r)
    : isOpened(false), x(posX), y(posY), rarity(r) {}

void TreasureChest::open() {
    isOpened = true;
}

// MineLevel Implementation
MineLevel::MineLevel() : depth(0), biome(CaveBiome::CAVERNS), width(20), height(15),
    hasStairsDown(false), hasStairsUp(false), stairsDownX(0), stairsDownY(0),
    stairsUpX(0), stairsUpY(0), isCleared(false) {}

MineLevel::MineLevel(int d, CaveBiome b, int w, int h) 
    : depth(d), biome(b), width(w), height(h), hasStairsDown(false), hasStairsUp(false),
      stairsDownX(0), stairsDownY(0), stairsUpX(0), stairsUpY(0), isCleared(false) {}

void MineLevel::generate(int seed) {
    // Procedural generation would go here
    // Simplified for now
}

// MiningTool Implementation
MiningTool::MiningTool() : type(ToolType::PICKAXE_WOOD), durability(100), maxDurability(100),
    efficiency(1.0f), mineRange(1) {}

MiningTool::MiningTool(ToolType t) : type(t), durability(100), maxDurability(100), efficiency(1.0f), mineRange(1) {
    switch (t) {
        case ToolType::PICKAXE_STONE:
            maxDurability = 150; durability = 150; efficiency = 1.2f; break;
        case ToolType::PICKAXE_IRON:
            maxDurability = 250; durability = 250; efficiency = 1.5f; break;
        case ToolType::PICKAXE_GOLD:
            maxDurability = 200; durability = 200; efficiency = 1.8f; break;
        case ToolType::PICKAXE_DIAMOND:
            maxDurability = 500; durability = 500; efficiency = 2.0f; break;
        default: break;
    }
}

std::string MiningTool::getTypeName() const {
    switch (type) {
        case ToolType::PICKAXE_WOOD: return "Wood Pickaxe";
        case ToolType::PICKAXE_STONE: return "Stone Pickaxe";
        case ToolType::PICKAXE_IRON: return "Iron Pickaxe";
        case ToolType::PICKAXE_GOLD: return "Gold Pickaxe";
        case ToolType::PICKAXE_DIAMOND: return "Diamond Pickaxe";
        case ToolType::BOMB: return "Bomb";
        case ToolType::DETECTOR: return "Ore Detector";
        case ToolType::LADDER: return "Ladder";
        default: return "Unknown Tool";
    }
}

bool MiningTool::use() {
    if (type == ToolType::BOMB) return true; // Consumed
    if (durability > 0) {
        durability--;
        return true;
    }
    return false;
}

void MiningTool::repair() {
    durability = maxDurability;
}

// Mine Implementation
Mine::Mine() : currentDepth(0), seed(42) {
    rng.seed(seed);
    generateLevel(0);
}

Mine::~Mine() {}

bool Mine::enterMine() {
    return currentDepth >= 0;
}

bool Mine::descendToNextLevel() {
    if (currentDepth >= MAX_DEPTH - 1) return false;
    if (!levels[currentDepth].hasStairsDown) return false;
    
    currentDepth++;
    if (levels.find(currentDepth) == levels.end()) {
        generateLevel(currentDepth);
    }
    return true;
}

bool Mine::ascendToPreviousLevel() {
    if (currentDepth <= 0) return false;
    currentDepth--;
    return true;
}

bool Mine::goToLevel(int depth) {
    if (depth < 0 || depth >= MAX_DEPTH) return false;
    if (levels.find(depth) == levels.end()) {
        generateLevel(depth);
    }
    currentDepth = depth;
    return true;
}

MineLevel* Mine::getCurrentLevel() {
    return &levels[currentDepth];
}

MineLevel* Mine::getLevel(int depth) {
    if (levels.find(depth) == levels.end()) {
        return nullptr;
    }
    return &levels[depth];
}

bool Mine::mineOre(int x, int y, MiningTool& tool) {
    MineLevel* level = getCurrentLevel();
    if (!level) return false;
    
    for (auto& deposit : level->oreDeposits) {
        if (deposit.x == x && deposit.y == y && !deposit.isDepleted) {
            if (!tool.use()) return false;
            
            deposit.quantity--;
            addToInventory(deposit.getTypeName());
            
            if (deposit.quantity <= 0) {
                deposit.isDepleted = true;
            }
            return true;
        }
    }
    return false;
}

bool Mine::mineGem(int x, int y, MiningTool& tool) {
    MineLevel* level = getCurrentLevel();
    if (!level) return false;
    
    for (auto& gem : level->gemNodes) {
        if (gem.x == x && gem.y == y && !gem.isMined) {
            if (!tool.use()) return false;
            
            gem.isMined = true;
            addToInventory(gem.getTypeName());
            return true;
        }
    }
    return false;
}

bool Mine::useBomb(int x, int y) {
    MineLevel* level = getCurrentLevel();
    if (!level) return false;
    
    // Clear area around bomb
    for (auto& deposit : level->oreDeposits) {
        if (abs(deposit.x - x) <= 2 && abs(deposit.y - y) <= 2 && !deposit.isDepleted) {
            addToInventory(deposit.getTypeName(), deposit.quantity);
            deposit.isDepleted = true;
        }
    }
    return true;
}

bool Mine::useDetector() {
    MineLevel* level = getCurrentLevel();
    if (!level) return false;
    
    // Return true if there are nearby ores
    return !level->oreDeposits.empty();
}

bool Mine::openChest(int x, int y) {
    MineLevel* level = getCurrentLevel();
    if (!level) return false;
    
    for (auto& chest : level->chests) {
        if (chest.x == x && chest.y == y && !chest.isOpened) {
            chest.open();
            for (const auto& item : chest.contents) {
                addToInventory(item.first, item.second);
            }
            return true;
        }
    }
    return false;
}

bool Mine::fightMonster(int x, int y, int playerAttack) {
    MineLevel* level = getCurrentLevel();
    if (!level) return false;
    
    for (auto& monster : level->monsters) {
        if (monster.x == x && monster.y == y && !monster.isDefeated) {
            monster.health -= playerAttack;
            if (monster.health <= 0) {
                auto drops = monster.defeat();
                for (const auto& drop : drops) {
                    addToInventory(drop);
                }
            }
            return true;
        }
    }
    return false;
}

std::map<std::string, int> Mine::getCollectedOres() const {
    return collectedOres;
}

std::map<std::string, int> Mine::getCollectedGems() const {
    return collectedGems;
}

int Mine::getTotalDepth() const {
    return MAX_DEPTH;
}

int Mine::getCurrentDepth() const {
    return currentDepth;
}

void Mine::addToInventory(const std::string& itemName, int quantity) {
    inventory[itemName] += quantity;
    // Track ores and gems separately
    collectedOres[itemName] += quantity;
}

std::map<std::string, int> Mine::getInventory() const {
    return inventory;
}

bool Mine::isLevelCleared(int depth) const {
    auto it = levels.find(depth);
    if (it == levels.end()) return false;
    return it->second.isCleared;
}

std::vector<std::string> Mine::getLevelMonsters(int depth) const {
    std::vector<std::string> result;
    auto it = levels.find(depth);
    if (it != levels.end()) {
        for (const auto& monster : it->second.monsters) {
            if (!monster.isDefeated) {
                result.push_back(monster.name);
            }
        }
    }
    return result;
}

std::string Mine::serializeState() const {
    std::ostringstream oss;
    oss << "{\"currentDepth\":" << currentDepth << ",";
    oss << "\"inventory\":{";
    bool first = true;
    for (const auto& pair : inventory) {
        if (!first) oss << ",";
        oss << "\"" << pair.first << "\":" << pair.second;
        first = false;
    }
    oss << "}}";
    return oss.str();
}

void Mine::deserializeState(const std::string& jsonState) {
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
    
    currentDepth = extractNumber(findKey("currentDepth"));
}

const std::map<OreType, int>& Mine::getOreValues() {
    static const std::map<OreType, int> values = {
        {OreType::COPPER, 15}, {OreType::IRON, 25}, {OreType::GOLD, 50},
        {OreType::SILVER, 35}, {OreType::DIAMOND, 150}, {OreType::EMERALD, 140},
        {OreType::RUBY, 130}, {OreType::SAPPHIRE, 125}, {OreType::AMETHYST, 80},
        {OreType::COAL, 10}, {OreType::MYTHRIL, 200}, {OreType::ANCIENT_FRAGMENT, 500}
    };
    return values;
}

const std::map<GemType, int>& Mine::getGemValues() {
    static const std::map<GemType, int> values = {
        {GemType::QUARTZ, 50}, {GemType::TOPAZ, 75}, {GemType::OPAL, 100},
        {GemType::PERIDOT, 90}, {GemType::GARNET, 85}, {GemType::MOONSTONE, 120},
        {GemType::SUNSTONE, 130}
    };
    return values;
}

std::string Mine::oreTypeToString(OreType type) {
    OreDeposit dep(type, 0, 0);
    return dep.getTypeName();
}

std::string Mine::gemTypeToString(GemType type) {
    GemNode node(type, 0, 0, 0);
    return node.getTypeName();
}

OreType Mine::stringToOreType(const std::string& str) {
    if (str == "Copper Ore") return OreType::COPPER;
    if (str == "Iron Ore") return OreType::IRON;
    if (str == "Gold Ore") return OreType::GOLD;
    if (str == "Diamond") return OreType::DIAMOND;
    if (str == "Coal") return OreType::COAL;
    return OreType::COPPER;
}

GemType Mine::stringToGemType(const std::string& str) {
    if (str == "Quartz") return GemType::QUARTZ;
    if (str == "Topaz") return GemType::TOPAZ;
    if (str == "Opal") return GemType::OPAL;
    return GemType::QUARTZ;
}

void Mine::generateLevel(int depth) {
    CaveBiome biome = selectBiomeForDepth(depth);
    MineLevel level(depth, biome, LEVEL_WIDTH, LEVEL_HEIGHT);
    level.generate(seed + depth);
    levels[depth] = level;
}

CaveBiome Mine::selectBiomeForDepth(int depth) {
    if (depth < 10) return CaveBiome::CAVERNS;
    if (depth < 25) return CaveBiome::LAVA_CAVES;
    if (depth < 40) return CaveBiome::ICE_CAVES;
    if (depth < 60) return CaveBiome::CRYSTAL_CAVES;
    if (depth < 80) return CaveBiome::ANCIENT_RUINS;
    return CaveBiome::MUSHROOM_CAVES;
}

std::string Mine::escapeJson(const std::string& input) const {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\n': output += "\\n"; break;
            default: output += c;
        }
    }
    return output;
}
