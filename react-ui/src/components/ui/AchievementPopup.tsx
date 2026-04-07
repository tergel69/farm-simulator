import { useEffect, useState } from 'react';
import { ACHIEVEMENTS } from '../../store/saveSystem';

interface AchievementPopupProps {
  achievementId: string;
}

export function AchievementPopup({ achievementId }: AchievementPopupProps) {
  const [visible, setVisible] = useState(true);
  const achievement = Object.values(ACHIEVEMENTS).find(a => a.id === achievementId);

  useEffect(() => {
    const timer = setTimeout(() => setVisible(false), 3500);
    return () => clearTimeout(timer);
  }, []);

  if (!visible || !achievement) return null;

  return (
    <div className="achievement-popup">
      <div className="achievement-card">
        <div className="achievement-icon">{achievement.icon}</div>
        <div className="achievement-info">
          <div className="achievement-label">🏆 Achievement Unlocked!</div>
          <div className="achievement-name">{achievement.name}</div>
          <div className="achievement-desc">{achievement.description}</div>
        </div>
      </div>
    </div>
  );
}
