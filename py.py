"""
winoptima_singlefile.py

Single-file local WinOptima app:
- Embedded HTML/CSS/JS served by Python's built-in http.server
- Live simulation using Python threads
- No external Python packages required
- Auto-opens browser on Windows
- UI matches your screenshot layout
"""

import http.server
import socketserver
import threading
import json
import time
import random
import webbrowser
from datetime import datetime
from urllib.parse import urlparse

PORT = 8000
TICK_RATE = 1.0
TOTAL_RAM_MB = 16384
MAX_HISTORY = 60

# ---------- PRIORITY + STATUS ----------
class Priority:
    REALTIME = "Realtime"
    HIGH = "High"
    NORMAL = "Normal"
    LOW = "Low"

class ProcessStatus:
    RUNNING = "Running"
    THROTTLED = "Throttled"
    CRITICAL = "Critical"

def nowtime():
    return datetime.now().strftime("%H:%M:%S")

# ---------- INITIAL PROCESS LIST ----------
INITIAL_PROGRAMS = [
    {"id": 1, "name": "System Kernel", "priority": Priority.REALTIME, "cpuDemand": 7.5, "ramDemand": 1500.0, "cpuAllocated": 0.0, "ramAllocated": 0.0, "status": ProcessStatus.RUNNING},
    {"id": 2, "name": "Chrome Browser", "priority": Priority.NORMAL, "cpuDemand": 15.0, "ramDemand": 3667.0, "cpuAllocated": 0.0, "ramAllocated": 0.0, "status": ProcessStatus.RUNNING},
    {"id": 3, "name": "VS Code", "priority": Priority.HIGH, "cpuDemand": 19.2, "ramDemand": 1023.0, "cpuAllocated": 0.0, "ramAllocated": 0.0, "status": ProcessStatus.RUNNING},
    {"id": 4, "name": "Docker Desktop", "priority": Priority.NORMAL, "cpuDemand": 29.3, "ramDemand": 6590.0, "cpuAllocated": 0.0, "ramAllocated": 0.0, "status": ProcessStatus.RUNNING},
    {"id": 5, "name": "Spotify Music", "priority": Priority.LOW, "cpuDemand": 6.5, "ramDemand": 380.0, "cpuAllocated": 0.0, "ramAllocated": 0.0, "status": ProcessStatus.RUNNING},
]

# ---------- SHARED STATE ----------
state_lock = threading.Lock()
programs = [dict(p) for p in INITIAL_PROGRAMS]
mode = "Balanced"
is_running = True
history = []
events = []
start_time = time.time()

# ---------- SIMULATION LOGIC ----------
def fluctuateDemand(current, variance, min_v, max_v):
    change = (random.random() - 0.5) * variance
    return max(min_v, min(max_v, current + change))

def calculateAllocation(progs, currentMode):
    new = [dict(p) for p in progs]

    for p in new:
        p["cpuDemand"] = fluctuateDemand(p["cpuDemand"], 5, 1, 90)
        p["ramDemand"] = fluctuateDemand(p["ramDemand"], 200, 200, 8000)

    totalCpu = sum(p["cpuDemand"] for p in new)
    totalRam = sum(p["ramDemand"] for p in new)

    # CPU allocation
    if totalCpu <= 100:
        for p in new:
            p["cpuAllocated"] = p["cpuDemand"]
            p["status"] = ProcessStatus.RUNNING
    else:
        weights = {
            Priority.REALTIME: 4 if currentMode == "Performance" else 3,
            Priority.HIGH: 3 if currentMode == "Performance" else 2,
            Priority.NORMAL: 1,
            Priority.LOW: 0.2 if currentMode == "Efficiency" else 0.5,
        }
        totalW = sum(weights[p["priority"]] * p["cpuDemand"] for p in new) or 1e-6
        for p in new:
            share = (weights[p["priority"]] * p["cpuDemand"]) / totalW
            alloc = min(share * 100, p["cpuDemand"])

            status = ProcessStatus.RUNNING
            if alloc < p["cpuDemand"] * 0.8:
                status = ProcessStatus.THROTTLED
            if alloc < p["cpuDemand"] * 0.4:
                status = ProcessStatus.CRITICAL

            if p["priority"] == Priority.REALTIME:
                alloc = min(p["cpuDemand"], 100)
                status = ProcessStatus.RUNNING

            p["cpuAllocated"] = alloc
            p["status"] = status

    # RAM allocation
    if totalRam > TOTAL_RAM_MB:
        overflow = totalRam - TOTAL_RAM_MB
        for p in new:
            if p["priority"] == Priority.REALTIME:
                p["ramAllocated"] = p["ramDemand"]
            else:
                share = p["ramDemand"] / totalRam
                cut = overflow * share * 1.2
                p["ramAllocated"] = max(100, p["ramDemand"] - cut)
                if (p["ramDemand"] - cut) < p["ramDemand"] * 0.9:
                    if p["status"] == ProcessStatus.RUNNING:
                        p["status"] = ProcessStatus.THROTTLED
    else:
        for p in new:
            p["ramAllocated"] = p["ramDemand"]

    return new

