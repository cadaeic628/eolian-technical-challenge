
import fs from "fs";
import http from "http";
import path from "path";
import { fileURLToPath } from "url";
import express from "express";
import { Server } from "socket.io";

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const FIFO = "/tmp/frames.fifo";

const app = express();
const server = http.createServer(app);
const io = new Server(server);

app.use(express.static(path.join(__dirname, "public")));  // sirve index.html

io.on("connection", () => console.log("cliente conectado"));

function watchFIFO() {
  // abre el FIFO de forma permanente
  const stream = fs.createReadStream(FIFO, { encoding: "utf8" });
  let buf = "";

  stream.on("data", chunk => {
    buf += chunk;
    const lines = buf.split("\n");
    buf = lines.pop(); // línea incompleta queda en buffer
    lines.forEach(l => { // emite cada frame completo
      if (l.trim()) io.emit("frame", l.trim());
    });
  });

  // si alguien hace 'rm' del fifo, nos reconectamos en un segundo
  stream.on("error", () => setTimeout(watchFIFO, 1000));
  stream.on("end",   () => setTimeout(watchFIFO, 100));
}
watchFIFO();

const PORT = 3000;
server.listen(PORT, () =>
  console.log(`Escucha en http://localhost:${PORT}`));

