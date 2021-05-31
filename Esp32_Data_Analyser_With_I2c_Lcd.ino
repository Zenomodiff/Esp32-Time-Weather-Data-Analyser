/*feel free to contact
 * sreeramaj53@gmail.com
 * www.youtube.com/ZenoModiff
 * last updated - time 12:18 AM - date 01 june 2021
 Github Link :-- https://github.com/Zenomodiff/Esp32-Time-Weather-Data-Analyser
 */

#include <WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "time.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ssid     = "PASTE WIFI SSID";              
const char* password = "PASTE WIFI PASSWORD";          
String APIKEY = "PASTE THE API KEY";
String CityID = "PASTE THE CITY ID";       
bool id = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

char servername[] = "api.openweathermap.org"; 
String result;
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void setup() {
  
  lcd.begin();
  Serial.begin(115200);
  Serial.print("Connecting to ");
  WiFi.mode(WIFI_STA); 

  lcd.print("Esp32 Weather ");
  lcd.setCursor(0,1);
  lcd.print("Analyser");
  delay(2000);
  Serial.println(ssid);
  lcd.clear();
  lcd.print("Wifi Name:-");
  lcd.setCursor(0,1);
  lcd.print(ssid);
  delay(2000);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  lcd.clear();
  lcd.print(".");
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.print("Wifi Ip ");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(2000);
  timeClient.begin();
  timeClient.setTimeOffset(19786);
}
void loop()                    
{
  lcd.clear();
  lcd.print("Fetching Data");
  timeClient.update();
  if (client.connect(servername, 80))
  { 
    client.println("GET /data/2.5/weather?id=" + CityID + "&units=metric&APPID=" + APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
  lcd.print("Connection Fail");
    Serial.println("connection failed");        
    Serial.println();
  }

  while (client.connected() && !client.available())
    delay(1);                                   
  while (client.connected() || client.available())
  { 
    char c = client.read();                  
    result = result + c;
  }
  client.stop();                                      
  result.replace('[', ' ');
  result.replace(']', ' ');
  char jsonArray [result.length() + 1];
  result.toCharArray(jsonArray, sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  StaticJsonDocument<1024> doc;
  DeserializationError  error = deserializeJson(doc, jsonArray);

  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }
  
  unsigned long epochTime = timeClient.getEpochTime();
  String formattedTime = timeClient.getFormattedTime();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds(); 
  String weekDay = weekDays[timeClient.getDay()];
  
  String location = doc["name"];
  String country = doc["sys"]["country"];
  int temperature = doc["main"]["temp"];
  int humidity = doc["main"]["humidity"];
  float pressure = doc["main"]["pressure"];
  
  int id = doc["id"];
  float Speed = doc["wind"]["speed"];
  int degree = doc["wind"]["deg"];
  float longitude = doc["coord"]["lon"];
  float latitude = doc["coord"]["lat"];   
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  String currentDate = String(monthDay) + "-" + String(currentMonth) + "-" + String(currentYear);
  String currentMonthName = months[currentMonth-1];
  String currentTime = String(currentHour) + ":" + String(currentMinute) + ":" + String(currentSecond);
  String currentMonthnameyear = String(currentMonthName) + "-" + String(currentYear);
  
  Serial.println();
  Serial.print("Country: ");
  Serial.println(country);
  Serial.print("Location: ");
  Serial.println(location);
  Serial.print("Time: ");
  Serial.println(currentTime);
  Serial.print("Date: ");
  Serial.println(currentDate);
  Serial.print("Month & Year: ");
  Serial.println(currentMonthnameyear);
  Serial.printf("Temperature: %d°C\r\n", temperature);
  Serial.printf("Humidity: %d %%\r\n", humidity);
  Serial.printf("Pressure: %.2f hpa\r\n", pressure);
  Serial.printf("Wind speed: %.1f m/s\r\n", Speed);
  Serial.printf("Wind degree: %d°\r\n", degree);
  Serial.printf("Longitude: %.2f\r\n", longitude);
  Serial.printf("Latitude: %.2f\r\n", latitude);
  Serial.println("");
  
  lcd.clear();
  lcd.print("Country");
  lcd.setCursor(0,1);
  lcd.print(country);
  delay(2000); 

  lcd.clear();
  lcd.print("Location");
  lcd.setCursor(0,1);
  lcd.print(location);
  delay(2000); 

  lcd.clear();
  lcd.print("Time");
  lcd.setCursor(0,1);
  lcd.print(currentTime);
  delay(2000);

  lcd.clear();
  lcd.print("Date");
  lcd.setCursor(0,1);
  lcd.print(currentDate);
  delay(2000); 

  lcd.clear();
  lcd.print("Month & year");
  lcd.setCursor(0,1);
  lcd.print(currentMonthnameyear);
  delay(2000); 

  lcd.clear();
  lcd.print("Temperature");
  lcd.setCursor(0,1);
  lcd.print(temperature);
  lcd.setCursor(4,1);
  lcd.print("*c");
  delay(2000);

  lcd.clear();
  lcd.print("Humidity");
  lcd.setCursor(0,1);
  lcd.print(humidity);
  lcd.setCursor(4,1);
  lcd.print("%");
  delay(2000); 

  lcd.clear();
  lcd.print("Pressure");
  lcd.setCursor(0,1);
  lcd.print(pressure);
  lcd.setCursor(9,1);
  lcd.print("ppi");
  delay(2000);

  lcd.clear();
  lcd.print("Wind Speed");
  lcd.setCursor(0,1);
  lcd.print(Speed);
  lcd.setCursor(5,1);
  lcd.print("m/s");
  delay(2000); 

  lcd.clear();
  lcd.print("Wind Degree");
  lcd.setCursor(0,1);
  lcd.print(degree);
  lcd.setCursor(4,1);
  lcd.print("*");
  delay(2000); 

  lcd.clear();
  lcd.print("Longitude");
  lcd.setCursor(0,1);
  lcd.print(longitude);
  delay(2000);

  lcd.clear();
  lcd.print("Latitude");
  lcd.setCursor(0,1);
  lcd.print(latitude);

  delay(2000);
}
