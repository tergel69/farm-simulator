import * as THREE from 'three';
import type { FarmPlot3D } from './types3d';

const TILE_SIZE = 2;
const GRID_WIDTH = 10;
const GRID_HEIGHT = 10;

const cropColors: Record<string, { seed: number; sprout: number; growing: number; mature: number; ready: number; withered: number }> = {
  Carrot: { seed: 0x8B4513, sprout: 0x228B22, growing: 0x32CD32, mature: 0x3CB371, ready: 0xFFA500, withered: 0x6B4226 },
  Tomato: { seed: 0x8B4513, sprout: 0x228B22, growing: 0x006400, mature: 0x2E8B57, ready: 0xFF4500, withered: 0x6B4226 },
  Potato: { seed: 0x8B4513, sprout: 0x228B22, growing: 0x3CB371, mature: 0x6B8E23, ready: 0xDAA520, withered: 0x6B4226 },
  Wheat: { seed: 0x8B4513, sprout: 0x9ACD32, growing: 0xDAA520, mature: 0xFFD700, ready: 0xFFE4B5, withered: 0x6B4226 },
  Corn: { seed: 0x8B4513, sprout: 0x228B22, growing: 0x32CD32, mature: 0x7CFC00, ready: 0xFFD700, withered: 0x6B4226 },
  Strawberry: { seed: 0x8B4513, sprout: 0x228B22, growing: 0x006400, mature: 0x2E8B57, ready: 0xFF1493, withered: 0x6B4226 },
};

function getStageColor(cropType: string, stage: string): number {
  const colors = cropColors[cropType];
  if (!colors) return 0x228B22;
  switch (stage) {
    case 'Seed': return colors.seed;
    case 'Sprout': return colors.sprout;
    case 'Growing': return colors.growing;
    case 'Mature': return colors.mature;
    case 'Ready to Harvest': return colors.ready;
    case 'Withered': return colors.withered;
    default: return colors.sprout;
  }
}

function createSoilTile(): THREE.Mesh {
  const geometry = new THREE.BoxGeometry(TILE_SIZE * 0.95, 0.15, TILE_SIZE * 0.95);
  const material = new THREE.MeshStandardMaterial({
    color: 0x5c3a1e,
    roughness: 0.95,
    metalness: 0.0,
  });
  const mesh = new THREE.Mesh(geometry, material);
  mesh.position.y = 0.075;
  mesh.receiveShadow = true;
  return mesh;
}

function createSoilBorder(): THREE.LineSegments {
  const geometry = new THREE.EdgesGeometry(new THREE.BoxGeometry(TILE_SIZE * 0.97, 0.16, TILE_SIZE * 0.97));
  const material = new THREE.LineBasicMaterial({ color: 0x3d2510 });
  return new THREE.LineSegments(geometry, material);
}

function createWaterDrops(): THREE.Group {
  const group = new THREE.Group();
  const dropMat = new THREE.MeshStandardMaterial({
    color: 0x4a90d9,
    transparent: true,
    opacity: 0.6,
    roughness: 0.1,
    metalness: 0.3,
  });
  for (let i = 0; i < 6; i++) {
    const dropGeo = new THREE.SphereGeometry(0.06, 6, 4);
    const drop = new THREE.Mesh(dropGeo, dropMat);
    const angle = (i / 6) * Math.PI * 2;
    drop.position.set(Math.cos(angle) * 0.5, 0.05, Math.sin(angle) * 0.5);
    drop.scale.set(1, 0.5, 1);
    group.add(drop);
  }
  return group;
}

function makeCastShadow(group: THREE.Group) {
  group.traverse((child) => {
    if (child instanceof THREE.Mesh) {
      child.castShadow = true;
    }
  });
}

