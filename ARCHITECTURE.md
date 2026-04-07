# Viby Farming Simulator - Architecture Notes

## Bridge Pattern: C++ to React

### Data Flow

```
User clicks "Plant Carrot" in React UI
         │
         ▼
useGameBridge.plantSeed(0, 0, "Carrot")
         │
         ▼
window.farmBridge.plantSeed(0, 0, "Carrot")  [WASM call]
         │
         ▼
FarmBridge::plantSeed() → Farm::plantSeed()
         │
         ▼
Farm emits "cropPlanted" event via callback
         │
         ▼
FarmBridge callback → window.dispatchFarmEvent()
         │
         ▼
React useEffect listener receives event
         │
         ▼
Zustand store updated → React re-renders
```

### Why This Works for Solo Devs

1. **Clear boundaries**: C++ handles simulation, React handles presentation
2. **Minimal glue code**: Emscripten bindings are ~50 lines
3. **Easy to test**: C++ can be tested standalone, React can use mocks
4. **Hot reload**: React dev server updates UI instantly without rebuilding WASM

## Performance Characteristics

- **Farm simulation**: O(n) where n = grid size (100 for 10x10)
- **WASM call overhead**: ~1-5ms per function call
- **React re-renders**: Only on event emission, not every frame
- **Memory**: ~5-10MB total (WASM bundle + React app)

## Extending the Bridge

### Adding a New C++ Function

1. Add method to `Farm` class in `cpp-core/include/Farm.h`
2. Implement in `cpp-core/src/Farm.cpp`
3. Add wrapper to `FarmBridge` in `cpp-core/src/FarmBridge.cpp`
4. Add to `EMSCRIPTEN_BINDINGS` in `cpp-core/include/FarmBridge.h`
5. Call from React via `window.farmBridge.yourMethod()`

### Adding a New Event Type

1. Add event name to `FarmBridge::init()` callback registration
2. Emit via `farm.emitEvent("yourEvent", jsonData)`
3. Listen in React via `window.dispatchFarmEvent` in `useGameBridge`

## Alternative Architectures Considered

### Tauri (Desktop App)
- **Pros**: Native performance, file system access, easier debugging
- **Cons**: Requires desktop install, larger download (~15MB vs ~5MB WASM)
- **Best for**: Games needing native features (save files, mods)

### Electron
- **Pros**: Easiest dev experience, mature ecosystem
- **Cons**: Huge bundle (~150MB), high RAM usage
- **Best for**: Team projects, rapid prototyping

### Node.js Native Addon
- **Pros**: Direct C++ integration, no compilation step for web
- **Cons**: Server-side only, requires backend
- **Best for**: Multiplayer games with server authority

## Why WASM Won for This Project

- Web-first distribution (share a link, no install)
- C++ performance for simulation (thousands of crops, weather, animals)
- React's animation ecosystem for the "cozy vibe"
- Single-person maintainable codebase