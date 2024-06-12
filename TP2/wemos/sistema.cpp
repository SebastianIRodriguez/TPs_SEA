/*==================[inclusions]=============================================*/
#include "sistema.h"
/*==================[macros and definitions]=================================*/
#define KL43_BAUDRATE 115200
#define MAX_MES_LEN 18
#define SWITCH_MESSAGE_LEN 2

// Pines digitales
#define WIFI_LED 14
#define BROKER_LED 12
#define ERROR_LED 13

/*==================[internal data declaration]==============================*/
bool DEBUG = 0;

WiFiClient espClient;
PubSubClient client(espClient);

// Topics
const char *sw1_t = "grupo2/KL43/sw1";                     // Topic donde se publica el estado del SW1
const char *sw3_t = "grupo2/KL43/sw3";                     // Topic donde se publica el estado del SW3
const char *accelx_t = "grupo2/KL43/accelX";               // Topic donde se publica la aceleración X
const char *accely_t = "grupo2/KL43/accelY";               // Topic donde se publica la aceleración y
const char *accelz_t = "grupo2/KL43/accelZ";               // Topic donde se publica la aceleración Z
const char *light_t = "grupo2/KL43/light";                 // Topic donde se publica el nivel de luz
const char *redLED_t = "grupo2/KL43/redLEDcommand";        // Topic desde donde se lee los comandos para el led rojo
const char *greenLED_t = "grupo2/KL43/greenLEDcommand";    // Topic desde donde se lee los comandos para el led verde
const char *greenLEDstate_t = "grupo2/KL43/greenLEDstate"; // Topic donde se reporta el estado del led verde
const char *redLEDstate_t = "grupo2/KL43/redLEDstate";     // Topic donde se reporta el estado del led rojo
const char *fail_t = "grupo2/KL43/failLog";                // Topic donde se publican los fallos que haya tenido el sistema

// Parámetros de red
const char *ssid = "Luchito Wi-Fi";
const char *password = "00416778400";
const char *mqtt_server = "192.168.1.124";

// Estado de los elementos de la KL
bool sw1_state = 0;
bool sw3_state = 0;
bool redLED_state = 0;
bool greenLED_state = 0;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/**
 * @brief Analiza el buffer recibido en busca de comandoso y reacciona en función de ellos
 *
 * @param buf buffer recibido
 * @param len longitud de los datos del buffer
 */