/* ---- Carrot ---- */
function createCarrotCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x228B22, roughness: 0.8, side: THREE.DoubleSide });
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });

  if (stage === 'Seed') {
    const seedGeo = new THREE.SphereGeometry(0.12, 8, 6);
    const seedMat = new THREE.MeshStandardMaterial({ color: 0x8B4513, roughness: 0.9 });
    const seed = new THREE.Mesh(seedGeo, seedMat);
    seed.position.y = 0.2;
    group.add(seed);
    const sproutGeo = new THREE.CylinderGeometry(0.02, 0.03, 0.15, 4);
    const sprout = new THREE.Mesh(sproutGeo, new THREE.MeshStandardMaterial({ color: 0x32CD32 }));
    sprout.position.y = 0.25;
    group.add(sprout);
  } else if (stage === 'Sprout') {
    const stemGeo = new THREE.CylinderGeometry(0.04, 0.05, 0.35, 6);
    group.add(new THREE.Mesh(stemGeo, stemMat).translateY(0.3));
    const leafGeo = new THREE.PlaneGeometry(0.3, 0.1);
    const l1 = new THREE.Mesh(leafGeo, leafMat); l1.position.set(0.12, 0.45, 0); l1.rotation.z = -0.5; group.add(l1);
    const l2 = new THREE.Mesh(leafGeo.clone(), leafMat); l2.position.set(-0.12, 0.42, 0); l2.rotation.z = 0.5; group.add(l2);
  } else if (stage === 'Growing') {
    const stemGeo = new THREE.CylinderGeometry(0.05, 0.07, 0.55, 6);
    group.add(new THREE.Mesh(stemGeo, stemMat).translateY(0.4));
    for (let i = 0; i < 4; i++) {
      const leafGeo = new THREE.PlaneGeometry(0.4, 0.12);
      const leaf = new THREE.Mesh(leafGeo, leafMat);
      const a = (i / 4) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.18, 0.35 + i * 0.08, Math.sin(a) * 0.18);
      leaf.rotation.y = a; leaf.rotation.z = -0.4; group.add(leaf);
    }
  } else if (stage === 'Mature') {
    const stemGeo = new THREE.CylinderGeometry(0.06, 0.08, 0.75, 6);
    group.add(new THREE.Mesh(stemGeo, stemMat).translateY(0.5));
    for (let i = 0; i < 6; i++) {
      const leafGeo = new THREE.PlaneGeometry(0.5, 0.14);
      const leaf = new THREE.Mesh(leafGeo, leafMat);
      const a = (i / 6) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.22, 0.35 + i * 0.07, Math.sin(a) * 0.22);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
  } else if (stage === 'Ready to Harvest') {
    const stemGeo = new THREE.CylinderGeometry(0.07, 0.09, 0.95, 6);
    group.add(new THREE.Mesh(stemGeo, stemMat).translateY(0.6));
    for (let i = 0; i < 8; i++) {
      const leafGeo = new THREE.PlaneGeometry(0.6, 0.16);
      const leaf = new THREE.Mesh(leafGeo, leafMat);
      const a = (i / 8) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.28, 0.35 + i * 0.08, Math.sin(a) * 0.28);
      leaf.rotation.y = a; leaf.rotation.z = -0.6; group.add(leaf);
    }
    const carrotMat = new THREE.MeshStandardMaterial({ color: 0xFFA500, roughness: 0.7 });
    const carrot = new THREE.Mesh(new THREE.ConeGeometry(0.18, 0.5, 8), carrotMat);
    carrot.position.y = 0.2; carrot.rotation.x = Math.PI; group.add(carrot);
    const top = new THREE.Mesh(new THREE.ConeGeometry(0.15, 0.35, 8), new THREE.MeshStandardMaterial({ color: 0xFF8C00 }));
    top.position.y = 1.1; group.add(top);
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.5, 0.8, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

