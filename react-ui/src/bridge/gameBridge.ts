import type { FarmEventDetail, FarmState, GameModule } from "./types";

declare global {
  interface Window {
    createModule?: (options?: Record<string, unknown>) => Promise<GameModule>;
    dispatchFarmEvent?: (type: string, data: string) => void;
    __vibyModule?: GameModule;
  }
}

let modulePromise: Promise<GameModule> | null = null;
let loaderPromise: Promise<void> | null = null;

const GRID_WIDTH = 10;
const GRID_HEIGHT = 10;
const WITHER_MULTIPLIER = 3;
const MOCK_GROWTH: Record<string, number> = {
  Carrot: 100,
  Tomato: 150,
  Potato: 120,
  Wheat: 80,
  Corn: 130,
  Strawberry: 180,
  Pumpkin: 250,
  Sunflower: 110,
  Pepper: 140,
  Blueberry: 200,
};

type MockCrop = {
  x: number;
  y: number;
  crop: string;
  growth: number;
  stage: "Seed" | "Sprout" | "Growing" | "Mature" | "Ready to Harvest" | "Withered";
  withered: boolean;
  age: number;
  growthTime: number;
  watered: boolean;
};

type MockState = {
  inventory: Record<string, number>;
  grid: Array<MockCrop | null>;
  ticks: number;
};

function safeParse<T>(value: string, fallback: T): T {
  try {
    return JSON.parse(value) as T;
  } catch {
    return fallback;
  }
}

function ensureEventDispatcher(): void {
  if (typeof window.dispatchFarmEvent === "function") {
    return;
  }

  window.dispatchFarmEvent = (type, data) => {
    const payload = typeof data === "string" ? safeParse(data, data) : data;
    window.dispatchEvent(
      new CustomEvent<FarmEventDetail>("viby-farm-event", {
        detail: { type, data, payload },
      })
    );
  };
}

function getIndex(x: number, y: number): number {
  return y * GRID_WIDTH + x;
}

function inBounds(x: number, y: number): boolean {
  return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT;
}

function getStage(age: number, growthTime: number): MockCrop["stage"] {
  if (age <= 0) return "Seed";
  if (age >= growthTime * WITHER_MULTIPLIER) return "Withered";

  const progress = Math.max(0, Math.min(100, Math.floor((age / growthTime) * 100)));
  if (progress < 25) return "Sprout";
  if (progress < 50) return "Growing";
  if (progress < 100) return "Mature";
  return "Ready to Harvest";
}

  function toSnapshot(state: MockState): FarmState {
    return {
      inventory: { ...state.inventory },
      ticks: state.ticks,
      grid: state.grid.map((crop) => {
        if (!crop) return null;
        return {
          x: crop.x,
          y: crop.y,
          crop: crop.crop,
          growth: crop.growth,
          stage: crop.stage,
          withered: crop.withered,
          watered: crop.watered,
        };
      }),
    };
  }

function createMockModule(): GameModule {
  ensureEventDispatcher();

  const state: MockState = {
    inventory: {},
    ticks: 0,
    grid: Array.from({ length: GRID_WIDTH * GRID_HEIGHT }, () => null),
  };

  const emit = (type: string, payload: Record<string, unknown>) => {
    window.dispatchFarmEvent?.(type, JSON.stringify(payload));
  };

  const initGame = () => {
    state.ticks = 0;
    state.inventory = {};
    state.grid = Array.from({ length: GRID_WIDTH * GRID_HEIGHT }, () => null);
  };

  const plantSeedImpl = (x: number, y: number, cropName: string): number => {
    if (!inBounds(x, y)) return 0;
    if (!MOCK_GROWTH[cropName]) return 0;

    const idx = getIndex(x, y);
    if (state.grid[idx]) return 0;

    state.grid[idx] = {
      x,
      y,
      crop: cropName,
      growth: 0,
      stage: "Seed",
      withered: false,
      age: 0,
      growthTime: MOCK_GROWTH[cropName],
      watered: false,
    };

    emit("cropPlanted", { x, y, crop: cropName });
    return 1;
  };

  const harvestCropImpl = (x: number, y: number): number => {
    if (!inBounds(x, y)) return 0;
    const idx = getIndex(x, y);
    const crop = state.grid[idx];
    if (!crop || crop.stage !== "Ready to Harvest") return 0;

    state.inventory[crop.crop] = (state.inventory[crop.crop] ?? 0) + 1;
    state.grid[idx] = null;
    emit("cropHarvested", { x, y, crop: crop.crop });
    emit("inventoryUpdated", { ...state.inventory });
    return 1;
  };

  const clearWitheredCropImpl = (x: number, y: number): number => {
    if (!inBounds(x, y)) return 0;
    const idx = getIndex(x, y);
    const crop = state.grid[idx];
    if (!crop || crop.stage !== "Withered") return 0;
    state.grid[idx] = null;
    emit("cropCleared", { x, y });
    return 1;
  };

  const updateWorldImpl = (): void => {
    state.ticks += 1;
    for (let i = 0; i < state.grid.length; i += 1) {
      const crop = state.grid[i];
      if (!crop) continue;

      const prevStage = crop.stage;
      const growthRate = crop.watered ? 1.5 : 1;
      crop.age += growthRate;
      crop.stage = getStage(crop.age, crop.growthTime);
      crop.withered = crop.stage === "Withered";
      crop.growth = Math.max(0, Math.min(100, Math.floor((crop.age / crop.growthTime) * 100)));

      if (crop.watered && state.ticks % 5 === 0) {
        crop.watered = false;
      }

      if (prevStage !== "Ready to Harvest" && crop.stage === "Ready to Harvest") {
        emit("cropReady", { x: crop.x, y: crop.y, crop: crop.crop });
      } else if (prevStage !== "Withered" && crop.stage === "Withered") {
        emit("cropWithered", { x: crop.x, y: crop.y, crop: crop.crop });
      }
    }
  };

  const waterCropImpl = (x: number, y: number): number => {
    if (!inBounds(x, y)) return 0;
    const idx = getIndex(x, y);
    const crop = state.grid[idx];
    if (!crop || crop.stage === "Withered") return 0;
    crop.watered = true;
    emit("cropWatered", { x, y });
    return 1;
  };

  const getGameStateJsonImpl = (): string => JSON.stringify(toSnapshot(state));

  const ccall = <T = unknown>(ident: string, _returnType: string | null, _argTypes: string[], args: unknown[]): T => {
    switch (ident) {
      case "initGame":
      case "_initGame":
        initGame();
        return null as T;
      case "plantSeed":
      case "_plantSeed":
        return plantSeedImpl(Number(args[0]), Number(args[1]), String(args[2])) as T;
      case "harvestCrop":
      case "_harvestCrop":
        return harvestCropImpl(Number(args[0]), Number(args[1])) as T;
      case "clearWitheredCrop":
      case "_clearWitheredCrop":
        return clearWitheredCropImpl(Number(args[0]), Number(args[1])) as T;
      case "updateWorld":
      case "_updateWorld":
        updateWorldImpl();
        return null as T;
      case "getGameStateJson":
      case "_getGameStateJson":
        return getGameStateJsonImpl() as T;
      case "waterCrop":
      case "_waterCrop":
        return waterCropImpl(Number(args[0]), Number(args[1])) as T;
      default:
        throw new Error(`Mock module missing export: ${ident}`);
    }
  };

  const cwrap = <T = (...args: unknown[]) => unknown>(ident: string, returnType: string | null, argTypes: string[]): T => {
    return ((...args: unknown[]) => ccall(ident, returnType, argTypes, args)) as T;
  };

  return { ccall, cwrap };
}

