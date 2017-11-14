
#include <ESP8266WiFi.h>
#include <time.h>
#include <Scheduler.h>

#define WIFI_SSID "xxxxxx"
#define WIFI_PASSWORD "pppppp"

#define LINE_TOKEN "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"
#define SW1 D1
#define SW2 D2
#define SW3 D3
#define LED1 D5
#define LED2 D6
#define LED3 D7

int count1 = 0;
int count2 = 0;
int count3 = 0;
int timezone = 7 * 3600;
int dst = 0;

unsigned long SW1_T1 = 0;
unsigned long SW1_T2 = 0;
unsigned long SW2_T1 = 0;
unsigned long SW2_T2 = 0;
unsigned long SW3_T1 = 0;
unsigned long SW3_T2 = 0;

String message1 = "%E0%B8%81%E0%B8%A5%E0%B9%88%E0%B8%AD%E0%B8%87%E0%B9%80%E0%B8%8A%E0%B9%89%E0%B8%B2%E0%B9%80%E0%B8%9B%E0%B8%B4%E0%B8%94%E0%B8%AD%E0%B8%AD%E0%B8%81";
String message2 = "%E0%B8%81%E0%B8%A5%E0%B9%88%E0%B8%AD%E0%B8%87%E0%B9%80%E0%B8%97%E0%B8%B5%E0%B9%88%E0%B8%A2%E0%B8%87%E0%B9%80%E0%B8%9B%E0%B8%B4%E0%B8%94%E0%B8%AD%E0%B8%AD%E0%B8%81";
String message3 = "%E0%B8%81%E0%B8%A5%E0%B9%88%E0%B8%AD%E0%B8%87%E0%B9%80%E0%B8%A2%E0%B9%87%E0%B8%99%E0%B9%80%E0%B8%9B%E0%B8%B4%E0%B8%94%E0%B8%AD%E0%B8%AD%E0%B8%81";


void Line_Notify(String message) {
  WiFiClientSecure client;
  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  // Serial.println(req);
  client.print(req);
  delay(20);
  // Serial.println("-------------");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    Serial.println(line);
  }
  Serial.println("-------------");
}

class Task1 : public Task {
protected:
    void loop()  {
      if (count1 == 1 && SW1_T1 == 0) SW1_T1 = millis();
      else if (count1 == 2 && SW1_T2 == 0) {
        SW1_T2 = millis();
        unsigned long HIGH_Time = SW1_T2 - SW1_T1;
        if (HIGH_Time > 950 && HIGH_Time < 1050) Line_Notify(message1);
        count1 = 0;
        SW1_T1 = 0;
        SW1_T2 = 0;
      }

      time_t now = time(nullptr);
      struct tm* p_tm = localtime(&now);

      if (p_tm->tm_sec == 10) {
        digitalWrite(LED1, HIGH);
        delay(1000);
        digitalWrite(LED1, LOW);
      }
    }
} task1;

class Task2 : public Task {
protected:
    void loop()  {
      if (count2 == 1 && SW2_T1 == 0) SW2_T1 = millis();
      else if (count2 == 2 && SW2_T2 == 0) {
        SW2_T2 = millis();
        unsigned long HIGH_Time = SW2_T2 - SW2_T1;
        if (HIGH_Time > 950 && HIGH_Time < 1050) Line_Notify(message2);
        count2 = 0;
        SW2_T1 = 0;
        SW2_T2 = 0;
      }

      time_t now = time(nullptr);
      struct tm* p_tm = localtime(&now);

      if (p_tm->tm_sec == 30) {
        digitalWrite(LED2, HIGH);
        delay(3000);
        digitalWrite(LED2, LOW);
      }
    }
} task2;

class Task3 : public Task {
protected:
    void loop()  {
      if (count3 == 1 && SW3_T1 == 0) SW3_T1 = millis();
      else if (count3 == 2 && SW3_T2 == 0) {
        SW3_T2 = millis();
        unsigned long HIGH_Time = SW3_T2 - SW3_T1;
        if (HIGH_Time > 950 && HIGH_Time < 1050) Line_Notify(message3);
        count3 = 0;
        SW3_T1 = 0;
        SW3_T2 = 0;
      }

      time_t now = time(nullptr);
      struct tm* p_tm = localtime(&now);

      if (p_tm->tm_sec == 50) {
        digitalWrite(LED3, HIGH);
        delay(5000);
        digitalWrite(LED3, LOW);
      }
    }
} task3;

void Switch1() {
  ++count1;
}

void Switch2() {
  ++count2;
}

void Switch3() {
  ++count3;
}

void setup() {

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  attachInterrupt(SW1,Switch1,CHANGE);
  attachInterrupt(SW2,Switch2,CHANGE);
  attachInterrupt(SW3,Switch3,CHANGE);

  Scheduler.start(&task1);
  Scheduler.start(&task2);
  Scheduler.start(&task3);
  Scheduler.begin();
}

void loop() {
}

