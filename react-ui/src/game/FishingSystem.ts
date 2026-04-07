import * as THREE from 'three';

export interface FishType {
  name: string;
  icon: string;
  value: number;
  xpReward: number;
  rarity: 'common' | 'uncommon' | 'rare' | 'legendary';
  color: number;
  size: number;
  minDepth: number;
}

export const FISH_TYPES: FishType[] = [
  { name: 'Minnow', icon: '🐟', value: 5, xpReward: 3, rarity: 'common', color: 0x88AACC, size: 0.15, minDepth: 0 },
  { name: 'Bass', icon: '🐠', value: 15, xpReward: 8, rarity: 'common', color: 0x558866, size: 0.25, minDepth: 0 },
  { name: 'Trout', icon: '🐟', value: 25, xpReward: 12, rarity: 'uncommon', color: 0xAA8866, size: 0.3, minDepth: 0.3 },
  { name: 'Salmon', icon: '🐠', value: 35, xpReward: 18, rarity: 'uncommon', color: 0xFF8866, size: 0.35, minDepth: 0.4 },
  { name: 'Catfish', icon: '🐡', value: 50, xpReward: 25, rarity: 'rare', color: 0x666644, size: 0.45, minDepth: 0.5 },
  { name: 'Pike', icon: '🐟', value: 65, xpReward: 30, rarity: 'rare', color: 0x447744, size: 0.5, minDepth: 0.6 },
  { name: 'Golden Fish', icon: '✨', value: 150, xpReward: 60, rarity: 'legendary', color: 0xFFD700, size: 0.4, minDepth: 0.8 },
  { name: 'Rainbow Fish', icon: '🌈', value: 100, xpReward: 45, rarity: 'legendary', color: 0xFF66AA, size: 0.35, minDepth: 0.7 },
];

export function getRandomFish(timeOfDay: 'day' | 'night', isRaining: boolean): FishType {
  let available = FISH_TYPES.filter(f => f.minDepth <= 0.5);
  
  if (timeOfDay === 'night') {
    available = FISH_TYPES.filter(f => f.rarity === 'rare' || f.rarity === 'legendary' || Math.random() > 0.5);
  }
  
  if (isRaining) {
    available = FISH_TYPES.filter(f => f.rarity !== 'common' || Math.random() > 0.7);
  }

  const weights = available.map(f => {
    switch (f.rarity) {
      case 'common': return 40;
      case 'uncommon': return 25;
      case 'rare': return 10;
      case 'legendary': return 3;
    }
  });

  const totalWeight = weights.reduce((a, b) => a + b, 0);
  let random = Math.random() * totalWeight;
  
  for (let i = 0; i < available.length; i++) {
    random -= weights[i];
    if (random <= 0) return available[i];
  }
  
  return available[0];
}

export interface FishingRod3D {
  group: THREE.Group;
  line: THREE.Line | null;
  bobber: THREE.Group | null;
  isCasting: boolean;
  isWaiting: boolean;
  isReeling: boolean;
  castTime: number;
  waitTimer: number;
  currentFish: FishType | null;
  biteProgress: number;
  hasBitten: boolean;
}

export function createFishingRod(): FishingRod3D {
  const group = new THREE.Group();

  const rodGeo = new THREE.CylinderGeometry(0.015, 0.025, 1.5, 6);
  const rodMat = new THREE.MeshStandardMaterial({ color: 0x8B4513, roughness: 0.8 });
  const rod = new THREE.Mesh(rodGeo, rodMat);
  rod.rotation.z = Math.PI / 6;
  rod.position.set(0.3, 0.8, 0.2);
  rod.castShadow = true;
  group.add(rod);

  const reelGeo = new THREE.CylinderGeometry(0.06, 0.06, 0.08, 8);
  const reelMat = new THREE.MeshStandardMaterial({ color: 0x888888, roughness: 0.4, metalness: 0.6 });
  const reel = new THREE.Mesh(reelGeo, reelMat);
  reel.rotation.x = Math.PI / 2;
  reel.position.set(0.25, 0.55, 0.2);
  group.add(reel);

  const handleGeo = new THREE.CylinderGeometry(0.02, 0.025, 0.3, 6);
  const handleMat = new THREE.MeshStandardMaterial({ color: 0x5c3a1e, roughness: 0.9 });
  const handle = new THREE.Mesh(handleGeo, handleMat);
  handle.rotation.z = Math.PI / 6;
  handle.position.set(0.15, 0.3, 0.2);
  group.add(handle);

  group.visible = false;

  return {
    group,
    line: null,
    bobber: null,
    isCasting: false,
    isWaiting: false,
    isReeling: false,
    castTime: 0,
    waitTimer: 0,
    currentFish: null,
    biteProgress: 0,
    hasBitten: false,
  };
}

