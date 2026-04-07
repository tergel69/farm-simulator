import * as THREE from 'three';

export interface FloatingText {
  mesh: THREE.Group;
  life: number;
  maxLife: number;
  velocity: THREE.Vector3;
}

export interface ParticleEffect {
  points: THREE.Points;
  life: number;
  maxLife: number;
}

export class EffectManager {
  floatingTexts: FloatingText[] = [];
  particleEffects: ParticleEffect[] = [];
  fireflies: THREE.Points | null = null;
  fireflyMaterial: THREE.PointsMaterial | null = null;

  createFloatingText(text: string, position: THREE.Vector3, color: number = 0xffd700): FloatingText {
    const canvas = document.createElement('canvas');
    const context = canvas.getContext('2d')!;
    canvas.width = 256;
    canvas.height = 64;

    context.fillStyle = 'transparent';
    context.fillRect(0, 0, canvas.width, canvas.height);

    context.font = 'bold 28px Arial';
    context.textAlign = 'center';
    context.textBaseline = 'middle';

    context.strokeStyle = '#000';
    context.lineWidth = 4;
    context.strokeText(text, canvas.width / 2, canvas.height / 2);

    context.fillStyle = '#' + color.toString(16).padStart(6, '0');
    context.fillText(text, canvas.width / 2, canvas.height / 2);

    const texture = new THREE.CanvasTexture(canvas);
    const material = new THREE.SpriteMaterial({ map: texture, transparent: true, opacity: 1 });
    const sprite = new THREE.Sprite(material);
    sprite.scale.set(2, 0.5, 1);
    sprite.position.copy(position);
    sprite.position.y += 1.5;

    const ft: FloatingText = {
      mesh: new THREE.Group().add(sprite) as any,
      life: 0,
      maxLife: 2,
      velocity: new THREE.Vector3(0, 1.5, 0),
    };

    const group = new THREE.Group();
    group.add(sprite);
    group.position.copy(position);
    ft.mesh = group;
    ft.velocity = new THREE.Vector3(0, 1.5, 0);

    this.floatingTexts.push(ft);
    return ft;
  }

  createHarvestParticles(position: THREE.Vector3): ParticleEffect {
    const count = 20;
    const geometry = new THREE.BufferGeometry();
    const positions = new Float32Array(count * 3);
    const velocities = new Float32Array(count * 3);
    const colors = new Float32Array(count * 3);

    const goldColor = new THREE.Color(0xffd700);
    const greenColor = new THREE.Color(0x5d9e50);

    for (let i = 0; i < count; i++) {
      positions[i * 3] = position.x;
      positions[i * 3 + 1] = position.y + 0.5;
      positions[i * 3 + 2] = position.z;

      velocities[i * 3] = (Math.random() - 0.5) * 3;
      velocities[i * 3 + 1] = Math.random() * 3 + 1;
      velocities[i * 3 + 2] = (Math.random() - 0.5) * 3;

      const c = Math.random() > 0.5 ? goldColor : greenColor;
      colors[i * 3] = c.r;
      colors[i * 3 + 1] = c.g;
      colors[i * 3 + 2] = c.b;
    }

    geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
    geometry.setAttribute('color', new THREE.BufferAttribute(colors, 3));

    const material = new THREE.PointsMaterial({
      size: 0.15,
      vertexColors: true,
      transparent: true,
      opacity: 1,
    });

    const points = new THREE.Points(geometry, material);
    points.userData.velocities = velocities;

    const effect: ParticleEffect = {
      points,
      life: 0,
      maxLife: 1.5,
    };

    this.particleEffects.push(effect);
    return effect;
  }

