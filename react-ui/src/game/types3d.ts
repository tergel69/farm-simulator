import * as THREE from 'three';

export interface FarmPlot3D {
  x: number;
  y: number;
  mesh: THREE.Group;
  soilMesh: THREE.Mesh;
  cropMesh: THREE.Group | null;
  cropType: string | null;
  growthStage: string;
  growthPercent: number;
  isWithered: boolean;
  isWatered: boolean;
  waterTimer: number;
  leafMeshes: THREE.Mesh[];
  windOffset: number;
  windSpeed: number;
}

export interface Player3D {
  group: THREE.Group;
  targetPosition: THREE.Vector3;
  isMoving: boolean;
  animationTime: number;
  currentAction: 'idle' | 'walking' | 'planting' | 'harvesting' | 'watering' | 'clearing';
}

export interface Environment3D {
  ground: THREE.Mesh;
  farmDirt: THREE.Mesh;
  trees: THREE.Group[];
  buildings: THREE.Group[];
  fence: THREE.Group[];
  rocks: THREE.Group[];
  flowers: THREE.Group[];
  grassPatches: THREE.Group[];
  pathStones: THREE.Mesh[];
}

export interface WeatherSystem {
  rainParticles: THREE.Points | null;
  rainVelocity: number;
  isRaining: boolean;
  intensity: number;
  clouds: THREE.Group[];
  cloudSpeed: number;
}

export interface DayNightCycle {
  sunLight: THREE.DirectionalLight;
  ambientLight: THREE.AmbientLight;
  hemisphereLight: THREE.HemisphereLight;
  timeOfDay: number;
  cycleSpeed: number;
  isDay: boolean;
  moonLight: THREE.DirectionalLight;
  sunMesh: THREE.Mesh;
  moonMesh: THREE.Mesh;
}

export interface SeasonSettings {
  name: 'spring' | 'summer' | 'fall' | 'winter';
  groundColor: number;
  skyTopColor: number;
  skyBottomColor: number;
  fogColor: number;
  ambientIntensity: number;
  treeColor: number;
  hasSnow: boolean;
}

export interface SeasonSettings {
  name: 'spring' | 'summer' | 'fall' | 'winter';
  groundColor: number;
  skyTopColor: number;
  skyBottomColor: number;
  fogColor: number;
  ambientIntensity: number;
  treeColor: number;
  hasSnow: boolean;
}
