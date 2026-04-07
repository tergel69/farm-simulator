import { useEffect, useState } from 'react';

interface LevelUpBannerProps {
  level: number;
}

export function LevelUpBanner({ level }: LevelUpBannerProps) {
  const [visible, setVisible] = useState(true);

  useEffect(() => {
    const timer = setTimeout(() => setVisible(false), 2500);
    return () => clearTimeout(timer);
  }, []);

  if (!visible) return null;

  return (
    <div className="level-up-banner">
      <div className="level-up-content">
        <div className="level-up-stars">⭐</div>
        <div className="level-up-text">LEVEL UP!</div>
        <div className="level-up-number">Level {level}</div>
        <div className="level-up-subtitle">Keep farming! 🌾</div>
      </div>
    </div>
  );
}