/* ---- Tomato ---- */
function createTomatoCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x006400, roughness: 0.8, side: THREE.DoubleSide });
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });
  const fruitMat = new THREE.MeshStandardMaterial({ color: 0xFF4500, roughness: 0.4, metalness: 0.1 });

  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.12, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.05, 0.3, 6), stemMat).translateY(0.28));
    const lg = new THREE.PlaneGeometry(0.25, 0.08);
    const l1 = new THREE.Mesh(lg, leafMat); l1.position.set(0.1, 0.4, 0); l1.rotation.z = -0.4; group.add(l1);
    const l2 = new THREE.Mesh(lg.clone(), leafMat); l2.position.set(-0.1, 0.38, 0); l2.rotation.z = 0.4; group.add(l2);
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.55, 6), stemMat).translateY(0.4));
    for (let i = 0; i < 4; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.35, 0.1), leafMat);
      const a = (i / 4) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.18, 0.3 + i * 0.1, Math.sin(a) * 0.18);
      leaf.rotation.y = a; leaf.rotation.z = -0.4; group.add(leaf);
    }
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 0.75, 6), stemMat).translateY(0.5));
    for (let i = 0; i < 5; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.45, 0.12), leafMat);
      const a = (i / 5) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.22, 0.35 + i * 0.08, Math.sin(a) * 0.22);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    for (let i = 0; i < 3; i++) {
      const f = new THREE.Mesh(new THREE.SphereGeometry(0.14, 8, 6), fruitMat);
      const a = (i / 3) * Math.PI * 2;
      f.position.set(Math.cos(a) * 0.2, 0.5 + i * 0.1, Math.sin(a) * 0.2); group.add(f);
    }
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.07, 0.09, 0.95, 6), stemMat).translateY(0.6));
    for (let i = 0; i < 6; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.55, 0.14), leafMat);
      const a = (i / 6) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.28, 0.35 + i * 0.1, Math.sin(a) * 0.28);
      leaf.rotation.y = a; leaf.rotation.z = -0.6; group.add(leaf);
    }
    for (let i = 0; i < 5; i++) {
      const f = new THREE.Mesh(new THREE.SphereGeometry(0.2, 8, 6), fruitMat);
      const a = (i / 5) * Math.PI * 2;
      f.position.set(Math.cos(a) * 0.25, 0.45 + i * 0.12, Math.sin(a) * 0.25); group.add(f);
    }
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.5, 0.8, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

/* ---- Potato ---- */
function createPotatoCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x3CB371, roughness: 0.8, side: THREE.DoubleSide });
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });
  const tuberMat = new THREE.MeshStandardMaterial({ color: 0xDAA520, roughness: 0.85 });

  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.12, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.05, 0.3, 6), stemMat).translateY(0.28));
    const lg = new THREE.PlaneGeometry(0.25, 0.08);
    const l1 = new THREE.Mesh(lg, leafMat); l1.position.set(0.1, 0.4, 0); l1.rotation.z = -0.4; group.add(l1);
    const l2 = new THREE.Mesh(lg.clone(), leafMat); l2.position.set(-0.1, 0.38, 0); l2.rotation.z = 0.4; group.add(l2);
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.55, 6), stemMat).translateY(0.4));
    for (let i = 0; i < 4; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.35, 0.1), leafMat);
      const a = (i / 4) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.18, 0.3 + i * 0.1, Math.sin(a) * 0.18);
      leaf.rotation.y = a; leaf.rotation.z = -0.4; group.add(leaf);
    }
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 0.75, 6), stemMat).translateY(0.5));
    for (let i = 0; i < 5; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.45, 0.12), leafMat);
      const a = (i / 5) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.22, 0.35 + i * 0.08, Math.sin(a) * 0.22);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.07, 0.09, 0.95, 6), stemMat).translateY(0.6));
    for (let i = 0; i < 6; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.55, 0.14), leafMat);
      const a = (i / 6) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.28, 0.35 + i * 0.1, Math.sin(a) * 0.28);
      leaf.rotation.y = a; leaf.rotation.z = -0.6; group.add(leaf);
    }
    for (let i = 0; i < 4; i++) {
      const t = new THREE.Mesh(new THREE.SphereGeometry(0.18, 8, 6), tuberMat);
      const a = (i / 4) * Math.PI * 2;
      t.position.set(Math.cos(a) * 0.2, 0.2, Math.sin(a) * 0.2);
      t.scale.set(1.2, 0.7, 1); group.add(t);
    }
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.5, 0.8, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

/* ---- Wheat ---- */
function createWheatCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const stalkMat = new THREE.MeshStandardMaterial({ color: 0xDAA520, roughness: 0.85 });
  const headMat = new THREE.MeshStandardMaterial({ color: 0xFFD700, roughness: 0.7 });

  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.12, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.03, 0.04, 0.35, 6), stalkMat).translateY(0.3));
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.05, 0.65, 6), stalkMat).translateY(0.45));
    for (let i = 0; i < 2; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.25, 0.06), stalkMat);
      const a = (i / 2) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.12, 0.35 + i * 0.15, Math.sin(a) * 0.12);
      leaf.rotation.y = a; leaf.rotation.z = -0.3; group.add(leaf);
    }
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.85, 6), stalkMat).translateY(0.55));
    const head = new THREE.Mesh(new THREE.ConeGeometry(0.14, 0.4, 6), headMat);
    head.position.y = 1.05; group.add(head);
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 1.05, 6), stalkMat).translateY(0.65));
    const head = new THREE.Mesh(new THREE.ConeGeometry(0.2, 0.55, 6), headMat);
    head.position.y = 1.3; group.add(head);
    for (let i = 0; i < 4; i++) {
      const g = new THREE.Mesh(new THREE.SphereGeometry(0.06, 6, 4), headMat);
      const a = (i / 4) * Math.PI * 2;
      g.position.set(Math.cos(a) * 0.12, 1.05 + i * 0.05, Math.sin(a) * 0.12); group.add(g);
    }
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.5, 0.8, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

