package com.example.app

import android.content.Context
import android.content.Intent
import android.net.nsd.NsdManager
import android.net.nsd.NsdServiceInfo
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import org.jetbrains.anko.toast
import java.net.InetAddress

private const val SERVICE_TYPE = "_http._tcp"
private const val TAG = "Group 2 - Debug"
class MainActivity : AppCompatActivity() {

    var nsdManager: NsdManager? = null
    private var mServiceInfo: NsdServiceInfo = NsdServiceInfo().apply {
        serviceName = "SmartCarApp"
        serviceType = SERVICE_TYPE
        port = 80
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        nsdManager = getSystemService(Context.NSD_SERVICE) as NsdManager
        connect_to_car.setOnClickListener { connectToCar() }
    }

    private fun connectToCar() {
        nsdManager!!.discoverServices(SERVICE_TYPE, NsdManager.PROTOCOL_DNS_SD, discoverListener)
    }

    private val discoverListener = object: NsdManager.DiscoveryListener {
        override fun onStartDiscoveryFailed(serviceType: String, errorCode: Int) {
            Log.e(com.example.app.TAG, "Discovery failed: Error code: $errorCode")
            nsdManager!!.stopServiceDiscovery(this)
        }

        override fun onStopDiscoveryFailed(serviceType: String?, errorCode: Int) {
            Log.e(com.example.app.TAG, "Discovery failed: Error code: $errorCode")
            nsdManager!!.stopServiceDiscovery(this)
        }

        override fun onDiscoveryStarted(regType: String) {
            // Start service discovery stuff
        }

        override fun onDiscoveryStopped(serviceType: String?) {
            Log.e(com.example.app.TAG, "Discovery stopped: $serviceType")
        }

        override fun onServiceFound(service: NsdServiceInfo) {
            // Found a service
            when {
                service.serviceType != SERVICE_TYPE -> Log.i(com.example.app.TAG, "Unknown Service")
                service.serviceName == mServiceInfo!!.serviceName -> Log.i(com.example.app.TAG, "Same machine: ${mServiceInfo!!.serviceName}")
                service.serviceName.contains("smartcar") -> nsdManager!!.resolveService(service, resolveListener)
                //TODO Correct the serviceName contains above to the car's service name.
            }
        }

        override fun onServiceLost(service: NsdServiceInfo) {
            // When service is no longer available
            Log.e(com.example.app.TAG, "service lost: $service")
        }
    }

    private val resolveListener = object: NsdManager.ResolveListener {
        override fun onResolveFailed(serviceInfo: NsdServiceInfo, errorCode: Int) {
            Log.e(com.example.app.TAG, "Error: $errorCode")
            toast("Connection to car failed.")
        }

        override fun onServiceResolved(serviceInfo: NsdServiceInfo) {
            Log.e(com.example.app.TAG, "Resolve succeeded. $serviceInfo")

            if (serviceInfo.serviceName == mServiceInfo!!.serviceName) {
                Log.d(com.example.app.TAG, "Same IP")
                return
            }
            mServiceInfo = serviceInfo
            mServiceInfo!!.port = serviceInfo.port
            val hostAddress: InetAddress = serviceInfo.host
            val intent = Intent(this@MainActivity, WifiConnectActivity::class.java)
            toast("Connected to car!")
            startActivity(intent)
        }
    }

    fun tearDown() {
        nsdManager!!.apply {
            stopServiceDiscovery(discoverListener)
        }
    }
}