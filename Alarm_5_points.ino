#include <Wire.h>
#include <LiquidCrystal.h>
#include <IRremote.h>

#define BAUDRATE 115200
#define DS1307_I2C_ADRES 0x68
#define IR_input_pin 51
#define lcdBackLight 22

// lcd pins order RS, R/W,  E, D4, D5, D6, D7
LiquidCrystal lcd(35,  33, 31, 34, 36, 38, 40);

//Remote Variables
IRrecv IRreceiver(IR_input_pin);
decode_results key;
decode_results previousKey;

// Time Variables
byte sec, minute, hour, dayOfWeek, dayOfMonth, month, year;

// Mode
String mode;

//Alarm Variables
int alarm1Hour;
int alarm1Minute;
int alarm2Hour;
int alarm2Minute;
int alarm3Hour;
int alarm3Minute;
int AlarmNum;


String alarmCursorPosition;
bool isAlarm1Set;
bool isAlarm2Set;
bool isAlarm3Set;

int flashingAlarm;

void setup() {
  // Clock Setup___________
  Wire.begin();
  Serial.begin(BAUDRATE);
  setDatesDs1307();

  //LCD Setup______________
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(lcdBackLight, OUTPUT);
  digitalWrite(lcdBackLight, HIGH);

  //Remote Setup
  IRreceiver.enableIRIn();

  isAlarm1Set = false;
  isAlarm2Set = false;
  isAlarm3Set = false;

  mode = "display";
  getDateDs1307();
  updateLcdClock();
}

void loop(){
  if(IRreceiver.decode(&key)){
    char button = decodeButtonPress(key.value);
    Serial.print("Mode :");
    Serial.println(mode);
    
    Serial.print("Pressed :");
    Serial.println(button);


    if(mode == "display"){
      setAlarmPrompt();
    }else if(mode == "set_alarm_prompt"){
      if(button == '#'){
        mode = "display";
        getDateDs1307();
        updateLcdClock();
        delay(1000);
      }else if(button == '1'){
        setAlarmScreen(1);
      }else if(button == '2'){
        setAlarmScreen(2);
      }else if(button == '3'){
        setAlarmScreen(3);
      }
    }else if(mode == "set_alarm_screen" ){
      if(button == '#'){
        mode = "display";
        getDateDs1307();
        updateLcdClock();
        delay(1000);
      }
      else if(button == '^'){
          if(alarmCursorPosition == "hour") {
            if(AlarmNum == 1){
              if(alarm1Hour == 23) alarm1Hour = 00;
              else alarm1Hour ++;
            }else if(AlarmNum == 2){
              if(alarm2Hour == 23) alarm2Hour = 00;
              else alarm2Hour ++;
            }else if(AlarmNum == 3){
              if(alarm3Hour == 23) alarm3Hour = 00;
              else alarm3Hour ++;
            }
          }
          else if(alarmCursorPosition == "minute"){
            if(AlarmNum == 1){
              if( alarm1Minute == 59)  alarm1Minute = 00;
              else  alarm1Minute++;
            }else if(AlarmNum == 2){
              if( alarm2Minute == 59)  alarm2Minute = 00;
              else  alarm2Minute++;
            }else if(AlarmNum == 3){
              if( alarm3Minute == 59)  alarm3Minute = 00;
              else  alarm3Minute++;
            } 
          }
          updateSetAlarmScreen(AlarmNum);
        }
        else if(button == 'v'){
          if(alarmCursorPosition == "hour") {
            if(AlarmNum == 1){
              if(alarm1Hour == 0) alarm1Hour = 23;
              else alarm1Hour--;  
            }else if(AlarmNum == 2){
              if(alarm2Hour == 0) alarm2Hour = 23;
              else alarm2Hour--;  
            }else if(AlarmNum == 3){
              if(alarm3Hour == 0) alarm3Hour = 23;
              else alarm3Hour--;  
            }
          }
          else if(alarmCursorPosition == "minute"){
            if(AlarmNum == 1){
              if( alarm1Minute == 0)  alarm1Minute = 59;
              else  alarm1Minute--;
            }else if(AlarmNum == 2){
              if( alarm2Minute == 0)  alarm2Minute = 59;
              else  alarm2Minute--;
            }else if(AlarmNum == 3){
              if( alarm3Minute == 0)  alarm3Minute = 59;
              else  alarm3Minute--;
            }
          }
          updateSetAlarmScreen(AlarmNum);
        }else if(button == '>' || button == '>'){
          if(alarmCursorPosition == "hour") alarmCursorPosition = "minute";
          else if(alarmCursorPosition == "minute") alarmCursorPosition = "hour";
          updateSetAlarmScreen(AlarmNum);
        }else if(button == 'O'){

          if(AlarmNum == 1) isAlarm1Set = true;
          else if(AlarmNum == 2) isAlarm2Set = true;
          else if(AlarmNum == 3) isAlarm3Set = true;
          alarmSetScreen();
        }
    }else if(mode == "alarm_set_screen"){
      if(button == 'O'){
        mode = "display";
        getDateDs1307();
        updateLcdClock();
        delay(1000);
      }
    }else if(mode == "alarm_screen"){
      if(button == 'O'){
       if(flashingAlarm == 1)isAlarm1Set = false;
       else if(flashingAlarm == 2)isAlarm2Set = false;
       else if(flashingAlarm == 3)isAlarm3Set = false;
        mode = "display";
        getDateDs1307();
        updateLcdClock();
        delay(1000);
      }
    }
    IRreceiver.resume();
  }else if(mode == "display"){
    getDateDs1307();
    updateLcdClock();
    delay(1000);

    if(isAlarm1Set){
      if(alarm1Hour == hour && alarm1Minute == minute){
        flashingAlarm = 1;
        alarmScreen();
      }
    }
    if(isAlarm2Set){
      if(alarm2Hour == hour && alarm2Minute == minute){
        flashingAlarm = 2;
        alarmScreen();
      }
    }
    if(isAlarm3Set){
      if(alarm3Hour == hour && alarm3Minute == minute){
        flashingAlarm = 3;
        alarmScreen();
      }
    }
  }else if(mode == "alarm_screen"){
    getDateDs1307();
    if(flashingAlarm == 1){
      if(alarm1Minute < minute){
        mode = "display";
        getDateDs1307();
        updateLcdClock();
        delay(1000);
        return;
      }
    }else if(flashingAlarm == 2){
      if(alarm2Minute < minute){
        mode = "display";
        getDateDs1307();
        updateLcdClock();
        delay(1000);
        return;
      }
    }else if(flashingAlarm == 2){
      if(alarm2Minute < minute){
        mode = "display";
        getDateDs1307();
        updateLcdClock();
        delay(1000);
        return;
      }
    }
    
    digitalWrite(lcdBackLight, LOW);
    delay(250);
    digitalWrite(lcdBackLight, HIGH);
    delay(250);
  }
}