/* ---- Corn ---- */
function createCornCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x32CD32, roughness: 0.8, side: THREE.DoubleSide });
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });
  const cobMat = new THREE.MeshStandardMaterial({ color: 0xFFD700, roughness: 0.6 });

  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.12, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.05, 0.35, 6), stemMat).translateY(0.3));
    const lg = new THREE.PlaneGeometry(0.35, 0.07);
    const l1 = new THREE.Mesh(lg, leafMat); l1.position.set(0.14, 0.42, 0); l1.rotation.z = -0.3; group.add(l1);
    const l2 = new THREE.Mesh(lg.clone(), leafMat); l2.position.set(-0.14, 0.4, 0); l2.rotation.z = 0.3; group.add(l2);
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 0.75, 6), stemMat).translateY(0.5));
    for (let i = 0; i < 3; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.55, 0.09), leafMat);
      const a = (i / 3) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.22, 0.35 + i * 0.15, Math.sin(a) * 0.22);
      leaf.rotation.y = a; leaf.rotation.z = -0.4; group.add(leaf);
    }
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.07, 0.08, 1.05, 6), stemMat).translateY(0.65));
    for (let i = 0; i < 5; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.65, 0.11), leafMat);
      const a = (i / 5) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.28, 0.35 + i * 0.12, Math.sin(a) * 0.28);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    const cob = new THREE.Mesh(new THREE.CylinderGeometry(0.1, 0.1, 0.4, 8), cobMat);
    cob.position.set(0.2, 0.7, 0); cob.rotation.z = 0.4; group.add(cob);
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.08, 0.09, 1.25, 6), stemMat).translateY(0.75));
    for (let i = 0; i < 6; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.75, 0.13), leafMat);
      const a = (i / 6) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.32, 0.35 + i * 0.12, Math.sin(a) * 0.32);
      leaf.rotation.y = a; leaf.rotation.z = -0.6; group.add(leaf);
    }
    for (let i = 0; i < 2; i++) {
      const cob = new THREE.Mesh(new THREE.CylinderGeometry(0.12, 0.12, 0.5, 8), cobMat);
      cob.position.set(0.24, 0.65 + i * 0.38, 0); cob.rotation.z = 0.4; group.add(cob);
    }
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.5, 0.8, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

/* ---- Strawberry ---- */
function createStrawberryCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x006400, roughness: 0.8, side: THREE.DoubleSide });
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });
  const fruitMat = new THREE.MeshStandardMaterial({ color: 0xFF1493, roughness: 0.4, metalness: 0.1 });

  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.12, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.05, 0.25, 6), stemMat).translateY(0.25));
    const lg = new THREE.PlaneGeometry(0.22, 0.07);
    const l1 = new THREE.Mesh(lg, leafMat); l1.position.set(0.09, 0.38, 0); l1.rotation.z = -0.4; group.add(l1);
    const l2 = new THREE.Mesh(lg.clone(), leafMat); l2.position.set(-0.09, 0.36, 0); l2.rotation.z = 0.4; group.add(l2);
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.45, 6), stemMat).translateY(0.35));
    for (let i = 0; i < 3; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.3, 0.09), leafMat);
      const a = (i / 3) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.14, 0.25 + i * 0.08, Math.sin(a) * 0.14);
      leaf.rotation.y = a; leaf.rotation.z = -0.4; group.add(leaf);
    }
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 0.65, 6), stemMat).translateY(0.45));
    for (let i = 0; i < 5; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.4, 0.11), leafMat);
      const a = (i / 5) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.2, 0.3 + i * 0.07, Math.sin(a) * 0.2);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    for (let i = 0; i < 2; i++) {
      const f = new THREE.Mesh(new THREE.SphereGeometry(0.12, 8, 6), fruitMat);
      const a = (i / 2) * Math.PI * 2;
      f.position.set(Math.cos(a) * 0.18, 0.4, Math.sin(a) * 0.18); group.add(f);
    }
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.07, 0.08, 0.85, 6), stemMat).translateY(0.55));
    for (let i = 0; i < 6; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.5, 0.13), leafMat);
      const a = (i / 6) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.24, 0.3 + i * 0.08, Math.sin(a) * 0.24);
      leaf.rotation.y = a; leaf.rotation.z = -0.6; group.add(leaf);
    }
    for (let i = 0; i < 5; i++) {
      const f = new THREE.Mesh(new THREE.SphereGeometry(0.16, 8, 6), fruitMat);
      const a = (i / 5) * Math.PI * 2;
      f.position.set(Math.cos(a) * 0.22, 0.4 + i * 0.08, Math.sin(a) * 0.22); group.add(f);
    }
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.5, 0.8, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

