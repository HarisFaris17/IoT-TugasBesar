#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "C:\Users\lenovo\Documents\PlatformIO\Projects\Tubes\.pio\libdeps\nod
emcuv2\DHT sensor library\DHT.h"
#include "C:\Users\lenovo\Documents\PlatformIO\Projects\Tubes\.pio\libdeps\nod
emcuv2\HTTPSRedirect\HTTPSRedirect.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include "C:\Users\lenovo\Documents\PlatformIO\Projects\Tubes\.pio\libdeps\nod
emcuv2\DHT sensor library\DHT_U.h"
#define DHTTYPE DHT11
#include <ESP8266HTTPUpdateServer.h>
const char* ssid = "Mi Phone";
const char* password = "qwerty67";
const char* host = "esp8266";
const char* mqtt_server = "ee.unsoed.ac.id";
const char *GScriptId = "AKfycbwmOVpet0YmfHmsWZLLiZHQIFp7Y1orV5vsypDLDXXwgezGG9zsokeaYaRhschw2tN";
String payload_base = "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1
                        \", \"values\": ";

String payload = "";
                        const char* host_gsheet = "script.google.com";
                        const int httpsPort = 443;
                        const char* fingerprint = "";
                        String url = String("/macros/s/") + GScriptId + "/exec?cal";
                        HTTPSRedirect* client_https = nullptr;
                        unsigned long waktu_led, waktu_perulangan;
                        float Temp;
                        float Humidity;
                        WiFiClient espClient;
                        PubSubClient client(espClient);
                        unsigned long lastMsg = 0;
                        unsigned long lastgsheet = 0;
#define MSG_BUFFER_SIZE (50)
                        char msg_LED[MSG_BUFFER_SIZE], msg_suhu[MSG_BUFFER_SIZE], msg_kelembapan[MSG_BUF
                            FER_SIZE], msg_ipaddress[MSG_BUFFER_SIZE], msg_chipid[MSG_BUFFER_SIZE];
                        ESP8266HTTPUpdateServer httpUpdater;
                        DHT dht(GPIO_ID_PIN(D5), DHTTYPE);
                        ESP8266WebServer server(80);
                        int state_saklar;
                        const int saklar = D7;
                        const int led = LED_BUILTIN_AUX;
String message(float temp, float humid) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=devicewidth, initialscale=1.0, user-scalable=no\">\n";
  ptr += "<title>Proyek dengan ESP8266</title>\n";
  ptr += "<style>html { fontfamily: Helvetica; display: inlineblock; margin: 0px auto; textalign: center;}\n";
  ptr += "body{margintop: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"identitas\">\n";
  ptr += "<h1>Haris Prasetyo</h1>\n";
  ptr += "<p>NIM: H1A018064 </p>";
  ptr += "<p>Jurusan Teknik Elektro FT Unsoed </p>";
  ptr += "</div>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Pembacaan suhu dan kelembaban</h1>\n";
  ptr += "<p>Suhu: ";
  ptr += temp;
  ptr += " <sup>o</sup>C</p>";
  ptr += "<p>Kelembaban: ";
  ptr += humid;
  ptr += " %</p>";
  ptr += "</div>\n";
  ptr += "<div id=\"saklar\">\n";
  ptr += "<h1>LED : ";
  if (digitalRead(led) == 0) {
    ptr += "Menyala</h1>\n";
  }
  else {
    ptr += "Mati</h1>\n";
  }
  ptr += "<a href=\"/led_hidup\"><button>ON</button></a>";
  ptr += "<a href=\"/led_mati\"><button>OFF</button></a>";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
