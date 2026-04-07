import { useEffect, useRef, forwardRef, useImperativeHandle } from 'react';
import { GameScene } from '../../game/GameScene';
import type { FarmState } from '../../bridge/types';

interface GameCanvasProps {
  onPlotClick: (x: number, y: number, tool?: string) => void;
  farmState: FarmState;
  selectedTool: string;
}

export const GameCanvas = forwardRef<any, GameCanvasProps>(({ onPlotClick, farmState, selectedTool }, ref) => {
  const containerRef = useRef<HTMLDivElement>(null);
  const gameSceneRef = useRef<GameScene | null>(null);
  const animFrameRef = useRef<number>(0);
  const onPlotClickRef = useRef(onPlotClick);
  const selectedToolRef = useRef(selectedTool);

  onPlotClickRef.current = onPlotClick;
  selectedToolRef.current = selectedTool;

  useImperativeHandle(ref, () => ({
    spawnHarvestParticles: (x: number, y: number) => gameSceneRef.current?.spawnHarvestParticles(x, y),
    spawnPlantParticles: (x: number, y: number) => gameSceneRef.current?.spawnPlantParticles(x, y),
    spawnWaterParticles: (x: number, y: number) => gameSceneRef.current?.spawnWaterParticles(x, y),
    spawnCoinText: (x: number, y: number, amount: number) => gameSceneRef.current?.spawnCoinText(x, y, amount),
  }), []);

  useEffect(() => {
    if (!containerRef.current) return;
    if (gameSceneRef.current) return;

    const gameScene = new GameScene(containerRef.current);
    gameScene.initializeFarmGrid();
    gameScene.onPlotClick = (x, y) => {
      onPlotClickRef.current(x, y, selectedToolRef.current);
    };
    gameSceneRef.current = gameScene;

    const animate = () => {
      gameScene.animate();
      animFrameRef.current = requestAnimationFrame(animate);
    };
    animate();

    const handleResize = () => gameScene.resize();
    window.addEventListener('resize', handleResize);

    return () => {
      cancelAnimationFrame(animFrameRef.current);
      window.removeEventListener('resize', handleResize);
      gameScene.dispose();
      gameSceneRef.current = null;
    };
  }, []);

  useEffect(() => {
    if (gameSceneRef.current) {
      gameSceneRef.current.setSelectedTool(selectedTool);
    }
  }, [selectedTool]);

  useEffect(() => {
    if (gameSceneRef.current && farmState.grid.length > 0) {
      gameSceneRef.current.updateFarmState(farmState);
    }
  }, [farmState, farmState.ticks, farmState.grid.length]);

  return <div ref={containerRef} className="game-canvas" />;
});

GameCanvas.displayName = 'GameCanvas';

interface GameHUDProps {
  selectedCrop: string;
  selectedTool: string;
  coins: number;
  xp: number;
  level: number;
  xpNeeded: number;
  inventory: Record<string, number>;
  dayNumber: number;
  timeOfDay: string;
  onCropSelect: (crop: string) => void;
  onToolSelect: (tool: string) => void;
  onToggleRain: () => void;
  onToggleAutoTick: () => void;
  onAdvanceTick: () => void;
  onOpenSettings: () => void;
  onToggleAchievements: () => void;
  autoTick: boolean;
  achievements: string[];
}

const tools = [
  { id: 'plant', name: 'Plant', icon: '🌱' },
  { id: 'harvest', name: 'Harvest', icon: '🌾' },
  { id: 'water', name: 'Water', icon: '💧' },
  { id: 'clear', name: 'Clear', icon: '🧹' },
];

const cropIcons: Record<string, string> = {
  Carrot: '🥕',
  Tomato: '🍅',
  Potato: '🥔',
  Wheat: '🌾',
  Corn: '🌽',
  Strawberry: '🍓',
  Pumpkin: '🎃',
  Sunflower: '🌻',
  Pepper: '🌶️',
  Blueberry: '🫐',
};

const cropInfo: Record<string, { seedCost: number; harvestYield: number; growthTime: number }> = {
  Carrot: { seedCost: 10, harvestYield: 25, growthTime: 100 },
  Tomato: { seedCost: 15, harvestYield: 40, growthTime: 150 },
  Potato: { seedCost: 12, harvestYield: 30, growthTime: 120 },
  Wheat: { seedCost: 5, harvestYield: 15, growthTime: 80 },
  Corn: { seedCost: 14, harvestYield: 35, growthTime: 130 },
  Strawberry: { seedCost: 20, harvestYield: 50, growthTime: 180 },
  Pumpkin: { seedCost: 30, harvestYield: 80, growthTime: 250 },
  Sunflower: { seedCost: 18, harvestYield: 45, growthTime: 110 },
  Pepper: { seedCost: 16, harvestYield: 38, growthTime: 140 },
  Blueberry: { seedCost: 25, harvestYield: 65, growthTime: 200 },
};

