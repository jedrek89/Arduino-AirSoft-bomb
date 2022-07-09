#include "U8glib.h"
#include <Keypad.h>
#include <Wire.h> 
#include <string.h> 
// 22, 24, 26, 42 diodes PINs
int greenDiode = 22;
int redDiode = 24;
int yellowDiode = 26;
int redDiode2 = 42;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;
unsigned short int countStatus = 0;
unsigned short int disarmStatus = 0;
char keypressed;
int bombStatus = 0;
int timeCursor = 0;
char time [8]= {'h', 'h', ':', 'm', 'm', ':', 's', 's'};
int hours = 0;
int minutes = 0;
int seconds = 0;
int hoursMillis = -1;
int minutesMillis;
short int secondsMillis;
long secMillis = 0;
long interval = 1000;
short int pinCursor1 = 0;
short int pinCursor2 = 0;
char pin[4] = {'0', '0', '0', '0'};
char pinEntered[4] = {'0', '0', '0', '0'};
char line1 [16];
char line2 [16];
char line3 [16];
char line4 [16];

U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {28, 30, 32, 34};  //7,2,3,5 for Black 4x3 keypad
byte colPins[COLS] = {36, 38, 40};     //6,8,4 for Black 4x3 Keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup(void) {
  digitalWrite (A8, LOW);
  Serial.begin(9600);
  pinMode(redDiode, OUTPUT); 
  pinMode(yellowDiode, OUTPUT);
  pinMode(greenDiode, OUTPUT);
  pinMode(A8, OUTPUT); // Buzzer

  u8g.setFont(u8g_font_unifont);  //Set font.
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) 
    u8g.setColorIndex(255);     // white
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT )
    u8g.setColorIndex(3);         // max intensity
  else if ( u8g.getMode() == U8G_MODE_BW )
    u8g.setColorIndex(1);         // pixel on
  startMillis = millis();
}

//////////////////////////////////////////////////////////
// LOOP
//////////////////////////////////////////////////////////

void loop(void)
{ 
    currentMillis = millis(); 
    // Serial.println("bomb status: ");
    // Serial.println(bombStatus);

  // lcdPrint
  u8g.firstPage();  
  do {
    lcdPrint();
  } while( u8g.nextPage() );

  // Get variables from keypad and set buzzer
  getKey();

  // bombStatus - 0 - disarmed - before set a time
  // bombStatus - 1 - disarmed - after set a time, before a set pin    
  // bombStatus - 2 - armed !!

  // Set time function - bombStatus = 0;
  if (bombStatus == 0)
    {
      if (bombStatus == 0 && timeCursor == 0)
      {
        digitalWrite (greenDiode, HIGH);
      }
      putStringToArray("Bomba rozbrojona", line1);
      putStringToArray("Ustaw czas:", line2);
        if (timeCursor == 0)
          {
            putStringToArray("hh:mm:ss", line3);
          }
      setTime(keypressed, line3);

        if (timeCursor > 0)
          {
            digitalWrite (greenDiode, LOW);
            digitalWrite (yellowDiode, HIGH);
          }
      
        if (timeCursor > 0 && timeCursor < 8)
          {
            putStringToArray("*-usun", line2);
          }

        if (timeCursor == 8)
          {
            putStringToArray("*-usun #-potw", line2);
          }

        if (timeCursor == 9)
          {
            putStringToArray("Czas ustawiony!", line2);
            bombStatus = 1;
          }
    }

  // Set pin function - bombStatus = 1;
  if (bombStatus == 1)
  { 
    if (pinCursor1 == 0)
    {
      putStringToArray("", line3);
      putStringToArray("Ustaw pin:", line2);
    }
    setPin(keypressed);
    // Serial.println("pinCursor1: "); Serial.print(pinCursor1);

    if (pinCursor1 == 5)
    {
      putStringToArray("Pin ustawiony!", line2);
      putStringToArray("", line3);
      bombStatus = 2;
    }
  }


  // Bomb armed - bombStatus = 2;
  if (bombStatus == 2)
  { 
    digitalWrite (yellowDiode, LOW);
    if (countStatus == 0)
    {
      putStringToArray("Bomba uzbrojona", line1);
      putStringToArray("Wybuchnie za:", line2);
      putStringToArray("PIN: ", line4);
      do
      {
        counting();
        disarmBomb(keypressed);
      } while (pinCursor2 == 5 && disarmStatus == 4);
      
      
      if (pinCursor2 == 5 && disarmStatus == 4)
      {
        bombStatus = 0;
      }
    }

    if (countStatus == 1 )
    {
      bombStatus++;
    }
  }

  // Bomb exploded! - bombStatus = 3;
    if (bombStatus == 3 )
  { 
      putStringToArray("Booooom!", line1);
      putStringToArray("", line2);
      putStringToArray("", line3);
      putStringToArray("", line4);
      digitalWrite (redDiode, HIGH);
      digitalWrite (redDiode2, HIGH);
      delay(100);
      digitalWrite (redDiode, LOW);
      digitalWrite (yellowDiode, HIGH);
      digitalWrite (redDiode2, LOW);
      delay(100);
      digitalWrite (yellowDiode, LOW);
      digitalWrite (greenDiode, HIGH);
      digitalWrite (redDiode2, HIGH);
      delay(100);
      digitalWrite (greenDiode, LOW);
      digitalWrite (redDiode2, LOW);

  }


}

