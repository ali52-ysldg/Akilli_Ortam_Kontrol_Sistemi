#define BLYNK_TEMPLATE_ID "TMPL6VeD-bG3W"
#define BLYNK_TEMPLATE_NAME "HavaKalitesi"
#define BLYNK_AUTH_TOKEN "i6Nz1iPkBuBi6pm9fLQN7sYctyHKVtAI"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

char ssid[] = "VODAFONE_367957";
char pass[] = "6KG9CtD5PAAchkxQ";

#define MQ135_PIN   A0
#define GREEN_LED   D5   // GPIO14
#define RED_LED     D6   // GPIO12
#define BUZZER_PIN  D7   // GPIO13

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1
#define OLED_ADDR    0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ALARM_THRESHOLD 600   

BlynkTimer timer;
bool alarmActive = false;

void readAndSend() {
  int rawValue = analogRead(MQ135_PIN);

  float ppm = map(rawValue, 0, 1024, 0, 1024);

  String durumStr;
  if (ppm < 750) {
    durumStr = "Temiz Hava";
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    alarmActive = false;
    Blynk.virtualWrite(V2, 0);
  } else if (ppm < 810) {
    durumStr = "Orta Kalite";
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    alarmActive = false;
    Blynk.virtualWrite(V2, 0);
  } else {
    durumStr = "KOTU HAVA!";
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);

    if (!alarmActive) {
      Blynk.virtualWrite(V2, 1);       
      alarmActive = true;
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("== Hava Kalitesi ==");

  display.setTextSize(2);
  display.setCursor(0, 18);
  display.print((int)ppm);
  display.println(" PPM");

  display.setTextSize(1);
  display.setCursor(0, 48);
  display.println(durumStr);

  display.display();

  // Blynk'e gönder
  Blynk.virtualWrite(V0, (int)ppm);
  Blynk.virtualWrite(V1, durumStr);

  Serial.print("Ham değer: ");
  Serial.print(rawValue);
  Serial.print(" | PPM tahmini: ");
  Serial.print((int)ppm);
  Serial.print(" | Durum: ");
  Serial.println(durumStr);
}

void setup() {
  Serial.begin(115200);

  pinMode(GREEN_LED,  OUTPUT);
  pinMode(RED_LED,    OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(GREEN_LED,  LOW);
  digitalWrite(RED_LED,    LOW);
  digitalWrite(BUZZER_PIN, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED bulunamadi!");
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 24);
  display.setTextSize(1);
  display.println("Baglaniyor...");
  display.display();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("MQ-135 isiniyor...");
  display.println("Lutfen bekleyin");
  display.display();
  delay(5000);

  timer.setInterval(2000L, readAndSend);
}

void loop() {
  Blynk.run();
  timer.run();
}
