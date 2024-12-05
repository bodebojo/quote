#include <ESP8266WiFi.h>
#include <SoftwareSerial.h> // For software serial communication if needed

const char* ssid = "CoachGroepTiuri";
const char* password = "CoachGroepTiuri";
WiFiServer server(80);

// Password protection
const String webpagePassword = "Qote";

String header;
const char* keyNames[] = {"W", "X", "Y", "Z", "Shift"};
String gpioState[2] = {"off", "off"};
bool keyPressed[5] = {false, false, false, false, false}; // Tracks W, A, S, D, Shift keys
bool sendExtraKey[5] = {false, false, false, false, false}; // Flags to send extra key press
const int gpios[2] = {26, 27}; // GPIO pins
bool authenticated = false;
String distnesValue = "0"; // Variable to store the distance value

void connectToWiFi() {
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi!");
    ESP.restart();
  } else {
    Serial.println("\nConnected to WiFi!");
    Serial.println(WiFi.localIP());
  }
}

void handleKey(int index, const String& state) {
  if (state == "Pressed" && !keyPressed[index]) {
    keyPressed[index] = true;
    Serial.printf("%s key pressed\n", keyNames[index]);
    for (int i = 0; i < 5; i++) sendExtraKey[i] = keyPressed[i] && i != index;
  } else if (state == "Released") {
    keyPressed[index] = false;
    Serial.printf("%s key released\n", keyNames[index]);
  }
}

void handleGPIO(int index, bool on) {
  gpioState[index] = on ? "on" : "off";
  digitalWrite(gpios[index], on ? HIGH : LOW);
  Serial.printf("GPIO %d %s\n", gpios[index], on ? "on" : "off");
}

bool verifyPassword(const String& receivedPassword) {
  return receivedPassword == webpagePassword;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 2; i++) pinMode(gpios[i], OUTPUT); // Set GPIOs as output
  connectToWiFi();
  server.begin();
}

void loop() {
  if (Serial.available()) {
    String incomingData = Serial.readStringUntil('\n');
    incomingData.trim();

    if (incomingData == "Average distance:") {
      distnesValue = Serial.readStringUntil('\n');
      distnesValue.trim();
      Serial.println("Distance:");
      Serial.println(distnesValue);
    }
  }

  WiFiClient client = server.available();
  if (!client) return;

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      header += c;

      if (c == '\n') {
        if (header.indexOf("GET /distance") >= 0) {
          Serial.println("Distance request received.");
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type: text/plain");
          client.println("Connection: close");
          client.println();
          client.println(distnesValue);
          delay(1); // Allow the response to be sent
          client.stop(); // Properly close the connection
          header = "";
          return;
        }

        if (header.indexOf("GET /?password=") >= 0) {
          int passwordIndex = header.indexOf("password=") + 9;
          String receivedPassword = header.substring(passwordIndex, header.indexOf(' ', passwordIndex));
          authenticated = verifyPassword(receivedPassword);
        }

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

        if (header.indexOf("GET /26/on") >= 0) handleGPIO(0, true);
        if (header.indexOf("GET /26/off") >= 0) handleGPIO(0, false);
        if (header.indexOf("GET /27/on") >= 0) handleGPIO(1, true);
        if (header.indexOf("GET /27/off") >= 0) handleGPIO(1, false);

        for (int i = 0; i < 5; i++) {
          String pressPath = String("GET /") + (char)(tolower(keyNames[i][0])) + "/press";
          String releasePath = String("GET /") + (char)(tolower(keyNames[i][0])) + "/release";
          if (header.indexOf(pressPath) >= 0) handleKey(i, "Pressed");
          if (header.indexOf(releasePath) >= 0) handleKey(i, "Released");
        }

        client.println("HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\n");
        client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
        client.println("<style>html{font-family:Helvetica;text-align:center}.button{padding:16px 40px;background:#4CAF50;color:white;border:none;margin:2px;font-size:30px}</style></head>");
        client.println("<body><h1>QOTE</h1>");

        for (int i = 0; i < 2; i++) {
          client.printf("<p>GPIO %d - State: %s</p>", gpios[i], gpioState[i].c_str());
          client.printf("<p><a href=\"/%d/%s\"><button class=\"button\">Turn %s GPIO %d</button></a></p>", 
                        gpios[i], gpioState[i] == "off" ? "on" : "off", gpioState[i] == "off" ? "On" : "Off", gpios[i]);
        }

        client.printf("<p>W Key: %s</p><p>X Key: %s</p><p>Y Key: %s</p><p>Z Key: %s</p><p>Shift Key: %s</p>", 
                      keyPressed[0] ? "Pressed" : "Released", keyPressed[1] ? "Pressed" : "Released", 
                      keyPressed[2] ? "Pressed" : "Released", keyPressed[3] ? "Pressed" : "Released", 
                      keyPressed[4] ? "Pressed" : "Released");

        client.printf("<p>Average Distance: <span id=\"distance\">%s</span> cm</p>", distnesValue.c_str());
        client.println("<script>let keys={w:false,a:false,s:false,d:false,shift:false};");
        client.println("['w','a','s','d','shift'].forEach(k=>{document.addEventListener('keydown',e=>{if(e.key.toLowerCase()==k&&!keys[k]){fetch('/'+k+'/press').catch(console.error);keys[k]=true}});document.addEventListener('keyup',e=>{if(e.key.toLowerCase()==k){fetch('/'+k+'/release').catch(console.error);keys[k]=false}})});");
        client.println("setInterval(()=>{fetch('/distance').then(res=>res.text()).then(data=>{document.getElementById('distance').textContent=data.trim()}).catch(err=>{console.error('Failed to fetch distance:', err);document.getElementById('distance').textContent='Error'})},5000);"); // Increased interval
        client.println("</script></body></html>");

        client.stop();
        header = "";
      }
    }
  }
}