def analyze_system_state():
    with state_lock:
        bott = [p for p in programs if p["status"] in (ProcessStatus.THROTTLED, ProcessStatus.CRITICAL)]
    if not bott:
        return f"{nowtime()}: System nominal. No immediate action required."
    lines = [f"{nowtime()}: Recommendations:"]
    for p in bott:
        lines.append(f"- Reduce memory footprint or lower priority of {p['name']} ({p['priority']}).")
    lines.append("- Switch to Performance mode if needed.")
    return "\n".join(lines)

# ---------- SIMULATION THREAD ----------
def sim_thread_fn():
    global programs, history, events
    while True:
        time.sleep(TICK_RATE)
        with state_lock:
            if not is_running:
                continue
            programs = calculateAllocation(programs, mode)
            cpu = sum(p["cpuAllocated"] for p in programs)
            ram = sum(p["ramAllocated"] for p in programs)
            thr = sum(1 for p in programs if p["status"] in (ProcessStatus.THROTTLED, ProcessStatus.CRITICAL))
            ts = nowtime()
            history.append({"time": ts, "cpuUsage": cpu, "ramUsage": ram / 1024.0, "bottlenecks": thr})
            if len(history) > MAX_HISTORY:
                history.pop(0)
            if thr > 0:
                events.insert(0, f"Resource Contention: {thr} processes throttled at {ts}")
            else:
                if random.random() < 0.02:
                    events.insert(0, f"System Nominal at {ts}")
            if len(events) > 100:
                events = events[:100]

threading.Thread(target=sim_thread_fn, daemon=True).start()

