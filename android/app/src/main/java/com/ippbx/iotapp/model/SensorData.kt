package com.ippbx.iotapp.model

import com.squareup.moshi.Json

class SensorData(
    @Json(name = "sensor_mac") var sensor_mac: String,
    @Json(name = "sensor_data") var sensor_data: List<DataSample>
)
