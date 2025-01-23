/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

struct Date {
  int day1;
  int month1;
  int year1;
};
Date date1;
Date date2;

uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int newMonth;
int newYear ;
int newDay;

const char* START_DATE = "startDate";
const char* END_DATE = "endDate";
const char* MONTH_GAP = "monthGap";
//int sday, smonth, syear;
// HTML web page to handle 3 input fields (startDate, endDate, inputFloat)
const char *HTML_CONTENT_MONTH PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
 <title>Start and End Date Selection (1 or 2 Month Gap) with Validation</title>
     <meta name="viewport" content="width=device-width, initial-scale=1">
     <script>
      function submitMessage() {
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
        // Function to update the end date based on the selected gap (1 or 2 months)
        function updateDateRanges() {
            var startDateInput = document.getElementById("startDate");
            var endDateInput = document.getElementById("endDate");
            var selectedStartDate = new Date(startDateInput.value);
            var gap = document.getElementById("monthGap").value;  // Get selected month gap (1 or 2)

            // If gap is not selected, return early
            if (!gap) return;

            // Calculate the end date based on the selected gap
            var endDate = new Date(selectedStartDate);
            endDate.setMonth(selectedStartDate.getMonth() + parseInt(gap)); // Add 1 or 2 months

            // Set the min and max date for end date input based on the selected start date and gap
            endDateInput.min = formatDate(selectedStartDate);  // End date cannot be earlier than start date
            endDateInput.max = formatDate(endDate);  // End date cannot be later than the calculated gap

            // Set the end date to be either 1 or 2 months after the start date based on the selected gap
            endDateInput.value = formatDate(endDate);
        }

        // Function to format a date in YYYY-MM-DD format
        function formatDate(date) {
            var year = date.getFullYear();
            var month = ('0' + (date.getMonth() + 1)).slice(-2); // Add leading zero if necessary
            var day = ('0' + date.getDate()).slice(-2); // Add leading zero if necessary
            return year + '-' + month + '-' + day;
        }

        // Function to validate the form before submitting
        function validateForm(event) {
            var startDateInput = document.getElementById("startDate");
            var endDateInput = document.getElementById("endDate");
            var monthGapInput = document.getElementById("monthGap");

            var selectedStartDate = new Date(startDateInput.value);
            var selectedEndDate = new Date(endDateInput.value);
            var gap = monthGapInput.value;

            // Check if month gap is selected
            if (!gap) {
                alert("Please select a month gap (1 or 2 months).");
                event.preventDefault(); // Prevent form submission
                return false;
            }

            // Check if start date and end date are selected
            if (!startDateInput.value) {
                alert("Please select a start date.");
                event.preventDefault(); // Prevent form submission
                return false;
            }

            if (!endDateInput.value) {
                alert("Please select an end date.");
                event.preventDefault(); // Prevent form submission
                return false;
            }

            // Check if end date is exactly 1 or 2 months after start date, based on gap
            var calculatedEndDate = new Date(selectedStartDate);
            calculatedEndDate.setMonth(selectedStartDate.getMonth() + parseInt(gap));

            if (selectedEndDate.getTime() !== calculatedEndDate.getTime()) {
                alert("End date must be exactly " + gap + " month(s) after the start date.");
                event.preventDefault(); // Prevent form submission
                return false;
            }


 submitMessage(); // Manually call the submitMessage() function to update LittleFS
    return true; 
        }
    </script></head><body>
     <h2>Select Start and End Date with Gap Option</h2>
    <form action="/monthcycle" target="hidden-form" onsubmit="return validateForm(event)">


    Month Cycle Starts from "%startDate%" to "%endDate%"
    <br><br>
    To Change the Date           
   <label for="monthGap">Select Month Gap:</label>
        <select id="monthGap" name="monthGap" onchange="updateDateRanges()" required>
            <option value="">--Select Month Gap--</option>
            <option value="1">1 Month</option>
            <option value="2">2 Months</option>
        </select>
      
        <br><br>
        
    
  <label for="startDate">Start Date:</label>
  <input type="date" id="startDate" name="startDate" onchange="updateDateRanges()" required>

   <br><br>
  
     

  <label for="endDate">End Date::</label>
  <input type="date" id="endDate" name="endDate"  required>

  <br><br>

       <input type="submit" value="Submit">
    </form>


   <iframe style="display:none" name="hidden-form"></iframe>
     <p>Go back to the <a href="/">homepage</a>.</p>
</body></html>)rawliteral";

