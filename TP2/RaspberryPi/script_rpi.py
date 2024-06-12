import time
from gpiozero import CPUTemperature
from paho.mqtt import client as mqtt

# Objeto para la medicion de temperatura interna (también se encuentra en el archivo /sys/class/thermal/thermal_zone0/temp) 
cpu = CPUTemperature()

# Datos mqtt
clientID = "Rpi"
broker = "192.168.1.120"
port = 1883

# Topic donde se reporta la temperatura
temp_topic = "grupo2/rpi/temperatura"
config_topic = "grupo2/rpi/config"
echo_topic = "grupo2/rpi/echo"

# Cantidad de milisegundos hasta q se reporta la temperatura
period = 1

# Tratamiendo del callback echo
def on_message_echo(mosq, obj, msg):
    print(f"Se recibió el mensaje: \"{msg.payload.decode()}\"")
    
# Tratamiento del callback de config
def on_message_config(mosq, obj, msg):
    per = int(msg.payload)
    if 1 <= per and per <= 60 :
        global period
        period = per
        print(f"El nuevo periodo de reporte es {period} segundos")
    else:
        print("Se recibio un dato erroneo")
		
client = mqtt.Client(clientID)

#Agrego funciones de callback para los topics a los cual ando subscripto
client.message_callback_add(echo_topic, on_message_echo)
client.message_callback_add(config_topic, on_message_config)

#Nos conectamos al broker
client.connect(broker,port)

#Nos subscribimos a los topics correspondientes
client.subscribe(echo_topic)
client.subscribe(config_topic)

# Publico un mensaje de que me conecté 
client.publish("miTopic","La RPI envía sus saludos al broker!")

# Elemento que corre en backround para manejar los mensajes recibidos
client.loop_start()

try:
    while (1):
	    time.sleep(period)
	    client.publish(temp_topic,"{temp:.2f}".format(temp=cpu.temperature))
	
except KeyboardInterrupt:
    print("Hasta la proximaaaa")
    client.disconnect()
    client.loop_stop()

# client.loop_forever()