void alarmScreen(){
  mode = "alarm_screen";

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alarm");
  lcd.print(flashingAlarm);
  lcd.print(" Triggered");
  lcd.setCursor(0,1);
  lcd.print("Press Ok 2 Stop");
  IRreceiver.resume();
}

void alarmSetScreen(){
  mode = "alarm_set_screen";
  lcd.noCursor();
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Alarm set");
  lcd.print(" ");

  if(AlarmNum == 1){
    if(alarm1Hour < 10) lcd.print(0);
    lcd.print(alarm1Hour);
    lcd.print(":");
    if(alarm1Minute < 10) lcd.print(0); 
    lcd.print(alarm1Minute);
  }if(AlarmNum == 2){
    if(alarm2Hour < 10) lcd.print(0);
    lcd.print(alarm2Hour);
    lcd.print(":");
    if(alarm2Minute < 10) lcd.print(0); 
    lcd.print(alarm2Minute);
  }if(AlarmNum == 3){
    if(alarm3Hour < 10) lcd.print(0);
    lcd.print(alarm3Hour);
    lcd.print(":");
    if(alarm3Minute < 10) lcd.print(0); 
    lcd.print(alarm3Minute);
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Press Ok Finish");
  delay(500);
}

void updateSetAlarmScreen(int alarmNum){
  lcd.setCursor(5, 1);

  if(alarmNum == 1){
    if(alarm1Hour < 10) lcd.print(0);
    lcd.print(alarm1Hour);
    lcd.print(":");
    if(alarm1Minute < 10) lcd.print(0);
    lcd.print(alarm1Minute);
  }else if(alarmNum == 2){
    if(alarm2Hour < 10) lcd.print(0);
    lcd.print(alarm2Hour);
    lcd.print(":");
    if(alarm2Minute < 10) lcd.print(0);
    lcd.print(alarm2Minute);
  }else if(alarmNum == 3){
    if(alarm3Hour < 10) lcd.print(0);
    lcd.print(alarm3Hour);
    lcd.print(":");
    if(alarm3Minute < 10) lcd.print(0);
    lcd.print(alarm3Minute);
  }
  
  if(alarmCursorPosition == "hour") lcd.setCursor(6, 1);
  if(alarmCursorPosition == "minute") lcd.setCursor(9, 1);
  delay(500);
}

void setAlarmScreen(int alarmNum){
  mode = "set_alarm_screen";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Alarm");
  lcd.print(alarmNum);
  lcd.print(" Time");

  lcd.setCursor(5, 1);
  
  if(alarmNum == 1){
    alarm1Hour = hour;
    alarm1Minute = minute;
    lcd.print(alarm1Hour);
    lcd.print(":");
    lcd.print(alarm1Minute);
  }else if(alarmNum == 2){
    alarm2Hour = hour;
    alarm2Minute = minute;
    lcd.print(alarm2Hour);
    lcd.print(":");
    lcd.print(alarm2Minute);
  }else if(alarmNum == 3){
    alarm3Hour = hour;
    alarm3Minute = minute;
    lcd.print(alarm3Hour);
    lcd.print(":");
    lcd.print(alarm3Minute);
  }

  AlarmNum = alarmNum;

  lcd.setCursor(6,1);
  lcd.blink();
  alarmCursorPosition = "hour";

  
}


char decodeButtonPress(int button){
  switch(button){
      case 16756815:  
        return '#';
        break;
      case 16738455: 
        return '*';
        break;
      case 16753245: 
        return '1';
        break;
      case 16736925: 
        return '2';
        break;
      case 16769565: 
        return '3';
        break;
      case 16720605: 
        return '4';
        break;
      case 16712445: 
        return '5';
        break;
      case 16761405: 
        return '6';
        break;
      case 16769055: 
        return '7';
        break;
      case 16754775: 
        return '8';
        break;
      case 16748655: 
        return '9';
        break;
      case 16750695: 
        return '0';
        break;
      case 16716015: 
        return '<';
        break;
      case 16734885: 
        return '>';
        break;
      case 16718055: 
        return '^';
        break;
      case 16730805: 
        return 'v';
        break;
      case 16726215: 
        return 'O';
        break;
        
    }
}

void setAlarmPrompt(){
  mode = "set_alarm_prompt";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set Which Alarm");
  lcd.setCursor(0,1);
  lcd.print(" 1, 2 or 3 #/No");
  lcd.print("  ");
}

void setDatesDs1307(){
  sec = 45;
  minute = 29;
  hour = 13;
  dayOfWeek = 2;
  dayOfMonth = 30;
  month = 6;
  year = 21;

  Wire.beginTransmission(DS1307_I2C_ADRES);
  Wire.write(decToBcd(0));
  Wire.write(decToBcd(sec));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

void getDateDs1307(){
  // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADRES);
  Wire.write(decToBcd(0));
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADRES, 7);
  sec = bcdToDec(Wire.read() & 0x7f);
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0x3f);
  dayOfWeek = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());
  
}