  createPlantParticles(position: THREE.Vector3): ParticleEffect {
    const count = 10;
    const geometry = new THREE.BufferGeometry();
    const positions = new Float32Array(count * 3);
    const velocities = new Float32Array(count * 3);
    const colors = new Float32Array(count * 3);

    const brownColor = new THREE.Color(0x5c3a1e);
    const greenColor = new THREE.Color(0x32CD32);

    for (let i = 0; i < count; i++) {
      positions[i * 3] = position.x + (Math.random() - 0.5) * 0.5;
      positions[i * 3 + 1] = position.y + Math.random() * 0.3;
      positions[i * 3 + 2] = position.z + (Math.random() - 0.5) * 0.5;

      velocities[i * 3] = (Math.random() - 0.5) * 1;
      velocities[i * 3 + 1] = Math.random() * 1.5;
      velocities[i * 3 + 2] = (Math.random() - 0.5) * 1;

      const c = Math.random() > 0.5 ? brownColor : greenColor;
      colors[i * 3] = c.r;
      colors[i * 3 + 1] = c.g;
      colors[i * 3 + 2] = c.b;
    }

    geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
    geometry.setAttribute('color', new THREE.BufferAttribute(colors, 3));

    const material = new THREE.PointsMaterial({
      size: 0.1,
      vertexColors: true,
      transparent: true,
      opacity: 1,
    });

    const points = new THREE.Points(geometry, material);
    points.userData.velocities = velocities;

    const effect: ParticleEffect = {
      points,
      life: 0,
      maxLife: 1,
    };

    this.particleEffects.push(effect);
    return effect;
  }

  createWaterParticles(position: THREE.Vector3): ParticleEffect {
    const count = 15;
    const geometry = new THREE.BufferGeometry();
    const positions = new Float32Array(count * 3);
    const velocities = new Float32Array(count * 3);
    const colors = new Float32Array(count * 3);

    const blueColor = new THREE.Color(0x4a90d9);

    for (let i = 0; i < count; i++) {
      positions[i * 3] = position.x + (Math.random() - 0.5) * 0.8;
      positions[i * 3 + 1] = position.y + 1 + Math.random() * 0.5;
      positions[i * 3 + 2] = position.z + (Math.random() - 0.5) * 0.8;

      velocities[i * 3] = (Math.random() - 0.5) * 0.5;
      velocities[i * 3 + 1] = -Math.random() * 2 - 1;
      velocities[i * 3 + 2] = (Math.random() - 0.5) * 0.5;

      colors[i * 3] = blueColor.r;
      colors[i * 3 + 1] = blueColor.g;
      colors[i * 3 + 2] = blueColor.b;
    }

    geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
    geometry.setAttribute('color', new THREE.BufferAttribute(colors, 3));

    const material = new THREE.PointsMaterial({
      size: 0.12,
      vertexColors: true,
      transparent: true,
      opacity: 0.8,
    });

    const points = new THREE.Points(geometry, material);
    points.userData.velocities = velocities;

    const effect: ParticleEffect = {
      points,
      life: 0,
      maxLife: 0.8,
    };

    this.particleEffects.push(effect);
    return effect;
  }

  createFireflies(scene: THREE.Scene) {
    if (this.fireflies) return;

    const count = 40;
    const geometry = new THREE.BufferGeometry();
    const positions = new Float32Array(count * 3);
    const phases = new Float32Array(count);

    for (let i = 0; i < count; i++) {
      positions[i * 3] = (Math.random() - 0.5) * 40;
      positions[i * 3 + 1] = 1 + Math.random() * 5;
      positions[i * 3 + 2] = (Math.random() - 0.5) * 40;
      phases[i] = Math.random() * Math.PI * 2;
    }

    geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
    geometry.setAttribute('phase', new THREE.BufferAttribute(phases, 1));

    this.fireflyMaterial = new THREE.PointsMaterial({
      color: 0xffff88,
      size: 0.2,
      transparent: true,
      opacity: 0,
      blending: THREE.AdditiveBlending,
    });

    this.fireflies = new THREE.Points(geometry, this.fireflyMaterial);
    this.fireflies.userData.phases = phases;
    scene.add(this.fireflies);
  }

