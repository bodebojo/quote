#ifndef INDEX_HTML_NO_CAM_H
#define INDEX_HTML_NO_CAM_H

// HTML page to be served to the client without the camera section, stored in flash memory (PROGMEM)
const char index_html_no_cam[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>ESP32 Dashboard - No Camera</title>
  <style>
    body {
      margin: 0;
      font-family: 'Segoe UI', sans-serif;
      background: #eef8ee;
      color: #333;
    }
    header {
      background: #4CAF50;
      color: white;
      padding: 1rem;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    header h1 {
      margin: 0;
      font-size: 1.5rem;
    }
    nav {
      display: flex;
      gap: 1rem;
    }
    .menu-toggle {
      display: none;
      font-size: 1.5rem;
      cursor: pointer;
    }
    .tabs {
      display: flex;
      flex-direction: column;
    }
    .tab-content {
      display: none;
      padding: 1rem;
    }
    .tab-content.active {
      display: block;
    }
    .sensor-boxes h3 {
      margin: 0.5rem 0;
    }
    .status {
      font-weight: bold;
      color: green;
    }
    .driving-buttons {
      display: flex;
      gap: 1rem;
      flex-wrap: wrap;
      margin: 1rem 0;
    }
    .driving-buttons button {
      background: linear-gradient(to right, #43e97b, #38f9d7);
      border: none;
      color: #fff;
      padding: 0.75rem 1.5rem;
      border-radius: 12px;
      font-size: 1rem;
      font-weight: bold;
      box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
      transition: transform 0.2s ease, box-shadow 0.2s ease;
    }
    .driving-buttons button:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 12px rgba(0, 0, 0, 0.2);
    }
    @media (max-width: 600px) {
      header .menu-toggle { display: block; }
      nav { display: none; flex-direction: column; background: #4CAF50; padding: 1rem; }
      nav.active { display: flex; }
      .driving-buttons { flex-direction: column; }
      .driving-buttons button { width: 100%; }
    }
  </style>
</head>
<body>
  <header>
    <h1>ESP32 Dashboard</h1>
    <span class="menu-toggle" onclick="toggleMenu()">&#9776;</span>
    <nav id="menu">
      <a href="#" onclick="showTab('home')">Home</a>
      <a href="#" onclick="showTab('sensors')">Sensors</a>
      <button onclick="gotoDatabase()" style="background:none;border:none;color:white;font-weight:bold;cursor:pointer;">Database</button>
    </nav>
  </header>

  <div class="tabs">
    <div id="home" class="tab-content active">
      <h2>Driving Mode</h2>
      <div class="driving-buttons">
        <button onclick="sendDrivingMode(1)">Mode 1</button>
        <button onclick="sendDrivingMode(2)">Mode 2</button>
        <button onclick="sendDrivingMode(3)">Mode 3</button>
        <button onclick="sendDrivingMode(4)">Mode 4</button>
      </div>
      <div class="sensor-boxes">
        <h3>Distance: <span id="distance">0</span> cm</h3>
        <h3>Light Left: <span id="light_left">0</span></h3>
        <h3>Light Right: <span id="light_right">0</span></h3>
        <p class="status" id="status">Connecting to WebSocket...</p>
      </div>
    </div>
    <div id="sensors" class="tab-content">
      <h2>Driving Mode</h2>
      <div class="driving-buttons">
        <button onclick="sendDrivingMode(1)">Mode 1</button>
        <button onclick="sendDrivingMode(2)">Mode 2</button>
        <button onclick="sendDrivingMode(3)">Mode 3</button>
        <button onclick="sendDrivingMode(4)">Mode 4</button>
      </div>
      <div class="sensor-boxes">
        <h3>Distance: <span id="distance2">0</span> cm</h3>
        <h3>Light Left: <span id="light_left2">0</span></h3>
        <h3>Light Right: <span id="light_right2">0</span></h3>
        <p class="status" id="status2">Connecting to WebSocket...</p>
      </div>
    </div>
  </div>

  <script>
    window.onload = () => { document.body.tabIndex = 0; document.body.focus(); };
    function toggleMenu() { document.getElementById('menu').classList.toggle('active'); }
    function showTab(id) {
      document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
      document.getElementById(id).classList.add('active');
    }
    function gotoDatabase() {
      // Remove ESP dashboard content and navigate
      document.body.innerHTML = '';
      window.location.replace('http://10.42.0.1:81');
    }

    const ws = new WebSocket('ws://' + location.hostname + '/ws');
    const keysDown = new Set();
    ws.onopen = () => {
      ['status','status2'].forEach(id => { const el = document.getElementById(id); el.textContent='WS Connected'; el.style.color='green'; });
    };
    ws.onmessage = evt => {
      const d = evt.data;
      if (d.startsWith('distance:')) update('distance','distance2',d);
      if (d.startsWith('Light Left:')) update('light_left','light_left2',d);
      if (d.startsWith('Light Right:')) update('light_right','light_right2',d);
    };
    ws.onclose = () => setTimeout(() => location.reload(),2000);
    function update(id1,id2,msg) { const val = msg.split(':')[1]; document.getElementById(id1).textContent=val; document.getElementById(id2).textContent=val; }
    function sendDrivingMode(m) { if (ws.readyState === 1) ws.send('Button = Change Driving Mode: '+m); }
    document.addEventListener('keydown', e => { if (!keysDown.has(e.key)) { keysDown.add(e.key); if(ws.readyState===1) ws.send('KeyDown:'+e.key); } });
    document.addEventListener('keyup', e => { if (keysDown.has(e.key)) { keysDown.delete(e.key); if(ws.readyState===1) ws.send('KeyUp:'+e.key); } });
  </script>
</body>
</html>
)rawliteral";

#endif
