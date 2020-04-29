package com.example.app

import android.bluetooth.BluetoothAdapter
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*
import org.jetbrains.anko.toast



class MainActivity : AppCompatActivity() {

    private val REQUEST_ENABLE_BLUETOOTH = 1
    private val mBluetoothAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        toggleConnectButton(false)
        switch_bluetooth.isChecked = mBluetoothAdapter?.isEnabled == true

        // Check to see if device support Bluetooth

        if (mBluetoothAdapter == null) {
            toast("This device does not support Bluetooth.")
        }
        // Check to see if devices bluetooth is enabled. If not, prompt the user to enable it.
        if (mBluetoothAdapter?.isEnabled == false) {
            toggleBluetooth()
        } else {
            toggleConnectButton(true)
        }
        //When clicking on the connect button, call connectCar-function
        connect_to_car.setOnClickListener{ connectCar()}
        switch_bluetooth.setOnClickListener {
            toggleBluetooth()
        }
    }

    private fun toggleBluetooth() {
        if (mBluetoothAdapter?.isEnabled == false) {
            val enableBluetoothIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBluetoothIntent, REQUEST_ENABLE_BLUETOOTH)
        } else if (mBluetoothAdapter?.isEnabled == true) {
            mBluetoothAdapter.disable()
            toggleConnectButton(false)
            toast("Please enable bluetooth to continue using the application.")
        }

    }

    private fun toggleConnectButton(value: Boolean) {
        connect_to_car.isEnabled = value
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == REQUEST_ENABLE_BLUETOOTH) {
            when (resultCode) {
                RESULT_OK -> {
                    toggleConnectButton(true)
                    switch_bluetooth.isChecked = true
                }
                else -> {
                    toast("Bluetooth is required for this application.")
                    toggleConnectButton(false)
                    switch_bluetooth.isChecked = false
                }
            }
        }
    }

    //Change view to the one when the car is connected
    private fun connectCar() {
        val intent = Intent(this, ConnectActivity::class.java)
        startActivity(intent)
    }
}