export function createPond(): THREE.Group {
  const group = new THREE.Group();

  const pondGeo = new THREE.CylinderGeometry(4, 3.5, 0.5, 24);
  const pondMat = new THREE.MeshStandardMaterial({
    color: 0x2E7D32,
    roughness: 0.95,
  });
  const pondEdge = new THREE.Mesh(pondGeo, pondMat);
  pondEdge.position.y = 0.25;
  pondEdge.receiveShadow = true;
  group.add(pondEdge);

  const waterGeo = new THREE.CylinderGeometry(3.8, 3.3, 0.4, 24);
  const waterMat = new THREE.MeshStandardMaterial({
    color: 0x1E88E5,
    transparent: true,
    opacity: 0.7,
    roughness: 0.1,
    metalness: 0.2,
  });
  const water = new THREE.Mesh(waterGeo, waterMat);
  water.position.y = 0.3;
  water.name = 'pondWater';
  group.add(water);

  const lilyPadMat = new THREE.MeshStandardMaterial({ color: 0x4CAF50, roughness: 0.8, side: THREE.DoubleSide });
  for (let i = 0; i < 6; i++) {
    const lilyGeo = new THREE.CircleGeometry(0.3 + Math.random() * 0.2, 8);
    const lily = new THREE.Mesh(lilyGeo, lilyPadMat);
    const angle = (i / 6) * Math.PI * 2;
    const radius = 1 + Math.random() * 2;
    lily.position.set(Math.cos(angle) * radius, 0.52, Math.sin(angle) * radius);
    lily.rotation.x = -Math.PI / 2;
    lily.rotation.z = Math.random() * Math.PI;
    group.add(lily);
  }

  for (let i = 0; i < 3; i++) {
    const flowerGeo = new THREE.SphereGeometry(0.08, 6, 4);
    const flowerMat = new THREE.MeshStandardMaterial({ color: i % 2 === 0 ? 0xFF69B4 : 0xFFFFFF, roughness: 0.6 });
    const flower = new THREE.Mesh(flowerGeo, flowerMat);
    const angle = (i / 3) * Math.PI * 2 + 0.5;
    flower.position.set(Math.cos(angle) * 2, 0.55, Math.sin(angle) * 2);
    group.add(flower);
  }

  const reedMat = new THREE.MeshStandardMaterial({ color: 0x5D4037, roughness: 0.9 });
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x66BB6A, roughness: 0.8, side: THREE.DoubleSide });
  for (let i = 0; i < 12; i++) {
    const angle = (i / 12) * Math.PI * 2;
    const radius = 3.8 + Math.random() * 0.5;
    const reedGroup = new THREE.Group();
    
    const reedGeo = new THREE.CylinderGeometry(0.02, 0.03, 0.8 + Math.random() * 0.4, 4);
    const reed = new THREE.Mesh(reedGeo, reedMat);
    reed.position.y = 0.4;
    reedGroup.add(reed);

    const leafGeo = new THREE.PlaneGeometry(0.15, 0.5);
    const leaf = new THREE.Mesh(leafGeo, leafMat);
    leaf.position.set(0.05, 0.6, 0);
    leaf.rotation.z = 0.3;
    reedGroup.add(leaf);

    reedGroup.position.set(Math.cos(angle) * radius, 0, Math.sin(angle) * radius);
    reedGroup.rotation.y = angle;
    group.add(reedGroup);
  }

  group.position.set(18, 0, 12);

  return group;
}

