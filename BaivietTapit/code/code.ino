
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Set up wifi
const char* ssid      = "<YOUR_SSID>";
const char* password  = "<YOUR_WIFI_PASSWORD>";

// Set up IoT hub
#define DEVICE_ID "device"

const char MQTT_HOST[] = "<Hostname>"; 
// ex: xxxxx.azure-devices.net
const int MQTT_PORT = 8883;
const char MQTT_USER[] = "<xxxx.azure-devices.net/<deviceId>/?api-version=2018-06-30>"; 
const char MQTT_PASS[] = "<SharedAccessSignature sr={URL-encoded-resourceURI}&sig={signature-string}&se={expiry}>"; 

const char MQTT_PUB_TOPIC[] = "$iothub/twin/PATCH/properties/reported/?$rid=1";
const char MQTT_SUB_TOPIC[] = "$iothub/twin/res/#";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setInsecure();
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
  mqtt_connect();
}


void setup_wifi() {
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Received. topic=");
    Serial.println(topic);
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.print("\n");
}


void mqtt_connect()
{
  while (!client.connected()) {
    Serial.print("MQTT connecting ... ");
    if (client.connect(DEVICE_ID, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected.");
      client.subscribe(MQTT_SUB_TOPIC);
      client.publish(MQTT_PUB_TOPIC, "");
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println(". Try again in 5 seconds.");
      Serial.print(DEVICE_ID);
      /* Wait 5 seconds before retrying */
      delay(5000);

    }
  }
}
//----------------------------------------------------
long messageSentAt = 0;
int dummyValue = 0;
char pubMessage[128];

void mqttLoop() {
  if (!client.connected())
  {
    mqtt_connect();
  }
  client.loop();
    unsigned long now = millis();
    if (now - messageSentAt > 20000)
    {
      messageSentAt = now;
      sprintf(pubMessage, "{\"count\":\"%d\"}",  dummyValue++);
      Serial.print("Publishing message to topic ");
      Serial.println(MQTT_PUB_TOPIC);
      Serial.println(pubMessage);
      client.publish(MQTT_PUB_TOPIC, pubMessage);
      Serial.println("Published.");
    }
}

void loop()
{
  mqttLoop();

}
