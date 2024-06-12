package com.grupo2.tp2_sea.ui

import androidx.compose.animation.core.AnimationSpec
import androidx.compose.animation.core.EaseInOut
import androidx.compose.animation.core.tween
import androidx.compose.ui.draw.CacheDrawScope
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Paint
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.TextMeasurer
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.drawText
import kotlin.math.floor

fun getOptimalFontSize(width: Float, height: Float, sampleText: String): Float {
    val paint = Paint().asFrameworkPaint()
    val sampleTextWidth = paint.measureText(sampleText)
    val sampleTextHeight = paint.fontMetrics.run { bottom - top + leading }
    val sampleTextFontSize = paint.textSize
    val sampleAspectRatio = sampleTextWidth / sampleTextHeight

    val targetAspecRatio = width / height

    val multiplier = if (sampleAspectRatio > targetAspecRatio) {
        //Scale by width
        width / sampleTextWidth
    } else {
        //Scale by Height
        height / sampleTextHeight
    }
    val fontSize = floor(multiplier * sampleTextFontSize * 0.9f)

    return fontSize
}

fun gaugeAnimation(durationMillis: Int): AnimationSpec<Float> =
    tween(durationMillis = durationMillis, easing = EaseInOut)

fun DrawScope.drawTextCentered(
    textMeasurer: TextMeasurer,
    title: String,
    offset: Offset,
    style: TextStyle
) {
    val textLayoutResult = textMeasurer.measure(text = AnnotatedString(title), style)
    val textSize = textLayoutResult.size
    drawText(textMeasurer, title, offset - Offset(textSize.width / 2f, textSize.height / 2f), style)
}

val CacheDrawScope.radius: Float
    get() = this.size.minDimension / 2

val DrawScope.radius: Float
    get() = this.size.minDimension / 2