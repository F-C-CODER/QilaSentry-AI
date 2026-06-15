#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ================= WIFI =================
const char* ssid = "Redmi";
const char* password = "12345678";

// ================= SERVER =================
ESP8266WebServer server(80);

// ================= PINS =================
#define MQ135_PIN A0
#define DHTPIN D2
#define DHTTYPE DHT11
#define TOUCH_SENSOR D8
#define BUZZER D5
#define MOISTURE_SENSOR D6
#define VIBRATION_SENSOR D7

DHT dht(DHTPIN, DHTTYPE);

// ================= SENSOR VALUES =================
int airValue;
float temperature;
float humidity;
int moistureState;
int touchState;
int vibration;

// ================= DATA STORAGE (for graphs) =================
#define MAX_POINTS 20
int airData[MAX_POINTS];
int moistureData[MAX_POINTS];
int vibrationData[MAX_POINTS];
float tempData[MAX_POINTS];
float humData[MAX_POINTS];
int touchData[MAX_POINTS];
int indexPos = 0;

// ================= HTML PAGE =================
String getPage()
{
  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>Red Fort Monitoring System</title>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

<style>
body{
  margin:0;
  font-family:Arial;
  background:#f4f4f4;
  color:white;
  text-align:center;
}
.header{
  background-image:url('https://images.unsplash.com/photo-1685790582503-1b2762d95407?q=80&w=870&auto=format&fit=crop');
  background-size:cover;
  background-position:center;
  height:220px;

  display:flex;
  justify-content:center;
  align-items:center;

  box-shadow:0 4px 10px rgba(0,0,0,0.4);
}

.header h1{
  background:rgba(0,0,0,0.6);
  padding:15px 30px;
  border-radius:10px;

  font-size:36px;
  color:yellow;
}
.card {
  background: rgba(0,0,0,0.6);
  margin:10px;
  padding:10px;
  border-radius:10px;
}
.graphContainer{
  display:flex;
  flex-wrap:wrap;
  justify-content:center;
  gap:15px;
  padding:10px;
}

.graphCard{
  width:32%;
  min-width:320px;
  background:rgba(0,0,0,0.6);
  padding:10px;
  border-radius:10px;
}

button{
  padding:12px 18px;
  margin:5px;
  cursor:pointer;
  border:none;
  border-radius:10px;
  color:white;
  font-size:16px;
  font-weight:bold;
}

.airBtn{
  background:#4CAF50;
}

.tempBtn{
  background:#ff9800;
}

.humBtn{
  background:#2196F3;
}

.moistBtn{
  background:#9C27B0;
}

.vibBtn{
  background:#F44336;
}

.touchBtn{
  background:#795548;
}

button:hover{
  transform:scale(1.05);
}

canvas{
  background:white;
  width:100%;
  height:220px !important;
  border-radius:10px;
}
</style>
</head>

<body>

<div class="header">

<h1>QilaSentry AI</h1>

</div>


<div class="card">
<p id="status"></p>
</div>

<div class="graphContainer">

  <div class="graphCard">
    <canvas id="airChart"></canvas>
  </div>

  <div class="graphCard">
    <canvas id="tempChart"></canvas>
  </div>

  <div class="graphCard">
    <canvas id="humChart"></canvas>
  </div>

  <div class="graphCard">
    <canvas id="moistChart"></canvas>
  </div>

  <div class="graphCard">
    <canvas id="vibChart"></canvas>
  </div>

  <div class="graphCard">
    <canvas id="touchChart"></canvas>
  </div>

</div>

<audio id="beep" src="https://actions.google.com/sounds/v1/alarms/beep_short.ogg"></audio>

<script>

let airChart;
let tempChart;
let humChart;
let moistChart;
let vibChart;
let touchChart;

function updateStatus()
{
  fetch("/live")
  .then(res=>res.json())
  .then(data=>{

document.getElementById("status").innerHTML =

"<div style='display:flex;flex-wrap:wrap;justify-content:center;gap:10px;'>"+

"<div style='background:#4CAF50;padding:12px;border-radius:10px;width:180px;'>" +
"<b>Air Quality</b><br>" +
data.air + "</div>" +

"<div style='background:#ff9800;padding:12px;border-radius:10px;width:180px;'>" +
"<b>Temperature</b><br>" +
data.temp + " &deg;C</div>" +

"<div style='background:#2196F3;padding:12px;border-radius:10px;width:180px;'>" +
"<b>Humidity</b><br>" +
data.hum + " %</div>" +

"<div style='background:#9C27B0;padding:12px;border-radius:10px;width:180px;'>" +
"<b>Moisture</b><br>" +
data.moist + "</div>" +

"<div style='background:#F44336;padding:12px;border-radius:10px;width:180px;'>" +
"<b>Vibration</b><br>" +
data.vib + "</div>" +

"<div style='background:#795548;padding:12px;border-radius:10px;width:180px;'>" +
"<b>Touch</b><br>" +
data.touch + "</div>" +

"</div>";

    if(data.touch==1 || data.vib==1)
    {
      document.getElementById("beep").play();
    }

  });
}

setInterval(updateStatus,2000);

updateStatus();
function updateGraphs()
{
  fetch("/data?type=all")
  .then(res=>res.json())
  .then(data=>{

    createOrUpdateChart(
      "airChart",
      "Air Quality Monitoring",
      "Time",
      "Air Quality Index",
      data.labels,
      data.air,
      "#4CAF50",
      airChart,
      c => airChart = c
    );

    createOrUpdateChart(
      "tempChart",
      "Temperature Monitoring",
      "Time",
      "Temperature (°C)",
      data.labels,
      data.temp,
      "#ff9800",
      tempChart,
      c => tempChart = c
    );

    createOrUpdateChart(
      "humChart",
      "Humidity Monitoring",
      "Time",
      "Humidity (%)",
      data.labels,
      data.hum,
      "#2196F3",
      humChart,
      c => humChart = c
    );

    createOrUpdateChart(
      "moistChart",
      "Moisture Monitoring",
      "Time",
      "Moisture Status",
      data.labels,
      data.moist,
      "#9C27B0",
      moistChart,
      c => moistChart = c
    );

    createOrUpdateChart(
      "vibChart",
      "Vibration Monitoring",
      "Time",
      "Vibration Status",
      data.labels,
      data.vib,
      "#F44336",
      vibChart,
      c => vibChart = c
    );

    createOrUpdateChart(
      "touchChart",
      "Touch Monitoring",
      "Time",
      "Touch Status",
      data.labels,
      data.touch,
      "#795548",
      touchChart,
      c => touchChart = c
    );

  });
}

setInterval(updateGraphs,2000);

updateGraphs();
function createOrUpdateChart(
id,title,xLabel,yLabel,
labels,values,color,
chartObj,setChart)
{
  const ctx =
  document.getElementById(id).getContext('2d');

  if(chartObj)
  {
    chartObj.destroy();
  }

  chartObj = new Chart(ctx,{
    type:'line',

    data:{
      labels:labels,
      datasets:[{
        label:title,
        data:values,
        borderColor:color,
        borderWidth:3,
        fill:false,
        tension:0.3
      }]
    },

    options:{
      responsive:true,

      plugins:{
        title:{
          display:true,
          text:title
        }
      },

      scales:{
        x:{
          title:{
            display:true,
            text:xLabel
          }
        },

        y:{
          beginAtZero:true,

          title:{
            display:true,
            text:yLabel
          }
        }
      }
    }
  });

  setChart(chartObj);
}
</script>

</body>
</html>
)rawliteral";

  return page;
}