export function createBobber(): THREE.Group {
  const group = new THREE.Group();

  const bottomGeo = new THREE.SphereGeometry(0.08, 8, 6, 0, Math.PI * 2, 0, Math.PI / 2);
  const bottomMat = new THREE.MeshStandardMaterial({ color: 0xFFFFFF, roughness: 0.5 });
  const bottom = new THREE.Mesh(bottomGeo, bottomMat);
  group.add(bottom);

  const topGeo = new THREE.SphereGeometry(0.08, 8, 6, 0, Math.PI * 2, 0, Math.PI / 2);
  const topMat = new THREE.MeshStandardMaterial({ color: 0xFF4444, roughness: 0.5 });
  const top = new THREE.Mesh(topGeo, topMat);
  top.position.y = 0;
  top.rotation.x = Math.PI;
  group.add(top);

  const antennaGeo = new THREE.CylinderGeometry(0.01, 0.01, 0.15, 4);
  const antennaMat = new THREE.MeshStandardMaterial({ color: 0xFF4444, roughness: 0.6 });
  const antenna = new THREE.Mesh(antennaGeo, antennaMat);
  antenna.position.y = 0.08;
  group.add(antenna);

  group.position.y = 0.55;
  return group;
}

export function updateFishingRod(rod: FishingRod3D, delta: number, time: number) {
  if (!rod.isWaiting && !rod.isCasting && !rod.isReeling) return;

  if (rod.isCasting) {
    rod.castTime += delta;
    if (rod.castTime > 0.5) {
      rod.isCasting = false;
      rod.isWaiting = true;
      rod.waitTimer = 3 + Math.random() * 7;
      rod.biteProgress = 0;
      rod.hasBitten = false;
    }
  }

  if (rod.isWaiting) {
    rod.waitTimer -= delta;
    rod.biteProgress += delta / (8 - rod.waitTimer);

    if (rod.bobber) {
      rod.bobber.position.y = 0.55 + Math.sin(time * 2) * 0.03;
    }

    if (rod.waitTimer <= 0 && !rod.hasBitten) {
      rod.hasBitten = true;
      rod.waitTimer = 2;
    }

    if (rod.hasBitten) {
      rod.waitTimer -= delta;
      if (rod.bobber) {
        rod.bobber.position.y = 0.55 + Math.sin(time * 10) * 0.08;
      }
      if (rod.waitTimer <= 0) {
        rod.hasBitten = false;
        rod.isWaiting = false;
        rod.currentFish = null;
        if (rod.bobber) {
          rod.bobber.visible = false;
        }
      }
    }
  }

  if (rod.isReeling) {
    rod.castTime += delta;
    if (rod.bobber) {
      rod.bobber.position.y = 0.55 - rod.castTime * 0.5;
    }
    if (rod.castTime > 1) {
      rod.isReeling = false;
      rod.isWaiting = false;
      if (rod.bobber) {
        rod.bobber.visible = false;
      }
    }
  }
}

export function startFishing(rod: FishingRod3D, fish: FishType): void {
  rod.isCasting = true;
  rod.castTime = 0;
  rod.currentFish = fish;
  rod.hasBitten = false;

  if (!rod.bobber) {
    rod.bobber = createBobber();
  }
  rod.bobber.visible = true;
}

export function reelInFish(rod: FishingRod3D): FishType | null {
  if (!rod.hasBitten || !rod.currentFish) return null;

  const fish = rod.currentFish;
  rod.isReeling = true;
  rod.castTime = 0;
  rod.hasBitten = false;
  rod.isWaiting = false;
  rod.currentFish = null;

  return fish;
}

export function cancelFishing(rod: FishingRod3D): void {
  rod.isCasting = false;
  rod.isWaiting = false;
  rod.isReeling = false;
  rod.hasBitten = false;
  rod.currentFish = null;
  if (rod.bobber) {
    rod.bobber.visible = false;
  }
}