//////////////////////////////////////////////////////////
//  END LOOP
//////////////////////////////////////////////////////////

// Disarm bom function
int disarmBomb(char key){
    if (key == '*' && pinCursor2 > 0){
    pinCursor2 --;
  }

  // pin - set confirm
  if (key == '#' && pinCursor2 == 4){
    pinCursor2 = 5;

    if (pinEntered[0] == pin[0])
      {
        disarmStatus++;
      }

    if (pinEntered[1] == pin[1])
      {
        disarmStatus++;
      }

    if (pinEntered[2] == pin[2])
      {
        disarmStatus++;
      }

    if (pinEntered[3] == pin[3])
      {
        disarmStatus++;
      }

  }

  if (key == '1' || keypressed == '2' || keypressed == '3' || keypressed == '4' || keypressed == '5' 
    || keypressed == '6' || keypressed == '7' || keypressed == '8'|| keypressed == '9' || keypressed == '0')
    {
      if (pinCursor2 <= 3)
      {
        pinCursor2++;
      }
    }

    if (pinCursor2 == 1)
      {
        putStringToArray("PIN: *", line4);
        pinEntered[0] = {key};
        Serial.println(pinEntered[0]);
      }

    if (pinCursor2 == 2)
      { 
        putStringToArray("PIN: **", line4);
        pinEntered[1] = {key};
        Serial.println(pinEntered[1]);
      }

    if (pinCursor2 == 3)
      {
        putStringToArray("PIN: ***", line4);
        pinEntered[2] = {key};
        Serial.println(pinEntered[2]);
      }

    if (pinCursor2 == 4)
      {
        putStringToArray("PIN: ****", line4);
        pinEntered[3] = {key};
        Serial.println(pinEntered[3]);
      }
      Serial.println("disaramStatus:");
      Serial.println(disarmStatus);

      return disarmStatus;
}

// Counting function
int counting(){
    if (currentMillis - startMillis >= period)  
    {
      startMillis = currentMillis; 
      // secondsMillis = ((startMillis/1000)%60);
      // Serial.print("secondsMillis: "), Serial.println(secondsMillis);
      if (seconds % 2)
      {
        digitalWrite (redDiode, HIGH);
      }
      else
      {
        digitalWrite (redDiode, LOW);
        // tone(A8, 0); // Buzzer with tone

      }
      
      tone(A8, 4000, 500); // Buzzer with tone
      analogWrite(8, LOW);
      seconds--;
  
      if (seconds == -1 && minutes > 0)
      {
        minutes --;
        seconds = 59;
      }

      if (seconds == -1 && minutes == 0 && hours > 0)
      {
        hours--;
        seconds = 59;
      }

      if (seconds == 0 && minutes == 0 && hours == 0)
      {
        countStatus++; // countStatur = 1 - couting is end
        // Serial.print("countring status: "), Serial.println(countStatus);
      }

      // Serial.print("seconds: "), Serial.println(seconds);
      
      // Split values to two char character && put to char aaray - line3
      String hoursStr;
      String minutesStr;
      String secondsStr;
      String zero = "0";
      secondsStr = seconds;
      if (seconds < 10)
      {
        secondsStr = zero + seconds;
      }
      minutesStr= minutes;
      if (minutes < 10)
      {
        minutesStr = zero + minutes;
      }
      hoursStr = hours;
      if (minutes < 10)
      {
        hoursStr = zero + hours;
      }
      String separator = ":";

      String clockTemp;
      clockTemp = hoursStr + separator + minutesStr + separator + secondsStr;
      putStringToArray(clockTemp, line3);
      
    }
  return countStatus;
}

