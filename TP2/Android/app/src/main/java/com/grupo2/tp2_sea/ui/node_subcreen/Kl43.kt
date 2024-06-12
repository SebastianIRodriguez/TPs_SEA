package com.grupo2.tp2_sea.ui.node_subcreen

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ColumnScope
import androidx.compose.foundation.layout.IntrinsicSize
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.aspectRatio
import androidx.compose.foundation.layout.defaultMinSize
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.requiredHeight
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.wrapContentHeight
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.drawBehind
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.unit.dp
import com.grupo2.tp2_sea.ui.gauge.HorizontalLinearGauge
import com.grupo2.tp2_sea.ui.gauge.SimpleRadialGauge

@Composable
fun ColumnScope.Kl43(
    accelX: String,
    accelY: String,
    accelZ: String,
    light: String,
    sw1: String,
    sw3: String,
    onClickToggleRedLed: () -> Unit,
    onClickToggleGreenLed: () -> Unit,
    modifier: Modifier = Modifier
) {
    Text(text = "Dashboard TP2", style = MaterialTheme.typography.displayMedium)
    Spacer(modifier = Modifier.height(32.dp))
    Text(
        text = "KL43Z",
        style = MaterialTheme.typography.titleLarge
    )
    Spacer(modifier = Modifier.height(16.dp))
    Row(
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.SpaceBetween
    ) {
        Column {
            HorizontalLinearGauge(
                title = "Aceleración X",
                unit = "G",
                value = accelX.toFloatOrNull() ?: 0f,
                displayValue = accelX,
                min = -1f,
                max = 1f,
                modifier = Modifier.size(175.dp, 50.dp)
            )
            Spacer(modifier = Modifier.height(16.dp))
            HorizontalLinearGauge(
                title = "Aceleración Y",
                unit = "G",
                value = accelY.toFloatOrNull() ?: 0f,
                displayValue = accelY,
                min = -1f,
                max = 1f,
                modifier = Modifier.size(175.dp, 50.dp)
            )
            Spacer(modifier = Modifier.height(16.dp))
            HorizontalLinearGauge(
                title = "Aceleración Z",
                unit = "G",
                value = accelZ.toFloatOrNull() ?: 0f,
                displayValue = accelZ,
                min = -1f,
                max = 1f,
                modifier = Modifier.size(175.dp, 50.dp)
            )
        }
        Column(Modifier.fillMaxWidth(), horizontalAlignment = Alignment.CenterHorizontally) {
            SimpleRadialGauge(
                title = "Luz",
                unit = "%",
                value = light.toFloatOrNull() ?: 0f,
                displayValue = light,
                min = 0f,
                max = 100f,
                modifier = Modifier.size(100.dp)
            )
            Spacer(modifier = Modifier.height(16.dp))
            Row(Modifier.height(IntrinsicSize.Min)) {
                Text(text = "SW1")
                Spacer(modifier = Modifier.width(16.dp))
                Indicator(on = sw1 == "1")
            }
            Spacer(modifier = Modifier.height(16.dp))
            Row(Modifier.height(IntrinsicSize.Min)) {
                Text(text = "SW3")
                Spacer(modifier = Modifier.width(16.dp))
                Indicator(on = sw3 == "1")
            }
        }
    }
    Spacer(modifier = Modifier.height(16.dp))

    Row(Modifier.fillMaxWidth()) {
        Button(onClick = onClickToggleRedLed, modifier = Modifier.weight(1f)) {
            Text(text = "Toggle LED Rojo")
        }
        Spacer(modifier = Modifier.width(16.dp))
        Button(onClick = onClickToggleGreenLed, modifier = Modifier.weight(1f)) {
            Text(text = "Toggle LED Verde")
        }
    }
}

@Composable
fun Indicator(on: Boolean) {
    val color = if (on) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.primaryContainer
    Spacer(modifier = Modifier
        .fillMaxHeight(1f)
        .aspectRatio(1f)
        .drawBehind { drawRoundRect(color, cornerRadius = CornerRadius(size.maxDimension * .25f)) })
}