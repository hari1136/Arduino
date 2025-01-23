#include "Pin_Details.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


// Set your Static IP address
IPAddress local_IP(192, 168, 29, 184);
IPAddress gateway(192, 168, 29, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //

String RVoltage() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(Voltage[0][0])) {
    return "--";
  }
  else {
    return String( Voltage[0][0]);
  }
}
String YVoltage() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(Voltage[0][1])) {
    return "--";
  }
  else {
    return String( Voltage[0][1]);
  }
}
String BVoltage() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(Voltage[0][2])) {
    return "--";
  }
  else {
    return String( Voltage[0][2]);
  }
}
String RCurrent() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(Current[0][0])) {
    return "--";
  }
  else {
    return String(Current[0][0]);
  }
}
String YCurrent() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(Current[0][1])) {
    return "--";
  }
  else {
    return String(Current[0][1]);
  }
}
String BCurrent() {
  if (isnan(Current[0][2])) {
    return "--";
  }
  else {
    return String(Current[0][2]);
  }
}
String RWatt() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(W[0][0])) {
    return "--";
  }
  else {
    return String(W[0][0]);
  }
}
String YWatt() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(W[0][1])) {
    return "--";
  }
  else {
    return String(W[0][1]);
  }
}
String BWatt() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(W[0][2])) {
    return "--";
  }
  else {
    return String(W[0][2]);
  }
}
String RUnit() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(Unit[0])) {
    return "--";
  }
  else {
    return String(Unit[0]);
  }
}
String YUnit() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(Unit[1])) {
    return "--";
  }
  else {
    return String(Unit[1]);
  }
}
String BUnit() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(Unit[2])) {
    return "--";
  }
  else {
    return String(Unit[2]);
  }
}
String TUnit() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (isnan(TotalUnit)) {
    return "--";
  }
  else {
    return String(TotalUnit);
  }
}
String Error() {

  return String(Err);
  //  }
}
String Notification() {

  return String(Not);
  //  }
}
//
//String readTime() {
//  //      Serial.println(CurrentTimetoDisplay);
//
//}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Energy Meter Monitor</title>
   <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" type="image/png" href="favicon.png">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css">
  <style>
      .header {
      font-size: 1.2rem;
      font-weight: bold;
      margin-left: 15px;
    }
    .topnav {
      display: flex;
      justify-content: space-between;
      align-items: center;
      background-color: #143642;
      color: white;
      flex-direction: row;
    }
    .time-view {
      background-color: #ccd3d6;
      color: #143642;
      padding: 20px 10px;
      border-radius: 0;
      font-size: 1rem;
      margin-left: auto;
      display: flex;
      flex-direction: column;
      gap: 10px;
    }
    .view {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    #current-date, #current-time {
      font-size: 0.8rem;
      font-weight: bold;
    }
   
    .content {
      padding: 20px;
      max-width: 1200px;
      margin: 0 auto;
    }
      .state {
       font-size: 1.5rem;
       color:#8c8c8c;
       font-weight: bold;
      }
    .state-box {
      display: flex;
      justify-content: space-around;
      margin: 20px 0;
    }
    .state-box .card {
      padding: 10px;
      text-align: center;
      width: 45%;
      border-radius: 10px;
      color: white;
      font-weight: bold;
      font-size: 1.2rem;
    }
    .state-box .card.on {
      background-color: green;
    }
    .state-box .card.off {
      background-color: red;
    }
  


    table {
      width: 100%;
      border-collapse: collapse;
       
      text-align: center;
    }

    thead th {
      background-color: #143642;
      color: white; 
     padding-left: 135px;
      padding-right:130px;
      padding-top:15px;
       padding-bottom:15px;
      font-size: 18px;
      text-align:center;
    }
 thead th tr{
   text-align:center;
}
    tbody td {
      padding: 10px;
      border: 1px solid #ddd;
      font-size: 16px;
       text-align:center;
    }

    tbody tr:nth-child(odd) {
      background-color: #f9f9f9;
      text-align:center;
    }

    tbody tr:nth-child(even) {
      background-color: #ffffff;
    }
    /* Center the Total Unit cell */
    td#tunit2 {
      text-align: center;
      font-weight: bold;
    }
  /* Center the Total Unit cell */
    td#tunit1 {
      text-align: center;
      font-weight: bold;color: white;
      background-color: #143642;
    }
    .card-grid {
        max-width: 800px;
        margin: 0 auto;
        display: grid;
        grid-gap: 2rem;
        grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    }
    .card {
      background-color: white;
      border-radius: 10px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
      padding: 20px;
      width: 300px;
      text-align: center;
      transition: transform 0.2s ease-in-out;
    }
    .card:hover {
      transform: scale(1.05);
    }
    .card h2 {
      font-size: 1.5rem;
      color: #143642;
    }
    .card p {
      font-size: 1rem;
      margin: 10px 0;
    }
    .card .button {
      background-color: #143642;
      color: white;
      padding: 10px 20px;
      border: none;
      border-radius: 5px;
      font-size: 1rem;
      cursor: pointer;
    }
    .card .button:hover {
      background-color: #0a1e26;
    }
    .notification-box, .error-box {
      padding: 20px;
      margin: 20px 0;
      border-radius: 10px;
      font-size: 1rem;
    }
    .notification-box {
      background-color: #d9ffb3;
      color: #00796b;
      text-align: center;
      font-family:bold;
    }
    .error-box {
      background-color: #ffe6e6;
      color: #ff1919;
      text-align: center;
      font-family:bold;
    }

    /* Mobile view adjustments */
    @media (max-width: 768px) {
      .topnav h1 {
        font-size: 1.2rem;
        text-align: center;
        margin-bottom: 10px;
      }

      .state-box {
        flex-direction: column;
        align-items: center;
      }
        .state {
       font-size: 1.5rem;
       color:#8c8c8c;
       font-weight: bold;
      }

      .state-box .card {
        width: 80%;
        margin: 10px 0;
      }

      .card-grid {
        max-width: 800px;
        margin: 0 auto;
        display: grid;
        grid-gap: 2rem;
        grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      }

      .card {
        width: 70%;
        margin: 10px 0;
      }

      .notification-box, .error-box {
        font-size: 0.9rem;
        font-family:bold;
        padding: 15px;
      }

      .time-view {
        margin-left: 0;
        text-align: right;
      }
    }

    /* Extra small screens */
    @media (max-width: 480px) {
      .topnav {
        flex-direction: row;
        text-align: center;
      }
      .time-view {
          background-color: white;
          color: #143642;
          padding: 5px 5px;
          border-radius: 5px;
          font-size: 0.8rem;
          margin-left: auto;
        }
      .state {
       font-size: 1.5rem;
       color:#8c8c8c;
       font-weight: bold;
      }

      .table-container table {
        font-size: 0.8rem;
      }

      .card-grid {
        max-width: 400px;
        margin: 0 auto;
        display: grid;
        grid-gap: 1rem;
        grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
      }
    }
        @media (max-width: 768px) {
    .card{
      padding: 20px;
     width: 130px;
    }
    }




