import * as THREE from 'three';

export interface Pet3D {
  group: THREE.Group;
  type: 'dog' | 'cat';
  targetPosition: THREE.Vector3;
  isMoving: boolean;
  animationTime: number;
  followDistance: number;
  isSitting: boolean;
  sitTimer: number;
  tailWagTimer: number;
  happinessLevel: number;
}

export function createDog(position: THREE.Vector3): Pet3D {
  const group = new THREE.Group();

  const bodyGeo = new THREE.BoxGeometry(0.4, 0.35, 0.6);
  const bodyMat = new THREE.MeshStandardMaterial({ color: 0xC8956C, roughness: 0.85 });
  const body = new THREE.Mesh(bodyGeo, bodyMat);
  body.position.y = 0.4;
  body.castShadow = true;
  group.add(body);

  const chestGeo = new THREE.BoxGeometry(0.35, 0.3, 0.25);
  const chestMat = new THREE.MeshStandardMaterial({ color: 0xF5DEB3, roughness: 0.85 });
  const chest = new THREE.Mesh(chestGeo, chestMat);
  chest.position.set(0, 0.38, 0.25);
  group.add(chest);

  const headGeo = new THREE.BoxGeometry(0.3, 0.3, 0.3);
  const headMat = new THREE.MeshStandardMaterial({ color: 0xC8956C, roughness: 0.85 });
  const head = new THREE.Mesh(headGeo, headMat);
  head.position.set(0, 0.6, 0.35);
  head.castShadow = true;
  head.name = 'head';
  group.add(head);

  const snoutGeo = new THREE.BoxGeometry(0.15, 0.12, 0.15);
  const snoutMat = new THREE.MeshStandardMaterial({ color: 0xF5DEB3, roughness: 0.8 });
  const snout = new THREE.Mesh(snoutGeo, snoutMat);
  snout.position.set(0, 0.55, 0.5);
  group.add(snout);

  const noseGeo = new THREE.SphereGeometry(0.04, 6, 4);
  const noseMat = new THREE.MeshStandardMaterial({ color: 0x222222, roughness: 0.5 });
  const nose = new THREE.Mesh(noseGeo, noseMat);
  nose.position.set(0, 0.56, 0.58);
  group.add(nose);

  const eyeGeo = new THREE.SphereGeometry(0.04, 6, 4);
  const eyeMat = new THREE.MeshStandardMaterial({ color: 0x3d2510, roughness: 0.3 });
  for (let side = -1; side <= 1; side += 2) {
    const eye = new THREE.Mesh(eyeGeo, eyeMat);
    eye.position.set(side * 0.08, 0.65, 0.48);
    group.add(eye);
  }

  const earGeo = new THREE.BoxGeometry(0.1, 0.18, 0.05);
  const earMat = new THREE.MeshStandardMaterial({ color: 0x8B6914, roughness: 0.9 });
  for (let side = -1; side <= 1; side += 2) {
    const ear = new THREE.Mesh(earGeo, earMat);
    ear.position.set(side * 0.15, 0.72, 0.3);
    ear.rotation.z = side * 0.3;
    ear.name = side === -1 ? 'leftEar' : 'rightEar';
    group.add(ear);
  }

  const legGeo = new THREE.CylinderGeometry(0.05, 0.045, 0.25, 6);
  const legMat = new THREE.MeshStandardMaterial({ color: 0xC8956C, roughness: 0.85 });
  for (let x = -1; x <= 1; x += 2) {
    for (let z = -1; z <= 1; z += 2) {
      const leg = new THREE.Mesh(legGeo, legMat);
      leg.position.set(x * 0.12, 0.12, z * 0.2);
      leg.castShadow = true;
      leg.name = `leg_${x > 0 ? 'r' : 'l'}_${z > 0 ? 'f' : 'b'}`;
      group.add(leg);
    }
  }

  const tailGroup = new THREE.Group();
  const tailGeo = new THREE.CylinderGeometry(0.03, 0.02, 0.25, 6);
  const tail = new THREE.Mesh(tailGeo, bodyMat);
  tail.position.y = 0.12;
  tailGroup.add(tail);
  const tailTipGeo = new THREE.SphereGeometry(0.04, 6, 4);
  const tailTipMat = new THREE.MeshStandardMaterial({ color: 0xF5DEB3, roughness: 0.8 });
  const tailTip = new THREE.Mesh(tailTipGeo, tailTipMat);
  tailTip.position.y = 0.25;
  tailGroup.add(tailTip);
  tailGroup.position.set(0, 0.5, -0.3);
  tailGroup.rotation.x = -0.8;
  tailGroup.name = 'tail';
  group.add(tailGroup);

  group.position.copy(position);

  return {
    group,
    type: 'dog',
    targetPosition: position.clone(),
    isMoving: false,
    animationTime: 0,
    followDistance: 2.5,
    isSitting: false,
    sitTimer: 0,
    tailWagTimer: 0,
    happinessLevel: 0.7,
  };
}

