package com.ippbx.iotapp.model

import androidx.room.Entity
import androidx.room.ForeignKey
import androidx.room.PrimaryKey
import androidx.room.TypeConverters
import com.ippbx.iotapp.utils.DataTypeConverter
import java.util.*

@Entity(tableName = "data")
data class Data(
    @PrimaryKey(autoGenerate = true)
    var dataId: Long,
    var sensorId: Long,
    var energy: Float,
    var time: Long,
    var sync: Boolean = false,
)
