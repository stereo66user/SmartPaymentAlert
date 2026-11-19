package com.smartpaymentalert

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.provider.Settings
import android.widget.Button
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.TextView

class MainActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val prefs = getSharedPreferences("config", MODE_PRIVATE)
        val layout = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(28, 36, 28, 24)
        }
        val heading = TextView(this).apply { text = "Smart Payment Alert"; textSize = 24f }
        val note = TextView(this).apply {
            text = "Prototype only. A Google Pay notification is not proof that money reached the bank."
            textSize = 14f
        }
        val url = EditText(this).apply {
            hint = "ESP32 URL, e.g. http://192.168.55.126/notify"
            setText(prefs.getString("url", "http://192.168.55.126/notify"))
        }
        val token = EditText(this).apply {
            hint = "Same device token as ESP32"
            setText(prefs.getString("token", ""))
        }
        val status = TextView(this).apply { text = "Save the ESP32 URL and token, then enable notification access." }
        val save = Button(this).apply {
            text = "Save settings"
            setOnClickListener {
                prefs.edit().putString("url", url.text.toString().trim())
                    .putString("token", token.text.toString()).apply()
                status.text = "Settings saved."
            }
        }
        val permission = Button(this).apply {
            text = "Open notification access settings"
            setOnClickListener { startActivity(Intent(Settings.ACTION_NOTIFICATION_LISTENER_SETTINGS)) }
        }
        layout.addView(heading); layout.addView(note); layout.addView(url)
        layout.addView(token); layout.addView(save); layout.addView(permission); layout.addView(status)
        setContentView(layout)
    }
}
