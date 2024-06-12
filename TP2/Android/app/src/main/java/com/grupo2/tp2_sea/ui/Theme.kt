package com.grupo2.tp2_sea.ui

import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable

private val LightColorScheme = lightColorScheme(
    //primary = md_theme_light_primary,
    //onPrimary = md_theme_light_onPrimary,
    //primaryContainer = md_theme_light_primaryContainer,
    // ..
)
private val DarkColorScheme = darkColorScheme(
    //primary = md_theme_dark_primary,
    //onPrimary = md_theme_dark_onPrimary,
    //primaryContainer = md_theme_dark_primaryContainer,
    // ..
)

@Composable
fun SeaTheme(
    darkTheme: Boolean = isSystemInDarkTheme(),
    content: @Composable () -> Unit
) {
    val colorScheme =
        if (!darkTheme) {
            LightColorScheme
        } else {
            DarkColorScheme
        }
    MaterialTheme(
        colorScheme = colorScheme,
        content = content
    )
}