export function createCat(position: THREE.Vector3): Pet3D {
  const group = new THREE.Group();

  const bodyGeo = new THREE.BoxGeometry(0.3, 0.25, 0.5);
  const bodyMat = new THREE.MeshStandardMaterial({ color: 0x808080, roughness: 0.8 });
  const body = new THREE.Mesh(bodyGeo, bodyMat);
  body.position.y = 0.35;
  body.castShadow = true;
  group.add(body);

  const stripeMat = new THREE.MeshStandardMaterial({ color: 0x505050, roughness: 0.8 });
  for (let i = 0; i < 3; i++) {
    const stripeGeo = new THREE.BoxGeometry(0.32, 0.03, 0.05);
    const stripe = new THREE.Mesh(stripeGeo, stripeMat);
    stripe.position.set(0, 0.4 + i * 0.05, -0.1 + i * 0.1);
    group.add(stripe);
  }

  const headGeo = new THREE.SphereGeometry(0.18, 8, 6);
  const headMat = new THREE.MeshStandardMaterial({ color: 0x808080, roughness: 0.8 });
  const head = new THREE.Mesh(headGeo, headMat);
  head.position.set(0, 0.5, 0.3);
  head.scale.set(1, 0.9, 0.9);
  head.castShadow = true;
  head.name = 'head';
  group.add(head);

  const earGeo = new THREE.ConeGeometry(0.06, 0.12, 4);
  const earMat = new THREE.MeshStandardMaterial({ color: 0x808080, roughness: 0.8 });
  const earInnerMat = new THREE.MeshStandardMaterial({ color: 0xFFB6C1, roughness: 0.7 });
  for (let side = -1; side <= 1; side += 2) {
    const ear = new THREE.Mesh(earGeo, earMat);
    ear.position.set(side * 0.1, 0.65, 0.3);
    ear.rotation.z = side * 0.2;
    group.add(ear);

    const innerEar = new THREE.Mesh(new THREE.ConeGeometry(0.03, 0.08, 4), earInnerMat);
    innerEar.position.set(side * 0.1, 0.64, 0.31);
    innerEar.rotation.z = side * 0.2;
    group.add(innerEar);
  }

  const eyeGeo = new THREE.SphereGeometry(0.04, 6, 4);
  const eyeMat = new THREE.MeshStandardMaterial({ color: 0x44CC44, emissive: 0x226622, emissiveIntensity: 0.3, roughness: 0.3 });
  for (let side = -1; side <= 1; side += 2) {
    const eye = new THREE.Mesh(eyeGeo, eyeMat);
    eye.position.set(side * 0.07, 0.52, 0.42);
    eye.scale.set(1, 1.3, 0.5);
    group.add(eye);
  }

  const noseGeo = new THREE.SphereGeometry(0.025, 6, 4);
  const noseMat = new THREE.MeshStandardMaterial({ color: 0xFFB6C1, roughness: 0.5 });
  const nose = new THREE.Mesh(noseGeo, noseMat);
  nose.position.set(0, 0.47, 0.47);
  group.add(nose);

  const whiskerMat = new THREE.MeshStandardMaterial({ color: 0xDDDDDD, roughness: 0.6 });
  for (let side = -1; side <= 1; side += 2) {
    for (let i = 0; i < 3; i++) {
      const whiskerGeo = new THREE.CylinderGeometry(0.003, 0.003, 0.15, 4);
      const whisker = new THREE.Mesh(whiskerGeo, whiskerMat);
      whisker.position.set(side * (0.1 + i * 0.02), 0.46 - i * 0.01, 0.45);
      whisker.rotation.z = Math.PI / 2 + side * (0.1 + i * 0.1);
      group.add(whisker);
    }
  }

  const legGeo = new THREE.CylinderGeometry(0.035, 0.03, 0.2, 6);
  const legMat = new THREE.MeshStandardMaterial({ color: 0x808080, roughness: 0.8 });
  for (let x = -1; x <= 1; x += 2) {
    for (let z = -1; z <= 1; z += 2) {
      const leg = new THREE.Mesh(legGeo, legMat);
      leg.position.set(x * 0.08, 0.1, z * 0.15);
      leg.castShadow = true;
      leg.name = `leg_${x > 0 ? 'r' : 'l'}_${z > 0 ? 'f' : 'b'}`;
      group.add(leg);
    }
  }

  const tailGroup = new THREE.Group();
  const tailSegments = 5;
  for (let i = 0; i < tailSegments; i++) {
    const segRadius = 0.035 - i * 0.004;
    const segGeo = new THREE.SphereGeometry(segRadius, 6, 4);
    const seg = new THREE.Mesh(segGeo, bodyMat);
    seg.position.y = i * 0.06;
    tailGroup.add(seg);
  }
  tailGroup.position.set(0, 0.4, -0.25);
  tailGroup.rotation.x = -0.5;
  tailGroup.name = 'tail';
  group.add(tailGroup);

  group.position.copy(position);

  return {
    group,
    type: 'cat',
    targetPosition: position.clone(),
    isMoving: false,
    animationTime: 0,
    followDistance: 3.5,
    isSitting: false,
    sitTimer: 0,
    tailWagTimer: 0,
    happinessLevel: 0.5,
  };
}

