#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>

// WiFi credentials
#define WIFI_SSID "WIFI"
#define WIFI_PASSWORD "12345678"

// Email configuration
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "YOUR_EMAIL@gmail.com"
#define AUTHOR_PASSWORD "YOUR_APP_PASSWORD"
#define RECIPIENT_EMAIL "RECIPIENT_EMAIL@example.com"

// Pins
#define PIR_PIN D5
#define BUZZER_PIN D6

// Timing constants
const unsigned long buzzerCooldown = 5000;     // 5 seconds
const unsigned long emailCooldown = 300000;    // 5 minutes
const unsigned long alarmDuration = 10000;     // 10 seconds alarm
const unsigned long wifiRetryInterval = 30000; // 30 seconds

// Variables
SMTPSession smtp;
unsigned long lastBuzzerTrigger = 0;
unsigned long lastEmailTrigger = 0;
unsigned long lastWifiAttempt = 0;
bool buzzerActive = false;
bool wifiConnected = false;

void setup() {
  Serial.begin(9600);
  while (!Serial) {} // Wait for serial port to connect (for debugging)
  
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH); // Active low - start with buzzer off

  Serial.println("\nIntruder Alert System Initializing...");
  Serial.println("Serial monitoring active regardless of WiFi status");
  attemptWiFiConnection();
}

void loop() {
  // Handle buzzer timeout
  if (buzzerActive && (millis() - lastBuzzerTrigger >= alarmDuration)) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerActive = false;
    Serial.println("[BUZZER] Alarm period ended");
  }

  // Check for motion
  if (digitalRead(PIR_PIN) == HIGH) {
    handleMotionDetection();
  }

  // Periodic WiFi reconnection attempt
  if (!wifiConnected && (millis() - lastWifiAttempt >= wifiRetryInterval)) {
    attemptWiFiConnection();
  }
}

void handleMotionDetection() {
  // Timestamp for this detection
  unsigned long currentTime = millis();
  
  // Buzzer logic (5s cooldown)
  if (currentTime - lastBuzzerTrigger >= buzzerCooldown) {
    triggerAlarm();
    lastBuzzerTrigger = currentTime;
  }

  // Email logic (5min cooldown, only if WiFi connected)
  if (wifiConnected && (currentTime - lastEmailTrigger >= emailCooldown)) {
    sendEmailAlert();
    lastEmailTrigger = currentTime;
  }
}

void attemptWiFiConnection() {
  Serial.println("[WiFi] Attempting connection...");
  WiFi.disconnect(true); // Reset previous connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(250);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\n[WiFi] Connected!");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
  } else {
    wifiConnected = false;
    Serial.println("\n[WiFi] Connection failed");
    Serial.println("[WiFi] System will operate in offline mode");
    Serial.println("[WiFi] Next attempt in 30 seconds");
  }
  lastWifiAttempt = millis();
}

void triggerAlarm() {
  Serial.println("[ALERT] Motion detected! Buzzer activated");
  
  digitalWrite(BUZZER_PIN, LOW); // Buzzer ON
  buzzerActive = true;
  
  // Initial pulse pattern
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH); delay(100);
    digitalWrite(BUZZER_PIN, LOW); delay(100);
  }
}

void sendEmailAlert() {
  if (!wifiConnected) {
    Serial.println("[EMAIL] Cannot send - no WiFi connection");
    return;
  }

  Serial.println("[EMAIL] Preparing to send alert...");
  
  // SMTP configuration
  smtp.debug(0); // Set to 1 for verbose SMTP logging
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  // Create message
  SMTP_Message message;
  message.sender.name = "Intruder Alert System";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "SECURITY ALERT: Motion Detected";
  message.addRecipient("Owner", RECIPIENT_EMAIL);

  String textMsg = "Intruder Alert System detected motion!\n\n";
  textMsg += "System Uptime: " + formatUptime(millis()) + "\n";
  textMsg += "Last Reset Reason: " + ESP.getResetReason() + "\n";
  message.text.content = textMsg.c_str();

  // Send email
  if (!smtp.connect(&session)) {
    Serial.println("[EMAIL] Connection to server failed");
    return;
  }

  if (MailClient.sendMail(&smtp, &message)) {
    Serial.println("[EMAIL] Alert sent successfully");
  } else {
    Serial.println("[EMAIL] Failed to send alert");
  }
}

String formatUptime(unsigned long milliseconds) {
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "%lud %02lu:%02lu:%02lu", 
           days, hours % 24, minutes % 60, seconds % 60);
  return String(buffer);
}