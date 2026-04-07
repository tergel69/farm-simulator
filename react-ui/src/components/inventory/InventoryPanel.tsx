import { AnimatePresence, motion } from "framer-motion";
import { InventoryItem } from "./InventoryItem";
import type { FarmState } from "../../bridge/types";
import { CROP_DEFINITIONS } from "../../bridge/types";

interface InventoryPanelProps {
  state: FarmState;
  ready: boolean;
  lastEventLabel: string;
  selectedCrop: string;
  onCropSelect: (crop: string) => void;
  onAdvanceTime: () => void | Promise<void>;
}

const cropSprites: Record<string, string> = {
  Carrot: "🥕",
  Tomato: "🍅",
  Potato: "🥔",
  Wheat: "🌾",
  Corn: "🌽",
  Strawberry: "🍓",
};

export function InventoryPanel({
  state,
  ready,
  lastEventLabel,
  selectedCrop,
  onCropSelect,
  onAdvanceTime,
}: InventoryPanelProps) {
  const inventoryEntries = Object.entries(state.inventory);

  return (
    <section className="pixel-panel p-4">
      {/* Header */}
      <div className="mb-3 flex items-center justify-between">
        <div>
          <p className="pixel-label">Inventory</p>
          <h2 className="pixel-title text-[11px]">Satchel</h2>
        </div>
        <div className={`pixel-badge ${ready ? "pixel-badge-active" : "pixel-badge-warning"}`}>
          {ready ? "SYNC" : "WAIT"}
        </div>
      </div>

      <div className="pixel-divider mb-3" />

      {/* Event log */}
      <div className="mb-3 rounded-sm border-2 border-[#3a2a18] bg-black/30 px-3 py-2">
        <span className="pixel-text-dim">
          {">"} {lastEventLabel || "Waiting..."}
        </span>
      </div>

      {/* Inventory list */}
      <div className="mb-3 grid gap-2">
        <AnimatePresence initial={false}>
          {inventoryEntries.length > 0 ? (
            inventoryEntries.map(([name, quantity]) => (
              <InventoryItem key={name} name={name} quantity={quantity} />
            ))
          ) : (
            <motion.div
              key="empty"
              initial={{ opacity: 0 }}
              animate={{ opacity: 1 }}
              exit={{ opacity: 0 }}
              className="border-2 border-dashed border-[#5c4a32] px-3 py-4 text-center"
            >
              <span className="pixel-text-dim">Empty satchel...</span>
            </motion.div>
          )}
        </AnimatePresence>
      </div>

      <div className="pixel-divider mb-3" />

      {/* Seed catalog */}
      <p className="pixel-label mb-2">Seed Catalog</p>
      <div className="mb-3 grid grid-cols-2 gap-2">
        {Object.entries(CROP_DEFINITIONS).map(([key, def]) => {
          const isSelected = selectedCrop === key;
          return (
            <motion.button
              key={key}
              whileTap={{ scale: 0.96 }}
              transition={{ type: "tween", duration: 0.06 }}
              onClick={() => onCropSelect(def.name)}
              className={`flex flex-col items-start border-2 px-3 py-2 text-left transition-[border-color,background] ${
                isSelected
                  ? "border-[#f0c040] bg-[#3a2a18]"
                  : "border-[#5c4a32] bg-[#1a120a] hover:border-[#8b7355] hover:bg-[#2a1f14]"
              }`}
            >
              <div className="flex w-full items-center justify-between">
                <span className="text-sm leading-none">{cropSprites[key] ?? "🌱"}</span>
                {isSelected && (
                  <span className="text-[8px]" style={{ color: "var(--gold)" }}>
                    ★
                  </span>
                )}
              </div>
              <span className="mt-1 pixel-text text-[8px]">{def.displayName}</span>
              <span className="pixel-text-dim">{def.growthTime}t</span>
            </motion.button>
          );
        })}
      </div>

      {/* Advance time button */}
      <motion.button
        whileTap={{ scale: 0.97 }}
        transition={{ type: "tween", duration: 0.06 }}
        onClick={() => void onAdvanceTime()}
        className="pixel-btn pixel-btn-gold w-full"
      >
        ⏭ Advance Tick
      </motion.button>
    </section>
  );
}
