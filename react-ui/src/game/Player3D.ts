import * as THREE from 'three';
import type { Player3D } from './types3d';

export function createPlayer(): Player3D {
  const group = new THREE.Group();

  const bodyGeo = new THREE.BoxGeometry(0.5, 0.7, 0.35);
  const bodyMat = new THREE.MeshStandardMaterial({ color: 0x4a90d9, roughness: 0.7 });
  const body = new THREE.Mesh(bodyGeo, bodyMat);
  body.position.y = 0.75;
  body.castShadow = true;
  group.add(body);

  const headGeo = new THREE.BoxGeometry(0.4, 0.4, 0.4);
  const headMat = new THREE.MeshStandardMaterial({ color: 0xffcc99, roughness: 0.8 });
  const head = new THREE.Mesh(headGeo, headMat);
  head.position.y = 1.3;
  head.castShadow = true;
  group.add(head);

  const hatGeo = new THREE.BoxGeometry(0.5, 0.15, 0.5);
  const hatMat = new THREE.MeshStandardMaterial({ color: 0x8b5e3c, roughness: 0.9 });
  const hat = new THREE.Mesh(hatGeo, hatMat);
  hat.position.y = 1.55;
  hat.castShadow = true;
  group.add(hat);

  const hatBrimGeo = new THREE.BoxGeometry(0.6, 0.05, 0.6);
  const hatBrim = new THREE.Mesh(hatBrimGeo, hatMat);
  hatBrim.position.y = 1.48;
  hatBrim.castShadow = true;
  group.add(hatBrim);

  const leftArmGeo = new THREE.BoxGeometry(0.15, 0.5, 0.15);
  const armMat = new THREE.MeshStandardMaterial({ color: 0x4a90d9, roughness: 0.7 });
  const leftArm = new THREE.Mesh(leftArmGeo, armMat);
  leftArm.position.set(-0.35, 0.7, 0);
  leftArm.castShadow = true;
  leftArm.name = 'leftArm';
  group.add(leftArm);

  const rightArmGeo = new THREE.BoxGeometry(0.15, 0.5, 0.15);
  const rightArm = new THREE.Mesh(rightArmGeo, armMat);
  rightArm.position.set(0.35, 0.7, 0);
  rightArm.castShadow = true;
  rightArm.name = 'rightArm';
  group.add(rightArm);

  const leftLegGeo = new THREE.BoxGeometry(0.18, 0.4, 0.18);
  const legMat = new THREE.MeshStandardMaterial({ color: 0x5c3a1e, roughness: 0.9 });
  const leftLeg = new THREE.Mesh(leftLegGeo, legMat);
  leftLeg.position.set(-0.12, 0.2, 0);
  leftLeg.castShadow = true;
  leftLeg.name = 'leftLeg';
  group.add(leftLeg);

  const rightLegGeo = new THREE.BoxGeometry(0.18, 0.4, 0.18);
  const rightLeg = new THREE.Mesh(rightLegGeo, legMat);
  rightLeg.position.set(0.12, 0.2, 0);
  rightLeg.castShadow = true;
  rightLeg.name = 'rightLeg';
  group.add(rightLeg);

  group.position.set(0, 0, 12);

  return {
    group,
    targetPosition: new THREE.Vector3(0, 0, 12),
    isMoving: false,
    animationTime: 0,
    currentAction: 'idle',
  };
}

export function updatePlayerAnimation(player: Player3D, delta: number) {
  player.animationTime += delta;

  const leftArm = player.group.getObjectByName('leftArm') as THREE.Mesh;
  const rightArm = player.group.getObjectByName('rightArm') as THREE.Mesh;
  const leftLeg = player.group.getObjectByName('leftLeg') as THREE.Mesh;
  const rightLeg = player.group.getObjectByName('rightLeg') as THREE.Mesh;

  if (player.currentAction === 'walking' && player.isMoving) {
    const swingAmount = Math.sin(player.animationTime * 8) * 0.5;
    if (leftArm) leftArm.rotation.x = swingAmount;
    if (rightArm) rightArm.rotation.x = -swingAmount;
    if (leftLeg) leftLeg.rotation.x = -swingAmount * 0.7;
    if (rightLeg) rightLeg.rotation.x = swingAmount * 0.7;

    const moveDir = player.targetPosition.clone().sub(player.group.position);
    if (moveDir.length() > 0.1) {
      const angle = Math.atan2(moveDir.x, moveDir.z);
      player.group.rotation.y = angle;
    }
  } else if (player.currentAction === 'planting' || player.currentAction === 'harvesting' || player.currentAction === 'watering' || player.currentAction === 'clearing') {
    const actionAmount = Math.sin(player.animationTime * 6) * 0.8;
    if (rightArm) rightArm.rotation.x = -actionAmount;
    if (leftArm) leftArm.rotation.x = actionAmount * 0.3;
  } else {
    const breathe = Math.sin(player.animationTime * 2) * 0.02;
    if (leftArm) leftArm.rotation.x = breathe;
    if (rightArm) rightArm.rotation.x = -breathe;
    if (leftLeg) leftLeg.rotation.x = 0;
    if (rightLeg) rightLeg.rotation.x = 0;
  }

  const distance = player.group.position.distanceTo(player.targetPosition);
  if (distance > 0.05) {
    const speed = 5;
    const direction = player.targetPosition.clone().sub(player.group.position).normalize();
    player.group.position.add(direction.multiplyScalar(speed * delta));
    player.isMoving = true;
    player.currentAction = 'walking';
  } else {
    player.group.position.copy(player.targetPosition);
    player.isMoving = false;
    if (player.currentAction === 'walking') {
      player.currentAction = 'idle';
    }
  }
}

export function movePlayerTo(player: Player3D, x: number, z: number) {
  player.targetPosition.set(x, 0, z);
  player.isMoving = true;
}
