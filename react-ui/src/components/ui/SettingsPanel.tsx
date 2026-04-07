import { useGameStore } from '../../store/gameStore';
import { audioSystem } from '../../store/audioSystem';
import { loadGame } from '../../store/saveSystem';

interface SettingsPanelProps {
  onClose: () => void;
  onSave: () => void;
}

export function SettingsPanel({ onClose, onSave }: SettingsPanelProps) {
  const { settings, setSettings, coins, xp, level, achievements, totalHarvests, totalPlantings, totalEarnings } = useGameStore();

  const handleMusicChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const val = parseFloat(e.target.value);
    setSettings({ musicVolume: val });
    audioSystem.setMusicVolume(val);
  };

  const handleSfxChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const val = parseFloat(e.target.value);
    setSettings({ sfxVolume: val });
    audioSystem.setSfxVolume(val);
  };

  const handleAmbientChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const val = parseFloat(e.target.value);
    setSettings({ ambientVolume: val });
    audioSystem.setAmbientVolume(val);
  };

  const handleLoad = () => {
    const saved = loadGame();
    if (saved) {
      window.location.reload();
    }
  };

  const handleReset = () => {
    if (confirm('Are you sure? This will delete all progress!')) {
      localStorage.removeItem('viby-farm-save');
      window.location.reload();
    }
  };

  return (
    <div className="settings-overlay" onClick={onClose}>
      <div className="settings-panel" onClick={(e) => e.stopPropagation()}>
        <div className="settings-header">
          <h2>⚙️ Settings</h2>
          <button className="settings-close" onClick={onClose}>✕</button>
        </div>

        <div className="settings-section">
          <h3>🔊 Audio</h3>
          <div className="setting-row">
            <label>🎵 Music</label>
            <input type="range" min="0" max="1" step="0.05" value={settings.musicVolume} onChange={handleMusicChange} />
            <span>{Math.round(settings.musicVolume * 100)}%</span>
          </div>
          <div className="setting-row">
            <label>🔔 Sound Effects</label>
            <input type="range" min="0" max="1" step="0.05" value={settings.sfxVolume} onChange={handleSfxChange} />
            <span>{Math.round(settings.sfxVolume * 100)}%</span>
          </div>
          <div className="setting-row">
            <label>🌿 Ambient</label>
            <input type="range" min="0" max="1" step="0.05" value={settings.ambientVolume} onChange={handleAmbientChange} />
            <span>{Math.round(settings.ambientVolume * 100)}%</span>
          </div>
        </div>

        <div className="settings-section">
          <h3>💾 Save / Load</h3>
          <div className="settings-buttons">
            <button className="settings-btn" onClick={onSave}>💾 Save Game</button>
            <button className="settings-btn" onClick={handleLoad}>📂 Load Game</button>
            <button className="settings-btn settings-btn-danger" onClick={handleReset}>🗑️ Reset Progress</button>
          </div>
        </div>

        <div className="settings-section">
          <h3>📊 Stats</h3>
          <div className="stats-grid">
            <div className="stat-item"><span>Level</span><span className="stat-value">{level}</span></div>
            <div className="stat-item"><span>XP</span><span className="stat-value">{xp}</span></div>
            <div className="stat-item"><span>Coins</span><span className="stat-value">{coins}</span></div>
            <div className="stat-item"><span>Harvests</span><span className="stat-value">{totalHarvests}</span></div>
            <div className="stat-item"><span>Plantings</span><span className="stat-value">{totalPlantings}</span></div>
            <div className="stat-item"><span>Earnings</span><span className="stat-value">{totalEarnings}</span></div>
            <div className="stat-item"><span>Achievements</span><span className="stat-value">{achievements.length}/10</span></div>
          </div>
        </div>

        <div className="settings-section">
          <h3>ℹ️ About</h3>
          <p className="about-text">Viby Farming Simulator v2.0</p>
          <p className="about-text">A cozy farming game with good vibes 🌾</p>
        </div>
      </div>
    </div>
  );
}
