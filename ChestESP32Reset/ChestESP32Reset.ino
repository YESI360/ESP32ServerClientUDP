//
//SENSOR PECHO
//
//  ______     ______     __   __     ______     ______     ______
///\  ___\   /\  ___\   /\ "-.\ \   /\  ___\   /\  __ \   /\  == \      
//  \ \___  \  \ \  __\   \ \ \-.  \  \ \___  \  \ \ \/\ \  \ \  __<
//   \/\_____\  \ \_____\  \ \_\\"\_\  \/\_____\  \ \_____\  \ \_\ \_\    
//    \/_____/   \/_____/   \/_/ \/_/   \/_____/   \/_____/   \/_/ /_/    V6
//
/*
    This sketch connects via WiFI + websockets to a ESP32S AP and sends sensor data as UDP packets
    Alexandre Castonguay acastonguay@artengine.ca For Yesica Duarte. Thank you to EWMA library creator Arsen Torbarina.
*/

#include <Plaquette.h>
#include <Thresholder.h>

// The analog input.35 = R1 ///////////////////////33NADA/////
//AnalogIn in(32); // 32 = R3 1K
AnalogIn in(34); // 34 = R2 2K

// The serial output.
StreamOut serialOut;

AdaptiveNormalizer norm;

Thresholder peakDetector(0.7, THRESHOLD_RISING, 0.5);
//Thresholder peakDetector(0.7, THRESHOLD_FALLING, 0.5);

////////////// TFT SCREEN ///////////////////////////////////////
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
///////////////////////////////////////////////////////////////////
//// WiFi UDP connection ////
#include <WiFi.h>
#include <WiFiUdp.h>
const char* ssid     = "ESPdatos";
const char* password = "respiracion";
IPAddress ip(192, 168, 4, 1);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
WiFiUDP Udp;
IPAddress myIP;                    // the IP address of your shield
//// Fin WiFi UDP connection ////

///// For Receiving UDP /////////////
unsigned int localPort = 8889;      // local port to listen on
char packetBuffer[20 + 1]; //buffer to hold incoming packet---------------------
///// Fin Receiving UDP ////////////

int datoL2 = 0; // Indicates state of breathing "1" or "2"

unsigned long initRutina;
unsigned long leTemps = 0;
unsigned long elViejoTiempo = 0;
unsigned long intervalleEntreResp = 0;
String calMessage = "CC, ";//CAL CHEST
String calibracion = "hello";

bool invertScreen = false;

void begin() {

  Serial.begin(9600);

  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //  Serial.println("");
  //  Serial.println("WiFi connected");
  //  Serial.println("IP address: ");
  //  Serial.println(WiFi.localIP());
  myIP = WiFi.localIP();

  ////// For Receiving UDP /////
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);
  //////////////////////////////

  pinMode (LED_BUILTIN, OUTPUT); // LED del ESP8266

  for ( int i = 0 ; i < 5 ; i++ ) { //el led parpadea 5 veces para saludar!
    digitalWrite(LED_BUILTIN, 1 ) ;
    delay ( 100 ) ;
    digitalWrite (LED_BUILTIN, 0 ) ;
    delay ( 100 ) ;

    ///////////// TFT DISPLAY /////////////
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;); // Don't proceed, loop forever
    }

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(1000); // Pause for 1 second

    // Clear the buffer
    display.clearDisplay();
    ////////////////////////////////
  }

  digitalWrite (LED_BUILTIN, HIGH);//apago led por las dudas reverse para los ESP8266

  // Smooth over a window of 100ms (ie. one tenth of a second).
  // NOTE: Try changing the smoothing value to see how it affects the outputs.
  in.smooth(0.2);

  norm.time(10);

  //initRutina = millis() + 10000;///////////////////////////////////10mil 10seg
  //initRutina = millis() + 40000; //calibracion en CITY v.default.
  initRutina = millis() + 20000; //calCITY (version intrucciones 20) [reset en 15]
}


