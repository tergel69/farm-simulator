import { spawn } from "node:child_process";
import os from "node:os";

const isWindows = os.platform() === "win32";
const command = isWindows ? "cmd.exe" : "bash";
const args = isWindows ? ["/c", "build-wasm.bat"] : ["build-wasm.sh"];

const child = spawn(command, args, {
  stdio: "inherit",
});

child.on("exit", (code) => {
  process.exit(code ?? 1);
});