/* ---- Pumpkin ---- */
function createPumpkinCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x228B22, roughness: 0.8, side: THREE.DoubleSide });
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });
  const pumpkinMat = new THREE.MeshStandardMaterial({ color: 0xFF8C00, roughness: 0.6 });

  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.15, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.3, 6), stemMat).translateY(0.25));
    const lg = new THREE.PlaneGeometry(0.3, 0.1);
    const l1 = new THREE.Mesh(lg, leafMat); l1.position.set(0.12, 0.4, 0); l1.rotation.z = -0.4; group.add(l1);
    const l2 = new THREE.Mesh(lg.clone(), leafMat); l2.position.set(-0.12, 0.38, 0); l2.rotation.z = 0.4; group.add(l2);
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 0.5, 6), stemMat).translateY(0.35));
    for (let i = 0; i < 4; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.4, 0.12), leafMat);
      const a = (i / 4) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.2, 0.3 + i * 0.08, Math.sin(a) * 0.2);
      leaf.rotation.y = a; leaf.rotation.z = -0.4; group.add(leaf);
    }
    const smallPumpkin = new THREE.Mesh(new THREE.SphereGeometry(0.2, 8, 6), pumpkinMat);
    smallPumpkin.position.set(0.15, 0.15, 0); smallPumpkin.scale.set(1, 0.7, 1); group.add(smallPumpkin);
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.07, 0.08, 0.65, 6), stemMat).translateY(0.45));
    for (let i = 0; i < 5; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.5, 0.14), leafMat);
      const a = (i / 5) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.25, 0.35 + i * 0.08, Math.sin(a) * 0.25);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    const pumpkin = new THREE.Mesh(new THREE.SphereGeometry(0.35, 8, 6), pumpkinMat);
    pumpkin.position.set(0.2, 0.2, 0); pumpkin.scale.set(1.1, 0.75, 1.1); group.add(pumpkin);
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.08, 0.09, 0.8, 6), stemMat).translateY(0.55));
    for (let i = 0; i < 6; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.6, 0.16), leafMat);
      const a = (i / 6) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.3, 0.35 + i * 0.08, Math.sin(a) * 0.3);
      leaf.rotation.y = a; leaf.rotation.z = -0.6; group.add(leaf);
    }
    for (let i = 0; i < 2; i++) {
      const pumpkin = new THREE.Mesh(new THREE.SphereGeometry(0.4 + i * 0.1, 10, 8), pumpkinMat);
      pumpkin.position.set(0.25 + i * 0.2, 0.25, i * 0.15);
      pumpkin.scale.set(1.1, 0.75, 1.1); group.add(pumpkin);
    }
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.6, 0.9, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

