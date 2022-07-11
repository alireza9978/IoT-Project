package com.ippbx.iotapp.views

import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.ippbx.iotapp.R
import com.ippbx.iotapp.data.remote.RetrofitService
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.rxkotlin.subscribeBy
import io.reactivex.schedulers.Schedulers

class LoginActivity : AppCompatActivity() {
    private val TAG = "LoginActivity"
    lateinit var username: EditText
    lateinit var password: EditText
    lateinit var button: Button
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_login)
        initViews()
        button.setOnClickListener {
            if (username.text.isNullOrEmpty())
                username.error = "invalid input!"
            else if (password.text.isNullOrEmpty())
                password.error = "invalid input!"
            else
                login(username = username.text.toString(), password = password.text.toString())
        }
    }

    private fun login(username: String, password: String) {
        RetrofitService.api.login(username, password)
            .subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribeBy(
                onSuccess = {
                    Toast.makeText(this, it.accessToken, Toast.LENGTH_SHORT).show()
                    val sharedPreferences: SharedPreferences =
                        this.getSharedPreferences("login_token", Context.MODE_PRIVATE)
                    val editor:SharedPreferences.Editor =  sharedPreferences.edit()
                    editor.putString("token_login_value", it.accessToken)
                    editor.apply()
                    startActivity(Intent(this,MainActivity::class.java))
                    finish()
                },
                onError = {
                    Log.e(TAG, "login: ${it.message}")
                }
            )
    }

    private fun initViews() {
        username = findViewById(R.id.login_username)
        password = findViewById(R.id.login_password)
        button = findViewById(R.id.login_button)
    }
}