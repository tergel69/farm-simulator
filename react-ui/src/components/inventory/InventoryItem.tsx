import { motion } from "framer-motion";

interface InventoryItemProps {
  name: string;
  quantity: number;
}

const cropSprites: Record<string, string> = {
  Carrot: "🥕",
  Tomato: "🍅",
  Potato: "🥔",
  Wheat: "🌾",
  Corn: "🌽",
  Strawberry: "🍓",
};

export function InventoryItem({ name, quantity }: InventoryItemProps) {
  return (
    <motion.div
      layout
      initial={{ opacity: 0, x: -4 }}
      animate={{ opacity: 1, x: 0 }}
      exit={{ opacity: 0, x: 4 }}
      transition={{ type: "tween", duration: 0.12 }}
      className="flex items-center justify-between border-2 border-[#5c4a32] bg-[#1a120a] px-3 py-2"
    >
      <div className="flex items-center gap-2">
        <span className="text-base leading-none">{cropSprites[name] ?? "📦"}</span>
        <span className="pixel-text text-[9px]">{name}</span>
      </div>
      <span className="pixel-badge pixel-badge-warning">
        x{quantity}
      </span>
    </motion.div>
  );
}
