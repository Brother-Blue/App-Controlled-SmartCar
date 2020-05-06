package com.example.app

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_wificonnect.*

class WifiConnectActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_wificonnect)

        buttonForward.setOnClickListener { sendCommand("f") }
        buttonBackward.setOnClickListener { sendCommand("b") }
        buttonLeft.setOnClickListener { sendCommand("l") }
        buttonRight.setOnClickListener { sendCommand("r") }
        toggleDriveMode.setOnClickListener{ sendCommand("a") }
        buttonStop.setOnClickListener { "§ Kebab is life §" }
    }

    private fun sendCommand(command: String) {
        // WIP
    }
}