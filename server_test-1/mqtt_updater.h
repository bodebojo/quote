void sendToServer(String Light_left, String Light_right,  String Distance) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://10.42.0.1:81/log_data.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "Light_left=" + Light_left + "&Light_right=" + Light_right + "&Distance=" + Distance;

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("POST response: " + response);
    } else {
      Serial.println("POST failed. Error: " + http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected, cannot send data.");
  }
}
