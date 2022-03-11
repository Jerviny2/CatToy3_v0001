var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
    initButton();
    getCurrentValue();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}
function onOpen(event) {
    console.log('Connection opened');
    websocket.send("states");
}
function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    var myObj = JSON.parse(event.data);
    console.log(myObj);
    for (i in myObj.gpios){
    var output = myObj.gpios[i].output;
    var state = myObj.gpios[i].state;
    console.log(output);
    console.log(state);
    if ((state == "1") && (myObj.gpios[0].output == 0)){
    document.getElementById(output).checked = true;
    document.getElementById(output+"s").innerHTML = "ON";
    }
    else if ((state == "0") && (myObj.gpios[0].output == 0)) {
    document.getElementById(output).checked = false;
    document.getElementById(output+"s").innerHTML = "OFF";
    }
    else if (myObj.gpios[0].output == 1){
        document.getElementById("4").value = this.responseText;
        document.getElementById("4s").innerHTML = this.responseText;
    }
    }
    console.log(event.data);
}
// Send Requests to Control GPIOs
function toggleCheckbox (element) {
console.log(element.id);
websocket.send(element.id);
    if (element.checked){
        document.getElementById(element.id+"s").innerHTML = "ON";
    }
    else  {
        document.getElementById(element.id+"s").innerHTML = "OFF";
    }

}


function initButton() {
    document.getElementById('bON').addEventListener('click', toggleON);
    document.getElementById('bOFF').addEventListener('click', toggleOFF);
}
function toggleON(event) {
    websocket.send('bON');
}
function toggleOFF(event) {
    websocket.send('bOFF');
}
function getCurrentValue() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("4").value = this.responseText;
            document.getElementById("4s").innerHTML = this.responseText;
            }
        };
        xhr.open("GET", "/currentValue", true);
        xhr.send();
        }

function updateSliderPWM(element) {
    var sliderValue = document.getElementById("4").value;
    document.getElementById("4s").innerHTML = sliderValue;
    console.log(sliderValue);
    websocket.send(sliderValue);
}

// Function to get and update GPIO states on the webpage when it loads for the first time
function getStates(){
websocket.send("states", sliderValue);
}

window.addEventListener('load', onLoad);