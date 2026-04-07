import * as THREE from 'three';
import type { WeatherSystem, DayNightCycle } from './types3d';

export function createDayNightCycle(): DayNightCycle {
  const sunLight = new THREE.DirectionalLight(0xfff4e0, 1.2);
  sunLight.position.set(10, 20, 10);
  sunLight.castShadow = true;
  sunLight.shadow.mapSize.width = 2048;
  sunLight.shadow.mapSize.height = 2048;
  sunLight.shadow.camera.near = 0.5;
  sunLight.shadow.camera.far = 80;
  sunLight.shadow.camera.left = -30;
  sunLight.shadow.camera.right = 30;
  sunLight.shadow.camera.top = 30;
  sunLight.shadow.camera.bottom = -30;
  sunLight.shadow.bias = -0.001;
  sunLight.shadow.normalBias = 0.02;

  const moonLight = new THREE.DirectionalLight(0x4a6fa5, 0.15);
  moonLight.position.set(-10, 15, -10);
  moonLight.castShadow = false;

  const ambientLight = new THREE.AmbientLight(0x404060, 0.4);

  const hemisphereLight = new THREE.HemisphereLight(0x87ceeb, 0x4a7c3f, 0.3);

  const sunGeo = new THREE.SphereGeometry(1.5, 16, 16);
  const sunMat = new THREE.MeshBasicMaterial({ color: 0xffd700 });
  const sunMesh = new THREE.Mesh(sunGeo, sunMat);

  const moonGeo = new THREE.SphereGeometry(1.0, 16, 16);
  const moonMat = new THREE.MeshBasicMaterial({ color: 0xe0e0e0 });
  const moonMesh = new THREE.Mesh(moonGeo, moonMat);

  return {
    sunLight,
    ambientLight,
    hemisphereLight,
    timeOfDay: 0.3,
    cycleSpeed: 0.0003,
    isDay: true,
    moonLight,
    sunMesh,
    moonMesh,
  };
}

export function updateDayNightCycle(cycle: DayNightCycle, delta: number) {
  cycle.timeOfDay += cycle.cycleSpeed * delta;
  if (cycle.timeOfDay > 1) cycle.timeOfDay -= 1;

  const sunAngle = cycle.timeOfDay * Math.PI * 2 - Math.PI / 2;
  const sunHeight = Math.sin(sunAngle);
  const sunX = Math.cos(sunAngle) * 20;
  const sunY = Math.max(sunHeight * 25, -5);
  const sunZ = 10;

  cycle.sunLight.position.set(sunX, sunY, sunZ);

  const moonAngle = sunAngle + Math.PI;
  const moonX = Math.cos(moonAngle) * 20;
  const moonY = Math.max(Math.sin(moonAngle) * 25, -5);
  const moonZ = -10;

  cycle.moonLight.position.set(moonX, moonY, moonZ);

  const dayFactor = Math.max(0, Math.min(1, (sunHeight + 0.2) / 0.6));

  cycle.sunLight.intensity = dayFactor * 1.5;
  cycle.moonLight.intensity = (1 - dayFactor) * 0.3;
  cycle.ambientLight.intensity = 0.15 + dayFactor * 0.35;
  cycle.hemisphereLight.intensity = 0.1 + dayFactor * 0.3;

  const dayColor = new THREE.Color(0xfff4e0);
  const sunsetColor = new THREE.Color(0xff8c42);
  const nightColor = new THREE.Color(0x4a6fa5);

  if (dayFactor > 0.6) {
    cycle.sunLight.color.copy(dayColor);
  } else if (dayFactor > 0.2) {
    cycle.sunLight.color.copy(sunsetColor).lerp(dayColor, (dayFactor - 0.2) / 0.4);
  } else {
    cycle.sunLight.color.copy(nightColor);
  }

  const dayAmbient = new THREE.Color(0x6688aa);
  const nightAmbient = new THREE.Color(0x1a1a3a);
  cycle.ambientLight.color.copy(nightAmbient).lerp(dayAmbient, dayFactor);

  cycle.isDay = dayFactor > 0.3;

  if (sunY > 0) {
    cycle.sunMesh.position.set(sunX, sunY, sunZ);
    cycle.sunMesh.visible = true;
  } else {
    cycle.sunMesh.visible = false;
  }

  if (moonY > 0) {
    cycle.moonMesh.position.set(moonX, moonY, moonZ);
    cycle.moonMesh.visible = true;
  } else {
    cycle.moonMesh.visible = false;
  }

  return dayFactor;
}

