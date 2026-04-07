import * as THREE from 'three';
import { EffectComposer, RenderPass } from 'postprocessing';
import { PixelationEffect, EffectPass } from 'postprocessing';
import { createFarmPlot, updateCropVisual, setPlotWatered, updateWindAnimation } from './FarmPlot3D';
import { createPlayer, updatePlayerAnimation, movePlayerTo } from './Player3D';
import { createEnvironment } from './Environment3D';
import { CameraController, type CameraMode } from './CameraController';
import { createDayNightCycle, updateDayNightCycle, createWeatherSystem, updateWeatherSystem, setRain } from './WeatherAndDay';
import { EffectManager } from './EffectManager';
import { createChicken, createCow, updateAnimal, collectProduct, type Animal3D } from './Animals3D';
import { createPond, createFishingRod, type FishingRod3D, type FishType } from './FishingSystem';
import { createDog, createCat,  type Pet3D } from './Pet3D';
import type { FarmPlot3D, Player3D, Environment3D, WeatherSystem, DayNightCycle, SeasonSettings } from './types3d';
import type { FarmState } from '../bridge/types';

export type PixelStylePreset = 'subtle' | 'balanced' | 'heavy';

export interface VisualSettings {
  cameraMode: CameraMode;
  pixelStyle: PixelStylePreset;
}

const GRID_WIDTH = 10;
const GRID_HEIGHT = 10;
const TILE_SIZE = 2;

export class GameScene {
  renderer: THREE.WebGLRenderer;
  scene: THREE.Scene;
  cameraController: CameraController;
  plots: FarmPlot3D[][];
  player: Player3D;
  environment: Environment3D;
  dayNightCycle: DayNightCycle;
  weatherSystem: WeatherSystem;
  clock: THREE.Clock;
  isInitialized: boolean;
  onPlotClick: ((x: number, y: number, tool?: string) => void) | null;
  raycaster: THREE.Raycaster;
  mouse: THREE.Vector2;
  hoveredPlot: FarmPlot3D | null;
  highlightMesh: THREE.Mesh;
  skybox: THREE.Mesh;
  selectedTool: string;
  windStrength: number;
  effectManager: EffectManager;
  lastDayNightState: boolean;
  animals: Animal3D[];
  currentSeason: SeasonSettings;
  seasonDay: number;
  onCollectProduct: ((productName: string) => void) | null;
  onFishCaught: ((fish: FishType) => void) | null;
  pond: THREE.Group | null;
  fishingRod: FishingRod3D;
  pets: Pet3D[];
  
  // Post-processing
  composer: EffectComposer | null = null;
  renderPass: RenderPass | null = null;
  pixelationPass: PixelationPass | null = null;
  resolutionScale: number = 1.0;
  currentPixelStyle: PixelStylePreset = 'balanced';

