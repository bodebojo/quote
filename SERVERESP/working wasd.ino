#include <ESP8266WiFi.h>

const char* ssid = "CoachGroepTiuri";
const char* password = "CoachGroepTiuri";
WiFiServer server(80);

// Password protection
const String webpagePassword = "Qote"; // Replace with your desired password

String header;
String gpioState[2] = {"off", "off"}, keyState[5] = {"Released", "Released", "Released", "Released", "Released"};
bool keyPressed[5] = {false, false, false, false, false}; // Tracks W, A, S, D, Shift keys
bool sendExtraKey[5] = {false, false, false, false, false}; // Flags to send extra key press after another key is released
const int gpios[2] = {26, 27}; // GPIO pin 26 and 27
bool authenticated = false;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 2; i++) pinMode(gpios[i], OUTPUT); // Set GPIOs as output
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  server.begin();
  Serial.println(WiFi.localIP());
}

void handleKey(int index, const String& state) {
  if (state == "Pressed" && !keyPressed[index]) {
    keyPressed[index] = true;
    keyState[index] = "Pressed";

    // Print appropriate label based on key index
    if (index == 0) {
      Serial.println("W key pressed");
    } else if (index == 1) {
      Serial.println("X key pressed");  // Changed A to X
    } else if (index == 2) {
      Serial.println("Y key pressed");  // Changed S to Y
    } else if (index == 3) {
      Serial.println("Z key pressed");  // Changed D to Z
    } else {
      Serial.println("Shift key pressed");
    }

    // Set extra key flag for other keys when this key is pressed
    for (int i = 0; i < 5; i++) {
      if (i != index && keyPressed[i]) {
        sendExtraKey[i] = true;
      }
    }
  } else if (state == "Released") {
    keyPressed[index] = false;
    keyState[index] = "Released";

    // Print appropriate label based on key index
    if (index == 0) {
      Serial.println("W key released");
    } else if (index == 1) {
      Serial.println("X key released");  // Changed A to X
    } else if (index == 2) {
      Serial.println("Y key released");  // Changed S to Y
    } else if (index == 3) {
      Serial.println("Z key released");  // Changed D to Z
    } else {
      Serial.println("Shift key released");
    }

    // Check if any other key is still pressed and send extra press if needed
    for (int i = 0; i < 5; i++) {
      if (sendExtraKey[i] && keyPressed[i]) {
        if (i == 0) Serial.println("W key pressed");
        else if (i == 1) Serial.println("X key pressed");
        else if (i == 2) Serial.println("Y key pressed");
        else if (i == 3) Serial.println("Z key pressed");
        else Serial.println("Shift key pressed");

        sendExtraKey[i] = false; // Reset the flag
      }
    }
  }
}

void handleGPIO(int index, bool on) {
  gpioState[index] = on ? "on" : "off";
  digitalWrite(gpios[index], on ? HIGH : LOW);
  Serial.printf("GPIO %d %s\n", gpios[index], on ? "on" : "off");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;
  
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      header += c;
      if (c == '\n') {
        // Parse the header for password
        if (header.indexOf("GET /?password=") >= 0) {
          int passwordIndex = header.indexOf("password=") + 9;
          String receivedPassword = header.substring(passwordIndex, header.indexOf(' ', passwordIndex));

          // Check if the password matches
          if (receivedPassword == webpagePassword) {
            authenticated = true;
          } else {
            authenticated = false;
          }
        }

        // If not authenticated, prompt for password
        if (!authenticated) {
          client.println("HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\n");
          client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
          client.println("<body><h1>Password Required</h1>");
          client.println("<script>");
          client.println("let password = prompt('Please enter the password:');");
          client.println("if (password) { window.location.href = '/?password=' + password; }");
          client.println("</script></body></html>");
          client.stop();
          header = "";
          return;
        }

        // Handle GPIO and key requests if authenticated
        if (header.indexOf("GET /26/on") >= 0) handleGPIO(0, true);
        if (header.indexOf("GET /26/off") >= 0) handleGPIO(0, false);
        if (header.indexOf("GET /27/on") >= 0) handleGPIO(1, true);
        if (header.indexOf("GET /27/off") >= 0) handleGPIO(1, false);
        
        if (header.indexOf("GET /w/press") >= 0) handleKey(0, "Pressed");
        if (header.indexOf("GET /w/release") >= 0) handleKey(0, "Released");
        if (header.indexOf("GET /a/press") >= 0) handleKey(1, "Pressed");
        if (header.indexOf("GET /a/release") >= 0) handleKey(1, "Released");
        if (header.indexOf("GET /s/press") >= 0) handleKey(2, "Pressed");
        if (header.indexOf("GET /s/release") >= 0) handleKey(2, "Released");
        if (header.indexOf("GET /d/press") >= 0) handleKey(3, "Pressed");
        if (header.indexOf("GET /d/release") >= 0) handleKey(3, "Released");
        if (header.indexOf("GET /shift/press") >= 0) handleKey(4, "Pressed");
        if (header.indexOf("GET /shift/release") >= 0) handleKey(4, "Released");

        client.println("HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\n");
        client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
        client.println("<style>html{font-family:Helvetica;text-align:center}.button{padding:16px 40px;background:#4CAF50;color:white;border:none;margin:2px;font-size:30px}</style></head>");
        client.println("<body><h1>QOTE</h1>");

        // GPIO 26 and 27 buttons
        for (int i = 0; i < 2; i++) {
          client.printf("<p>GPIO %d - State: %s</p>", gpios[i], gpioState[i].c_str());
          client.printf("<p><a href=\"/%d/%s\"><button class=\"button\">Turn %s GPIO %d</button></a></p>", 
            gpios[i], gpioState[i] == "off" ? "on" : "off", gpioState[i] == "off" ? "On" : "Off", gpios[i]);
        }

        // WASD and Shift key states
        client.printf("<p>W Key: %s</p><p>X Key: %s</p><p>Y Key: %s</p><p>Z Key: %s</p><p>Shift Key: %s</p>", 
                      keyState[0].c_str(), keyState[1].c_str(), keyState[2].c_str(), keyState[3].c_str(), keyState[4].c_str());

        // JavaScript to handle WASD and Shift key presses
        client.println("<script>let keys={w:false,a:false,s:false,d:false,shift:false};['w','a','s','d','shift'].forEach(k=>{document.addEventListener('keydown',e=>{if(e.key.toLowerCase()==k&&!keys[k]){fetch('/'+k+'/press').catch(console.error);keys[k]=true}});document.addEventListener('keyup',e=>{if(e.key.toLowerCase()==k){fetch('/'+k+'/release').catch(console.error);keys[k]=false}})});</script>");
        client.println("</body></html>");
        
        client.stop();
        header = "";
      }
    }
  }
}
