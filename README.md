# Viby Farming Simulator

A hybrid architecture farming simulator with a C++ core and React frontend, compiled to WebAssembly with Emscripten.

## Architecture

```
┌─────────────────────────────────────────────┐
│           React Frontend (UI)               │
│  ┌──────────────┐  ┌─────────────────────┐  │
│  │  FarmGrid    │  │   InventoryUI       │  │
│  │  Component   │  │   Component         │  │
│  └──────────────┘  └─────────────────────┘  │
│           │                    │             │
│  ┌─────────────────────────────────────┐    │
│  │     useGameBridge Hook              │    │
│  │  (WASM communication layer)         │    │
│  └─────────────────────────────────────┘    │
└──────────────────┬──────────────────────────┘
                   │ JavaScript Bridge
                   │ Event System
┌──────────────────┴──────────────────────────┐
│         WebAssembly Module                  │
│  ┌─────────────────────────────────────┐    │
│  │     FarmBridge (Emscripten)         │    │
│  │  - plantSeed(x, y, cropName)        │    │
│  │  - harvestCrop(x, y)                │    │
│  │  - updateWorld()                    │    │
│  │  - getInventory()                   │    │
│  └─────────────────────────────────────┘    │
│  ┌──────────────┐  ┌─────────────────────┐  │
│  │  Farm        │  │   Crop              │  │
│  │  (10x10      │  │   (Growth stages,   │  │
│  │   grid)      │  │    time tracking)   │  │
│  └──────────────┘  └─────────────────────┘  │
└─────────────────────────────────────────────┘
```

## Tech Stack

- **C++ Core**: Farm simulation logic, crop growth, inventory management
- **Emscripten**: Compiles C++ to WebAssembly
- **React + TypeScript**: UI layer
- **Framer Motion**: Smooth animations for the "vibe"
- **Tailwind CSS**: Glassmorphism UI styling
- **Zustand**: Lightweight state management
- **Vite**: Fast dev server and build tool

## Prerequisites

- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
- Node.js 18+ and npm
- CMake 3.13+

## Getting Started

### Quick Start (recommended)

From the project root:

```bash
npm install
npm run game
```

If PowerShell blocks `npm` scripts on your machine, use:

```powershell
npm.cmd run game
```

What this does:
- Builds WASM only when `react-ui/public/viby-farm.js` or `react-ui/public/viby-farm.wasm` is missing
- Starts the React dev server

Optional:

```bash
# Start UI only (skip WASM build check)
npm run game:skip-wasm
```

### 1. Install Dependencies

```bash
# Install React dependencies
cd react-ui
npm install
```

### 2. Build C++ Core to WebAssembly

```bash
# From project root
mkdir build && cd build

# Configure with Emscripten (adjust path to your emsdk)
emcmake cmake .. -DBUILD_FOR_WEB=ON

# Build
emmake make
```

This generates `viby-farm.js` and `viby-farm.wasm` in `react-ui/public/`.

### 3. Run Development Server

```bash
cd react-ui
npm run dev
```

## Project Structure

```

## Where to Read the Code

- C++ crop logic: `cpp-core/src/Crop.cpp`
- C++ farm simulation: `cpp-core/src/Farm.cpp`
- C++ to JS bridge: `cpp-core/src/FarmBridge.cpp`
- WASM entry: `bridge/main.cpp`
- React game screen: `react-ui/src/App.tsx`
- React bridge hook: `react-ui/src/hooks/useGameBridge.ts`
vibe/
├── CMakeLists.txt              # Build configuration
├── cpp-core/
│   ├── include/
│   │   ├── Crop.h              # Crop class with growth stages
│   │   ├── Farm.h              # Farm grid management
│   │   └── FarmBridge.h        # Emscripten bindings
│   └── src/
│       ├── Crop.cpp            # Crop growth logic
│       ├── Farm.cpp            # Farm grid & inventory
│       └── FarmBridge.cpp      # Bridge implementation
├── bridge/
│   └── main.cpp                # WASM entry point
└── react-ui/
    ├── src/
    │   ├── components/
    │   │   ├── FarmGrid.tsx    # Interactive farm grid
    │   │   └── InventoryUI.tsx # Inventory with animations
    │   ├── hooks/
    │   │   └── useGameBridge.ts # WASM communication hook
    │   ├── store/
    │   │   └── farmStore.ts    # Zustand state management
    │   ├── App.tsx
    │   ├── main.tsx
    │   └── index.css
    ├── index.html
    ├── package.json
    ├── tailwind.config.js
    ├── postcss.config.js
    ├── tsconfig.json
    └── vite.config.ts
```

## Key Design Decisions

### Why WebAssembly?

For a solo indie dev building a "cozy" farming sim:

**Pros:**
- Single deployable (just HTML + WASM files)
- C++ performance for simulation logic
- Runs in any modern browser
- No server required for game logic

**Cons:**
- Harder debugging (browser devtools for WASM are limited)
- Larger initial load (~2-5MB WASM bundle)
- Emscripten toolchain setup required

**Alternative considered:** Tauri for desktop distribution. Would provide native performance and easier debugging, but requires users to install a desktop app.

### Event System Design

C++ emits events via callbacks registered through `Farm::registerCallback()`. The `FarmBridge` forwards these to JavaScript using `emscripten::val::global("window").call<void>("dispatchFarmEvent", ...)`.

React listens via `window.dispatchFarmEvent` and updates Zustand store, which triggers re-renders only for affected components.

### State Management

- **C++ owns**: Farm grid state, crop growth, inventory counts
- **React owns**: UI state (selected tool, hover states, animations)
- **Sync**: One-way from C++ to React via events; React calls C++ functions for actions

## Crop Types

| Crop   | Growth Time (ticks) |
|--------|---------------------|
| Carrot | 100                 |
| Tomato | 150                 |
| Potato | 120                 |

## Growth Stages

1. **Seed** (0% growth)
2. **Sprout** (0-25%)
3. **Growing** (25-50%)
4. **Mature** (50-99%)
5. **Ready to Harvest** (100%)

## Adding New Features

### New Crop Type

1. Add to `Farm::plantSeed()` in `cpp-core/src/Farm.cpp`:
```cpp
else if (cropName == "Pumpkin") growthTime = 200;
```

2. Add icon to `InventoryUI.tsx`:
```tsx
const cropIcons: Record<string, string> = {
  Pumpkin: '🎃',
  // ...
}
```

### New UI Component

1. Create in `react-ui/src/components/`
2. Use `useGameBridge()` hook for C++ communication
3. Wrap with `motion.div` for Framer Motion animations
4. Use Tailwind classes with `backdrop-blur` and `bg-white/10` for glassmorphism

## Build Commands

```bash
# Development (React only, mock WASM)
cd react-ui && npm run dev

# Production build
cd react-ui && npm run build

# Full rebuild (C++ WASM + React)
emcmake cmake build -DBUILD_FOR_WEB=ON && emmake make -C build
cd react-ui && npm run build
```

## License

MIT - Build something cozy.
