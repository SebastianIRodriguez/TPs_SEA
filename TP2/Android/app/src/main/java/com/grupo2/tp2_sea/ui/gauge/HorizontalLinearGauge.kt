package com.grupo2.tp2_sea.ui.gauge

import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.aspectRatio
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.width
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Paint
import androidx.compose.ui.graphics.PaintingStyle
import androidx.compose.ui.graphics.compositeOver
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.drawIntoCanvas
import androidx.compose.ui.graphics.drawscope.inset
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.grupo2.tp2_sea.ui.SeaTheme
import com.grupo2.tp2_sea.ui.gaugeAnimation
import com.grupo2.tp2_sea.ui.getOptimalFontSize

@Composable
fun HorizontalLinearGauge(
    title: String,
    unit: String,
    value: Float,
    displayValue: String,
    min: Float,
    max: Float,
    animationTimeMillis: Int = 400,
    modifier: Modifier = Modifier
) {
    Column(modifier) {
        BoxWithConstraints(
            modifier = Modifier
                .fillMaxWidth()
                .weight(.6f)
        ) {
            val titleWidth by remember(maxWidth) {
                derivedStateOf { (maxWidth * .35f).value }
            }
            val valueWidth by remember(maxWidth) {
                derivedStateOf { (maxWidth * .125f).value }
            }
            val unitWidth by remember(maxWidth) {
                derivedStateOf { (maxWidth * .125f).value }
            }

            val titleFontSize by remember(titleWidth, maxHeight) {
                derivedStateOf {
                    getOptimalFontSize(
                        width = titleWidth,
                        height = maxHeight.value,
                        sampleText = "aaabbb"
                    ).sp
                }
            }

            val valueFontSize by remember(valueWidth, maxHeight) {
                derivedStateOf {
                    getOptimalFontSize(
                        width = valueWidth,
                        height = maxHeight.value,
                        sampleText = "aa"
                    ).sp
                }
            }

            val unitFontSize by remember(unitWidth, maxHeight) {
                derivedStateOf {
                    getOptimalFontSize(
                        width = unitWidth,
                        height = maxHeight.value,
                        sampleText = "aaa"
                    ).sp
                }
            }

            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text(
                    title,
                    fontSize = titleFontSize,
                    color = Color.White,
                    modifier = Modifier.alignByBaseline()
                )
                Spacer(modifier = Modifier.weight(1f))
                Text(
                    "$displayValue ",
                    fontSize = valueFontSize,
                    color = Color.White,
                    fontWeight = FontWeight.Medium,
                    modifier = Modifier.alignByBaseline()
                )
                Text(
                    unit,
                    fontSize = unitFontSize,
                    color = Color.White.copy(alpha = 0.5f),
                    fontWeight = FontWeight.Normal,
                    modifier = Modifier.alignByBaseline()
                )
            }
        }
        val normalizedValue by remember(value, min, max) {
            derivedStateOf {
                if (min != max) ((value - min) / (max - min)).coerceIn(0f..1f)
                else 0f
            }
        }
        val fillPercentage by animateFloatAsState(
            targetValue = normalizedValue,
            label = "Indicator Angle",
            animationSpec = gaugeAnimation(animationTimeMillis)
        )
        LinearIndicator(
            fillPercentage,
            modifier = Modifier.weight(.4f)
        )
    }
}

@Composable
fun LinearIndicator(
    percentage: Float,
    modifier: Modifier
) {
    val color1 = Color(red = 210, green = 157, blue = 172)
    val color2 = Color(red = 239, green = 184, blue = 200)
    val backgroundColor = Color(0xFF313137)

    Canvas(modifier = modifier
        .fillMaxWidth()
    ) {
        inset(
            top = size.height * 0f,
            bottom = 0f,
            left = 0f,
            right = 0f
        ) {
            drawIndicator(
                percentage,
                color1,
                color2,
                backgroundColor
            )
        }
    }
}

