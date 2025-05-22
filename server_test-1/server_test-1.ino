#include <WiFi.h>                    // WiFi support for ESP32
#include <ESPAsyncWebServer.h>         // Async web server for non-blocking HTTP
#include <AsyncTCP.h>                  // TCP support for AsyncWebServer
#include <HTTPClient.h>                // HTTP client for sending data


#include "mqtt_updater.h"              // MQTT-like updater to PHP server
#include "camera-main.h"               // Custom camera logic
#include "index_html.h"                // Web interface HTML content
#include "index_html-no-cam.h"
// ------------------------- Configuration -------------------------
bool mqttEnabled = true;           // Enable MQTT-like uploader (default: true)
bool cameraEnabled = true;         // Enable camera streaming (default: true)
bool cameraFlip = true;            // Flip camera image vertically (default: true)

const char* ssid = "CoachGroepTiuri";      // WiFi SSID
const char* password = "CoachGroepTiuri"; // WiFi password

// ------------------------- Web Server Setup -------------------------
AsyncWebServer server(80);               // Web server listening on port 80
AsyncWebSocket webSocket("/ws");        // WebSocket at "/ws" endpoint

// ------------------------- Sensor Data -------------------------
String distance = "";            // Distance sensor value
String lightLeft = "";           // Left light sensor value
String lightRight = "";          // Right light sensor value

bool gotDistance = false;        // Flag to track if distance is updated
bool gotLeft = false;            // Flag to track if left light is updated
bool gotRight = false;           // Flag to track if right light is updated

// ------------------------- Function Declarations -------------------------
void readSerial();                               // Reads data from the serial port
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len);  // Handles WebSocket events

// ------------------------- Setup -------------------------
void setup() {
  Serial.begin(115200);  // Start serial communication for debugging

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected! IP: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (cameraEnabled) {
      // Serve second HTML if condition is true
      request->send(200, "text/html", index_html);
    } else {
      // Serve the first HTML page
      request->send(200, "text/html", index_html_no_cam);
    }
  });

  // Attach WebSocket handler
  webSocket.onEvent(onWebSocketEvent);
  server.addHandler(&webSocket);
  
  // Start the server
  server.begin();

  // Start camera task if enabled
  if (cameraEnabled) {
    startCameraTask();
  }
}

void loop() {
  // Continuously check for serial input from sensors
  readSerial();
  static unsigned long lastPing = 0;
  if (millis() - lastPing > 15000) {
    webSocket.textAll("ping");
    lastPing = millis();
  }
}

// ------------------------- WebSocket Handler -------------------------
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket connected");  // When a WebSocket client connects

  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket disconnected");  // When a WebSocket client disconnects

  } else if (type == WS_EVT_DATA) {
    String message = String((char *)data);

    // Parse WebSocket messages and handle different cases
    if (message.startsWith("KeyDown:")) {
      String key = message.substring(8);
      Serial.println("Key Down: " + key);  // Handle key down event

    } else if (message.startsWith("KeyUp:")) {
      String key = message.substring(6);
      Serial.println("Key Up: " + key);  // Handle key up event

    } else if (message.startsWith("Button = Change Driving Mode:")) {
      String mode = message.substring(String("Button = Change Driving Mode:").length());
      Serial.println("Mode: " + mode);  // Handle driving mode change
    
    } else if (message.startsWith("lcdMesage:")) {
      String lcd_mesage = message.substring(10);
      Serial.println("message_to_lcd: " + lcd_mesage);
    
    } else if  (message.startsWith("Clearlcd")) {
      Serial.println("clear_lcd");

    } else {
      Serial.println("Received unknown message: " + message);  // Handle unknown message
    }
  }
}

// ------------------------- Read Serial Sensor Data -------------------------
void readSerial() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');  // Read data from the serial port
    data.trim();

    // Process sensor data for distance, light left, and light right
    if (data.startsWith("Distance:")) {
      distance = data.substring(9);
      distance.trim();
      // Serial.println("Updated Distance: " + distance);
      webSocket.textAll("distance:" + distance);  // Send distance data to all connected clients
      gotDistance = true;

    } else if (data.startsWith("Light Left:")) {
      lightLeft = data.substring(11);
      lightLeft.trim();
      webSocket.textAll("Light Left:" + lightLeft);  // Send left light data to all connected clients
      gotLeft = true;

    } else if (data.startsWith("Light Right:")) {
      lightRight = data.substring(12);
      lightRight.trim();
      webSocket.textAll("Light Right:" + lightRight);  // Send right light data to all connected clients
      gotRight = true;
    }

    // If all sensor values are received, send them to the server
    if (gotDistance && gotLeft && gotRight) {
      if (mqttEnabled) {
        sendToServer(lightLeft, lightRight, distance);  // Upload sensor data to the server
      }

      // Reset flags after sending the data
      gotDistance = false;
      gotLeft = false;
      gotRight = false;
    }
  }
}
