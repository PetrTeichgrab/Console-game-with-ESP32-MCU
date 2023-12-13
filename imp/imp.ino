/************************************************************************

  Test of the Pmod JSTK2

*************************************************************************

  Description: Pmod_JSTK2
  The X and Y values are displayed (normalized or not) in the serial monitor as a table
  and the Pmod LED lights up in a different color
  when the buttons are active.

  Material
  1. Arduino Uno
  2. Pmod JSTK2

  Wiring
  Module<----------> Arduino
  VCC     to        3V3
  GND     to        GND
  SCK     to        13 (SCK)
  MISO    to        12 (MISO)
  MOSI    to        11 (MOSI)
  CS      to        10

************************************************************************/

#include <SPI.h> // call library
// #define NORMALIZE_XY  //comment out to get axis' in range of (0:1023) or leave it for (-1:1)
#define LED_time 3  //nr of seconds the LED remains lit for

#define CS 5 // CS pin
int posX = 5, posY = 5, score = 0;
int snackPosX = rand() % 20;
int snackPosY = rand() % 19;
int delayTime = 1000;
int width = 30;
int height = 20;

void JSTK2_read(float& X, float& Y, int& B) {
  byte* data = new byte[5];

  digitalWrite(CS, LOW); // activation of CS line
  delayMicroseconds(15); // see doc: wait 15us after activation of CS line
  for (int i = 0; i < 5; i++) { // get 5 bytes of data
    data[i] = SPI.transfer(0);
    delayMicroseconds(10); // see doc: wait 10us after sending each data
  }
  digitalWrite(CS, HIGH); // deactivation of CS line
  delay(10);

  X = (data[1] << 8) | data[0]; //recunstruct 10-bit X value
  Y = (data[3] << 8) | data[2]; //recunstruct 10-bit Y value

  //if the keyword is defined, normalize axis' value
#ifdef NORMALIZE_XY
  X -= 512;
  X /= (512 * 1.0);
  Y -= 512;
  Y /= (512 * 1.0);
#endif

  B = (data[4] & 1) | (data[4] & 2); //0: no button, 1: trigger, 2: joystick
  if (B == 3) {                      //4: both
    B++;
  }
  return;
}

void setSnacks(){
  snackPosX = rand() % 20;
  snackPosY = rand() % 19;
  if(snackPosX == 0 || snackPosY == 0){
    setSnacks();
  } 
}

void reset(){
  score = 0;
  posX = 15;
  posY = 10;
  setSnacks();
  delayTime = 1000;
}

// Function to draw a boundary 
void draw() 
{
  printf("\n");
  printf("\n");
    for (int i = 0; i < height; i++) { 
        for (int j = 0; j < width; j++) { 
            if ((i == 0 || i == height - 1) && (j == 0 || j == width - 1)) {
                printf("+");
            } else if (i == 0 || i == height - 1) {
                printf("-");
            } else if (j == 0 || j == width - 1) {
                printf("|");
            }
            else { 
                if (i == posY && j == posX) 
                    printf("0"); 
                else if (i == snackPosY 
                         && j == snackPosX) 
                    printf("*"); 
                else
                    printf(" "); 
            }
        } 
        printf("\n"); 
    }
    printf("\n");
    printf("Score: %d", score); 
} 

void setup()
{
  pinMode(CS, OUTPUT);
  Serial.begin(921600); // initialization of serial communication
  SPI.begin(); // initialization of SPI port
  SPI.setDataMode(SPI_MODE0); // configuration of SPI communication in mode 0
  SPI.setClockDivider(SPI_CLOCK_DIV16); // configuration of clock at 1MHz
  reset();
}

void setPositions(float joystickPosX, float joystickPosY){
  joystickPosX+=63;
  if(joystickPosX > joystickPosY){
    if(joystickPosX > joystickPosY && joystickPosX > 580){
        posX++;
      }
      if(joystickPosY < joystickPosX && joystickPosY < 440){
        posY++;
    }
  }
  else{
    if(joystickPosX < joystickPosY && joystickPosX < 440){
      posX--;
    }
    if(joystickPosY > joystickPosX && joystickPosY > 580){
      posY--;
    }
  }
}

int controlGame(){
  if(posX == snackPosX && posY == snackPosY){
    score+=1;
    setSnacks();
    if(delayTime > 200){
      delayTime-=10;
    }
  }
  if(posX >= width || posX <= 0 || posY >= height || posY <= 0){
    return 1;
  }
  return 0;
}

void loop()
{
  //X: 470 Y: 533
  float joystickPosX, joystickPosY; //variables for X axis, Y axis and buttons
  int B;
  JSTK2_read(joystickPosX, joystickPosY, B); //read data from pmod (the function modifies the parameters)
  setPositions(joystickPosX, joystickPosY);
  int isGameOver = controlGame();
  draw();
  if(isGameOver){
    printf("GAME OVER");
  }
  delay(delayTime);
}