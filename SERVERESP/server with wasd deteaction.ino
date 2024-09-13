// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid = "CoachGroepTiuri";
const char* password = "CoachGroepTiuri";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Variables to store WASD key states
String wState = "Released";
String aState = "Released";
String sState = "Released";
String dState = "Released";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(9600);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }

            // Check for keyboard capture (WASD)
            if (header.indexOf("GET /w/press") >= 0) {
              wState = "Pressed";
              Serial.println("W key pressed");
            } else if (header.indexOf("GET /w/release") >= 0) {
              wState = "Released";
              Serial.println("W key released");
            }
            if (header.indexOf("GET /a/press") >= 0) {
              aState = "Pressed";
              Serial.println("A key pressed");
            } else if (header.indexOf("GET /a/release") >= 0) {
              aState = "Released";
              Serial.println("A key released");
            }
            if (header.indexOf("GET /s/press") >= 0) {
              sState = "Pressed";
              Serial.println("S key pressed");
            } else if (header.indexOf("GET /s/release") >= 0) {
              sState = "Released";
              Serial.println("S key released");
            }
            if (header.indexOf("GET /d/press") >= 0) {
              dState = "Pressed";
              Serial.println("D key pressed");
            } else if (header.indexOf("GET /d/release") >= 0) {
              dState = "Released";
              Serial.println("D key released");
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server with Keyboard Control</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            if (output26State == "off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            if (output27State == "off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display the state of the W, A, S, D keys
            client.println("<p>W Key: " + wState + "</p>");
            client.println("<p>A Key: " + aState + "</p>");
            client.println("<p>S Key: " + sState + "</p>");
            client.println("<p>D Key: " + dState + "</p>");

            // JavaScript to handle keyboard capture
           w client.println("<script>");
            client.println("wpressed = false");
            client.println("document.addEventListener('keydown', function(event) {");
            client.println("  if (event.key === 'w' || event.key === 'W') { if (!wpressed) { fetch('/w/press'); wpressed = true; } }");
            client.println("  if (event.key === 'a' || event.key === 'A') { fetch('/a/press'); }");
            client.println("  if (event.key === 's' || event.key === 'S') { fetch('/s/press'); }");
            client.println("  if (event.key === 'd' || event.key === 'D') { fetch('/d/press'); }");
            client.println("});");
            client.println("document.addEventListener('keyup', function(event) {");
            client.println("  if (event.key === 'w' || event.key === 'W') { fetch('/w/release'); wpressed = false; }");
            client.println("  if (event.key === 'a' || event.key === 'A') { fetch('/a/release'); }");
            client.println("  if (event.key === 's' || event.key === 'S') { fetch('/s/release'); }");
            client.println("  if (event.key === 'd' || event.key === 'D') { fetch('/d/release'); }");
            client.println("});");
            client.println("</script>");
            resp = ""
            resp += "
            if (event)
            hfjdksld
            "

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    // Serial.println("Client disconnected.");
  }
}
