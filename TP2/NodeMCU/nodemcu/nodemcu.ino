#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <HTU2xD_SHT2x_Si70xx.h>

const char* ssid = "Siglo 21 2.4";
const char* password = "20potrerofunes12";
const char* mqtt_server = "rpi.local";
const char* topic_name = "humidity";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

float htValue;
HTU2xD_SHT2x_SI70xx ht2x(HTU2xD_SENSOR, HUMD_12BIT_TEMP_14BIT); //sensor type, resolution

void setup_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Conectandose a ");
  Serial.println(ssid);

  //Connect to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
}

void setup_sensor() {
  while (ht2x.begin() != true) //reset sensor, set heater off, set resolution, check power (sensor doesn't operate correctly if VDD < +2.25v)
  {
    Serial.println(F("HTU21D no conectado, falla o VDD < +2.25v")); //(F()) save string to flash & keeps dynamic memory free

    delay(5000);
  }

  Serial.println(F("HTU21D OK"));
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {

    Serial.print("Intentando conexion MQTT...");

    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      client.publish(topic_name, "Sensor conectado"); // Once connected, publish an announcement...
    } 
    else {
      Serial.print("fallo, rc=");
      Serial.print(client.state());
      Serial.println(" reintentando en 5 segundos");

      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setup_wifi();
  setup_sensor();

  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    htValue = ht2x.getCompensatedHumidity(htValue);
    snprintf(msg, MSG_BUFFER_SIZE, "%.2f", htValue);

    Serial.print("Mensaje publicado: ");
    Serial.println(msg);

    client.publish(topic_name, msg);
  }
}
