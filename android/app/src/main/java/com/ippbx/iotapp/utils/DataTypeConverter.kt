package com.ippbx.iotapp.utils

import androidx.room.TypeConverter
import java.util.*

class DataTypeConverter {
    @TypeConverter
    fun toDate(value: Long): Date = Date(value)

    @TypeConverter
    fun fromDate(date:Date):Long = date.time
}