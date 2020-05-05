
import retrofit2.Call
import retrofit2.http.*

interface Endpoint {
    @GET("command")
    fun sendCommand(@Query("commandID")commandID: String) : Call<List<Command>>
}