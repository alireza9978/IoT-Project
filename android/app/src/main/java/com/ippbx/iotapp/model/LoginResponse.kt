package com.ippbx.iotapp.model

import com.google.gson.annotations.SerializedName

data class LoginResponse(
    @SerializedName("access") var accessToken:String,
)