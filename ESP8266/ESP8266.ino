#define BLYNK_TEMPLATE_ID "TMPL6OHAsEUiI"
#define BLYNK_TEMPLATE_NAME "Smart Trash Project"
#define BLYNK_AUTH_TOKEN "Yd3d1eigldodVvM1aL4WEp39q_aafHa3"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "IOT_VIPPRO";
char pass[] = "12345678";

#define trig1 D5
#define echo1 D6
#define trig2 D1
#define echo2 D2
#define trig3 D7
#define echo3 D8

#define FULL_LEVEL_CM 3
#define WARNING_LEVEL_CM 6
#define USING_LEVEL_CM 15
#define MAX_DISTANCE_CM 20

long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

int calculateFullnessPercent(long distance) {
  if (distance >= MAX_DISTANCE_CM) return 0;
  if (distance <= 0) return 100;
  float fullness = ((float)(MAX_DISTANCE_CM - distance) / MAX_DISTANCE_CM) * 100.0;
  return (int)fullness;
}

void setup() {
  Serial.begin(9600);
  pinMode(trig1, OUTPUT); pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT); pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT); pinMode(echo3, INPUT);

  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  long d[3];
  int p[3];

  d[0] = readDistanceCM(trig1, echo1);
  d[1] = readDistanceCM(trig2, echo2);
  d[2] = readDistanceCM(trig3, echo3);

  p[0] = calculateFullnessPercent(d[0]);
  p[1] = calculateFullnessPercent(d[1]);
  p[2] = calculateFullnessPercent(d[2]);

  Serial.print("Bin 1: "); Serial.print(d[0]); Serial.print(" cm ("); Serial.print(p[0]); Serial.print("%)\t");
  Serial.print("Bin 2: "); Serial.print(d[1]); Serial.print(" cm ("); Serial.print(p[1]); Serial.print("%)\t");
  Serial.print("Bin 3: "); Serial.print(d[2]); Serial.print(" cm ("); Serial.print(p[2]); Serial.println("%)");

  Blynk.virtualWrite(V0, d[0]);
  Blynk.virtualWrite(V1, d[1]);
  Blynk.virtualWrite(V2, d[2]);

  Blynk.virtualWrite(V6, p[0]);
  Blynk.virtualWrite(V7, p[1]);
  Blynk.virtualWrite(V8, p[2]);

  for (int i = 0; i < 3; i++) {
    String status;
    int virtualPin = V3 + i;

    if (d[i] <= FULL_LEVEL_CM) {
      status = "T" + String(i+1) + ": ĐẦY!";
      Blynk.logEvent("full", "Thùng " + String(i+1) + " đã đầy!");
    } else if (d[i] <= WARNING_LEVEL_CM) {
      status = "T" + String(i+1) + ": CẢNH BÁO!";
      Blynk.logEvent("alert", "Thùng " + String(i+1) + " sắp đầy!");
    } else if (d[i] <= USING_LEVEL_CM) {
      status = "T" + String(i+1) + ": SỬ DỤNG";
    } else {
      status = "";
    }

    Blynk.virtualWrite(virtualPin, status);
  }

  delay(1000); // Gửi lại mỗi 5 giây
}
