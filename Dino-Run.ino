#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Arduino.h>

#define CLK   11
#define OE    9
#define LAT   10
#define A     A0
#define B     A1
#define C     A2
#define D     A3
#define JMP   12

/*
 * GLOBALS REQUIRED FOR SPRITECLASS
 */
static const uint8_t PROGMEM sDino[][30] = {
{0x01,0xf0 // run frame 1
,0x03,0x78
,0x03,0xf8
,0x03,0xf8
,0x03,0x80
,0x03,0xf8
,0x87,0x00
,0xcf,0xc0
,0xff,0x40
,0xff,0x00
,0x7e,0x00
,0x3c,0x00
,0x24,0x00
,0x34,0x00
,0x06,0x00},
{0x01,0xf0 // run frame 2
,0x03,0x78
,0x03,0xf8
,0x03,0xf8
,0x03,0x80
,0x03,0xf8
,0x87,0x00
,0xcf,0xc0
,0xff,0x40
,0xff,0x00
,0x7e,0x00
,0x3c,0x00
,0x24,0x00
,0x26,0x00
,0x30,0x00},
{0x00,0x00 // duck frame 1
,0x00,0x00
,0x00,0x00
,0x01,0xf0
,0x03,0x78
,0x03,0xf8
,0x87,0xf8
,0xcf,0x80
,0xff,0xf8
,0xff,0xc0
,0x7f,0x40
,0x3f,0x00
,0x24,0x00
,0x34,0x00
,0x06,0x00},
{0x00,0x00 // duck frame 2
,0x00,0x00
,0x00,0x00
,0x01,0xf0
,0x03,0x78
,0x03,0xf8
,0x87,0xf8
,0xcf,0x80
,0xff,0xf8
,0xff,0xc0
,0x7f,0x40
,0x3f,0x00
,0x24,0x00
,0x26,0x00
,0x30,0x00},
{0x01,0xf0 // jump frame
,0x03,0x78
,0x03,0xf8
,0x03,0xf8
,0x03,0x80
,0x03,0xf8
,0x87,0x20
,0xcf,0xc0
,0xff,0x00
,0xff,0x00
,0x7e,0x00
,0x3c,0x00
,0x24,0x00
,0x36,0x00
,0x00,0x00}
};
static const uint8_t PROGMEM sCact[] = {
0x20
,0xa0
,0xa8
,0xa8
,0xf8
,0x20
,0x20
,0x20
};
static const uint8_t PROGMEM sBird[][16] = {
{0x03,0x00 // frame 1
,0x07,0x00
,0x36,0x20
,0xff,0xc0
,0x1f,0x80
,0x07,0x00
,0x00,0x00
,0x00,0x00},
{0x00,0x00 // frame 2
,0x00,0x00
,0x33,0x20
,0xff,0xc0
,0x1f,0x80
,0x07,0x00
,0x00,0x00
,0x00,0x00},
{0x00,0x00 // frame 3
,0x00,0x00
,0x30,0x20
,0xff,0xc0
,0x1f,0x80
,0x0f,0x00
,0x06,0x00
,0x02,0x00}
};
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true);

/*
 * SPRITE CLASS AND FUNCTIONS
 */
class Sprite{
  int posX, posY, rectWidth, rectHeight, pR, pC, frame, cSprite;
  char sprite, state;
  bool flip;

  public:
  Sprite(int,int,int,int,char);
  spriteUpdatePos(int,int);
  spriteDisplay();
  setState(char);
  bool frameUpdate();
  char getState();
  int spriteAnimate();
  int getLeft();
  int getRight();
  int getBottom();
  int getTop();
  
};

Sprite::Sprite(int positionX, int positionY, int width, int height, char num){
  posX = positionX;
  posY = positionY;
  rectWidth = width;  
  rectHeight = height;
  pC = 0;
  pR = 0;
  sprite = num;
  cSprite = 0;
  state = 'a';
  flip = false;
  
  };

Sprite::spriteDisplay(){
  if(sprite == 'd'){
    matrix.drawBitmap(posX,posY,sDino[spriteAnimate()],rectWidth,rectHeight,255);
    }
  if(sprite == 'c'){
    matrix.drawBitmap(posX,posY,sCact,rectWidth,rectHeight,255);
    }
  if(sprite == 'b'){
    matrix.drawBitmap(posX,posY,sBird[spriteAnimate()],rectWidth,rectHeight,255);
    }
  };