bool isLeapYear(int year) {
  // A year is a leap year if divisible by 4 but not by 100, unless divisible by 400
  if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
    return true;
  }
  return false;
}
void datechange(){
   if (newMonth == 2) {
    // Adjust February days if it's a leap year
    if (isLeapYear(newYear)) {
      daysInMonth[1] = 29;  // Leap year adjustment
    } else {
      daysInMonth[1] = 28;  // Regular year
    }
  }
  if (newDay > daysInMonth[newMonth - 1]) {
    newDay = daysInMonth[newMonth - 1];  // Set to the last valid day of the month
  }
    if (newDay > daysInMonth[newMonth  - 1]) {
   newDay = 1;  // Reset day to 1
    newMonth++;    // Increment month
    }
  if (newMonth > 12) {
    newMonth = 1;  // Reset to January if we go past December
    newYear++;
  }

 
}

void Mn_loop() {
  // To access your stored values on startDate, endDate, inputFloat
  String yourstartDate = readFile(LittleFS, "/startDate.txt");
  Serial.print("*** Your startDate: ");
  Serial.println(yourstartDate);
 String yourendDate = readFile(LittleFS, "/endDate.txt");
  Serial.print("*** Your endDate: ");
  Serial.println(yourendDate);
  int firstSlashIndex = yourendDate.indexOf("-");  // Find the first slash
  int secondSlashIndex = yourendDate.lastIndexOf("-");  // Find the second slash

  if (firstSlashIndex > 0 && secondSlashIndex > firstSlashIndex) {
    // Extract day, month, and year substrings
    String yearStr = yourendDate.substring(0, firstSlashIndex);
    String monthStr = yourendDate.substring(firstSlashIndex + 1, secondSlashIndex);
    String dayStr = yourendDate.substring(secondSlashIndex + 1);
  date1.day1 = dayStr.toInt();
  date1.month1 = monthStr.toInt();
  date1.year1 = yearStr.toInt();
  }

 newMonth =date1.month1;
newYear=date1.year1 ;
 newDay = date1.day1 ;
  // Print the extracted values
  Serial.print("Day: ");
  Serial.println(date1.day1);

  Serial.print("Month: ");
  Serial.println( date1.month1);

  Serial.print("Year: ");
  Serial.println(date1.year1);
 

 int yourmonthGap = readFile(LittleFS, "/monthGap.txt").toInt();
  Serial.print("*** Your monthGap: ");
  Serial.println(yourmonthGap);


  // Add one month to the current month
//  newDay++;

  char newDate[15];
  char CDate[15];
  snprintf(newDate, sizeof(newDate), "%04d-%02d-%02d", date2.year1, date2.month1, date2.day1);
  snprintf(CDate, sizeof(CDate), "%04d-%02d-%02d", date1.year1, date1.month1, date1.day1);

  Serial.println(newDate);
   Serial.println(CDate);
  int result = (date1.year1 != date2.year1) ? (date1.year1 - date2.year1) :
               (date1.month1 != date2.month1) ? (date1.month1 - date2.month1) :
               (date1.day1 - date2.day1);

  // Output the comparison result
  if (result < 0) {
  changeDate=1;
    Serial.println("Date1 is earlier than Date2");
  } else if (result > 0) {
     changeDate=0;
    Serial.println("Date1 is later than Date2");
  } else {
    Serial.println("Both dates are the same");
  }

  // Ensure the new day does not exceed the number of days in the new month
  if( changeDate==1){
//     newDay++;
  datechange();
//    String new1Date=String(newYear)+"-"+String(newMonth)+"-"+String(newDay);
char newstDate[15];
char newenDate[15];
  snprintf(newstDate, sizeof(newstDate), "%04d-%02d-%02d", newYear, newMonth, newDay);
//     Print the new date (1 month later)
writeFile(LittleFS, "/startDate.txt",newstDate);
//Serial.println();
    Serial.print("Date 1 month later: ");
    Serial.print(newYear);
    Serial.print('-');
    Serial.print(newMonth);
    Serial.print('-');
    Serial.print(newDay);
    Serial.println();
newMonth+=yourmonthGap;
  if (newMonth > 12) {
   newMonth -= 12;  // Reset to January if we go past December
    newYear++;
  }
datechange();
   Serial.print("Date 1 month later: ");
    Serial.print(newYear);
    Serial.print('-');
    Serial.print(newMonth);
    Serial.print('-');
    Serial.print(newDay);
snprintf(newenDate, sizeof(newenDate), "%04d-%02d-%02d", newYear, newMonth, newDay);
 writeFile(LittleFS, "/endDate.txt", newenDate);

        MaxPower = 0;
        preferences.begin("myApp", false);
        preferences.putFloat("MaxPower", MaxPower);
        preferences.end();
//  Serial.println(new1Date);
  }
  //
}
