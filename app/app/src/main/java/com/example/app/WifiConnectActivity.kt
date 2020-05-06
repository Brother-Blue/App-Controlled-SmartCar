package com.example.app

import com.example.app.backend.RetrofitClient
import Command
import android.os.Bundle
import android.util.Log
import retrofit2.Call
import retrofit2.Response
import retrofit2.Callback
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_wificonnect.*

import org.jetbrains.anko.toast

private const val TAG = "Group 2 - Debug:"

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
        RetrofitClient.instance.sendCommand(command).enqueue(object: Callback<List<Command>> {
            override fun onFailure(call: Call<List<Command>>, t: Throwable) {
                Log.e(TAG, "Input error")
            }

            override fun onResponse(call: Call<List<Command>>, response: Response<List<Command>>) {
                if (response.isSuccessful) {
                    Log.i(TAG, "Input validated")
                } else {
                    val message = when(response.code()) {
                        500 -> "Error 500: Internal server error."
                        401 -> "Error 401: Unauthorized access."
                        403 -> "Error 403: Forbidden input."
                        404 -> "Error 404: Command not found."
                        else -> "Please report this error to the developers."
                    }
                    toast(message)
                }
            }
        })
    }
}