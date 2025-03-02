#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <ESP_Mail_Client.h>
#include <time.h>
#include "MAX30105.h"
#include "heartRate.h"

const char* ssid = "SP_Network";  // Tên mạng Wi-Fi
const char* password = "password";  // Mật khẩu Wi-Fi

const char* serverIP = "192.168.179.61";  // IP của node Linux gần nhất chạy OLSR
const int serverPort = 12345;              // Cổng để gửi dữ liệu

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

const char* emailSenderAccount = "22520089@gm.uit.edu.vn";
const char* emailSenderPassword = "password";

SMTPSession smtp;

WiFiClient client;
MAX30105 particleSensor;

String user;
String emailRecipient;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute;
int beatAvg;

bool sendEmailFlag = false; // Đánh dấu để gửi email

unsigned long lastEmailTime = 0;
unsigned long emailInterval = 300000;

// Hàm để thiết lập kết nối NTP
void initTime() {
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");  // Thiết lập múi giờ GMT+7 và máy chủ NTP
  Serial.println("Waiting for time");

  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println("Time synchronized!");
}

// Hàm để lấy thời gian hiện tại dưới dạng chuỗi
String getFormattedTime() {
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    return "Failed to obtain time";
  }

  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%d/%m/%Y %H:%M:%S", &timeInfo);
  return String(timeStringBuff);
}

void sendEmailTask(void * parameter) {
  while (true) {
    if (sendEmailFlag) {
      String emailData = "Chào " + user + "!\nNhịp tim của bạn là " + String(beatAvg) + " bpm\nSố liệu được đo vào lúc " + getFormattedTime();
      sendEmail(emailData);
      sendEmailFlag = false;  // Reset flag sau khi gửi xong
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Chờ 1 giây trước khi kiểm tra lại
  }
}

int HeartRate(long& lastBeat, byte& rateSpot) {
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  return beatAvg;
}

void sendEmail(String data) {
    smtp.debug(1);
    ESP_Mail_Session session;
    session.server.host_name = SMTP_HOST;
    session.server.port = SMTP_PORT;
    session.login.email = emailSenderAccount;
    session.login.password = emailSenderPassword;
    session.login.user_domain = "";

    SMTP_Message message;
    message.sender.name = "ESP32_HeartBeat";
    message.sender.email = emailSenderAccount;
    message.subject = "Dữ liệu sức khỏe";
    message.addRecipient("Recipient Name", emailRecipient);
    message.text.content = data;
    message.text.charSet = "us-ascii";
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

    if (!smtp.connect(&session))
      return;

    if (!MailClient.sendMail(&smtp, &message)) {
        Serial.println("Error sending email: " + smtp.errorReason());
    } else {
        Serial.println("Email sent successfully!");
    }
    smtp.closeSession();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Enter name:");
  while (user == "") {
    if (Serial.available()) {
      user = Serial.readStringUntil('\n');
      user.trim();
    }
  }

  Serial.println("Enter email:");
  while (emailRecipient == "") {
    if (Serial.available()) {
      emailRecipient = Serial.readStringUntil('\n');
      emailRecipient.trim();
    }
  }  

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to OLSR node WiFi...");
  }

  Serial.println("Connected to WiFi network");

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  xTaskCreatePinnedToCore(
    sendEmailTask,     // Hàm thực hiện task
    "Email Task",      // Tên task
    10000,             // Stack size
    NULL,              // Tham số truyền vào task
    1,                 // Độ ưu tiên của task
    NULL,              // Handle của task
    1                  // Core mà task chạy (ESP32 có 2 core, 0 và 1)
  );

  initTime();
}

void loop() {
  unsigned long currentMillis = millis();

  int currentHeartRate = HeartRate(lastBeat, rateSpot);

  if (WiFi.status() == WL_CONNECTED) {
    if (client.connect(serverIP, serverPort)) {
      String data = "Heart_rate: " + String(currentHeartRate) + " bpm";
      client.println(data);
      Serial.println("Data sent to OLSR: " + data);
      client.stop();
    } else {
      Serial.println("Unable to connect to node!");
    }
  } else {
    Serial.println("WiFi not connected!");
  }

  if (currentMillis - lastEmailTime >= emailInterval || (lastEmailTime == 0 && currentMillis - lastEmailTime >= 90000)) {
    sendEmailFlag = true;  // Kích hoạt task gửi email
    lastEmailTime = currentMillis;
  }
}
