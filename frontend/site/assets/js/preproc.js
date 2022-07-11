var base_url = 'http://109.122.255.15:8000/'
var local_url = 'http://109.122.255.15/'


function getUsers(){
    let table = document.getElementById('filter_table')
    let counter = 0
    let token = window.localStorage.getItem("token")
    const http = new XMLHttpRequest()
    http.open('GET', base_url + 'users/admin/users/', true)
    http.setRequestHeader('Authorization', 'Bearer ' + token)
    http.send()
    http.onload = function () {
        if (http.status == 200) {
            let sensors = JSON.parse(http.responseText)['results']
            console.log(sensors)

            var tr = document.createElement('tr')

            for (var i = 0; i < sensors.length; i++) {
                tr = table.insertRow(-1)
                
                let amaliat = tr.insertCell(-1)
                let name = tr.insertCell(-1)
                let number = tr.insertCell(-1)

                let li7 = document.createElement('li')
                let button7 = document.createElement('button')

                number.innerHTML = counter++;
                name.setAttribute('id' , sensors[i]['id'])
                name.setAttribute('class','openPage')
                name.innerHTML = sensors[i]['username'] //ddddd
                

                li7.setAttribute('class', 'fa fa-eye')
                button7.setAttribute('type', 'button')
                button7.setAttribute('class', 'btn waves-effect waves-light btn-rounded btn-xs btn-info')
                button7.setAttribute('id', sensors[i]['id'])
                button7.setAttribute('onclick', 'openSensorsPage(this.id)')
                button7.setAttribute('style', ' margin-right:4.5px')

                button7.appendChild(li7)
                amaliat.appendChild(button7)

                tr.appendChild(number)
                tr.appendChild(name)
                tr.appendChild(amaliat)
                table.appendChild(tr)
            }

        }
    }
}
function getSensors(){
    
    let user_id = window.localStorage.getItem('user_id') 
    let table = document.getElementById('filter_table')
    let counter = 0
    let token = window.localStorage.getItem("token")
    const http = new XMLHttpRequest()
    http.open('GET', base_url + 'sensors/sensors/?user=' + user_id, true)
    http.setRequestHeader('Authorization', 'Bearer ' + token)
    http.send()
    http.onload = function () {
        if (http.status == 200) {
            let sensors = JSON.parse(http.responseText)['results']
            console.log(sensors)

            var tr = document.createElement('tr')

            for (var i = 0; i < sensors.length; i++) {
                tr = table.insertRow(-1)
                
                let amaliat = tr.insertCell(-1)
                let name = tr.insertCell(-1)
                let number = tr.insertCell(-1)

                let li7 = document.createElement('li')
                let button7 = document.createElement('button')

                number.innerHTML = counter++;
                name.setAttribute('id' , sensors[i]['mac'])
                name.innerHTML = sensors[i]['mac'] //ddddd
                

                li7.setAttribute('class', 'fa fa-eye')
                button7.setAttribute('type', 'button')
                button7.setAttribute('class', 'btn waves-effect waves-light btn-rounded btn-xs btn-info')
                button7.setAttribute('id', sensors[i]['mac'])
                button7.setAttribute('onclick', 'event.preventDefault();showSensorPage(this.id)')
                button7.setAttribute('style', ' margin-right:4.5px')

                button7.appendChild(li7)
                amaliat.appendChild(button7)

                tr.appendChild(number)
                tr.appendChild(name)
                tr.appendChild(amaliat)
                table.appendChild(tr)
            }

        }
    }
}
function getCons(){
    let mac = window.localStorage.getItem('sensor_mac') 
    let table = document.getElementById('filter_table')
    let counter = 0
    let token = window.localStorage.getItem("token")
    const http = new XMLHttpRequest()
    http.open('GET', base_url + 'data_manager/sensor_data/?sensor=' + mac, true)
    http.setRequestHeader('Authorization', 'Bearer ' + token)
    http.send()
    http.onload = function () {
        if (http.status == 200) {
            let sensors = JSON.parse(http.responseText)['results']
            console.log(sensors)

            var tr = document.createElement('tr')

            for (var i = 0; i < sensors.length; i++) {
                tr = table.insertRow(-1)
                
                let time = tr.insertCell(-1)
                let energy = tr.insertCell(-1)
                let number = tr.insertCell(-1)

                let temp = sensors[i]['time'].split('T')

                number.innerHTML = counter++;
                energy.innerHTML = sensors[i]['energy'] //ddddd
                time.innerHTML = temp[1].split('+')[0] + ' ' + temp[0]

                tr.appendChild(number)
                tr.appendChild(energy)
                tr.appendChild(time)
                table.appendChild(tr)
            }

        }
    }
}
function showSensorPage(mac){
    window.localStorage.setItem('sensor_mac' , mac)
    window.location.replace(local_url + 'sensor_cons.html')
}
function openSensorsPage(id){
    window.localStorage.setItem('user_id' , id)
    window.location.replace(local_url + 'sensors.html')
}


function chart_sensors_user(){
    let mac = window.localStorage.getItem('sensor_mac') 
    let token = window.localStorage.getItem("token")
    const http = new XMLHttpRequest()
    http.open('GET', base_url + 'data_manager/cumulative_digram/?mac=' + mac, true)
    http.setRequestHeader('Authorization', 'Bearer ' + token)
    http.send()
    http.onload = function () {
        if (http.status == 200) {
            var chart_labels = []
            var consumption = []
            let sensors = JSON.parse(http.responseText)
            console.log(sensors)

            for (var i = 0; i < sensors.length; i++) {
                let temp = sensors[i]['time'].split('T')
                chart_labels.push(temp[1].split('+')[0].split('.')[0] + ' ' + temp[0])
                consumption.push(sensors[i]['sum'])
            }
            console.log(consumption)
            const labels = chart_labels;
            const chartData = {
                labels: labels,
                datasets: [{
                    label: 'Energy consumption',
                    data: consumption,
                    fill: false,
                    borderColor: 'rgb(75, 192, 192)',
                    tension: 0.4
                }]
            };
            var ctx = document.getElementById("myChart2").getContext("2d");
            var myBar = new Chart(ctx, {
                type: 'line',
                data: {...chartData},
                options: {
                    scales: {
                        y: {
                            // beginAtZero: true
                            stacked: true
                        }
                    }
                }
            })
            
        }
    }
}

