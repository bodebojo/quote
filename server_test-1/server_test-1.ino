#include <WiFi.h>                      // WiFi support
#include <ESPAsyncWebServer.h>        // Async web server for better performance
#include <AsyncTCP.h>                 // Async TCP support (required by the web server)
#include "camera-main.h"             // Your custom camera logic
#include "index_html.h"              // Web interface (served as HTML string)

// WiFi credentials
const char* ssid = "CoachGroepTiuri";
const char* password = "CoachGroepTiuri";

// Create web server and WebSocket instances
AsyncWebServer server(80);                // HTTP server on port 80
AsyncWebSocket webSocket("/ws");          // WebSocket handler at /ws

String distance;                          // Holds the most recent distance value

// Forward declarations
void readSerial();                        // Function to parse serial data (distance)
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len); // WebSocket event handler

void setup() {
  Serial.begin(115200); // Start serial monitor for debugging

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected! IP: " + WiFi.localIP().toString());

  // Serve the web interface at root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html); // Send preloaded HTML page
  });

  // Set up WebSocket event handling
  webSocket.onEvent(onWebSocketEvent);
  server.addHandler(&webSocket);

  // Start the server
  server.begin();

  // Initialize and start camera stream task
  startCameraTask();
}

void loop() {
  // Continuously check for new serial data (e.g., distance sensor)
  readSerial();
}

// WebSocket Events: handles communication with the webpage
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket connected");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket disconnected");
  } else if (type == WS_EVT_DATA) {
    String message = String((char*)data); // Convert received bytes to String

    // Handle messages based on their prefix
    if (message.startsWith("KeyDown:")) {
      String key = message.substring(8);
      Serial.println("Key Down: " + key);
    } else if (message.startsWith("KeyUp:")) {
      String key = message.substring(6);
      Serial.println("Key Up: " + key);
    } else if (message.startsWith("Button = Change Driving Mode:")) {
      String mode = message.substring(String("Button = Change Driving Mode:").length());
      Serial.println("Mode:" + mode);
     // Optionally send this to another serial device
    } else {
      Serial.println("Received unknown message: " + message);
    }
  }
}

// Reads serial input for distance sensor data and updates global variable
void readSerial() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n'); // Read a line
    data.trim(); // Remove whitespace

      if (data.startsWith("Distance:")) {
        distance = data.substring(String("distance:").length());
        distance.trim();
        Serial.println("Updated Distance: " + distance);

        webSocket.textAll("distance:" + distance); //send distance tho websocket 
    } else if (data.startsWith("Light Left:")) {
        String left = data.substring(String("Light Left:").length());
        left.trim();
        webSocket.textAll("Light Left:" + left);
    } else if (data.startsWith("Light Right:")) {
        String right = data.substring(String("Light Right:").length());
        right.trim();
        webSocket.textAll("Light Right:" + right);
    }
  }
}
