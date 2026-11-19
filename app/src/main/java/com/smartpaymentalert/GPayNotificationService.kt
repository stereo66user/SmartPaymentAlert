package com.smartpaymentalert

import android.service.notification.NotificationListenerService
import android.service.notification.StatusBarNotification
import android.util.Log
import java.net.HttpURLConnection
import java.net.URL
import java.net.URLEncoder
import java.util.concurrent.Executors

class GPayNotificationService : NotificationListenerService() {
    private val executor = Executors.newSingleThreadExecutor()
    private val gpayPackage = "com.google.android.apps.nbu.paisa.user"

    override fun onNotificationPosted(sbn: StatusBarNotification) {
        if (sbn.packageName != gpayPackage) return
        val extras = sbn.notification.extras
        val title = extras.getCharSequence("android.title")?.toString().orEmpty()
        val body = extras.getCharSequence("android.text")?.toString().orEmpty()
        val text = "$title $body"
        val match = Regex("(?:₹|Rs\\.?\\s*)([0-9][0-9,]*(?:\\.[0-9]{1,2})?)", RegexOption.IGNORE_CASE).find(text) ?: return
        val amount = match.groupValues[1].replace(",", "")
        val prefs = getSharedPreferences("config", MODE_PRIVATE)
        val baseUrl = prefs.getString("url", "").orEmpty()
        val token = prefs.getString("token", "").orEmpty()
        if (baseUrl.isBlank() || token.isBlank()) return

        executor.execute {
            var connection: HttpURLConnection? = null
            try {
                val query = "amount=${enc(amount)}&token=${enc(token)}"
                val separator = if (baseUrl.contains("?")) "&" else "?"
                connection = (URL(baseUrl + separator + query).openConnection() as HttpURLConnection).apply {
                    requestMethod = "GET"
                    connectTimeout = 5000
                    readTimeout = 5000
                }
                Log.i("SmartPaymentAlert", "ESP32 response: ${connection.responseCode}")
            } catch (e: Exception) {
                Log.e("SmartPaymentAlert", "Could not send test alert", e)
            } finally { connection?.disconnect() }
        }
    }

    private fun enc(value: String): String = URLEncoder.encode(value, Charsets.UTF_8.name())
}
