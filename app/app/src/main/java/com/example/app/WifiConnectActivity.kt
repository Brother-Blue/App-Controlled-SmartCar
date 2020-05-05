package com.example.app

import android.content.Context
import android.os.AsyncTask
import android.os.Bundle
import android.util.Log
import kotlinx.android.synthetic.main.activity_wificonnect.*
import org.jetbrains.anko.toast
import java.io.IOException
import java.util.*

private const val TAG = "Group 2 - Debug:"

class WifiConnectActivity {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_wificonnect)

        /*
        buttonForward.setOnClickListener { sendMessage("f") }
        buttonBackward.setOnClickListener { sendMessage("b") }
        buttonLeft.setOnClickListener { sendMessage("l") }
        buttonRight.setOnClickListener { sendMessage("r") }
        buttonStop.setOnClickListener { sendMessage("§") }
        buttonExit.setOnClickListener { disconnect() }
        toggleDriveMode.setOnClickListener{
            if (toggleDriveMode.isChecked) {
                toast("Automatic driving is WIP.")
            }
        }
         */
    }
    
}