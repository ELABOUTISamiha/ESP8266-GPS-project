#include <ESP8266WiFi.h>
#include <TinyGPS++.h> // Tiny GPS Plus Library
#include <WiFiClient.h>
#include <SoftwareSerial.h> 
#include <Adafruit_SSD1306.h>  // Adafruit oled library for display
#include <ESP8266HTTPClient.h>
#define OLED_RESET LED_BUILTIN
const char* ssid = "INPT-TEST";
const char* password = "";
Adafruit_SSD1306 display(OLED_RESET);
static const int RXPin = 4, TXPin = 5;                // Ublox 6m GPS module to pins 12 and 13
static const uint32_t GPSBaud = 9600; 
const double Home_LAT = 33.9772127;                      // Your Home Latitude
const double Home_LNG = -6.8919605;                     // Your Home Longitude
TinyGPSPlus gps;                                        // Create an Instance of the TinyGPS++ object called gps
SoftwareSerial ss(RXPin, TXPin);
WiFiClient client;
IPAddress server(10,5,7,87);
void setup() {
  // init liaison serie
  Serial.begin(115200);
  delay(10);
  //display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();                                // Clear OLED display                            
  display.setTextSize(1);                               // Set OLED text size to small
  display.setTextColor(WHITE);                          // Set OLED color to White
  display.setCursor(0,0);                               // Set cursor to 0,0
  display.println("GPS example");  
  display.println(TinyGPSPlus::libraryVersion());
  display.display();                                     // Update display
  delay(1500);                                          // Pause 1.5 seconds  
  ss.begin(GPSBaud);                                    // Set Software Serial Comm Speed to 9600    
    // connexion au wifi
  Serial.println();
  Serial.print("lancement connexion au reseau wifi :  ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("connexion au WiFi OK");
  Serial.print("IP Address: ");
   Serial.println(WiFi.localIP());
   
  
}
void loop() {
  //display oled
   // affiche sur OLED
  display.clearDisplay();
  display.setCursor(0,0); 
  //display.print("Latitude  : ");
  //display.println(gps.location.lat(), 13);
  //display.print("Longitude : ");
  //display.println(gps.location.lng(), 12);
  //display.print("Satellites: ");
  //display.println(gps.satellites.value());
  //display.print("Elevation : ");
  //display.print(gps.altitude.feet());
  //display.println("ft"); 
  display.print("Time UTC  : ");
  display.print(gps.time.hour());                       // GPS time UTC 
  display.print(":");
  display.print(gps.time.minute());                     // Minutes
  display.print(":");
  display.println(gps.time.second());                   // Seconds
  display.print("Heading   : ");
  display.println(gps.course.deg());
  display.print("Speed     : ");
  display.println(gps.speed.mph());
  
  unsigned long Distance_To_Home = (unsigned long)TinyGPSPlus::distanceBetween(gps.location.lat(),gps.location.lng(),Home_LAT, Home_LNG);
  display.print("KM to Home: ");                        // Have TinyGPS Calculate distance to home and display it
  display.print(Distance_To_Home);
  display.display();                                     // Update display
  delay(200); 
  smartDelay(500);                                      // Run Procedure smartDelay

  if (millis() > 5000 && gps.charsProcessed() < 10)
    display.println(F("No GPS data received: check wiring"));

    
  //client connection
  if (client.connect(server,80)) {
    Serial.println("--> connection ok\n");
    client.print("GET /write_data.php?");
    client.print("latitude=");
    client.print(gps.location.lat());
    client.print("&&");
    client.print("longitude=");
    client.print(gps.location.lng());
    client.print("&&");
    client.print("satellite=");
    client.print(gps.satellites.value());
    client.print("&&");
    client.print("elevation=");
    client.print(gps.altitude.feet());
    client.print("&&");
    client.print("heading=");
    client.print(gps.course.deg());
    client.print("&&");
    client.print("speed=");
    client.print(gps.speed.mph());
    client.println(" HTTP/1.1"); // Part of the GET request
    client.print( "Host: " );
    client.println(server);
    client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
    client.println(); // Empty line
    client.println(); // Empty line
    client.stop();    // Closing connection to server
    Serial.println("--> finished transmission\n"); 
    delay(10000);  
  }
    else {
    // If Arduino can't connect to the server (your computer or web page)
    Serial.println("--> connection failed\n");
  }
  delay(10000);
}
static void smartDelay(unsigned long ms)                // This custom version of delay() ensures that the gps object is being "fed".
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}


