#ifndef NPC_H
#define NPC_H

#include <string>
#include <vector>
#include <map>
#include <functional>

enum class NPCRole {
    BLACKSMITH,
    BAKER,
    MAYOR,
    MERCHANT,
    FISHERMAN,
    MINER,
    FARMER,
    DOCTOR,
    LIBRARIAN
};

enum class RelationshipLevel {
    STRANGER = 0,
    ACQUAINTANCE = 1,
    FRIEND = 2,
    GOOD_FRIEND = 3,
    BEST_FRIEND = 4,
    ROMANTIC = 5,
    MARRIED = 6
};

struct NPCScheduleEntry {
    std::string location;
    int startHour;
    int endHour;
    std::string activity;
};

struct NPCQuest {
    std::string id;
    std::string title;
    std::string description;
    std::map<std::string, int> requiredItems;
    std::map<std::string, int> rewardItems;
    int rewardMoney;
    int rewardFriendship;
    bool isCompleted;
    bool isActive;
    std::string giverNPC;
};

struct DialogueLine {
    std::string text;
    int minRelationship;
    std::string condition; // e.g., "season:SPRING", "time:10-14"
    std::vector<std::string> responses;
};

class NPC {
public:
    std::string name;
    std::string displayName;
    NPCRole role;
    RelationshipLevel relationship;
    int friendshipPoints;
    std::vector<NPCScheduleEntry> schedule;
    std::vector<NPCQuest> availableQuests;
    std::vector<NPCQuest> activeQuests;
    std::vector<NPCQuest> completedQuests;
    std::map<std::string, int> likedItems;
    std::map<std::string, int> dislikedItems;
    std::string spouse;
    bool isRomanceable;
    std::vector<std::string> giftsReceived;
    std::string currentLocation;
    int currentHour;
    
    NPC();
    NPC(const std::string& name, NPCRole role);
    
    void updateSchedule(int hour);
    bool giveGift(const std::string& itemName);
    int calculateFriendshipChange(const std::string& itemName);
    bool acceptQuest(const std::string& questId);
    bool completeQuest(const std::string& questId, const std::map<std::string, int>& deliveredItems);
    std::string getDialogue(int hour, int season, int weather);
    RelationshipLevel getRelationshipLevel() const;
    void advanceRelationship();
    bool canMarry() const;
    bool proposeMarriage();
    
    std::string serializeState() const;
    void deserializeState(const std::string& jsonState);
    
private:
    std::map<std::string, std::vector<DialogueLine>> dialogues;
    void initializeDialogues();
    std::string escapeJson(const std::string& input) const;
};

class NPCManager {
public:
    static NPCManager& getInstance();
    
    void initializeNPCs();
    NPC* getNPC(const std::string& name);
    std::vector<NPC*> getAllNPCs();
    std::vector<NPC*> getNPCsAtLocation(const std::string& location);
    std::vector<NPC*> getAvailableQuestGivers();
    std::vector<NPCQuest> getActiveQuests();
    std::vector<NPCQuest> getCompletedQuests();
    
    void updateTime(int hour);
    void updateAllSchedules();
    
    bool giveGiftToNPC(const std::string& npcName, const std::string& itemName);
    bool acceptQuestFromNPC(const std::string& npcName, const std::string& questId);
    bool turnInQuest(const std::string& questId, const std::map<std::string, int>& items);
    
    std::string serializeState() const;
    void deserializeState(const std::string& jsonState);
    
private:
    NPCManager();
    ~NPCManager();
    NPCManager(const NPCManager&) = delete;
    NPCManager& operator=(const NPCManager&) = delete;
    
    std::map<std::string, NPC> npcs;
    std::vector<NPCQuest> globalQuests;
};

#endif
