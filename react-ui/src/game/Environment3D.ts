import * as THREE from 'three';
import type { Environment3D } from './types3d';

const GRID_WIDTH = 10;
const GRID_HEIGHT = 10;
const TILE_SIZE = 2;

export function createEnvironment(): Environment3D {
  const farmWidth = GRID_WIDTH * TILE_SIZE;
  const farmHeight = GRID_HEIGHT * TILE_SIZE;

  const environment: Environment3D = {
    ground: createGround(),
    farmDirt: createFarmDirt(farmWidth, farmHeight),
    trees: [],
    buildings: [],
    fence: [],
    rocks: [],
    flowers: [],
    grassPatches: [],
    pathStones: [],
  };

  for (let i = 0; i < 12; i++) {
    const angle = (i / 12) * Math.PI * 2;
    const radius = 18 + Math.random() * 8;
    const treeType = Math.random() > 0.5 ? 'oak' : 'pine';
    const tree = createTree(treeType);
    tree.position.set(
      Math.cos(angle) * radius,
      0,
      Math.sin(angle) * radius
    );
    tree.rotation.y = Math.random() * Math.PI * 2;
    const scale = 0.7 + Math.random() * 0.6;
    tree.scale.set(scale, scale, scale);
    environment.trees.push(tree);
  }

  for (let i = 0; i < 6; i++) {
    const angle = (i / 6) * Math.PI * 2 + 0.3;
    const radius = 14 + Math.random() * 4;
    const bush = createBush();
    bush.position.set(
      Math.cos(angle) * radius,
      0,
      Math.sin(angle) * radius
    );
    const scale = 0.6 + Math.random() * 0.5;
    bush.scale.set(scale, scale, scale);
    environment.trees.push(bush);
  }

  const barn = createBarn();
  barn.position.set(-14, 0, -8);
  barn.rotation.y = Math.PI / 4;
  environment.buildings.push(barn);

  const well = createWell();
  well.position.set(14, 0, -6);
  environment.buildings.push(well);

  const fencePositions = [
    { x: -farmWidth / 2 - 2, z: 0, rotY: Math.PI / 2, count: Math.ceil(farmHeight / 2) + 2 },
    { x: farmWidth / 2 + 2, z: 0, rotY: Math.PI / 2, count: Math.ceil(farmHeight / 2) + 2 },
    { x: 0, z: -farmHeight / 2 - 2, rotY: 0, count: Math.ceil(farmWidth / 2) + 2 },
    { x: 0, z: farmHeight / 2 + 4, rotY: 0, count: Math.ceil(farmWidth / 2) + 2 },
  ];

  fencePositions.forEach(({ x, z, rotY, count }) => {
    for (let i = 0; i < count; i++) {
      const fencePost = createFencePost();
      if (rotY === 0) {
        fencePost.position.set(x + (i - count / 2) * 2, 0, z);
      } else {
        fencePost.position.set(x, 0, z + (i - count / 2) * 2);
      }
      fencePost.rotation.y = rotY;
      environment.fence.push(fencePost);
    }
  });

  for (let i = 0; i < 10; i++) {
    const rock = createRock();
    const angle = Math.random() * Math.PI * 2;
    const radius = 16 + Math.random() * 10;
    rock.position.set(Math.cos(angle) * radius, 0, Math.sin(angle) * radius);
    rock.rotation.y = Math.random() * Math.PI * 2;
    const scale = 0.4 + Math.random() * 0.8;
    rock.scale.set(scale, scale * (0.5 + Math.random() * 0.5), scale);
    environment.rocks.push(rock);
  }

  for (let i = 0; i < 20; i++) {
    const flower = createFlower();
    const angle = Math.random() * Math.PI * 2;
    const radius = 15 + Math.random() * 12;
    flower.position.set(Math.cos(angle) * radius, 0, Math.sin(angle) * radius);
    environment.flowers.push(flower);
  }

  for (let i = 0; i < 30; i++) {
    const grass = createGrassPatch();
    const angle = Math.random() * Math.PI * 2;
    const radius = 14 + Math.random() * 15;
    grass.position.set(Math.cos(angle) * radius, 0, Math.sin(angle) * radius);
    grass.rotation.y = Math.random() * Math.PI * 2;
    environment.grassPatches.push(grass);
  }

  for (let i = 0; i < 20; i++) {
    const flower = createFlower();
    const angle = Math.random() * Math.PI * 2;
    const radius = 15 + Math.random() * 12;
    flower.position.set(Math.cos(angle) * radius, 0, Math.sin(angle) * radius);
    environment.flowers.push(flower);
  }

  for (let i = 0; i < 30; i++) {
    const grass = createGrassPatch();
    const angle = Math.random() * Math.PI * 2;
    const radius = 14 + Math.random() * 15;
    grass.position.set(Math.cos(angle) * radius, 0, Math.sin(angle) * radius);
    grass.rotation.y = Math.random() * Math.PI * 2;
    environment.grassPatches.push(grass);
  }

  for (let z = -2; z <= 2; z++) {
    const pathStone = createPathStone();
    pathStone.position.set(-farmWidth / 2 - 4 + Math.abs(z) * 0.3, 0.01, z * 2.5);
    pathStone.rotation.y = Math.PI / 2;
    environment.pathStones.push(pathStone);
  }

  return environment;
}

