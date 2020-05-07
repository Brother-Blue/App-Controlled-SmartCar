package com.example.app

import android.content.Context
import android.content.Intent
import android.net.nsd.NsdManager
import android.net.nsd.NsdServiceInfo
import android.os.Bundle
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import org.jetbrains.anko.longToast
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
        progressBarMain.visibility = View.INVISIBLE
        buttonStopSearch.isEnabled = false
        nsdManager = getSystemService(Context.NSD_SERVICE) as NsdManager
        connect_to_car.setOnClickListener { connectToCar() }
        buttonStopSearch.setOnClickListener { stopSearch() }
    }

    private fun stopSearch() {
        buttonStopSearch.isEnabled = false
        progressBarMain.visibility = View.INVISIBLE
        nsdManager!!.stopServiceDiscovery(discoverListener)
        tearDown()
        connect_to_car.isEnabled = true
    }

    private fun connectToCar() {
        connect_to_car.isEnabled = false
        progressBarMain.visibility = View.VISIBLE
        nsdManager!!.discoverServices(SERVICE_TYPE, NsdManager.PROTOCOL_DNS_SD, discoverListener)
        buttonStopSearch.isEnabled = true
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

        override fun onDiscoveryStarted(serviceType: String?) {
            // Start service discovery stuff
            longToast("Searching for nearby car...")
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
            intent.putExtra(ADDRESS_KEY, hostAddress)
            intent.putExtra(PORT_KEY, mServiceInfo!!.port)
            toast("Connected to car!")
            startActivity(intent)
        }
    }

    fun tearDown() {
        nsdManager!!.apply {
            stopServiceDiscovery(discoverListener)
        }
    }

    companion object {
        private const val ADDRESS_KEY = "INET_HOST_ADDRESS"
        private const val PORT_KEY = "INET_ADDRESS_PORT"
    }
}