/* For laptops (1024px to 1366px) */
@media screen and (min-width: 1024px) and (max-width: 1366px) {
  table {
    font-size: 14px; /* Slightly smaller font for compact layouts */
  }

  th, td {
    padding: 8px; /* Reduce padding for a tighter look */
  }

  tbody tr:nth-child(even) {
    background-color: #f5f5f5; /* Slight color adjustment */
  }
}


    /* Responsive styles */
@media screen and (max-width: 768px) {
  table {
    display: block;
    overflow-x: auto;
    white-space: nowrap;
  }
}

/* Adjust for smaller screens */
@media screen and (max-width: 480px) {
  table {
    border: 0;
  }

  thead {
    display: none; /* Hide the header for small screens */
  }

  tbody, tr, td {
    display: block;
    width: 100%;
  }

  tr {
    margin-bottom: 15px;
  }

  td {
    text-align: left;
    padding-left: 60%; /* Adjust padding to fit smaller screens */
    position: relative;
  }

#tunit2{
  background-color:#143642;
  color:#fff;
}
}
  td::before {
 /* Dynamically add column label using data-label */
    position: absolute;
    left: 10px;
    top: 10px;
    text-align: left;
    font-weight: bold;

  }
}

  </style>
</head>
<body>
 <div class="topnav">
    <h2 class="header">Energy Meter Monitor</h2>
 <div style="background-color: #ccd3d6; color: #143642; padding: 20px 10px; border-radius: 0px; font-size: 1rem; margin-left: auto; display: flex; flex-direction: column; gap: 10px; margin-bottom: 0px;">
      <div style="display: flex; align-items: center; gap: 10px; flex-direction: row;">
        <i class="fas fa-calendar-day"></i>
        <div>
          <span id="date" style="font-size: 0.8rem; font-weight: bold;">%DATE%</span>
        </div>
      </div>
      <div style="display: flex; align-items: center; gap: 10px; flex-direction: row;">
        <i class="fas fa-clock"></i>
        <div>
          <span id="timq" style="font-size: 0.8rem; font-weight: bold;">%TIME%</span>
        </div>
      </div>
    </div>
  </div>
  <div style="color: #143642; background:white; padding: 20px 10px; border-radius: 0px; font-size: 1rem; margin-left: auto; display: flex; flex-direction: column; gap: 10px; margin-bottom: 0px;">
    <div style="display: flex; align-items: center; gap: 10px; flex-direction: row; justify-content: space-between;">
      <div></div> 
      <!-- Refresh icon that reloads the page on click -->
      <i class="fas fa-sync" style="cursor: pointer;" onclick="window.location.reload();"></i>
    </div>
  </div>
  <div class="content">
    <!-- Card Grid -->
    <div class="card-grid">
      <!-- RELAY Output Card -->
      <div class="card">
        <h2>RESTART</h2>
        <p class="state">State: <span id="state">%STATE%</span></p>
        <p><button id="button1" class="button">CHANGE</button></p>
      </div>
      <!-- LIGHT Output Card -->
      <div class="card">
        <h2>LIGHT</h2>
        <p class="state">State: <span id="lstate">%lSTATE%</span></p>
        <p><button id="button2" class="button">CHANGE</button></p>
      </div>
    </div>

    <!-- Notification View -->
    <div class="notification-box" id="notificationBox">
      <p><span>Notification: </span><span id="notification">%NOTIFICATION%</span></p>
    </div>

    <!-- Error View -->
    <div class="error-box" id="errorBox">
      <p><span>Error: </span><span id="error">%ERROR%</span></p>
    </div>

    <table>
        <thead>
          <tr>
            <th>Parameters</th>
            <th>R</th>
            <th>Y</th>
            <th>B</th>
          </tr>
        </thead>
        <tbody>
         <tr>
            <td data-label="Parameters" id="tunit2">Voltage (V)</td>
            <td data-label="R"  id="rvoltage">%RVOLTAGE%</td>
            <td data-label="y" id="yvoltage">%YVOLTAGE%</td>
            <td data-label="B" id="bvoltage">%BVOLTAGE%</td>
          </tr>
          <tr>
            <td data-label="Parameters" id="tunit2">Current (A)</td>
            <td data-label="R" id="rcurrent">%RCURRENT%</td>
            <td data-label="Y" id="ycurrent">%YCURRENT%</td>
            <td data-label="B" id="bcurrent">%BCURRENT%</td>
          </tr>
          <tr>
            <td data-label="Parameters" id="tunit2">Watt (W)</td>
            <td data-label="R" id="rwatt">%RWATT%</td>
            <td data-label="Y" id="ywatt">%YWATT%</td>
            <td data-label="B" id="bwatt">%BWATT%</td>
          </tr>
          <tr>
            <td data-label="Parameters" id="tunit2">Unit (kWh)</td>
            <td data-label="R" id="runit">%RUNIT%</td>
            <td data-label="Y" id="yunit">%YUNIT%</td>
            <td data-label="B" id="bunit">%BUNIT%</td>
          </tr>
          <tr>
            <td colspan="2" id="tunit1">Total Unit</td>
            <td colspan="2" id="tunit1">%TUNIT%
            <span style="font-size: smaller;">kWh</span>
            </td>
            
          </tr>
        </tbody>
        </div>
      </table>
  </div>

    <!-- Data Table -->

      
   
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {

     var RelayStates = event.data.split(',');
   var state = (RelayStates[0] == "1") ? "ON" : "OFF";
    document.getElementById('state').innerHTML = state;
  var lstate = (RelayStates[1] == "1") ? "ON" : "OFF";
    document.getElementById('lstate').innerHTML = lstate;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button1').addEventListener('click', toggle1);
    document.getElementById('button2').addEventListener('click', toggle2);
  }
  function toggle1(){
    websocket.send('toggle1');
  }

   function toggle2(){
    websocket.send('toggle2');
  }
  setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rvoltage").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/rvoltage", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("yvoltage").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/yvoltage", true);
  xhttp.send();
}, 10000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("bvoltage").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/bvoltage", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rcurrent").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/rcurrent", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ycurrent").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/ycurrent", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("bcurrent").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/bcurrent", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rwatt").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/rwatt", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ywatt").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/ywatt", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("bwatt").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/bwatt", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("runit").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/runit", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("yunit").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/yunit", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("bunit").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/bunit", true);
  xhttp.send();
}, 10000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("tunit").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/tunit", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("timq").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/timq", true);
  xhttp.send();
}, 10000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("date").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/date", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("error").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/error", true);
  xhttp.send();
}, 10000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("notification").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/notification", true);
  xhttp.send();
}, 10000 ) ;

</script>
 <ul>
        <li><a href="/daytime.html">Daytime Page</a></li>
        <li><a href="/monthcycle.html">Month Cycle</a></li>
        <li><a href="/logs.html">logs Page</a></li>
    </ul>
</body>
</html>
)rawliteral";

void notifyClients() {
  
 ws.textAll(String(RelayState) + "," + String(digitalRead(LightPin)));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle1") == 0) {
      RelayState = !RelayState;
//      notifyClients();
    }
       else if (strcmp((char*)data, "toggle2") == 0) {
      Relay2State = !Relay2State;
//      notifyClients();
    }
    
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
