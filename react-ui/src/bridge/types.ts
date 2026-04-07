export type CropStageType = "Seed" | "Sprout" | "Growing" | "Mature" | "Ready to Harvest" | "Withered";

export interface CropSnapshot {
  x: number;
  y: number;
  crop: string;
  growth: number;
  stage: CropStageType;
  withered: boolean;
  watered: boolean;
}

export interface FarmState {
  inventory: Record<string, number>;
  grid: Array<CropSnapshot | null>;
  ticks: number;
}

export interface FarmEventDetail {
  type: string;
  data: string;
  payload: Record<string, unknown> | string;
}

export interface GameModule {
  ccall: <T = unknown>(
    ident: string,
    returnType: string | null,
    argTypes: string[],
    args: unknown[]
  ) => T;
  cwrap: <T = (...args: unknown[]) => unknown>(
    ident: string,
    returnType: string | null,
    argTypes: string[]
  ) => T;
}

export interface CropDefinition {
  name: string;
  growthTime: number;
  displayName: string;
  seedCost: number;
  harvestYield: number;
  waterBonus: number;
}

export const CROP_DEFINITIONS: Record<string, CropDefinition> = {
  Carrot: { name: "Carrot", growthTime: 100, displayName: "Carrot", seedCost: 10, harvestYield: 25, waterBonus: 1.5 },
  Tomato: { name: "Tomato", growthTime: 150, displayName: "Tomato", seedCost: 15, harvestYield: 40, waterBonus: 1.8 },
  Potato: { name: "Potato", growthTime: 120, displayName: "Potato", seedCost: 12, harvestYield: 30, waterBonus: 1.4 },
  Wheat: { name: "Wheat", growthTime: 80, displayName: "Wheat", seedCost: 5, harvestYield: 15, waterBonus: 1.2 },
  Corn: { name: "Corn", growthTime: 130, displayName: "Corn", seedCost: 14, harvestYield: 35, waterBonus: 1.6 },
  Strawberry: { name: "Strawberry", growthTime: 180, displayName: "Strawberry", seedCost: 20, harvestYield: 50, waterBonus: 2.0 },
  Pumpkin: { name: "Pumpkin", growthTime: 250, displayName: "Pumpkin", seedCost: 30, harvestYield: 80, waterBonus: 2.2 },
  Sunflower: { name: "Sunflower", growthTime: 110, displayName: "Sunflower", seedCost: 18, harvestYield: 45, waterBonus: 1.7 },
  Pepper: { name: "Pepper", growthTime: 140, displayName: "Pepper", seedCost: 16, harvestYield: 38, waterBonus: 1.6 },
  Blueberry: { name: "Blueberry", growthTime: 200, displayName: "Blueberry", seedCost: 25, harvestYield: 65, waterBonus: 2.1 },
};

export const TOOL_DEFINITIONS = {
  plant: { name: "Plant", icon: "\u{1F331}" },
  harvest: { name: "Harvest", icon: "\u{1F33E}" },
  water: { name: "Water", icon: "\u{1F4A7}" },
  clear: { name: "Clear", icon: "\u{1F9F9}" },
};
