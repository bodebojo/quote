#include <WiFi.h>
#include <SoftwareSerial.h>  // For software serial communication if needed

const char* ssid = "CoachGroepTiuri";
const char* password = "CoachGroepTiuri";
WiFiServer server(80);

// Password protection
const String webpagePassword = "Qote";  // Replace with your desired password

String header;
String gpioState[2] = {"off", "off"}, keyState[5] = {"Released", "Released", "Released", "Released", "Released"};
bool keyPressed[5] = {false, false, false, false, false}; // Tracks W, A, S, D, Shift keys
bool sendExtraKey[5] = {false, false, false, false, false}; // Flags to send extra key press after another key is released
const int gpios[2] = {43, 44}; // GPIO pin 26 and 27
bool authenticated = false;
String incomingData;
String distnesValue = "No Data";  // Variable to store the distance value
WiFiClient client = server.available();
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 2; i++) pinMode(gpios[i], OUTPUT);  // Set GPIOs as output
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  server.begin();
  Serial.println(WiFi.localIP());
}



void handleKey(int index, const String& state) {
  if (state == "Pressed" && !keyPressed[index]) {
    keyPressed[index] = true;
    keyState[index] = "Pressed";
    Serial.println("got index");
    if (index == 0) {
      Serial.println("W key pressed");
    } else if (index == 1) {
      Serial.println("A key pressed");
    } else if (index == 2) {
      Serial.println("S key pressed");
    } else if (index == 3) {
      Serial.println("D key pressed");
    } else {
      Serial.println("Shift key pressed");
    }

    for (int i = 0; i < 5; i++) {
      if (i != index && keyPressed[i]) {
        sendExtraKey[i] = true;
      }
    }
  } else if (state == "Released") {
    keyPressed[index] = false;
    keyState[index] = "Released";

    if (index == 0) {
      Serial.println("W key released");
    } else if (index == 1) {
      Serial.println("A key released");
    } else if (index == 2) {
      Serial.println("S key released");
    } else if (index == 3) {
      Serial.println("D key released");
    } else {
      Serial.println("Shift key released");
    }

    for (int i = 0; i < 5; i++) {
      if (sendExtraKey[i] && keyPressed[i]) {
        if (i == 0) Serial.println("W key pressed");
        else if (i == 1) Serial.println("A key pressed");
        else if (i == 2) Serial.println("S key pressed");
        else if (i == 3) Serial.println("D key pressed");
        else Serial.println("Shift key pressed");

        sendExtraKey[i] = false;
      }
    }
  }
}

void handleGPIO(int index, bool on) {
  gpioState[index] = on ? "on" : "off";
  digitalWrite(gpios[index], on ? HIGH : LOW);
  Serial.printf("GPIO %d %s\n", gpios[index], on ? "on" : "off");
}

void print_distance() {
  if (Serial.available()) {
    String incomingData = Serial.readStringUntil('\n');
    incomingData.trim();
    if (incomingData == "Average distance:") {
      distnesValue = Serial.readStringUntil('\n');
      distnesValue.trim();
      Serial.println("Distance detected");
      Serial.println(distnesValue);
      

    }
  }
}

void loop() {
  print_distance();
  WiFiClient client = server.available();
  if (!client) return;

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      header += c;
      if (c == '\n') {
        
      if (header.indexOf("GET /distance") >= 0) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/plain");
        client.println("Connection: close");
        client.println();
        client.println(distnesValue);
        break;
        }

if (header.indexOf("GET /?password=") >= 0) {
  int passwordIndex = header.indexOf("password=") + 9;
  String receivedPassword = header.substring(passwordIndex, header.indexOf(' ', passwordIndex));
  
  if (receivedPassword == webpagePassword) {
    authenticated = true;  // Password is correct
  } else {
    authenticated = false;  // Password is incorrect
  }
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

        for (int i = 0; i < 2; i++) {
          client.printf("<p>GPIO %d - State: %s</p>", gpios[i], gpioState[i].c_str());
          client.printf("<p><a href=\"/%d/%s\"><button class=\"button\">Turn %s GPIO %d</button></a></p>",
                        gpios[i], gpioState[i] == "off" ? "on" : "off", gpioState[i] == "off" ? "On" : "Off", gpios[i]);
        }

        client.printf("<p>W Key: %s</p><p>A Key: %s</p><p>S Key: %s</p><p>D Key: %s</p><p>Shift Key: %s</p>",
                      keyState[0].c_str(), keyState[1].c_str(), keyState[2].c_str(), keyState[3].c_str(), keyState[4].c_str());
        client.printf("<p>Average Distance: <span id=\"distance\">%s</span> cm</p>", distnesValue.c_str());

        client.println("<script>let keys={w:false,a:false,s:false,d:false,shift:false};['w','a','s','d','shift'].forEach(k=>{document.addEventListener('keydown',e=>{if(e.key.toLowerCase()==k&&!keys[k]){fetch('/'+k+'/press').catch(console.error);keys[k]=true}});document.addEventListener('keyup',e=>{if(e.key.toLowerCase()==k){fetch('/'+k+'/release').catch(console.error);keys[k]=false}})});");
        client.println("setInterval(()=>{fetch('/distance').then(res=>res.text()).then(data=>{document.getElementById('distance').textContent=data})},1000);</script>");
        client.println("</body></html>");
        client.stop();
        header = "";
      }
    }
  }
}