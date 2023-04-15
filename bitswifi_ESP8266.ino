// Hi there!
// This code will help you access the internet on your ESP8266 or in fact any other board (Of course, some tweaking will be required).
// IMP: This code is specifically designed for my fellow IoT enthusiasts studying in BITS Pilani who are fed up of using their mobile hotspots to get their boards to access the internet and update.
// This code's use can also be expanded to university students who may have a captive portal on their network (You just need to change the parameters). 
// To summarise, this code allows your ESP8266 to connect to the BITS network and then view the login screen where the credentials are entered which grants it the power of the internet.
// The Example Code was added with the sole purpose that it might come handy to someone. The Example Code basically shows you what all you need to add to the code to get the "time" shown on your website that is being hosted by the ESP8266 on the BITS Network at its Local IP.
// Made with ❤ by Anubhav Panda  

#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>


// include wpa2 enterprise code
extern "C" {
  #include "user_interface.h"
  #include "wpa2_enterprise.h"
}

//Update with yours!
static const char* ssid = "BITS-STUDENT";
static const char* username = "";  //yourusername
static const char* password = "";  //yourpasswd 

c
onst char* serverName = "https://fw.bits-pilani.ac.in:8090/login.xml";

const uint8_t fingerprint[20] = {0x47, 0x40, 0x2A, 0xAD, 0x9F, 0x38, 0x4A, 0x40, 0x50, 0x9F, 0x53, 0x5C, 0xC1, 0xBA, 0xD6, 0xBB, 0xFC, 0xBB, 0xE8, 0x3D};


unsigned long previousMillis = 0;    

unsigned long lastTime = 0;


AsyncWebServer server(80);

// Part of the Example Code
//String d;

const char index_html[] PROGMEM = R"rawliteral(
  </html><!DOCTYPE HTML>
    
  //your HTML code here
    
  //Part of the Example Code
  <script>
    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("time").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/time", true);
        xhttp.send();
    }, 1000);
    </script>

    )rawliteral";


// Part of the Example Code
// Replaces placeholder with values
//  String processor(const String& var){
//    //Serial.println(var);
//    if(var == "TIME"){
//      return String(d);
//    }
//    else if(var == ""){  //your variable name
//      return String();   //your variable 
//    }
//      return String();
//  }

void refresh()
{
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient https;
    
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

  client->setFingerprint(fingerprint);
      
  // Your Domain name with URL path or IP address with path
  https.begin(*client, serverName);

  // Specify content-type header
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // https.addHeader("Set-Cookie", "");
  // Go to https://fw.bits-pilani.ac.in:8090/httpclient.html and open the developer console and look for the payload in the network tab that is sent when you login.
  // It should be present in the login.xml file that is sent when you click on "Sign In". Go to the headers tab and copy the cookie from there and paste it here.

  https.setUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36");

  https.setReuse("keep-alive");

  // Data to send with HTTP POST
  String httpsRequestData = ""; 
  // Go to https://fw.bits-pilani.ac.in:8090/httpclient.html and open the developer console and look for the payload in the network tab that is sent when you login.
  // It should be present in the login.xml file that is sent when you click on "Sign In". Copy the payload from there (in source code view) and paste it here.
  // Example: String httpsRequestData = "mode=191&username=yourusername&password=yourpwd%23&a=123456789&producttype=0"; 
  
  
  
  // Send HTTP POST request
  int httpsResponseCode = https.POST(httpsRequestData);

      //Serial.print("HTTP Response code: ");
      //Serial.println(httpResponseCode);
        
      // Free resources
      https.end();
    }
    else {
      Serial.println("Internet Disconnected");
    }
    lastTime = millis();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(baud_rate);

  // Setting ESP into STATION mode only (no AP mode or dual mode)
  wifi_set_opmode(STATION_MODE);

  struct station_config wifi_config;

  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, ssid);

  wifi_station_set_config(&wifi_config);
  
  // Clean up to be sure no old data is still inside
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();

  wifi_station_set_wpa2_enterprise_auth(1);

  
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_identity((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen(password));
  
  wifi_station_connect();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Part of the Example Code
  // Route for root / web page
  //  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //  request->send_P(200, "text/html", index_html, processor);
  //  });
  //  Example of sending data from the ESP8266 to the server
  //  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
  //  request->send_P(200, "text/plain", String(d).c_str());

  // Start server
  server.begin();

  refresh();

}
String c;

void loop(){

}


//v1