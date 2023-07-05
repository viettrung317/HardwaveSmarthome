
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define RAIN_SENSOR_PIN D4 // Chân kết nối cảm biến báo mưa
#define ULTRASONIC_TRIG_PIN D5 // Chân kết nối cảm biến siêu âm - Trig
#define ULTRASONIC_ECHO_PIN D6 // Chân kết nối cảm biến siêu âm - Echo
#define PIR_PIN D7 // Chân kết nối cảm biến chuyển động SR501
#define LM393_PIN D8 // Chân kết nối cảm biến LM393
#define GAS_SENSOR_PIN A0 // Chân kết nối cảm biến khí gas MQ2

#define SPEAKER_PIN0 D0 // Chân kết nối LED cảnh báo của LM393 có ánh sáng hồng ngoại hoặc vật di cản di chuyển
#define SPEAKER_PIN1 D1 // Chân kết nối LED cảnh báo của vật thể hồng ngoại di chuyển PIR
#define SPEAKER_PIN2 D2 // Chân kết nối Còi cảnh báo của cảm biến báo Ultrasonic khoảng cách dưới 10cm
#define SPEAKER_PIN3 D3 // Chân kết nối còi cảnh báo của MQ2 và rain

#define WLAN_SSID ".." // Tên mạng WiFi
#define WLAN_PASS "11111111" // Mật khẩu WiFi
  
// Khai báo đối tượng WiFiClient để kết nối WiFi
WiFiClient client;

void setup() {
Serial.begin(115200);

// Kết nối với mạng WiFi
WiFi.begin(WLAN_SSID, WLAN_PASS);
while (WiFi.status() != WL_CONNECTED) {
delay(5000);
Serial.print(".");
}
Serial.println();
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());

pinMode(PIR_PIN, INPUT);
pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
pinMode(ULTRASONIC_ECHO_PIN, INPUT);
pinMode(GAS_SENSOR_PIN, INPUT);
pinMode(RAIN_SENSOR_PIN, INPUT);
pinMode(LM393_PIN, INPUT);
pinMode(SPEAKER_PIN0, OUTPUT);
pinMode(SPEAKER_PIN1, OUTPUT);
pinMode(SPEAKER_PIN2, OUTPUT);
pinMode(SPEAKER_PIN3, OUTPUT);

}

void loop() {
// Đọc giá trị từ cảm biến chuyển động SR501
int motion = digitalRead(PIR_PIN);

// Đọc khoảng cách từ cảm biến siêu âm
long duration, distance;
digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
delayMicroseconds(2);
digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
distance = duration * 0.034 / 2;

// Đọc giá trị từ cảm biến khí gas MQ2
int gasValue = analogRead(GAS_SENSOR_PIN);
Serial.println(gasValue);
// Đọc trạng thái từ cảm biến báo mưa
int rainStatus = digitalRead(RAIN_SENSOR_PIN);

// Đọc trạng thái từ cảm biến LM393
int lm393Status = digitalRead(LM393_PIN);

  if (motion == HIGH || distance < 10 || gasValue > 400 || rainStatus == HIGH || lm393Status == HIGH) {
    if (motion == HIGH) {
    // Cảnh báo khi phát hiện chuyển động
    Serial.println("Có vật thể chuyển động!!!!!!!");
    tone(SPEAKER_PIN1, 1000, 1000); // Kích hoạt LED cảnh báo trong 100ms
    delay(1); // Chờ 10ms trước khi tiếp tục
    }
    if (distance < 10) {
      // Cảnh báo khi khoảng cách nhỏ hơn 10cm
      Serial.println("Khoảng cách nhỏ hơn 10cm!");
      tone(SPEAKER_PIN2, 1000, 1000);  // Kích hoạt âm thanh cảnh báo trong 1 giây
      delay(1);  // Chờ 10ms trước khi tiếp tục
    }
        

    if (gasValue > 200) {
      // Cảnh báo khi khí gas vượt ngưỡng
      Serial.println("Khí gas vượt ngưỡng!");
      tone(SPEAKER_PIN3, 1000, 1000);  // Kích hoạt âm thanh cảnh báo trong 1 giây
      delay(1);  // Chờ 10ms trước khi tiếp tục
      }
      
    if (rainStatus == LOW) {
      // Cảnh báo khi phát hiện trạng thái báo mưa
      Serial.println("Trời Mưa!!!!!!!!!!!");
      tone(SPEAKER_PIN3, 1000, 1000);  // Kích hoạt âm thanh cảnh báo trong 1 giây
      delay(1);  // Chờ 10ms trước khi tiếp tục
    }
  
    if (lm393Status == HIGH) {
      // Cảnh báo khi phát hiện trạng thái từ cảm biến LM393
      Serial.println("Cảnh báo từ cảm biến LM393!");
      tone(SPEAKER_PIN0, 1000, 1000);  // Kích hoạt âm thanh cảnh báo trong 1 giây
      delay(1);  // Chờ 10ms trước khi tiếp tục
    }
  
    // Gửi thông báo qua Wi-Fi
    if (client.connect("YOUR_SERVER_IP", 80)) {
      client.println("GET /send_notification HTTP/1.1");
      client.println("Host: YOUR_SERVER_IP");
      client.println("Connection: close");
      client.println();
      delay(100);
      client.stop();
    }
}
delay(100); // Chờ 100ms trước khi đọc lại cảm biến
}