Sprite::spriteUpdatePos(int x, int y){
  posX = x;
  posY = y;
  
  };

Sprite::setState(char s){
  state = s;
  flip = true;
  };

bool Sprite::frameUpdate(){
    frame = frame + 1;
    
    if(frame == 8){
      frame = 0;
      return true;
    };
    
    return false;
  };

char Sprite::getState(){
  return state;
  }

int Sprite::spriteAnimate(){
    // check for any state updates
  
     if(sprite == 'd'){   
        if(flip == true){
          if(cSprite == 0){
              cSprite = 2;
            }

           if(cSprite == 1){
              cSprite = 3;
            }

            if(cSprite == 2){
              cSprite = 0;
              }

            if(cSprite == 3){
              cSprite = 1;
            }
            
            flip = false;
            return cSprite;
          }
     }

    // check auto frame updates
    if(frameUpdate()){
          
      if(sprite == 'd'){
        
        if(state == 'a'){
          if(cSprite != 0){
            cSprite = 0;
            }else{
              cSprite = 1;
            }
          }
  
        if(state == 'b'){
          if(cSprite != 2){
            cSprite = 2;
            }else{
              cSprite = 3;
            }
          }
  
        if(state == 'c'){
          cSprite = 4;
          }
      
      }

      if(sprite == 'b'){
        if(cSprite == 2){
          flip = true;
        }
        if(cSprite == 0){
          flip = false;
        }

        if(flip == true){
          cSprite = cSprite - 1;}
        else{
          cSprite = cSprite + 1;
        }
        
          
      }
    }
    
    return cSprite;
  };

int Sprite::getLeft(){
  return posX;
  
  };

int Sprite::getRight(){
  return posX + rectWidth;
  
  };

int Sprite::getBottom(){
  return posY + rectHeight;
  
  };

int Sprite::getTop(){
  return posY;
  
  };

/*
 * GLOBALS FOR THE GAME & TEST FUNCTIONS
 */
int i = 0;
long tock = 0;
Sprite dino(0,0,13,15,'d');
Sprite cact(13,0,5,8,'c');
Sprite birb(18,0,11,8,'b');

class Game{
    int gSpeed, score, screenRate, highScore, height;
    bool isJumping, isDucking;

    public:
      Game();
      gameUpdate();
      displayAll();
      displayScore(int, int);
      checkInputs();
      jump();
      bool checkCollision();
      reset();
      bool tick();
  
  };

Game::Game(){
    gSpeed = 33;
    score = 0;
    highScore = 0;
    isJumping = false;
    isDucking = false;
  };

Game::checkInputs(){
  if(digitalRead(JMP) == 1 && isJumping == false){
      jump();
    }
  };

bool Game::tick(){
  if (millis() > tock * 33){
    tock++;
    return true;
  }
  return false;
};

Game::gameUpdate(){
  if(tick()){
    if(isJumping == true){
      height++;
      
      if(height > 100){
        height = 0;
        isJumping = false;
        }
      }
      
    displayAll();
    }
  };

Game::jump(){
    isJumping = true;
  }


Game::displayAll(){
  //animate
  if(isJumping == true && dino.getState() != 'c'){
      dino.setState('c');
      }
      
  if(isJumping == false && dino.getState() != 'a'){
      dino.setState('a');
      }
  
  //display
  matrix.fillScreen(matrix.Color333(0,0,0));
  birb.spriteDisplay();
  dino.spriteDisplay();
  cact.spriteDisplay();
  matrix.swapBuffers(false);
};



/*
 * TEST CODE FOR DISPLAYING SPRITES
 * Do not use this with the game class, they use the same variables.
 */
void spriteTest(){
    if (millis() > tock * 33){
      tock++;
      i++;
      if(i == 33){
        dino.setState('b');
        }
      if(i == 66){
        dino.setState('c');
        }
      if(i == 99){
        dino.setState('a');
        i = 0;
        }
      matrix.fillScreen(matrix.Color333(0,0,0));
      dino.spriteDisplay();
      cact.spriteDisplay();
      birb.spriteDisplay();
      }
}

/*
 * CREATE NEW GAME, SETUP AND RUN A LOOP
 */

Game dinorun;

void setup() {
    matrix.begin();
    pinMode(JMP, INPUT);
}

void loop() {
  dinorun.checkInputs();
  dinorun.gameUpdate();
}
