var hostreader = new XMLHttpRequest() || new ActiveXObject('MSXML2.XMLHTTP');

function loadhost() {
    hostreader.open('get', '/hostname.txt', true); 
    hostreader.onreadystatechange = displayContents();
    hostreader.send(null);
}

function TextRead(pathOfFileToReadFrom)

{
        var request = new XMLHttpRequest();
        request.open("GET", pathOfFileToReadFrom, false);
        request.send(null);
        var returnValue = request.responseText;

        return returnValue;
    }

function displayContents() {
    if(hostreader.readyState==4) {
        var el = document.getElementById('hostnameHeader');
        el.innerHTML = hostreader.responseText;
    }
}

function loadDataWidgets() {
    
    var hostname = TextRead("/hostname.txt");
    document.getElementById('hostnameHeader').innerHTML = hostname;

    var ip = TextRead("/ipaddress.txt");
    document.getElementById('ipHeader').innerHTML = ip;

    var tp = TextRead("/tpversion.txt");
    document.getElementById('tpHeader').innerHTML = tp;
}