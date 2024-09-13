#include <ESP8266WiFi.h>

const char* ssid = "CoachGroepTiuri";
const char* password = "CoachGroepTiuri";
WiFiServer server(80);

String header;
String gpioState[2] = {"off", "off"}, wasdState[4] = {"Released", "Released", "Released", "Released"};
bool wasdPressed[4] = {false, false, false, false}; // Tracks W, A, S, D key states
const int gpios[2] = {26, 27}; // GPIO pin 26 and 27

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 2; i++) pinMode(gpios[i], OUTPUT); // Set GPIOs as output
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  server.begin();
  Serial.println(WiFi.localIP());
}

void handleKey(int index, const String& state) {
  if (state == "Pressed" && !wasdPressed[index]) {
    wasdPressed[index] = true;
    wasdState[index] = "Pressed";
    Serial.printf("%c key pressed\n", 'W' + index); // W, A, S, D based on index
  } else if (state == "Released") {
    wasdPressed[index] = false;
    wasdState[index] = "Released";
    Serial.printf("%c key released\n", 'W' + index);
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

        client.println("HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\n");
        client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
        client.println("<style>html{font-family:Helvetica;text-align:center}.button{padding:16px 40px;background:#4CAF50;color:white;border:none;margin:2px;font-size:30px}</style></head>");
        client.println("<body><h1>ESP8266 Web Server</h1>");

        // GPIO 26 and 27 buttons
        for (int i = 0; i < 2; i++) {
          client.printf("<p>GPIO %d - State: %s</p>", gpios[i], gpioState[i].c_str());
          client.printf("<p><a href=\"/%d/%s\"><button class=\"button\">Turn %s GPIO %d</button></a></p>", 
            gpios[i], gpioState[i] == "off" ? "on" : "off", gpioState[i] == "off" ? "On" : "Off", gpios[i]);
        }

        // WASD key states
        client.printf("<p>W Key: %s</p><p>A Key: %s</p><p>S Key: %s</p><p>D Key: %s</p>", 
                      wasdState[0].c_str(), wasdState[1].c_str(), wasdState[2].c_str(), wasdState[3].c_str());

        // JavaScript to handle WASD key presses
        client.println("<script>let keys={w:false,a:false,s:false,d:false};['w','a','s','d'].forEach(k=>{document.addEventListener('keydown',e=>{if(e.key==k&&!keys[k]){fetch('/'+k+'/press').catch(console.error);keys[k]=true}});document.addEventListener('keyup',e=>{if(e.key==k){fetch('/'+k+'/release').catch(console.error);keys[k]=false}})});</script>");
        client.println("</body></html>");
        
        client.stop();
        header = "";
      }
    }
  }
}
