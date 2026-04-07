import { create } from 'zustand';
import type { FarmState } from '../bridge/types';
import type { GameSettings, SaveData } from './saveSystem';
import { DEFAULT_SETTINGS } from './saveSystem';

interface GameState {
  coins: number;
  xp: number;
  level: number;
  achievements: string[];
  selectedCrop: string;
  selectedTool: string;
  autoTick: boolean;
  totalHarvests: number;
  totalPlantings: number;
  totalEarnings: number;
  totalCleared: number;
  totalWatered: number;
  nightsPassed: number;
  settings: GameSettings;
  showSettings: boolean;
  showAchievements: boolean;
  notification: string | null;
  levelUpNotification: boolean;
  newAchievement: string | null;

  setCoins: (coins: number) => void;
  addCoins: (amount: number) => void;
  addXp: (amount: number) => void;
  setLevel: (level: number) => void;
  addAchievement: (id: string) => void;
  setSelectedCrop: (crop: string) => void;
  setSelectedTool: (tool: string) => void;
  setAutoTick: (autoTick: boolean) => void;
  incrementStat: (stat: 'totalHarvests' | 'totalPlantings' | 'totalEarnings' | 'totalCleared' | 'totalWatered' | 'nightsPassed', amount?: number) => void;
  setSettings: (settings: Partial<GameSettings>) => void;
  setShowSettings: (show: boolean) => void;
  setShowAchievements: (show: boolean) => void;
  setNotification: (msg: string | null) => void;
  setLevelUpNotification: (show: boolean) => void;
  setNewAchievement: (id: string | null) => void;
  loadSave: (data: SaveData) => void;
  exportSave: (farmState: FarmState) => SaveData;
}

export const useGameStore = create<GameState>((set, get) => ({
  coins: 50,
  xp: 0,
  level: 1,
  achievements: [],
  selectedCrop: 'Carrot',
  selectedTool: 'plant',
  autoTick: false,
  totalHarvests: 0,
  totalPlantings: 0,
  totalEarnings: 0,
  totalCleared: 0,
  totalWatered: 0,
  nightsPassed: 0,
  settings: DEFAULT_SETTINGS,
  showSettings: false,
  showAchievements: false,
  notification: null,
  levelUpNotification: false,
  newAchievement: null,

  setCoins: (coins) => set({ coins }),
  addCoins: (amount) => set((state) => ({ coins: state.coins + amount })),
  addXp: (amount) => set((state) => ({ xp: state.xp + amount })),
  setLevel: (level) => set({ level }),
  addAchievement: (id) => set((state) => ({ achievements: [...state.achievements, id] })),
  setSelectedCrop: (selectedCrop) => set({ selectedCrop }),
  setSelectedTool: (selectedTool) => set({ selectedTool }),
  setAutoTick: (autoTick) => set({ autoTick }),
  incrementStat: (stat, amount = 1) => set((state) => ({ [stat]: state[stat] + amount })),
  setSettings: (newSettings) => set((state) => ({ settings: { ...state.settings, ...newSettings } })),
  setShowSettings: (showSettings) => set({ showSettings }),
  setShowAchievements: (showAchievements) => set({ showAchievements }),
  setNotification: (notification) => set({ notification }),
  setLevelUpNotification: (levelUpNotification) => set({ levelUpNotification }),
  setNewAchievement: (newAchievement) => set({ newAchievement }),

  loadSave: (data) => set({
    coins: data.coins,
    xp: data.xp,
    level: data.level,
    achievements: data.achievements,
    selectedCrop: data.selectedCrop,
    selectedTool: data.selectedTool,
    totalHarvests: data.totalHarvests,
    totalPlantings: data.totalPlantings,
    totalEarnings: data.totalEarnings,
  }),

  exportSave: (farmState) => {
    const state = get();
    return {
      farmState,
      coins: state.coins,
      xp: state.xp,
      level: state.level,
      achievements: state.achievements,
      selectedCrop: state.selectedCrop,
      selectedTool: state.selectedTool,
      totalHarvests: state.totalHarvests,
      totalPlantings: state.totalPlantings,
      totalEarnings: state.totalEarnings,
      timestamp: Date.now(),
    };
  },
}));