byte decToBcd(byte dec){return ((dec / 10 * 16) + (dec % 10));}
byte bcdToDec(byte bcd){return ((bcd / 16 * 10) + (bcd % 16));}

void updateLcdClock(){
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print(hour);
  lcd.print(":");
  lcd.print(minute);
  lcd.print(":");
  if(sec < 10) lcd.print("0");
  lcd.print(sec);


  lcd.setCursor(0,1);
  lcd.print(convertToDayOfWeek(dayOfWeek));
  lcd.print(". ");

  lcd.print(dayOfMonth);
  lcd.print(" ");

  lcd.print(convertToMonth(month));
  lcd.print(" ");  

  lcd.print("20");
  lcd.print(year);
}

String convertToDayOfWeek(int dayOfWeek){
   switch(dayOfWeek){
    case 1:
      return "Sun";
      break;
    case 2:
      return "Mon";
      break;
    case 3:
      return "Tue";
      break;
    case 4:
      return "Wed";
      break;
    case 5:
      return "Thu";
      break;
    case 6:
      return "Fri";
      break;
    case 7:
      return "Sat";
      break;
   }
}

String convertToMonth(int Month){
   switch(Month){
    case 1:
      return "Jan";
      break;
    case 2:
      return "Feb";
      break;
    case 3:
      return "Mar";
      break;
    case 4:
      return "Apr";
      break;
    case 5:
      return "May";
      break;
    case 6:
      return "Jun";
      break;
    case 7:
      return "Jul";
      break;
      case 8:
      return "Aug";
      break;
    case 9:
      return "Sep";
      break;
    case 10:
      return "Oct";
      break;
    case 11:
      return "Nov";
      break;
    case 12:
      return "Dec";
      break;
   }
}

String convertToYear(int year){
  return "20" + year;
}
