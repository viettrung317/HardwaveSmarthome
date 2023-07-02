#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include "FirebaseESP8266.h"
#include "DHT.h"

// đường dẫn đến project của bạn trên Firebase
#define FIREBASE_HOST "https://smarthome-49ec9-default-rtdb.firebaseio.com/"    
// khóa xác thực Firebase của bạn
#define FIREBASE_AUTH "S567mO63zA5HT7LNYiv383ungpOWrbR8M13dFLyF"
// tên mạng wifi của bạn
#define WIFI_SSID "S8"  
 // mật khẩu mạng wifi của bạn                      
#define WIFI_PASSWORD "12345678"  

// Khởi tạo đối tượng FirebaseESP8266
FirebaseData firebaseData; 
// Pin chứa tín hiệu điều khiển đèn
//Phòng khách
int gate = 16; 
int pkDv1 = 5;
int pkDv2= 4;
#define DHTPINPK D3
//Phòng ngủ
int pnDoor= 2;
int pnDv1 = 14;
int pnDv2 = 12;
#define DHTPINPN D7
//Phòng bếp
int pbDv1 = 15;
int pbDv2 = 3;
int gas=A0;
//thiết lập loại cảm biến
#define DHTTYPE DHT11 
// Khởi tạo biến cảm biến DHT
//Phòng khách
  DHT dhtpk(DHTPINPK, DHTTYPE);
//Phòng ngủ
  DHT dhtpn(DHTPINPN, DHTTYPE);
// Đường dẫn đến nút cơ sở dữ liệu chứa thông tin thiết bị
//Phòng khách
String pathGate = "User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/statusGate"; 
String pathPKdv1="User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/1/listDevice/0/status";
String pathPKdv2="User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/1/listDevice/1/status";
String pathPKnd="User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/1/sensorList/0/sensorParameters";
String pathPKda="User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/1/sensorList/1/sensorParameters";
//Phòng ngủ
String pathPNDoor = "User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/0/door";  
String pathPNdv1 = "User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/0/listDevice/0/status";
String pathPNdv2 = "User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/0/listDevice/1/status";
String pathPNnd="User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/0/sensorList/0/sensorParameters";
String pathPNda="User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/0/sensorList/1/sensorParameters";
//Phòng bếp
String pathPBdv1 = "User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/2/listDevice/0/status";
String pathPBdv2 = "User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/2/listDevice/1/status";
String pathGas="User/DGKzIeGxdXQK07LxQy2gFS08hFv1/homeList/0/roomList/2/sensorList/0/sensorParameters";
void setup() 
{
  Serial.begin(9600);
  delay(1000);
  //Phòng khách    
  setupdevice(gate);
  setupdevice(pkDv1);
  setupdevice(pkDv2);
  //Phòng ngủ
  setupdevice(pnDoor);
  setupdevice(pnDv1);
  setupdevice(pnDv2);
  //Phòng bếp
  setupdevice(pbDv1);
  setupdevice(pbDv2);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);            
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  //khởi tạo cảm biến DHT
  dhtpk.begin();
  dhtpn.begin();

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);//Kết nối đến firebase
}
void setupdevice(int device){
  pinMode(device, OUTPUT);
  digitalWrite(device, LOW);     // thiết bị tắt
}

 
void loop() 
{
  //Phòng khách
  getDataDv(pathGate,gate);
  getDataDv(pathPKdv1,pkDv1);
  getDataDv(pathPKdv2,pkDv2);
  setDataSensor(dhtpk,pathPKnd,pathPKda);
  //Phòng ngủ
  getDataDv(pathPNDoor,pnDoor);
  getDataDv(pathPNdv1,pnDv1);
  getDataDv(pathPNdv2,pnDv2);
  setDataSensor(dhtpn,pathPNnd,pathPNda);
  //phòng bếp
  getGas(pathGas,gas);
  getDataDv(pathPBdv1,pbDv1);
  getDataDv(pathPBdv2,pbDv2);
}
//Hàm đọc giá trị khí gas
void getGas(String path,int gas){
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 500) {
    previousMillis = currentMillis;

    int gasValue = analogRead(gas); 
    Firebase.setInt(firebaseData, path, gasValue);
    Serial.println(gasValue);
  }
}
//Hàm điều khiển bật tắt thiết bị
void getDataDv(String path,int device){
  Firebase.getBool(firebaseData, path); //Lấy dữ liệu từ firebase
  bool fireStatus = firebaseData.boolData();
  if (fireStatus == true) 
  {   
    Serial.println("device on");       
    digitalWrite(device, HIGH);       // device on
  } 
  else if (fireStatus == false) 
  {  
    Serial.println("device off");
    digitalWrite(device, LOW);     // device off
  }
  else 
  {
    Serial.println("Lỗi dữ liệu sai !!!");
  }
}

//Hàm đọc và gửi giá trị của cảm biến nhiệt độ và độ ẩm lên Firebase
void setDataSensor(DHT dht1,String path1,String path2){
  // Đọc giá trị nhiệt độ và độ ẩm từ cảm biến
  double temperature = dht1.readTemperature();
  double humidity = dht1.readHumidity();
  // làm tròn đến 1 chữ số thập phân
  temperature = round(temperature * 10) / 10;
  humidity = round(humidity * 10) / 10;

  // Kiểm tra xem đọc giá trị thành công chưa
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Nhiet do: ");

  Serial.print(temperature);

  Serial.print("*C  ");

  Serial.print("Do am: ");

  Serial.print(humidity);

  Serial.println("%  ");

  // Gửi giá trị nhiệt độ và độ ẩm lên Firebase
  Firebase.setDouble(firebaseData,path1, temperature);
  Firebase.setDouble(firebaseData,path2, humidity);

  // Đợi 0.5 giây trước khi đọc dữ liệu tiếp theo
  delay(500);

}