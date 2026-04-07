import * as THREE from 'three';

export interface Animal3D {
  group: THREE.Group;
  type: 'chicken' | 'cow';
  targetPosition: THREE.Vector3;
  isMoving: boolean;
  animationTime: number;
  wanderTimer: number;
  productTimer: number;
  hasProduct: boolean;
}

export function createChicken(position: THREE.Vector3): Animal3D {
  const group = new THREE.Group();

  const bodyGeo = new THREE.SphereGeometry(0.3, 8, 6);
  const bodyMat = new THREE.MeshStandardMaterial({ color: 0xFFFFFF, roughness: 0.8 });
  const body = new THREE.Mesh(bodyGeo, bodyMat);
  body.position.y = 0.35;
  body.scale.set(1, 0.8, 1.2);
  body.castShadow = true;
  group.add(body);

  const headGeo = new THREE.SphereGeometry(0.15, 8, 6);
  const headMat = new THREE.MeshStandardMaterial({ color: 0xFFFFFF, roughness: 0.8 });
  const head = new THREE.Mesh(headGeo, headMat);
  head.position.set(0, 0.55, 0.2);
  head.castShadow = true;
  head.name = 'head';
  group.add(head);

  const combGeo = new THREE.ConeGeometry(0.06, 0.12, 4);
  const combMat = new THREE.MeshStandardMaterial({ color: 0xFF0000, roughness: 0.6 });
  const comb = new THREE.Mesh(combGeo, combMat);
  comb.position.set(0, 0.7, 0.2);
  group.add(comb);

  const beakGeo = new THREE.ConeGeometry(0.04, 0.1, 4);
  const beakMat = new THREE.MeshStandardMaterial({ color: 0xFFA500, roughness: 0.6 });
  const beak = new THREE.Mesh(beakGeo, beakMat);
  beak.position.set(0, 0.52, 0.35);
  beak.rotation.x = Math.PI / 2;
  group.add(beak);

  const tailGeo = new THREE.ConeGeometry(0.1, 0.2, 4);
  const tailMat = new THREE.MeshStandardMaterial({ color: 0xF5F5DC, roughness: 0.8 });
  const tail = new THREE.Mesh(tailGeo, tailMat);
  tail.position.set(0, 0.45, -0.25);
  tail.rotation.x = -0.5;
  group.add(tail);

  const legGeo = new THREE.CylinderGeometry(0.02, 0.02, 0.2, 4);
  const legMat = new THREE.MeshStandardMaterial({ color: 0xFFA500, roughness: 0.7 });
  for (let side = -1; side <= 1; side += 2) {
    const leg = new THREE.Mesh(legGeo, legMat);
    leg.position.set(side * 0.08, 0.1, 0);
    leg.castShadow = true;
    leg.name = side === -1 ? 'leftLeg' : 'rightLeg';
    group.add(leg);
  }

  group.position.copy(position);

  return {
    group,
    type: 'chicken',
    targetPosition: position.clone(),
    isMoving: false,
    animationTime: 0,
    wanderTimer: Math.random() * 5,
    productTimer: 0,
    hasProduct: false,
  };
}

export function createCow(position: THREE.Vector3): Animal3D {
  const group = new THREE.Group();

  const bodyGeo = new THREE.BoxGeometry(0.7, 0.5, 1.0);
  const bodyMat = new THREE.MeshStandardMaterial({ color: 0x8B4513, roughness: 0.85 });
  const body = new THREE.Mesh(bodyGeo, bodyMat);
  body.position.y = 0.6;
  body.castShadow = true;
  group.add(body);

  const spotGeo = new THREE.SphereGeometry(0.15, 6, 4);
  const spotMat = new THREE.MeshStandardMaterial({ color: 0xFFFFFF, roughness: 0.85 });
  const spot1 = new THREE.Mesh(spotGeo, spotMat);
  spot1.position.set(0.2, 0.7, 0.1);
  spot1.scale.set(1, 0.5, 1.5);
  group.add(spot1);
  const spot2 = new THREE.Mesh(spotGeo.clone(), spotMat);
  spot2.position.set(-0.15, 0.65, -0.2);
  spot2.scale.set(1.2, 0.5, 1);
  group.add(spot2);

  const headGeo = new THREE.BoxGeometry(0.35, 0.35, 0.35);
  const headMat = new THREE.MeshStandardMaterial({ color: 0x8B4513, roughness: 0.85 });
  const head = new THREE.Mesh(headGeo, headMat);
  head.position.set(0, 0.75, 0.6);
  head.castShadow = true;
  head.name = 'head';
  group.add(head);

  const snoutGeo = new THREE.BoxGeometry(0.25, 0.15, 0.15);
  const snoutMat = new THREE.MeshStandardMaterial({ color: 0xFFB6C1, roughness: 0.7 });
  const snout = new THREE.Mesh(snoutGeo, snoutMat);
  snout.position.set(0, 0.65, 0.78);
  group.add(snout);

  const hornGeo = new THREE.ConeGeometry(0.04, 0.15, 6);
  const hornMat = new THREE.MeshStandardMaterial({ color: 0xF5F5DC, roughness: 0.6 });
  for (let side = -1; side <= 1; side += 2) {
    const horn = new THREE.Mesh(hornGeo, hornMat);
    horn.position.set(side * 0.15, 0.95, 0.55);
    horn.rotation.z = side * 0.3;
    group.add(horn);
  }

  const udderGeo = new THREE.SphereGeometry(0.12, 6, 4);
  const udderMat = new THREE.MeshStandardMaterial({ color: 0xFFB6C1, roughness: 0.7 });
  const udder = new THREE.Mesh(udderGeo, udderMat);
  udder.position.set(0, 0.35, -0.1);
  udder.scale.set(1, 0.6, 1.2);
  group.add(udder);

  const legGeo = new THREE.CylinderGeometry(0.06, 0.05, 0.4, 6);
  const legMat = new THREE.MeshStandardMaterial({ color: 0x654321, roughness: 0.9 });
  for (let x = -1; x <= 1; x += 2) {
    for (let z = -1; z <= 1; z += 2) {
      const leg = new THREE.Mesh(legGeo, legMat);
      leg.position.set(x * 0.2, 0.2, z * 0.35);
      leg.castShadow = true;
      leg.name = `leg_${x}_${z}`;
      group.add(leg);
    }
  }

  const tailGroup = new THREE.Group();
  const tailStemGeo = new THREE.CylinderGeometry(0.02, 0.015, 0.4, 4);
  const tailStem = new THREE.Mesh(tailStemGeo, bodyMat);
  tailStem.position.y = 0.2;
  tailGroup.add(tailStem);
  const tailTipGeo = new THREE.SphereGeometry(0.04, 6, 4);
  const tailTipMat = new THREE.MeshStandardMaterial({ color: 0x3d2510, roughness: 0.9 });
  const tailTip = new THREE.Mesh(tailTipGeo, tailTipMat);
  tailTip.position.y = 0.4;
  tailGroup.add(tailTip);
  tailGroup.position.set(0, 0.7, -0.5);
  tailGroup.rotation.x = -0.5;
  tailGroup.name = 'tail';
  group.add(tailGroup);

  group.position.copy(position);

  return {
    group,
    type: 'cow',
    targetPosition: position.clone(),
    isMoving: false,
    animationTime: 0,
    wanderTimer: Math.random() * 8,
    productTimer: 0,
    hasProduct: false,
  };
}

