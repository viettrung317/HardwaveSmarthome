#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DHT.h> // Thư viện cho cảm biến DHT11

#define led1           D1
#define led2           D2
#define led3           D3
#define led4           D4
#define led5           D5
#define led6           D6
#define DHTPIN D7 // Chân kết nối cảm biến DHT11
#define DHTTYPE DHT11 // Loại cảm biến DHT11

#define WLAN_SSID       ".."             // Your SSID
#define WLAN_PASS       "11111111"        // Your password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" //Adafruit Server
#define AIO_SERVERPORT  1883                   

#define IO_USERNAME  "levu"
#define IO_KEY       "aio_NpKl82ZGb6Fz5IwG5NDWP08rcf24"
DHT dht(DHTPIN, DHTTYPE);

// Khai báo và khởi tạo các biến cho dữ liệu cảm biến
float temperature = 0;
float humidity = 0;
//WIFI CLIENT
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_KEY);

Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME"/feeds/led01"); // Feeds name should be same everywhere
Adafruit_MQTT_Subscribe Light2 = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME"/feeds/led02");
Adafruit_MQTT_Subscribe Light3 = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME"/feeds/led03");
Adafruit_MQTT_Subscribe Light4 = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME"/feeds/led04");
Adafruit_MQTT_Subscribe Light5 = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME"/feeds/led05");
Adafruit_MQTT_Subscribe Light6 = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME"/feeds/led06");


void MQTT_connect();

void setup() {
  Serial.begin(115200);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
        
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
 
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&Light2);
  mqtt.subscribe(&Light3);
  mqtt.subscribe(&Light4);
  mqtt.subscribe(&Light5);
  mqtt.subscribe(&Light6);
}

void loop() {
 
  MQTT_connect();
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
    if (!isnan(humidity) && !isnan(temperature)) {
   // Gửi giá trị nhiệt độ và độ ẩm lên Adafruit.io qua MQTT
  Adafruit_MQTT_Publish temperaturePub = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/temperature");
  Adafruit_MQTT_Publish humidityPub = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/humidity");
  temperaturePub.publish(temperature);
  humidityPub.publish(humidity);
  }
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {

    if (subscription == &Light1) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light1.lastread);
      int Light1_State = atoi((char *)Light1.lastread);
      digitalWrite(led1, Light1_State);
    }
    if (subscription == &Light2) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light2.lastread);
      int Light2_State = atoi((char *)Light2.lastread);
      digitalWrite(led2, Light2_State);
    }
        if (subscription == &Light3) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light3.lastread);
      int Light3_State = atoi((char *)Light3.lastread);
      digitalWrite(led3, Light3_State);
    }
        if (subscription == &Light4) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light4.lastread);
      int Light4_State = atoi((char *)Light4.lastread);    
      digitalWrite(led4,Light4_State);
    }
        if (subscription == &Light5) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light5.lastread);
      int Light5_State = atoi((char *)Light5.lastread);
      digitalWrite(led5, Light5_State);
    }
        if (subscription == &Light6) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light6.lastread);
      int Light6_State = atoi((char *)Light6.lastread);
      digitalWrite(led6, Light6_State);
    }
  }
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); 
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