  updateFireflies(delta: number, isNight: boolean) {
    if (!this.fireflies || !this.fireflyMaterial) return;

    const targetOpacity = isNight ? 0.8 : 0;
    this.fireflyMaterial.opacity += (targetOpacity - this.fireflyMaterial.opacity) * delta * 2;

    if (this.fireflyMaterial.opacity < 0.01) return;

    const positions = this.fireflies.geometry.attributes.position.array as Float32Array;
    const phases = this.fireflies.userData.phases as Float32Array;

    for (let i = 0; i < positions.length / 3; i++) {
      const phase = phases[i];
      positions[i * 3] += Math.sin(Date.now() * 0.001 + phase) * delta * 0.5;
      positions[i * 3 + 1] += Math.cos(Date.now() * 0.0008 + phase * 1.3) * delta * 0.3;
      positions[i * 3 + 2] += Math.sin(Date.now() * 0.0012 + phase * 0.7) * delta * 0.5;
    }

    this.fireflies.geometry.attributes.position.needsUpdate = true;
    this.fireflyMaterial.size = 0.15 + Math.sin(Date.now() * 0.002) * 0.05;
  }

  update(delta: number, scene: THREE.Scene) {
    for (let i = this.floatingTexts.length - 1; i >= 0; i--) {
      const ft = this.floatingTexts[i];
      ft.life += delta;

      if (ft.life >= ft.maxLife) {
        scene.remove(ft.mesh);
        ft.mesh.traverse((child) => {
          if (child instanceof THREE.Mesh || child instanceof THREE.Sprite) {
            if (child.material) {
              if (Array.isArray(child.material)) child.material.forEach(m => m.dispose());
              else child.material.dispose();
            }
          }
        });
        this.floatingTexts.splice(i, 1);
        continue;
      }

      ft.mesh.position.add(ft.velocity.clone().multiplyScalar(delta));
      ft.velocity.multiplyScalar(0.95);

      const progress = ft.life / ft.maxLife;
      const opacity = progress < 0.7 ? 1 : 1 - (progress - 0.7) / 0.3;
      ft.mesh.traverse((child) => {
        if (child instanceof THREE.Sprite && child.material) {
          (child.material as THREE.SpriteMaterial).opacity = opacity;
        }
      });
    }

    for (let i = this.particleEffects.length - 1; i >= 0; i--) {
      const effect = this.particleEffects[i];
      effect.life += delta;

      if (effect.life >= effect.maxLife) {
        scene.remove(effect.points);
        effect.points.geometry.dispose();
        (effect.points.material as THREE.Material).dispose();
        this.particleEffects.splice(i, 1);
        continue;
      }

      const positions = effect.points.geometry.attributes.position.array as Float32Array;
      const velocities = effect.points.userData.velocities as Float32Array;

      for (let j = 0; j < positions.length / 3; j++) {
        velocities[j * 3 + 1] -= 9.8 * delta * 0.3;
        positions[j * 3] += velocities[j * 3] * delta;
        positions[j * 3 + 1] += velocities[j * 3 + 1] * delta;
        positions[j * 3 + 2] += velocities[j * 3 + 2] * delta;

        if (positions[j * 3 + 1] < 0) {
          positions[j * 3 + 1] = 0;
          velocities[j * 3 + 1] *= -0.3;
        }
      }

      effect.points.geometry.attributes.position.needsUpdate = true;
      const progress = effect.life / effect.maxLife;
      (effect.points.material as THREE.PointsMaterial).opacity = 1 - progress;
    }
  }

  dispose(scene: THREE.Scene) {
    this.floatingTexts.forEach(ft => {
      scene.remove(ft.mesh);
    });
    this.floatingTexts = [];

    this.particleEffects.forEach(effect => {
      scene.remove(effect.points);
      effect.points.geometry.dispose();
      (effect.points.material as THREE.Material).dispose();
    });
    this.particleEffects = [];

    if (this.fireflies) {
      scene.remove(this.fireflies);
      this.fireflies.geometry.dispose();
      if (this.fireflyMaterial) this.fireflyMaterial.dispose();
      this.fireflies = null;
      this.fireflyMaterial = null;
    }
  }
}