private fun DrawScope.drawIndicator(
    percentage: Float,
    color1: Color,
    color2: Color,
    backgroundColor: Color
) {
    val segmentHeight = size.height * .4f
    val halfSegmentHeight = segmentHeight / 2

    val brush = Brush.horizontalGradient(
        listOf(color1, color2),
        startX = 0f,
        endX = size.width
    )

    val segmentCount = 5

    val segmentWidth = this.size.width / segmentCount * 0.9f
    val quarterWidth = this.size.width / segmentCount

    val paint = Paint().apply {
        style = PaintingStyle.Stroke
        strokeWidth = segmentHeight
    }

    val transparent = color2.copy(alpha = 0f).toArgb()
    val shadowColor = color2.copy(alpha = .6f).toArgb()

    paint.asFrameworkPaint().apply {
        color = transparent
        setShadowLayer(
            halfSegmentHeight,
            0f,
            0f,
            shadowColor
        )
    }

    drawIntoCanvas {
        for (i in 0 until segmentCount) {
            //Draw background segment
            drawRoundRect(
                backgroundColor,
                topLeft = Offset(
                    x = i * quarterWidth,
                    y = center.y - halfSegmentHeight
                ),
                size = Size(segmentWidth, segmentHeight),
                cornerRadius = CornerRadius(halfSegmentHeight)
            )

            if (percentage < 0.25f * i && i > 0) continue
            val segmentCompletionPercentage = if(percentage == 0f && i == 0)
                0.1f
            else
                ((percentage - i / segmentCount.toFloat()) * segmentCount / 0.9f).coerceAtMost(1f)

            //Draw glow around segment
            it.drawRoundRect(
                i * quarterWidth,
                center.y,
                quarterWidth * i + segmentWidth * segmentCompletionPercentage,
                center.y,
                radiusX = halfSegmentHeight,
                radiusY = halfSegmentHeight,
                paint
            )

            //Draw green segment
            drawRoundRect(
                brush,
                topLeft = Offset(
                    x = i * quarterWidth,
                    y = center.y - halfSegmentHeight
                ),
                size = Size(segmentWidth * segmentCompletionPercentage, segmentHeight),
                cornerRadius = CornerRadius(halfSegmentHeight)
            )
        }
    }
}

@Preview(apiLevel = 31)
@Composable
private fun PreviewGauge() {
    SeaTheme {
        HorizontalLinearGauge(
            title = "Velocidad",
            unit = "Km/h", value = 65f,
            displayValue = "65",
            min = 0f,
            max = 200f,
            modifier = Modifier
                .width(400.dp)
                .aspectRatio(14f / 3f)
        )
    }
}

@Preview(apiLevel = 31)
@Composable
private fun PreviewGaugeFull() {
    SeaTheme {
        HorizontalLinearGauge(
            title = "Velocidad",
            unit = "Km/h",
            value = 200f,
            displayValue = "200",
            min = 0f,
            max = 200f,
            modifier = Modifier
                .width(400.dp)
                .aspectRatio(14f / 3f)
        )
    }
}

@Preview(apiLevel = 31)
@Composable
private fun PreviewGaugeExceedsMax() {
    SeaTheme {
        HorizontalLinearGauge(
            title = "Velocidad",
            unit = "Km/h",
            value = 201f,
            displayValue = "201",
            min = 0f,
            max = 200f,
            modifier = Modifier
                .width(400.dp)
                .aspectRatio(14f / 3f)
        )
    }
}

@Preview(apiLevel = 31)
@Composable
private fun PreviewGaugeExceedsMin() {
    SeaTheme {
        HorizontalLinearGauge(
            title = "Velocidad",
            unit = "Km/h",
            value = -1f,
            displayValue = "-1",
            min = 0f,
            max = 200f,
            modifier = Modifier
                .width(400.dp)
                .aspectRatio(14f / 3f)
        )
    }
}