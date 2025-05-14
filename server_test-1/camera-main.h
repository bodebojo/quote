// Prevent this header file from being included more than once
#pragma once

#include "esp_camera.h"       // ESP32 camera library
#include "camera_pins.h"      // Custom pin definitions for your camera module
#include <WiFi.h>             // For using WiFi and WiFiServer
extern bool cameraEnabled;
extern bool cameraFlip;


// Function declarations so other files can use these
bool initCamera();                       // Initializes the camera hardware
void streamCameraMJPEG(void *parameter); // Function to stream MJPEG frames over HTTP
void startCameraTask();                  // Starts the streaming task

// Task handler and server setup
TaskHandle_t Core0Task;                  // Handle for the MJPEG stream task
WiFiServer mjpegStreamServer(82);        // HTTP server for MJPEG streaming on port 82

// Function to initialize the camera with specific settings
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;      // LEDC channel for camera clock
  config.ledc_timer   = LEDC_TIMER_0;        // LEDC timer for camera clock
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 30000000;            // Clock speed
  config.pixel_format = PIXFORMAT_JPEG;      // JPEG output format
  config.frame_size   = FRAMESIZE_VGA;       // VGA resolution (640x480)
  config.fb_location  = CAMERA_FB_IN_DRAM;   // Frame buffer location
  config.jpeg_quality = 20;                  // JPEG quality (lower is better quality)
  config.fb_count     = 1;                   // Only one frame buffer

  // Initialize the camera and check for errors
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return false;
  }

  if (cameraFlip) {
    sensor_t * s = esp_camera_sensor_get();
      s->set_vflip(s, 1);    // Flip vertically (1 = on, 0 = off)
  }
  Serial.println("Camera init success");
  return true;
}

// This function starts the camera streaming task on Core 0
void startCameraTask() {
  if (initCamera()) { // Only start the task if camera was initialized successfully
    xTaskCreatePinnedToCore(
      streamCameraMJPEG,   // Task function
      "MJPEG Task",        // Name of task
      8192,                // Stack size in bytes
      NULL,                // Parameters
      1,                   // Task priority
      &Core0Task,          // Task handle
      0                    // Run on Core 0
    );
  }
}

// Task function that continuously streams MJPEG frames to connected clients
void streamCameraMJPEG(void *parameter) {
  if (cameraEnabled) {
  mjpegStreamServer.begin();  // Start the MJPEG HTTP server
  Serial.println("MJPEG Stream Task started on Core: " + String(xPortGetCoreID()));

  while (true) {
    WiFiClient client = mjpegStreamServer.available(); // Check for new client

    if (client) {
      Serial.println("MJPEG client connected");

      // Send MJPEG stream headers
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
      client.println();

      // Keep streaming while client is connected
      unsigned long lastFrameTime = millis();
      while (client.connected() && millis() - lastFrameTime < 5000) {  // 5s timeout
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
          Serial.println("Camera capture failed");
          delay(100);  // Prevent tight error loop
          continue;
        }

        client.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
        client.write(fb->buf, fb->len);
        client.println();
        esp_camera_fb_return(fb);

        lastFrameTime = millis();
        delay(33);  // 30 FPS cap
      }


      client.stop(); // Client disconnected
      Serial.println("MJPEG client disconnected");
    }

    vTaskDelay(10 / portTICK_PERIOD_MS); // Short delay to prevent watchdog triggers
  }
}

}