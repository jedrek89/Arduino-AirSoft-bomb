#include "U8glib.h"
#include <Keypad.h>
#include <Wire.h> 
#include <string.h> 

// 22, 24, 26, 42 diodes PINs

char keypressed;

int greenDiode = 22;
int redDiode = 24;
int yellowDiode = 26;
int redDiode2 = 42;
int buzzer = 44;

int bombStatus = 0;

int timeCursor = 0;
char time [8]= {'0', '0', ':', '0', '0', ':', '0', '0'};
int hours = 0;
int minutes = 0;
int seconds = 0;
long secMillis = 0;
long interval = 1000;
char password[4];
char entered[4];


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
  // putStringToArray(time, line4);
  // lcdPrint
  u8g.firstPage();  
  do {
    lcdPrint();
  } while( u8g.nextPage() );

  // Get variables from keypad and set buzzer
  getKey();

  // char keypressed = keypad.getKey();
  //   if (keypressed != NO_KEY)
  //     {
  //       digitalWrite (buzzer, HIGH);
  //       delay (300);
  //       digitalWrite (buzzer, LOW);
  //       // Serial.println(keypressed);
  //     }

  Serial.println(keypressed);

  // bombStatus == 0 - disarmed
  // bombStatus == 1 - time set
  // bombStatus == 2 - pin set
  // bombStatus == 4 - armed

  // Set time function - if bomb is disarmed
  if (bombStatus == 0)
    {
      putStringToArray("Bomba rozbrojona", line1);
      putStringToArray("Ustaw czas:", line2);
        if (timeCursor == 0)
          {
            putStringToArray("00:00:00", line3);
          }

        if (keypressed != NO_KEY)
          {
            setTime(keypressed, line3);
          }

        if (timeCursor > 0 && timeCursor < 8)
          {
            putStringToArray("*-usun element", line2);
          }

        if (timeCursor == 8)
          {
            putStringToArray("Czas ustawiony!", line2);
            bombStatus = 1;
          }
    }

      // Set pin function - if bomb is disarmed
  if (bombStatus == 1)
    {
      putStringToArray("Czas ustawiony!", line2);
      delay(1000);
      putStringToArray("# - zatwierdz", line2);
      delay(1000);
    }
  
    

    // lcdPrintString(0, 15, "line 1");
    // lcdPrintString(0, 30, "line 2");
    // unsigned int arrSize = sizeof(time);
    // unsigned int arrSizeLine1 = sizeof(line1);
    // lcdPrintArray(0, 15, line1, arrSize);
    // lcdPrintArray(0, 30, time, arrSize);

    // // Pozyskiwanie rozmiaru macierzy i wyświetlanie w serialu - działa
    // unsigned int arrSize = sizeof(time);
    // lcdPrintArray(time, arrSize);
    // // Serial.println(arrSize);
    // for(int i = 0; i < arrSize; i++)
    // {
    //   Serial.println(time[i]);
    // } 
    



    // lcdPrintString(0, 30, "line 2");
    

}

//////////////////////////////////////////////////////////
//  END LOOP
//////////////////////////////////////////////////////////

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

int setTime(char key, char line[]){
    // time[timeCursor] = {key};
    // Serial.println("setTime values: ");
    // Serial.print(time[timeCursor]);
    // timeCursor ++;
    // // putStringToArray(time, line3);
    // return line3;
    if (keypressed == '1' || keypressed == '2' || keypressed == '3' || keypressed == '4' || keypressed == '5' 
        || keypressed == '6' || keypressed == '7' || keypressed == '8'|| keypressed == '9')
    {
      if (timeCursor < 8)
        {
          line3[timeCursor] = {key};
          timeCursor ++;
          if (timeCursor == 2 || timeCursor == 5)
            {
              line3[timeCursor] = {':'};
              timeCursor ++;
            }
        Serial.print("timeCursor: "); 
        Serial.println(timeCursor);
        }
    }
    

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