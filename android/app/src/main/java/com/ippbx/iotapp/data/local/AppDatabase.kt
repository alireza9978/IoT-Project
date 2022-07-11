package com.rpsi.remote_services.data.local

import android.content.Context
import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase
import androidx.room.TypeConverters
import com.ippbx.iotapp.data.local.DataDao
import com.ippbx.iotapp.model.Data
import com.ippbx.iotapp.model.Sensor
import com.ippbx.iotapp.utils.Constants
import com.ippbx.iotapp.utils.DataTypeConverter

@Database(entities = [Sensor::class, Data::class], version = 2, exportSchema = false)
abstract class AppDatabase : RoomDatabase() {
    companion object {
        private var INSTANCE: AppDatabase? = null

        fun getInstance(context: Context): AppDatabase? {
            if (INSTANCE == null) {
                synchronized(AppDatabase::class) {
                    INSTANCE = Room.databaseBuilder(context.applicationContext,
                        AppDatabase::class.java, "sensor.db").allowMainThreadQueries()
                        .build()
                }
            }
            return INSTANCE
        }

        fun destroyInstance() {
            INSTANCE = null
        }
    }

    abstract fun DataDao(): DataDao

}