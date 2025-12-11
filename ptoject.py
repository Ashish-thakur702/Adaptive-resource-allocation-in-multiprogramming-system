"""
CyberDash+ v2.0
Adds:
 - Process Manager (list/search/kill)
 - Network Monitor (live upload/download + graph)
 - System Info page
 - Notifications (threshold alerts with popup & optional sound)
 - Dark/Light theme toggle
 - Animated Loading Screen
 - AI Assistant (local rule-based; optional OpenAI API entry for later)
Dependencies:
 - ttkbootstrap, psutil, matplotlib, pillow, requests, sounddevice (optional), numpy
"""

import tkinter as tk
from tkinter import ttk, messagebox, simpledialog
import ttkbootstrap as tb
from ttkbootstrap.constants import *
from PIL import Image, ImageTk
import requests
from io import BytesIO
import psutil
import platform
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
from collections import deque
import time
import threading
import os
import sys
import math

# optional sound for notifications
try:
    import numpy as np
    import sounddevice as sd
    SOUND_AVAILABLE = True
except Exception:
    SOUND_AVAILABLE = False

# ----------------- Config -----------------
DEFAULT_THEME = "darkly"
LIGHT_THEME = "morph"
SIDEBAR_EXPANDED_WIDTH = 260
SIDEBAR_COLLAPSED_WIDTH = 70

# ----------------- Helpers -----------------
def fetch_image(url, size=None, fallback_color="#222"):
    try:
        resp = requests.get(url, timeout=6)
        img = Image.open(BytesIO(resp.content)).convert("RGBA")
        if size:
            img = img.resize(size, Image.LANCZOS)
        return ImageTk.PhotoImage(img)
    except Exception:
        # make a blank image placeholder
        img = Image.new("RGBA", size or (64,64), fallback_color)
        return ImageTk.PhotoImage(img)

def beep(frequency=880, duration=0.15):
    if not SOUND_AVAILABLE:
        return
    fs = 44100
    t = np.linspace(0, duration, int(fs*duration), False)
    note = 0.2 * np.sin(frequency * t * 2 * math.pi)
    sd.play(note, fs)
    sd.wait()