/* ---- Sunflower ---- */
function createSunflowerCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x32CD32, roughness: 0.8, side: THREE.DoubleSide });
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });
  const petalMat = new THREE.MeshStandardMaterial({ color: 0xFFD700, roughness: 0.6, side: THREE.DoubleSide });
  const centerMat = new THREE.MeshStandardMaterial({ color: 0x3d2510, roughness: 0.9 });

  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.12, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.05, 0.4, 6), stemMat).translateY(0.3));
    const lg = new THREE.PlaneGeometry(0.3, 0.08);
    const l1 = new THREE.Mesh(lg, leafMat); l1.position.set(0.12, 0.45, 0); l1.rotation.z = -0.3; group.add(l1);
    const l2 = new THREE.Mesh(lg.clone(), leafMat); l2.position.set(-0.12, 0.42, 0); l2.rotation.z = 0.3; group.add(l2);
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.8, 6), stemMat).translateY(0.5));
    for (let i = 0; i < 3; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.4, 0.1), leafMat);
      const a = (i / 3) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.18, 0.4 + i * 0.15, Math.sin(a) * 0.18);
      leaf.rotation.y = a; leaf.rotation.z = -0.4; group.add(leaf);
    }
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 1.1, 6), stemMat).translateY(0.65));
    for (let i = 0; i < 4; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.5, 0.12), leafMat);
      const a = (i / 4) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.22, 0.4 + i * 0.12, Math.sin(a) * 0.22);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    const flowerHead = new THREE.Group();
    flowerHead.position.y = 1.3;
    const center = new THREE.Mesh(new THREE.CylinderGeometry(0.15, 0.15, 0.1, 12), centerMat);
    center.rotation.x = Math.PI / 2; flowerHead.add(center);
    for (let i = 0; i < 10; i++) {
      const petal = new THREE.Mesh(new THREE.SphereGeometry(0.08, 6, 4), petalMat);
      const a = (i / 10) * Math.PI * 2;
      petal.position.set(Math.cos(a) * 0.2, 0, Math.sin(a) * 0.2);
      petal.scale.set(1.5, 0.3, 1); flowerHead.add(petal);
    }
    group.add(flowerHead);
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.07, 0.08, 1.3, 6), stemMat).translateY(0.75));
    for (let i = 0; i < 5; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.55, 0.14), leafMat);
      const a = (i / 5) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.25, 0.4 + i * 0.12, Math.sin(a) * 0.25);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    const flowerHead = new THREE.Group();
    flowerHead.position.y = 1.5;
    const center = new THREE.Mesh(new THREE.CylinderGeometry(0.2, 0.2, 0.12, 12), centerMat);
    center.rotation.x = Math.PI / 2; flowerHead.add(center);
    for (let i = 0; i < 14; i++) {
      const petal = new THREE.Mesh(new THREE.SphereGeometry(0.1, 6, 4), petalMat);
      const a = (i / 14) * Math.PI * 2;
      petal.position.set(Math.cos(a) * 0.28, 0, Math.sin(a) * 0.28);
      petal.scale.set(1.5, 0.3, 1); flowerHead.add(petal);
    }
    group.add(flowerHead);
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.5, 0.8, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

/* ---- Pepper ---- */
function createPepperCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x228B22, roughness: 0.8, side: THREE.DoubleSide });
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });
  const pepperMat = new THREE.MeshStandardMaterial({ color: 0xFF4500, roughness: 0.4, metalness: 0.1 });

  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.1, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.03, 0.04, 0.3, 6), stemMat).translateY(0.25));
    const lg = new THREE.PlaneGeometry(0.2, 0.06);
    const l1 = new THREE.Mesh(lg, leafMat); l1.position.set(0.08, 0.38, 0); l1.rotation.z = -0.4; group.add(l1);
    const l2 = new THREE.Mesh(lg.clone(), leafMat); l2.position.set(-0.08, 0.36, 0); l2.rotation.z = 0.4; group.add(l2);
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.05, 0.5, 6), stemMat).translateY(0.35));
    for (let i = 0; i < 4; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.3, 0.08), leafMat);
      const a = (i / 4) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.15, 0.3 + i * 0.08, Math.sin(a) * 0.15);
      leaf.rotation.y = a; leaf.rotation.z = -0.4; group.add(leaf);
    }
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.65, 6), stemMat).translateY(0.45));
    for (let i = 0; i < 5; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.35, 0.1), leafMat);
      const a = (i / 5) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.18, 0.35 + i * 0.08, Math.sin(a) * 0.18);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    for (let i = 0; i < 2; i++) {
      const pepper = new THREE.Mesh(new THREE.ConeGeometry(0.08, 0.3, 6), pepperMat);
      pepper.position.set(0.1 + i * 0.15, 0.35, (i - 0.5) * 0.1);
      pepper.rotation.z = (i - 0.5) * 0.3; group.add(pepper);
    }
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 0.8, 6), stemMat).translateY(0.55));
    for (let i = 0; i < 6; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.4, 0.12), leafMat);
      const a = (i / 6) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.22, 0.35 + i * 0.08, Math.sin(a) * 0.22);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    for (let i = 0; i < 4; i++) {
      const pepper = new THREE.Mesh(new THREE.ConeGeometry(0.1, 0.4, 6), pepperMat);
      const angle = (i / 4) * Math.PI * 2;
      pepper.position.set(Math.cos(angle) * 0.15, 0.3 + Math.random() * 0.15, Math.sin(angle) * 0.15);
      pepper.rotation.z = (Math.random() - 0.5) * 0.4; group.add(pepper);
    }
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.5, 0.8, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

