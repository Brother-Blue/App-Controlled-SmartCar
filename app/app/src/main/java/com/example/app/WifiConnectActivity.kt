package com.example.app

import android.content.Context
import android.os.AsyncTask
import android.os.Bundle
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_wificonnect.*
import org.jetbrains.anko.toast
import java.io.IOException
import java.util.*

private const val TAG = "Group 2 - Debug:"

class WifiConnectActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_wificonnect)
    }

    fun sendCommand(v: View) {
        var commandID: String = when (v.id) {
            R.id.buttonForward -> "f"
            R.id.buttonBackward -> "b"
            R.id.buttonLeft -> "l"
            R.id.buttonRight -> "r"
            R.id.buttonStop -> "§"
            R.id.toggleDriveMode -> "I do nothing atm. Waiting for command to toggle auto"
            else -> "YEET"
        }
    }
}