//  Khai báo Template ID và Tên project trên nền tảng Blynk
#define BLYNK_TEMPLATE_ID "TMPL64UTRbSGB"
#define BLYNK_TEMPLATE_NAME "Thung Rac Thong Minh"

//  Thư viện hỗ trợ WiFi và Blynk cho ESP8266
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//  Thông tin kết nối WiFi và token xác thực từ Blynk
char auth[] = "nCEZCxep_CNuW9bWJaU14XLuMy3ITcLm"; // code Blynk cung cấp
char ssid[] = "IOT_VIPPRO";                       // Tên WiFi
char pass[] = "12345678";                         // Mật khẩu WiFi
// cái này copy nguyên trên blynk dán vào, hết ! 

//  Khai báo chân nối với 3 cảm biến siêu âm (Trig và Echo)
#define trig1 D5
#define echo1 D6
#define trig2 D1
#define echo2 D2
#define trig3 D7
#define echo3 D8

//  Chiều cao của thùng rác (đơn vị cm, dùng để tính phần trăm đầy)
const int height = 9;

//  Cờ cảnh báo để đảm bảo chỉ gửi thông báo 1 lần khi thùng đầy
bool warned1 = false, warned2 = false, warned3 = false;


//  Hàm đo khoảng cách bằng cảm biến siêu âm
long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);          // Reset xung
  delayMicroseconds(2);                // Chờ 2 micro giây
  // đảm bảo mức LOW, tắt hết, không chạy

  digitalWrite(trigPin, HIGH);         // Gửi xung siêu âm
  delayMicroseconds(10);              // Giữ xung 10 micro giây
  digitalWrite(trigPin, LOW);         // Tắt xung
  // phát siêu âm rồi tắt
  
  
  long thoiGianPhanXa = pulseIn(echoPin, HIGH, 30000);

  // Nếu không có sóng phản hồi (timeout) → trả về 999 để đánh dấu lỗi
  if (thoiGianPhanXa == 0) return 999;

  //  Tính khoảng cách: 
  // Tốc độ âm thanh ≈ 0.034 cm/us. Nhân thời gian rồi chia 2 (vì sóng đi 1 lượt và về 1 lượt)
  return thoiGianPhanXa * 0.034 / 2; 

}

void setup() {
  //Serial.begin(9600);                 // Khởi động Serial để debug
  Blynk.begin(auth, ssid, pass);      // Kết nối Blynk + WiFi

  //  Khai báo chân input/output cho 3 cảm biến
  pinMode(trig1, OUTPUT); pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT); pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT); pinMode(echo3, INPUT);
}

void loop() {
  Blynk.run();                        // Bắt buộc gọi để duy trì kết nối Blynk

  //  Đọc khoảng cách từ 3 cảm biến
  long d1 = readDistanceCM(trig1, echo1);
  long d2 = readDistanceCM(trig2, echo2);
  long d3 = readDistanceCM(trig3, echo3);

  //  Chuyển khoảng cách thành phần trăm đầy
  int p1 = constrain(map(height - d1, 0, height, 0, 100), 0, 100);
  int p2 = constrain(map(height - d2, 0, height, 0, 100), 0, 100);
  int p3 = constrain(map(height - d3, 0, height, 0, 100), 0, 100);

  //  In giá trị phần trăm ra Serial để theo dõi
  //Serial.print("Thung 1: "); Serial.print(p1); Serial.println("% day");
  //Serial.print("Thung 2: "); Serial.print(p2); Serial.println("% day");
  //Serial.print("Thung 3: "); Serial.print(p3); Serial.println("% day");
  //Serial.println("----------------------------");

  // Gửi dữ liệu phần trăm đầy lên Blynk (Virtual Pin V1 → V3)
  Blynk.virtualWrite(V1, p1);
  Blynk.virtualWrite(V2, p2);
  Blynk.virtualWrite(V3, p3);

  // Gửi cảnh báo nếu % đầy vượt ngưỡng (ví dụ: 75%)
  if (p1 >= 75 && !warned1) {
    Blynk.logEvent("full_bin1", "Thung1 da day!"); // Log event trên Blynk
    warned1 = true;  // Ghi nhận đã cảnh báo
  } else if (p1 < 75) warned1 = false; // Nếu đã đổ rác → cho phép cảnh báo lại

  if (p2 >= 75 && !warned2) {
    Blynk.logEvent("full_bin2", "Thung2 da day!");
    warned2 = true;
  } else if (p2 < 75) warned2 = false;

  if (p3 >= 75 && !warned3) {
    Blynk.logEvent("full_bin3", "Thung3 da day!");
    warned3 = true;
  } else if (p3 < 75) warned3 = false;

  delay(1000);  //  Đợi 1 giây trước khi đọc lần tiếp theo
}
