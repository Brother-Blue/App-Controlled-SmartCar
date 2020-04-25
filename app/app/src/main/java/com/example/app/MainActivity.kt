package com.example.app

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.os.AsyncTask
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.ProgressBar
import java.io.IOException
import java.util.*

class MainActivity : AppCompatActivity() {

    companion object {
        lateinit var bluetooth: ConnectActivity
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    fun connectCar(view: View) {
        val intent = Intent(this, ConnectActivity::class.java)
        startActivity(intent)
    }

}