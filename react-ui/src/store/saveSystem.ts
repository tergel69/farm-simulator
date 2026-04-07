import type { FarmState } from "../bridge/types";

const SAVE_KEY = "viby-farm-save";
const SETTINGS_KEY = "viby-farm-settings";

export interface GameSettings {
  musicVolume: number;
  sfxVolume: number;
  ambientVolume: number;
  showTutorial: boolean;
  graphicsQuality: "low" | "medium" | "high";
}

export interface SaveData {
  farmState: FarmState;
  coins: number;
  xp: number;
  level: number;
  achievements: string[];
  selectedCrop: string;
  selectedTool: string;
  totalHarvests: number;
  totalPlantings: number;
  totalEarnings: number;
  timestamp: number;
}

export const DEFAULT_SETTINGS: GameSettings = {
  musicVolume: 0.5,
  sfxVolume: 0.7,
  ambientVolume: 0.4,
  showTutorial: true,
  graphicsQuality: "high",
};

export const ACHIEVEMENTS = {
  FIRST_HARVEST: { id: "first_harvest", name: "First Harvest", description: "Harvest your first crop", icon: "🌾" },
  GREEN_THUMB: { id: "green_thumb", name: "Green Thumb", description: "Harvest 10 crops", icon: "🌿" },
  FARMING_PRO: { id: "farming_pro", name: "Farming Pro", description: "Harvest 50 crops", icon: "🚜" },
  FIRST_COINS: { id: "first_coins", name: "First Coins", description: "Earn 50 coins", icon: "💰" },
  RICH_FARMER: { id: "rich_farmer", name: "Rich Farmer", description: "Earn 500 coins", icon: "💎" },
  PLANTER: { id: "planter", name: "Planter", description: "Plant 25 crops", icon: "🌱" },
  MASTER_FARMER: { id: "master_farmer", name: "Master Farmer", description: "Reach level 10", icon: "⭐" },
  NO_WASTE: { id: "no_waste", name: "No Waste", description: "Clear 10 withered crops", icon: "🧹" },
  NIGHT_OWL: { id: "night_owl", name: "Night Owl", description: "Play through 5 nights", icon: "🦉" },
  WATER_WIZARD: { id: "water_wizard", name: "Water Wizard", description: "Water 20 crops", icon: "💧" },
};

export function saveGame(data: SaveData): void {
  try {
    localStorage.setItem(SAVE_KEY, JSON.stringify(data));
  } catch (e) {
    console.warn("Failed to save game:", e);
  }
}

export function loadGame(): SaveData | null {
  try {
    const raw = localStorage.getItem(SAVE_KEY);
    if (!raw) return null;
    return JSON.parse(raw) as SaveData;
  } catch (e) {
    console.warn("Failed to load game:", e);
    return null;
  }
}

export function saveSettings(settings: GameSettings): void {
  try {
    localStorage.setItem(SETTINGS_KEY, JSON.stringify(settings));
  } catch (e) {
    console.warn("Failed to save settings:", e);
  }
}

export function loadSettings(): GameSettings {
  try {
    const raw = localStorage.getItem(SETTINGS_KEY);
    if (!raw) return { ...DEFAULT_SETTINGS };
    return { ...DEFAULT_SETTINGS, ...JSON.parse(raw) };
  } catch (e) {
    console.warn("Failed to load settings:", e);
    return { ...DEFAULT_SETTINGS };
  }
}

export function checkAchievements(
  current: string[],
  stats: { totalHarvests: number; totalEarnings: number; totalPlantings: number; level: number; totalCleared: number; totalWatered: number; nightsPassed: number }
): string[] {
  const newAchievements = [...current];
  const addIf = (id: string, condition: boolean) => {
    if (condition && !newAchievements.includes(id)) {
      newAchievements.push(id);
    }
  };

  addIf(ACHIEVEMENTS.FIRST_HARVEST.id, stats.totalHarvests >= 1);
  addIf(ACHIEVEMENTS.GREEN_THUMB.id, stats.totalHarvests >= 10);
  addIf(ACHIEVEMENTS.FARMING_PRO.id, stats.totalHarvests >= 50);
  addIf(ACHIEVEMENTS.FIRST_COINS.id, stats.totalEarnings >= 50);
  addIf(ACHIEVEMENTS.RICH_FARMER.id, stats.totalEarnings >= 500);
  addIf(ACHIEVEMENTS.PLANTER.id, stats.totalPlantings >= 25);
  addIf(ACHIEVEMENTS.MASTER_FARMER.id, stats.level >= 10);
  addIf(ACHIEVEMENTS.NO_WASTE.id, stats.totalCleared >= 10);
  addIf(ACHIEVEMENTS.NIGHT_OWL.id, stats.nightsPassed >= 5);
  addIf(ACHIEVEMENTS.WATER_WIZARD.id, stats.totalWatered >= 20);

  return newAchievements;
}

export function getXpForLevel(level: number): number {
  return level * 50 + level * level * 10;
}

export function addXp(currentXp: number, currentLevel: number, amount: number): { xp: number; level: number; leveledUp: boolean } {
  let xp = currentXp + amount;
  let level = currentLevel;
  let leveledUp = false;
  const needed = getXpForLevel(level);

  while (xp >= needed) {
    xp -= getXpForLevel(level);
    level++;
    leveledUp = true;
  }

  return { xp, level, leveledUp };
}
