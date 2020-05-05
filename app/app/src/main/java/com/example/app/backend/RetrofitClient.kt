package com.example.app.backend

import Endpoint
import retrofit2.Retrofit

object RetrofitClient {
    private const val URL = "smartcar.local/"

    val instance: Endpoint by lazy {
        val retro = Retrofit.Builder().baseUrl(URL).build()

        retro.create(Endpoint::class.java)
    }
}