void step() {

  in >> norm >> peakDetector;
  //Serial.print(6 * norm);
  //Serial.println(" ");
  //  Serial.print(6 * peakDetector);
  //  Serial.println(" ");

  //Udp.beginPacket(ip, 8888);
  //String msg = String(localData.temp);

  if (millis() < initRutina) //////////////////////solo envia msg calibracion
  {
    String calibracion = String (norm * 6);
    String sendCal = calMessage + calibracion;
    Udp.beginPacket(ip, 8888);
    Udp.print(sendCal);
    Udp.endPacket();

    /////// Display values on the screen
    display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0, 0);
    display.println(myIP);// Start at top-left corner
    //display.println(F("Client connected ?"));

    display.setTextSize(2);             // Draw 2X-scale text
    display.setTextColor(WHITE);
    display.setCursor(5, 16);
    display.println(F("CHEST"));

    display.setTextSize(4);             // Draw 2X-scale text
    display.setTextColor(WHITE);
    display.setCursor(6, 35);
    display.println(6 * norm);

    display.display();
    delay(50);
  }

  if (millis() > initRutina)////////////////////////envia msg sensado
  {

    if (peakDetector == 1)
    {
      leTemps = millis();
      intervalleEntreResp = millis() - elViejoTiempo;

      datoL2 == 1; // peak detected
      digitalWrite (LED_BUILTIN, LOW);
      Udp.beginPacket(ip, 8888);
      Udp.print("chest, 2");
      Udp.endPacket();

      //    Serial.println("peak detected, data one sent");
      //    Serial.print("intervalle entre respiration : ");
      //    Serial.println(intervalleEntreResp);

      elViejoTiempo = leTemps;

    }
    else if ( (millis() - elViejoTiempo) < intervalleEntreResp / 4) { // si nous sommes ?? l'int??rieur du tiers de l'ijntervalle de respiration
      digitalWrite (LED_BUILTIN, LOW);
      Udp.beginPacket(ip, 8888);
      Udp.print("chest, 2");
      Udp.endPacket();
      Serial.println("CHEST, 2");

      /////// Display values on the screen
      display.clearDisplay();

      if (invertScreen == true)
      {
        display.invertDisplay(true);
      }

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(WHITE);        // Draw white text
      display.setCursor(0, 0);
      display.println(myIP);// Start at top-left corner
      //display.println(F("Client connected ?"));

      display.setTextSize(2);             // Draw 2X-scale text
      display.setCursor(5, 16);
      display.setTextColor(WHITE);
      display.println(F("CHEST"));

      display.setTextSize(4);
      display.setCursor(6, 35);
      display.println(2);

      display.display();

    }
    else
    {
      datoL2 == 2; // nope
      digitalWrite (LED_BUILTIN, HIGH);
      Udp.beginPacket(ip, 8888);
      Udp.print("chest, 1");
      Udp.endPacket();
      Serial.println("CHEST, 1");

      /////// Display values on the screen
      display.clearDisplay();

      if (invertScreen == true)
      {
        display.invertDisplay(true);
      }

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(WHITE);        // Draw white text
      display.setCursor(0, 0);
      display.println(myIP);// Start at top-left corner
      //display.println(F("Client connected ?"));

      display.setTextSize(2);             // Draw 2X-scale text
      display.setCursor(5, 16);
      display.setTextColor(WHITE);
      display.println(F("CHEST"));

      display.setTextSize(4);
      display.setCursor(6, 35);
      display.println(1);

      display.display();
    }

    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      /* Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                     packetSize,
                     Udp.remoteIP().toString().c_str(), Udp.remotePort(),
                     Udp.destinationIP().toString().c_str(), Udp.localPort(),
                     ESP.getFreeHeap());
      */
      // read the packet into packetBufffer
      //int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      int n = Udp.read(packetBuffer, 20);

      packetBuffer[n] = 0;
      //Serial.println("Contents:");
      Serial.println(packetBuffer);
      if (packetBuffer[0] == '1') { // This compares the received message (we're sending '1')
        //"Resetting";
        invertScreen == true;
        ESP.restart();
      }
    }

    


  }

  //delay(200);///?NO VA
} // fin step
