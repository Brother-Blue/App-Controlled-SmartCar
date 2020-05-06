package com.example.app

import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        connect_to_car.setOnClickListener { connectToCar() }
    }

    private fun connectToCar() {
        val intent = Intent(this, WifiConnectActivity::class.java)
        startActivity(intent)
    }
}