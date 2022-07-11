var base_url = 'http://109.122.255.15:8000/'
var local_url = 'http://109.122.255.15/'

$("#username_input").change(function (){       
    document.getElementById('username_input').setAttribute('style',' direction: ltr;')
    console.log('hi')
});

$("#password_input").change(function (){
    document.getElementById('password_input').setAttribute('style',' direction: ltr;margin-bottom: 2rem;')

});
function login(){
    console.log('login')
    var user = document.getElementById('username_input').value;
    var pass = document.getElementById('password_input').value;


    const params = {
        username: user,
        password: pass
    }
    const http = new XMLHttpRequest()
    http.open('POST', base_url + 'users/token/', true)
    http.setRequestHeader('Content-Type', 'application/json')
    http.send(JSON.stringify(params)) // Make sure to stringify
    http.onload = function () {
        if (http.status == 200) {
            let response = JSON.parse(http.responseText)
            token = JSON.parse(http.responseText)['access'];
            window.localStorage.setItem("token", token);
            console.log(response)
            document.location = local_url+'index.html'; //check username and password then go to choose file page
        }else{
            alert("Try again")
        }

    }
}