# ---------- EMBEDDED HTML PAGE WITH HEIGHT FIX ----------
PAGE = r"""
<!doctype html>
<html>
<head>
  <meta charset="utf-8"/>
  <meta name="viewport" content="width=device-width, initial-scale=1"/>
  <title>WinOptima Resource Manager</title>

  <script src="https://cdn.tailwindcss.com"></script>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

  <style>
    body { font-family:'Segoe UI'; background:#f3f6fb; }
    .sidebar { width:250px; background:white; border-right:1px solid #e5e7eb; }
    .card { background:white; padding:14px; border-radius:12px; box-shadow:0 4px 12px rgba(0,0,0,0.04); }
    .mode-btn.selected { background:#eef2ff; color:#3730a3; }
    .bar-track { height:8px; background:#e5e7eb; border-radius:6px; overflow:hidden; }
    .bar-inner { height:100%; background:#3b82f6; }
    .proc-bar { height:8px; background:#e5e7eb; border-radius:6px; overflow:hidden; }
    .proc-bar-inner { height:100%; background:#3b82f6; }
    .event-item { background:#fff7ed; border:1px solid #fde2b6; padding:8px; border-radius:8px; margin-bottom:8px; color:#92400e; }
  </style>
</head>

<body class="flex">

<!-- Sidebar -->
<div class="sidebar p-4 flex flex-col">
  <div class="text-xl font-bold mb-6">WinOptima</div>

  <button class="p-2 mb-2 hover:bg-slate-100 rounded">Dashboard</button>
  <button class="p-2 mb-2 hover:bg-slate-100 rounded">Processes</button>
  <button class="p-2 mb-2 hover:bg-slate-100 rounded">Settings</button>

  <div class="mt-auto">
    <div class="text-xs text-slate-400 mb-2">ALLOCATION MODE</div>
    <div class="bg-slate-50 p-2 rounded mb-4">
      <button class="mode-btn selected p-2 w-full text-left mb-1" data-mode="Balanced">Balanced</button>
      <button class="mode-btn p-2 w-full text-left mb-1" data-mode="Performance">Performance</button>
      <button class="mode-btn p-2 w-full text-left" data-mode="Efficiency">Efficiency</button>
    </div>

    <div class="flex gap-2">
      <button id="pauseBtn" class="card w-1/2">Pause</button>
      <button id="resetBtn" class="card w-1/2">Reset</button>
    </div>
  </div>
</div>

<!-- Main -->
<div class="flex-1 p-6 overflow-auto">

  <div class="grid grid-cols-3 gap-4 mb-6">
    <div class="card">
      <div class="text-sm text-slate-500">CPU Load</div>
      <div id="cpuValue" class="text-3xl font-bold">0%</div>
      <div class="bar-track mt-2"><div id="cpuBar" class="bar-inner" style="width:0%"></div></div>
    </div>

    <div class="card">
      <div class="text-sm text-slate-500">Memory Usage</div>
      <div id="ramValue" class="text-3xl font-bold">0 GB</div>
      <div class="bar-track mt-2"><div id="ramBar" class="bar-inner" style="width:0%"></div></div>
    </div>

    <div class="card">
      <button id="analyzeBtn" class="p-2 border rounded w-full">Analyze State</button>
      <pre id="aiResult" class="text-xs mt-2"></pre>
    </div>
  </div>

  <!-- FIXED HEIGHT HISTORY SECTION (MATCHES YOUR SCREENSHOT) -->
  <div class="grid grid-cols-3 gap-4 mb-6">

    <div class="col-span-2 card" style="height:300px;">
      <div class="text-sm text-slate-500 mb-2">RESOURCE UTILIZATION HISTORY</div>
      <div style="height:240px;">
        <canvas id="historyChart"></canvas>
      </div>
    </div>

    <div class="card">
      <div class="text-sm text-slate-500 mb-2">SYSTEM EVENTS</div>
      <div id="eventsList" style="max-height:260px; overflow:auto"></div>
    </div>

  </div>

  <!-- Process Table -->
  <div class="card">
    <div class="text-sm mb-2">ACTIVE PROCESSES</div>
    <table class="w-full text-sm">
      <thead>
        <tr><th>Name</th><th>Priority</th><th>Usage</th><th>Status</th></tr>
      </thead>
      <tbody id="procTableBody"></tbody>
    </table>
  </div>

</div>

<!-- JS -->
<script>
const POLL_MS = 1000;
let chart = null;

function initChart(){
  const ctx = document.getElementById('historyChart');
  chart = new Chart(ctx, {
    type:'line',
    data:{ labels:[], datasets:[
      { label:'CPU %', data:[], borderColor:'#3b82f6', fill:true, backgroundColor:'rgba(59,130,246,0.1)', tension:0.3 },
      { label:'RAM (GB)', data:[], borderColor:'#6366f1', fill:true, backgroundColor:'rgba(99,102,241,0.1)', tension:0.3 }
    ]},
    options:{
      responsive:true,
      maintainAspectRatio:false,
      scales:{
        y:{ min:0, max:100 },
        y1:{ position:'right', min:0 }
      }
    }
  });
}

async function fetchState(){
  const res = await fetch('/api/state');
  const data = await res.json();
  applyState(data);
}

function applyState(data){
  const hist = data.history || [];
  const last = hist.length ? hist[hist.length-1] : null;

  if(last){
    document.getElementById('cpuValue').innerText = last.cpuUsage.toFixed(1)+'%';
    document.getElementById('cpuBar').style.width = Math.min(100,last.cpuUsage)+'%';

    document.getElementById('ramValue').innerText = last.ramUsage.toFixed(1)+' GB';
    document.getElementById('ramBar').style.width =
      Math.min(100,(last.ramUsage/16)*100)+'%';
  }

  // Events
  const el = document.getElementById('eventsList');
  el.innerHTML = '';
  (data.events||[]).slice(0,20).forEach(e=>{
    const d=document.createElement('div');
    d.className='event-item';
    d.innerText=e;
    el.appendChild(d);
  });

  // Processes
  const tb = document.getElementById('procTableBody');
  tb.innerHTML='';
  (data.programs||[]).forEach(p=>{
    const tr=document.createElement('tr');
    tr.innerHTML=`
      <td>${p.name}</td>
      <td>${p.priority}</td>
      <td>
        CPU ${p.cpuAllocated.toFixed(1)}% / ${p.cpuDemand.toFixed(1)}%
        <div class='proc-bar'><div class='proc-bar-inner' style='width:${Math.min(100,p.cpuAllocated)}%'></div></div>
        RAM ${Math.round(p.ramAllocated)} / ${Math.round(p.ramDemand)} MB
      </td>
      <td>${p.status}</td>`;
    tb.appendChild(tr);
  });

  // Chart update
  if(chart){
    chart.data.labels = hist.map(h=>h.time);
    chart.data.datasets[0].data = hist.map(h=>h.cpuUsage);
    chart.data.datasets[1].data = hist.map(h=>h.ramUsage);
    chart.update();
  }
}

async function send(action, extra={}){
  await fetch('/api/control',{
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body:JSON.stringify(Object.assign({action},extra))
  });
}

document.getElementById('pauseBtn').onclick = ()=>{
  const b = document.getElementById('pauseBtn');
  if(b.innerText==='Pause'){ b.innerText='Resume'; send('pause'); }
  else { b.innerText='Pause'; send('resume'); }
};

document.getElementById('resetBtn').onclick = ()=> send('reset');

document.querySelectorAll('.mode-btn').forEach(btn=>{
  btn.onclick = ()=>{
    document.querySelectorAll('.mode-btn').forEach(b=>b.classList.remove('selected'));
    btn.classList.add('selected');
    send('mode',{mode:btn.dataset.mode});
  };
});

document.getElementById('analyzeBtn').onclick = async ()=>{
  const res = await fetch('/api/analyze',{method:'POST'});
  const j = await res.json();
  document.getElementById('aiResult').innerText = j.analysis;
};

window.onload = ()=>{
  initChart();
  fetchState();
  setInterval(fetchState, POLL_MS);
};
</script>

</body>
</html>
"""

