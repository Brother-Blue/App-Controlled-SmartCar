package com.example.app.backend

import Command
import retrofit2.Call
import retrofit2.http.*

interface Endpoint {
    @GET("/?{command}")
    fun sendCommand(@Path("command")commandID: String) : Call<List<Command>>
}