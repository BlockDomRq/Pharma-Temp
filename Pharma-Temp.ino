#include <DHT.h>
#include <DHT_U.h>


#include <WiFi.h>
#include <HTTPClient.h>


#define DHTPIN 2
#define DHTTYPE    DHT22 


static const unsigned char PROGMEM temp_bmp[] =
{ B00001100, B00000000,
  B00010010, B00000000,
  B00011010, B00000000,
  B00010010, B00000000,
  B00011010, B00000000,
  B00010010, B00000000,
  B00011010, B00000000,
  B00010010, B00000000,
  B00101101, B00000000,
  B01011110, B10000000,
  B01011110, B10000000,
  B01011110, B10000000,
  B00100001, B00000000,
  B00011110, B00000000};


static const unsigned char PROGMEM hum_bmp[] =
{ B00001100, B00000000,
  B00001100, B00000000,
  B00011110, B00000000,
  B00010110, B00000000,
  B00110111, B00000000,
  B00100111, B00000000,
  B01100111, B10000000,
  B01001111, B10000000,
  B01001111, B10000000,
  B01111111, B10000000,
  B01111111, B10000000,
  B01111111, B10000000,
  B00111111, B00000000,
  B00011110, B00000000};

DHT dht(DHTPIN, DHTTYPE);

Adafruit_PCD8544 display = Adafruit_PCD8544(1, 3, 15, 13, 12);



const char* ssid = "TP-LINK_27DE";
const char* password = "BlockDom1997";

const char* serverName = "http://192.168.1.124/esp-post-data.php";

String apiKeyValue = "Fs841yEAfgA98";
String sensorName = "DHT22";
String sensorLocation = "Living Room";

unsigned long lastTime = 0;
unsigned long lastDHT = 0;

unsigned long timerDelay = 3600000;
unsigned long DHTDelay = 2000;

float t,h;

void setup() {
  dht.begin();

  display.begin();
  display.setTextSize(1);
  display.setContrast(50);
  display.clearDisplay();
  display.display();
  
  WiFi.begin(ssid, password);
  display.setCursor(0,0);
  display.println("Baglaniyor...");
  display.display();
  while(WiFi.status() != WL_CONNECTED) {
    
    delay(500);
    display.print(".");
    display.display();
  }
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi'a");
  display.println("Baglanilan");
  display.println("IP Adresi: ");
  display.println(WiFi.localIP());
  display.display();
  delay(2000);
}

void loop() {
  //Send an HTTP POST request every 10 minutes

  if ((millis() - lastDHT ) > DHTDelay){
    t = dht.readTemperature();
    h = dht.readHumidity();

    
    if (!isnan(h) || !isnan(t)){
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(BLACK);
      display.drawBitmap(7, 6,  temp_bmp, 10, 14, 1);
      display.setCursor(19,6);
      display.println(t);
      display.drawBitmap(7, 28,  hum_bmp, 10, 14, 1);
      display.setCursor(19,28);
      display.println(h);
      display.display();
    }
    
    lastDHT = millis();
  }
  
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      
      if (isnan(h) || isnan(t)) {
        return;
      }
      if (t >= 24.8)
        t = 24.8;
      if (h >= 64.8)
        h = 64.8;
      // Prepare your HTTP POST request data
      String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                            + "&location=" + sensorLocation + "&value1=" + String(t)
                            + "&value2=" + String(h) + "";
      //Serial.print("httpRequestData: ");
      //Serial.println(httpRequestData);

      // You can comment the httpRequestData variable above
      // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
      //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");

      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

      if (httpResponseCode>0) {
        //Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
      }
      else {
        //Serial.print("Error code: ");
        //Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("WiFi");
      display.println("Baglantisi");
      display.println("Koptu");
      display.display();
    }
    lastTime = millis();
  }
}