#include <Preferences.h>

Preferences preferences;
#include <Arduino.h>
#include "SPI.h"
#include "ADE7758.h"
#include "Web_upload.h"
#include "error404.h"
#include "error405.h"
#include "Daytime_Entry.h"
#include "Month_cycle.h"
#include "logs.h"

//const char* ssid     = "TP-Link_59C6";
//const char* password = "36178940";


const char* ssid     = "Tucker Jio 4G";
const char* password = "Tucker#123";

void IRAM_ATTR handleInterrupt() {
  i++;
  Intf = 1;
  detachInterrupt(digitalPinToInterrupt(interruptPin));
}

void IRAM_ATTR onTimer() {
  Unit[0] += ( W  [0][0] / 3600000);
  Unit[1] += ( W  [0][1] / 3600000);
  Unit[2] += ( W  [0][2] / 3600000);
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);


  pinMode(OnOffRelayPin, OUTPUT);
  digitalWrite(OnOffRelayPin, LOW);
  pinMode(SwitchOverRelayPin, OUTPUT);
  digitalWrite(SwitchOverRelayPin, LOW);
  pinMode(EmergencyPin, INPUT);
  pinMode(LightPin, OUTPUT);
  digitalWrite(LightPin, LOW);

  preferences.begin("myApp", false);
  //  preferences.remove("TotalUnit");
  EUnit = preferences.getFloat("EUnit", 0);
 MaxPower = preferences.getFloat("MaxPower", 0);
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  energy_setup();
  LittleFS_setup();
  initWebSocket();
  log_setup();


  timer = timerBegin(0, 80, true); // Timer 0, prescaler of 80 (1 MHz clock)
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true); // 1,000,000 microseconds = 1 second
  timerAlarmEnable(timer);


  pinMode(interruptPin, INPUT_PULLDOWN);  // Use pull-down resistor
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  // Route for root / web page






  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/rvoltage", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", RVoltage().c_str());
  });
  server.on("/yvoltage", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", YVoltage().c_str());
  });
  server.on("/bvoltage", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BVoltage().c_str());
  });
  server.on("/rcurrent", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", RCurrent().c_str());
  });
  server.on("/ycurrent", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", YCurrent().c_str());
  });
  server.on("/bcurrent", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BCurrent().c_str());
  });
  server.on("/rwatt", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", RWatt().c_str());
  });
  server.on("/ywatt", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", YWatt().c_str());
  });
  server.on("/bwatt", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BWatt().c_str());
  });
  server.on("/runit", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", RUnit().c_str());
  });
  server.on("/yunit", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", YUnit().c_str());
  });
  server.on("/bunit", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BUnit().c_str());
  });
  server.on("/tunit", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", TUnit().c_str());
  });
  server.on("/error", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", Error().c_str());
  });
  server.on("/notification", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", Notification().c_str());
  });
  server.on("/date", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", CurrentDate);
  });
  server.on("/timq", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", CurrentTime);
  });
  server.on("/download", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/data.txt", String(), true);
  });

  // Handle the View Data button
  server.on("/view-data", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/data.txt", "text/plain", false);
  });

  // Handle the delete button
  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest * request) {
    deleteFile(LittleFS, dataPath);
    request->send(200, "text/plain", "data.txt was deleted.");
  });



  server.on("/daytime", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    if (request->hasParam(DT_START)) {
      inputMessage = request->getParam(DT_START)->value();
      writeFile(LittleFS, "/dtStart.txt", inputMessage.c_str());
    }
    // GET dtStop value on <ESP_IP>/get?dtStop=<inputMessage>
    if (request->hasParam(DT_STOP)) {
      inputMessage = request->getParam(DT_STOP)->value();
      writeFile(LittleFS, "/dtStop.txt", inputMessage.c_str());
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    //    request->send(200, "text/text", inputMessage);
  });


  server.on("/monthcycle", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    if (request->hasParam(START_DATE)) {
      inputMessage = request->getParam(START_DATE)->value();
      writeFile(LittleFS, "/startDate.txt", inputMessage.c_str());
    }
    // GET dtStop value on <ESP_IP>/get?dtStop=<inputMessage>
    if (request->hasParam(END_DATE)) {
      inputMessage = request->getParam(END_DATE)->value();
      writeFile(LittleFS, "/endDate.txt", inputMessage.c_str());
    }
    if (request->hasParam(MONTH_GAP)) {
      inputMessage = request->getParam(MONTH_GAP)->value();
      writeFile(LittleFS, "/monthGap.txt", inputMessage.c_str());
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    //    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound([](AsyncWebServerRequest * request) {
    if (request->method() == HTTP_GET) {
      // Handle 404 Not Found error
      Serial.println("Web Server: Not Found");
      String html = HTML_CONTENT_404;  // Use the HTML content from the error_404.h file
      request->send(404, "text/html", html);
    } else {
      // Handle 405 Method Not Allowed error
      Serial.println("Web Server: Method Not Allowed");
      String html = HTML_CONTENT_405;  // Use the HTML content from the error_405.h file
      request->send(405, "text/html", html);
    }
  });
  server.on("/daytime.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    String html = HTML_CONTENT_DAYLIGHT;  // Use the HTML content from the temperature.h file

    html.replace("%dtStart%", processor("dtStart"));
    html.replace("%dtStop%", processor("dtStop"));// update the temperature value
    request->send(200, "text/html", html);
    //    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/monthcycle.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    String html = HTML_CONTENT_MONTH;  // Use the HTML content from the temperature.h file

    html.replace("%startDate%", processor("startDate"));
    html.replace("%endDate%", processor("endDate"));// update the temperature value
    request->send(200, "text/html", html);
    //    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/logs.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    String html = HTML_CONTENT_LOGS;  // Use the HTML content from the temperature.h file
    request->send(200, "text/html", html);
    //    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  //  LedState = RelayState;
  LittleFS_loop();
  Mn_loop();
  digitalWrite(OnOffRelayPin, RelayState);

  //  int analogValue = analogRead(LIGHT_SENSOR_PIN);
  int analogValue = 67;
  Serial.print("Analog Value = ");
  Serial.println(analogValue);


  //  if (analogValue < 40) {
  //    Serial.println(" => Dark");
  //  } else if (analogValue < 2000) {
  //    Serial.println(" => Dim");
  //  } else
  if (analogValue < 500) {
    if ((currenthour < StartHour || (currenthour == StartHour && currentminute <= StartMinute)) ||
        (currenthour > StopHour || (currenthour == StopHour && currentminute >= StopMinute))) {
      // Code to execute if the current time is before the start time or after the stop time
      Light = 1;
      Relay2State = 0;
      Serial.print("Light1");

    }
    else
    {
      Light = 0;
      Serial.print("Light0");

    }
  }
  else
  {
    if (!((currenthour < StartHour || (currenthour == StartHour && currentminute <= StartMinute)) ||
          (currenthour > StopHour || (currenthour == StopHour && currentminute >= StopMinute))))
    {
      Serial.print("Light0.o");

      Light = 0;
    }
  }
  if (Light == 1)
    digitalWrite(LightPin, HIGH);
  else
    digitalWrite(LightPin, Relay2State);
  notifyClients();
  printLocalTime();

  meter.setSPI();


  //  Serial.println(meter.getVpeak());
  //  Serial.println(meter.getIpeak());
  var1 = meter.avrms();
  var4 = meter.airms();
  var2 = meter.bvrms();
  var5 = meter.birms();
  var3  = meter.cvrms();
  var6  = meter.cirms();
  freq = meter.getFreq();
  Serial.println("meter.getStatus()");
  Serial.println(meter.getStatus());
  intrstat = meter.getStatus();
  Serial.println(intrstat);




  if (Intf == true) {
    er = 0;
    Serial.println("Interrupt triggered!");
    Serial.println(i);
    if (intrstat & 0x001C0) {
      Serial.println("Loss Power");
      if (eprom == 1) {
        preferences.begin("myApp", false);
        preferences.putFloat("EUnit", TotalUnit);
        preferences.end();
        Serial.println("Eprom");
        eprom = 0;
      }
    }
    //    else
    //      eprom = 1;
    //

    if ((intrstat & 0x001C0) == 0x001C0) {
      Err = "Power Loss";
      ac++;

    }
    else if (intrstat & 0x08000) {
      Err = "Over Voltage";
    }
    else if (intrstat & 0x00008) {
      Err = "R Phase Sag";
    }
    else if (intrstat & 0x00010) {
      Err = "Y Phase Sag";
    }
    else if (intrstat & 0x00020) {
      Err = "B Phase Sag";
    }
    else if (intrstat & 0x00040) {
      Err = "Missing R Phase";
    }
    else if (intrstat & 0x00080) {
      Err = "Missing Y Phase";
    }
    else if (intrstat & 0x000100) {
      Err = "Missing B Phase";
    }
    else if (intrstat & 0x20000) {
      Err = "Sign Change in Watt";
    }
    else if (intrstat & 0x80000) {
      Err = "Phase Sequence Error";
    }
    else
    {
      ac = 0;
      Not = "0";
      digitalWrite(SwitchOverRelayPin, LOW);
    }

    Intf = 0;
    logs();
    attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  }
  if (!(intrstat & 0x001C0))
  { //  else
    eprom = 1;
    ac = 0;
  }
  Serial.println(Err);
  Serial.println(Not);
  er++;
  if (er >= 3)
  {
    Err = "0";
    er = 60;
  }
  if (ac > 1) {
    ac = 50;
    Not = "Auto ChangeOver ON";
    digitalWrite(SwitchOverRelayPin, HIGH);
    logs();
  }
  else {
    digitalWrite(SwitchOverRelayPin, LOW);
    Not = "0";
  }
  if (digitalRead(EmergencyPin)) {
    Not = "Emergency Stop";
    logs();
  }
  Voltage[0][0] = ((var1) * (res1));
  if (Voltage[0][0] > 0 && Voltage[0][0] < 5)Voltage[0][0] = 0;
  Voltage[0][1] = ((var2) * (res2));
  if (Voltage[0][1] > 0 && Voltage[0][1] < 6)Voltage[0][1] = 0;
  Voltage[0][2] = ((var3) * (res3));
  if (Voltage[0][2] > 0 && Voltage[0][2] < 5)Voltage[0][2] = 0;

  if (var4 != 0)
  {
    Current[0][0] = ((var4) * (res4)) * EXTERNAL_CT_RATIO;
    W  [0][0] = (var7 * res7 ) * EXTERNAL_CT_RATIO;


    if (Current[0][0] < 0)Current[0][0] = 0;
  }
  else {
    Current[0][0] = 0;
    W  [0][0] = 0;
  }
  if (var5 != 0)
  {
    Current[0][1] = ((var5) * (res5)) * EXTERNAL_CT_RATIO;
    if (Current[0][1] < 0)Current[0][1] = 0;
    W  [0][1] = (var8 * res8 ) * EXTERNAL_CT_RATIO;

  }
  else
  {
    Current[0][1] = 0;
    W  [0][1] = 0;
  }
  if (var6 != 0)
  {
    Current[0][2] = ((var6) * (res6)) * EXTERNAL_CT_RATIO;
    if (Current[0][2] < 0)Current[0][2] = 0;
    W  [0][2] = (var9 * res9 ) * EXTERNAL_CT_RATIO;
  }
  else
  {
    Current[0][2] = 0;
    W  [0][2] = 0;
  }
  Frequency = freq * 0.0625;   //1/(freq*0.0000096);
  if (Frequency > 60)
  {
    Frequency = 0;
  }
  if (meter.setPotLine(PHASE_A, 100)) {
    var7 = meter.getWatt(PHASE_A);
    //        Var_a = meter.getVar(PHASE_A);
    var10 = meter.getVa(PHASE_A);
  } else {
    Serial.println("There is no crossing by zero in PhaseA");
  }
  if (meter.setPotLine(PHASE_B, 100)) {
    var8 = meter.getWatt(PHASE_B);
    //        Var_b = meter.getVar(PHASE_B);
    var11 = meter.getVa(PHASE_B);
  } else {
    Serial.println("There is no crossing by zero in PhaseB");
  }
  if (meter.setPotLine(PHASE_C, 100)) {
    var9 = meter.getWatt(PHASE_C);
    //        Var_c = meter.getVar(PHASE_C);
    var12 = meter.getVa(PHASE_C);
  } else {
    Serial.println("There is no crossing by zero in PhaseC");
  }

  //  W  [0][0] = (var7 * res7 ) * EXTERNAL_CT_RATIO;
  //
  //  W  [0][1] = (var8 * res8 ) * EXTERNAL_CT_RATIO;
  //
  //  W  [0][2] = (var9 * res9 ) * EXTERNAL_CT_RATIO;


  VAA = (var10 * res10) * EXTERNAL_CT_RATIO;
  VAB = (var11 * res11) * EXTERNAL_CT_RATIO;
  VAC = (var12 * res12) * EXTERNAL_CT_RATIO;
  //  ]
  pf1 = var7 / var10;
  if (isnan(pf1))pf1 = 0;
  pf2 = var8 / var11;
  if (isnan(pf2))pf2 = 0;
  pf3 = var9 / var12;
  if (isnan(pf3))pf3 = 0;


  meter.closeSPI();




  delay(500);
  Serial.print("phase R:");
  Serial.print("\t"); Serial.print("V:");  Serial.println( var1, 2 );
  Serial.print("\t\t"); Serial.print("I:");  Serial.println( var4, 2 );
  Serial.print("\t\t"); Serial.print("WATTHR Register Value:");  Serial.println( var7, 2 );
  Serial.print("\t\t"); Serial.print("VARHR Register Value:");  Serial.println( Var_a, 2 );
  Serial.print("\t\t"); Serial.print("VAHR Register Value:");  Serial.println( var10, 2 );
  //
  Serial.print("phase Y:");
  Serial.print("\t"); Serial.print("V:");  Serial.println( var2, 2 );
  Serial.print("\t\t"); Serial.print("I:");  Serial.println( var5, 2 );
  Serial.print("\t\t"); Serial.print("WATTHR Register Value:");  Serial.println( var8, 2 );
  Serial.print("\t\t"); Serial.print("VARHR Register Value:");  Serial.println( Var_b, 2 );
  Serial.print("\t\t"); Serial.print("VAHR Register Value:");  Serial.println( var11, 2 );

  TotalUnit = Unit[0] + Unit[1] + Unit[2] + EUnit;
  TotalPower = W[0][0] + W[0][1] + W[0][2];
  if (TotalPower > MaxPower)
   {
    MaxPower = TotalPower;
        preferences.begin("myApp", false);
        preferences.putFloat("MaxPower", MaxPower);
        preferences.end();
   }
  else
    MaxPower = MaxPower;
  Serial.print("phase B:");
  Serial.print("\t"); Serial.print("V:");  Serial.println( var3, 2 );
  Serial.print("\t\t"); Serial.print("I:");  Serial.println( var6, 2 );
  Serial.print("\t\t"); Serial.print("WATTHR Register Value:");  Serial.println( var9, 2 );
  Serial.print("\t\t"); Serial.print("VARHR Register Value:");  Serial.println( Var_c, 2 );
  Serial.print("\t\t"); Serial.print("VAHR Register Value:");  Serial.println( var12, 2 );
  //  sprintf(VoltageR, "%.2f", Voltage[0][0]);
  //  Serial.print(VoltageR);
  //  Serial.println("-----------------------------");
  Serial.print("\t\t"); Serial.print("Frequency: "); Serial.println(Frequency, 2);
  Serial.println("---------------------PHASE R---------------------");
  Serial.print("\t\t"); Serial.print("V:");  Serial.println( Voltage[0][0], 2 );
  Serial.print("\t\t"); Serial.print("I:");  Serial.println( Current[0][0], 2 );
  Serial.print("\t\t"); Serial.print("W:");  Serial.println( W[0][0], 2 );
  //    Serial.print("\t\t");Serial.print("VAR:");  Serial.println( VARA,2 );
  Serial.print("\t\t"); Serial.print("VA:");  Serial.println( VAA, 2 );
  //  Serial.print("\t\t"); Serial.print("W HR:");  Serial.println( AWattHrSum, 2 );
  //  //  Serial.print("\t\t");Serial.print("VAR HR:");  Serial.println( AVARHrSum,2 );
  //  Serial.print("\t\t"); Serial.print("VA HR:");  Serial.println( AVAHrSum, 2 );
  Serial.print("\t\t"); Serial.print("Unit:");  Serial.println( Unit[0], 4 );
  Serial.print("\t\t"); Serial.print("pf:");  Serial.println( pf1, 3 );

  Serial.println("---------------------PHASE Y---------------------");
  Serial.print("\t\t"); Serial.print("V:");  Serial.println( Voltage[0][1], 2 );
  Serial.print("\t\t"); Serial.print("I:");  Serial.println( Current[0][1], 2 );
  Serial.print("\t\t"); Serial.print("W:");  Serial.println( W[0][1], 2 );
  //  Serial.print("\t\t");Serial.print("VAR:");  Serial.println( VARB,2 );
  Serial.print("\t\t"); Serial.print("VA:");  Serial.println( VAB, 2 );
  //  Serial.print("\t\t"); Serial.print("W HR:");  Serial.println( BWattHrSum, 2 );
  //  //  Serial.print("\t\t");Serial.print("VAR HR:");  Serial.println( BVARHrSum,2 );
  //  Serial.print("\t\t"); Serial.print("VA HR:");  Serial.println( BVAHrSum, 2 );
  Serial.print("\t\t"); Serial.print("Unit:");  Serial.println( Unit[1], 4 );
  Serial.print("\t\t"); Serial.print("pf :");  Serial.println( pf2, 3 );

  Serial.println("---------------------PHASE B---------------------");
  Serial.print("\t\t"); Serial.print("V:");  Serial.println( Voltage[0][2], 2 );
  Serial.print("\t\t"); Serial.print("I:");  Serial.println( Current[0][2], 2 );
  Serial.print("\t\t"); Serial.print("W:");  Serial.println( W[0][2], 2 );
  //  Serial.print("\t\t");Serial.print("VAR:");  Serial.println( VARC,2 );
  Serial.print("\t\t"); Serial.print("VA:");  Serial.println(VAC, 2 );
  //  Serial.print("\t\t"); Serial.print("W HR:");  Serial.println( CWattHrSum, 2 );
  //  //  Serial.print("\t\t");Serial.print("VAR HR:");  Serial.println( CVARHrSum,2 );
  //  Serial.print("\t\t"); Serial.print("VA HR:");  Serial.println( CVAHrSum, 2 );
  Serial.print("\t\t"); Serial.print("Unit:");  Serial.println(Unit[2], 4 );
  Serial.print("\t\t"); Serial.print("pf :");  Serial.println(pf3 , 3 );

  Serial.print("Total Unit :");  Serial.println(TotalUnit , 4 );
  Serial.print("currenthour");  Serial.println(currenthour );
  Serial.print("currentyear");  Serial.println(currentyear );
}

