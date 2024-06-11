package com.grupo2.tp2_sea;

import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.OnApplyWindowInsetsListener;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import java.util.Objects;

public class MainActivity extends AppCompatActivity {

    private static final String BROKER_URL = "tcp://192.168.0.13:1883";
    private static final String CLIENT_ID = "Android_user";
    private static final String TOPIC_NAME = "grupo2/nodemcu/humedad";
    private MqttHandler mqttHandler;
    static Handler mHandler =  new Handler();
    private TextView humidityTextView;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), new OnApplyWindowInsetsListener() {
            @NonNull
            @Override
            public WindowInsetsCompat onApplyWindowInsets(@NonNull View v, @NonNull WindowInsetsCompat insets) {
                Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
                v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
                return insets;
            }
        });

        humidityTextView = findViewById(R.id.humedad);

        mqttHandler = new MqttHandler();
        mqttHandler.connect(BROKER_URL, CLIENT_ID, this::updateUI);
        mqttHandler.subscribe(TOPIC_NAME);
    }

    @Override
    protected void onDestroy() {
        mqttHandler.disconnect();
        super.onDestroy();
    }

    void updateUI(String topic, String msg) {
        if (Objects.equals(topic, TOPIC_NAME)) {
            try {
                mHandler.post(() -> humidityTextView.setText(msg));
            } catch (Exception e) {
                Log.i("MQTT", "Excepcion en el Textview");
            }
        }
    }
}