function createCloud(): THREE.Group {
  const group = new THREE.Group();
  const cloudMat = new THREE.MeshStandardMaterial({
    color: 0xffffff,
    roughness: 1.0,
    metalness: 0.0,
    transparent: true,
    opacity: 0.85,
  });

  const puffCount = 4 + Math.floor(Math.random() * 4);
  for (let i = 0; i < puffCount; i++) {
    const puffGeo = new THREE.SphereGeometry(1 + Math.random() * 1.5, 8, 6);
    const puff = new THREE.Mesh(puffGeo, cloudMat);
    puff.position.set(
      (Math.random() - 0.5) * 4,
      (Math.random() - 0.5) * 0.5,
      (Math.random() - 0.5) * 2
    );
    puff.scale.y = 0.5 + Math.random() * 0.3;
    group.add(puff);
  }

  group.userData.speed = 0.5 + Math.random() * 1.0;
  group.userData.startX = -40 - Math.random() * 20;

  return group;
}

export function createWeatherSystem(): WeatherSystem {
  const rainCount = 2000;
  const rainGeometry = new THREE.BufferGeometry();
  const rainPositions = new Float32Array(rainCount * 3);
  const rainVelocities = new Float32Array(rainCount);

  for (let i = 0; i < rainCount; i++) {
    rainPositions[i * 3] = (Math.random() - 0.5) * 60;
    rainPositions[i * 3 + 1] = Math.random() * 30;
    rainPositions[i * 3 + 2] = (Math.random() - 0.5) * 60;
    rainVelocities[i] = 0.5 + Math.random() * 0.5;
  }

  rainGeometry.setAttribute('position', new THREE.BufferAttribute(rainPositions, 3));

  const rainMaterial = new THREE.PointsMaterial({
    color: 0xaaaaaa,
    size: 0.1,
    transparent: true,
    opacity: 0.6,
  });

  const rainParticles = new THREE.Points(rainGeometry, rainMaterial);
  rainParticles.visible = false;

  const clouds: THREE.Group[] = [];
  for (let i = 0; i < 8; i++) {
    const cloud = createCloud();
    cloud.position.set(
      cloud.userData.startX + Math.random() * 80,
      15 + Math.random() * 10,
      (Math.random() - 0.5) * 40
    );
    clouds.push(cloud);
  }

  return {
    rainParticles,
    rainVelocity: 0,
    isRaining: false,
    intensity: 0,
    clouds,
    cloudSpeed: 0.01,
  };
}

export function updateWeatherSystem(weather: WeatherSystem, delta: number) {
  if (weather.rainParticles && weather.isRaining) {
    const positions = weather.rainParticles.geometry.attributes.position.array as Float32Array;

    for (let i = 0; i < positions.length / 3; i++) {
      positions[i * 3 + 1] -= weather.rainVelocity * delta * 30;

      if (positions[i * 3 + 1] < 0) {
        positions[i * 3 + 1] = 25 + Math.random() * 5;
        positions[i * 3] = (Math.random() - 0.5) * 60;
        positions[i * 3 + 2] = (Math.random() - 0.5) * 60;
      }
    }

    weather.rainParticles.geometry.attributes.position.needsUpdate = true;
  }

  weather.clouds.forEach((cloud) => {
    cloud.position.x += cloud.userData.speed * delta * 2;
    if (cloud.position.x > 50) {
      cloud.position.x = cloud.userData.startX;
    }
  });
}

export function setRain(weather: WeatherSystem, isRaining: boolean, intensity: number = 0.5) {
  weather.isRaining = isRaining;
  weather.intensity = intensity;
  weather.rainVelocity = intensity;

  if (weather.rainParticles) {
    weather.rainParticles.visible = isRaining;
    (weather.rainParticles.material as THREE.PointsMaterial).opacity = intensity * 0.6;
  }
}
