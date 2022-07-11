package com.ippbx.iotapp.model

import com.google.gson.annotations.SerializedName

data class CreateSensorResponse(
    @SerializedName("mac") var mac: String,
)