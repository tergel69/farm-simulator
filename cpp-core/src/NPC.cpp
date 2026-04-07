#include "NPC.h"
#include <sstream>
#include <algorithm>
#include <cstdlib>

// NPC Implementation
NPC::NPC() 
    : role(NPCRole::FARMER), relationship(RelationshipLevel::STRANGER), 
      friendshipPoints(0), isRomanceable(false), currentHour(8) {
    initializeDialogues();
}

NPC::NPC(const std::string& n, NPCRole r) 
    : name(n), displayName(n), role(r), relationship(RelationshipLevel::STRANGER),
      friendshipPoints(0), isRomanceable(true), currentHour(8) {
    initializeDialogues();
}

void NPC::initializeDialogues() {
    // Default greetings
    std::vector<DialogueLine> greetings;
    
    DialogueLine morningGreeting;
    morningGreeting.text = "Good morning! Beautiful day, isn't it?";
    morningGreeting.minRelationship = 0;
    morningGreeting.condition = "time:6-12";
    greetings.push_back(morningGreeting);
    
    DialogueLine afternoonGreeting;
    afternoonGreeting.text = "Hello there! How's your day going?";
    afternoonGreeting.minRelationship = 0;
    afternoonGreeting.condition = "time:12-17";
    greetings.push_back(afternoonGreeting);
    
    DialogueLine eveningGreeting;
    eveningGreeting.text = "Good evening! Time to rest soon.";
    eveningGreeting.minRelationship = 0;
    eveningGreeting.condition = "time:17-22";
    greetings.push_back(eveningGreeting);
    
    dialogues["greeting"] = greetings;
}

void NPC::updateSchedule(int hour) {
    currentHour = hour;
    for (const auto& entry : schedule) {
        if (hour >= entry.startHour && hour < entry.endHour) {
            currentLocation = entry.location;
            break;
        }
    }
}

bool NPC::giveGift(const std::string& itemName) {
    int change = calculateFriendshipChange(itemName);
    if (change != 0) {
        friendshipPoints = std::max(0, std::min(1000, friendshipPoints + change));
        giftsReceived.push_back(itemName);
        return true;
    }
    return false;
}

int NPC::calculateFriendshipChange(const std::string& itemName) {
    auto likedIt = likedItems.find(itemName);
    if (likedIt != likedItems.end()) {
        return likedIt->second;
    }
    
    auto dislikedIt = dislikedItems.find(itemName);
    if (dislikedIt != dislikedItems.end()) {
        return -dislikedIt->second;
    }
    
    return 0; // Neutral
}

bool NPC::acceptQuest(const std::string& questId) {
    for (auto& quest : availableQuests) {
        if (quest.id == questId && !quest.isActive && !quest.isCompleted) {
            quest.isActive = true;
            activeQuests.push_back(quest);
            return true;
        }
    }
    return false;
}

bool NPC::completeQuest(const std::string& questId, const std::map<std::string, int>& deliveredItems) {
    for (auto& quest : activeQuests) {
        if (quest.id == questId) {
            // Check if all required items are delivered
            bool canComplete = true;
            for (const auto& req : quest.requiredItems) {
                auto deliveredIt = deliveredItems.find(req.first);
                if (deliveredIt == deliveredItems.end() || deliveredIt->second < req.second) {
                    canComplete = false;
                    break;
                }
            }
            
            if (canComplete) {
                quest.isCompleted = true;
                quest.isActive = false;
                completedQuests.push_back(quest);
                
                // Remove from active quests
                activeQuests.erase(
                    std::remove_if(activeQuests.begin(), activeQuests.end(),
                        [&questId](const NPCQuest& q) { return q.id == questId; }),
                    activeQuests.end()
                );
                return true;
            }
        }
    }
    return false;
}

std::string NPC::getDialogue(int hour, int season, int weather) {
    // Simple dialogue selection based on time
    if (hour >= 6 && hour < 12) {
        return "Good morning! The early bird catches the worm!";
    } else if (hour >= 12 && hour < 17) {
        return "Hello! Having a productive day?";
    } else if (hour >= 17 && hour < 22) {
        return "Good evening! Time to wind down.";
    } else {
        return "*yawn* It's late...";
    }
}

