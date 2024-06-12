package com.grupo2.tp2_sea;

import android.os.Bundle;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.MutableLiveData;

import com.grupo2.tp2_sea.ui.UIKt;

public class MainActivity extends AppCompatActivity {

    private static final String BROKER_URL = "tcp://192.168.0.13:1883";
    private static final String CLIENT_ID = "Android_user";
    private static final String TOPIC_HUMIDITY = "grupo2/nodemcu/humedad";
    private static final String TOPIC_TEMPERATURE = "grupo2/rpi/temperatura";
    private static final String TOPIC_ACCELX = "grupo2/KL43/accelX";
    private static final String TOPIC_ACCELY = "grupo2/KL43/accelY";
    private static final String TOPIC_ACCELZ = "grupo2/KL43/accelZ";
    private static final String TOPIC_LIGHT = "grupo2/KL43/light";
    private static final String TOPIC_SW1 = "grupo2/KL43/sw1";
    private static final String TOPIC_SW3 = "grupo2/KL43/sw3";
    private static final String TOPIC_RED_LED = "grupo2/KL43/redLED";
    private static final String TOPIC_GREEN_LED = "grupo2/KL43/greenLED";
    private MqttHandler mqttHandler;
    private TextView humidityTextView;

    private final MutableLiveData<String> humidity = new MutableLiveData<String>();
    private final MutableLiveData<String> temperature = new MutableLiveData<String>();
    private final MutableLiveData<String> accelX = new MutableLiveData<String>();
    private final MutableLiveData<String> accelY = new MutableLiveData<String>();
    private final MutableLiveData<String> accelZ = new MutableLiveData<String>();
    private final MutableLiveData<String> light = new MutableLiveData<String>();
    private final MutableLiveData<String> sw1 = new MutableLiveData<String>();
    private final MutableLiveData<String> sw3 = new MutableLiveData<String>();

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        UIKt.renderUI(
                this,
                humidity,
                temperature,
                accelX,
                accelY,
                accelZ,
                light,
                sw1,
                sw3,
                () -> {
                    onToggleRedLed();
                    return null;
                },
                () -> {
                    onToggleGreenLed();
                    return null;
                }
        );

        mqttHandler = new MqttHandler();
        mqttHandler.connect(BROKER_URL, CLIENT_ID, this::updateUI);
        mqttHandler.subscribe(TOPIC_HUMIDITY);
        mqttHandler.subscribe(TOPIC_TEMPERATURE);
        mqttHandler.subscribe(TOPIC_ACCELX);
        mqttHandler.subscribe(TOPIC_ACCELY);
        mqttHandler.subscribe(TOPIC_ACCELZ);
        mqttHandler.subscribe(TOPIC_LIGHT);
        mqttHandler.subscribe(TOPIC_SW1);
        mqttHandler.subscribe(TOPIC_SW3);
    }

    @Override
    protected void onDestroy() {
        mqttHandler.disconnect();
        super.onDestroy();
    }

    void updateUI(String topic, String msg) {
        switch (topic) {
            case TOPIC_HUMIDITY:
                humidity.postValue(msg);
                break;
            case TOPIC_TEMPERATURE:
                temperature.postValue(msg);
                break;
            case TOPIC_ACCELX:
                accelX.postValue(msg);
                break;
            case TOPIC_ACCELY:
                accelY.postValue(msg);
                break;
            case TOPIC_ACCELZ:
                accelZ.postValue(msg);
                break;
            case TOPIC_LIGHT:
                light.postValue(msg);
                break;
            case TOPIC_SW1:
                sw1.postValue(msg);
                break;
            case TOPIC_SW3:
                sw3.postValue(msg);
                break;
        }
    }

    void onToggleRedLed() {
        mqttHandler.publish(TOPIC_RED_LED, "T");
    }

    void onToggleGreenLed() {
        mqttHandler.publish(TOPIC_RED_LED, "T");
    }
}
