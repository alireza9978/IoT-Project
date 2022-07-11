// var base_url = 'http://109.122.255.15:8000/'
var local_url = 'http://127.0.0.1:5500/'

var viewMode = getCookie("view-mode");

if(viewMode == "desktop"){
    viewport.setAttribute('content', 'width=1024');
}else if (viewMode == "mobile"){
    viewport.setAttribute('content', 'width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=no');
}
function getCookie(cname) {
    var name = cname + "=";
    var ca = document.cookie.split(';');
    for(var i=0; i<ca.length; i++) {
       var c = ca[i];
       while (c.charAt(0)==' ') c = c.substring(1);
       if(c.indexOf(name) == 0)
          return c.substring(name.length,c.length);
    }
    return "";
}

function gotologin(){
    document.location =  local_url+'login.html'
    
}

function logout(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if(this.readyState == 4 && this.status == 200){
            document.location = local_url +'index.html'; //url of logout
        }
    };
}