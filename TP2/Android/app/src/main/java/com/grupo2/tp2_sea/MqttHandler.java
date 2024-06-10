package com.grupo2.tp2_sea;

import android.util.Log;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.util.ArrayList;

public class MqttHandler {

    private MqttClient client;

    public void connect(String brokerUrl, String clientId, TopicCallback callback) {
        try {
            // Set up the persistence layer
            MemoryPersistence persistence = new MemoryPersistence();

            // Initialize the MQTT client
            client = new MqttClient(brokerUrl, clientId, persistence);

            // Set up the connection options
            MqttConnectOptions connectOptions = new MqttConnectOptions();
            connectOptions.setAutomaticReconnect(true);
            connectOptions.setCleanSession(true);

            client.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    Log.i("MQTT", "Conexion perdida");
                }

                @Override
                public void messageArrived(String topic, MqttMessage message) throws Exception {
                    String msgAsString = new String(message.getPayload());
                    Log.i("MQTT", "Topico: " + topic + " Msg: " + msgAsString);
                    callback.onMessageReceived(topic, msgAsString);
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    Log.i("MQTT", "Delivery completed");
                }
            });

            // Connect to the broker
            client.connect(connectOptions);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void disconnect() {
        try {
            client.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void publish(String topic, String message) {
        try {
            MqttMessage mqttMessage = new MqttMessage(message.getBytes());
            client.publish(topic, mqttMessage);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void subscribe(String topic) {
        try {
            client.subscribe(topic);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}