RelationshipLevel NPC::getRelationshipLevel() const {
    if (friendshipPoints >= 800) return RelationshipLevel::MARRIED;
    if (friendshipPoints >= 600) return RelationshipLevel::ROMANTIC;
    if (friendshipPoints >= 400) return RelationshipLevel::BEST_FRIEND;
    if (friendshipPoints >= 250) return RelationshipLevel::GOOD_FRIEND;
    if (friendshipPoints >= 100) return RelationshipLevel::FRIEND;
    if (friendshipPoints >= 50) return RelationshipLevel::ACQUAINTANCE;
    return RelationshipLevel::STRANGER;
}

void NPC::advanceRelationship() {
    RelationshipLevel current = getRelationshipLevel();
    // Logic for relationship advancement events
}

bool NPC::canMarry() const {
    return isRomanceable && getRelationshipLevel() == RelationshipLevel::ROMANTIC && spouse.empty();
}

bool NPC::proposeMarriage() {
    if (!canMarry()) return false;
    // Marriage proposal logic
    return true;
}

std::string NPC::serializeState() const {
    std::ostringstream oss;
    oss << "{\"name\":\"" << escapeJson(name) << "\",";
    oss << "\"displayName\":\"" << escapeJson(displayName) << "\",";
    oss << "\"role\":" << static_cast<int>(role) << ",";
    oss << "\"friendshipPoints\":" << friendshipPoints << ",";
    oss << "\"isRomanceable\":" << (isRomanceable ? "true" : "false") << ",";
    oss << "\"currentLocation\":\"" << escapeJson(currentLocation) << "\",";
    oss << "\"currentHour\":" << currentHour << "}";
    return oss.str();
}

