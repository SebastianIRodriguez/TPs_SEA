package com.grupo2.tp2_sea;

import android.os.Bundle;
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

    private static final String BROKER_URL = "tcp://rpi.local:1883";
    private static final String CLIENT_ID = "Android_user";
    private MqttHandler mqttHandler;

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
        mqttHandler.subscribe("humidity");
        mqttHandler.publish("miTopic", "Hola, soy el celular!");
    }

    void updateUI(String topic, String msg) {
        if (Objects.equals(topic, "humidity")) { // grupo2/nodemcu/humidity
            humidityTextView.setText(msg);
        }
    }
}
