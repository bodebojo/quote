void sendToServer(String Light_left, String Light_right, String Distance) {
  // Check if WiFi is connected before attempting to send data
  if (WiFi.status() == WL_CONNECTED) {
    
    HTTPClient http;  // Create HTTPClient instance for HTTP requests

    // Initialize the HTTP request to the server
    http.begin("http://10.42.0.1:81/log_data.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Set content type for POST data

    // Prepare data to be sent: Light sensor values and Distance sensor value
    String postData = "Light_left=" + Light_left + "&Light_right=" + Light_right + "&Distance=" + Distance;

    // Send the data via HTTP POST and capture the response code
    int httpResponseCode = http.POST(postData);

    // Check if POST request was successful
    if (httpResponseCode > 0) {
      // If successful, get and print the response from the server
      String response = http.getString();
      // Serial.println("POST response: " + response);
    } else {
      // If failed, print the error response code
      // Serial.println("POST failed. Error: " + http.errorToString(httpResponseCode));
    }

    // Close the HTTP connection to free resources
    http.end();
  } else {
    // If WiFi is not connected, print an error message
    Serial.println("WiFi not connected, cannot send data.");
  }
}