export function GameHUD({
  selectedCrop,
  selectedTool,
  coins,
  xp,
  level,
  xpNeeded,
  inventory,
  dayNumber,
  timeOfDay,
  onCropSelect,
  onToolSelect,
  onToggleRain,
  onToggleAutoTick,
  onAdvanceTick,
  onOpenSettings,
  onToggleAchievements,
  autoTick,
  achievements,
}: GameHUDProps) {
  const currentCropInfo = cropInfo[selectedCrop];
  const xpPercent = xpNeeded > 0 ? Math.min(100, (xp / xpNeeded) * 100) : 0;

  return (
    <div className="game-hud">
      <div className="hud-top">
        <div className="hud-stats">
          <div className="hud-stat">
            <span className="hud-stat-icon">💰</span>
            <span className="hud-stat-value">{coins}</span>
          </div>
          <div className="hud-stat">
            <span className="hud-stat-icon">⭐</span>
            <span className="hud-stat-value">Lv.{level}</span>
          </div>
          <div className="hud-stat hud-stat-wide">
            <div className="xp-bar-container">
              <div className="xp-bar-fill" style={{ width: `${xpPercent}%` }} />
              <span className="xp-bar-text">{xp}/{xpNeeded} XP</span>
            </div>
          </div>
          <div className="hud-stat">
            <span className="hud-stat-icon">📅</span>
            <span className="hud-stat-value">Day {dayNumber}</span>
          </div>
          <div className="hud-stat">
            <span className="hud-stat-icon">{timeOfDay === 'day' ? '☀️' : '🌙'}</span>
            <span className="hud-stat-value">{timeOfDay === 'day' ? 'Day' : 'Night'}</span>
          </div>
        </div>

        <div className="hud-tools">
          {tools.map((tool) => (
            <button
              key={tool.id}
              className={`hud-tool ${selectedTool === tool.id ? 'active' : ''}`}
              onClick={() => onToolSelect(tool.id)}
            >
              <span className="hud-tool-icon">{tool.icon}</span>
              <span className="hud-tool-name">{tool.name}</span>
            </button>
          ))}
        </div>

        <div className="hud-menu-buttons">
          <button className="hud-menu-btn" onClick={onToggleAchievements} title="Achievements">
            🏆 ({achievements.length}/10)
          </button>
          <button className="hud-menu-btn" onClick={onOpenSettings} title="Settings">
            ⚙️
          </button>
        </div>
      </div>

      <div className="hud-bottom">
        <div className="hud-crops">
          {Object.keys(cropIcons).map((crop) => (
            <button
              key={crop}
              className={`hud-crop ${selectedCrop === crop ? 'active' : ''}`}
              onClick={() => onCropSelect(crop)}
              title={`Cost: ${cropInfo[crop].seedCost} | Yield: ${cropInfo[crop].harvestYield} | Growth: ${cropInfo[crop].growthTime} ticks`}
            >
              <span className="hud-crop-icon">{cropIcons[crop]}</span>
              <span className="hud-crop-name">{crop}</span>
            </button>
          ))}
        </div>

        <div className="hud-controls">
          <button className="hud-btn" onClick={onToggleRain}>
            🌧️ Rain
          </button>
          <button
            className={`hud-btn ${autoTick ? 'active' : ''}`}
            onClick={onToggleAutoTick}
          >
            {autoTick ? '⏸ Auto' : '▶ Auto'}
          </button>
          <button className="hud-btn hud-btn-gold" onClick={onAdvanceTick}>
            ⏭ Tick
          </button>
        </div>
      </div>

      {Object.keys(inventory).length > 0 && (
        <div className="hud-inventory">
          <span className="hud-inventory-label">Inventory:</span>
          {Object.entries(inventory).map(([name, qty]) => (
            <span key={name} className="hud-inventory-item">
              {cropIcons[name] ?? '📦'} {name} x{qty}
            </span>
          ))}
        </div>
      )}

      {currentCropInfo && (
        <div className="hud-crop-info">
          <span className="hud-crop-info-name">{selectedCrop}</span>
          <span className="hud-crop-info-detail">Seed: {currentCropInfo.seedCost} | Yield: {currentCropInfo.harvestYield}</span>
          <span className="hud-crop-info-detail">Growth: {currentCropInfo.growthTime} ticks</span>
        </div>
      )}
    </div>
  );
}