function createGround(): THREE.Mesh {
  const groundGeo = new THREE.PlaneGeometry(80, 80, 20, 20);
  const positions = groundGeo.attributes.position;
  for (let i = 0; i < positions.count; i++) {
    const x = positions.getX(i);
    const y = positions.getY(i);
    const distFromCenter = Math.max(Math.abs(x), Math.abs(y));
    let z = 0;
    if (distFromCenter > 12) {
      z = Math.sin(x * 0.1) * 0.3 + Math.cos(y * 0.15) * 0.2;
    }
    positions.setZ(i, z);
  }
  groundGeo.computeVertexNormals();

  const groundMat = new THREE.MeshStandardMaterial({
    color: 0x4a7c3f,
    roughness: 0.95,
    metalness: 0.0,
  });
  const ground = new THREE.Mesh(groundGeo, groundMat);
  ground.rotation.x = -Math.PI / 2;
  ground.position.y = -0.01;
  ground.receiveShadow = true;
  return ground;
}

function createFarmDirt(farmWidth: number, farmHeight: number): THREE.Mesh {
  const padding = 1.5;
  const dirtGeo = new THREE.PlaneGeometry(farmWidth + padding * 2, farmHeight + padding * 2);
  const dirtMat = new THREE.MeshStandardMaterial({
    color: 0x5c3a1e,
    roughness: 0.95,
    metalness: 0.0,
  });
  const dirt = new THREE.Mesh(dirtGeo, dirtMat);
  dirt.rotation.x = -Math.PI / 2;
  dirt.position.y = 0.005;
  dirt.receiveShadow = true;
  return dirt;
}

function createTree(type: string): THREE.Group {
  const group = new THREE.Group();

  const trunkGeo = new THREE.CylinderGeometry(0.2, 0.3, 2, 8);
  const trunkMat = new THREE.MeshStandardMaterial({ color: 0x6b4226, roughness: 0.9 });
  const trunk = new THREE.Mesh(trunkGeo, trunkMat);
  trunk.position.y = 1;
  trunk.castShadow = true;
  group.add(trunk);

  if (type === 'oak') {
    const foliageColors = [0x2d5a1e, 0x3a7c2f, 0x4a9c3f];
    const foliageLayers = [
      { y: 2.5, radius: 1.8, height: 1.8, color: foliageColors[0] },
      { y: 3.5, radius: 1.4, height: 1.4, color: foliageColors[1] },
      { y: 4.2, radius: 1.0, height: 1.2, color: foliageColors[2] },
    ];

    foliageLayers.forEach(({ y, radius, height, color }) => {
      const foliageGeo = new THREE.SphereGeometry(radius, 8, 6);
      const foliageMat = new THREE.MeshStandardMaterial({ color, roughness: 0.85 });
      const foliage = new THREE.Mesh(foliageGeo, foliageMat);
      foliage.position.y = y;
      foliage.scale.y = height / (radius * 2);
      foliage.castShadow = true;
      group.add(foliage);
    });

    for (let i = 0; i < 3; i++) {
      const branchGeo = new THREE.CylinderGeometry(0.05, 0.1, 1.2, 6);
      const branch = new THREE.Mesh(branchGeo, trunkMat);
      const angle = (i / 3) * Math.PI * 2;
      branch.position.set(Math.cos(angle) * 0.5, 2.2 + i * 0.3, Math.sin(angle) * 0.5);
      branch.rotation.z = Math.cos(angle) * 0.8;
      branch.castShadow = true;
      group.add(branch);
    }
  } else {
    const foliageColors = [0x1a4a10, 0x2d5a1e, 0x3a6c2f];
    for (let i = 0; i < 4; i++) {
      const foliageGeo = new THREE.ConeGeometry(1.5 - i * 0.25, 1.2, 8);
      const foliageMat = new THREE.MeshStandardMaterial({ color: foliageColors[i % 3], roughness: 0.85 });
      const foliage = new THREE.Mesh(foliageGeo, foliageMat);
      foliage.position.y = 2.2 + i * 0.8;
      foliage.castShadow = true;
      group.add(foliage);
    }
  }

  return group;
}

