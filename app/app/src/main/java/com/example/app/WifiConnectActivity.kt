package com.example.app

import com.example.app.backend.RetrofitClient
import Command

import android.os.Bundle
import android.util.Log
import android.view.View

import retrofit2.Call
import retrofit2.Response
import retrofit2.Callback

import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*

import org.jetbrains.anko.toast

private const val TAG = "Group 2 - Debug:"

class WifiConnectActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_wificonnect)
    }

    fun sendCommand(v: View) {
        var commandID: String = when (v.getId()) {
            R.id.buttonForward -> "f"
            R.id.buttonBackward -> "b"
            R.id.buttonLeft -> "l"
            R.id.buttonRight -> "r"
            R.id.buttonStop -> "§"
            R.id.toggleDriveMode -> "I do nothing atm. Waiting for command to toggle auto"
            else -> "First kebab, then falafel"
        }

        RetrofitClient.instance.sendCommand(commandID).enqueue(object: Callback<List<Command>> {
            override fun onFailure(call: Call<List<Command>>, t: Throwable) {
                Log.e(TAG, "Input error")
                textView.text = "Failure"
            }

            override fun onResponse(call: Call<List<Command>>, response: Response<List<Command>>) {
                if (response.isSuccessful) {
                    Log.i(TAG, "Input validated")
                    textView.text = "Success"
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