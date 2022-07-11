package com.ippbx.iotapp.model

import com.google.gson.annotations.SerializedName

data class SyncResponse(
    @SerializedName("time") var time: Int?,
    @SerializedName("data") var data: List<Float>?,
)