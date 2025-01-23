const char* dataPath = "/data.txt";

unsigned long lastTime = 0;
unsigned long timerDelay = 3000;
float temp, hum, pres;
char dataMessage[64];

#define MAX_ENTRIES 10  // Max entries in the circular buffer
char lines[MAX_ENTRIES][64];  // Fixed-size buffer for storing lines (64 characters max per line)
int lineCount = 0;  // Count of lines added to the buffer
int bufferIndex = 0;
const char*  SErr = "0";

const char *HTML_CONTENT_LOGS PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Datalogger</title>
    <style>
        html {
            font-family: Arial, Helvetica, sans-serif;
        }

        body {
            background-color: #f4f4f4;
            margin: 0;
            padding: 0;
        }

        .container {
            max-width: 800px;
            margin: 50px auto;
            text-align: center;
        }

        h1 {
            color: #333;
        }

        .button {
            display: inline-block;
            padding: 10px 20px;
            margin: 10px;
            font-size: 16px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition-duration: 0.4s;    }

        .button-data {
            background-color: #858585;
            color: #fff;
        }

        .button-delete {
            background-color: #784703;
            color: #fff;
        }

        .button:hover {
            background-color: #0056b3;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 Datalogger - Manage Data</h1>
        <a href="view-data"><button class="button button-data">View Data</button></a>
        <a href="download"><button class="button button-data">Download Data</button></a>
        <a href="delete"><button class="button button-delete">Delete All Data</button></a>
    </div>
   <p>Go back to the <a href="/">homepage</a>.</p>
</body></html>)rawliteral";


void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}

void writeLog(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void log_setup()
{
  File file = LittleFS.open(dataPath, "r");
  if (!file) {
    // File doesn't exist, create it and add headers
    writeFile(LittleFS, dataPath, "Epoch Time, Temperature, Humidity, Pressure \r\n");
  } else {
    Serial.println("File already exists.");
  }
  while (file.available() && lineCount < MAX_ENTRIES) {
    int i = 0;
    char ch;
    // Read each character until newline or max buffer size is reached
    while (file.available() && i < sizeof(lines[lineCount]) - 1) {
      ch = file.read();
      if (ch == '\n') {
        break;  // Stop reading when a newline is encountered
      }
      lines[lineCount][i++] = ch;  // Store the character
    }
    lines[lineCount][i] = '\0';  // Null-terminate the string
    lineCount++;
  }
  file.close();
}


void saveDataToBuffer(const char* dataMessage) {
  // If the buffer is full, overwrite the oldest entry
  if (lineCount == MAX_ENTRIES) {
    // Directly overwrite the oldest line (no need to shift)
    strncpy(lines[bufferIndex], dataMessage, sizeof(lines[bufferIndex]) - 1);
    lines[bufferIndex][sizeof(lines[bufferIndex]) - 1] = '\0';  // Ensure null-termination
  } else {
    // If buffer is not full, just add the new line
    strncpy(lines[lineCount], dataMessage, sizeof(lines[lineCount]) - 1);
    lines[lineCount][sizeof(lines[lineCount]) - 1] = '\0';  // Ensure null-termination
    lineCount++;
  }

  // Update the circular buffer index
  bufferIndex = (bufferIndex + 1) % MAX_ENTRIES;
}

// Function to save buffer data to a file
void saveBufferToFile() {
  File file = LittleFS.open(dataPath, "w");  // Open file in write mode, overwriting the file

  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  // Write the data buffer to the file
  for (int i = 0; i < MAX_ENTRIES; i++) {
    if (strlen(lines[i]) > 0) {  // Write only non-empty entries
      file.println(lines[i]);
    }
  }
  file.close();
  Serial.println("Buffer saved to file.");
}

void logs() {



  //      printLocalTime();
  // Concatenate all info separated by commas
  snprintf(dataMessage, sizeof(dataMessage), "%s -> %s, %s",
           CurrentTimetoDisplay, Err,Not);
  Serial.print("Saving data: ");
  saveDataToBuffer(dataMessage);

  // Save the buffer to the file at regular intervals (e.g., every 10 minutes)
  //    if (millis() - lastSaveTime > saveDelay) {
  saveBufferToFile();

  // Append the data to the file
  //    appendFile(LittleFS, "/data.txt", dataMessage.c_str());

}
