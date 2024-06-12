package com.grupo2.tp2_sea.ui.gauge

import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.size
import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.drawWithCache
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Paint
import androidx.compose.ui.graphics.PaintingStyle
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.drawscope.drawIntoCanvas
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.rememberTextMeasurer
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.grupo2.tp2_sea.ui.SeaTheme
import com.grupo2.tp2_sea.ui.drawTextCentered
import com.grupo2.tp2_sea.ui.gaugeAnimation
import com.grupo2.tp2_sea.ui.radius

@Preview
@Composable
fun PreviewSimpleRadialGauge() {
    SeaTheme(darkTheme = true) {
        Column {
            SimpleRadialGauge(
                title = "RPM",
                unit = "rpm",
                value = 4567f,
                displayValue = "4567",
                min = 0f,
                max = 8000f,
                modifier = Modifier.size(400.dp)
            )
            SimpleRadialGauge(
                title = "Vacio",
                unit = "",
                value = 0f,
                displayValue = "0",
                min = 0f,
                max = 0f,
                modifier = Modifier.size(400.dp)
            )
        }
    }
}

@Composable
fun SimpleRadialGauge(
    modifier: Modifier = Modifier,
    title: String,
    unit: String,
    value: Float,
    displayValue: String,
    min: Float,
    max: Float,
    animationTimeMillis: Int = 400
) {
    val normalizedValue by remember(value, min, max) {
        derivedStateOf {
            if (min != max) ((value - min) / (max - min)).coerceIn(0f..1f)
            else 0f
        }
    }
    val percentage by animateFloatAsState(
        label = "Gauge fill percentage",
        targetValue = normalizedValue,
        animationSpec = gaugeAnimation(animationTimeMillis)
    )

    val orangeColor = MaterialTheme.colorScheme.secondary
    val titleColor = MaterialTheme.colorScheme.secondary
    val backgroundColor = Color(0xFF313137)

    val textMeasurer = rememberTextMeasurer(cacheSize = 3)

    Spacer(modifier.drawWithCache {
        val strokeWidth = this.size.minDimension / 2 * 0.1f
        val topLeft = Offset(strokeWidth / 2, strokeWidth / 2)
        val gaugeSize = Size(this.size.minDimension - strokeWidth, this.size.minDimension - strokeWidth)
        val stroke = Stroke(strokeWidth, cap = StrokeCap.Round)

        val paint = Paint().apply {
            style = PaintingStyle.Stroke
            this.strokeWidth = 8.dp.toPx()
            strokeCap = StrokeCap.Round
        }

        val transparent = orangeColor.copy(alpha = 0f).toArgb()
        val shadowColor = orangeColor.copy(alpha = .6f).toArgb()

        paint.asFrameworkPaint().apply {
            color = transparent
            setShadowLayer(
                4.dp.toPx(),
                0f,
                0f,
                shadowColor
            )
        }

        val titleStyle = TextStyle(
            color = titleColor,
            textAlign = TextAlign.Center,
            fontSize = (radius * 0.2f).toSp()
        )
        val valueStyle = TextStyle(
            color = Color(0xFFFFFFFF),
            textAlign = TextAlign.Center,
            fontSize = (radius * 0.5f).toSp(),
            fontWeight = FontWeight.Bold
        )
        val unitStyle = TextStyle(
            color = Color(0xFF888888),
            textAlign = TextAlign.Center,
            fontSize = (radius * 0.20f).toSp(),
            fontWeight = FontWeight.Light
        )

        onDrawBehind {
            drawArc(
                backgroundColor,
                startAngle = 120f,
                sweepAngle = 300f,
                useCenter = false,
                topLeft = topLeft,
                size = gaugeSize,
                style = stroke
            )

            drawTextCentered(
                textMeasurer,
                title,
                style = titleStyle,
                offset = Offset(center.x, center.y - radius * 0.5f)
            )
            drawTextCentered(
                title = displayValue,
                textMeasurer = textMeasurer,
                style = valueStyle,
                offset = Offset(center.x, center.y)
            )
            drawTextCentered(
                textMeasurer,
                unit,
                Offset(center.x, center.y + radius * 0.4f),
                unitStyle
            )

            if (percentage == 0f || min == max) return@onDrawBehind

            drawArc(
                orangeColor,
                startAngle = 120f,
                sweepAngle = 300f * percentage,
                useCenter = false,
                topLeft = topLeft,
                size = gaugeSize,
                style = stroke
            )

            drawIntoCanvas {
                it.drawArc(
                    left = topLeft.x,
                    top = topLeft.y,
                    right = topLeft.x + gaugeSize.width,
                    bottom = topLeft.y + gaugeSize.height,
                    startAngle = 120f,
                    sweepAngle = 300f * percentage,
                    useCenter = false,
                    paint = paint
                )
            }
        }
    })
}