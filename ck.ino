#define BLYNK_TEMPLATE_ID "TMPL6DsPqaDKD"
#define BLYNK_TEMPLATE_NAME "wifi plug"
#define BLYNK_AUTH_TOKEN "l-fdVBNcmkVsq43EGZ_H5HZQnD-8kxJ2"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
int output_relay1 = 6;
int output_relay2 = 5;
int scl_oled = 0;
int sda_oled = 1;
int button = 2;
//Change the virtual pins according the rooms
#define VPIN_0 V0
#define VPIN_1 V1
#define VPIN_2 V2
#define VPIN_3 V3
#define VPIN_4 V4
#define VPIN_5 V5
#define VPIN_6 V6
//------------------------------------------------------

float doam;
float doC;
bool relay1_state = 1;
bool relay2_state = 1;
unsigned long time1 = 0;
unsigned long time2 = 0;

// NTP Server
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // GMT+7 (giờ Việt Nam)
const int daylightOffset_sec = 0;

//oled
#define SCREEN_WIDTH 128 // Chiều rộng của màn hình OLED
#define SCREEN_HEIGHT 64 // Chiều cao của màn hình OLED
#define OLED_RESET    -1 // Đặt reset về -1 nếu không dùng chân reset
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//oled
WiFiManager wm;
//
String alarm_event = "detect";
//

//dht11
const int DHTPIN = 3;//Chân Out của cảm biến nối chân số 5 
const int DHTTYPE = DHT11;   // Khai báo kiểu cảm biến là DHT11
DHT dht(DHTPIN, DHTTYPE); //Khai báo thư viện chân cảm biến và kiểu cảm biến
//dht11

//connect Wifi - tên wifi - mật khẩu
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[32] = "";
char pass[32] = "";
//
unsigned long ledOffTime = 0;
int delayTime1=0;
int delayTime2=0;
int delayTime3=0;
void setup() 
{   
Wire.begin(sda_oled, scl_oled);
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C là địa chỉ I2C của màn hình OLED
    //Serial.println("SSD1306 allocation failed");
    for(;;); // Dừng chương trình nếu không kết nối được
  }  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); // Màu chữ
  display.setCursor(0, 10);
  display.println("       WELCOME");
  display.setCursor(0, 20);
  display.println(" PLEASE SETUP WIFI IN");
  display.setCursor(0, 30);
  display.println("   60 SECOND TO USE");
  display.setCursor(0, 40);
  display.println("     ONLINE MODE");
  display.display();
  //delay(500);
  //Serial.println("reset after 2 second");
 
//Serial.begin(115200);
pinMode(output_relay1, OUTPUT);
pinMode(output_relay2, OUTPUT);
digitalWrite(output_relay1, 1);
digitalWrite(output_relay2, 1);
// Kết nối đến WiFi
 Blynk.config(auth);

wm.setConfigPortalTimeout(60);
bool res;
res = wm.autoConnect("ESP32-CONFIG","12345678");
    if(!res) {
        display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); // Màu chữ
  display.setCursor(0, 20);
  display.println("   FAIL SETUP WIFI");
  display.println("   ON OFFLINE MODE");
  display.display();
  delay(1000);
    } 
    else {
              display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); // Màu chữ
  display.setCursor(0, 20);
  display.println("  SUCCESS SETUP WIFI");
  display.println("    ON ONLINE MODE");
  display.display();
  delay(1000);
       strcpy(ssid, WiFi.SSID().c_str());  // Lưu SSID vào biến ssid
        strcpy(pass, WiFi.psk().c_str());   // Lưu mật khẩu vào biến pass
        //if you get here you have connected to the WiFi    
        //Serial.println("Kết nối Wifi thành công");
        Blynk.connect();
    }
 // WiFi.begin(ssid, pass);
 // if (WiFi.status()==WL_CONNECTED) {
 //Blynk.connect();
 // }

dht.begin(); //Khởi động cảm biến

pinMode(button, INPUT_PULLUP);

  // Cấu hình thời gian NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

