import { useCallback, useEffect, useRef, useState } from "react";
import {
  getFarmState,
  harvestCrop,
  clearWitheredCrop,
  plantSeed,
  waterCrop,
  subscribeToFarmEvents,
  updateWorld,
} from "../bridge/gameBridge";
import type { FarmEventDetail, FarmState } from "../bridge/types";

const TICK_INTERVAL_MS = 2000;

export function useGameBridge() {
  const [state, setState] = useState<FarmState>({
    inventory: {},
    grid: [],
    ticks: 0,
  });
  const [ready, setReady] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [lastEvent, setLastEvent] = useState<FarmEventDetail | null>(null);
  const tickIntervalRef = useRef<ReturnType<typeof setInterval> | null>(null);
  const aliveRef = useRef(true);

  const refreshState = useCallback(async () => {
    try {
      const snapshot = await getFarmState();
      if (!aliveRef.current) return;
      setState(snapshot);
      setReady(true);
      setError(null);
    } catch (error) {
      if (aliveRef.current) {
        setError(error instanceof Error ? error.message : "Unknown bridge initialization error");
        console.warn("Farm bridge is still loading:", error);
      }
    }
  }, []);

  useEffect(() => {
    aliveRef.current = true;

    const unsubscribe = subscribeToFarmEvents((event) => {
      if (!aliveRef.current) return;
      setLastEvent(event);

      if (
        event.type === "inventoryUpdated" ||
        event.type === "cropPlanted" ||
        event.type === "cropHarvested" ||
        event.type === "cropReady" ||
        event.type === "cropWithered" ||
        event.type === "cropWatered"
      ) {
        void refreshState();
      }
    });

    void refreshState();

    return () => {
      aliveRef.current = false;
      unsubscribe();
      if (tickIntervalRef.current) {
        clearInterval(tickIntervalRef.current);
        tickIntervalRef.current = null;
      }
    };
  }, [refreshState]);

  const startAutoTick = useCallback(() => {
    if (tickIntervalRef.current) return;
    tickIntervalRef.current = setInterval(() => {
      if (aliveRef.current) {
        void updateWorld();
      }
    }, TICK_INTERVAL_MS);
  }, []);

  const stopAutoTick = useCallback(() => {
    if (tickIntervalRef.current) {
      clearInterval(tickIntervalRef.current);
      tickIntervalRef.current = null;
    }
  }, []);

  const handlePlantSeed = useCallback(async (x: number, y: number, cropName: string) => {
    const success = await plantSeed(x, y, cropName);
    if (success) void refreshState();
    return success;
  }, [refreshState]);

  const handleHarvestCrop = useCallback(async (x: number, y: number) => {
    const success = await harvestCrop(x, y);
    if (success) void refreshState();
    return success;
  }, [refreshState]);

  const handleClearWithered = useCallback(async (x: number, y: number) => {
    const success = await clearWitheredCrop(x, y);
    if (success) void refreshState();
    return success;
  }, [refreshState]);

  const handleWaterCrop = useCallback(async (x: number, y: number) => {
    const success = await waterCrop(x, y);
    if (success) void refreshState();
    return success;
  }, [refreshState]);

  const handleUpdateWorld = useCallback(async () => {
    await updateWorld();
    void refreshState();
  }, [refreshState]);

  return {
    ready,
    error,
    state,
    lastEvent,
    plantSeed: handlePlantSeed,
    harvestCrop: handleHarvestCrop,
    clearWithered: handleClearWithered,
    waterCrop: handleWaterCrop,
    updateWorld: handleUpdateWorld,
    startAutoTick,
    stopAutoTick,
  };
}