# ---------- HTTP SERVER ----------
class Handler(http.server.BaseHTTPRequestHandler):
    def _send(self, code, mime="text/html"):
        self.send_response(code)
        self.send_header("Content-Type", mime)
        self.send_header("Access-Control-Allow-Origin","*")
        self.end_headers()

    def do_GET(self):
        parsed = urlparse(self.path)
        if parsed.path in ("/", "/index", "/index.html"):
            self._send(200)
            self.wfile.write(PAGE.encode())
            return

        if parsed.path == "/api/state":
            with state_lock:
                data = {
                    "programs": programs,
                    "history": history,
                    "events": events[:50],
                    "mode": mode,
                    "is_running": is_running,
                    "total_threads": len(programs)*12,
                    "uptime_seconds": int(time.time()-start_time),
                }
            self._send(200,"application/json")
            self.wfile.write(json.dumps(data).encode())
            return

        self._send(404,"text/plain")
        self.wfile.write(b"Not Found")

    def do_POST(self):
        parsed = urlparse(self.path)
        if parsed.path == "/api/control":
            length = int(self.headers.get("Content-Length",0))
            body = self.rfile.read(length).decode()
            try: data = json.loads(body)
            except: data = {}

            global is_running, programs, mode, history, events
            action = data.get("action")
            with state_lock:
                if action=="pause": is_running=False
                elif action=="resume": is_running=True
                elif action=="reset":
                    programs = [dict(p) for p in INITIAL_PROGRAMS]
                    history.clear(); events.clear()
                elif action=="mode":
                    m = data.get("mode")
                    if m in ("Balanced","Performance","Efficiency"):
                        mode = m

            self._send(200,"application/json")
            self.wfile.write(json.dumps({"ok":True}).encode())
            return

        if parsed.path == "/api/analyze":
            a = analyze_system_state()
            self._send(200,"application/json")
            self.wfile.write(json.dumps({"analysis":a}).encode())
            return

        self._send(404,"text/plain")
        self.wfile.write(b"Not Found")


def run_server():
    with socketserver.ThreadingTCPServer(("", PORT), Handler) as httpd:
        print(f"Running at http://127.0.0.1:{PORT}")
        httpd.serve_forever()

if __name__ == "__main__":
    threading.Thread(target=lambda: (time.sleep(0.5), webbrowser.open(f"http://127.0.0.1:{PORT}")), daemon=True).start()
    run_server()