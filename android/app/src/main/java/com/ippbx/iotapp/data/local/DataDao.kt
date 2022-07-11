package com.ippbx.iotapp.data.local

import androidx.room.*
import com.ippbx.iotapp.model.Data
import com.ippbx.iotapp.model.Sensor
import io.reactivex.Flowable
import io.reactivex.Single

@Dao()
interface DataDao {
    @Query("select MAX(data.dataId) from data")
    fun getLastData(): Long?

    @Query("select * from data")
    fun getAllData(): List<Data>

    @Query("select * from data where sensorId == :sensorId")
    fun getDataBySensorId(sensorId: Int): Flowable<List<Data>>

    @Query("select * from data where data.sync == 0 ")
    fun getNonSyncData(): List<Data>

    @Query("select * from data where data.sync == 0 and data.sensorId == :sensorId")
    fun getNonSyncDataSensor(sensorId: Long): List<Data>

    @Query("delete from data")
    fun deleteAllData()

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun addData(data: Data)

    @Update(onConflict = OnConflictStrategy.REPLACE)
    suspend fun updateData(data: Data)

    @Delete
    fun deleteData(data: Data)

    @Query("select * from sensors")
    fun getAllSensors(): List<Sensor>

    @Query("select * from sensors where macAddress==:macAddress")
    fun getSensorByMacAddress(macAddress: String): Single<Sensor>

    @Query("delete from sensors")
    fun deleteAllSensors()

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun addSensor(sensor: Sensor): Long

    @Update(onConflict = OnConflictStrategy.REPLACE)
    suspend fun updateSensor(sensor: Sensor)

    @Delete
    fun deleteSensor(sensor: Sensor)

}