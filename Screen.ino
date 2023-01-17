#include <Wire.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <Arduino_FreeRTOS.h>

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0x8410
#define ORANGE  0xE880
#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define COLORBOXSIZE my_lcd.Get_Display_Width()/6
#define PENBOXSIZE my_lcd.Get_Display_Width()/4

Adafruit_GFX_Button Play_btn, Pause_btn,song_btn,gear_btn;
LCDWIKI_KBV my_lcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
const int XP = 8, XM = A2, YP = A3, YM = 9; //ID=0x9341
const int TS_LEFT = 130, TS_RT = 893, TS_TOP = 943, TS_BOT = 100;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

MCUFRIEND_kbv tft;
Adafruit_GFX_Button nextButton,previousButton,pausePlayButton;  
int pixel_x, pixel_y; //Touch getXY() updates global vars

int x = 0;
int play =1;
int songNumber=1;
char myString = songNumber;
int gears=1;
int test=0;
int c=1;
int lastGear=1;

void screenTask(void *pvParameters);

bool Touch_getXY(void){
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    {
      pixel_x = map(p.x, TS_LEFT, TS_RT, 0, 320); //.kbv makes sense to me  
      pixel_y = map(p.y, TS_TOP, TS_BOT, 0, 480);
      return true;   
    }
    return false;
}

void receiveEvent(int bytes) {
  c = Wire.read();
}

void setup() {
  Serial.begin(9600);
  wireSettings();
  draw();
  tft.fillRect(40, 80, 160, 80, GREEN);

  xTaskCreate(screenTask, "screen", 256, NULL, 1, NULL);

}

void screenTask(void *pvParameters){
  while (1){
    if(c!=lastGear){
      if(c==1){
        gears=1;
        lastGear=1;
        draw();
      }else if(c==2){
        gears=2;
        lastGear=2;
        draw();
      }else if(c==3){
        gears=3;
        lastGear=3;
        draw();
      }else if(c==4){
        gears=4;
        lastGear=4;
        draw();
      }
    // }
  }
  bool down = Touch_getXY();
    Play_btn.press(down && Play_btn.contains(pixel_x, pixel_y));
    Pause_btn.press(down && Pause_btn.contains(pixel_x, pixel_y));
    nextButton.press(down && nextButton.contains (pixel_x, pixel_y));
    previousButton.press(down && previousButton.contains(pixel_x, pixel_y));  
    if (Play_btn.justReleased())
        Play_btn.drawButton();
    if (Pause_btn.justReleased())
        Pause_btn.drawButton();
    if (Play_btn.justPressed()) {
        Play_btn.drawButton(true);
        play=1;
       // stop transmitting
        Wire.beginTransmission(9); // transmit to device #9
        Wire.write(4);              // sends x 
        Wire.endTransmission();    // stop transmitting
    
        draw();
    }
    if (Pause_btn.justPressed()) {
        Pause_btn.drawButton(true);
        play=0;
        Wire.beginTransmission(9); // transmit to device #9
        Wire.write(3);              // sends x 
        Wire.endTransmission();    // stop transmitting
    
        draw();
        }
    if(play==1){
    if (nextButton.justReleased()){
      
      nextButton.drawButton();
    }
       
    if (previousButton.justReleased()){
      previousButton.drawButton();
    }    
    if (nextButton.justPressed()) {
    
    nextButton.drawButton(true);
    Wire.beginTransmission(9); // transmit to device #9
    Wire.write(1);              // sends x 
    Wire.endTransmission();    // stop transmitting
    songNumber++;
    if(songNumber==5){
      songNumber=1;
    }
    draw();
   }
  if (previousButton.justPressed()&&songNumber!=1) {
    previousButton.drawButton(true);

    Wire.beginTransmission(9); // transmit to device #9
    Wire.write(2);              // sends x 
    Wire.endTransmission();    // stop transmitting
    songNumber--;
    if(songNumber==0){
      songNumber=4;
    }
    draw();
    }
  }
  }
}
void loop() {

}

void draw(){
  tft.begin(0x9486);
  tft.setRotation(0); //PORTRAIT
  
  tft.fillScreen(BLACK);
    nextButton.initButton(&tft, 180, 250, 100, 40, WHITE, CYAN, BLACK, "Next",2);
    previousButton.initButton(&tft, 60, 250, 100, 40, WHITE, CYAN, BLACK, "previous",2);
  nextButton.drawButton(false);
  previousButton.drawButton(false);
  Play_btn.initButton(&tft,  60, 200, 100, 40, WHITE, CYAN, BLACK, "Play", 2);
    Pause_btn.initButton(&tft, 180, 200, 100, 40, WHITE, CYAN, BLACK, "Pause", 2);
    Play_btn.drawButton(false);
    Pause_btn.drawButton(false);
    if(songNumber==1){
      song_btn.initButton(&tft, 100, 30, 100, 40, WHITE, CYAN, BLACK, "1", 2);
    }else if(songNumber==2){
      song_btn.initButton(&tft, 100, 30, 100, 40, WHITE, CYAN, BLACK, "2", 2);
    }else if(songNumber==3){
      song_btn.initButton(&tft, 100, 30, 100, 40, WHITE, CYAN, BLACK, "3", 2);
    }else if(songNumber==4){
      song_btn.initButton(&tft, 100, 30, 100, 40, WHITE, CYAN, BLACK, "4", 2);
    }
  song_btn.drawButton(false);
  if(gears==1){
      gear_btn.initButton(&tft, 100, 300, 100, 40, WHITE, CYAN, BLACK, "P", 2);
    }else if(gears==2){
      gear_btn.initButton(&tft, 100, 300, 100, 40, WHITE, CYAN, BLACK, "R", 2);
    }else if(gears==3){
      gear_btn.initButton(&tft, 100, 300, 100, 40, WHITE, CYAN, BLACK, "N", 2);
    }else if(gears==4){
      gear_btn.initButton(&tft, 100, 300, 100, 40, WHITE, CYAN, BLACK, "D", 2);
    }
  gear_btn.drawButton(false);
  if(play==1){
    tft.fillRect(40, 80, 160, 80, GREEN);
  }else{
    tft.fillRect(40, 80, 160, 80, RED);
  }
}


void wireSettings(){
  // Start the I2C Bus as Master
  Wire.begin(5);   
  Wire.onReceive(receiveEvent);
}