void ICACHE_RAM_ATTR saklar_berubah() {
  if (digitalRead(saklar) == 1) {
    state_saklar = 1;
    Serial.println("Saklar OFF");
  }
  else {
    Serial.println("Saklar ON");
    state_saklar = 0;
  }
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected MQTT");
      // Once connected, publish an announcement...
      client.publish("iot20212/H1A018064/OutTopic", "hello world");
      // ... and resubscribe
      client.subscribe("iot20212/H1A018064/ip_address/saklar");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    //digitalWrite(, LOW);
    state_saklar = 0;
  }
  else {
    //digitalWrite(led, HIGH);
    state_saklar = 1;
  }
}
void handleRoot() {
  float Temp = dht.readTemperature();
  float Humidity = dht.readHumidity();
  server.send(200, "text/html", message(Temp, Humidity));
  delay(2000);
}
void handleNotFound() {
  delay(2000);
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}
void lampu() {
  if (state_saklar == 0 && Temp > 28) {
    waktu_led = waktu_led + millis() - waktu_perulangan;
    waktu_perulangan = millis();
  }
  else {
    waktu_led = 100000;
  }
  if (waktu_led < 60000) {
    digitalWrite(led, 0);
    Serial.println("LED Menyala");
    client.publish("iot20212/H1A018064/LED", "1", bool(1));
  }
  if (waktu_led > 60000) {
    digitalWrite(led, 1);
    Serial.println("LED Mati");
    client.publish("iot20212/H1A018064/LED", "0", bool(1));
    waktu_led = 0;
    waktu_perulangan = millis();
  }
}
void setup(void) {
  Serial.begin(115200);
  pinMode(saklar, INPUT);
  attachInterrupt(digitalPinToInterrupt(saklar), saklar_berubah, CHANGE);
  //attachInterrupt(saklar,saklar_berubah,CHANGE);
  dht.begin();
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  state_saklar = digitalRead(D7);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (MDNS.begin(host)) {
    Serial.println("\nMDNS responder started");
  }
  httpUpdater.setup(&server);
  client_https = new HTTPSRedirect(httpsPort);
  client_https->setInsecure();
  client_https->setPrintResponseBody(true);
  client_https->setContentTypeHeader("application/json");
  Serial.print("Connecting to ");
  Serial.println(host_gsheet);
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client_https->connect(host_gsheet, httpsPort);
    if (retval == 1) {
      flag = true;
      Serial.println("Connected");
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host_gsheet);
    return;
  }
  delete client_https; // delete HTTPSRedirect object
  client_https = nullptr; // delete HTTPSRedirect object
  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your b
                rowser\n", host);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    }
    else { // U_FS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    }
    else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    }
    else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    }
    else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    }
    else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/led_hidup", []() {
    server.send(200, "text/plain", "Led Hidup");
    digitalWrite(led, 0);
    Serial.println("Led Hidup");
    if (digitalRead(led) == 0) {
      client.publish("iot20212/H1A018064/LED", "1");
    }
    if (digitalRead(led) == 1) {
      client.publish("iot20212/H1A018064/LED", "0");
    }
  });
  server.on("/led_mati", []() {
    server.send(200, "text/plain", "Led Padam");
    digitalWrite(led, 1);
    Serial.println("Led Padam");
    if (digitalRead(led) == 0) {
      client.publish("iot20212/H1A018064/LED", "1");
    }
    if (digitalRead(led) == 1) {
      client.publish("iot20212/H1A018064/LED", "0");
    }
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  snprintf (msg_chipid, MSG_BUFFER_SIZE, "Chip ID : %d", ESP.getChipId());
  Serial.println(msg_chipid);
  String myip = WiFi.localIP().toString();
  myip.toCharArray(msg_ipaddress, MSG_BUFFER_SIZE);
  client.publish("iot20212/H1A018064/ipaddress", msg_ipaddress, bool(1));
  //client.publish("iot20212/H1A018064/chipid",msg_chipid,bool(1));
}
int i = 0;
void loop(void) {
  server.handleClient();
  ArduinoOTA.handle();
  MDNS.update();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  callback;
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lampu();
    Serial.println(digitalRead(saklar));
    lastMsg = now;
    Temp = dht.readTemperature();
    Humidity = dht.readHumidity();
    snprintf (msg_LED, MSG_BUFFER_SIZE, "%d", !digitalRead(led));
    snprintf (msg_suhu, MSG_BUFFER_SIZE, "%f", Temp);
    snprintf (msg_kelembapan, MSG_BUFFER_SIZE, "%f", Humidity);
    client.publish("iot20212/H1A018064/ipaddress", msg_ipaddress, bool(1));
    client.publish("iot20212/H1A018064/chipid", msg_chipid, bool(1));
    /*if(digitalRead(led)==0){
      client.publish("iot20212/H1A018064/LED","1",bool(1));
      }
      if(digitalRead(led)==1){
      client.publish("iot20212/H1A018064/LED","0",bool(1));
      }*/
    Serial.print("\n\nPublish message: \n");
    Serial.println(msg_LED);
    Serial.println(msg_suhu);
    Serial.println(msg_kelembapan);
    //client.publish("")
    client.publish("iot20212/H1A018064/suhu", msg_suhu);
    client.publish("iot20212/H1A018064/kelembapan", msg_kelembapan);
  }
  if (now - lastgsheet > 10000) {
    lastgsheet = millis();
    static bool flag = false;
    if (!flag) {
      client_https = new HTTPSRedirect(httpsPort);
      client_https->setInsecure();
      flag = true;
      client_https->setPrintResponseBody(true);
      client_https->setContentTypeHeader("application/json");
    }
    if (client_https != nullptr) {
      if (!client_https->connected()) {
        client_https->connect(host_gsheet, httpsPort);
      }
    }
    else {
      Serial.println("Error creating client object!");
    }
    payload = payload_base + "\"" + Temp + "," + Humidity + "\"}";
    Serial.println("Publishing data...");
    Serial.println(payload);
    if (client_https->POST(url, host_gsheet, payload)) {
      // do stuff here if publish was successful
    }
    else {
      // do stuff here if publish was not successful
      Serial.println("Error while connecting");
    }
  }
