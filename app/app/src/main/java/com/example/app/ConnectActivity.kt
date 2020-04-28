package com.example.app

//import android.app.ProgressDialog
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.os.AsyncTask
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import java.io.IOException
import java.util.*

class ConnectActivity : AppCompatActivity() {

    companion object{
        var m_myUUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
        var m_bluetoothSocket: BluetoothSocket? = null
        //var m_progress: ProgressDialog? = null
        var m_bluetoothAdapter: BluetoothAdapter? = null
        var m_isConnected: Boolean = false
        var m_address: String? = null
    }

    override fun onCreate(savedInstanceState: Bundle?){
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_connect)
        //Get the car MAC-address that is set in MainActivity
        m_address = intent.getStringExtra(MainActivity.CAR_ADDRESS)
        ConnectToDevice(this).execute()
    }

    //Class in charge of connecting the device with the car
    private class ConnectToDevice(c: Context) : AsyncTask<Void, Void, String>(){

        private var connectSuccess: Boolean = true
        private val context: Context

        //Constructor
        init {
            this.context = c
        }

        override fun onPreExecute() {
            super.onPreExecute()
            //m_progress = ProgressDialog.show(context, "Connecting...", "please wait")
        }
        //Connect device to car
        override fun doInBackground(vararg params: Void?): String? {
            try {
                if (m_bluetoothSocket == null || !m_isConnected){
                    m_bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
                    val device: BluetoothDevice = m_bluetoothAdapter!!.getRemoteDevice(m_address)
                    m_bluetoothSocket = device.createInsecureRfcommSocketToServiceRecord(m_myUUID)
                    //Stop looking for other devices to save battery
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery()
                    m_bluetoothSocket!!.connect()
                }
            } catch (e: IOException) {
                connectSuccess = false
                e.printStackTrace()
            }
            return null
        }

        override fun onPostExecute(result: String?) {
            super.onPostExecute(result)
            if(!connectSuccess){
                Log.i("data", "could not connect")
            } else {
                m_isConnected = true
            }
            //m_progress?.dismiss()

        }
    }
}
