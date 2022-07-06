#include "U8glib.h"
#include <Keypad.h>
#include <Wire.h> 
#include <string.h> 
// 22, 24, 26, 42 diodes PINs
int greenDiode = 22;
int redDiode = 24;
int yellowDiode = 26;
int redDiode2 = 42;
int buzzer = 44;

char keypressed;
int bombStatus = 0;
int timeCursor = 0;
char time [8]= {'h', 'h', ':', 'm', 'm', ':', 's', 's'};
int hours = 0;
int minutes = 0;
int seconds = 0;
long secMillis = 0;
long interval = 1000;
short int pinCursor = 0;
char pin[4] = {'0', '0', '0', '0'};
char pinEntered[4];
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
  digitalWrite (buzzer, LOW);
  Serial.begin(9600);
  pinMode(redDiode, OUTPUT); 
  pinMode(yellowDiode, OUTPUT);
  pinMode(greenDiode, OUTPUT);
  pinMode(buzzer, OUTPUT);

  u8g.setFont(u8g_font_unifont);  //Set font.
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) 
    u8g.setColorIndex(255);     // white
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT )
    u8g.setColorIndex(3);         // max intensity
  else if ( u8g.getMode() == U8G_MODE_BW )
    u8g.setColorIndex(1);         // pixel on

    //  print arr to serial works !!
    // unsigned int arrSize = sizeof(time);
    // lcdPrintArray(time, arrSize);

}

//////////////////////////////////////////////////////////
// LOOP
//////////////////////////////////////////////////////////


void loop(void)
{

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
      putStringToArray("Bomba rozbrojona", line1);
      putStringToArray("Ustaw czas:", line2);
        if (timeCursor == 0)
          {
            putStringToArray("hh:mm:ss", line3);
          }
      setTime(keypressed, line3);

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
        lcdPrint();
        bombStatus++;
        }
    }

  // Set pin function - bombStatus = 0;
  if (bombStatus == 1)
  { 
    if (pinCursor == 0)
    {
      putStringToArray("", line3);
      putStringToArray("Ustaw pin:", line2);
    }
    setPin(keypressed);
    // Serial.println("pinCursor: "); Serial.print(pinCursor);

    if (pinCursor == 5)
    {

      putStringToArray("Pin ustawiony!", line2);
      putStringToArray("", line3);

      bombStatus ++;
    }

  }


  // Bomb armed - bombStatus = 2;
  if (bombStatus == 2)
  { 
    putStringToArray("Bomba uzbrojona", line1);

  }


}

//////////////////////////////////////////////////////////
//  END LOOP
//////////////////////////////////////////////////////////

// Set pin function
int setPin(char key){

  if (keypressed == '*' && pinCursor > 0){
    pinCursor --;
  }

  // pin - set confirm
  if (keypressed == '#' && pinCursor == 4){
    pinCursor = 5;
  }

  if (key == '1' || keypressed == '2' || keypressed == '3' || keypressed == '4' || keypressed == '5' 
    || keypressed == '6' || keypressed == '7' || keypressed == '8'|| keypressed == '9' || keypressed == '0')
    {
      if (pinCursor <= 3)
      {
        pinCursor++;
      }
    }

    if (pinCursor == 1)
      {
        putStringToArray("*-usun", line2);
        putStringToArray("*", line3);
      }

    if (pinCursor == 2)
      { 
        putStringToArray("*-usun", line2);
        putStringToArray("**", line3);
      }

    if (pinCursor == 3)
      {
        putStringToArray("*-usun", line2);
        putStringToArray("***", line3);
      }

    if (pinCursor == 4)
      {
        putStringToArray("*-usun #-potw", line2);
        putStringToArray("****", line3);
      }

      char tempVal = pinCursor;
      pin[tempVal - 1] = {key};
      Serial.println(pin[tempVal - 1]);

    return pinCursor;
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
        if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5')
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
      // Put chars from lcd to variables
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






// LCD print 
// (0, 15, line1);
// (0, 30, line2);
// (0, 45, line3);
// (0, 60, line4);
// void lcdPrintString(int x, int y, String a){
//   u8g.firstPage();
//   u8g.setFont(u8g_font_unifont);
//   do
//   {
//     String val = a;
//     u8g.setPrintPos(x, y);
//     u8g.print(val);
//   } while (u8g.nextPage());
//   }

  // void lcdPrintChar(int x, int y, char letter){
  // do
  // {
  //   Serial.println("letter: ");
  //   Serial.println(letter);
  //   u8g.setPrintPos(x, y);
  //   u8g.print(letter);
  // } while (u8g.nextPage());
  // }


  // u8g.firstPage();

  // do {
  //       u8g.setFont(u8g_font_unifont);
  //       u8g.drawStr( 0, 15, lcdLine1);
  //       u8g.drawStr( 0, 30, lcdLine2);
  //       u8g.drawStr( 0, 45, lcdLine3);
  //       u8g.drawStr( 0, 60, lcdLine4);
  //     } while( u8g.nextPage() );
  

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




  //  print arr to serial works !!
  // void lcdPrintArray(char arr[], int arrSize){
  //   for (int i = 0; i < arrSize; i++)
  //   {
  //     Serial.print(arr[i]);
  //     delay(100);
  //   }
  // }



  // Get variable from keypad and set buzzer
char getKey(){
    keypressed = keypad.getKey();
    if (keypressed != NO_KEY)
      {
        digitalWrite (buzzer, HIGH);
        delay (300);
        digitalWrite (buzzer, LOW);
      }
      return keypressed;
}