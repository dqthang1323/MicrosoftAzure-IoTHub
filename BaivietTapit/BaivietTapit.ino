#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid      = "THEBOOKS"; //VELO BOOK CAFE
const char* password  = "thebooks.vn"; //velobookcafe

#define HOSTNAME "device"

const char MQTT_HOST[] = "ThangHub.azure-devices.net";
const int MQTT_PORT = 8883;
const char MQTT_USER[] = "ThangHub.azure-devices.net/device/?api-version=2018-06-30"; // leave blank if no credentials used
const char MQTT_PASS[] = "SharedAccessSignature sr=ThangIoTHub.azure-devices.net%2Fdevices%2Fmydevice&sig=yMQE8XVQD5O%2F8om9c4x9Fp6qV8LYiI6DsT5sEOsIqJA%3D&se=1618684076"; // leave blank if no credentials used

const char MQTT_SUB_TOPIC[] = "devices/mydevice/messages/devicebound/#"; // $iothub/twin/res/# 
const char MQTT_PUB_TOPIC[] = "devices/mydevice/messages/events/";


#ifdef CHECK_FINGERPRINT
static const char fp[] PROGMEM = "9E:53:1E:45:5C:DB:9B:F7:44:EA:0B:58:28:19:A2:19:98:ED:36:7E"; 
#endif

BearSSL::WiFiClientSecure espClient;
PubSubClient client(espClient);


//----------------------------------------------------------
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  #ifdef CHECK_FINGERPRINT
    espClient.setFingerprint(fp);
  #endif
  #if (!defined(CHECK_FINGERPRINT))
    espClient.setInsecure();
  #endif
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
  mqtt_connect();
}
//----------------------------------------------------------
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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
//----------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
//----------------------------------------------------------
void mqtt_connect()
{
  while (!client.connected()) {
    Serial.print("MQTT connecting ... ");
    if (client.connect(HOSTNAME, MQTT_USER, MQTT_PASS)) 
    {
      Serial.println("connected.");
      client.subscribe(MQTT_SUB_TOPIC);
      Serial.println("Subscribed to topics.");
      Serial.print("\n");
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println(". Try again in 5 seconds.");
      Serial.print(HOSTNAME);
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
        sprintf(pubMessage, "{\"desired\":""}");
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
