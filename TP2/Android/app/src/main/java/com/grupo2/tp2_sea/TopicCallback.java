package com.grupo2.tp2_sea;

public interface TopicCallback {
    void onMessageReceived(String topic, String msg);
}