void NPC::deserializeState(const std::string& jsonState) {
    auto findKey = [&](const std::string& key) -> size_t {
        return jsonState.find("\"" + key + "\":");
    };
    
    auto extractString = [&](size_t pos) -> std::string {
        if (pos == std::string::npos) return "";
        size_t quoteStart = jsonState.find('"', pos + 1);
        if (quoteStart == std::string::npos) return "";
        size_t quoteEnd = jsonState.find('"', quoteStart + 1);
        if (quoteEnd == std::string::npos) return "";
        return jsonState.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
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
    
    name = extractString(findKey("name"));
    displayName = extractString(findKey("displayName"));
    role = static_cast<NPCRole>(extractNumber(findKey("role")));
    friendshipPoints = extractNumber(findKey("friendshipPoints"));
    isRomanceable = (jsonState.find("\"isRomanceable\":true") != std::string::npos);
    currentLocation = extractString(findKey("currentLocation"));
    currentHour = extractNumber(findKey("currentHour"));
}

std::string NPC::escapeJson(const std::string& input) const {
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

// NPCManager Implementation
NPCManager& NPCManager::getInstance() {
    static NPCManager instance;
    return instance;
}

NPCManager::NPCManager() {}

NPCManager::~NPCManager() {}

void NPCManager::initializeNPCs() {
    // Blacksmith
    NPC blacksmith("Forge", NPCRole::BLACKSMITH);
    blacksmith.displayName = "Forge the Blacksmith";
    blacksmith.likedItems["Iron Ore"] = 30;
    blacksmith.likedItems["Gold Ore"] = 40;
    blacksmith.likedItems["Diamond"] = 50;
    blacksmith.dislikedItems["Fish"] = 10;
    npcs["Forge"] = blacksmith;
    
    // Baker
    NPC baker("Flour", NPCRole::BAKER);
    baker.displayName = "Flour the Baker";
    baker.likedItems["Wheat"] = 30;
    baker.likedItems["Sugar"] = 25;
    baker.likedItems["Egg"] = 20;
    baker.dislikedItems["Raw Fish"] = 15;
    npcs["Flour"] = baker;
    
    // Mayor
    NPC mayor("Hart", NPCRole::MAYOR);
    mayor.displayName = "Mayor Hart";
    mayor.likedItems["Ancient Artifact"] = 50;
    mayor.likedItems["Gold Bar"] = 40;
    mayor.isRomanceable = false;
    npcs["Hart"] = mayor;
    
    // Merchant
    NPC merchant("Penny", NPCRole::MERCHANT);
    merchant.displayName = "Penny the Merchant";
    merchant.likedItems["Rare Seeds"] = 40;
    merchant.likedItems["Gemstone"] = 35;
    merchant.likedItems["Gold Coin"] = 30;
    npcs["Penny"] = merchant;
    
    // Fisherman
    NPC fisherman("Reed", NPCRole::FISHERMAN);
    fisherman.displayName = "Reed the Fisherman";
    fisherman.likedItems["Legendary Fish"] = 50;
    fisherman.likedItems["Squid"] = 30;
    fisherman.likedItems["Shrimp"] = 25;
    npcs["Reed"] = fisherman;
    
    // Miner
    NPC miner("Stone", NPCRole::MINER);
    miner.displayName = "Stone the Miner";
    miner.likedItems["Diamond"] = 50;
    miner.likedItems["Emerald"] = 45;
    miner.likedItems["Mythril"] = 40;
    npcs["Stone"] = miner;
}

NPC* NPCManager::getNPC(const std::string& name) {
    auto it = npcs.find(name);
    if (it != npcs.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<NPC*> NPCManager::getAllNPCs() {
    std::vector<NPC*> result;
    for (auto& pair : npcs) {
        result.push_back(&pair.second);
    }
    return result;
}

std::vector<NPC*> NPCManager::getNPCsAtLocation(const std::string& location) {
    std::vector<NPC*> result;
    for (auto& pair : npcs) {
        if (pair.second.currentLocation == location) {
            result.push_back(&pair.second);
        }
    }
    return result;
}

std::vector<NPC*> NPCManager::getAvailableQuestGivers() {
    std::vector<NPC*> result;
    for (auto& pair : npcs) {
        if (!pair.second.availableQuests.empty()) {
            result.push_back(&pair.second);
        }
    }
    return result;
}

std::vector<NPCQuest> NPCManager::getActiveQuests() {
    std::vector<NPCQuest> result;
    for (auto& pair : npcs) {
        for (const auto& quest : pair.second.activeQuests) {
            result.push_back(quest);
        }
    }
    return result;
}

std::vector<NPCQuest> NPCManager::getCompletedQuests() {
    std::vector<NPCQuest> result;
    for (auto& pair : npcs) {
        for (const auto& quest : pair.second.completedQuests) {
            result.push_back(quest);
        }
    }
    return result;
}

void NPCManager::updateTime(int hour) {
    updateAllSchedules();
}

void NPCManager::updateAllSchedules() {
    for (auto& pair : npcs) {
        pair.second.updateSchedule(pair.second.currentHour);
    }
}

bool NPCManager::giveGiftToNPC(const std::string& npcName, const std::string& itemName) {
    NPC* npc = getNPC(npcName);
    if (npc) {
        return npc->giveGift(itemName);
    }
    return false;
}

bool NPCManager::acceptQuestFromNPC(const std::string& npcName, const std::string& questId) {
    NPC* npc = getNPC(npcName);
    if (npc) {
        return npc->acceptQuest(questId);
    }
    return false;
}

bool NPCManager::turnInQuest(const std::string& questId, const std::map<std::string, int>& items) {
    for (auto& pair : npcs) {
        if (pair.second.completeQuest(questId, items)) {
            return true;
        }
    }
    return false;
}

std::string NPCManager::serializeState() const {
    std::ostringstream oss;
    oss << "{\"npcs\":[";
    bool first = true;
    for (const auto& pair : npcs) {
        if (!first) oss << ",";
        oss << pair.second.serializeState();
        first = false;
    }
    oss << "]}";
    return oss.str();
}

void NPCManager::deserializeState(const std::string& jsonState) {
    // Parse and restore NPC states
    // Simplified for brevity
}
