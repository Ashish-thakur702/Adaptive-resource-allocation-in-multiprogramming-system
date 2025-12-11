<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Adaptive Resource Allocation in Multiprogramming System</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }

        :root {
            --primary-color: #2c3e50;
            --secondary-color: #3498db;
            --accent-color: #e74c3c;
            --success-color: #27ae60;
            --warning-color: #f39c12;
            --light-bg: #ecf0f1;
            --dark-bg: #2c3e50;
            --text-color: #333;
            --light-text: #f8f9fa;
        }

        body {
            background-color: #f5f7fa;
            color: var(--text-color);
            line-height: 1.6;
        }

        .container {
            max-width: 1400px;
            margin: 0 auto;
            padding: 20px;
        }

        header {
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            color: white;
            padding: 25px 0;
            border-radius: 10px;
            margin-bottom: 30px;
            box-shadow: 0 5px 15px rgba(0, 0, 0, 0.1);
        }

        .header-content {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 0 30px;
        }

        h1 {
            font-size: 2.5rem;
            margin-bottom: 10px;
        }

        .subtitle {
            font-size: 1.2rem;
            opacity: 0.9;
        }

        .logo {
            font-size: 3rem;
            color: white;
        }

        .main-content {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 30px;
            margin-bottom: 30px;
        }

        .card {
            background: white;
            border-radius: 10px;
            padding: 25px;
            box-shadow: 0 5px 15px rgba(0, 0, 0, 0.05);
            transition: transform 0.3s ease;
        }

        .card:hover {
            transform: translateY(-5px);
        }

        .card-title {
            color: var(--primary-color);
            border-bottom: 2px solid var(--light-bg);
            padding-bottom: 10px;
            margin-bottom: 20px;
            font-size: 1.5rem;
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .card-title i {
            color: var(--secondary-color);
        }

        .control-panel {
            grid-column: span 2;
        }

        .process-controls, .resource-controls {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
            gap: 15px;
            margin-bottom: 20px;
        }

        .control-group {
            display: flex;
            flex-direction: column;
        }

        label {
            margin-bottom: 8px;
            font-weight: 600;
            color: var(--primary-color);
        }

        input, select, button {
            padding: 12px 15px;
            border: 1px solid #ddd;
            border-radius: 5px;
            font-size: 1rem;
        }

        input:focus, select:focus {
            outline: none;
            border-color: var(--secondary-color);
        }

        button {
            background-color: var(--secondary-color);
            color: white;
            border: none;
            cursor: pointer;
            transition: all 0.3s ease;
            font-weight: 600;
        }

        button:hover {
            background-color: var(--primary-color);
            transform: scale(1.03);
        }

        .btn-danger {
            background-color: var(--accent-color);
        }

        .btn-success {
            background-color: var(--success-color);
        }

        .btn-warning {
            background-color: var(--warning-color);
        }

        .button-group {
            display: flex;
            gap: 10px;
            margin-top: 20px;
        }

        .process-list, .resource-list {
            margin-top: 15px;
        }

        .process-item, .resource-item {
            background: var(--light-bg);
            padding: 12px 15px;
            border-radius: 5px;
            margin-bottom: 10px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-left: 4px solid var(--secondary-color);
        }

        .process-item.completed {
            border-left-color: var(--success-color);
            opacity: 0.8;
        }

        .process-item.waiting {
            border-left-color: var(--warning-color);
        }

        .process-item.terminated {
            border-left-color: var(--accent-color);
            opacity: 0.6;
        }

        .resource-item {
            border-left-color: var(--primary-color);
        }

        .process-info, .resource-info {
            display: flex;
            flex-direction: column;
            gap: 5px;
        }

        .process-name {
            font-weight: 600;
            color: var(--primary-color);
        }

        .process-status {
            font-size: 0.85rem;
            padding: 3px 10px;
            border-radius: 20px;
            background: #e0e0e0;
            display: inline-block;
            width: fit-content;
        }

        .status-running {
            background-color: rgba(52, 152, 219, 0.2);
            color: var(--secondary-color);
        }

        .status-waiting {
            background-color: rgba(243, 156, 18, 0.2);
            color: var(--warning-color);
        }

        .status-completed {
            background-color: rgba(39, 174, 96, 0.2);
            color: var(--success-color);
        }

        .status-terminated {
            background-color: rgba(231, 76, 60, 0.2);
            color: var(--accent-color);
        }

        .charts-container {
            grid-column: span 2;
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 30px;
        }

        .chart-container {
            height: 300px;
            position: relative;
        }

        .system-metrics {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
            gap: 20px;
            margin-top: 20px;
        }

        .metric-card {
            background: white;
            border-radius: 10px;
            padding: 20px;
            text-align: center;
            box-shadow: 0 3px 10px rgba(0, 0, 0, 0.08);
        }

        .metric-value {
            font-size: 2.2rem;
            font-weight: 700;
            color: var(--primary-color);
            margin: 10px 0;
        }

        .metric-label {
            font-size: 0.9rem;
            color: #7f8c8d;
            text-transform: uppercase;
            letter-spacing: 1px;
        }

        .log-container {
            max-height: 300px;
            overflow-y: auto;
            margin-top: 15px;
            padding: 10px;
            background: #f8f9fa;
            border-radius: 5px;
            border: 1px solid #eee;
        }

        .log-entry {
            padding: 8px 10px;
            border-bottom: 1px solid #eee;
            font-size: 0.9rem;
            font-family: monospace;
        }

        .log-entry:nth-child(even) {
            background-color: #f2f2f2;
        }

        .log-time {
            color: #7f8c8d;
            margin-right: 10px;
        }

        .log-info {
            color: var(--secondary-color);
        }

        .log-warning {
            color: var(--warning-color);
        }

        .log-success {
            color: var(--success-color);
        }

        .log-error {
            color: var(--accent-color);
        }

        footer {
            text-align: center;
            padding: 25px;
            margin-top: 40px;
            color: #7f8c8d;
            border-top: 1px solid #eee;
        }

        .algorithm-info {
            margin-top: 20px;
            padding: 15px;
            background: rgba(52, 152, 219, 0.1);
            border-radius: 5px;
            border-left: 4px solid var(--secondary-color);
        }

        .responsive-table {
            overflow-x: auto;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
        }

        th, td {
            padding: 12px 15px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }

        th {
            background-color: var(--primary-color);
            color: white;
        }

        tr:hover {
            background-color: #f5f5f5;
        }

        .deadlock-warning {
            background-color: rgba(231, 76, 60, 0.1);
            border-left: 4px solid var(--accent-color);
            padding: 10px;
            border-radius: 5px;
            margin-top: 10px;
            display: none;
        }

        .performance-stats {
            display: flex;
            justify-content: space-between;
            margin-top: 15px;
            padding-top: 15px;
            border-top: 1px solid #eee;
        }

        .stat-item {
            text-align: center;
        }

        .stat-value {
            font-size: 1.5rem;
            font-weight: bold;
            color: var(--primary-color);
        }

        .stat-label {
            font-size: 0.8rem;
            color: #7f8c8d;
        }

        .modal {
            display: none;
            position: fixed;
            z-index: 1000;
            left: 0;
            top: 0;
            width: 100%;
            height: 100%;
            background-color: rgba(0,0,0,0.5);
        }

        .modal-content {
            background-color: white;
            margin: 5% auto;
            padding: 30px;
            border-radius: 10px;
            width: 80%;
            max-width: 600px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
        }

        .close-modal {
            float: right;
            font-size: 28px;
            cursor: pointer;
        }

        @media (max-width: 1100px) {
            .main-content, .charts-container {
                grid-template-columns: 1fr;
            }
            
            .control-panel {
                grid-column: span 1;
            }
        }

        @media (max-width: 768px) {
            .header-content {
                flex-direction: column;
                text-align: center;
                gap: 20px;
            }
            
            .process-controls, .resource-controls {
                grid-template-columns: 1fr;
            }
            
            .button-group {
                flex-direction: column;
            }
            
            h1 {
                font-size: 2rem;
            }
        }
    </style>
</head>
<body>
    <!-- Algorithm Explanation Modal -->
    <div id="algorithmModal" class="modal">
        <div class="modal-content">
            <span class="close-modal">&times;</span>
            <h2>Algorithm Details</h2>
            <div id="algorithmDetails">
                <!-- Content will be populated dynamically -->
            </div>
        </div>
    </div>

    <div class="container">
        <header>
            <div class="header-content">
                <div>
                    <h1><i class="fas fa-microchip logo"></i> Adaptive Resource Allocation</h1>
                    <p class="subtitle">Multiprogramming System Simulation</p>
                    <p>Simulating dynamic resource allocation with adaptive algorithms</p>
                </div>
                <div class="system-time">
                    <h3 id="simulation-time">Time: 0s</h3>
                    <p>CPU Utilization: <span id="cpu-util">0%</span></p>
                    <p>Processes: <span id="total-processes">0</span></p>
                </div>
            </div>
        </header>

        <div class="main-content">
            <div class="card control-panel">
                <h2 class="card-title"><i class="fas fa-sliders-h"></i> Control Panel</h2>
                
                <div class="process-controls">
                    <div class="control-group">
                        <label for="process-count">Number of Processes</label>
                        <input type="number" id="process-count" min="1" max="20" value="5">
                    </div>
                    
                    <div class="control-group">
                        <label for="algorithm">Allocation Algorithm</label>
                        <select id="algorithm">
                            <option value="adaptive">Adaptive (Priority + Need)</option>
                            <option value="roundrobin">Round Robin</option>
                            <option value="priority">Priority Based</option>
                            <option value="fcfs">First Come First Serve</option>
                            <option value="multilevel">Multilevel Queue</option>
                        </select>
                    </div>
                    
                    <div class="control-group">
                        <label for="time-slice">Time Quantum (ms)</label>
                        <input type="number" id="time-slice" min="100" max="2000" value="500">
                    </div>

                    <div class="control-group">
                        <label for="auto-add">Auto Add Process Every</label>
                        <select id="auto-add">
                            <option value="0">Off</option>
                            <option value="5">5 Seconds</option>
                            <option value="10">10 Seconds</option>
                            <option value="15">15 Seconds</option>
                        </select>
                    </div>
                </div>
                
                <div class="resource-controls">
                    <div class="control-group">
                        <label for="cpu-count">CPU Cores</label>
                        <input type="number" id="cpu-count" min="1" max="8" value="2">
                    </div>
                    
                    <div class="control-group">
                        <label for="memory-size">Memory (MB)</label>
                        <input type="number" id="memory-size" min="100" max="2000" value="512">
                    </div>
                    
                    <div class="control-group">
                        <label for="io-devices">I/O Devices</label>
                        <input type="number" id="io-devices" min="1" max="10" value="3">
                    </div>

                    <div class="control-group">
                        <label for="deadlock-detection">Deadlock Detection</label>
                        <select id="deadlock-detection">
                            <option value="on">Enabled</option>
                            <option value="off">Disabled</option>
                        </select>
                    </div>
                </div>
                
                <div class="button-group">
                    <button id="start-btn" class="btn-success"><i class="fas fa-play"></i> Start Simulation</button>
                    <button id="pause-btn" class="btn-warning"><i class="fas fa-pause"></i> Pause</button>
                    <button id="reset-btn" class="btn-danger"><i class="fas fa-redo"></i> Reset</button>
                    <button id="add-process-btn"><i class="fas fa-plus-circle"></i> Add Process</button>
                    <button id="algo-info-btn"><i class="fas fa-info-circle"></i> Algorithm Info</button>
                </div>

                <div class="deadlock-warning" id="deadlock-warning">
                    <i class="fas fa-exclamation-triangle"></i> 
                    <strong>Deadlock Detected!</strong> Some processes are waiting indefinitely. 
                    Consider terminating some processes or increasing resources.
                </div>
            </div>

            <div class="card">
                <h2 class="card-title"><i class="fas fa-tasks"></i> Active Processes</h2>
                <div class="process-list" id="process-list">
                    <!-- Process items will be dynamically added here -->
                </div>
                <div class="algorithm-info">
                    <p><strong>Current Algorithm:</strong> <span id="current-algorithm">Adaptive (Priority + Need)</span></p>
                    <p>This algorithm dynamically adjusts resource allocation based on process priority and resource needs.</p>
                </div>
            </div>

            <div class="card">
                <h2 class="card-title"><i class="fas fa-server"></i> System Resources</h2>
                <div class="resource-list" id="resource-list">
                    <!-- Resource items will be dynamically added here -->
                </div>
                
                <div class="system-metrics">
                    <div class="metric-card">
                        <div class="metric-label">CPU Utilization</div>
                        <div class="metric-value" id="metric-cpu">0%</div>
                    </div>
                    <div class="metric-card">
                        <div class="metric-label">Memory Usage</div>
                        <div class="metric-value" id="metric-memory">0%</div>
                    </div>
                    <div class="metric-card">
                        <div class="metric-label">Throughput</div>
                        <div class="metric-value" id="metric-throughput">0/min</div>
                    </div>
                    <div class="metric-card">
                        <div class="metric-label">Avg Wait Time</div>
                        <div class="metric-value" id="metric-wait">0s</div>
                    </div>
                </div>

                <div class="performance-stats">
                    <div class="stat-item">
                        <div class="stat-value" id="stat-context-switch">0</div>
                        <div class="stat-label">Context Switches</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="stat-page-faults">0</div>
                        <div class="stat-label">Page Faults</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="stat-deadlocks">0</div>
                        <div class="stat-label">Deadlocks</div>
                    </div>
                </div>
            </div>

            <div class="card">
                <h2 class="card-title"><i class="fas fa-history"></i> System Log</h2>
                <div class="log-container" id="system-log">
                    <!-- Log entries will be dynamically added here -->
                </div>
                <div style="margin-top: 10px;">
                    <button id="clear-log-btn" style="padding: 5px 10px; font-size: 0.9rem;">
                        <i class="fas fa-trash-alt"></i> Clear Log
                    </button>
                </div>
            </div>
        </div>

        <div class="charts-container">
            <div class="card">
                <h2 class="card-title"><i class="fas fa-chart-line"></i> Resource Utilization Over Time</h2>
                <div class="chart-container">
                    <canvas id="resource-chart"></canvas>
                </div>
            </div>
            
            <div class="card">
                <h2 class="card-title"><i class="fas fa-chart-bar"></i> Process Status Distribution</h2>
                <div class="chart-container">
                    <canvas id="process-chart"></canvas>
                </div>
            </div>
        </div>

        <div class="card">
            <h2 class="card-title"><i class="fas fa-table"></i> Allocation Table</h2>
            <div class="responsive-table">
                <table id="allocation-table">
                    <thead>
                        <tr>
                            <th>PID</th>
                            <th>Process Name</th>
                            <th>Status</th>
                            <th>Priority</th>
                            <th>CPU Allocated</th>
                            <th>Memory Allocated</th>
                            <th>I/O Allocated</th>
                            <th>Wait Time</th>
                            <th>Turnaround Time</th>
                        </tr>
                    </thead>
                    <tbody id="allocation-table-body">
                        <!-- Table rows will be dynamically added here -->
                    </tbody>
                </table>
            </div>
        </div>

        <!-- Project Documentation Section -->
        <div class="card">
            <h2 class="card-title"><i class="fas fa-file-alt"></i> Project Documentation</h2>
            <div style="margin-top: 15px;">
                <h3>Project Overview</h3>
                <p>This simulation demonstrates adaptive resource allocation in a multiprogramming environment. 
                It models how an operating system manages CPU cores, memory, and I/O devices among multiple processes 
                using various scheduling algorithms.</p>
                
                <h3 style="margin-top: 20px;">Key Features</h3>
                <ul style="margin-left: 20px; margin-top: 10px;">
                    <li>Multiple scheduling algorithms (Adaptive, Round Robin, Priority, FCFS)</li>
                    <li>Dynamic process creation and termination</li>
                    <li>Real-time resource utilization visualization</li>
                    <li>Deadlock detection and prevention mechanisms</li>
                    <li>Performance metrics calculation</li>
                    <li>Interactive control panel</li>
                </ul>
                
                <h3 style="margin-top: 20px;">Technologies Used</h3>
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 10px;">
                    <div><strong>Frontend:</strong> HTML5, CSS3, JavaScript (ES6+)</div>
                    <div><strong>Visualization:</strong> Chart.js</div>
                    <div><strong>Icons:</strong> Font Awesome</div>
                    <div><strong>Version Control:</strong> Git & GitHub</div>
                </div>
            </div>
        </div>

        <footer>
            <p>Adaptive Resource Allocation in Multiprogramming System | CSE 316 Operating Systems Project</p>
            <p>Simulation uses adaptive algorithms to optimize CPU, memory, and I/O allocation among multiple processes</p>
            <p>© 2023 Lovely Professional University | School of Computer Science and Engineering</p>
        </footer>
    </div>

    <script>
        // ==============================
        // GLOBAL VARIABLES AND CONSTANTS
        // ==============================
        let simulationTime = 0;
        let simulationInterval;
        let isRunning = false;
        let processes = [];
        let resources = {
            cpu: 2,
            memory: 512,
            io: 3,
            allocatedCPU: 0,
            allocatedMemory: 0,
            allocatedIO: 0
        };
        let resourceHistory = [];
        let processStatusCounts = { running: 0, waiting: 0, completed: 0, terminated: 0 };
        let completedProcesses = 0;
        let totalWaitTime = 0;
        let totalTurnaroundTime = 0;
        let contextSwitches = 0;
        let pageFaults = 0;
        let deadlockCount = 0;
        let autoAddInterval;
        
        // Chart instances
        let resourceChart, processChart;
        
        // DOM elements
        const processCountInput = document.getElementById('process-count');
        const algorithmSelect = document.getElementById('algorithm');
        const timeSliceInput = document.getElementById('time-slice');
        const cpuCountInput = document.getElementById('cpu-count');
        const memorySizeInput = document.getElementById('memory-size');
        const ioDevicesInput = document.getElementById('io-devices');
        const autoAddSelect = document.getElementById('auto-add');
        const deadlockDetectionSelect = document.getElementById('deadlock-detection');
        const startBtn = document.getElementById('start-btn');
        const pauseBtn = document.getElementById('pause-btn');
        const resetBtn = document.getElementById('reset-btn');
        const addProcessBtn = document.getElementById('add-process-btn');
        const algoInfoBtn = document.getElementById('algo-info-btn');
        const clearLogBtn = document.getElementById('clear-log-btn');
        const processList = document.getElementById('process-list');
        const resourceList = document.getElementById('resource-list');
        const systemLog = document.getElementById('system-log');
        const simulationTimeElement = document.getElementById('simulation-time');
        const cpuUtilElement = document.getElementById('cpu-util');
        const totalProcessesElement = document.getElementById('total-processes');
        const allocationTableBody = document.getElementById('allocation-table-body');
        const currentAlgorithmElement = document.getElementById('current-algorithm');
        const deadlockWarningElement = document.getElementById('deadlock-warning');
        const algorithmModal = document.getElementById('algorithmModal');
        const closeModal = document.querySelector('.close-modal');
        const algorithmDetails = document.getElementById('algorithmDetails');
        
        // Metric elements
        const metricCpuElement = document.getElementById('metric-cpu');
        const metricMemoryElement = document.getElementById('metric-memory');
        const metricThroughputElement = document.getElementById('metric-throughput');
        const metricWaitElement = document.getElementById('metric-wait');
        
        // Stat elements
        const statContextSwitch = document.getElementById('stat-context-switch');
        const statPageFaults = document.getElementById('stat-page-faults');
        const statDeadlocks = document.getElementById('stat-deadlocks');
        
        // Process names for random generation
        const processNames = [
            "System Init", "User Interface", "Data Processor", "File Handler", 
            "Network Service", "Security Check", "Backup Task", "Update Service",
            "Multimedia Player", "Print Spooler", "Antivirus Scan", "Database Query",
            "Web Server", "Email Client", "Compilation Job", "Rendering Task"
        ];
        
        // Process types with different characteristics
        const processTypes = [
            { name: "CPU Intensive", cpuMultiplier: 2, memoryMultiplier: 0.5, ioMultiplier: 0.3 },
            { name: "Memory Intensive", cpuMultiplier: 0.5, memoryMultiplier: 2, ioMultiplier: 0.5 },
            { name: "I/O Intensive", cpuMultiplier: 0.3, memoryMultiplier: 0.7, ioMultiplier: 2 },
            { name: "Balanced", cpuMultiplier: 1, memoryMultiplier: 1, ioMultiplier: 1 }
        ];
        
        // ==============================
        // INITIALIZATION FUNCTIONS
        // ==============================
        
        // Initialize the simulation
        function initSimulation() {
            clearInterval(simulationInterval);
            clearInterval(autoAddInterval);
            simulationTime = 0;
            processes = [];
            completedProcesses = 0;
            totalWaitTime = 0;
            totalTurnaroundTime = 0;
            contextSwitches = 0;
            pageFaults = 0;
            deadlockCount = 0;
            
            // Set resources based on inputs
            resources = {
                cpu: parseInt(cpuCountInput.value),
                memory: parseInt(memorySizeInput.value),
                io: parseInt(ioDevicesInput.value),
                allocatedCPU: 0,
                allocatedMemory: 0,
                allocatedIO: 0
            };
            
            // Generate initial processes
            const processCount = parseInt(processCountInput.value);
            for (let i = 0; i < processCount; i++) {
                addRandomProcess();
            }
            
            // Initialize resource history
            resourceHistory = [];
            updateResourceHistory();
            
            // Update UI
            updateProcessList();
            updateResourceList();
            updateAllocationTable();
            updateMetrics();
            updateStats();
            updateCharts();
            clearLog();
            addLog("System initialized with " + processCount + " processes", "info");
            
            // Update simulation time display
            simulationTimeElement.textContent = "Time: " + simulationTime + "s";
            totalProcessesElement.textContent = processes.length;
            
            // Hide deadlock warning
            deadlockWarningElement.style.display = 'none';
        }
        
        // ==============================
        // PROCESS MANAGEMENT FUNCTIONS
        // ==============================
        
        // Add a random process
        function addRandomProcess() {
            const processType = processTypes[Math.floor(Math.random() * processTypes.length)];
            const name = processNames[Math.floor(Math.random() * processNames.length)];
            const priority = Math.floor(Math.random() * 10) + 1; // 1-10
            const cpuNeed = Math.min(Math.floor(Math.random() * resources.cpu * processType.cpuMultiplier) + 1, resources.cpu);
            const memoryNeed = Math.min(Math.floor(Math.random() * (resources.memory / 4) * processType.memoryMultiplier) + 50, resources.memory);
            const ioNeed = Math.min(Math.floor(Math.random() * resources.io * processType.ioMultiplier) + 1, resources.io);
            const burstTime = Math.floor(Math.random() * 10) + 3; // 3-12 seconds
            
            const process = {
                id: processes.length + 1,
                name: `${name} (${processType.name})`,
                type: processType.name,
                priority: priority,
                cpuNeed: cpuNeed,
                memoryNeed: memoryNeed,
                ioNeed: ioNeed,
                cpuAllocated: 0,
                memoryAllocated: 0,
                ioAllocated: 0,
                burstTime: burstTime,
                remainingTime: burstTime,
                status: "waiting",
                waitTime: 0,
                arrivalTime: simulationTime,
                completionTime: null,
                pageFaults: 0
            };
            
            processes.push(process);
            addLog(`Process ${process.id} (${process.name}) created with priority ${priority}`, "info");
            updateTotalProcessesDisplay();
        }
        
        // Add a custom process
        function addCustomProcess() {
            const processType = processTypes[Math.floor(Math.random() * processTypes.length)];
            const name = "Custom Process " + (processes.length + 1);
            const priority = Math.floor(Math.random() * 10) + 1;
            const cpuNeed = Math.min(Math.floor(Math.random() * resources.cpu) + 1, resources.cpu);
            const memoryNeed = Math.min(Math.floor(Math.random() * 100) + 50, resources.memory);
            const ioNeed = Math.min(Math.floor(Math.random() * resources.io) + 1, resources.io);
            const burstTime = Math.floor(Math.random() * 8) + 2;
            
            const process = {
                id: processes.length + 1,
                name: `${name} (${processType.name})`,
                type: processType.name,
                priority: priority,
                cpuNeed: cpuNeed,
                memoryNeed: memoryNeed,
                ioNeed: ioNeed,
                cpuAllocated: 0,
                memoryAllocated: 0,
                ioAllocated: 0,
                burstTime: burstTime,
                remainingTime: burstTime,
                status: "waiting",
                waitTime: 0,
                arrivalTime: simulationTime,
                completionTime: null,
                pageFaults: 0
            };
            
            processes.push(process);
            addLog(`Custom process ${process.id} added to the system`, "info");
            updateProcessList();
            updateAllocationTable();
            updateTotalProcessesDisplay();
        }
        
        // Terminate a process
        function terminateProcess(processId) {
            const processIndex = processes.findIndex(p => p.id === processId);
            if (processIndex !== -1) {
                const process = processes[processIndex];
                
                // Free resources
                resources.allocatedCPU -= process.cpuAllocated;
                resources.allocatedMemory -= process.memoryAllocated;
                resources.allocatedIO -= process.ioAllocated;
                
                process.status = "terminated";
                process.completionTime = simulationTime;
                
                addLog(`Process ${processId} terminated manually`, "warning");
                updateProcessList();
                updateResourceList();
                updateAllocationTable();
                updateProcessStatusCounts();
            }
        }
        
        // ==============================
        // RESOURCE ALLOCATION ALGORITHMS
        // ==============================
        
        // Adaptive resource allocation algorithm
        function allocateResourcesAdaptive() {
            // Free resources from completed/terminated processes
            processes.forEach(process => {
                if (process.status === "completed" || process.status === "terminated") {
                    resources.allocatedCPU -= process.cpuAllocated;
                    resources.allocatedMemory -= process.memoryAllocated;
                    resources.allocatedIO -= process.ioAllocated;
                    
                    process.cpuAllocated = 0;
                    process.memoryAllocated = 0;
                    process.ioAllocated = 0;
                }
            });
            
            const waitingProcesses = processes.filter(p => p.status === "waiting");
            
            // Adaptive algorithm: consider priority, resource needs, and wait time
            waitingProcesses.sort((a, b) => {
                // Higher priority first
                if (b.priority !== a.priority) return b.priority - a.priority;
                
                // If same priority, consider wait time (longer waiting processes get priority)
                if (b.waitTime !== a.waitTime) return b.waitTime - a.waitTime;
                
                // If same wait time, consider resource needs (smaller needs first)
                return (a.cpuNeed + a.memoryNeed/100 + a.ioNeed) - (b.cpuNeed + b.memoryNeed/100 + b.ioNeed);
            });
            
            // Try to allocate resources to waiting processes
            waitingProcesses.forEach(process => {
                const cpuAvailable = resources.cpu - resources.allocatedCPU;
                const memoryAvailable = resources.memory - resources.allocatedMemory;
                const ioAvailable = resources.io - resources.allocatedIO;
                
                if (process.cpuNeed <= cpuAvailable && 
                    process.memoryNeed <= memoryAvailable && 
                    process.ioNeed <= ioAvailable) {
                    
                    // Allocate resources
                    process.cpuAllocated = process.cpuNeed;
                    process.memoryAllocated = process.memoryNeed;
                    process.ioAllocated = process.ioNeed;
                    process.status = "running";
                    
                    // Update system resource allocation
                    resources.allocatedCPU += process.cpuNeed;
                    resources.allocatedMemory += process.memoryNeed;
                    resources.allocatedIO += process.ioNeed;
                    
                    contextSwitches++;
                    addLog(`Process ${process.id} allocated resources (CPU: ${process.cpuNeed}, Memory: ${process.memoryNeed}MB, I/O: ${process.ioNeed})`, "success");
                } else {
                    // Process continues waiting
                    process.waitTime += 1;
                    totalWaitTime += 1;
                }
            });
        }
        
        // Round Robin allocation
        function allocateResourcesRoundRobin() {
            // Free resources first
            processes.forEach(process => {
                if (process.status === "completed" || process.status === "terminated") {
                    resources.allocatedCPU -= process.cpuAllocated;
                    resources.allocatedMemory -= process.memoryAllocated;
                    resources.allocatedIO -= process.ioAllocated;
                    
                    process.cpuAllocated = 0;
                    process.memoryAllocated = 0;
                    process.ioAllocated = 0;
                }
            });
            
            const waitingProcesses = processes.filter(p => p.status === "waiting");
            
            // Simple round robin: allocate to first process that fits
            for (let process of waitingProcesses) {
                const cpuAvailable = resources.cpu - resources.allocatedCPU;
                const memoryAvailable = resources.memory - resources.allocatedMemory;
                const ioAvailable = resources.io - resources.allocatedIO;
                
                if (process.cpuNeed <= cpuAvailable && 
                    process.memoryNeed <= memoryAvailable && 
                    process.ioNeed <= ioAvailable) {
                    
                    process.cpuAllocated = process.cpuNeed;
                    process.memoryAllocated = process.memoryNeed;
                    process.ioAllocated = process.ioNeed;
                    process.status = "running";
                    
                    resources.allocatedCPU += process.cpuNeed;
                    resources.allocatedMemory += process.memoryNeed;
                    resources.allocatedIO += process.ioNeed;
                    
                    contextSwitches++;
                    addLog(`Process ${process.id} allocated resources (Round Robin)`, "success");
                    break; // Only allocate to one process per cycle in RR
                } else {
                    process.waitTime += 1;
                    totalWaitTime += 1;
                }
            }
        }
        
        // Priority-based allocation
        function allocateResourcesPriority() {
            // Free resources first
            processes.forEach(process => {
                if (process.status === "completed" || process.status === "terminated") {
                    resources.allocatedCPU -= process.cpuAllocated;
                    resources.allocatedMemory -= process.memoryAllocated;
                    resources.allocatedIO -= process.ioAllocated;
                    
                    process.cpuAllocated = 0;
                    process.memoryAllocated = 0;
                    process.ioAllocated = 0;
                }
            });
            
            const waitingProcesses = processes.filter(p => p.status === "waiting");
            
            // Sort by priority (higher priority first)
            waitingProcesses.sort((a, b) => b.priority - a.priority);
            
            // Allocate to highest priority processes that fit
            waitingProcesses.forEach(process => {
                const cpuAvailable = resources.cpu - resources.allocatedCPU;
                const memoryAvailable = resources.memory - resources.allocatedMemory;
                const ioAvailable = resources.io - resources.allocatedIO;
                
                if (process.cpuNeed <= cpuAvailable && 
                    process.memoryNeed <= memoryAvailable && 
                    process.ioNeed <= ioAvailable) {
                    
                    process.cpuAllocated = process.cpuNeed;
                    process.memoryAllocated = process.memoryNeed;
                    process.ioAllocated = process.ioNeed;
                    process.status = "running";
                    
                    resources.allocatedCPU += process.cpuNeed;
                    resources.allocatedMemory += process.memoryNeed;
                    resources.allocatedIO += process.ioNeed;
                    
                    contextSwitches++;
                    addLog(`Process ${process.id} allocated resources (Priority)`, "success");
                } else {
                    process.waitTime += 1;
                    totalWaitTime += 1;
                }
            });
        }
        
        // First Come First Serve allocation
        function allocateResourcesFCFS() {
            // Free resources first
            processes.forEach(process => {
                if (process.status === "completed" || process.status === "terminated") {
                    resources.allocatedCPU -= process.cpuAllocated;
                    resources.allocatedMemory -= process.memoryAllocated;
                    resources.allocatedIO -= process.ioAllocated;
                    
                    process.cpuAllocated = 0;
                    process.memoryAllocated = 0;
                    process.ioAllocated = 0;
                }
            });
            
            const waitingProcesses = processes.filter(p => p.status === "waiting");
            
            // Sort by arrival time
            waitingProcesses.sort((a, b) => a.arrivalTime - b.arrivalTime);
            
            // Allocate to processes in order of arrival
            waitingProcesses.forEach(process => {
                const cpuAvailable = resources.cpu - resources.allocatedCPU;
                const memoryAvailable = resources.memory - resources.allocatedMemory;
                const ioAvailable = resources.io - resources.allocatedIO;
                
                if (process.cpuNeed <= cpuAvailable && 
                    process.memoryNeed <= memoryAvailable && 
                    process.ioNeed <= ioAvailable) {
                    
                    process.cpuAllocated = process.cpuNeed;
                    process.memoryAllocated = process.memoryNeed;
                    process.ioAllocated = process.ioNeed;
                    process.status = "running";
                    
                    resources.allocatedCPU += process.cpuNeed;
                    resources.allocatedMemory += process.memoryNeed;
                    resources.allocatedIO += process.ioNeed;
                    
                    contextSwitches++;
                    addLog(`Process ${process.id} allocated resources (FCFS)`, "success");
                } else {
                    process.waitTime += 1;
                    totalWaitTime += 1;
                }
            });
        }
        
        // Main allocation function that calls the appropriate algorithm
        function allocateResources() {
            const algorithm = algorithmSelect.value;
            
            switch(algorithm) {
                case 'adaptive':
                    allocateResourcesAdaptive();
                    break;
                case 'roundrobin':
                    allocateResourcesRoundRobin();
                    break;
                case 'priority':
                    allocateResourcesPriority();
                    break;
                case 'fcfs':
                    allocateResourcesFCFS();
                    break;
                default:
                    allocateResourcesAdaptive();
            }
            
            // Update process status counts
            updateProcessStatusCounts();
            
            // Check for deadlocks if enabled
            if (deadlockDetectionSelect.value === 'on') {
                checkForDeadlocks();
            }
        }
        
        // ==============================
        // DEADLOCK DETECTION
        // ==============================
        
        function checkForDeadlocks() {
            const waitingProcesses = processes.filter(p => p.status === "waiting");
            let deadlockDetected = false;
            
            // Simple deadlock detection: if a process has been waiting too long
            waitingProcesses.forEach(process => {
                if (process.waitTime > 10) { // Threshold for deadlock detection
                    deadlockDetected = true;
                    addLog(`Warning: Process ${process.id} has been waiting for ${process.waitTime}s - possible deadlock`, "warning");
                }
            });
            
            // If deadlock detected and no processes are running
            if (deadlockDetected && processStatusCounts.running === 0 && waitingProcesses.length > 0) {
                deadlockCount++;
                deadlockWarningElement.style.display = 'block';
                addLog(`DEADLOCK DETECTED! ${waitingProcesses.length} processes are blocked`, "error");
                
                // Automatically terminate the lowest priority process to break deadlock
                if (waitingProcesses.length > 0) {
                    waitingProcesses.sort((a, b) => a.priority - b.priority); // Lowest priority first
                    terminateProcess(waitingProcesses[0].id);
                    addLog(`Process ${waitingProcesses[0].id} terminated to break deadlock`, "error");
                }
            } else {
                deadlockWarningElement.style.display = 'none';
            }
            
            updateStats();
        }
        
        // ==============================
        // PROCESS EXECUTION
        // ==============================
        
        function executeProcesses() {
            processes.forEach(process => {
                if (process.status === "running") {
                    process.remainingTime -= 1;
                    
                    // Simulate page faults for memory-intensive processes
                    if (process.type === "Memory Intensive" && Math.random() < 0.1) {
                        process.pageFaults++;
                        pageFaults++;
                        addLog(`Page fault occurred in Process ${process.id}`, "warning");
                    }
                    
                    // If process completed its execution
                    if (process.remainingTime <= 0) {
                        process.status = "completed";
                        process.completionTime = simulationTime;
                        completedProcesses++;
                        
                        // Calculate turnaround time
                        const turnaroundTime = simulationTime - process.arrivalTime;
                        totalTurnaroundTime += turnaroundTime;
                        
                        addLog(`Process ${process.id} completed execution (Turnaround: ${turnaroundTime}s)`, "info");
                    }
                } else if (process.status === "waiting") {
                    process.waitTime += 1;
                    totalWaitTime += 1;
                }
            });
        }
        
        // ==============================
        // UI UPDATE FUNCTIONS
        // ==============================
        
        function updateProcessStatusCounts() {
            processStatusCounts = {
                running: processes.filter(p => p.status === "running").length,
                waiting: processes.filter(p => p.status === "waiting").length,
                completed: processes.filter(p => p.status === "completed").length,
                terminated: processes.filter(p => p.status === "terminated").length
            };
        }
        
        function updateResourceHistory() {
            resourceHistory.push({
                time: simulationTime,
                cpu: (resources.allocatedCPU / resources.cpu) * 100,
                memory: (resources.allocatedMemory / resources.memory) * 100,
                io: (resources.allocatedIO / resources.io) * 100
            });
            
            // Keep only last 20 data points for chart
            if (resourceHistory.length > 20) {
                resourceHistory.shift();
            }
        }
        
        function updateProcessList() {
            processList.innerHTML = "";
            
            processes.forEach(process => {
                const processElement = document.createElement("div");
                processElement.className = `process-item ${process.status}`;
                
                const statusClass = `status-${process.status}`;
                const progressPercent = ((process.burstTime - process.remainingTime) / process.burstTime) * 100;
                
                processElement.innerHTML = `
                    <div class="process-info">
                        <div class="process-name">${process.id}. ${process.name}</div>
                        <div>Priority: ${process.priority} | Type: ${process.type}</div>
                        <div>CPU: ${process.cpuAllocated}/${process.cpuNeed} | Memory: ${process.memoryAllocated}MB/${process.memoryNeed}MB</div>
                        <div style="margin-top: 5px; width: 100%; height: 5px; background: #ddd; border-radius: 3px;">
                            <div style="width: ${progressPercent}%; height: 100%; background: var(--secondary-color); border-radius: 3px;"></div>
                        </div>
                    </div>
                    <div>
                        <span class="process-status ${statusClass}">${process.status.toUpperCase()}</span>
                        <div style="margin-top: 5px; font-size: 0.9rem;">${process.remainingTime}s remaining</div>
                        ${process.status === "waiting" ? 
                          `<button onclick="terminateProcess(${process.id})" style="margin-top: 5px; padding: 3px 8px; font-size: 0.8rem; background: var(--accent-color);">Terminate</button>` : 
                          ''}
                    </div>
                `;
                
                processList.appendChild(processElement);
            });
        }
        
        function updateResourceList() {
            resourceList.innerHTML = "";
            
            const cpuUtilization = ((resources.allocatedCPU / resources.cpu) * 100).toFixed(1);
            const memoryUtilization = ((resources.allocatedMemory / resources.memory) * 100).toFixed(1);
            const ioUtilization = ((resources.allocatedIO / resources.io) * 100).toFixed(1);
            
            const resourcesData = [
                { name: "CPU Cores", used: resources.allocatedCPU, total: resources.cpu, utilization: cpuUtilization },
                { name: "Memory", used: resources.allocatedMemory, total: resources.memory, utilization: memoryUtilization },
                { name: "I/O Devices", used: resources.allocatedIO, total: resources.io, utilization: ioUtilization }
            ];
            
            resourcesData.forEach(resource => {
                const resourceElement = document.createElement("div");
                resourceElement.className = "resource-item";
                
                resourceElement.innerHTML = `
                    <div class="resource-info">
                        <div style="font-weight: 600; color: var(--primary-color);">${resource.name}</div>
                        <div>${resource.used} / ${resource.total} (${resource.utilization}% utilized)</div>
                    </div>
                    <div style="width: 100px; height: 10px; background: #e0e0e0; border-radius: 5px; overflow: hidden;">
                        <div style="width: ${resource.utilization}%; height: 100%; background: var(--secondary-color);"></div>
                    </div>
                `;
                
                resourceList.appendChild(resourceElement);
            });
            
            // Update CPU utilization in header
            cpuUtilElement.textContent = cpuUtilization + "%";
        }
        
        function updateAllocationTable() {
            allocationTableBody.innerHTML = "";
            
            processes.forEach(process => {
                const row = document.createElement("tr");
                
                const statusClass = `status-${process.status}`;
                const turnaroundTime = process.completionTime ? 
                    process.completionTime - process.arrivalTime : 
                    simulationTime - process.arrivalTime;
                
                row.innerHTML = `
                    <td>${process.id}</td>
                    <td>${process.name}</td>
                    <td><span class="process-status ${statusClass}">${process.status.toUpperCase()}</span></td>
                    <td>${process.priority}</td>
                    <td>${process.cpuAllocated}/${process.cpuNeed}</td>
                    <td>${process.memoryAllocated}MB/${process.memoryNeed}MB</td>
                    <td>${process.ioAllocated}/${process.ioNeed}</td>
                    <td>${process.waitTime}s</td>
                    <td>${turnaroundTime}s</td>
                `;
                
                allocationTableBody.appendChild(row);
            });
        }
        
        function updateMetrics() {
            const cpuUtilization = ((resources.allocatedCPU / resources.cpu) * 100).toFixed(1);
            const memoryUtilization = ((resources.allocatedMemory / resources.memory) * 100).toFixed(1);
            
            // Calculate throughput (processes completed per minute)
            const throughput = completedProcesses > 0 ? 
                ((completedProcesses / simulationTime) * 60).toFixed(1) : "0";
            
            // Calculate average wait time
            const avgWaitTime = processes.length > 0 ? 
                (totalWaitTime / processes.length).toFixed(1) : "0";
            
            metricCpuElement.textContent = cpuUtilization + "%";
            metricMemoryElement.textContent = memoryUtilization + "%";
            metricThroughputElement.textContent = throughput + "/min";
            metricWaitElement.textContent = avgWaitTime + "s";
        }
        
        function updateStats() {
            statContextSwitch.textContent = contextSwitches;
            statPageFaults.textContent = pageFaults;
            statDeadlocks.textContent = deadlockCount;
        }
        
        function updateTotalProcessesDisplay() {
            totalProcessesElement.textContent = processes.length;
        }
        
        function updateCharts() {
            // Resource utilization chart
            const resourceCtx = document.getElementById('resource-chart').getContext('2d');
            
            if (resourceChart) {
                resourceChart.destroy();
            }
            
            const timeLabels = resourceHistory.map(point => point.time + "s");
            const cpuData = resourceHistory.map(point => point.cpu);
            const memoryData = resourceHistory.map(point => point.memory);
            const ioData = resourceHistory.map(point => point.io);
            
            resourceChart = new Chart(resourceCtx, {
                type: 'line',
                data: {
                    labels: timeLabels,
                    datasets: [
                        {
                            label: 'CPU Utilization',
                            data: cpuData,
                            borderColor: '#3498db',
                            backgroundColor: 'rgba(52, 152, 219, 0.1)',
                            tension: 0.3,
                            fill: true
                        },
                        {
                            label: 'Memory Utilization',
                            data: memoryData,
                            borderColor: '#2ecc71',
                            backgroundColor: 'rgba(46, 204, 113, 0.1)',
                            tension: 0.3,
                            fill: true
                        },
                        {
                            label: 'I/O Utilization',
                            data: ioData,
                            borderColor: '#e74c3c',
                            backgroundColor: 'rgba(231, 76, 60, 0.1)',
                            tension: 0.3,
                            fill: true
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 100,
                            title: {
                                display: true,
                                text: 'Utilization %'
                            }
                        },
                        x: {
                            title: {
                                display: true,
                                text: 'Simulation Time (s)'
                            }
                        }
                    }
                }
            });
            
            // Process status chart
            const processCtx = document.getElementById('process-chart').getContext('2d');
            
            if (processChart) {
                processChart.destroy();
            }
            
            processChart = new Chart(processCtx, {
                type: 'doughnut',
                data: {
                    labels: ['Running', 'Waiting', 'Completed', 'Terminated'],
                    datasets: [{
                        data: [
                            processStatusCounts.running, 
                            processStatusCounts.waiting, 
                            processStatusCounts.completed,
                            processStatusCounts.terminated
                        ],
                        backgroundColor: [
                            '#3498db',
                            '#f39c12',
                            '#27ae60',
                            '#e74c3c'
                        ],
                        borderWidth: 1
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            position: 'bottom'
                        }
                    }
                }
            });
        }
        
        function addLog(message, type = "info") {
            const logEntry = document.createElement("div");
            logEntry.className = `log-entry log-${type}`;
            
            const timeStr = simulationTime.toString().padStart(3, '0');
            logEntry.innerHTML = `<span class="log-time">[${timeStr}s]</span> ${message}`;
            
            systemLog.appendChild(logEntry);
            
            // Auto-scroll to bottom
            systemLog.scrollTop = systemLog.scrollHeight;
        }
        
        function clearLog() {
            systemLog.innerHTML = "";
        }
        
        // ==============================
        // SIMULATION CONTROL FUNCTIONS
        // ==============================
        
        function startSimulation() {
            if (isRunning) return;
            
            isRunning = true;
            addLog("Simulation started", "success");
            
            // Start auto-add process if enabled
            const autoAddTime = parseInt(autoAddSelect.value);
            if (autoAddTime > 0) {
                autoAddInterval = setInterval(() => {
                    addRandomProcess();
                    addLog(`Auto-added new process (Total: ${processes.length})`, "info");
                }, autoAddTime * 1000);
            }
            
            simulationInterval = setInterval(() => {
                simulationTime++;
                simulationTimeElement.textContent = "Time: " + simulationTime + "s";
                
                // Update current algorithm display
                const algorithmText = algorithmSelect.options[algorithmSelect.selectedIndex].text;
                currentAlgorithmElement.textContent = algorithmText;
                
                // Execute simulation step
                executeProcesses();
                allocateResources();
                
                // Update UI
                updateProcessList();
                updateResourceList();
                updateAllocationTable();
                updateResourceHistory();
                updateMetrics();
                updateStats();
                updateCharts();
                
                // Add periodic log
                if (simulationTime % 5 === 0) {
                    addLog(`System check: ${processStatusCounts.running} running, ${processStatusCounts.waiting} waiting, ${processStatusCounts.completed} completed`, "info");
                }
                
                // Check if all processes are completed
                if (processStatusCounts.completed + processStatusCounts.terminated === processes.length && processes.length > 0) {
                    addLog("All processes completed. Simulation finished.", "success");
                    pauseSimulation();
                }
                
            }, parseInt(timeSliceInput.value));
        }
        
        function pauseSimulation() {
            if (!isRunning) return;
            
            isRunning = false;
            clearInterval(simulationInterval);
            clearInterval(autoAddInterval);
            addLog("Simulation paused", "warning");
        }
        
        function resetSimulation() {
            pauseSimulation();
            initSimulation();
            addLog("Simulation reset", "info");
        }
        
        // ==============================
        // ALGORITHM INFORMATION MODAL
        // ==============================
        
        function showAlgorithmInfo() {
            const algorithm = algorithmSelect.value;
            let details = "";
            
            switch(algorithm) {
                case 'adaptive':
                    details = `
                        <h3>Adaptive (Priority + Need) Algorithm</h3>
                        <p>This algorithm dynamically adjusts resource allocation based on multiple factors:</p>
                        <ul>
                            <li><strong>Priority:</strong> Higher priority processes get resources first</li>
                            <li><strong>Wait Time:</strong> Longer waiting processes get priority boost</li>
                            <li><strong>Resource Needs:</strong> Considers CPU, memory, and I/O requirements</li>
                            <li><strong>Adaptive:</strong> Adjusts allocation based on current system state</li>
                        </ul>
                        <p><strong>Advantages:</strong> Balances fairness and efficiency, prevents starvation</p>
                        <p><strong>Disadvantages:</strong> More complex to implement</p>
                    `;
                    break;
                case 'roundrobin':
                    details = `
                        <h3>Round Robin Algorithm</h3>
                        <p>Each process gets a fixed time slice (quantum) for execution:</p>
                        <ul>
                            <li>Processes are allocated resources in circular order</li>
                            <li>Time quantum determines how long a process runs</li>
                            <li>If process doesn't complete in time slice, it goes back to queue</li>
                        </ul>
                        <p><strong>Advantages:</strong> Fair allocation, no starvation</p>
                        <p><strong>Disadvantages:</strong> High context switching overhead</p>
                    `;
                    break;
                case 'priority':
                    details = `
                        <h3>Priority-Based Algorithm</h3>
                        <p>Processes are allocated resources based on their priority level:</p>
                        <ul>
                            <li>Higher priority processes get resources first</li>
                            <li>Priority can be static or dynamic</li>
                            <li>May use aging to prevent starvation</li>
                        </ul>
                        <p><strong>Advantages:</strong> Important processes get resources quickly</p>
                        <p><strong>Disadvantages:</strong> Low priority processes may starve</p>
                    `;
                    break;
                case 'fcfs':
                    details = `
                        <h3>First Come First Serve Algorithm</h3>
                        <p>Processes are served in the order they arrive:</p>
                        <ul>
                            <li>Simplest scheduling algorithm</li>
                            <li>Non-preemptive</li>
                            <li>Uses FIFO (First In First Out) queue</li>
                        </ul>
                        <p><strong>Advantages:</strong> Simple to implement</p>
                        <p><strong>Disadvantages:</strong> Poor for short processes, convoy effect</p>
                    `;
                    break;
                default:
                    details = `<p>Algorithm information not available.</p>`;
            }
            
            algorithmDetails.innerHTML = details;
            algorithmModal.style.display = 'block';
        }
        
        // ==============================
        // EVENT LISTENERS
        // ==============================
        
        startBtn.addEventListener('click', startSimulation);
        pauseBtn.addEventListener('click', pauseSimulation);
        resetBtn.addEventListener('click', resetSimulation);
        addProcessBtn.addEventListener('click', addCustomProcess);
        algoInfoBtn.addEventListener('click', showAlgorithmInfo);
        clearLogBtn.addEventListener('click', clearLog);
        
        // Modal close button
        closeModal.addEventListener('click', () => {
            algorithmModal.style.display = 'none';
        });
        
        // Close modal when clicking outside
        window.addEventListener('click', (event) => {
            if (event.target === algorithmModal) {
                algorithmModal.style.display = 'none';
            }
        });
        
        // Handle input changes
        processCountInput.addEventListener('change', () => {
            if (!isRunning) {
                initSimulation();
            }
        });
        
        algorithmSelect.addEventListener('change', () => {
            const algorithmText = algorithmSelect.options[algorithmSelect.selectedIndex].text;
            addLog("Allocation algorithm changed to: " + algorithmText, "info");
        });
        
        // Initialize on load
        window.addEventListener('load', () => {
            initSimulation();
            updateCharts();
            
            // Add welcome message
            addLog("Adaptive Resource Allocation System initialized", "info");
            addLog("Configure parameters and click 'Start Simulation' to begin", "info");
        });
    </script>
</body>
</html>