  constructor(container: HTMLElement) {
    this.scene = new THREE.Scene();
    this.scene.fog = new THREE.Fog(0x87ceeb, 30, 60);

    this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: false });
    this.renderer.setSize(window.innerWidth, window.innerHeight);
    this.renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
    this.renderer.shadowMap.enabled = true;
    this.renderer.shadowMap.type = THREE.PCFSoftShadowMap;
    this.renderer.toneMapping = THREE.ACESFilmicToneMapping;
    this.renderer.toneMappingExposure = 1.0;
    container.appendChild(this.renderer.domElement);

    // Set scene background color to match the sky
    this.scene.background = new THREE.Color(0x87ceeb);

    this.cameraController = new CameraController();
    this.scene.add(this.cameraController.camera);

    this.plots = [];
    this.player = createPlayer();
    this.scene.add(this.player.group);

    this.environment = createEnvironment();
    this.scene.add(this.environment.ground);
    this.scene.add(this.environment.farmDirt);
    this.environment.trees.forEach((tree) => this.scene.add(tree));
    this.environment.buildings.forEach((building) => this.scene.add(building));
    this.environment.fence.forEach((fence) => this.scene.add(fence));
    this.environment.rocks.forEach((rock) => this.scene.add(rock));
    this.environment.flowers.forEach((flower) => this.scene.add(flower));
    this.environment.grassPatches.forEach((grass) => this.scene.add(grass));
    this.environment.pathStones.forEach((stone) => this.scene.add(stone));

    this.dayNightCycle = createDayNightCycle();
    this.scene.add(this.dayNightCycle.sunLight);
    this.scene.add(this.dayNightCycle.ambientLight);
    this.scene.add(this.dayNightCycle.hemisphereLight);
    this.scene.add(this.dayNightCycle.moonLight);
    this.scene.add(this.dayNightCycle.sunMesh);
    this.scene.add(this.dayNightCycle.moonMesh);

    this.weatherSystem = createWeatherSystem();
    if (this.weatherSystem.rainParticles) {
      this.scene.add(this.weatherSystem.rainParticles);
    }
    this.weatherSystem.clouds.forEach((cloud) => this.scene.add(cloud));

    this.clock = new THREE.Clock();
    this.isInitialized = false;
    this.onPlotClick = null;
    this.raycaster = new THREE.Raycaster();
    this.mouse = new THREE.Vector2();
    this.hoveredPlot = null;
    this.selectedTool = 'plant';
    this.windStrength = 0.3;
    this.effectManager = new EffectManager();
    this.lastDayNightState = true;
    this.animals = [];
    this.seasonDay = 0;
    this.currentSeason = this.getSeasonForDay(0);
    this.onCollectProduct = null;
    this.onFishCaught = null;
    this.pond = null;
    this.fishingRod = createFishingRod();
    this.pets = [];

    this.initializeAnimals();
    this.pond = createPond();
    this.scene.add(this.pond);
    this.initializePets();
    this.scene.add(this.fishingRod.group);

    const highlightGeo = new THREE.BoxGeometry(TILE_SIZE * 0.95, 0.2, TILE_SIZE * 0.95);
    const highlightMat = new THREE.MeshBasicMaterial({
      color: 0xffff00,
      transparent: true,
      opacity: 0.3,
    });
    this.highlightMesh = new THREE.Mesh(highlightGeo, highlightMat);
    this.highlightMesh.visible = false;
    this.scene.add(this.highlightMesh);

    this.skybox = this.createSkybox();
    this.scene.add(this.skybox);

    this.setupInputHandlers();
    this.isInitialized = true;
  }

  setupPostProcessing(pixelStyle: PixelStylePreset) {
    if (this.composer) {
      this.composer.dispose();
      this.composer = null;
    }
    const width = window.innerWidth;
    const height = window.innerHeight;
    const scales: Record<PixelStylePreset, number> = { subtle: 0.75, balanced: 0.5, heavy: 0.25 };
    this.resolutionScale = scales[pixelStyle];
    this.composer = new EffectComposer(this.renderer, { multisampling: 0 });
    this.renderPass = new RenderPass(this.scene, this.cameraController.camera);
    this.composer.addPass(this.renderPass);
    this.pixelationPass = new PixelationPass({ granularity: 16, resolution: new THREE.Vector2(Math.floor(width * this.resolutionScale), Math.floor(height * this.resolutionScale)) });
    this.composer.addPass(this.pixelationPass);
    this.currentPixelStyle = pixelStyle;
  }

  applyVisualSettings(settings: VisualSettings) {
    this.cameraController.setCameraMode(settings.cameraMode);
    if (settings.pixelStyle !== this.currentPixelStyle) {
      this.setupPostProcessing(settings.pixelStyle);
    }
  }

  createSkybox(): THREE.Mesh {
    const skyGeo = new THREE.SphereGeometry(80, 32, 32);
    const skyMat = new THREE.ShaderMaterial({
      uniforms: {
        topColor: { value: new THREE.Color(0x0077ff) },
        bottomColor: { value: new THREE.Color(0xffffff) },
        offset: { value: 20 },
        exponent: { value: 0.6 },
      },
      vertexShader: `
        varying vec3 vWorldPosition;
        void main() {
          vec4 worldPosition = modelMatrix * vec4(position, 1.0);
          vWorldPosition = worldPosition.xyz;
          gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
        }
      `,
      fragmentShader: `
        uniform vec3 topColor;
        uniform vec3 bottomColor;
        uniform float offset;
        uniform float exponent;
        varying vec3 vWorldPosition;
        void main() {
          float h = normalize(vWorldPosition + offset).y;
          gl_FragColor = vec4(mix(bottomColor, topColor, max(pow(max(h, 0.0), exponent), 0.0)), 1.0);
        }
      `,
      side: THREE.BackSide,
    });
    return new THREE.Mesh(skyGeo, skyMat);
  }

  initializeFarmGrid() {
    for (let y = 0; y < GRID_HEIGHT; y++) {
      this.plots[y] = [];
      for (let x = 0; x < GRID_WIDTH; x++) {
        const plot = createFarmPlot(x, y);
        this.plots[y][x] = plot;
        this.scene.add(plot.mesh);
      }
    }
  }

  initializeAnimals() {
    const chickenPositions = [
      new THREE.Vector3(-12, 0, 8),
      new THREE.Vector3(-8, 0, 10),
      new THREE.Vector3(10, 0, 8),
    ];
    const cowPositions = [
      new THREE.Vector3(12, 0, -10),
      new THREE.Vector3(-14, 0, -4),
    ];

    chickenPositions.forEach(pos => {
      const chicken = createChicken(pos);
      this.animals.push(chicken);
      this.scene.add(chicken.group);
    });

    cowPositions.forEach(pos => {
      const cow = createCow(pos);
      this.animals.push(cow);
      this.scene.add(cow.group);
    });
  }

  initializePets() {
    const dog = createDog(new THREE.Vector3(-8, 0, 6));
    this.pets.push(dog);
    this.scene.add(dog.group);

    const cat = createCat(new THREE.Vector3(8, 0, -6));
    this.pets.push(cat);
    this.scene.add(cat.group);
  }

  getSeasonForDay(day: number): SeasonSettings {
    const seasonIndex = Math.floor(day / 30) % 4;
    const seasons: SeasonSettings[] = [
      { name: 'spring', groundColor: 0x4a7c3f, skyTopColor: 0x87ceeb, skyBottomColor: 0xffe4b5, fogColor: 0xc8e6c9, ambientIntensity: 0.5, treeColor: 0x4a9c3f, hasSnow: false },
      { name: 'summer', groundColor: 0x5d9e50, skyTopColor: 0x0077ff, skyBottomColor: 0x87ceeb, fogColor: 0x87ceeb, ambientIntensity: 0.6, treeColor: 0x2d5a1e, hasSnow: false },
      { name: 'fall', groundColor: 0x8B6914, skyTopColor: 0x4a6fa5, skyBottomColor: 0xffa07a, fogColor: 0xd2b48c, ambientIntensity: 0.4, treeColor: 0xc8753c, hasSnow: false },
      { name: 'winter', groundColor: 0xd4d4d4, skyTopColor: 0x708090, skyBottomColor: 0xb0c4de, fogColor: 0xe0e0e0, ambientIntensity: 0.3, treeColor: 0x5a7a5a, hasSnow: true },
    ];
    return seasons[seasonIndex];
  }

  updateSeason(day: number) {
    this.seasonDay = day;
    const newSeason = this.getSeasonForDay(day);
    if (newSeason.name !== this.currentSeason.name) {
      this.currentSeason = newSeason;
      this.applySeasonEffects();
    }
  }

  applySeasonEffects() {
    const season = this.currentSeason;
    (this.environment.ground.material as THREE.MeshStandardMaterial).color.setHex(season.groundColor);

    this.scene.fog = new THREE.Fog(season.fogColor, 30, 60);

    this.environment.trees.forEach(tree => {
      tree.traverse(child => {
        if (child instanceof THREE.Mesh && child.material instanceof THREE.MeshStandardMaterial) {
          if (child.geometry.type === 'SphereGeometry' || child.geometry.type === 'ConeGeometry') {
            child.material.color.setHex(season.treeColor);
          }
        }
      });
    });

    if (season.hasSnow) {
      this.environment.rocks.forEach(rock => {
        rock.traverse(child => {
          if (child instanceof THREE.Mesh && child.material instanceof THREE.MeshStandardMaterial) {
            child.material.color.lerp(new THREE.Color(0xe0e0e0), 0.5);
          }
        });
      });
    }
  }

  collectAnimalProductAt(position: THREE.Vector3): string | null {
    for (const animal of this.animals) {
      const dist = animal.group.position.distanceTo(position);
      if (dist < 2 && animal.hasProduct) {
        const product = collectProduct(animal);
        if (product && this.onCollectProduct) {
          this.onCollectProduct(product);
        }
        return product;
      }
    }
    return null;
  }

  updateFarmState(state: FarmState) {
    for (let y = 0; y < GRID_HEIGHT; y++) {
      for (let x = 0; x < GRID_WIDTH; x++) {
        const index = y * GRID_WIDTH + x;
        const crop = state.grid[index];
        const plot = this.plots[y][x];

        if (crop) {
          updateCropVisual(plot, crop.crop, crop.stage, crop.growth, crop.withered);
        } else if (plot.cropMesh) {
          plot.mesh.remove(plot.cropMesh);
          plot.cropMesh.traverse((child) => {
            if (child instanceof THREE.Mesh) {
              child.geometry.dispose();
              if (Array.isArray(child.material)) {
                child.material.forEach((m) => m.dispose());
              } else {
                child.material.dispose();
              }
            }
          });
          plot.cropMesh = null;
          plot.cropType = null;
          plot.growthStage = '';
          plot.growthPercent = 0;
          plot.isWithered = false;
          (plot.soilMesh.material as THREE.MeshStandardMaterial).color.setHex(0x5c3a1e);
        }
      }
    }
  }

  setSelectedTool(tool: string) {
    this.selectedTool = tool;
    const highlightColors: Record<string, number> = {
      plant: 0xffff00,
      harvest: 0x5d9e50,
      water: 0x4a90d9,
      clear: 0xd94a38,
    };
    const color = highlightColors[tool] || 0xffff00;
    (this.highlightMesh.material as THREE.MeshBasicMaterial).color.setHex(color);
  }

  getPlotWorldPosition(x: number, y: number): THREE.Vector3 {
    const plot = this.plots[y]?.[x];
    if (!plot) return new THREE.Vector3();
    return plot.mesh.position.clone();
  }

  spawnHarvestParticles(x: number, y: number) {
    const pos = this.getPlotWorldPosition(x, y);
    this.effectManager.createHarvestParticles(pos);
    this.effectManager.createFloatingText('+1', pos, 0x5d9e50);
  }

  spawnPlantParticles(x: number, y: number) {
    const pos = this.getPlotWorldPosition(x, y);
    this.effectManager.createPlantParticles(pos);
  }

  spawnWaterParticles(x: number, y: number) {
    const pos = this.getPlotWorldPosition(x, y);
    this.effectManager.createWaterParticles(pos);
  }

  spawnCoinText(x: number, y: number, amount: number) {
    const pos = this.getPlotWorldPosition(x, y);
    this.effectManager.createFloatingText(`+${amount}💰`, pos, 0xffd700);
  }

  setupInputHandlers() {
    this.renderer.domElement.addEventListener('click', (e) => this.onClick(e));
    this.renderer.domElement.addEventListener('mousemove', (e) => this.onMouseMove(e));
    window.addEventListener('keydown', (e) => this.onKeyDown(e));
  }

  onClick(e: MouseEvent) {
    this.updateMousePosition(e);
    const intersects = this.getPlotIntersects();

    if (intersects.length > 0 && this.onPlotClick) {
      const hit = intersects[0].object;
      for (let y = 0; y < GRID_HEIGHT; y++) {
        for (let x = 0; x < GRID_WIDTH; x++) {
          const plot = this.plots[y][x];
          if (plot.soilMesh === hit || plot.mesh === hit || plot.mesh.children.includes(hit as THREE.Object3D)) {
            this.onPlotClick(x, y, this.selectedTool);

            const worldPos = new THREE.Vector3();
            plot.mesh.getWorldPosition(worldPos);
            movePlayerTo(this.player, worldPos.x, worldPos.z + 1.5);

            if (this.selectedTool === 'water') {
              this.player.currentAction = 'watering';
              setPlotWatered(plot, true);
            } else if (this.selectedTool === 'clear') {
              this.player.currentAction = 'clearing';
            } else {
              this.player.currentAction = plot.cropMesh ? 'harvesting' : 'planting';
            }

            setTimeout(() => {
              if (this.player.currentAction === 'planting' || this.player.currentAction === 'harvesting' || this.player.currentAction === 'watering' || this.player.currentAction === 'clearing') {
                this.player.currentAction = 'idle';
              }
            }, 1000);
            return;
          }
        }
      }
    }
  }

  onMouseMove(e: MouseEvent) {
    if (this.cameraController.isPanning || this.cameraController.isRotating) return;

    this.updateMousePosition(e);
    const intersects = this.getPlotIntersects();

    if (intersects.length > 0) {
      for (let y = 0; y < GRID_HEIGHT; y++) {
        for (let x = 0; x < GRID_WIDTH; x++) {
          const plot = this.plots[y][x];
          if (plot.soilMesh === intersects[0].object || plot.mesh === intersects[0].object) {
            this.hoveredPlot = plot;
            if (this.highlightMesh) {
              this.highlightMesh.visible = true;
              this.highlightMesh.position.copy(plot.mesh.position);
              this.highlightMesh.position.y = 0.1;
            }
            return;
          }
        }
      }
    }

    this.hoveredPlot = null;
    if (this.highlightMesh) {
      this.highlightMesh.visible = false;
    }
  }

  onKeyDown(e: KeyboardEvent) {
    const speed = 2;
    const moveX = this.player.group.position.x;
    const moveZ = this.player.group.position.z;

    switch (e.key.toLowerCase()) {
      case 'w':
      case 'arrowup':
        movePlayerTo(this.player, moveX, moveZ - speed);
        break;
      case 's':
      case 'arrowdown':
        movePlayerTo(this.player, moveX, moveZ + speed);
        break;
      case 'a':
      case 'arrowleft':
        movePlayerTo(this.player, moveX - speed, moveZ);
        break;
      case 'd':
      case 'arrowright':
        movePlayerTo(this.player, moveX + speed, moveZ);
        break;
      case 'r':
        setRain(this.weatherSystem, !this.weatherSystem.isRaining, 0.5);
        break;
      case '1':
        this.setSelectedTool('plant');
        break;
      case '2':
        this.setSelectedTool('harvest');
        break;
      case '3':
        this.setSelectedTool('water');
        break;
      case '4':
        this.setSelectedTool('clear');
        break;
      case 'e':
        const product = this.collectAnimalProductAt(this.player.group.position);
        if (product) {
          this.effectManager.createFloatingText(`+1 ${product}`, this.player.group.position.clone().add(new THREE.Vector3(0, 1.5, 0)), 0x87CEEB);
        }
        break;
    }
  }

  updateMousePosition(e: MouseEvent) {
    const rect = this.renderer.domElement.getBoundingClientRect();
    this.mouse.x = ((e.clientX - rect.left) / rect.width) * 2 - 1;
    this.mouse.y = -((e.clientY - rect.top) / rect.height) * 2 + 1;
  }

  getPlotIntersects(): THREE.Intersection[] {
    this.raycaster.setFromCamera(this.mouse, this.cameraController.camera);
    const soilMeshes = this.plots.flat().map((p) => p.soilMesh);
    return this.raycaster.intersectObjects(soilMeshes, false);
  }

  animate() {
    if (!this.isInitialized) return;

    const delta = this.clock.getDelta();
    const elapsed = this.clock.getElapsedTime();

    updatePlayerAnimation(this.player, delta);

    const dayFactor = updateDayNightCycle(this.dayNightCycle, delta);

    const dayNumber = Math.floor(elapsed / 10);
    this.updateSeason(dayNumber);

    const season = this.currentSeason;
    const skyTop = new THREE.Color(season.skyTopColor).lerp(new THREE.Color(0x0a0a2e), 1 - dayFactor);
    const skyBottom = new THREE.Color(season.skyBottomColor).lerp(new THREE.Color(0x1a1a3a), 1 - dayFactor);
    if (this.skybox.material instanceof THREE.ShaderMaterial) {
      this.skybox.material.uniforms.topColor.value.copy(skyTop);
      this.skybox.material.uniforms.bottomColor.value.copy(skyBottom);
    }

    this.scene.fog = new THREE.Fog(
      skyBottom.clone().lerp(skyTop, 0.5),
      30,
      60
    );

    updateWeatherSystem(this.weatherSystem, delta);

    this.windStrength = 0.2 + Math.sin(elapsed * 0.5) * 0.15;
    this.plots.flat().forEach((plot) => {
      updateWindAnimation(plot, elapsed, this.windStrength);
    });

    if (this.highlightMesh && this.highlightMesh.visible) {
      const pulse = Math.sin(elapsed * 5) * 0.15 + 0.3;
      (this.highlightMesh.material as THREE.MeshBasicMaterial).opacity = pulse;
    }

    this.plots.flat().forEach((plot) => {
      if (plot.cropMesh && plot.growthStage === 'Ready to Harvest') {
        const glow = plot.cropMesh.children.find(
          (c) => c instanceof THREE.Mesh && (c.material as THREE.Material)?.transparent
        ) as THREE.Mesh;
        if (glow) {
          const pulse = Math.sin(elapsed * 5) * 0.15 + 0.3;
          (glow.material as THREE.MeshBasicMaterial).opacity = pulse;
        }
      }
    });

    this.plots.flat().forEach((plot) => {
      if (plot.isWatered && plot.waterTimer > 0) {
        plot.waterTimer -= delta;
        if (plot.waterTimer <= 0) {
          setPlotWatered(plot, false);
        }
      }
    });

    this.effectManager.update(delta, this.scene);

    this.animals.forEach(animal => updateAnimal(animal, delta));

    const isNight = !this.dayNightCycle.isDay;
    this.effectManager.updateFireflies(delta, isNight);

    if (isNight !== this.lastDayNightState) {
      this.lastDayNightState = isNight;
      if (isNight) {
        this.effectManager.createFireflies(this.scene);
      }
    }

    // Use composer for post-processing if available, otherwise direct render
    if (this.composer && this.composer.passes.length > 0) {
      this.composer.render();
    } else {
      this.renderer.render(this.scene, this.cameraController.camera);
    }
  }

  resize() {
    this.renderer.setSize(window.innerWidth, window.innerHeight);
    this.cameraController.onResize();
    
    // Update composer resolution on resize
    if (this.composer) {
      const width = window.innerWidth;
      const height = window.innerHeight;
      this.composer.setSize(width, height);
      if (this.pixelationPass) {
        this.pixelationPass.resolution.set(
          Math.floor(width * this.resolutionScale),
          Math.floor(height * this.resolutionScale)
        );
      }
    }
  }

  dispose() {
    this.isInitialized = false;

    this.plots.flat().forEach((plot) => {
      if (plot.cropMesh) {
        plot.mesh.remove(plot.cropMesh);
        plot.cropMesh.traverse((child) => {
          if (child instanceof THREE.Mesh) {
            child.geometry.dispose();
            if (Array.isArray(child.material)) {
              child.material.forEach((m) => m.dispose());
            } else {
              child.material.dispose();
            }
          }
        });
        plot.cropMesh = null;
      }
      plot.mesh.traverse((child) => {
        if (child instanceof THREE.Mesh) {
          child.geometry.dispose();
          if (Array.isArray(child.material)) {
            child.material.forEach((m) => m.dispose());
          } else {
            child.material.dispose();
          }
        }
      });
      this.scene.remove(plot.mesh);
    });
    this.plots = [];

    if (this.player.group) this.scene.remove(this.player.group);
    if (this.environment.ground) this.scene.remove(this.environment.ground);
    if (this.environment.farmDirt) this.scene.remove(this.environment.farmDirt);
    if (this.environment.farmDirt) this.scene.remove(this.environment.farmDirt);
    this.environment.trees.forEach((tree) => this.scene.remove(tree));
    this.environment.buildings.forEach((building) => this.scene.remove(building));
    this.environment.fence.forEach((fence) => this.scene.remove(fence));
    this.environment.rocks.forEach((rock) => this.scene.remove(rock));
    this.environment.flowers.forEach((flower) => this.scene.remove(flower));
    this.environment.grassPatches.forEach((grass) => this.scene.remove(grass));
    this.environment.pathStones.forEach((stone) => this.scene.remove(stone));

    if (this.dayNightCycle.sunLight) this.scene.remove(this.dayNightCycle.sunLight);
    if (this.dayNightCycle.moonLight) this.scene.remove(this.dayNightCycle.moonLight);
    if (this.dayNightCycle.ambientLight) this.scene.remove(this.dayNightCycle.ambientLight);
    if (this.dayNightCycle.hemisphereLight) this.scene.remove(this.dayNightCycle.hemisphereLight);
    if (this.dayNightCycle.sunMesh) this.scene.remove(this.dayNightCycle.sunMesh);
    if (this.dayNightCycle.moonMesh) this.scene.remove(this.dayNightCycle.moonMesh);

    if (this.weatherSystem.rainParticles) this.scene.remove(this.weatherSystem.rainParticles);
    this.weatherSystem.clouds.forEach((cloud) => this.scene.remove(cloud));

    if (this.highlightMesh) {
      this.scene.remove(this.highlightMesh);
      this.highlightMesh.geometry.dispose();
      (this.highlightMesh.material as THREE.Material).dispose();
      this.highlightMesh = null as any;
    }

    if (this.skybox) {
      this.scene.remove(this.skybox);
      this.skybox.geometry.dispose();
      if (this.skybox.material instanceof THREE.ShaderMaterial) {
        this.skybox.material.dispose();
      }
    }

    if (this.cameraController.camera) this.scene.remove(this.cameraController.camera);

    this.effectManager.dispose(this.scene);

    this.renderer.domElement.remove();
    this.renderer.dispose();
    this.scene.clear();
  }
}
