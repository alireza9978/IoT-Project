package com.ippbx.iotapp.model

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "sensors")
data class Sensor(
    @PrimaryKey(autoGenerate = true)
    var id : Long,
    var macAddress: String,
    var sync: Boolean
)