// ================= SETUP =================
void setup()
{
  Serial.begin(115200);

  pinMode(TOUCH_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(MOISTURE_SENSOR, INPUT);
  pinMode(VIBRATION_SENSOR, INPUT);

  dht.begin();

  WiFi.begin(ssid, password);

  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println(WiFi.localIP());

  server.on("/", [](){
    server.send(200,"text/html",getPage());
  });

  // LIVE DATA API
  server.on("/live", [](){

  String airStatus;

  if (airValue < 850) airStatus = "Excellent";
  else if (airValue < 900) airStatus = "Good";
  else if (airValue < 950) airStatus = "Moderate";
  else if (airValue < 1000) airStatus = "Poor";
  else airStatus = "Hazardous";

  String moistureStatus = (moistureState == LOW) ? "Yes" : "No";
  String vibrationStatus = (vibration == HIGH) ? "Yes" : "No";
  String touchStatus = (touchState == HIGH) ? "Yes" : "No";

  String json = "{";
  json += "\"air\":\"" + airStatus + "\",";
  json += "\"temp\":" + String(temperature) + ",";
  json += "\"hum\":" + String(humidity) + ",";
  json += "\"moist\":\"" + moistureStatus + "\",";
  json += "\"touch\":\"" + touchStatus + "\",";
  json += "\"vib\":\"" + vibrationStatus + "\"";
  json += "}";

  server.send(200, "application/json", json);
});
server.on("/data", [](){

  String json = "{";

  // Labels
  json += "\"labels\":[";
  for(int i=0;i<MAX_POINTS;i++)
  {
    json += "\"" + String(i+1) + "\"";

    if(i<MAX_POINTS-1)
      json += ",";
  }
  json += "],";

  // Air Quality
  json += "\"air\":[";
  for(int i=0;i<MAX_POINTS;i++)
  {
    json += String(airData[i]);
    if(i<MAX_POINTS-1) json += ",";
  }
  json += "],";

  // Temperature
  json += "\"temp\":[";
  for(int i=0;i<MAX_POINTS;i++)
  {
    json += String(tempData[i]);
    if(i<MAX_POINTS-1) json += ",";
  }
  json += "],";

  // Humidity
  json += "\"hum\":[";
  for(int i=0;i<MAX_POINTS;i++)
  {
    json += String(humData[i]);
    if(i<MAX_POINTS-1) json += ",";
  }
  json += "],";

  // Moisture
  json += "\"moist\":[";
  for(int i=0;i<MAX_POINTS;i++)
  {
    json += String(moistureData[i]);
    if(i<MAX_POINTS-1) json += ",";
  }
  json += "],";

  // Vibration
  json += "\"vib\":[";
  for(int i=0;i<MAX_POINTS;i++)
  {
    json += String(vibrationData[i]);
    if(i<MAX_POINTS-1) json += ",";
  }
  json += "],";

  // Touch
  json += "\"touch\":[";
  for(int i=0;i<MAX_POINTS;i++)
  {
    json += String(touchData[i]);
    if(i<MAX_POINTS-1) json += ",";
  }
  json += "]";

  json += "}";

  server.send(200,"application/json",json);
});
  server.begin();
}

// ================= LOOP =================
void loop()
{
  server.handleClient();

  airValue = analogRead(MQ135_PIN);
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  moistureState = digitalRead(MOISTURE_SENSOR);
  touchState = digitalRead(TOUCH_SENSOR);
  vibration = digitalRead(VIBRATION_SENSOR);
airData[indexPos] = airValue;
tempData[indexPos] = temperature;
humData[indexPos] = humidity;
moistureData[indexPos] = moistureState;
vibrationData[indexPos] = vibration;
touchData[indexPos] = touchState;
indexPos++;

if(indexPos >= MAX_POINTS)
{
  indexPos = 0;
}
  // BUZZER LOGIC
  if(touchState==HIGH || vibration==HIGH)
  {
    digitalWrite(BUZZER,HIGH);
  }
  else
  {
    digitalWrite(BUZZER,LOW);
  }

  delay(1000);
}