void decode(char *buf, int len)
{
    // Es un reporte de aceleración y luz
    if (buf[0] == ':' && len == MAX_MES_LEN - 1)
    {
        char light[8];
        char acc_x[7];
        char acc_y[7];
        char acc_z[7];
        char aux[15];
        float acc_value;
        float light_value;

        // Luz
        aux[0] = buf[1];
        aux[1] = buf[2];
        aux[2] = buf[3];
        aux[3] = buf[4];
        aux[4] = 0;
        strcpy(light, aux);
        light_value = atof(light);
        light_value = 100.0 - (light_value * 100.0) / 4095.0; // Se puede ajustar ordenada y saturación
        sprintf(light, "%.2f", light_value);

        // Acel x
        aux[0] = buf[5];
        aux[1] = buf[6];
        aux[2] = buf[7];
        aux[3] = buf[8];
        aux[4] = 0;
        strcpy(acc_x, aux);
        acc_value = atof(acc_x) / 100.0;
        sprintf(acc_x, "%.2f", acc_value);

        // Acel y
        aux[0] = buf[9];
        aux[1] = buf[10];
        aux[2] = buf[11];
        aux[3] = buf[12];
        aux[4] = 0;
        strcpy(acc_y, aux);
        acc_value = atof(acc_y) / 100.0;
        sprintf(acc_y, "%.2f", acc_value);

        // Acel z
        aux[0] = buf[13];
        aux[1] = buf[14];
        aux[2] = buf[15];
        aux[3] = buf[16];
        aux[4] = 0;
        strcpy(acc_z, aux);
        acc_value = atof(acc_z) / 100.0;
        sprintf(acc_z, "%.2f", acc_value);

        // Publicamos
        client.publish(light_t, light);
        client.publish(accelx_t, acc_x);
        client.publish(accely_t, acc_y);
        client.publish(accelz_t, acc_z);
    }

    // Es una respuesta a comando
    if (buf[0] == ':' && buf[1] == '0' && len == 4)
    {
        // Formato :0xy, donde "x" es el led e "y" es la acción realizada
        char led = buf[2];
        char accion = buf[3];
        bool *ledstate;
        bool previous;
        const char *ledstate_t;
        char aux[2];

        // Verifico que el numero de led sea válido
        if (led != '1' && led != '2')
            return;

        // Verifico que la acción sea valida
        if (accion != 'A' && accion != 'E' && accion != 'T')
            return;

        // Es el led rojo?
        if (led == '1')
        {
            ledstate = &redLED_state;
            ledstate_t = redLEDstate_t;
        }
        else
        {
            ledstate = &greenLED_state;
            ledstate_t = greenLEDstate_t;
        }

        previous = *ledstate;

        switch (accion)
        {
        case 'T':
            *ledstate = !(*ledstate);
            break;
        case 'A':
            *ledstate = 0;
            break;
        case 'E':
            *ledstate = 1;
            break;

        default:
            break;
        }

        if (previous != *ledstate)
        {
            sprintf(aux, "%d", *ledstate);
            client.publish(ledstate_t, aux, 1);
        }
    }

    // Es un aviso de switch
    if (buf[0] == '!' && len == SWITCH_MESSAGE_LEN)
    {
        bool sw1 = buf[1] & 0b1;
        bool sw3 = buf[1] & 0b10;
        char sw[2] = " ";

        if (sw1_state != sw1)
        {
            sw1_state = sw1;
            sw[0] = sw1_state + 48;
            client.publish(sw1_t, sw, 1);
        }

        if (sw3_state != sw3)
        {
            sw3_state = sw3;
            sw[0] = sw3_state + 48;
            client.publish(sw3_t, sw, 1);
        }
    }
}

void listen_serial()
{
    static char buf[22];
    static int len = 0;

    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            decode(buf, len);
            len = 0;
        }
        else
        {
            if (c == ':')
                len = 0;

            buf[len++] = c;
            if (len > MAX_MES_LEN)
                len = 0;
        }
    }
}

/**
 * @brief Método de callback que se ejecuta si se recibe un mensaje por mqtt
 *
 * @param topic topic desde el cual se recibe el mensaje
 * @param payload mensaje
 * @param length longitud del mensaje
 */
