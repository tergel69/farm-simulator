import { existsSync } from "node:fs";
import { spawn } from "node:child_process";
import path from "node:path";
import process from "node:process";

const rootDir = process.cwd();
const wasmJsPath = path.join(rootDir, "react-ui", "public", "viby-farm.js");
const wasmBinPath = path.join(rootDir, "react-ui", "public", "viby-farm.wasm");
const skipWasm = process.argv.includes("--skip-wasm");
const hasWasmArtifacts = existsSync(wasmJsPath) && existsSync(wasmBinPath);

function run(command, args = []) {
  return new Promise((resolve, reject) => {
    const normalizedCommand =
      process.platform === "win32" && command === "npm" ? "npm.cmd" : command;

    const child = spawn(normalizedCommand, args, {
      stdio: "inherit",
      shell: process.platform === "win32",
    });

    child.on("exit", (code) => {
      if (code === 0) {
        resolve();
        return;
      }
      reject(new Error(`${normalizedCommand} exited with code ${code ?? "unknown"}`));
    });
  });
}

async function main() {
  if (!skipWasm && !hasWasmArtifacts) {
    console.log("WASM artifacts missing. Building C++ core first...");
    await run("npm", ["run", "wasm:build"]);
  } else if (!skipWasm && hasWasmArtifacts) {
    console.log("WASM artifacts found. Starting game...");
  } else {
    console.log("Skipping WASM build. Starting UI dev server...");
  }

  await run("npm", ["run", "dev"]);
}

main().catch((error) => {
  console.error(error.message);
  process.exit(1);
});
