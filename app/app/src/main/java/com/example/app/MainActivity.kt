package com.example.app

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*
import org.jetbrains.anko.toast


class MainActivity : AppCompatActivity() {

    private var m_bluetoothAdapter: BluetoothAdapter? = null
    private val REQUEST_ENABLE_BLUETOOTH = 1

    companion object {
        const val CAR_ADDRESS: String = "FC:F5:C4:0F:87:62"
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        //Set m_bluetoothAdapter to devices Bluetooth
        m_bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        //Check to see if device support Bluetooth
        if(m_bluetoothAdapter == null){
            toast("Device does not support Bluetooth")
        }
        //Check to see if devices bluetooth is enabled. If not, prompt the user to enable it.
        if(!m_bluetoothAdapter!!.isEnabled){
            val enableBluetoothIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBluetoothIntent, REQUEST_ENABLE_BLUETOOTH)
        }

        //When clicking on the connect button, call connectCar-function
        connect_to_car.setOnClickListener{ connectCar()}

    }
    //Change view to the one when the car is connected
    private fun connectCar() {
        val intent = Intent(this, ConnectActivity::class.java)
        startActivity(intent)
    }
}
