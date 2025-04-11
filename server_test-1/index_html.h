#ifndef INDEX_HTML_H
#define INDEX_HTML_H

// HTML page to be served to the client, stored in flash memory (PROGMEM)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>ESP32 Camera and Distance</title>
  <style>
    /* Basic styling for page layout */
    body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 0; }
    #videoFeed { width: 640px; height: 480px; border: 1px solid #ccc; }  /* Camera feed container */
    #distance { font-size: 20px; font-weight: bold; }  /* Distance display style */
    #status { font-size: 16px; color: red; }  /* WebSocket connection status */
  </style>
</head>
<body>
  <!-- Header with title -->
  <h1>ESP32 Camera Live Feed</h1>
  <!-- MJPEG stream dynamically loaded from ESP32 camera -->
  <img id="videoFeed" src="http://" alt="Camera Stream" />
  
  <h2>Key Detection</h2>
  <p>Press any key to send a message to the ESP32</p>
  
  <!-- Distance display -->
  <h3>Distance: <span id="distance">0</span></h3>
   
  <!-- WebSocket status display -->
  <p id="status">Connecting to WebSocket...</p>

<script>
  // Dynamically set MJPEG feed URL to the ESP32's IP address (port 82 for MJPEG stream)
  document.getElementById('videoFeed').src = 'http://' + window.location.hostname + ':82/mjpeg';

  var ws;

  // WebSocket connection to communicate with ESP32
  function connectWebSocket() {
    // Establish WebSocket connection to the ESP32
    ws = new WebSocket('ws://' + window.location.hostname + '/ws');
    
    // WebSocket open event - When the connection is successfully established
    ws.onopen = function() {
      console.log("WebSocket connected");
      // Update the status to show connected
      document.getElementById('status').textContent = "WebSocket Connected!";
      document.getElementById('status').style.color = 'green';
    };

    // WebSocket message event - When a message is received from ESP32
    ws.onmessage = function(event) {
      if (event.data.startsWith("distance:")) {
        var value = event.data.split(":")[1];  // Extract the distance value
        console.log("Distance received:", value);  // Debugging message
        document.getElementById('distance').textContent = value; 
      }
    };

    // WebSocket close event - When the connection is closed
    ws.onclose = function() {
      console.log("WebSocket disconnected, trying to reconnect...");
      // Update the status to show WebSocket is disconnected
      document.getElementById('status').textContent = "WebSocket Disconnected. Retrying...";
      document.getElementById('status').style.color = 'red';
      // Retry the connection after 2 seconds
      setTimeout(connectWebSocket, 2000);
    };

    // WebSocket error event - If there’s an error, attempt to close the connection and reconnect
    ws.onerror = function(err) {
      console.error("WebSocket error:", err);
      ws.close(); // Close WebSocket to trigger reconnect
    };
  }

  // Start WebSocket connection when page loads
  connectWebSocket();

  // Handle key press events (send data on key down and key up)
  const keysDown = new Set();

  // Key down event - send a message when a key is pressed
  document.addEventListener('keydown', function(event) {
    if (!keysDown.has(event.key) && ws.readyState === WebSocket.OPEN) {
      keysDown.add(event.key); // Mark key as pressed
      ws.send("KeyDown:" + event.key); // Send key press event to ESP32
    }
  });

  // Key up event - send a message when a key is released
  document.addEventListener('keyup', function(event) {
    if (keysDown.has(event.key) && ws.readyState === WebSocket.OPEN) {
      keysDown.delete(event.key); // Mark key as released
      ws.send("KeyUp:" + event.key); // Send key release event to ESP32
    }
  });
</script>

</body>
</html>
)rawliteral";

#endif
