package com.grupo2.tp2_sea.ui

import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawingPadding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.lifecycle.LiveData
import com.grupo2.tp2_sea.MainActivity
import com.grupo2.tp2_sea.ui.node_subscreen.Kl43
import com.grupo2.tp2_sea.ui.node_subscreen.NodeMCU
import com.grupo2.tp2_sea.ui.node_subscreen.RaspberryPi

fun MainActivity.renderUI(
    humidityState: LiveData<String>,
    temperatureState: LiveData<String>,
    accelXState: LiveData<String>,
    accelYState: LiveData<String>,
    accelZState: LiveData<String>,
    lightState: LiveData<String>,
    sw1State: LiveData<String>,
    sw3State: LiveData<String>,
    redLedState: LiveData<String>,
    greenLedState: LiveData<String>,
    onClickToggleRedLed: () -> Unit,
    onClickToggleGreenLed: () -> Unit
) {
    enableEdgeToEdge()
    setContent {
        val humidity by humidityState.observeAsState("")
        val temperature by temperatureState.observeAsState("")
        val accelX by accelXState.observeAsState("")
        val accelY by accelYState.observeAsState("")
        val accelZ by accelZState.observeAsState("")
        val light by lightState.observeAsState("")
        val sw1 by sw1State.observeAsState("")
        val sw3 by sw3State.observeAsState("")
        val redLed by redLedState.observeAsState("")
        val greenLed by greenLedState.observeAsState("")
        val state by remember(humidity, temperature, accelX, accelY, accelZ, light, sw1, sw3, redLed, greenLed) {
            derivedStateOf {
                UiState(humidity, temperature, accelX, accelY, accelZ, light, sw1, sw3, redLed, greenLed)
            }
        }
        SeaTheme {
            Surface(
                Modifier
                    .fillMaxSize()
                    .safeDrawingPadding()) {
                MainUI(
                    state,
                    onClickToggleRedLed = onClickToggleRedLed,
                    onClickToggleGreenLed = onClickToggleGreenLed,
                    modifier = Modifier.fillMaxSize()
                )
            }
        }
    }
}

@Composable
fun MainUI(
    state: UiState,
    onClickToggleRedLed: () -> Unit,
    onClickToggleGreenLed: () -> Unit,
    modifier: Modifier = Modifier
) {
    Column(modifier = modifier.padding(16.dp)) {
        Kl43(
            state.accelX,
            state.accelY,
            state.accelZ,
            state.light,
            state.sw1,
            state.sw3,
            state.redLed,
            state.greenLed,
            onClickToggleRedLed,
            onClickToggleGreenLed
        )
        Spacer(modifier = Modifier.height(72.dp))
        Row(Modifier.fillMaxWidth()) {
            RaspberryPi(state.temperature, modifier = Modifier.weight(1f))
            NodeMCU(state.humidity, modifier = Modifier.weight(1f))
        }
        Spacer(modifier = Modifier.weight(1f))
        Text(
            text = "TP2 SEA - Luciano Raffagnini - Sebastián I. Rodríguez",
            style = MaterialTheme.typography.bodyMedium
        )
    }
}

@Preview(device = "spec:width=411dp,height=891dp")
@Composable
private fun Preview() {
    val state = UiState(
        humidity = "45.4",
        temperature = "18.4",
        accelX = "0.5",
        accelY = "-1",
        accelZ = "-0.75",
        light = "75",
        sw1 = "1",
        sw3 = "0",
        redLed = "1",
        greenLed = "0"
    )
    SeaTheme(darkTheme = true) {
        Surface(Modifier.fillMaxSize()) {
            MainUI(
                state = state,
                onClickToggleGreenLed = {},
                onClickToggleRedLed = {}
            )
        }
    }
}