export async function loadFarmModule(): Promise<GameModule> {
  if (typeof window === "undefined") {
    throw new Error("The farm bridge only runs in the browser.");
  }

  if (window.__vibyModule) {
    return window.__vibyModule;
  }

  if (!window.createModule) {
    if (!loaderPromise) {
      loaderPromise = new Promise<void>((resolve, reject) => {
        const existingScript = document.querySelector<HTMLScriptElement>('script[data-viby-farm-loader="true"]');
        if (existingScript) {
          existingScript.addEventListener("load", () => resolve(), { once: true });
          existingScript.addEventListener("error", () => reject(new Error("Failed to load /viby-farm.js")), { once: true });
          return;
        }

        const script = document.createElement("script");
        script.src = "/viby-farm.js";
        script.async = true;
        script.dataset.vibyFarmLoader = "true";
        script.onload = () => resolve();
        script.onerror = () =>
          reject(
            new Error(
              "Could not load /viby-farm.js. Build the WASM bundle first (try: npm run game)."
            )
          );
        document.head.appendChild(script);
      });
    }

    try {
      await loaderPromise;
    } catch (error) {
      console.warn("WASM loader unavailable, falling back to JS simulation:", error);
      const mock = createMockModule();
      window.__vibyModule = mock;
      return mock;
    }
  }

  if (!window.createModule) {
    const mock = createMockModule();
    window.__vibyModule = mock;
    return mock;
  }

  if (!modulePromise) {
    modulePromise = window
      .createModule({
        onRuntimeInitialized() {
          ensureEventDispatcher();
        },
      })
      .then((module) => {
        window.__vibyModule = module;
        module.ccall("initGame", null, [], []);
        return module;
      })
      .catch((error) => {
        modulePromise = null;
        throw error;
      });
  }

  return modulePromise;
}

export async function getFarmState(): Promise<FarmState> {
  const module = await loadFarmModule();
  const rawState = module.ccall<string>("getGameStateJson", "string", [], []);

  return safeParse<FarmState>(rawState, {
    inventory: {},
    grid: [],
    ticks: 0,
  });
}

export async function plantSeed(x: number, y: number, cropName: string): Promise<boolean> {
  const module = await loadFarmModule();
  const result = module.ccall<number>("plantSeed", "number", ["number", "number", "string"], [x, y, cropName]);
  return result === 1;
}

export async function harvestCrop(x: number, y: number): Promise<boolean> {
  const module = await loadFarmModule();
  const result = module.ccall<number>("harvestCrop", "number", ["number", "number"], [x, y]);
  return result === 1;
}

export async function clearWitheredCrop(x: number, y: number): Promise<boolean> {
  const module = await loadFarmModule();
  const result = module.ccall<number>("clearWitheredCrop", "number", ["number", "number"], [x, y]);
  return result === 1;
}

export async function waterCrop(x: number, y: number): Promise<boolean> {
  const module = await loadFarmModule();
  const result = module.ccall<number>("waterCrop", "number", ["number", "number"], [x, y]);
  return result === 1;
}

export async function updateWorld(): Promise<void> {
  const module = await loadFarmModule();
  module.ccall("updateWorld", null, [], []);
}

export function subscribeToFarmEvents(listener: (event: FarmEventDetail) => void): () => void {
  const handler = (event: Event) => {
    listener((event as CustomEvent<FarmEventDetail>).detail);
  };

  window.addEventListener("viby-farm-event", handler);

  return () => {
    window.removeEventListener("viby-farm-event", handler);
  };
}
