import android.os.Parcelable
import com.google.gson.annotations.SerializedName
import kotlinx.android.parcel.Parcelize

@Parcelize
data class Command (
    @SerializedName("commandID")
    val id: String? = ""
) : Parcelable