void callback(char *topic, byte *payload, unsigned int length)
{
    // Comando del led verde
    if (strcmp(topic, greenLED_t) == 0)
    {
        // Verificación de seguridad
        char value = payload[0];
        if (length == 1 && (value == 'A' || value == 'E' || value == 'T'))
        {
            char mes[7];
            int i = 0;
            sprintf(mes, ":02%c\n", value);
            Serial.print(mes);
            delay(10);
        }
    }

    // Comando del led rojo
    if (strcmp(topic, redLED_t) == 0)
    {
        // Verificación de seguridad
        char value = payload[0];
        if (length == 1 && (value == 'A' || value == 'E' || value == 'T'))
        {
            char mes[7];
            int i = 0;
            sprintf(mes, ":01%c\n", value);
            Serial.print(mes);
            delay(10);
        }
    }

    // Estado del led rojo
    if (strcmp(topic, redLEDstate_t) == 0)
    {
        // Verificación de seguridad
        char value = payload[0];
        if (length == 1 && (value == '1' || value == '0'))
        {
            // Nos desubscribimos
            client.unsubscribe(redLEDstate_t);
            char mes[7];
            bool desired_state = value - 48;
            char action = (value == '1' ? 'E' : 'A');
            int i = 0;

            // Enviamos el valor deseado del led a la KL y esperamos que nos lo confirme
            sprintf(mes, ":01%c\n", action);
            Serial.print(mes);
            delay(30);
            listen_serial();
            while (desired_state != redLED_state)
            {
                Serial.print(mes);
                delay(30);
                listen_serial();
            }
        }
    }

    // Estad del led verde
    if (strcmp(topic, greenLEDstate_t) == 0)
    {
        // Verificación de seguridad
        char value = payload[0];
        if (length == 1 && (value == '1' || value == '0'))
        {
            // Nos desubscribimos
            client.unsubscribe(greenLEDstate_t);
            char mes[7];
            bool desired_state = value - 48;
            char action = (value == '1' ? 'E' : 'A');
            int i = 0;

            // Enviamos el valor deseado del led a la KL y esperamos que nos lo confirme
            sprintf(mes, ":02%c\n", action);
            Serial.print(mes);
            delay(30);
            listen_serial();
            while (desired_state != greenLED_state)
            {
                Serial.print(mes);
                delay(30);
                listen_serial();
            }
        }
    }
}

/**
 * @brief Método que se llama para (re)conectarese al broker MQTT
 *
 */
void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        if (DEBUG)
            Serial.println("Attempting MQTT connection...");

        digitalWrite(BROKER_LED, LOW);

        if (client.connect("KL43"))
        {
            // Damos aviso que estamos conectados
            client.publish(fail_t, "Device on");

            //  Y nos subscribimos a los topics correspondientes
            client.subscribe(redLED_t);        // Comando del led rojo
            client.subscribe(greenLED_t);      // Comando del led verde
            client.subscribe(redLEDstate_t);   // Estado del led rojo
            client.subscribe(greenLEDstate_t); // Estado del led verde

            digitalWrite(BROKER_LED, HIGH);
        }
        else
        {
            unsigned int tiempo_i = millis();
            while (millis() - tiempo_i < 5000)
            {
                digitalWrite(BROKER_LED, HIGH);
                delay(250);
                digitalWrite(BROKER_LED, LOW);
                delay(250);
            }
        }
    }
}

/*==================[external functions definition]==========================*/

/**
 * @brief Inicia todos los elementos necesarios para el funcionamiento del sistema
 *
 */
void sistema_begin()
{
    // Seteamos el comportamiento y estado de los pines digitales
    pinMode(WIFI_LED, OUTPUT);
    pinMode(BROKER_LED, OUTPUT);
    pinMode(ERROR_LED, OUTPUT);

    digitalWrite(WIFI_LED, LOW);
    digitalWrite(BROKER_LED, LOW);
    digitalWrite(ERROR_LED, LOW);

    // Iniciamos la instancia de comunicación con la KL
    Serial.begin(KL43_BAUDRATE);

    if (DEBUG)
    {
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);
    }

    // Iniciamos el wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Esperamos hasta estar conectados
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(WIFI_LED, LOW);
        delay(250);
        digitalWrite(WIFI_LED, HIGH);
        delay(250);
    }

    if (DEBUG)
    {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }

    // Se indica la conexión exitosa
    digitalWrite(WIFI_LED, HIGH);

    // Configuramos el servidor mqtt y la función de callback
    // client.setClient(espClient);
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    // Nos conectamos al broker
    reconnect();
    // Serial.println(WiFi.localIP());
}

/**
 * @brief Método que actualiza las variables del sistema
 *
 */
void sistema_run()
{
    // Verificamos conexión al broker MQTT
    if (!client.connected())
    {
        digitalWrite(BROKER_LED, LOW);
        reconnect();
    }

    // Verifica si debe realizar algún callback
    client.loop();

    // Escuchamos el puerto de la KL43
    listen_serial();
}

/*==================[end of file]============================================*/
