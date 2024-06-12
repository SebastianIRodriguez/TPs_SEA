package com.grupo2.tp2_sea.ui.node_subcreen

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.size
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.grupo2.tp2_sea.ui.gauge.SimpleRadialGauge

@Composable
fun NodeMCU(
    humidity: String,
    modifier: Modifier
) {
    Column(modifier, horizontalAlignment = Alignment.CenterHorizontally) {
        Text(
            text = "NodeMCU",
            style = MaterialTheme.typography.titleLarge
        )
        Spacer(modifier = Modifier.height(16.dp))
        SimpleRadialGauge(
            title = "Humedad",
            unit = "%",
            value = humidity.toFloatOrNull() ?: 0f,
            displayValue = humidity,
            min = 0f,
            max = 100f,
            modifier = Modifier.size(140.dp)
        )
    }
}