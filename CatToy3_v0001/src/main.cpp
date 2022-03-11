#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>

// Replace with your network credentials
const char* ssid     = "Telia-15286F";
const char* password = "88F186ACC0";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object 
AsyncWebSocket ws("/ws");

// Set number of outputs
#define NUM_OUTPUTS 2

// Assign each GPIO to an output
int outputGPIOs[NUM_OUTPUTS] = {2, 4};

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
String sliderValue = "0";
int dutyCycle;

// Initialize SPIFFS
void initSPIFFS() {
if (!SPIFFS.begin(true)) {
Serial.println("An error has occurred while mounting SPIFFS");
}
Serial.println("SPIFFS mounted successfully");
}
// Initialize WiFi
void initWiFi() {
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
Serial.print("Connecting to WiFi ..");
while (WiFi.status() != WL_CONNECTED) {
Serial.print('.');
delay(1000);
}
Serial.println(WiFi.localIP());
}
String getOutputStates(){
JSONVar myArray;
for (int i =0; i<NUM_OUTPUTS; i++){
myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
}
String jsonString = JSON.stringify(myArray);
return jsonString;
}
void notifyClients(String state) {
ws.textAll(state);
}
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
AwsFrameInfo *info = (AwsFrameInfo*)arg;
if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
data[len] = 0;
if (strcmp((char*)data, "states") == 0) {
notifyClients(getOutputStates());
}
else{
int gpio = atoi((char*)data);
digitalWrite(gpio, !digitalRead(gpio));
notifyClients(getOutputStates());
}
}
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
void *arg, uint8_t *data, size_t len) {
switch (type) {
case WS_EVT_CONNECT:
Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
break;
case WS_EVT_DISCONNECT:
Serial.printf("WebSocket client #%u disconnected\n", client->id());
break;
case WS_EVT_DATA:
handleWebSocketMessage(arg, data, len);
break;
case WS_EVT_PONG:
case WS_EVT_ERROR:
break;
}
}
void initWebSocket() {
ws.onEvent(onEvent);
server.addHandler(&ws);
}
void setup(){
// Serial port for debugging purposes
Serial.begin(115200);
// Set GPIOs as outputs
pinMode(outputGPIOs[0], OUTPUT);
pinMode(outputGPIOs[1], OUTPUT);

// configure LED PWM functionalitites
ledcSetup(ledChannel, freq, resolution);
// attach the channel to the GPIO to be controlled
ledcAttachPin(outputGPIOs[1], ledChannel);


initSPIFFS();
initWiFi();
initWebSocket();
// Route for root / web page
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
request->send(SPIFFS, "/index.html", "text/html",false);
request->send(200, "/text/plain", String(sliderValue).c_str());
});
server.serveStatic("/", SPIFFS, "/");


// Start server
server.begin();
}
void loop() {
ledcWrite(ledChannel, dutyCycle);
ws.cleanupClients();
}