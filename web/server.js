
import fs from "fs";
import http from "http";
import path from "path";
import readline from "node:readline";
import express from "express";
import { fileURLToPath } from "url";
import { Server } from "socket.io";

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const FIFO = "/tmp/frames.fifo";

const app = express();
const server = http.createServer(app);
const io = new Server(server);

app.use(express.static(path.join(__dirname, "public")));  // sirve index.html

io.on("connection", () => console.log("cliente conectado"));

const seenIds = new Set();

function startReader() {
  // bloquea hasta que haya alguien escribiendo en el FIFO
  const stream = fs.createReadStream(FIFO, { encoding: "utf8" });
  const rl = readline.createInterface({ input: stream });

  let lastLine = null;

  rl.on("line", line => {
    const l = line.trim();
    const parts = line.split(",");
    const id = parts[1]?.toUpperCase();
    if (!id) return;
    if (seenIds.has(id)) return;
    seenIds.add(id);
    io.emit("frame", line);
  });

  rl.on("close", () => {
    setTimeout(startReader, 1000); // reintenta tras 1s
  });

  rl.on("error", err => {
    console.error("Error FIFO:", err.message);
    setTimeout(startReader, 1000);
  });
}
startReader();

const PORT = 3000;
server.listen(PORT, () =>
  console.log(`Escucha en http://localhost:${PORT}`));

