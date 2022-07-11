package com.ippbx.iotapp.views

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Context.WIFI_SERVICE
import android.content.Intent
import android.net.NetworkInfo
import android.net.wifi.SupplicantState
import android.net.wifi.WifiInfo
import android.net.wifi.WifiManager
import android.util.Log
import com.ippbx.iotapp.data.local.DataRepo
import com.ippbx.iotapp.data.remote.RetrofitService
import com.ippbx.iotapp.model.Data
import com.ippbx.iotapp.model.Sensor
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.rxkotlin.subscribeBy
import io.reactivex.schedulers.Schedulers
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import kotlin.coroutines.CoroutineContext


class WifiChangeReceiver : BroadcastReceiver(), CoroutineScope {

    private var job: Job = Job()

    override val coroutineContext: CoroutineContext
        get() = Dispatchers.Main + job

    override fun onReceive(context: Context?, intent: Intent?) {

        if ((intent?.extras?.get("networkInfo") as NetworkInfo).state.name == "CONNECTED") {

            val repo = DataRepo(context!!)
            var sensors: List<Sensor> = repo.getAllSensors()
            val lastId: Long = repo.getLastDataId() + 1

            val wifiManager: WifiManager =
                context.applicationContext?.getSystemService(WIFI_SERVICE) as WifiManager
            val wifiInfo: WifiInfo = wifiManager.connectionInfo
            if (wifiInfo.supplicantState == SupplicantState.COMPLETED) {
                val ssid = wifiInfo.ssid
                Log.e("TAG", "WifiChangeReceiver: onReceive: connected to wifi with ssid = $ssid")
                if (ssid.toString().contains("iot_s_")) {
                    Log.e("TAG", "WifiChangeReceiver: onReceive: sensor connection")
                    RetrofitService.api2.mac()
                        .subscribeOn(Schedulers.io())
                        .observeOn(AndroidSchedulers.mainThread())
                        .subscribeBy(
                            onSuccess = {
                                val tempMac: String = it.mac.trim()
                                Log.e("TAG", "WifiChangeReceiver: onReceive: sensor mac = $tempMac")
                                var found = false
                                var sensorId: Long? = null
                                for (sensor in sensors) {
                                    if (sensor.macAddress == tempMac) {
                                        found = true
                                        sensorId = sensor.id
                                        break
                                    }
                                }

                                if (found) {
                                    Log.e("TAG", "WifiChangeReceiver: onReceive: sensor found")
                                    getDataAndSave(repo, lastId, sensorId)
                                } else {
                                    Log.e("TAG", "WifiChangeReceiver: onReceive: sensor not found")
                                    fun goOn(sensorId: Long?) {
                                        Log.e("TAG", "WifiChangeReceiver: onReceive: new sensor id = $sensorId")
                                        getDataAndSave(repo, lastId, sensorId)
                                    }
                                    launch {
                                        val newSensorId =
                                            repo.insertSensor(Sensor(0, tempMac, false))
                                        goOn(newSensorId)
                                    }
                                }

                            },
                            onError = {
                                Log.e("TAG", "WifiChangeReceiver: onReceive: ${it.message}")
                            }
                        )
                } else {
                    Log.e("TAG", "WifiChangeReceiver: onReceive: no connection with sensor ")
                }
            }
        }
    }

    private fun getDataAndSave(repo: DataRepo, lastIdInput: Long, sensorId: Long?) {
        val now = System.currentTimeMillis() / 1000
        Log.e("TAG", "WifiChangeReceiver: getDataAndSave: getting data")
        var lastId = lastIdInput
        RetrofitService.api2.syncOld()
            .subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribeBy(
                onSuccess = { it ->
                    val tempTime = it.time?.toLong()
                    val tempData = it.data
                    if (tempData != null && tempData.isNotEmpty()) {
                        Log.e("TAG", "get data not first time")
                        var currentTime: Long = tempTime!!
                        val datas = ArrayList<Data>()
                        for (i in tempData) {
                            datas.add(
                                Data(
                                    lastId,
                                    sensorId!!,
                                    i,
                                    currentTime,
                                    false
                                )
                            )
                            currentTime += 10
                            lastId += 1
                        }
                        launch {
                            repo.insertAllData(datas)
                            Log.e("TAG", "old data saved")
                        }
                    }

                    RetrofitService.api2.sync(now)
                        .subscribeOn(Schedulers.io())
                        .observeOn(AndroidSchedulers.mainThread())
                        .subscribeBy(
                            onSuccess = { innerIt ->
                                val innerTempTime = innerIt.time?.toLong()
                                val innerTempData = innerIt.data
                                if (innerTempData != null && innerTempTime != null) {
                                    if (innerTempTime == 0L && innerTempData.isNotEmpty()) {
                                        Log.e("TAG", "get data first time")
                                        var currentTime = now
                                        val datas = ArrayList<Data>()
                                        for (i in innerTempData.indices.reversed()) {
                                            datas.add(
                                                Data(
                                                    lastId,
                                                    sensorId!!,
                                                    innerTempData[i],
                                                    currentTime,
                                                    false
                                                )
                                            )
                                            currentTime -= 10
                                            lastId += 1
                                        }
                                        launch {
                                            repo.insertAllData(datas.reversed())
                                        }
                                    } else {
                                        Log.e("TAG", "get data not first time")
                                        var currentTime: Long = innerTempTime
                                        val datas = ArrayList<Data>()
                                        for (i in innerTempData) {
                                            datas.add(
                                                Data(
                                                    lastId,
                                                    sensorId!!,
                                                    i,
                                                    currentTime,
                                                    false
                                                )
                                            )
                                            currentTime += 10
                                            lastId += 1
                                        }
                                        launch {
                                            repo.insertAllData(datas)
                                        }
                                    }
                                    Log.e("TAG", "new data saved")
                                }
                            },
                            onError = {
                                Log.e("TAG", "WifiChangeReceiver: getDataAndSave: ${it.message}")
                            }
                        )

                },
                onError = {
                    Log.e("TAG", "WifiChangeReceiver: getDataAndSave: ${it.message}")
                }
            )
    }

}