# ----------------- Main Application -----------------
class CyberDashPlus:
    def __init__(self):
        self.style = tb.Style(theme=DEFAULT_THEME)
        self.root = self.style.master
        self.root.title("CyberDash+ — Enhanced")
        self.root.geometry("1200x720")
        self.root.configure(bg="#080808")

        # runtime state
        self.sidebar_collapsed = False
        self.stats_running = True
        self.network_prev = psutil.net_io_counters()
        self.net_lock = threading.Lock()
        self.max_points = 60
        self.cpu_hist = deque([0]*self.max_points, maxlen=self.max_points)
        self.ram_hist = deque([0]*self.max_points, maxlen=self.max_points)
        self.down_hist = deque([0]*self.max_points, maxlen=self.max_points)
        self.up_hist = deque([0]*self.max_points, maxlen=self.max_points)

        # notification settings
        self.alerts = {
            "cpu": 85.0,
            "ram": 90.0,
            "disk": 95.0,
            "enabled": True,
            "sound": SOUND_AVAILABLE
        }

        # optional OpenAI/API key (not used automatically)
        self.api_key = None

        # build UI
        self._show_loading_screen()
        self.root.after(1800, self._build_ui)  # show loading then build

    # ---------------- Loading Screen ----------------
    def _show_loading_screen(self):
        self.splash = tk.Toplevel(self.root)
        self.splash.overrideredirect(True)
        w = 600; h = 300
        x = (self.root.winfo_screenwidth() - w)//2
        y = (self.root.winfo_screenheight() - h)//2
        self.splash.geometry(f"{w}x{h}+{x}+{y}")
        self.splash.configure(bg="#000")
        lbl = tk.Label(self.splash, text="CyberDash+", font=("Segoe UI", 34, "bold"), fg="#ff2d2d", bg="#000")
        lbl.pack(pady=(40,10))
        sub = tk.Label(self.splash, text="Loading modules · Initializing neon cores · Syncing sensors", fg="#ddd", bg="#000", font=("Segoe UI", 10))
        sub.pack()
        pb = ttk.Progressbar(self.splash, length=480, bootstyle="danger-striped", maximum=100, mode="determinate")
        pb.pack(pady=20)
        def run_progress():
            for i in range(0,101,4):
                pb['value'] = i
                self.splash.update_idletasks()
                time.sleep(0.04)
            time.sleep(0.25)
            self.splash.destroy()
        threading.Thread(target=run_progress, daemon=True).start()

    # ---------------- Build main UI ----------------
    def _build_ui(self):
        # main container
        self.container = ttk.Frame(self.root)
        self.container.pack(fill="both", expand=True)

        # sidebar
        self.sidebar = tk.Frame(self.container, width=SIDEBAR_EXPANDED_WIDTH, bg="#070707")
        self.sidebar.pack(side="left", fill="y")
        self._build_sidebar()

        # main area
        self.main = tk.Frame(self.container, bg="#0b0b0b")
        self.main.pack(side="left", fill="both", expand=True)

        # header
        self.header = tk.Frame(self.main, height=64, bg="#0b0b0b")
        self.header.pack(fill="x")
        tk.Label(self.header, text="CyberDash+", font=("Segoe UI", 16, "bold"), fg="#ff3b3b", bg="#0b0b0b").pack(side="left", padx=18)
        self.clock_lbl = tk.Label(self.header, text="", fg="#ccc", bg="#0b0b0b", font=("Segoe UI", 10))
        self.clock_lbl.pack(side="right", padx=18)
        self._update_clock()

        # content area with pages
        self.content = tk.Frame(self.main, bg="#0b0b0b")
        self.content.pack(fill="both", expand=True, padx=12, pady=8)

        self.pages = {}
        for name in ("dashboard","analytics","processes","network","system","settings","assistant"):
            f = tk.Frame(self.content, bg="#0b0b0b")
            f.place(relx=0, rely=0, relwidth=1, relheight=1)
            self.pages[name] = f

        # build each page
        self._build_dashboard(self.pages["dashboard"])
        self._build_analytics(self.pages["analytics"])
        self._build_processes(self.pages["processes"])
        self._build_network(self.pages["network"])
        self._build_system(self.pages["system"])
        self._build_settings(self.pages["settings"])
        self._build_assistant(self.pages["assistant"])

        self.show_page("dashboard")

        # start background stats loop
        self.stats_thread = threading.Thread(target=self._stats_loop, daemon=True)
        self.stats_thread.start()

    # ---------------- Sidebar ----------------
    def _build_sidebar(self):
        logo = fetch_image("https://i.imgur.com/5u1YqvD.png", size=(56,56))
        tk.Label(self.sidebar, image=logo, bg="#070707").image = logo
        tk.Label(self.sidebar, text="CyberDash+", bg="#070707", fg="#ff4d4d", font=("Segoe UI", 12, "bold")).pack(pady=(4,12))

        buttons = [
            ("Dashboard","dashboard"),
            ("Analytics","analytics"),
            ("Processes","processes"),
            ("Network","network"),
            ("System Info","system"),
            ("Assistant","assistant"),
            ("Settings","settings"),
        ]
        for (t, p) in buttons:
            b = tk.Button(self.sidebar, text=f"  {t}", anchor="w", font=("Segoe UI",10),
                          fg="#fff", bg="#111", bd=0, relief="flat",
                          activebackground="#220000", command=lambda page=p: self.show_page(page))
            b.pack(fill="x", pady=6, padx=10)

        # bottom controls
        tk.Button(self.sidebar, text="Toggle Theme", font=("Segoe UI",9), fg="#fff", bg="#111", bd=0, command=self._toggle_theme).pack(side="bottom", fill="x", pady=8, padx=10)
        tk.Button(self.sidebar, text="Collapse Sidebar", font=("Segoe UI",9), fg="#fff", bg="#111", bd=0, command=self._toggle_sidebar).pack(side="bottom", fill="x", pady=6, padx=10)

    def _toggle_sidebar(self):
        if self.sidebar_collapsed:
            self.sidebar.config(width=SIDEBAR_EXPANDED_WIDTH)
            for w in self.sidebar.winfo_children():
                w.pack_configure(padx=10)
            self.sidebar_collapsed = False
        else:
            self.sidebar.config(width=SIDEBAR_COLLAPSED_WIDTH)
            for w in self.sidebar.winfo_children():
                w.pack_configure(padx=2)
            self.sidebar_collapsed = True

    # ---------------- Pages ----------------
    def show_page(self, name):
        for p in self.pages.values():
            p.place_forget()
        self.pages[name].place(relx=0, rely=0, relwidth=1, relheight=1)

    def _build_dashboard(self, frame):
        # summary cards and small graph
        left = tk.Frame(frame, bg="#0b0b0b")
        left.place(relx=0, rely=0, relwidth=0.32, relheight=1)

        def card(parent, title, var_text, y):
            outer = tk.Frame(parent, bg="#250000")
            outer.place(relx=0.04, rely=y, relwidth=0.92, height=90)
            inner = tk.Frame(outer, bg="#111")
            inner.pack(fill="both", expand=True, padx=6, pady=6)
            tk.Label(inner, text=title, fg="#cfcfcf", bg="#111", font=("Segoe UI",10)).pack(anchor="w")
            lbl = tk.Label(inner, text=var_text, fg="#ff3b3b", bg="#111", font=("Segoe UI",16,"bold"))
            lbl.pack(anchor="w")
            return lbl

        self.lbl_cpu = card(left, "CPU Usage", "0%", 0.05)
        self.lbl_ram = card(left, "RAM Usage", "0%", 0.25)
        self.lbl_procs = card(left, "Processes", "0", 0.45)
        self.lbl_net = card(left, "Network (D/U)", "0 / 0 KB/s", 0.65)

        # quick actions
        act = tk.Frame(left, bg="#0b0b0b")
        act.place(relx=0.04, rely=0.78, relwidth=0.92, relheight=0.18)
        tk.Button(act, text="Open Processes", command=lambda: self.show_page("processes"), bg="#111", fg="#fff", bd=0).pack(fill="x", pady=6)
        tk.Button(act, text="Open Network", command=lambda: self.show_page("network"), bg="#111", fg="#fff", bd=0).pack(fill="x")

        # right area: small graphs (CPU + RAM)
        right = tk.Frame(frame, bg="#0b0b0b")
        right.place(relx=0.32, rely=0, relwidth=0.68, relheight=1)
        fig = Figure(figsize=(6,4), dpi=100)
        self.ax1 = fig.add_subplot(211)
        self.ax2 = fig.add_subplot(212)
        self.ax1.set_ylim(0,100); self.ax2.set_ylim(0,100)
        self.ax1.set_title("CPU % (last 60 samples)")
        self.ax2.set_title("RAM % (last 60 samples)")
        self.line1, = self.ax1.plot([],[])
        self.line2, = self.ax2.plot([],[])
        self.canvas_dashboard = FigureCanvasTkAgg(fig, master=right)
        self.canvas_dashboard.get_tk_widget().pack(fill="both", expand=True, padx=12, pady=12)

    def _build_analytics(self, frame):
        # bigger analytics page (reuse data)
        fig = Figure(figsize=(7,4), dpi=100)
        ax_cpu = fig.add_subplot(311)
        ax_ram = fig.add_subplot(312)
        ax_net = fig.add_subplot(313)
        ax_cpu.set_ylim(0,100); ax_ram.set_ylim(0,100)
        ax_cpu.set_title("CPU %")
        ax_ram.set_title("RAM %")
        ax_net.set_title("Network KB/s")
        self.a_cpu_line, = ax_cpu.plot([],[])
        self.a_ram_line, = ax_ram.plot([],[])
        self.a_down_line, = ax_net.plot([],[], label="down")
        self.a_up_line, = ax_net.plot([],[], label="up")
        ax_net.legend()
        canvas = FigureCanvasTkAgg(fig, master=frame)
        canvas.get_tk_widget().pack(fill="both", expand=True, padx=12, pady=12)
        self.analytics_canvas = canvas
        self.analytics_axes = (ax_cpu, ax_ram, ax_net)

    def _build_processes(self, frame):
        # process manager table + controls
        top = tk.Frame(frame, bg="#0b0b0b")
        top.pack(fill="x", padx=12, pady=8)
        tk.Label(top, text="Process Manager", fg="#ff3b3b", bg="#0b0b0b", font=("Segoe UI",14,"bold")).pack(side="left")
        tk.Button(top, text="Refresh", bg="#111", fg="#fff", bd=0, command=self._refresh_proc).pack(side="right", padx=6)
        tk.Button(top, text="Kill Selected", bg="#770000", fg="#fff", bd=0, command=self._kill_selected_proc).pack(side="right", padx=6)
        self.proc_search_var = tk.StringVar()
        tk.Entry(top, textvariable=self.proc_search_var).pack(side="right", padx=6)
        tk.Label(top, text="Search:", bg="#0b0b0b", fg="#ddd").pack(side="right")

        columns = ("pid","name","cpu","memory","status")
        tree = ttk.Treeview(frame, columns=columns, show="headings")
        for c in columns:
            tree.heading(c, text=c.upper())
            tree.column(c, anchor="center")
        tree.pack(fill="both", expand=True, padx=12, pady=6)
        self.proc_tree = tree
        self._refresh_proc()

    def _refresh_proc(self):
        query = self.proc_search_var.get().lower().strip()
        for i in self.proc_tree.get_children():
            self.proc_tree.delete(i)
        for p in psutil.process_iter(['pid','name','cpu_percent','memory_percent','status']):
            try:
                info = p.info
                name = info.get('name') or ""
                pid = info.get('pid')
                if query and query not in name.lower() and query not in str(pid):
                    continue
                cpu = info.get('cpu_percent') or 0.0
                mem = info.get('memory_percent') or 0.0
                status = info.get('status') or ""
                self.proc_tree.insert("", "end", values=(pid, name, f"{cpu:.1f}%", f"{mem:.1f}%", status))
            except Exception:
                continue

    def _kill_selected_proc(self):
        sel = self.proc_tree.selection()
        if not sel:
            messagebox.showinfo("Select", "Select a process row first.")
            return
        pid = int(self.proc_tree.item(sel[0])['values'][0])
        try:
            p = psutil.Process(pid)
            p.terminate()
            messagebox.showinfo("Terminated", f"Process {pid} terminated.")
            self._refresh_proc()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to kill {pid}: {e}")

    def _build_network(self, frame):
        top = tk.Frame(frame, bg="#0b0b0b")
        top.pack(fill="x", padx=12, pady=8)
        tk.Label(top, text="Network Monitor", fg="#ff3b3b", bg="#0b0b0b", font=("Segoe UI",14,"bold")).pack(side="left")
        self.net_label = tk.Label(top, text="Down: 0 KB/s  Up: 0 KB/s", bg="#0b0b0b", fg="#ccc")
        self.net_label.pack(side="right", padx=12)

        fig = Figure(figsize=(7,4), dpi=100)
        ax = fig.add_subplot(111)
        ax.set_title("Network (KB/s)")
        ax.set_ylim(0, 1024)  # dynamic later
        self.net_ax = ax
        self.net_line_down, = ax.plot([],[], label="down")
        self.net_line_up, = ax.plot([],[], label="up")
        ax.legend()
        self.net_canvas = FigureCanvasTkAgg(fig, master=frame)
        self.net_canvas.get_tk_widget().pack(fill="both", expand=True, padx=12, pady=12)

    def _build_system(self, frame):
        # display system info
        tk.Label(frame, text="System Information", fg="#ff3b3b", bg="#0b0b0b", font=("Segoe UI",14,"bold")).pack(anchor="w", padx=12, pady=8)
        info_frame = tk.Frame(frame, bg="#0b0b0b")
        info_frame.pack(fill="both", expand=True, padx=12, pady=8)
        left = tk.Frame(info_frame, bg="#0b0b0b")
        left.pack(side="left", fill="y", padx=8)
        right = tk.Frame(info_frame, bg="#0b0b0b")
        right.pack(side="left", fill="both", expand=True, padx=8)

        # collect info
        uname = platform.uname()
        cpu_count = psutil.cpu_count(logical=True)
        total_ram = round(psutil.virtual_memory().total / (1024**3), 2)
        boot = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(psutil.boot_time()))
        battery = psutil.sensors_battery() if hasattr(psutil, "sensors_battery") else None

        items = [
            ("System", f"{uname.system} {uname.release}"),
            ("Node", uname.node),
            ("Machine", uname.machine),
            ("Processor", uname.processor or 'N/A'),
            ("CPU Cores", str(cpu_count)),
            ("Total RAM (GB)", str(total_ram)),
            ("Boot Time", boot),
        ]
        for t,v in items:
            tk.Label(left, text=f"{t}:", fg="#ccc", bg="#0b0b0b", font=("Segoe UI",10,"bold")).pack(anchor="w", pady=6)
            tk.Label(right, text=v, fg="#ddd", bg="#0b0b0b", font=("Segoe UI",10)).pack(anchor="w", pady=6)

        if battery:
            tk.Label(left, text="Battery:", fg="#ccc", bg="#0b0b0b", font=("Segoe UI",10,"bold")).pack(anchor="w", pady=6)
            tk.Label(right, text=f"{battery.percent}% - {'Plugged In' if battery.power_plugged else 'On Battery'}", fg="#ddd", bg="#0b0b0b", font=("Segoe UI",10)).pack(anchor="w", pady=6)

    def _build_settings(self, frame):
        tk.Label(frame, text="Settings", fg="#ff3b3b", bg="#0b0b0b", font=("Segoe UI",14,"bold")).pack(anchor="w", padx=12, pady=8)
        f = tk.Frame(frame, bg="#0b0b0b")
        f.pack(fill="x", padx=12, pady=6)
        # thresholds
        tk.Label(f, text="CPU Alert (%)", bg="#0b0b0b", fg="#ddd").grid(row=0, column=0, sticky="w", padx=6, pady=6)
        self.var_cpu = tk.IntVar(value=int(self.alerts["cpu"]))
        tk.Entry(f, textvariable=self.var_cpu, width=6).grid(row=0, column=1, padx=6)
        tk.Label(f, text="RAM Alert (%)", bg="#0b0b0b", fg="#ddd").grid(row=1, column=0, sticky="w", padx=6, pady=6)
        self.var_ram = tk.IntVar(value=int(self.alerts["ram"]))
        tk.Entry(f, textvariable=self.var_ram, width=6).grid(row=1, column=1, padx=6)
        tk.Checkbutton(f, text="Enable Alerts", variable=tk.BooleanVar(value=self.alerts["enabled"]), command=self._toggle_alerts).grid(row=2, column=0, columnspan=2, pady=8)
        tk.Button(f, text="Save", command=self._save_settings, bg="#111", fg="#fff").grid(row=3, column=0, columnspan=2, pady=8)

        # OpenAI key field (optional)
        tk.Label(frame, text="AI Assistant (optional)", fg="#ff3b3b", bg="#0b0b0b", font=("Segoe UI",12,"bold")).pack(anchor="w", padx=12, pady=(12,4))
        ai_f = tk.Frame(frame, bg="#0b0b0b")
        ai_f.pack(fill="x", padx=12)
        tk.Label(ai_f, text="Paste OpenAI API key (optional):", fg="#ddd", bg="#0b0b0b").pack(side="left")
        self.var_apikey = tk.StringVar(value=self.api_key or "")
        tk.Entry(ai_f, textvariable=self.var_apikey, width=40).pack(side="left", padx=6)
        tk.Button(ai_f, text="Save Key", command=self._save_api_key, bg="#111", fg="#fff").pack(side="left", padx=6)

    def _build_assistant(self, frame):
        tk.Label(frame, text="Assistant", fg="#ff3b3b", bg="#0b0b0b", font=("Segoe UI",14,"bold")).pack(anchor="w", padx=12, pady=8)
        conv = tk.Text(frame, bg="#0b0b0b", fg="#ddd", height=18)
        conv.pack(fill="both", expand=True, padx=12, pady=6)
        self.assistant_conv = conv
        entry_fr = tk.Frame(frame, bg="#0b0b0b")
        entry_fr.pack(fill="x", padx=12, pady=6)
        self.assist_var = tk.StringVar()
        tk.Entry(entry_fr, textvariable=self.assist_var).pack(side="left", fill="x", expand=True, padx=6)
        tk.Button(entry_fr, text="Send", command=self._assistant_query, bg="#111", fg="#fff").pack(side="left", padx=6)
        # initial message
        self.assistant_conv.insert("end", "Assistant: Hi Kamil — ask me about CPU, RAM, processes, network, or general tips.\n")

    # ---------------- Stats loop ----------------
    def _stats_loop(self):
        while True:
            try:
                cpu = psutil.cpu_percent(interval=0.5)
                mem = psutil.virtual_memory().percent
                proc_count = len(psutil.pids())
                # network
                with self.net_lock:
                    c