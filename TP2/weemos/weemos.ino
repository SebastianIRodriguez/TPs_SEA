/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define WIFI_LED 14
#define BROKER_LED 12
#define ERROR_LED 13

// Update these with values suitable for your network.

//const char *ssid = "Luchito Wi-Fi";
//const char *password = "00416778400";
//const char *mqtt_server = "192.168.180";

const char *ssid = "Laboratorio DSI";
const char *password = "alumnolab";
const char *mqtt_server = "192.168.100.180";

// Topics
const char *sw1_t = "grupo2/KL43/sw1";
const char *sw3_t = "grupo2/KL43/sw3";
const char *accelx_t = "grupo2/KL43/accelX";
const char *accely_t = "grupo2/KL43/accelY";
const char *accelz_t = "grupo2/KL43/accelZ";
const char *light_t = "grupo2/KL43/light";
const char *redLED_t = "grupo2/KL43/redLED";
const char *greenLED_t = "grupo2/KL43/greenLED";
const char *fail_t = "grupo2/KL43/failLog";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(WIFI_LED, HIGH);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  /*
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();*/

  // Estado del led verde
  if (strcmp(topic, greenLED_t) == 0)
  {
    // Verificación de seguridad
    char value = payload[0];
    if (length == 1 && (value == 'A' || value == 'E' || value == 'T'))
    {
      char mes[7];
      char res[7];
      int i = 0;
      sprintf(mes, ":02%c\n", value);
      Serial.printf(mes);
      delay(50);

      while (Serial.available())
        res[i++] = Serial.read();
      res[i] = 0;

      if (strcmp(res, mes) != 0)
      {
        char text[40];
        sprintf(text, "Error sending green LED %c instruction", value);
        client.publish(fail_t, text);
        digitalWrite(ERROR_LED, HIGH);
      }
      else
      {
        digitalWrite(ERROR_LED, LOW);
      }
    }
  }

  // Estado del led rojo
  if (strcmp(topic, redLED_t) == 0)
  {
    // Verificación de seguridad
    char value = payload[0];
    if (length == 1 && (value == 'A' || value == 'E' || value == 'T'))
    {
      char mes[7];
      char res[7];
      int i = 0;
      sprintf(mes, ":01%c\n", value);
      Serial.printf(mes);
      delay(50);

      while (Serial.available())
        res[i++] = Serial.read();
      res[i] = 0;

      if (strcmp(res, mes) != 0)
      {
        char text[40];
        sprintf(text, "Error sending red LED %c instruction", value);
        client.publish(fail_t, text);
        digitalWrite(ERROR_LED, HIGH);
      }
      else
      {
        digitalWrite(ERROR_LED, LOW);
      }
    }
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      // Once connected, publish an announcement...
      client.publish(fail_t, "Device on");
      // ... and resubscribe
      client.subscribe(redLED_t);
      client.subscribe(greenLED_t);
      digitalWrite(BROKER_LED, HIGH);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(WIFI_LED, OUTPUT);
  pinMode(BROKER_LED, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);

  digitalWrite(WIFI_LED, LOW);
  digitalWrite(BROKER_LED, LOW);
  digitalWrite(ERROR_LED, LOW);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

char buf[22];
int len = 0;
int MAX_MES_LEN = 20;

void loop()
{

  if (!client.connected())
  {
    digitalWrite(BROKER_LED, LOW);
    reconnect();
  }
  client.loop();

  while (Serial.available())
  {
    char c = Serial.read();
    if (c == '\n')
    {
      // Verificamos cositas
      if (buf[0] == ':' && len == MAX_MES_LEN - 1)
      {
        char light[5];
        char acc_x[5];
        char acc_y[5];
        char acc_z[5];
        bool sw1;
        bool sw3;
        char aux[5];

        // Luz
        aux[0] = buf[1];
        aux[1] = buf[2];
        aux[2] = buf[3];
        aux[3] = buf[4];
        aux[4] = 0;
        strcpy(light, aux);

        // Acel x
        aux[0] = buf[5];
        aux[1] = buf[6];
        aux[2] = buf[7];
        aux[3] = buf[8];
        aux[4] = 0;
        strcpy(acc_x, aux);

        // Acel y
        aux[0] = buf[9];
        aux[1] = buf[10];
        aux[2] = buf[11];
        aux[3] = buf[12];
        aux[4] = 0;
        strcpy(acc_y, aux);

        // Acel z
        aux[0] = buf[13];
        aux[1] = buf[14];
        aux[2] = buf[15];
        aux[3] = buf[16];
        aux[4] = 0;
        strcpy(acc_z, aux);

        sw1 = buf[17] - 48;
        sw3 = buf[18] - 48;

        // El 18 es el \n

        // Publicamos
        client.publish(light_t, light);
        client.publish(accelx_t, acc_x);
        client.publish(accely_t, acc_y);
        client.publish(accelz_t, acc_z);

        if (sw1)
          strcpy(aux, "Pressed");
        else
          strcpy(aux, "Not pressed");
        client.publish(sw1_t, aux);

        if (sw3)
          strcpy(aux, "Pressed");
        else
          strcpy(aux, "Not pressed");
        client.publish(sw3_t, aux);
      }
      len = 0;
    }
    else
    {
      buf[len++] = c;
      if (len > MAX_MES_LEN)
        len = 0;
    }
  }
}