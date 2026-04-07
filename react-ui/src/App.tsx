import { useCallback, useEffect, useRef, useState } from "react";
import { GameCanvas, GameHUD } from "./components/game3d/Game3D";
import { useGameBridge } from "./hooks/useGameBridge";
import { useGameStore } from "./store/gameStore";
import { audioSystem } from "./store/audioSystem";
import { saveGame, loadGame, loadSettings, checkAchievements, ACHIEVEMENTS, addXp, getXpForLevel } from "./store/saveSystem";
import { CROP_DEFINITIONS } from "./bridge/types";
import { SettingsPanel } from "./components/ui/SettingsPanel";
import { AchievementPopup } from "./components/ui/AchievementPopup";
import { LevelUpBanner } from "./components/ui/LevelUpBanner";

const AUTO_SAVE_INTERVAL = 30000;

export default function App() {
  const { ready, error, state, plantSeed, harvestCrop, clearWithered, waterCrop, updateWorld, startAutoTick, stopAutoTick } = useGameBridge();
  const [autoTick, setAutoTick] = useState(false);
  const [showSaveNotification, setShowSaveNotification] = useState(false);
  const gameSceneRef = useRef<any>(null);
  const lastTickRef = useRef(0);
  const autoSaveRef = useRef<ReturnType<typeof setInterval> | null>(null);

  const {
    coins, xp, level, achievements, selectedCrop, selectedTool,
    totalHarvests, totalPlantings, totalEarnings, totalCleared, totalWatered, nightsPassed,
    settings, showSettings, showAchievements, notification, levelUpNotification, newAchievement,
    addCoins, addXp: storeAddXp, setLevel, addAchievement, setSelectedCrop, setSelectedTool,
    setAutoTick: storeSetAutoTick, incrementStat, setSettings, setShowSettings,
    setShowAchievements, setNotification, setLevelUpNotification, setNewAchievement,
    loadSave, exportSave,
  } = useGameStore();

  useEffect(() => {
    const savedSettings = loadSettings();
    setSettings(savedSettings);
    audioSystem.setMusicVolume(savedSettings.musicVolume);
    audioSystem.setSfxVolume(savedSettings.sfxVolume);
    audioSystem.setAmbientVolume(savedSettings.ambientVolume);
  }, []);

  useEffect(() => {
    const saved = loadGame();
    if (saved) {
      loadSave(saved);
    }
  }, []);

  useEffect(() => {
    if (autoSaveRef.current) clearInterval(autoSaveRef.current);
    autoSaveRef.current = setInterval(() => {
      const saveData = exportSave(state);
      saveGame(saveData);
      setShowSaveNotification(true);
      setTimeout(() => setShowSaveNotification(false), 1500);
    }, AUTO_SAVE_INTERVAL);
    return () => {
      if (autoSaveRef.current) clearInterval(autoSaveRef.current);
    };
  }, [state, exportSave]);

  useEffect(() => {
    if (autoTick) {
      startAutoTick();
    } else {
      stopAutoTick();
    }
    return () => stopAutoTick();
  }, [autoTick, startAutoTick, stopAutoTick]);

  useEffect(() => {
    const currentTick = state.ticks || 0;
    if (currentTick > lastTickRef.current) {
      const nightsPassed = Math.floor((currentTick % 20) / 10) - Math.floor((lastTickRef.current % 20) / 10);
      if (nightsPassed > 0) {
        incrementStat('nightsPassed', nightsPassed);
      }
    }
    lastTickRef.current = currentTick;
  }, [state.ticks]);

  const handleAchievementCheck = useCallback((newStats: any) => {
    const newAchievements = checkAchievements(achievements, newStats);
    if (newAchievements.length > achievements.length) {
      const latest = newAchievements[newAchievements.length - 1];
      const achievement = Object.values(ACHIEVEMENTS).find(a => a.id === latest);
      if (achievement) {
        addAchievement(latest);
        setNewAchievement(latest);
        audioSystem.playAchievement();
        setTimeout(() => setNewAchievement(null), 4000);
      }
    }
  }, [achievements]);

  const handlePlotClick = useCallback((x: number, y: number, tool?: string) => {
    audioSystem.resume();
    const activeTool = tool || selectedTool;
    const index = y * 10 + x;
    const crop = state.grid[index];

    if (activeTool === 'water' && crop && crop.stage !== "Withered") {
      void waterCrop(x, y).then((success) => {
        if (success) {
          incrementStat('totalWatered');
          audioSystem.playWater();
          if (gameSceneRef.current?.spawnWaterParticles) gameSceneRef.current.spawnWaterParticles(x, y);
          setNotification(`Watered ${crop.crop}! Growth boosted.`);
          setTimeout(() => setNotification(null), 2000);
        }
      });
      return;
    }

    if (activeTool === 'harvest' && crop?.stage === "Ready to Harvest") {
      void harvestCrop(x, y).then((success) => {
        if (success) {
          const def = CROP_DEFINITIONS[crop.crop];
          const yieldCoins = def?.harvestYield || 10;
          addCoins(yieldCoins);
          const xpGain = Math.floor(yieldCoins / 2);
          const xpResult = addXp(xp, level, xpGain);
          storeAddXp(xpGain);
          if (xpResult.leveledUp) {
            setLevel(xpResult.level);
            setLevelUpNotification(true);
            audioSystem.playLevelUp();
            setTimeout(() => setLevelUpNotification(false), 3000);
          }
          incrementStat('totalHarvests');
          incrementStat('totalEarnings', yieldCoins);
          audioSystem.playHarvest();
          if (gameSceneRef.current?.spawnHarvestParticles) gameSceneRef.current.spawnHarvestParticles(x, y);
          if (gameSceneRef.current?.spawnCoinText) gameSceneRef.current.spawnCoinText(x, y, yieldCoins);
          setNotification(`Harvested ${crop.crop}! +${yieldCoins} coins`);
          setTimeout(() => setNotification(null), 2000);

          handleAchievementCheck({
            totalHarvests: totalHarvests + 1,
            totalEarnings: totalEarnings + yieldCoins,
            totalPlantings,
            level: xpResult.level,
            totalCleared,
            totalWatered,
            nightsPassed,
          });
        }
      });
      return;
    }

    if (activeTool === 'clear' && crop?.stage === "Withered") {
      void clearWithered(x, y).then(() => {
        incrementStat('totalCleared');
        audioSystem.playClear();
        setNotification("Cleared withered crop.");
        setTimeout(() => setNotification(null), 2000);
      });
      return;
    }

    if (activeTool === 'plant' && !crop) {
      const def = CROP_DEFINITIONS[selectedCrop];
      const seedCost = def?.seedCost || 10;
      if (coins < seedCost) {
        setNotification(`Not enough coins! Need ${seedCost}`);
        setTimeout(() => setNotification(null), 2000);
        return;
      }
      void plantSeed(x, y, selectedCrop).then((success) => {
        if (success) {
          addCoins(-seedCost);
          incrementStat('totalPlantings');
          audioSystem.playPlant();
          if (gameSceneRef.current?.spawnPlantParticles) gameSceneRef.current.spawnPlantParticles(x, y);
          setNotification(`Planted ${selectedCrop}! -${seedCost} coins`);
          setTimeout(() => setNotification(null), 2000);

          handleAchievementCheck({
            totalHarvests,
            totalEarnings,
            totalPlantings: totalPlantings + 1,
            level,
            totalCleared,
            totalWatered,
            nightsPassed,
          });
        } else {
          setNotification("Can't plant there!");
          setTimeout(() => setNotification(null), 2000);
        }
      });
      return;
    }

    if (crop?.stage === "Ready to Harvest") {
      void harvestCrop(x, y).then((success) => {
        if (success) {
          const def = CROP_DEFINITIONS[crop.crop];
          const yieldCoins = def?.harvestYield || 10;
          addCoins(yieldCoins);
          const xpGain = Math.floor(yieldCoins / 2);
          const xpResult = addXp(xp, level, xpGain);
          storeAddXp(xpGain);
          if (xpResult.leveledUp) {
            setLevel(xpResult.level);
            setLevelUpNotification(true);
            audioSystem.playLevelUp();
            setTimeout(() => setLevelUpNotification(false), 3000);
          }
          incrementStat('totalHarvests');
          incrementStat('totalEarnings', yieldCoins);
          audioSystem.playHarvest();
          if (gameSceneRef.current?.spawnHarvestParticles) gameSceneRef.current.spawnHarvestParticles(x, y);
          if (gameSceneRef.current?.spawnCoinText) gameSceneRef.current.spawnCoinText(x, y, yieldCoins);
          setNotification(`Harvested ${crop.crop}! +${yieldCoins} coins`);
          setTimeout(() => setNotification(null), 2000);
        }
      });
      return;
    }

    if (crop?.stage === "Withered") {
      void clearWithered(x, y).then(() => {
        incrementStat('totalCleared');
        audioSystem.playClear();
        setNotification("Cleared withered crop.");
        setTimeout(() => setNotification(null), 2000);
      });
      return;
    }

    if (!crop) {
      const def = CROP_DEFINITIONS[selectedCrop];
      const seedCost = def?.seedCost || 10;
      if (coins < seedCost) {
        setNotification(`Not enough coins! Need ${seedCost}`);
        setTimeout(() => setNotification(null), 2000);
        return;
      }
      void plantSeed(x, y, selectedCrop).then((success) => {
        if (success) {
          addCoins(-seedCost);
          incrementStat('totalPlantings');
          audioSystem.playPlant();
          if (gameSceneRef.current?.spawnPlantParticles) gameSceneRef.current.spawnPlantParticles(x, y);
          setNotification(`Planted ${selectedCrop}! -${seedCost} coins`);
          setTimeout(() => setNotification(null), 2000);
        } else {
          setNotification("Can't plant there!");
          setTimeout(() => setNotification(null), 2000);
        }
      });
    }
  }, [selectedCrop, selectedTool, coins, plantSeed, harvestCrop, clearWithered, waterCrop, state.grid]);

  const handleToggleRain = useCallback(() => {
    window.dispatchEvent(new CustomEvent('toggle-rain'));
  }, []);

  const handleToggleAutoTick = useCallback(() => {
    const newVal = !autoTick;
    setAutoTick(newVal);
    storeSetAutoTick(newVal);
  }, [autoTick]);

  const handleAdvanceTick = useCallback(() => {
    void updateWorld();
  }, [updateWorld]);

  const dayNumber = Math.floor(state.ticks / 20) + 1;
  const timeOfDay = (state.ticks % 20) < 10 ? 'day' : 'night';
  const xpNeeded = getXpForLevel(level);

  return (
    <div className="game-container">
      <GameCanvas
        onPlotClick={handlePlotClick}
        farmState={state}
        selectedTool={selectedTool}
        ref={gameSceneRef}
      />

      <GameHUD
        selectedCrop={selectedCrop}
        selectedTool={selectedTool}
        coins={coins}
        xp={xp}
        level={level}
        xpNeeded={xpNeeded}
        inventory={state.inventory}
        dayNumber={dayNumber}
        timeOfDay={timeOfDay}
        onCropSelect={setSelectedCrop}
        onToolSelect={setSelectedTool}
        onToggleRain={handleToggleRain}
        onToggleAutoTick={handleToggleAutoTick}
        onAdvanceTick={handleAdvanceTick}
        onOpenSettings={() => setShowSettings(true)}
        onToggleAchievements={() => setShowAchievements(!showAchievements)}
        autoTick={autoTick}
        achievements={achievements}
      />

      {notification && (
        <div className="game-notification">
          {notification}
        </div>
      )}

      {showSaveNotification && (
        <div className="game-save-notification">
          💾 Game Saved
        </div>
      )}

      {settings.showTutorial && ready && (
        <div className="game-tutorial">
          <div className="tutorial-content">
            <h3>🌱 Welcome to Viby Farm!</h3>
            <p>Click on empty soil to plant crops. Harvest when ready to earn coins!</p>
            <p>Use tools: 🌱 Plant | 🌾 Harvest | 💧 Water | 🧹 Clear</p>
            <p>Press 1-4 for tools, R for rain, WASD to move.</p>
            <button className="tutorial-close" onClick={() => setSettings({ showTutorial: false })}>Got it!</button>
          </div>
        </div>
      )}

      {showSettings && <SettingsPanel onClose={() => setShowSettings(false)} onSave={() => {
        const data = exportSave(state);
        saveGame(data);
        setShowSaveNotification(true);
        setTimeout(() => setShowSaveNotification(false), 1500);
      }} />}
      {newAchievement && <AchievementPopup achievementId={newAchievement} />}
      {levelUpNotification && <LevelUpBanner level={level} />}

      {!ready && (
        <div className="game-loading">
          <div className="game-loading-spinner" />
          <span>Loading farm...</span>
        </div>
      )}

      {error && (
        <div className="game-error">
          Error: {error}
        </div>
      )}
    </div>
  );
}
