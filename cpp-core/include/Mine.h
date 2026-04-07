#ifndef MINE_H
#define MINE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <random>

enum class OreType {
    COPPER,
    IRON,
    GOLD,
    SILVER,
    DIAMOND,
    EMERALD,
    RUBY,
    SAPPHIRE,
    AMETHYST,
    COAL,
    MYTHRIL,
    ANCIENT_FRAGMENT
};

enum class GemType {
    NONE,
    QUARTZ,
    TOPAZ,
    OPAL,
    PERIDOT,
    GARNET,
    MOONSTONE,
    SUNSTONE
};

enum class CaveBiome {
    CAVERNS,
    LAVA_CAVES,
    ICE_CAVES,
    CRYSTAL_CAVES,
    ANCIENT_RUINS,
    MUSHROOM_CAVES
};

struct OreDeposit {
    OreType type;
    int quantity;
    int maxQuantity;
    bool isDepleted;
    int x;
    int y;
    int z; // depth level
    
    OreDeposit();
    OreDeposit(OreType t, int qty, int depth);
    std::string getTypeName() const;
    int getValue() const;
};

struct GemNode {
    GemType type;
    bool isMined;
    int x;
    int y;
    int z;
    
    GemNode();
    GemNode(GemType t, int depthX, int depthY, int depthZ);
    std::string getTypeName() const;
    int getValue() const;
};

struct MonsterDrop {
    std::string itemName;
    int minQuantity;
    int maxQuantity;
    float dropChance;
};

struct CaveMonster {
    std::string name;
    int health;
    int maxHealth;
    int damage;
    std::vector<MonsterDrop> drops;
    bool isDefeated;
    int x;
    int y;
    
    CaveMonster();
    CaveMonster(const std::string& n, int hp, int dmg, int posX, int posY);
    std::vector<std::string> defeat();
};

struct TreasureChest {
    std::string id;
    bool isOpened;
    std::map<std::string, int> contents;
    int x;
    int y;
    std::string rarity; // common, rare, legendary
    
    TreasureChest();
    TreasureChest(int posX, int posY, const std::string& r);
    void open();
};

struct MineLevel {
    int depth;
    CaveBiome biome;
    std::vector<OreDeposit> oreDeposits;
    std::vector<GemNode> gemNodes;
    std::vector<CaveMonster> monsters;
    std::vector<TreasureChest> chests;
    int width;
    int height;
    bool hasStairsDown;
    bool hasStairsUp;
    int stairsDownX, stairsDownY;
    int stairsUpX, stairsUpY;
    bool isCleared;
    
    MineLevel();
    MineLevel(int d, CaveBiome b, int w, int h);
    void generate(int seed);
};

enum class ToolType {
    PICKAXE_WOOD,
    PICKAXE_STONE,
    PICKAXE_IRON,
    PICKAXE_GOLD,
    PICKAXE_DIAMOND,
    BOMB,
    DETECTOR,
    LADDER
};

struct MiningTool {
    ToolType type;
    int durability;
    int maxDurability;
    float efficiency;
    int mineRange;
    
    MiningTool();
    MiningTool(ToolType t);
    std::string getTypeName() const;
    bool use();
    void repair();
};

class Mine {
public:
    static constexpr int MAX_DEPTH = 100;
    static constexpr int LEVEL_WIDTH = 20;
    static constexpr int LEVEL_HEIGHT = 15;
    
    Mine();
    ~Mine();
    
    bool enterMine();
    bool descendToNextLevel();
    bool ascendToPreviousLevel();
    bool goToLevel(int depth);
    
    MineLevel* getCurrentLevel();
    MineLevel* getLevel(int depth);
    
    bool mineOre(int x, int y, MiningTool& tool);
    bool mineGem(int x, int y, MiningTool& tool);
    bool useBomb(int x, int y);
    bool useDetector();
    
    bool openChest(int x, int y);
    bool fightMonster(int x, int y, int playerAttack);
    
    std::map<std::string, int> getCollectedOres() const;
    std::map<std::string, int> getCollectedGems() const;
    int getTotalDepth() const;
    int getCurrentDepth() const;
    
    void addToInventory(const std::string& itemName, int quantity = 1);
    std::map<std::string, int> getInventory() const;
    
    bool isLevelCleared(int depth) const;
    std::vector<std::string> getLevelMonsters(int depth) const;
    
    std::string serializeState() const;
    void deserializeState(const std::string& jsonState);
    
    static const std::map<OreType, int>& getOreValues();
    static const std::map<GemType, int>& getGemValues();
    static std::string oreTypeToString(OreType type);
    static std::string gemTypeToString(GemType type);
    static OreType stringToOreType(const std::string& str);
    static GemType stringToGemType(const std::string& str);
    
private:
    std::map<int, MineLevel> levels;
    int currentDepth;
    std::map<std::string, int> collectedOres;
    std::map<std::string, int> collectedGems;
    std::map<std::string, int> inventory;
    int seed;
    std::mt19937 rng;
    
    void generateLevel(int depth);
    CaveBiome selectBiomeForDepth(int depth);
    std::vector<OreDeposit> generateOreDeposits(int depth, CaveBiome biome);
    std::vector<GemNode> generateGemNodes(int depth, CaveBiome biome);
    std::vector<CaveMonster> generateMonsters(int depth, CaveBiome biome);
    std::vector<TreasureChest> generateTreasureChests(int depth, CaveBiome biome);
    std::string escapeJson(const std::string& input) const;
};

#endif