export function updateAnimal(animal: Animal3D, delta: number) {
  animal.animationTime += delta;
  animal.wanderTimer -= delta;

  if (animal.wanderTimer <= 0 && !animal.isMoving) {
    const wanderRadius = animal.type === 'chicken' ? 3 : 5;
    const angle = Math.random() * Math.PI * 2;
    animal.targetPosition.set(
      animal.group.position.x + Math.cos(angle) * wanderRadius * Math.random(),
      0,
      animal.group.position.z + Math.sin(angle) * wanderRadius * Math.random()
    );
    animal.isMoving = true;
    animal.wanderTimer = 3 + Math.random() * 5;
  }

  animal.productTimer += delta;
  if (animal.productTimer > 60 && !animal.hasProduct) {
    animal.hasProduct = true;
  }

  const distance = animal.group.position.distanceTo(animal.targetPosition);
  if (distance > 0.1 && animal.isMoving) {
    const speed = animal.type === 'chicken' ? 1.5 : 0.8;
    const direction = animal.targetPosition.clone().sub(animal.group.position).normalize();
    animal.group.position.add(direction.multiplyScalar(speed * delta));

    const angle = Math.atan2(direction.x, direction.z);
    animal.group.rotation.y = angle;

    const leftLeg = animal.group.getObjectByName('leftLeg') as THREE.Mesh;
    const rightLeg = animal.group.getObjectByName('rightLeg') as THREE.Mesh;
    if (leftLeg || rightLeg) {
      const swing = Math.sin(animal.animationTime * 6) * 0.3;
      if (leftLeg) leftLeg.rotation.x = swing;
      if (rightLeg) rightLeg.rotation.x = -swing;
    }

    const head = animal.group.getObjectByName('head') as THREE.Mesh;
    if (head) {
      head.position.y += Math.sin(animal.animationTime * 3) * 0.002;
    }
  } else {
    animal.isMoving = false;
    const tail = animal.group.getObjectByName('tail') as THREE.Group;
    if (tail) {
      tail.rotation.y = Math.sin(animal.animationTime * 2) * 0.3;
    }
  }

  if (animal.hasProduct) {
    if (!animal.group.getObjectByName('productIndicator')) {
      const indicatorGeo = new THREE.OctahedronGeometry(0.12, 0);
      const indicatorMat = new THREE.MeshStandardMaterial({
        color: animal.type === 'chicken' ? 0xFFF8DC : 0x87CEEB,
        emissive: animal.type === 'chicken' ? 0xFFF8DC : 0x87CEEB,
        emissiveIntensity: 0.5,
        transparent: true,
        opacity: 0.8,
      });
      const indicator = new THREE.Mesh(indicatorGeo, indicatorMat);
      indicator.position.y = animal.type === 'chicken' ? 0.8 : 1.2;
      indicator.name = 'productIndicator';
      indicator.userData.isProductIndicator = true;
      animal.group.add(indicator);
    }
    const indicator = animal.group.getObjectByName('productIndicator') as THREE.Mesh;
    if (indicator) {
      indicator.rotation.y = animal.animationTime * 2;
      indicator.position.y += Math.sin(animal.animationTime * 3) * 0.001;
    }
  }
}

export function collectProduct(animal: Animal3D): string | null {
  if (!animal.hasProduct) return null;

  const indicator = animal.group.getObjectByName('productIndicator');
  if (indicator) {
    animal.group.remove(indicator);
    if (indicator instanceof THREE.Mesh) {
      indicator.geometry.dispose();
      (indicator.material as THREE.Material).dispose();
    }
  }

  animal.hasProduct = false;
  animal.productTimer = 0;
  return animal.type === 'chicken' ? 'Egg' : 'Milk';
}
