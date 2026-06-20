#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// Kết nối WiFi
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ThingSpeak
unsigned long myChannelNumber = 123456; // Thay bằng channel của bạn
const char * myWriteAPIKey = "YOUR_API_KEY";

WiFiClient client;
Servo servo;

const int trigPin = D5;
const int echoPin = D6;
const int servoPin = D7;

long duration;
int distance;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.attach(servoPin);
  servo.write(0); // đóng nắp

  // Đợi kết nối WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  // Đo khoảng cách
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Khoảng cách: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Nếu có người lại gần => mở nắp
  if (distance < 20) {
    servo.write(90); // mở nắp
    delay(3000);
    servo.write(0);  // đóng lại
  }

  // Gửi dữ liệu lên ThingSpeak
  ThingSpeak.setField(1, distance); // Field 1 là khoảng cách
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  // Nếu rác đầy (dưới 10cm) thì cảnh báo
  if (distance < 10) {
    Serial.println("CẢNH BÁO: RÁC ĐẦY!");
    // Có thể dùng thêm buzzer hoặc LED
  }

  delay(15000); // chờ 15 giây
}