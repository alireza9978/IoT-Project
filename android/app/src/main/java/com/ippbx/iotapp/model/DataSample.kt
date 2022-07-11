package com.ippbx.iotapp.model

import com.squareup.moshi.Json

class DataSample(
    @Json(name = "energy") var energy: Float,
    @Json(name = "time") var time: Long,
)


