package com.ippbx.iotapp.data.local

import android.content.Context
import android.os.AsyncTask
import com.ippbx.iotapp.model.Data
import com.ippbx.iotapp.model.Sensor
import com.ippbx.iotapp.utils.InsertDone
import com.rpsi.remote_services.data.local.AppDatabase

class DataRepo(context: Context) {

    private val context: Context = context
    private var db: DataDao = AppDatabase.getInstance(context)?.DataDao()!!

    fun getLastDataId(): Long {
        return db.getLastData() ?: return 0
    }

    fun getAllNotSyncedDataSensor(sensorId: Long): List<Data> {
        return db.getNonSyncDataSensor(sensorId)
    }

    fun getAllData(): List<Data> {
        return db.getAllData()
    }

    //Fetch All the Users
    fun getAllSensors(): List<Sensor> {
        return db.getAllSensors()
    }

    // Insert new user
    suspend fun insertSensor(sensor: Sensor) : Long {
        return db.addSensor(sensor)
    }

    suspend fun updateData(data: Data) {
        db.addData(data)
    }

    suspend fun updateSensor(sensor: Sensor) {
        db.updateSensor(sensor)

    }

    suspend fun insertAllData(datas: List<Data>) {
        for (data in datas)
            db.addData(data)
    }

}