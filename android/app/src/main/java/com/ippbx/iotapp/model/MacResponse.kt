package com.ippbx.iotapp.model

import com.google.gson.annotations.SerializedName

data class MacResponse(
    @SerializedName("mac address") var mac: String,
)