import * as THREE from 'three';

export type CameraMode = 'fixed_isometric' | 'free';
export type PixelStylePreset = 'subtle' | 'balanced' | 'heavy';

export class CameraController {
  camera: THREE.PerspectiveCamera;
  target: THREE.Vector3;
  distance: number;
  minDistance: number;
  maxDistance: number;
  theta: number;
  phi: number;
  minPhi: number;
  maxPhi: number;
  isPanning: boolean;
  isRotating: boolean;
  lastMouse: { x: number; y: number };
  panSpeed: number;
  rotateSpeed: number;
  zoomSpeed: number;
  
  // Fixed isometric settings
  cameraMode: CameraMode;
  fixedTheta: number;
  fixedPhi: number;
  fixedDistance: number;
  followTarget: THREE.Vector3;
  worldBounds: { minX: number; maxX: number; minZ: number; maxZ: number };
  
  // Smooth follow
  followLerp: number;
  followOffset: THREE.Vector3;

  constructor() {
    this.camera = new THREE.PerspectiveCamera(50, window.innerWidth / window.innerHeight, 0.1, 200);
    this.target = new THREE.Vector3(0, 0, 0);
    this.followTarget = new THREE.Vector3(0, 0, 0);
    this.distance = 28;
    this.minDistance = 12;
    this.maxDistance = 50;
    this.theta = Math.PI / 4;
    this.phi = Math.PI / 3.5;
    this.minPhi = Math.PI / 6;
    this.maxPhi = Math.PI / 2.2;
    this.isPanning = false;
    this.isRotating = false;
    this.lastMouse = { x: 0, y: 0 };
    this.panSpeed = 0.02;
    this.rotateSpeed = 0.005;
    this.zoomSpeed = 1.5;
    
    // Fixed isometric defaults (3/4 view)
    this.cameraMode = 'fixed_isometric';
    this.fixedTheta = Math.PI / 4; // 45 degrees
    this.fixedPhi = Math.acos(1 / Math.sqrt(3)); // ~54.7 degrees (true isometric)
    this.fixedDistance = 32;
    this.worldBounds = { minX: -30, maxX: 30, minZ: -30, maxZ: 30 };
    this.followLerp = 0.08;
    this.followOffset = new THREE.Vector3(0, 0, 0);

    this.updateCameraPosition();
    this.setupControls();
  }

  setupControls() {
    window.addEventListener('wheel', (e) => this.onWheel(e), { passive: false });
    window.addEventListener('mousedown', (e) => this.onMouseDown(e));
    window.addEventListener('mousemove', (e) => this.onMouseMove(e));
    window.addEventListener('mouseup', () => this.onMouseUp());
    window.addEventListener('contextmenu', (e) => e.preventDefault());
    window.addEventListener('resize', () => this.onResize());
  }

  onWheel(e: WheelEvent) {
    // Disable zoom in fixed isometric mode
    if (this.cameraMode === 'fixed_isometric') return;
    
    e.preventDefault();
    this.distance += e.deltaY * this.zoomSpeed * 0.01;
    this.distance = Math.max(this.minDistance, Math.min(this.maxDistance, this.distance));
    this.updateCameraPosition();
  }

  onMouseDown(e: MouseEvent) {
    // Disable rotation/pan in fixed isometric mode
    if (this.cameraMode === 'fixed_isometric') return;
    
    if (e.button === 0) {
      this.isRotating = true;
    } else if (e.button === 2) {
      this.isPanning = true;
    }
    this.lastMouse = { x: e.clientX, y: e.clientY };
  }

  onMouseMove(e: MouseEvent) {
    // No manual control in fixed isometric mode
    if (this.cameraMode === 'fixed_isometric') return;
    
    const dx = e.clientX - this.lastMouse.x;
    const dy = e.clientY - this.lastMouse.y;
    this.lastMouse = { x: e.clientX, y: e.clientY };

    if (this.isRotating) {
      this.theta -= dx * this.rotateSpeed;
      this.phi -= dy * this.rotateSpeed;
      this.phi = Math.max(this.minPhi, Math.min(this.maxPhi, this.phi));
      this.updateCameraPosition();
    }

    if (this.isPanning) {
      const panX = -dx * this.panSpeed;
      const panZ = dy * this.panSpeed;
      const sinTheta = Math.sin(this.theta);
      const cosTheta = Math.cos(this.theta);
      this.target.x += panX * cosTheta + panZ * sinTheta;
      this.target.z += -panX * sinTheta + panZ * cosTheta;
      this.updateCameraPosition();
    }
  }

  onMouseUp() {
    this.isPanning = false;
    this.isRotating = false;
  }

  onResize() {
    this.camera.aspect = window.innerWidth / window.innerHeight;
    this.camera.updateProjectionMatrix();
  }

  updateCameraPosition() {
    if (this.cameraMode === 'fixed_isometric') {
      // Use fixed isometric angles
      this.theta = this.fixedTheta;
      this.phi = this.fixedPhi;
      this.distance = this.fixedDistance;
      
      // Smooth follow to target
      this.target.lerp(this.followTarget, this.followLerp);
      
      // Clamp target to world bounds
      this.target.x = Math.max(this.worldBounds.minX, Math.min(this.worldBounds.maxX, this.target.x));
      this.target.z = Math.max(this.worldBounds.minZ, Math.min(this.worldBounds.maxZ, this.target.z));
    }
    
    this.camera.position.x = this.target.x + this.distance * Math.sin(this.phi) * Math.sin(this.theta);
    this.camera.position.y = this.target.y + this.distance * Math.cos(this.phi);
    this.camera.position.z = this.target.z + this.distance * Math.sin(this.phi) * Math.cos(this.theta);
    this.camera.lookAt(this.target);
  }

  setTarget(x: number, y: number, z: number) {
    if (this.cameraMode === 'fixed_isometric') {
      this.followTarget.set(x, y, z);
    } else {
      this.target.set(x, y, z);
    }
    this.updateCameraPosition();
  }

  smoothMoveTo(x: number, y: number, z: number, duration: number = 0.5) {
    const startTarget = this.target.clone();
    const endTarget = new THREE.Vector3(x, y, z);
    const startTime = Date.now();

    const animate = () => {
      const elapsed = (Date.now() - startTime) / 1000;
      const t = Math.min(elapsed / duration, 1);
      const eased = t < 0.5 ? 2 * t * t : 1 - Math.pow(-2 * t + 2, 2) / 2;

      this.target.lerpVectors(startTarget, endTarget, eased);
      this.updateCameraPosition();

      if (t < 1) {
        requestAnimationFrame(animate);
      }
    };

    animate();
  }

  setCameraMode(mode: CameraMode) {
    this.cameraMode = mode;
    if (mode === 'fixed_isometric') {
      this.followTarget.copy(this.target);
    }
    this.updateCameraPosition();
  }

  setWorldBounds(bounds: { minX: number; maxX: number; minZ: number; maxZ: number }) {
    this.worldBounds = bounds;
  }

  setFollowLerp(lerp: number) {
    this.followLerp = lerp;
  }
}
