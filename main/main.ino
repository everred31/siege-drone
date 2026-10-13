#include <WiFi.h>
#include "esp_camera.h"
#include "web.h"

WiFiServer controlServer(80);
WiFiServer streamServer(81);

TaskHandle_t streamTaskHandle;
void streamTask(void * parameter);

void setup() {
    Serial.begin(115200);

    camera_config_t config; //camera config CONFUSING AF
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = 5;
    config.pin_d1 = 18;
    config.pin_d2 = 19;
    config.pin_d3 = 21;
    config.pin_d4 = 36;
    config.pin_d5 = 39;
    config.pin_d6 = 34;
    config.pin_d7 = 35;
    config.pin_xclk = 0;
    config.pin_pclk = 22;
    config.pin_vsync = 25;
    config.pin_href = 23;
    config.pin_sscb_sda = 26;
    config.pin_sscb_scl = 27;
    config.pin_pwdn = 32;
    config.pin_reset = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;

    esp_err_t err = esp_camera_init(&config);

    if (err != ESP_OK) {
        Serial.printf("Camera init failed: 0x%x\n", err);
        return;
    }

    WiFi.softAP("BOT", "1234");

    pinMode(4, OUTPUT); //light

    pinMode(13, OUTPUT); //motor --
    pinMode(15, OUTPUT); //motor --

    pinMode(14, OUTPUT); //motor --
    pinMode(2, OUTPUT); //motor --

    analogWrite(13, 0);
    analogWrite(15, 0);

    analogWrite(14, 0);
    analogWrite(2, 0);

    controlServer.begin();
    streamServer.begin();

    xTaskCreatePinnedToCore(
        streamTask,
        "streamTask",
        8192,
        NULL,
        1,
        &streamTaskHandle,
        0
    );

    Serial.println();
    Serial.print("Access Point started! Connect to 'BOT' and open: http://");
    Serial.println(WiFi.softAPIP());
}

void streamTask(void * parameter) {
  for (;;) {
    WiFiClient client = streamServer.available();

    if (client) {
      Serial.println("Stream client connected");

      String currentLine = "";
      while (client.connected() && client.available() == 0) {
        delay(1);
      }
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          if (c == '\n') {
            if (currentLine.length() == 0) break;
            currentLine = "";
          } else if (c != '\r') {
            currentLine += c;
          }
        } else {
          break;
        }
      }

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: multipart/x-mixed-replace;boundary=frame");
      client.println();

      while (client.connected()) {
        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {
          Serial.println("Camera capture failed");
          break;
        }

        client.println("--frame");
        client.println("Content-Type: image/jpeg");
        client.print("Content-Length: ");
        client.print(fb->len);
        client.print("\r\n\r\n");
        client.write(fb->buf, fb->len);
        client.println();

        esp_camera_fb_return(fb);

        vTaskDelay(1 / portTICK_PERIOD_MS);
      }

      client.stop();
      Serial.println("Stream client disconnected");
    }

    vTaskDelay(2 / portTICK_PERIOD_MS);
  }
}

void loop() {
  WiFiClient client = controlServer.available();

  if (client) {
    Serial.println("Control client connect or sum shi");
    String currentLine = "";
    String req = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        req += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    if (req.indexOf("GET /forward") >= 0){
        
      analogWrite(13, 0);
      analogWrite(15, 64);

      analogWrite(14, 0);
      analogWrite(2, 64);
      client.println("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nOK");
    }

    else if (req.indexOf("GET /stop") >= 0){
        analogWrite(13, 0);
        analogWrite(15, 0);

        analogWrite(14, 0);
        analogWrite(2, 0);
        client.println("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nOK");
    }

    else if (req.indexOf("GET /backward") >= 0){
      analogWrite(13, 64);
      analogWrite(15, 0);

      analogWrite(14, 64);
      analogWrite(2, 0);
      client.println("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nOK");
    }
    else if (req.indexOf("GET /left") >= 0){
      analogWrite(13, 64);
      analogWrite(15, 0);

      analogWrite(14, 0);
      analogWrite(2, 64);
      client.println("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nOK");
    }

    else if (req.indexOf("GET /right") >= 0){
      analogWrite(13, 0);
      analogWrite(15, 64);

      analogWrite(14, 64);
      analogWrite(2, 0);
      client.println("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nOK");
    }

    else if (req.indexOf("GET /lightOn") >= 0){
      digitalWrite(4, 1);
      client.println("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nOK");
    }

    else if (req.indexOf("GET /lightOff") >= 0){
      digitalWrite(4, 0);
      client.println("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nOK");
    }

    else{
      client.println(header);
      client.println();
      client.println(page);
    }

    delay(1);
    client.stop();
  }

  delay(1);
}