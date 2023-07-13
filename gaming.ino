#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// CONFIGS

#define readpin 14                                                                                                                                                                         

const char* ssid = "";
const char* password =  "";
const char* mqttServer = "";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

// END CONFIGS

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long pulse_length = 0;

void setup() {
  pinMode(readpin, INPUT);
  digitalWrite(readpin, HIGH);
  
  Serial.begin(115200);
  Serial.println("Began serial at speed 115200");
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to the WiFi network");
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  Serial.println("=== BEGIN LOOPING ===");
}

void loop() {
  pulse_length = pulseIn(readpin, HIGH, 5000UL);
  if(pulse_length > 450UL){
    String bits = "";
    pulse_length = pulseIn(readpin, HIGH, 5000UL);
    while(pulse_length > 30UL){
        if(pulse_length > 100UL){
          bits += '1';
        }
        else
        {
          bits += '0';
        }
      pulse_length = pulseIn(readpin, HIGH, 5000UL);
    }

    if(bits.length() != 64){
      Serial.print("Raw: ");
      Serial.print(bits);
      Serial.println(": not 64 bits!");
      return;
    }

    char to_send[64];
    bits.toCharArray(to_send, 64);
    
    Serial.print("Raw: ");
    Serial.println(bits);
    Serial.println("Sending...");

    while(!client.connected()){
      Serial.print("Connecting...");
      client.connect("ESP8266Client", mqttUser, mqttPassword);
    }
    
    client.publish("home/electricity/total-raw", to_send);
    Serial.println("Sent!");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  return;
}
