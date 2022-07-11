package com.ippbx.iotapp.views

import android.content.Context
import android.content.IntentFilter
import android.content.SharedPreferences
import android.net.wifi.WifiManager
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.widget.Button
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.ippbx.iotapp.R
import com.ippbx.iotapp.data.local.DataRepo
import com.ippbx.iotapp.data.remote.RetrofitService
import com.ippbx.iotapp.model.Data
import com.ippbx.iotapp.model.DataSample
import com.ippbx.iotapp.model.Sensor
import com.ippbx.iotapp.model.SensorData
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.rxkotlin.subscribeBy
import io.reactivex.schedulers.Schedulers
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import kotlin.coroutines.CoroutineContext


class MainActivity : AppCompatActivity(), CoroutineScope {
    private lateinit var receiver: WifiChangeReceiver
    private lateinit var button: Button

    @RequiresApi(Build.VERSION_CODES.N)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        button = findViewById(R.id.sync_button)

        receiver = WifiChangeReceiver()
        IntentFilter(WifiManager.NETWORK_STATE_CHANGED_ACTION).also {
            registerReceiver(receiver, it)
        }

        button.setOnClickListener {
            val repo = DataRepo(applicationContext)
            val sensors: List<Sensor> = repo.getAllSensors()
            for (sensor in sensors) {
                val datas: List<Data> = repo.getAllNotSyncedDataSensor(sensor.id)
                val dataSamples = ArrayList<DataSample>()
                for (data in datas) {
                    dataSamples.add(DataSample(data.energy, data.time))
                }
                if (sensor.sync) {
                    createSensor(sensor, dataSamples, datas, repo)
                } else {
                    sendData(dataSamples, sensor.macAddress, datas, repo)
                }
            }
        }
    }

    private fun getToken(): String {
        val sharedPreferences: SharedPreferences =
            this.getSharedPreferences("login_token", Context.MODE_PRIVATE)
        val token: String? = sharedPreferences.getString("token_login_value", "no_token")
        return "Bearer ${token!!}"
    }

    private fun createSensor(
        sensor: Sensor,
        datas: List<DataSample>,
        mainDatas: List<Data>,
        repo: DataRepo
    ) {
        val token: String = getToken()
        RetrofitService.api.createSensor(token, sensor.macAddress)
            .subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribeBy(
                onSuccess = {
                    Log.e("TAG", "sensor synced with server")
                    sendData(datas, sensor.macAddress, mainDatas, repo)
                    sensor.sync = true
                    launch {
                        repo.updateSensor(sensor)
                        Log.e("TAG", "sensor synced in database")
                    }
                },
                onError = {
                    Log.e("TAG", "sensor exists")
                }
            )
    }

    private fun sendData(
        datas: List<DataSample>,
        sensorMac: String,
        mainDatas: List<Data>,
        repo: DataRepo
    ) {
        val token: String = getToken()
        val body = SensorData(sensorMac, datas)
        RetrofitService.api3.syncServer(token, body)
            .subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribeBy(
                onSuccess = {
                    Log.e("TAG", "data synced with server")
                    for (data in mainDatas) {
                        data.sync = true
                        launch {
                            repo.updateData(data)
                            Log.e("TAG", "data synced in database")
                        }
                    }
                },
                onError = {
                    Log.e("TAG", "data exists")
                }
            )
    }

    private var job: Job = Job()

    override val coroutineContext: CoroutineContext
        get() = Dispatchers.Main + job

}