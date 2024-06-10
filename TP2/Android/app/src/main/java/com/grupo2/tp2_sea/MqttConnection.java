package com.grupo2.tp2_sea;

import android.content.Context;
import android.util.Log;

//import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.nio.charset.StandardCharsets;

public class MqttConnection {
    //Codigo de ejemplo:
    //https://github.com/eclipse/paho.mqtt.java/tree/master/org.eclipse.paho.sample.mqttclient/src/main/java/org/eclipse/paho/sample/mqttclient/mqttv5
   /* private MqttAndroidClient client;

    public MqttConnection(Context context) {
        try {
            client = new MqttAndroidClient(context, "tcp://rpi.local:1883", "android-node");
            MqttConnectOptions connOpts = new MqttConnectOptions();
            client.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {

                }

                @Override
                public void messageArrived(String topic, MqttMessage message) throws Exception {
                    Log.i("MQTT", "Topic: " + topic + " Message: " + new String(message.getPayload(), StandardCharsets.UTF_8));
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {

                }
            });
            client.connect(connOpts);
            client.subscribe("inTopic", 0);
        } catch (org.eclipse.paho.client.mqttv3.MqttException e) {
            Log.i("MQTT", "Excepcion");
            //Hacer algo
        }
    }

    void publish() {
        try {
            client.publish("topicAndroid", "Mensajito".getBytes(), 0, false);
        } catch (MqttException e) {
            //Hacer algo
        }
    }*/
}