function createBush(): THREE.Group {
  const group = new THREE.Group();
  const bushMat = new THREE.MeshStandardMaterial({ color: 0x3a7c2f, roughness: 0.85 });

  for (let i = 0; i < 4; i++) {
    const bushGeo = new THREE.SphereGeometry(0.4 + Math.random() * 0.3, 8, 6);
    const bush = new THREE.Mesh(bushGeo, bushMat);
    const angle = (i / 4) * Math.PI * 2;
    bush.position.set(Math.cos(angle) * 0.3, 0.3, Math.sin(angle) * 0.3);
    bush.castShadow = true;
    group.add(bush);
  }

  if (Math.random() > 0.5) {
    const berryMat = new THREE.MeshStandardMaterial({ color: 0xff1493, roughness: 0.4 });
    for (let i = 0; i < 3; i++) {
      const berryGeo = new THREE.SphereGeometry(0.06, 6, 4);
      const berry = new THREE.Mesh(berryGeo, berryMat);
      const angle = (i / 3) * Math.PI * 2;
      berry.position.set(Math.cos(angle) * 0.35, 0.5, Math.sin(angle) * 0.35);
      group.add(berry);
    }
  }

  return group;
}

function createBarn(): THREE.Group {
  const group = new THREE.Group();

  const baseGeo = new THREE.BoxGeometry(4, 3, 3);
  const baseMat = new THREE.MeshStandardMaterial({ color: 0x8b4513, roughness: 0.85 });
  const base = new THREE.Mesh(baseGeo, baseMat);
  base.position.y = 1.5;
  base.castShadow = true;
  group.add(base);

  const roofGeo = new THREE.ConeGeometry(3.2, 2, 4);
  const roofMat = new THREE.MeshStandardMaterial({ color: 0x654321, roughness: 0.9 });
  const roof = new THREE.Mesh(roofGeo, roofMat);
  roof.position.y = 4;
  roof.rotation.y = Math.PI / 4;
  roof.castShadow = true;
  group.add(roof);

  const doorGeo = new THREE.BoxGeometry(1, 2, 0.1);
  const doorMat = new THREE.MeshStandardMaterial({ color: 0x4a2a10, roughness: 0.9 });
  const door = new THREE.Mesh(doorGeo, doorMat);
  door.position.set(0, 1, 1.55);
  group.add(door);

  const windowMat = new THREE.MeshStandardMaterial({ color: 0x87ceeb, roughness: 0.2, metalness: 0.3 });
  for (let side = -1; side <= 1; side += 2) {
    const windowGeo = new THREE.BoxGeometry(0.5, 0.5, 0.1);
    const window = new THREE.Mesh(windowGeo, windowMat);
    window.position.set(side * 1.2, 2, 1.55);
    group.add(window);
  }

  return group;
}

function createWell(): THREE.Group {
  const group = new THREE.Group();

  const baseGeo = new THREE.CylinderGeometry(0.8, 0.9, 1, 12);
  const baseMat = new THREE.MeshStandardMaterial({ color: 0x808080, roughness: 0.85 });
  const base = new THREE.Mesh(baseGeo, baseMat);
  base.position.y = 0.5;
  base.castShadow = true;
  group.add(base);

  const waterGeo = new THREE.CylinderGeometry(0.6, 0.6, 0.1, 12);
  const waterMat = new THREE.MeshStandardMaterial({
    color: 0x4a90d9,
    transparent: true,
    opacity: 0.7,
    roughness: 0.1,
    metalness: 0.3,
  });
  const water = new THREE.Mesh(waterGeo, waterMat);
  water.position.y = 0.8;
  group.add(water);

  const postGeo = new THREE.CylinderGeometry(0.05, 0.05, 2, 6);
  const postMat = new THREE.MeshStandardMaterial({ color: 0x6b4226, roughness: 0.9 });
  const leftPost = new THREE.Mesh(postGeo, postMat);
  leftPost.position.set(-0.6, 1.5, 0);
  leftPost.castShadow = true;
  group.add(leftPost);

  const rightPost = new THREE.Mesh(postGeo, postMat);
  rightPost.position.set(0.6, 1.5, 0);
  rightPost.castShadow = true;
  group.add(rightPost);

  const beamGeo = new THREE.BoxGeometry(1.4, 0.1, 0.1);
  const beam = new THREE.Mesh(beamGeo, postMat);
  beam.position.y = 2.5;
  beam.castShadow = true;
  group.add(beam);

  const roofGeo = new THREE.ConeGeometry(0.8, 0.5, 4);
  const roofMat = new THREE.MeshStandardMaterial({ color: 0x654321, roughness: 0.9 });
  const roof = new THREE.Mesh(roofGeo, roofMat);
  roof.position.y = 2.8;
  roof.rotation.y = Math.PI / 4;
  roof.castShadow = true;
  group.add(roof);

  return group;
}