export function updatePet(pet: Pet3D, playerPosition: THREE.Vector3, delta: number) {
  pet.animationTime += delta;
  pet.tailWagTimer += delta;

  const distanceToPlayer = pet.group.position.distanceTo(playerPosition);

  if (distanceToPlayer > pet.followDistance + 1) {
    pet.isSitting = false;
    const direction = playerPosition.clone().sub(pet.group.position);
    direction.y = 0;
    direction.normalize();
    pet.targetPosition.copy(playerPosition).sub(direction.multiplyScalar(pet.followDistance));
    pet.isMoving = true;
  } else if (pet.isSitting) {
    pet.isMoving = false;
    pet.sitTimer -= delta;
    if (pet.sitTimer <= 0) {
      pet.isSitting = false;
    }
  } else if (Math.random() < 0.005 && distanceToPlayer < pet.followDistance) {
    pet.isSitting = true;
    pet.sitTimer = 3 + Math.random() * 5;
    pet.isMoving = false;
  }

  if (pet.isMoving) {
    const speed = pet.type === 'dog' ? 3 : 3.5;
    const direction = pet.targetPosition.clone().sub(pet.group.position);
    direction.y = 0;

    if (direction.length() > 0.1) {
      const angle = Math.atan2(direction.x, direction.z);
      pet.group.rotation.y = angle;

      const moveDir = direction.normalize();
      pet.group.position.add(moveDir.multiplyScalar(speed * delta));

      const legNames = ['leg_r_f', 'leg_l_f', 'leg_r_b', 'leg_l_b'];
      const legSpeed = pet.type === 'dog' ? 8 : 10;
      legNames.forEach((name, i) => {
        const leg = pet.group.getObjectByName(name) as THREE.Mesh;
        if (leg) {
          const phase = i % 2 === 0 ? 0 : Math.PI;
          leg.rotation.x = Math.sin(pet.animationTime * legSpeed + phase) * 0.4;
        }
      });

      const head = pet.group.getObjectByName('head') as THREE.Mesh;
      if (head) {
        head.position.y += Math.sin(pet.animationTime * 4) * 0.001;
      }
    } else {
      pet.isMoving = false;
    }
  } else {
    const legNames = ['leg_r_f', 'leg_l_f', 'leg_r_b', 'leg_l_b'];
    legNames.forEach(name => {
      const leg = pet.group.getObjectByName(name) as THREE.Mesh;
      if (leg) leg.rotation.x = 0;
    });
  }

  const tail = pet.group.getObjectByName('tail') as THREE.Group;
  if (tail) {
    if (pet.type === 'dog') {
      const wagSpeed = pet.isMoving ? 12 : (pet.isSitting ? 3 : 6);
      const wagAmount = pet.isMoving ? 0.5 : (pet.isSitting ? 0.15 : 0.3);
      tail.rotation.y = Math.sin(pet.tailWagTimer * wagSpeed) * wagAmount;
    } else {
      const swaySpeed = pet.isMoving ? 4 : 2;
      const swayAmount = pet.isMoving ? 0.3 : 0.15;
      tail.rotation.z = Math.sin(pet.tailWagTimer * swaySpeed) * swayAmount;
      tail.rotation.x = -0.5 + Math.sin(pet.tailWagTimer * 1.5) * 0.1;
    }
  }

  if (pet.isSitting) {
    pet.group.position.y = THREE.MathUtils.lerp(pet.group.position.y, -0.05, delta * 3);
  } else {
    pet.group.position.y = THREE.MathUtils.lerp(pet.group.position.y, 0, delta * 3);
  }
}

export function petAnimal(pet: Pet3D): void {
  pet.happinessLevel = Math.min(1, pet.happinessLevel + 0.1);
  pet.isSitting = false;
  pet.sitTimer = 0;

  const tail = pet.group.getObjectByName('tail') as THREE.Group;
  if (tail) {
    tail.rotation.y = 0;
    const happyJump = () => {
      pet.group.position.y += 0.1;
      setTimeout(() => {
        pet.group.position.y = Math.max(0, pet.group.position.y - 0.1);
      }, 200);
    };
    happyJump();
  }
}