// Set pin function
int setPin(char key){

  if (keypressed == '*' && pinCursor1 > 0){
    pinCursor1 --;
  }

  // pin - set confirm
  if (keypressed == '#' && pinCursor1 == 4){
    pinCursor1 = 5;
    // Serial.println(pin[0]);
    // Serial.println(pin[1]);
    // Serial.println(pin[2]);
    // Serial.println(pin[3]);
  }

  if (key == '1' || keypressed == '2' || keypressed == '3' || keypressed == '4' || keypressed == '5' 
    || keypressed == '6' || keypressed == '7' || keypressed == '8'|| keypressed == '9' || keypressed == '0')
    {
      if (pinCursor1 <= 3)
      {
        pinCursor1++;
      }
    }

    if (pinCursor1 == 1)
      {
        putStringToArray("*-usun", line2);
        putStringToArray("*", line3);
        pin[0] = {keypressed};
        Serial.print(pin[0]);
      }

    if (pinCursor1 == 2)
      { 
        putStringToArray("*-usun", line2);
        putStringToArray("**", line3);
        pin[1] = {keypressed};
        Serial.print(pin[1]);
      }

    if (pinCursor1 == 3)
      {
        putStringToArray("*-usun", line2);
        putStringToArray("***", line3);
        pin[2] = {keypressed};
        Serial.print(pin[2]);
      }

    if (pinCursor1 == 4)
      {
        putStringToArray("*-usun #-potw", line2);
        putStringToArray("****", line3);
        pin[3] = {keypressed};
        Serial.print(pin[3]);
      }

      // char tempVal = pinCursor1;
      // pin[tempVal - 1] = {key};
      // Serial.println(pin[tempVal - 1]);

    return pinCursor1;
}

  // lcdPrint function
  void lcdPrint(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  u8g.drawStr( 0, 15, line1);
  u8g.drawStr( 0, 30, line2);
  u8g.drawStr( 0, 45, line3);
  u8g.drawStr( 0, 60, line4);
}

  // Put string to array - modify line 1-4 on lcd
  void putStringToArray(String a, char arr[]){
    // 16 - default array size
    String val = a;
      for (int i = 0; i < 16; i++)
        {
          arr[i] = val[i];
          // Serial.println(arr[i]);
        }
  }

// Set time function
int setTime(char key, char line[]){
    if (timeCursor == 2 || timeCursor == 5)
      {
        line3[timeCursor] = {':'};
        timeCursor ++;
      }

    if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' 
        || key == '6' || key == '7' || key == '8'|| key == '9' || key == '0')
    {
        if (timeCursor == 0 || timeCursor == 1 || timeCursor == 4 || timeCursor == 7)
          {
            line3[timeCursor] = {key};
            timeCursor ++;
          }

    else if (timeCursor == 3 || timeCursor == 6)
      {
        if (key == '0' ||key == '1' || key == '2' || key == '3' || key == '4' || key == '5')
          {
            line3[timeCursor] = {key};
            timeCursor ++;
          }
      }
    }

    if (keypressed == '*' && timeCursor >= 1)
    {
      timeCursor--;
        if (timeCursor == 7 || timeCursor == 6)
          {
            line3[timeCursor] = {'s'};
          }

        if (timeCursor == 2 || timeCursor == 5)
          {
            timeCursor--;
          }

        if (timeCursor == 4 || timeCursor == 3)
          {
            line3[timeCursor] = {'m'};
          }

        if (timeCursor == 1 || timeCursor == 0)
          {
            line3[timeCursor] = {'h'};
          }
      // line3[timeCursor] = {'0'};
    }

    if (keypressed == '#' && timeCursor == 8)
    {
      // Put chars from lcd to int variables
      hours = ( 10 * (line[0] - '0' ) ) + line[1] - '0';
      minutes = ( 10 * (line[3] - '0' ) ) + line[4] - '0';
      seconds = ( 10 * (line[6] - '0' ) ) + line[7] - '0';
      Serial.print("hours: "); 
      Serial.println(hours);
      Serial.print("minutes: "); 
      Serial.println(minutes);
      Serial.print("seconds: "); 
      Serial.println(seconds);
      timeCursor++;
    }
    // Serial.print("timeCursor: "); 
    // Serial.println(timeCursor);
    return timeCursor;
}

  void lcdPrintString(int x, int y, String a){
  u8g.firstPage();
  u8g.setFont(u8g_font_unifont);
    do
    {
      String val = a;
      // u8g.setPrintPos(x, y);
      // u8g.print(val);
      // u8g.drawStr(0, 20, val);
      delay(100);
    } while (u8g.nextPage());
  }

  void lcdPrintArray(int x, int y, char arr[], int arrSize){
    u8g.setFont(u8g_font_unifont);
    String val = "";
        for (int i = 0; i < arrSize; i++)
          {
            val = val + arr[i];
          }
            do
            {
              u8g.setPrintPos(x, y);
              u8g.print(val);
              delay(100);
            } while (u8g.nextPage());
  }

  // Get variable from keypad and set buzzer
char getKey(){
    keypressed = keypad.getKey();
    if (keypressed != NO_KEY)
      {
        if (bombStatus != 2)
        {
          tone(A8, 4000, 300); // Buzzer with tone
        }
        
        digitalWrite (redDiode2, HIGH);
        delay (200);
        digitalWrite (redDiode2, LOW);
      }
      return keypressed;
}