/* ---- Blueberry ---- */
function createBlueberryCrop(stage: string): THREE.Group {
  const group = new THREE.Group();
  const leafMat = new THREE.MeshStandardMaterial({ color: 0x228B22, roughness: 0.8, side: THREE.DoubleSide });
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });
  const berryMat = new THREE.MeshStandardMaterial({ color: 0x4169E1, roughness: 0.3, metalness: 0.2 });

  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.1, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.03, 0.04, 0.25, 6), stemMat).translateY(0.22));
    const lg = new THREE.PlaneGeometry(0.18, 0.06);
    const l1 = new THREE.Mesh(lg, leafMat); l1.position.set(0.07, 0.35, 0); l1.rotation.z = -0.4; group.add(l1);
    const l2 = new THREE.Mesh(lg.clone(), leafMat); l2.position.set(-0.07, 0.33, 0); l2.rotation.z = 0.4; group.add(l2);
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.05, 0.45, 6), stemMat).translateY(0.3));
    for (let i = 0; i < 3; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.25, 0.08), leafMat);
      const a = (i / 3) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.12, 0.25 + i * 0.1, Math.sin(a) * 0.12);
      leaf.rotation.y = a; leaf.rotation.z = -0.4; group.add(leaf);
    }
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.6, 6), stemMat).translateY(0.4));
    for (let i = 0; i < 4; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.3, 0.1), leafMat);
      const a = (i / 4) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.15, 0.3 + i * 0.08, Math.sin(a) * 0.15);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    for (let i = 0; i < 3; i++) {
      const berry = new THREE.Mesh(new THREE.SphereGeometry(0.08, 8, 6), berryMat);
      const angle = (i / 3) * Math.PI * 2;
      berry.position.set(Math.cos(angle) * 0.12, 0.4 + i * 0.05, Math.sin(angle) * 0.12);
      group.add(berry);
    }
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 0.75, 6), stemMat).translateY(0.5));
    for (let i = 0; i < 5; i++) {
      const leaf = new THREE.Mesh(new THREE.PlaneGeometry(0.35, 0.12), leafMat);
      const a = (i / 5) * Math.PI * 2;
      leaf.position.set(Math.cos(a) * 0.18, 0.3 + i * 0.08, Math.sin(a) * 0.18);
      leaf.rotation.y = a; leaf.rotation.z = -0.5; group.add(leaf);
    }
    for (let i = 0; i < 7; i++) {
      const berry = new THREE.Mesh(new THREE.SphereGeometry(0.1, 8, 6), berryMat);
      const angle = (i / 7) * Math.PI * 2;
      berry.position.set(Math.cos(angle) * 0.15, 0.35 + (i % 3) * 0.08, Math.sin(angle) * 0.15);
      group.add(berry);
    }
    const glow = new THREE.Mesh(new THREE.RingGeometry(0.5, 0.8, 16), new THREE.MeshBasicMaterial({ color: 0xFFD700, transparent: true, opacity: 0.3, side: THREE.DoubleSide }));
    glow.rotation.x = -Math.PI / 2; glow.position.y = 0.16; group.add(glow);
  }
  makeCastShadow(group);
  return group;
}

const cropCreators: Record<string, (stage: string) => THREE.Group> = {
  Carrot: createCarrotCrop,
  Tomato: createTomatoCrop,
  Potato: createPotatoCrop,
  Wheat: createWheatCrop,
  Corn: createCornCrop,
  Strawberry: createStrawberryCrop,
  Pumpkin: createPumpkinCrop,
  Sunflower: createSunflowerCrop,
  Pepper: createPepperCrop,
  Blueberry: createBlueberryCrop,
};

