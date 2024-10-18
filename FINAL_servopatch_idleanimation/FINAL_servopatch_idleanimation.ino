
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int servotime = 13;
#define SERVOMIN  2000 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  SERVOMIN+servotime

//LCD vars
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
bool skipIntroDelay = false;
const int selectMsgTime = 2000;

//LED RELAY vars
const int relaycount = 16; // less than or equal to 16.
const byte pin[relaycount] = {26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41};
int idletimer = 29;
int idletimeout = 40;
int idleframedelay = 300;

/*
      --- animation bi-dimensional array---
   (i)> 32 is the number of frames for the entire animation. We have a separate animation for each direction (NW, SW, NE, SE).
   (j)> 16 is the number of booleans, which determine whether or not the kth LED should be turned on for that frame.

*/
const boolean aniData[32][16] = {

  // 1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16
  { false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f1
  { false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    true, false, false, false
  }, //f2
  { false, false, false, false,
    false, false, false, false,
    true, false, false, false,
    false, true, false, false
  }, //f3
  { false, false, false, false,
    true, false, false, false,
    false, true, false, false,
    false, false, true, false
  }, //f4
  { true, false, false, false,
    false, true, false, false,
    false, false, true, false,
    false, false, false, true
  }, //f5
  { false, true, false, false,
    false, false, true, false,
    false, false, false, true,
    false, false, false, false
  }, //f6
  { false, false, true, false,
    false, false, false, true,
    false, false, false, false,
    false, false, false, false
  }, //f7
  { false, false, false, true,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f8
  //SE aniset


  // 1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16
  { false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f1
  { false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, true
  }, //f2
  { false, false, false, false,
    false, false, false, false,
    false, false, false, true,
    false, false, true, false
  }, //f3
  { false, false, false, false,
    false, false, false, true,
    false, false, true, false,
    false, true, false, false
  }, //f4
  { false, false, false, true,
    false, false, true, false,
    false, true, false, false,
    true, false, false, false
  }, //f5
  { false, false, true, false,
    false, true, false, false,
    true, false, false, false,
    false, false, false, false
  }, //f6
  { false, true, false, false,
    true, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f7
  { true, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f8
  //SW aniset


  // 1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16
  { false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f1
  { false, false, false, true,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f2
  { false, false, true, false,
    false, false, false, true,
    false, false, false, false,
    false, false, false, false
  }, //f3
  { false, true, false, false,
    false, false, true, false,
    false, false, false, true,
    false, false, false, false
  }, //f4
  { true, false, false, false,
    false, true, false, false,
    false, false, true, false,
    false, false, false, true
  }, //f5
  { false, false, false, false,
    true, false, false, false,
    false, true, false, false,
    false, false, true, false
  }, //f6
  { false, false, false, false,
    false, false, false, false,
    true, false, false, false,
    false, true, false, false
  }, //f7
  { false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    true, false, false, false
  }, //f8
  //NW aniset


  // 1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16
  { false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f1
  { true, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f2
  { false, true, false, false,
    true, false, false, false,
    false, false, false, false,
    false, false, false, false
  }, //f3
  { false, false, true, false,
    false, true, false, false,
    true, false, false, false,
    false, false, false, false
  }, //f4
  { false, false, false, true,
    false, false, true, false,
    false, true, false, false,
    true, false, false, false
  }, //f5
  { false, false, false, false,
    false, false, false, true,
    false, false, true, false,
    false, true, false, false
  }, //f6
  { false, false, false, false,
    false, false, false, false,
    false, false, false, true,
    false, false, true, false
  }, //f7
  { false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, true
  }, //f8
  //NE aniset
};

//BUTTON vars
const byte buttonPin = A3;
bool buttonStatus = false;

//COIN READER vars
const byte coinSig = A5; //change for our arduino
const byte counterSig = A6;
bool previousCoinSignal = false;
const float coinValue = 0.05;
float bankValue = 0.00;
const double price[] = {0.25, 0.25, 0.25, 0.25, 0.50, 0.50, 0.50, 0.50, 0.75, 0.75, 0.75, 0.75, 1.00, 1.00, 1.00, 1.00};
bool frugalMode = true;

unsigned long previousUpdateMillis = 0;
unsigned long updateInterval = 110;
bool printOnceFlag = false;

unsigned long previousEEPROMwrite = 0;
unsigned long updateEEPROMinterval = 110;
float EEPROMbankValue = 0.00;

//SERVO vars
uint8_t servonum = 0;
int selectedrelay = 0;
boolean servotrigger = false;
int servodelay = 1500;
//int turntime = 10;

//JOYSTICK vars
int joyPin1 = A0;                 // slider variable connecetd to analog pin 0
int joyPin2 = A1;                 // slider variable connecetd to analog pin 1
int value1 = 0;                  // variable to read the value from the analog pin 0
int value2 = 0;                  // variable to read the value from the analog pin 1
int gridPos[4][4] = {
  {0, 1, 2, 3},
  {4, 5, 6, 7},
  {8, 9, 10, 11},
  {12, 13, 14, 15}
};
boolean isCentered = true;
int gridX = 0;
int gridY = 0;
//int currentPos;//= gridPos[gridX][gridY];
int oldX;
int oldY;

//functions
void loadEEPROM()
{
  EEPROM.get(0, bankValue);
  EEPROMbankValue = bankValue;
}

void printBank()
{
  Serial.print("BankValue: ");
  Serial.println(bankValue);
}


void setup() {
  Serial.begin(115200);
  Serial.print(relaycount);
  Serial.println(" channel Servo test!");
  //Serial.print("Current Relay: ");
  //Serial.println(selectedrelay);
  Serial.print("Selected Snack: ");
  Serial.println(gridPos[gridY][gridX]);

  pwm.begin();

  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  //RED BUTTON
  pinMode(buttonPin, INPUT);

  //LEDS
  for (int i = 0; i < relaycount; i++)
  {
    pinMode(pin[i], OUTPUT);
  }

  for (int i = 0; i < relaycount; i++)
  {
    digitalWrite(pin[i], HIGH);
  }

  //SERVO RESET
  for (int i = 0; i < relaycount; i++)
  {
    for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
      pwm.setPWM(selectedrelay, 4096, 0);
    }
  }

  pwm.setPWM(3, 0, 360);

  //LCD
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Select yo snack!");

  if (!skipIntroDelay)
  {
    delay(selectMsgTime);
  }
  else
  {
    delay(10);
  }

  //COIN COUNTER
  pinMode(coinSig, INPUT_PULLUP);
  previousCoinSignal = digitalRead(coinSig);
  loadEEPROM();
  bankValue = 0.00;
  printBank();
}

void handleSerial()
{
  char incomingChar = Serial.read();

  switch (incomingChar)
  {
    case '!':
      Serial.println("Clearing...");
      bankValue = 10.00;
      Serial.print("BankValue: ");
      Serial.println(bankValue);
      //EEPROMbankValue = 0.00;
      break;
  }
}

int treatValue(int data) {
  return (data * 9 / 1024 - 4);
}

bool checkAniExitTriggers()
{
  //button update
  buttonStatus = analogRead(buttonPin);
  value1 = analogRead(joyPin1);
  //delay(100);
  value2 = analogRead(joyPin2);

  //joystick update
  if (treatValue(value1) == 0 && treatValue(value2) == 0)
  {
    isCentered = true;
  }
  else
  {
    isCentered = false;
  }

  //coin reader update
  byte currentcounterSig = digitalRead(counterSig);
  /*
    Serial.print("cr: ");
    Serial.println(currentCoinSignal);
    Serial.print("cntr: ");
    Serial.println(currentcounterSig);
  */

  if (!currentcounterSig)
  {
    bankValue += 0.25; //this exact value is okay. i tested with a nickel, and it did not produce change in bankValue or an exit in the idle animation.
    //also, we're only using quarters, so no need to double check the number of pulses from currentCoinSignal.
  }

  /*
    Serial.print("Button: ");
    Serial.println(buttonStatus);
    Serial.print("JS: ");
    Serial.println(isCentered);
  */
  if (!isCentered || !buttonStatus || !currentcounterSig)
  {
    idletimer = 0;
    return true;
  }
  else
  {
    return false;
  }
}

void loop() {
  handleSerial();

  idletimer++;

  //Serial.println(idletimer);

  for (int i = 0; i < relaycount; i++)
  {
    digitalWrite(pin[i], HIGH);
  }
  //digitalWrite(pin[selectedrelay], LOW);        //needs to change to accept the joystick's input.

  if (idletimer < idletimeout)
  {
    digitalWrite(pin[ gridPos[gridY][gridX] ], LOW);   //new code for selecting lights. arrays in arrays!
  }
  else// if(idletimer % 1 == 0)
  {
    lcd.clear();
    lcd.print("Select yo snack!");
    bool exittemp = false;
    for (int i = 0; i < 32; i++)
    {
      if (idletimer % 1 == 0)
      {
        if (exittemp)
        {
          break;
        }

        //checkAniExitTriggers();
        for (int j = 0; j < 16; j++)
        {
          digitalWrite(pin[j], !aniData[i][j]);
          //in anidata, on-lights are represented as true. so, because the relay is low-trigger, the returned boolean is negated. 
          if (checkAniExitTriggers())
          {
            //idletimer = 0;
            exittemp = true;
            break;
          }
        }
        delay(idleframedelay);
        //delay(1);
      }

    }
  }

  delay(300);

  if (servotrigger && bankValue >= price[ gridPos[gridY][gridX] ])
  {
    if (gridPos[gridY][gridX] == 3)
    {
      for (uint16_t pulselen = 100; pulselen < 135; pulselen++) {
        pwm.setPWM(gridPos[gridY][gridX], 0, pulselen);
        delay(20);
      }
      //Do not modify any code related to servo 3, its response curve is way different from the others for some reason.
    }
    else
    {
      for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
        pwm.setPWM(gridPos[gridY][gridX], 0, pulselen);
        delay(20);
      }
    }

    lcd.clear();
    lcd.print("Snack " + (String)(gridPos[gridY][gridX] + 1));
    lcd.setCursor(0, 1);
    lcd.print("       SELECTED!");
    lcd.setCursor(0, 0);

    delay(servodelay);
    Serial.println("servo stop");
    if (gridPos[gridY][gridX] == 3)
    {
      pwm.setPWM(3, 0, 360); //360 is an arbitrary value ****NOT**** related to degrees measure in any way. Sending 4096 to servo 3 (originally broken) results in that servo continuing to turn.
      //Do not modify any code related to servo 3.
    }
    else
    {
      for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
        pwm.setPWM(gridPos[gridY][gridX], 4096, 0);
      }
    }

    if (frugalMode)
    {
      bankValue -= price[ gridPos[gridY][gridX] ];
    }
    else
    {
      bankValue = 0.00;
    }
    Serial.println("bankValue: " + (String)bankValue);

    lcd.clear();
    lcd.print("Select yo snack!");

    servotrigger = false;
    delay(selectMsgTime);
  }

  //LCD joystickmovement response code
  if (!servotrigger)
  {
    lcd.clear();
    lcd.print("Snack " + (String)(gridPos[gridY][gridX] + 1) + ": $" + (String)price[ gridPos[gridY][gridX] ]);
    lcd.setCursor(0, 1);
    lcd.print(" $" + (String)bankValue + " inserted!");
    lcd.setCursor(0, 0);
  }

  //JOYSTICK CODE
  // reads the value of the variable resistor
  value1 = analogRead(joyPin1);
  // this small pause is needed between reading
  // analog pins, otherwise we get the same value twice
  delay(100);
  // reads the value of the variable resistor
  value2 = analogRead(joyPin2);

  if (treatValue(value1) == 0 && treatValue(value2) == 0)
  {
    isCentered = true;
  }
  else
  {
    isCentered = false;

    idletimer = 0;
  }

  if (oldX != gridX || oldY != gridY)
  {
    Serial.print("Selected Snack: ");
    Serial.println(gridPos[gridY][gridX]); //test with "currentPos", should work. my mistake earlier!

    oldX = gridX;
    oldY = gridY;
  }

  if (!isCentered)
  {
    //grid X values
    if (treatValue(value1) > 1)
    {
      if (gridX < 3)
      {
        gridX++;
      }
      else
      {
        gridX = 0;
      }
    }

    if (treatValue(value1) < -1)
    {
      if (gridX > 0)
      {
        gridX--;
      }
      else
      {
        gridX = 3;
      }
    }

    //grid Y values
    if (treatValue(value2) > 1)
    {
      if (gridY < 3)
      {
        gridY++;
      }
      else
      {
        gridY = 0;
      }
    }

    if (treatValue(value2) < -1)
    {
      if (gridY > 0)
      {
        gridY--;
      }
      else
      {
        gridY = 3;
      }
    }
  }

  //BUTTON CODE
  buttonStatus = analogRead(buttonPin);
  //Serial.println(buttonStatus);
  bool buttonflag = false;
  if (buttonStatus)
  {
    //Serial.println("button OFF");
    servotrigger = false;
    buttonflag = false;
  }
  else
  {
    Serial.println("button ON");
    if (!buttonflag)// && bankValue >= price[ gridPos[gridX][gridY] ])
    {
      servotrigger = true;
      buttonflag = true;

      idletimer = 0;
    }
  }
  delay(10);


  //COIN READER CODE
  byte currentCoinSignal = digitalRead(coinSig);

  if (currentCoinSignal == HIGH)
  {
    previousUpdateMillis = millis();
    printOnceFlag = true;
  }

  if (printOnceFlag && (millis() - previousUpdateMillis >= updateInterval))
  {
    printBank();
    printOnceFlag = false;
  }

  if (millis() - previousEEPROMwrite >= updateEEPROMinterval)
  {
    previousEEPROMwrite = millis();

    if (bankValue != EEPROMbankValue)
    {
      EEPROM.put(0, bankValue);
      Serial.println("!!! EEPROM WRITE !!!");
      EEPROMbankValue = bankValue;
    }
  }

  if (currentCoinSignal != previousCoinSignal)
  {
    previousCoinSignal = currentCoinSignal;

    if (currentCoinSignal == HIGH)
    {
      bankValue = bankValue + 5 * coinValue;
    }
  }

  delay(100);
}

/*
   Alex/Hunter, I figured out the source of the problem- turns out the PCA9685 board was
   bad somehow. It's replaced now, and the servo works perfectly. I'm going to work on
   getting the lights and relays to sync up.

   UPDATE: I'm deciding to make it so that the servos only turn when triggered ("@" symbol is a stand-in for the button signal, and "!" is a stand-in for the joystick input.)
   I've got the lights to turn on, but right now I'm trying to make them turn on and off. Please mark any and all edits.

   UPDATE2: I'm combining the joystick code and the servo/light relay code together in this file from home. Since I have no idea how this'll work without the machine next to
   me, I cannot guarantee this code will work. See "joystickgrid" and "lightrelaysync" folders for the original files. Also keep in mind, we will have to make space for two
   analog pins on the mega board so that the joystick can work. Since we're using the analog pins for the light relay as digital pins anyway, it shouldn't be too much work to
   replace the values in the pin Array.

   By the way, are we sticking to the big red button for the "lock in selection" button? me, alex and reed found out the other day that you can push down on the joystick like
   a button, but coach would get upset at not having the pizazz of the button :D

  UPDATE3: I worked on the combined joystick/light code from home, and after some drastic changes to the wiring (see the paper), it works perfectly. You can hold the joystick
  in a certain direction, and it will move that way seamlessly (if it "goes off the border", it will wrap around to the other side.

  We also need to cut a small notch above the holes in the back board so that both the LED cable and the servo can fit through. Let's wait till Ziegler gets back before
  modifying the vending machine's frame.

  UPDATE4: Guys, the servos are synced up with the relays and lights! Now we need to integrate the big red button, and fix the coin reader. Then, when if the button is pressed, it
  will check if enough change was inserted: if so, it will then turn the servo, else it will display a message on the LCD.

  UPDATE5: Haven't written an update log in a while - I've got the code working for everything: the red button, the LCD, and the coin reader. There is still a minor bug with the
  coin reader not registering some inputs (roughly 10% of the time). With the help of Kenneth, I diagnosed the broken LEDs (fixed two of the three, and we know that one has to be
  resoldered). So now, here's what we need to do: create better coils (or install more powerful servos), and make this thing look sleek and streamlined (I can cut out more panels
  and paint em black.) All code is uploaded via Google Drive - if anyone wants to look at my finished code, ask me or Ziegler, and you'll be granted access.

  UPDATE6: Idle, 32-frame animations are added. All servos are currently fixed (that one servo now has custom code to account for the irregularities in the potentiometer setting)
  and all coils are turned the same way. Idle animations will now work until the joystick is moved or if it receives a button press. Will work on getting it to be interrupted by
  coin input. For now we just need wall panels, and saran wrap on the front. /s
  We will also need the wiring diagram completed, and the physics sign to be finished. A lever cover would also be nice but we could honestly just cut a piece of plastic, though
  I feel like a 3D-printed cover would be way more slick.
*/
