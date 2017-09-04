#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

extern "C" {
   #include <user_interface.h>
}

/*
 * NodeMCU/ESP8266 act as AP (Access Point) and simple Web Server
 * Connect to AP
 * Open browser, visit 192.168.4.1
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <user_interface.h>

const char *ssid = "brickhouse";
const char *password = "password1911";

const String AVAILABLE = "available";
const String KNOCK = "knock";
const String DND = "dnd";

String state = AVAILABLE;

const int buttonPin = D1;

int buttonState = HIGH;
int lastButtonState = HIGH;

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

WiFiEventHandler stationConnectedHandler;

void handleConnected() {

    unsigned char softap_stations_cnt;
    struct station_info *stat_info;
    struct ip_addr *IPaddress;
    uint32 uintaddress;

    Serial.print("root: \n");

    softap_stations_cnt = wifi_softap_get_station_num(); // Count of stations which are connected to ESP8266 soft-AP
    Serial.print("Stations Connected: ");
    Serial.println(softap_stations_cnt);

    Serial.print("DHCP Status: ");
    Serial.println(wifi_softap_dhcps_status());
    
    stat_info = wifi_softap_get_station_info();
    while (stat_info != NULL) {
      IPaddress = &stat_info->ip;
      uintaddress = IPaddress->addr;
      Serial.print("Station IP: ");
      Serial.println(uintaddress);

      String ip = ipToString(uintaddress);
      Serial.println(ip);
      
      stat_info = STAILQ_NEXT(stat_info, next);
    }
      
}

void handleAvailable() {
  Serial.print("available: \n");
  handleConnected();
  httpGet("http://192.168.04.02:8080/api/v1/available");
}

void handleKnock() {
  handleConnected();
  Serial.print("knock: \n");
  httpGet("http://192.168.04.02:8080/api/v1/knock");
}

void handleDND() {
  handleConnected();
  Serial.print("dnd: \n");
  httpGet("http://192.168.04.02:8080/api/v1/dnd");
}

void httpGet(String address) {
  HTTPClient http;
  http.begin(address);
  int httpCode = http.GET(); 
  if(httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }  http.end();   
}

void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
  Serial.print("Station connected: ");
  Serial.println(macToString(evt.mac));
  handleConnected();
}

String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

String ipToString(uint32 uintaddress) {

  unsigned char bytes[4];
  bytes[0] = uintaddress & 0xFF;
  bytes[1] = (uintaddress >> 8) & 0xFF;
  bytes[2] = (uintaddress >> 16) & 0xFF;
  bytes[3] = (uintaddress >> 24) & 0xFF;       
  
  char buf[20];
  snprintf(buf, sizeof(buf), "%02d.%02d.%02d.%02d",
    bytes[0], bytes[1], bytes[2], bytes[3]);
    
  return String(buf);
}

void setup() {
    Serial.begin(19200);
    Serial.println();

    pinMode(buttonPin, INPUT_PULLUP);

    WiFi.softAP(ssid, password);

    IPAddress apip = WiFi.softAPIP();
    stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&onStationConnected);

    handleConnected();
}

void loop() {

  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {

        if (state == AVAILABLE) {
          state = KNOCK;
          handleKnock();
        }

        else if (state == KNOCK) {
          state = DND;
          handleDND();
        }

        else if (state == DND) {
          state = AVAILABLE;
          handleAvailable();
        }
        
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}
