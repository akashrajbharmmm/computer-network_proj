// run.js
import http from "http";
import { exec } from "child_process";
import { readFile } from "fs/promises";
import path from "path";

const PORT = 5000;
const __dirname = process.cwd();

http
  .createServer(async (req, res) => {
    if (req.url.startsWith("/lookup")) {
      const urlParams = new URL(req.url, `http://${req.headers.host}`).searchParams;
      const mode = urlParams.get("mode");
      const query = urlParams.get("query");

      if (!mode || !query) {
        res.writeHead(400, { "Content-Type": "text/plain" });
        res.end("Missing parameters");
        return;
      }

      // Run your DNS tool (synchronously)
      exec(`dns_tool.exe ${mode === "domain" ? 1 : 2} ${query}`, (err, stdout, stderr) => {
        res.writeHead(200, { "Content-Type": "text/plain" });
        if (err) res.end(stderr || err.message);
        else res.end(stdout);
      });
    } else if (req.url === "/" || req.url === "/index.html") {
      const html = await readFile(path.join(__dirname, "index.html"), "utf-8");
      res.writeHead(200, { "Content-Type": "text/html" });
      res.end(html);
    } else {
      res.writeHead(404);
      res.end("Not Found");
    }
  })
  .listen(PORT, () => {
    console.log(`🚀 Server running at http://localhost:${PORT}`);
  });