void printLocalTime() {
  struct tm *timeinfo;
  epochTime = getTime();
  //Finally, print the epoch time every second:
  epochTime += 19800;
  timeinfo = localtime(&epochTime);
  currentyear = timeinfo->tm_year + 1900;
  if (!(currentyear == 1970)) {

    date2.day1 = timeinfo->tm_mday;
    date2.month1 = timeinfo->tm_mon+1;
    date2.year1 = currentyear;
    currenthour = timeinfo->tm_hour;
    currentminute = timeinfo->tm_min;
    snprintf(CurrentTimetoDisplay, sizeof(CurrentTimetoDisplay), "%04d-%02d-%02d %02d:%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
    //      CurrentTimetoDisplay=String(timeinfo->tm_year + 1900)+"-"+String(timeinfo-> tm_mon + 1)+"-"+String(timeinfo->tm_mday)+" "+String(timeinfo->tm_hour)+":"+String(timeinfo-> tm_min);
    snprintf(CurrentDate, sizeof(CurrentDate), "%02d-%02d-%04d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
    snprintf(CurrentTime, sizeof(CurrentTime), "%02d:%02d",  timeinfo->tm_hour, timeinfo->tm_min);
  }
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return (0);
  }
  time(&now);
  return now;
}


String processor(const String& var) {
  Serial.println(var);
  if (var == "STATE") {
    if (RelayState) {
      return "ON";
    }
    else {
      return "OFF";
    }
  }
  else   if (var == "lSTATE") {
    if (digitalRead(LightPin)) {
      return "ON";
    }
    else {
      return "OFF";
    }
  }
  else if (var == "RVOLTAGE") {
    return RVoltage();
  }
  else if (var == "YVOLTAGE") {
    return YVoltage();
  }
  else if (var == "BVOLTAGE") {
    return BVoltage();
  }
  else if (var == "RCURRENT") {
    return RCurrent();
  }
  else if (var == "YCURRENT") {
    return YCurrent();
  }
  else if (var == "BCURRENT") {
    return BCurrent();
  }

  else if (var == "RWATT") {
    return RWatt();
  }
  else if (var == "YWATT") {
    return YWatt();
  }
  else if (var == "BWATT") {
    return BWatt();
  }
  else if (var == "RUNIT") {
    return RUnit();
  }
  else if (var == "YUNIT") {
    return YUnit();
  }
  else if (var == "BUNIT") {
    return BUnit();
  }
  else if (var == "TUNIT") {
    return TUnit();
  }
  else if (var == "TIME") {
    return (CurrentTime);
  }
  else if (var == "DATE") {
    return (CurrentDate);
  }
  else if (var == "ERROR") {
    return Error();
  }
  else if (var == "NOTIFICATION") {
    return Notification();
  }
  else if (var == "dtStart") {
    return readFile(LittleFS, "/dtStart.txt");
  }
  else if (var == "dtStop") {
    return readFile(LittleFS, "/dtStop.txt");
  }
  else if (var == "startDate") {
    return readFile(LittleFS, "/startDate.txt");
  }
  else if (var == "endDate") {
    return readFile(LittleFS, "/endDate.txt");
  }
  return String();
}