function createCropModel(cropType: string, stage: string): THREE.Group {
  const creator = cropCreators[cropType];
  if (creator) return creator(stage);

  const group = new THREE.Group();
  const color = getStageColor(cropType, stage);
  const material = new THREE.MeshStandardMaterial({ color, roughness: 0.8 });
  if (stage === 'Seed') {
    const s = new THREE.Mesh(new THREE.SphereGeometry(0.12, 8, 6), new THREE.MeshStandardMaterial({ color: 0x8B4513 }));
    s.position.y = 0.2; group.add(s);
  } else if (stage === 'Sprout') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.04, 0.05, 0.35, 6), material).translateY(0.3));
  } else if (stage === 'Growing') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.55, 6), material).translateY(0.4));
  } else if (stage === 'Mature') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.06, 0.07, 0.75, 6), material).translateY(0.5));
  } else if (stage === 'Ready to Harvest') {
    group.add(new THREE.Mesh(new THREE.CylinderGeometry(0.07, 0.08, 0.95, 6), material).translateY(0.6));
  } else if (stage === 'Withered') {
    const s = new THREE.Mesh(new THREE.CylinderGeometry(0.05, 0.06, 0.4, 6), material);
    s.position.y = 0.25; s.rotation.z = 0.3; group.add(s);
  }
  makeCastShadow(group);
  return group;
}

export function createFarmPlot(x: number, y: number): FarmPlot3D {
  const group = new THREE.Group();
  const soilMesh = createSoilTile();
  group.add(soilMesh);
  const border = createSoilBorder();
  group.add(border);
  const worldX = (x - GRID_WIDTH / 2) * TILE_SIZE;
  const worldZ = (y - GRID_HEIGHT / 2) * TILE_SIZE;
  group.position.set(worldX, 0, worldZ);
  return {
    x, y, mesh: group, soilMesh, cropMesh: null,
    cropType: null, growthStage: '', growthPercent: 0,
    isWithered: false, isWatered: false, waterTimer: 0,
    leafMeshes: [], windOffset: Math.random() * Math.PI * 2, windSpeed: 0.5 + Math.random() * 0.5,
  };
}

export function updateCropVisual(plot: FarmPlot3D, cropType: string, stage: string, growthPercent: number, isWithered: boolean) {
  if (plot.cropMesh) {
    plot.mesh.remove(plot.cropMesh);
    plot.cropMesh.traverse((child) => {
      if (child instanceof THREE.Mesh) {
        child.geometry.dispose();
        if (Array.isArray(child.material)) child.material.forEach((m) => m.dispose());
        else child.material.dispose();
      }
    });
  }
  plot.leafMeshes = [];

  if (stage && stage !== '') {
    const cropMesh = createCropModel(cropType, stage);
    plot.mesh.add(cropMesh);
    plot.cropMesh = cropMesh;
    cropMesh.traverse((child) => {
      if (child instanceof THREE.Mesh) {
        child.castShadow = true;
        if (child.geometry.type === 'PlaneGeometry') plot.leafMeshes.push(child);
      }
    });
    if (!plot.mesh.children.find((c) => c instanceof THREE.Group && c.userData.isWaterDrops)) {
      const waterDrops = createWaterDrops();
      waterDrops.userData.isWaterDrops = true;
      waterDrops.visible = plot.isWatered;
      plot.mesh.add(waterDrops);
    }
  } else {
    plot.cropMesh = null;
  }

  plot.cropType = cropType;
  plot.growthStage = stage;
  plot.growthPercent = growthPercent;
  plot.isWithered = isWithered;
  const soilColor = isWithered ? 0x4a2e16 : (plot.isWatered ? 0x4a2e16 : 0x5c3a1e);
  (plot.soilMesh.material as THREE.MeshStandardMaterial).color.setHex(soilColor);
}

export function setPlotWatered(plot: FarmPlot3D, watered: boolean) {
  plot.isWatered = watered;
  plot.waterTimer = watered ? 30 : 0;
  const soilColor = watered ? 0x4a2e16 : (plot.isWithered ? 0x4a2e16 : 0x5c3a1e);
  (plot.soilMesh.material as THREE.MeshStandardMaterial).color.setHex(soilColor);
  plot.mesh.children.forEach((child) => {
    if (child instanceof THREE.Group && child.userData.isWaterDrops) child.visible = watered;
  });
}

export function updateWindAnimation(plot: FarmPlot3D, time: number, windStrength: number) {
  if (plot.leafMeshes.length === 0) return;
  plot.leafMeshes.forEach((leaf) => {
    const wind = Math.sin(time * plot.windSpeed + plot.windOffset) * windStrength * 0.1;
    leaf.rotation.z += wind * 0.02;
  });
}
