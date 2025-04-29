# Intruder-Alert-System-using-HC-SR501-PIR-Sensorwith-NodeMCU-ESP8266

A motion-activated security system using PIR sensor, buzzer, and optional email alerts with separate cooldown periods.

## Features
- 🚨 **Instant Buzzer Alarm** (5-second reactivation)
- ✉️ **Email Alerts** (5-minute cooldown)
- 📡 **Auto-reconnect WiFi**
- 📊 **Serial Monitor Diagnostics**
- 🔋 **Offline Operation Supported**

## Hardware Requirements
| Component              | Quantity |
|------------------------|----------|
| NodeMCU ESP8266        | 1        |
| HC-SR501 PIR Sensor    | 1        |
| Active Low Buzzer      | 1        |
| Breadboard & Jumper Wires | As needed |

## Circuit Diagram (ASCII)

+-------------------+ +-------------------+ +-------------------+
| | | | | |
| PIR SENSOR | | NODEMCU | | BUZZER |
| | | ESP8266 | | |
| +---------------+ | | +---------------+ | | +---------------+ |
| | VCC |----5V---->| 5V | | | | (+) | |
| | OUT |----D5---->| GPIO14 (D5) | | | | (-) | |
| | GND |----GND--->| GND | | | +---------------+ |
| +---------------+ | | | |<-------| GPIO12 (D6) | |
| | | | | | +-------------------+
+-------------------+ | +---------------+ |
+-------------------+

## Installation

1. **Hardware Setup**:
   ```bash
   # Follow the ASCII diagram above to connect:
   # PIR VCC → NodeMCU 5V
   # PIR OUT → NodeMCU D5
   # PIR GND → NodeMCU GND
   # Buzzer + → NodeMCU D6
   # Buzzer - → NodeMCU GND
   
  ## Software Setup:
  # Required Libraries:
# - ESP8266WiFi (included with ESP8266 board package)
# - ESP_Mail_Client by Mobizt

## Configure Settings:
Edit these lines in the sketch:

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define AUTHOR_EMAIL "YOUR_EMAIL@gmail.com"
#define AUTHOR_PASSWORD "YOUR_APP_PASSWORD"
#define RECIPIENT_EMAIL "RECIPIENT@example.com"

## Serial Monitor Output Example
[SYSTEM] Intruder Alert System Initialized
[WiFi] Attempting connection to "MyWiFi"...
......
[WiFi] Connected! IP: 192.168.1.150
[ALERT] Motion detected! Buzzer activated
[EMAIL] Preparing to send alert...
[EMAIL] Alert sent successfully
[BUZZER] Alarm period ended (10s elapsed)

## Customization Options
Setting	--> Default --> Value	Description
buzzerCooldown	--> 5000 ms	--> Minimum time between buzzer triggers
emailCooldown	--> 300000 ms	--> Minimum time between email alerts
alarmDuration	--> 10000 ms	--> How long buzzer stays active
wifiRetryInterval	--> 30000 ms	--> WiFi reconnection attempts
## Troubleshooting
Issue	--> Solution
Buzzer not sounding	--> Check active-low wiring
Email failures	--> Verify App Password for Gmail
False triggers	--> Adjust PIR sensitivity potentiometer
WiFi not connecting	--> Check debug(1) in SMTP config
## License
MIT License - Free for personal and educational use