function createFencePost(): THREE.Group {
  const group = new THREE.Group();

  const postGeo = new THREE.BoxGeometry(0.15, 0.8, 0.15);
  const postMat = new THREE.MeshStandardMaterial({ color: 0x8b5e3c, roughness: 0.9 });
  const post = new THREE.Mesh(postGeo, postMat);
  post.position.y = 0.4;
  post.castShadow = true;
  group.add(post);

  const railGeo = new THREE.BoxGeometry(2, 0.08, 0.08);
  const rail1 = new THREE.Mesh(railGeo, postMat);
  rail1.position.y = 0.6;
  group.add(rail1);

  const rail2 = new THREE.Mesh(railGeo, postMat);
  rail2.position.y = 0.3;
  group.add(rail2);

  return group;
}

function createRock(): THREE.Group {
  const group = new THREE.Group();
  const rockMat = new THREE.MeshStandardMaterial({
    color: 0x808080,
    roughness: 0.95,
    metalness: 0.05,
  });

  const rockGeo = new THREE.DodecahedronGeometry(0.5, 1);
  const positions = rockGeo.attributes.position;
  for (let i = 0; i < positions.count; i++) {
    const x = positions.getX(i);
    const y = positions.getY(i);
    const z = positions.getZ(i);
    const noise = 0.8 + Math.random() * 0.4;
    positions.setXYZ(i, x * noise, y * noise * 0.6, z * noise);
  }
  rockGeo.computeVertexNormals();

  const rock = new THREE.Mesh(rockGeo, rockMat);
  rock.castShadow = true;
  rock.receiveShadow = true;
  group.add(rock);

  if (Math.random() > 0.6) {
    const mossGeo = new THREE.SphereGeometry(0.2, 6, 4);
    const mossMat = new THREE.MeshStandardMaterial({ color: 0x4a7c3f, roughness: 0.9 });
    const moss = new THREE.Mesh(mossGeo, mossMat);
    moss.position.set(0.2, 0.15, 0.1);
    moss.scale.set(1, 0.5, 1);
    group.add(moss);
  }

  return group;
}

function createFlower(): THREE.Group {
  const group = new THREE.Group();
  const stemMat = new THREE.MeshStandardMaterial({ color: 0x2d5a1e, roughness: 0.9 });

  const stemGeo = new THREE.CylinderGeometry(0.02, 0.02, 0.4, 4);
  const stem = new THREE.Mesh(stemGeo, stemMat);
  stem.position.y = 0.2;
  group.add(stem);

  const flowerColors = [0xff6b6b, 0xffd93d, 0x6bcb77, 0x4d96ff, 0xff69b4, 0xffa500];
  const color = flowerColors[Math.floor(Math.random() * flowerColors.length)];
  const petalMat = new THREE.MeshStandardMaterial({ color, roughness: 0.6, side: THREE.DoubleSide });

  for (let i = 0; i < 5; i++) {
    const petalGeo = new THREE.SphereGeometry(0.06, 6, 4);
    const petal = new THREE.Mesh(petalGeo, petalMat);
    const angle = (i / 5) * Math.PI * 2;
    petal.position.set(Math.cos(angle) * 0.08, 0.42, Math.sin(angle) * 0.08);
    petal.scale.set(1, 0.3, 1);
    group.add(petal);
  }

  const centerGeo = new THREE.SphereGeometry(0.04, 6, 4);
  const centerMat = new THREE.MeshStandardMaterial({ color: 0xffd700, roughness: 0.5 });
  const center = new THREE.Mesh(centerGeo, centerMat);
  center.position.y = 0.42;
  group.add(center);

  return group;
}

function createGrassPatch(): THREE.Group {
  const group = new THREE.Group();
  const grassMat = new THREE.MeshStandardMaterial({
    color: 0x4a9c3f,
    roughness: 0.9,
    side: THREE.DoubleSide,
  });

  const bladeCount = 3 + Math.floor(Math.random() * 4);
  for (let i = 0; i < bladeCount; i++) {
    const bladeGeo = new THREE.PlaneGeometry(0.05, 0.3 + Math.random() * 0.2);
    const blade = new THREE.Mesh(bladeGeo, grassMat);
    blade.position.set(
      (Math.random() - 0.5) * 0.3,
      0.15,
      (Math.random() - 0.5) * 0.3
    );
    blade.rotation.z = (Math.random() - 0.5) * 0.3;
    group.add(blade);
  }

  return group;
}

function createPathStone(): THREE.Mesh {
  const geo = new THREE.BoxGeometry(1.5, 0.05, 0.8);
  const mat = new THREE.MeshStandardMaterial({
    color: 0x9a8a7a,
    roughness: 0.95,
    metalness: 0.0,
  });
  const mesh = new THREE.Mesh(geo, mat);
  mesh.receiveShadow = true;
  return mesh;
}
