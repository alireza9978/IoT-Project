package com.ippbx.iotapp.data.remote

import android.se.omapi.Session
import com.ippbx.iotapp.model.*
import io.reactivex.Single
import okhttp3.ResponseBody
import retrofit2.http.*

interface IoTApi {
    @FormUrlEncoded
    @POST("users/token/")
    fun login(
        @Field("username") username: String,
        @Field("password") password: String
    ): Single<LoginResponse>

    @POST("data_manager/new_sensor_data/")
    fun syncServer(
        @Header("authorization") auth: String,
        @Body() body: SensorData,
    ):Single<ResponseBody>

    @FormUrlEncoded
    @POST("sensors/sensors/")
    fun createSensor(
        @Header("Authorization") auth: String,
        @Field("mac") sensor_mac: String,
    ):Single<CreateSensorResponse>

    @GET("sync")
    fun sync(@Query("time")time:Long):Single<SyncResponse>

    @GET("sync_old")
    fun syncOld():Single<SyncResponse>

    @GET("mac")
    fun mac():Single<MacResponse>
}