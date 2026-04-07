export interface Recipe {
  id: string;
  name: string;
  icon: string;
  ingredients: Record<string, number>;
  sellPrice: number;
  xpReward: number;
}

export const RECIPES: Recipe[] = [
  {
    id: 'bread',
    name: 'Bread',
    icon: '🍞',
    ingredients: { Wheat: 3 },
    sellPrice: 30,
    xpReward: 15,
  },
  {
    id: 'tomato_soup',
    name: 'Tomato Soup',
    icon: '🍲',
    ingredients: { Tomato: 2 },
    sellPrice: 50,
    xpReward: 20,
  },
  {
    id: 'cornbread',
    name: 'Cornbread',
    icon: '🌽',
    ingredients: { Corn: 2, Wheat: 1 },
    sellPrice: 55,
    xpReward: 25,
  },
  {
    id: 'veggie_stew',
    name: 'Veggie Stew',
    icon: '🥘',
    ingredients: { Potato: 1, Tomato: 1, Carrot: 1 },
    sellPrice: 70,
    xpReward: 30,
  },
  {
    id: 'strawberry_jam',
    name: 'Strawberry Jam',
    icon: '🍓',
    ingredients: { Strawberry: 3 },
    sellPrice: 90,
    xpReward: 35,
  },
  {
    id: 'pumpkin_pie',
    name: 'Pumpkin Pie',
    icon: '🥧',
    ingredients: { Pumpkin: 2, Wheat: 1 },
    sellPrice: 100,
    xpReward: 40,
  },
  {
    id: 'blueberry_muffin',
    name: 'Blueberry Muffin',
    icon: '🧁',
    ingredients: { Blueberry: 2, Wheat: 1 },
    sellPrice: 95,
    xpReward: 38,
  },
  {
    id: 'pepper_sauce',
    name: 'Pepper Sauce',
    icon: '🌶️',
    ingredients: { Pepper: 2, Tomato: 1 },
    sellPrice: 60,
    xpReward: 28,
  },
  {
    id: 'sunflower_oil',
    name: 'Sunflower Oil',
    icon: '🌻',
    ingredients: { Sunflower: 3 },
    sellPrice: 75,
    xpReward: 32,
  },
  {
    id: 'farm_feast',
    name: 'Farm Feast',
    icon: '🎉',
    ingredients: { Carrot: 1, Tomato: 1, Potato: 1, Corn: 1, Wheat: 1 },
    sellPrice: 150,
    xpReward: 60,
  },
  {
    id: 'egg_salad',
    name: 'Egg Salad',
    icon: '🥗',
    ingredients: { Egg: 2 },
    sellPrice: 40,
    xpReward: 18,
  },
  {
    id: 'cheese',
    name: 'Cheese',
    icon: '🧀',
    ingredients: { Milk: 2 },
    sellPrice: 45,
    xpReward: 20,
  },
];

export function canCraftRecipe(recipe: Recipe, inventory: Record<string, number>): boolean {
  for (const [ingredient, amount] of Object.entries(recipe.ingredients)) {
    if ((inventory[ingredient] || 0) < amount) return false;
  }
  return true;
}

export function craftRecipe(recipe: Recipe, inventory: Record<string, number>): { newInventory: Record<string, number>; sellPrice: number; xpReward: number } | null {
  if (!canCraftRecipe(recipe, inventory)) return null;

  const newInventory = { ...inventory };
  for (const [ingredient, amount] of Object.entries(recipe.ingredients)) {
    newInventory[ingredient] = (newInventory[ingredient] || 0) - amount;
    if (newInventory[ingredient] <= 0) {
      delete newInventory[ingredient];
    }
  }

  return {
    newInventory,
    sellPrice: recipe.sellPrice,
    xpReward: recipe.xpReward,
  };
}
