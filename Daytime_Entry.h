/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <LittleFS.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* DT_START = "dtStart";
const char* DT_STOP = "dtStop";



// HTML web page to handle 3 input fields (dtStart, dtStop, inputFloat)
const char *HTML_CONTENT_DAYLIGHT PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      setTimeout(function(){ document.location.reload(false); }, 2500);   
    }
  </script>
</head>
<body>
  <form action="/daytime" target="hidden-form" onsubmit="submitMessage()">
    <div>
      Daytime Start at(current value "%dtStart%"): 
      <label for="dtStart">Select a time between 04.00 to 07.30:</label>
      <input type="time" id="dtStart" name="dtStart" min="04:00" max="07:30" required>
    </div>
    <br>
    <div>
      Daytime Stop at(current value "%dtStop%"): 
      <label for="dtStop">Select a time between 16.00 to 19.30:</label>
      <input type="time" id="dtStop" name="dtStop" min="16:00" max="19:30" required>
    </div>
    <br>
    <input type="submit" value="Submit">
  </form>
  <br>
  <iframe style="display:none" name="hidden-form"></iframe>
  <p>Go back to the <a href="/">homepage</a>.</p>
</body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void StartTime(String input) {
  int colonIndex = input.indexOf(":");
  if (colonIndex > 0) {
    String hourStr = input.substring(0, colonIndex);
    String minuteStr = input.substring(colonIndex + 1);


    StartHour = hourStr.toInt();
    StartMinute = minuteStr.toInt();

    Serial.print("Limit time set to: ");
    Serial.print(StartHour);
    Serial.print(":");
    Serial.println(StartMinute);
  }
}

void StopTime(String input) {
  int colonIndex = input.indexOf(":");
  if (colonIndex > 0) {
    String hourStr = input.substring(0, colonIndex);
    String minuteStr = input.substring(colonIndex + 1);

    StopHour = hourStr.toInt();
    StopMinute = minuteStr.toInt();

    Serial.print("Limit time set to: ");
    Serial.print(StopHour);
    Serial.print(":");
    Serial.println(StopMinute);
  }
}


String readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while (file.available()) {
    fileContent += String((char)file.read());
  }
  file.close();
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message) {

  File file = fs.open(path, "w");
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

// Replaces placeholder with stored values
//String processor(const String& var) {
//  //Serial.println(var);
//  if (var == "inputString") {
//    return readFile(LittleFS, "/inputString.txt");
//  }
//  else if (var == "dtStop") {
//    return readFile(LittleFS, "/dtStop.txt");
//  }
//  return String();
//}

void LittleFS_setup() {
  if (!LittleFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  Serial.println("LittleFS Mounted");
}

void LittleFS_loop() {
  // To access your stored values on inputString, dtStop, inputFloat
  String yourInputString = readFile(LittleFS, "/dtStart.txt");
  StartTime(yourInputString);
  Serial.print("*** Your inputString: ");
  Serial.println(yourInputString);

  String yourInputInt = readFile(LittleFS, "/dtStop.txt");
  StopTime(yourInputInt);
  Serial.print("*** Your dtStop: ");
  Serial.println(yourInputInt);
}