if (WiFi.status() == WL_CONNECTED) {
  Blynk.virtualWrite(VPIN_2, 1);
Blynk.virtualWrite(VPIN_3, 1);
}
}
//nút nhấn
BLYNK_WRITE(VPIN_2) {

relay1_state = param.asInt();

digitalWrite(output_relay1, relay1_state);
}
//nút nhấn
BLYNK_WRITE(VPIN_3) {

relay2_state = param.asInt();

digitalWrite(output_relay2, relay2_state);

} 
// Hàm Blynk để hẹn giờ tắt LED
BLYNK_WRITE(VPIN_4) {  // V1 là nút trên Blynk app
  delayTime1 = param.asInt();  // Lấy giá trị từ slider trong Blynk (giới hạn từ 0-3600 giây)

  // Nếu giá trị delayTime > 0, bật LED và bắt đầu hẹn giờ
  if (delayTime1 > 0) {
    
    ledOffTime = millis() + delayTime1 * 1000 * 60 * 60 + delayTime2 * 1000 * 60 + delayTime3 * 1000;  // Cập nhật thời gian tắt LED
  } 
}
// Hàm Blynk để hẹn giờ tắt LED
BLYNK_WRITE(VPIN_5) {  // V1 là nút trên Blynk app
  delayTime2 = param.asInt();  // Lấy giá trị từ slider trong Blynk (giới hạn từ 0-3600 giây)

  // Nếu giá trị delayTime > 0, bật LED và bắt đầu hẹn giờ
  if (delayTime2 > 0) {
    
    ledOffTime = millis() + delayTime1 * 1000 * 60 * 60 + delayTime2 * 1000 * 60 + delayTime3 * 1000;  // Cập nhật thời gian tắt LED
  } 
}
// Hàm Blynk để hẹn giờ tắt LED
BLYNK_WRITE(VPIN_6) {  // V1 là nút trên Blynk app
  delayTime3 = param.asInt();  // Lấy giá trị từ slider trong Blynk (giới hạn từ 0-3600 giây)

  // Nếu giá trị delayTime > 0, bật LED và bắt đầu hẹn giờ
  if (delayTime3 > 0) {
    
    ledOffTime = millis() + delayTime1 * 1000 * 60 * 60 + delayTime2 * 1000 * 60 + delayTime3 * 1000;  // Cập nhật thời gian tắt LED
  } 
}
void loop() 
{ 
  if (WiFi.status() == WL_CONNECTED) {
Blynk.run();
  } else {
    WiFi.disconnect();
    WiFi.begin(ssid, pass);
    if (WiFi.status() == WL_CONNECTED) {
      //Serial.println("Kết nối Wifi thành công.");
      Blynk.connect();
    } else {
      //Serial.println("Kết nối Wifi thất bại.");
    }
  }
  
  if ( (unsigned long) (millis() - time1) > 600 )
    {
        cambiendht11();
        time1 = millis();
    }
  if ( (unsigned long) (millis() - time2) > 600 )
    {
        hienthioled();
        time2 = millis();
    }
  if (digitalRead(button)==0) {
  display.clearDisplay(); // Xóa màn hình
  display.setTextSize(1);      // Cỡ chữ
  display.setTextColor(SSD1306_WHITE); // Màu chữ
  display.setCursor(0, 30);
  display.print(" RESET DEVICE SUCCESS");
  display.display();
  //Serial.println("reset after 2 second");
  delay(1000);
  
  wm.resetSettings();
  ESP.restart();
  }
   //Kiểm tra thời gian hẹn giờ để tắt LED
  if (millis() >= ledOffTime && ledOffTime > 0) {
    digitalWrite(output_relay1, 0);
digitalWrite(output_relay2, 0);
Blynk.virtualWrite(VPIN_2, 0);
Blynk.virtualWrite(VPIN_3, 0);
relay1_state=0;
relay2_state=0;
    ledOffTime = 0;  // Reset thời gian hẹn giờ
  }
}

void cambiendht11() {

  doam = dht.readHumidity(); //Đọc độ ẩm

  doC = dht.readTemperature(); //Đọc nhiệt độ C

  

  // Kiểm tra cảm biến có hoạt động hay không
  if (isnan(doam) || isnan(doC) ) 
  {
    //Serial.println("Không có giá trị trả về từ cảm biến DHT");
    return;
  }
  if (doC>50) {
    digitalWrite(output_relay1, 0);
digitalWrite(output_relay2, 0);
relay1_state=0;
relay2_state=0;
if (WiFi.status() == WL_CONNECTED) {
  String text = "Detect high temperature, auto off plug.";
Blynk.logEvent(alarm_event, text);
Blynk.virtualWrite(VPIN_2, 0);
Blynk.virtualWrite(VPIN_3, 0);
}
  }
  if (WiFi.status() == WL_CONNECTED) {
 Blynk.virtualWrite(VPIN_0, doC);
Blynk.virtualWrite(VPIN_1, doam);
  }
  
 // Serial.print("Độ ẩm: ");
  //Serial.print(doam);
 
  //Serial.print("%  Nhiệt độ: ");
 // Serial.print(doC);
  
 // Serial.println("°C");
  
}
void hienthioled() {
      // Lấy thời gian thực
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Không lấy được thời gian");
    //return;
  }
  // Hiển thị thời gian và ngày trên OLED
  char timeStr[10];
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);

  char dateStr[12];
  strftime(dateStr, sizeof(dateStr), "%d-%m-%Y", &timeinfo);
  display.clearDisplay(); // Xóa màn hình

  display.setTextSize(1);      // Cỡ chữ
  display.setTextColor(SSD1306_WHITE); // Màu chữ

  // 
  display.setCursor(0, 0);
  display.print("TEMP: ");
  display.print(doC);
  display.println(" oC");

  // 
  display.setCursor(0, 10);
  display.print("HUMIDITY: ");
  display.print(doam);
  display.println(" %");

    // 
  display.setCursor(0, 20);
  display.print("TIME: ");
    if (WiFi.status() == WL_CONNECTED) {
 display.println(timeStr);
  } else {
  display.println("------");
  }
  // 
  display.setCursor(0, 30);
  display.print("DATE: ");
     if (WiFi.status() == WL_CONNECTED) {
 display.println(dateStr);
  } else {
  display.println("------");
  }
    // 
  display.setCursor(0, 40);
  display.print("  PLUG 1:   ");
  display.println("PLUG 2:");
  //
  display.setCursor(0, 50);
  display.print("    ");
  if (relay1_state==1) {
    display.print("ON");
  } else {
display.print("OFF");
  }
  display.setCursor(84, 50);
  if (relay2_state==1) {
    display.print("ON");
  } else {
display.print("OFF");
  }
  display.display(); // Hiển thị nội dung
}
