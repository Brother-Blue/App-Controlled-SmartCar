package com.example.app.backend

import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory

object RetrofitClient {
    private const val URL = "smartcar.local"

    val instance: Endpoint by lazy {
        val retro = Retrofit.Builder()
            .addConverterFactory(GsonConverterFactory.create())
            .baseUrl(URL).build()

        retro.create(Endpoint::class.java)
    }
}