#include <Adafruit_GFX.h>   // LED matrix-specific library
#include <RGBmatrixPanel.h> // LED matrix-specific library
#include <Arduino.h>        // Arduino libraries

#define CLK   11
#define OE    9
#define LAT   10
#define A     A0
#define B     A1
#define C     A2
#define D     A3
#define JMP   12
#define DCK   13

/*
   GLOBALS REQUIRED FOR SPRITECLASS
   uint8_t arrays define the display characteristics of each sprite.
*/
static const uint8_t PROGMEM sDino[][30] = {
  { 0x01, 0xf0 // run frame 1
    , 0x03, 0x78
    , 0x03, 0xf8
    , 0x03, 0xf8
    , 0x03, 0x80
    , 0x03, 0xf8
    , 0x87, 0x00
    , 0xcf, 0xc0
    , 0xff, 0x40
    , 0xff, 0x00
    , 0x7e, 0x00
    , 0x3c, 0x00
    , 0x24, 0x00
    , 0x34, 0x00
    , 0x06, 0x00
  },
  { 0x01, 0xf0 // run frame 2
    , 0x03, 0x78
    , 0x03, 0xf8
    , 0x03, 0xf8
    , 0x03, 0x80
    , 0x03, 0xf8
    , 0x87, 0x00
    , 0xcf, 0xc0
    , 0xff, 0x40
    , 0xff, 0x00
    , 0x7e, 0x00
    , 0x3c, 0x00
    , 0x24, 0x00
    , 0x26, 0x00
    , 0x30, 0x00
  },
  { 0x00, 0x00 // duck frame 1
    , 0x00, 0x00
    , 0x00, 0x00
    , 0x01, 0xf0
    , 0x03, 0x78
    , 0x03, 0xf8
    , 0x87, 0xf8
    , 0xcf, 0x80
    , 0xff, 0xf8
    , 0xff, 0xc0
    , 0x7f, 0x40
    , 0x3f, 0x00
    , 0x24, 0x00
    , 0x34, 0x00
    , 0x06, 0x00
  },
  { 0x00, 0x00 // duck frame 2
    , 0x00, 0x00
    , 0x00, 0x00
    , 0x01, 0xf0
    , 0x03, 0x78
    , 0x03, 0xf8
    , 0x87, 0xf8
    , 0xcf, 0x80
    , 0xff, 0xf8
    , 0xff, 0xc0
    , 0x7f, 0x40
    , 0x3f, 0x00
    , 0x24, 0x00
    , 0x26, 0x00
    , 0x30, 0x00
  },
  { 0x01, 0xf0 // jump frame
    , 0x03, 0x78
    , 0x03, 0xf8
    , 0x03, 0xf8
    , 0x03, 0x80
    , 0x03, 0xf8
    , 0x87, 0x20
    , 0xcf, 0xc0
    , 0xff, 0x00
    , 0xff, 0x00
    , 0x7e, 0x00
    , 0x3c, 0x00
    , 0x24, 0x00
    , 0x36, 0x00
    , 0x00, 0x00
  }
};
static const uint8_t PROGMEM sCact[] = {
  0x20
  , 0xa0
  , 0xa8
  , 0xa8
  , 0xf8
  , 0x20
  , 0x20
  , 0x20
};
static const uint8_t PROGMEM sBird[][16] = {
  { 0x03, 0x00 // frame 1
    , 0x07, 0x00
    , 0x36, 0x20
    , 0xff, 0xc0
    , 0x1f, 0x80
    , 0x07, 0x00
    , 0x00, 0x00
    , 0x00, 0x00
  },
  { 0x00, 0x00 // frame 2
    , 0x00, 0x00
    , 0x33, 0x20
    , 0xff, 0xc0
    , 0x1f, 0x80
    , 0x07, 0x00
    , 0x00, 0x00
    , 0x00, 0x00
  },
  { 0x00, 0x00 // frame 3
    , 0x00, 0x00
    , 0x30, 0x20
    , 0xff, 0xc0
    , 0x1f, 0x80
    , 0x0f, 0x00
    , 0x06, 0x00
    , 0x02, 0x00
  }
};
// Initialise the LED matrix
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true);

/*
   SPRITE CLASS AND FUNCTIONS
*/
class Sprite {
    int posX, posY, rectWidth, rectHeight, pR, pC, frame, cSprite;
    char sprite, state;
    bool flip;

  public:
    Sprite(int, int, int, int, char);   // constructor for the sprite class
    updatePosition(int, int);           // positional update for the sprites location
    spriteDisplay();                    // method to display sprites on the LED matrix
    setState(char);                     // state characteristics for each sprite
    char getState();                    // get current state of sprite
    int getHeight();                    // get the height of the sprite 
    int getWidth();                     // get the width of the sprite
    int getLeft();                      // get the left side of the sprite
    int getRight();                     // get the right side of the sprite
    int getBottom();                    // get the bottom side of the sprite
    int getTop();                       // get the top side of the sprite

  private:
    bool frameUpdate();             // frame check for sprite animations
    int spriteAnimate();            // returns the frame that the sprite should be on, used in sprite display

};

Sprite::Sprite(int positionX, int positionY, int width, int height, char num) {
  posX = positionX;
  posY = positionY;
  rectWidth = width;
  rectHeight = height;
  pC = 0;
  pR = 0;
  sprite = num;           // used for determining which sprite to display
  cSprite = 0;            // used for checking frames
  state = 'a';            // used for checking the current state, used only for the dinosuar
  flip = false;           // used for updating the sprite relative to the current sprite when there is a state change

};

Sprite::spriteDisplay() {
  // check what the sprite is, and then draw the sprite on the matrix accordingly
  if (sprite == 'd') {
    matrix.drawBitmap(posX, posY, sDino[spriteAnimate()], rectWidth, rectHeight, 255);
  }
  if (sprite == 'c') {
    matrix.drawBitmap(posX, posY, sCact, rectWidth, rectHeight, 255);
  }
  if (sprite == 'b') {
    matrix.drawBitmap(posX, posY, sBird[spriteAnimate()], rectWidth, rectHeight, 255);
  }
};

Sprite::updatePosition(int x, int y) {
  posX = x;
  posY = y;

};

Sprite::setState(char s) {
  state = s;
  flip = true;
  spriteAnimate();
};

bool Sprite::frameUpdate() {
  // increment the frame value, when this function is called and this is the 8th frame, return true and update the sprite
  // doesn't have to be 8, can decrease if you want slower animation
  frame = frame + 1;

  if (frame == 8) {
    frame = 0;
    return true;
  };

  return false;
};

char Sprite::getState() {
  return state;
}

int Sprite::spriteAnimate() {
  // check for any state updates

  // instant updates for the dinosuar sprites, depending on what input is pressed. Changes based on the current state.
  if (sprite == 'd') {
    if (flip == true) {
      if (cSprite == 0) {
        cSprite = 2;
      }

      if (cSprite == 1) {
        cSprite = 3;
      }

      if (cSprite == 2) {
        cSprite = 0;
      }

      if (cSprite == 3) {
        cSprite = 1;
      }

      flip = false;
      return cSprite;
    }
  }

  // check if a frame update is required
  if (frameUpdate()) {

    // check which sprite this object contains
    if (sprite == 'd') {

      // check states, and update the sprite accordingly
      if (state == 'a') {
        if (cSprite != 0) {
          cSprite = 0;
        } else {
          cSprite = 1;
        }
      }

      if (state == 'b') {
        if (cSprite != 2) {
          cSprite = 2;
        } else {
          cSprite = 3;
        }
      }

      if (state == 'c') {
        cSprite = 4;
      }

    }

    // if the sprite is a birb, do birb stuff, flip is re-used to make use of a redundant variable otherwise.
    if (sprite == 'b') {
      if (cSprite == 2) {
        flip = true;
      }
      if (cSprite == 0) {
        flip = false;
      }

      if (flip == true) {
        cSprite = cSprite - 1;
      }
      else {
        cSprite = cSprite + 1;
      }


    }
  }

  // return what the sprite is currently supposed to be.
  return cSprite;
};

int Sprite::getHeight(){
  return rectHeight;  
};

int Sprite::getWidth(){
  return rectWidth;  
};

int Sprite::getLeft() {
  return posX;

};

int Sprite::getRight() {
  return posX + rectWidth;

};

int Sprite::getBottom() {
  return posY + rectHeight;

};

int Sprite::getTop() {
  return posY;

};

/*
   GLOBALS FOR THE GAME & TEST FUNCTIONS
*/
int i = 0;                          // used for a loop in the test class
long tock = 0;                      // used for timing, game will really mess up when this reaches the end
Sprite dino(0, 0, 13, 15, 'd');     // create a dinosuar sprite
Sprite cact(33, 33, 5, 8, 'c');      // create a cactus sprite
Sprite birb(33, 33, 11, 8, 'b');     // create a birb

class Game {
    int gSpeed, highScore, jumpHeight, jumpFrame, jumpDirection, 
        enemyCounter, enemyPos, jumpDelay, gDifficulty;                                   // game variables
    int dHit[6], bHit[3], cHit[3], fHit[2];                                               // Int arrays for hitboxes, because everything is traveling horizontally, we should only need three sides EXCEPT
                                                                                          //   for the dinosuar, which has multiple sides.
    bool isJumping, isDucking, enemyOnScreen, gOver;                                             // state check for dinosuar and enemies.
    long score, randDelay, randEnemy;

  public:
    Game();           // game constructor
    gameUpdate();     // update the game
    checkInputs();    // check what the current inputs are

  private:
    displayAll();             // display all the sprites on the matrix
    displayScore();           // TODO: display the current score on the matrix
    updateHitboxes();         // update the position of all hitboxes
    setDinoState();           // set the state of the dinosuar
    moveSprites();            // moves the sprites to their desired locations
    jump();                   // jump the dinosuar
    duck();                   // duck the dinosuar
    drawFloor();              // draw the game floor
    increaseSpeed();          // increase the speed of the game
    spawnEnemies();           // TODO: spawn enemies on the screen.
    gameOver();               // TODO: finish the game
    bool checkCollision();    // TODO: check for any collisions
    reset();                  // TODO: reset the game
    bool tick();              // virtual timing for the game
    hitboxDisplay();          // Displays hitboxes for testing purposes.

};

Game::Game() {
  randomSeed(analogRead(5));
  gSpeed = 33;            // set the game speed
  score = 1;              // score is 0 when the game starts
  isJumping = false;
  isDucking = false;
  jumpDirection = false;
  jumpHeight = 0;
  jumpDelay = 0;
  fHit[0] = 0;            // x pos of floor hitbox
  fHit[1] = 30;           // y pos of floor hitbox
  jumpFrame = -14;
  randDelay = random(75) + 45;
  enemyOnScreen = false;
  gDifficulty = 2;
  gOver = false;
  cact.updatePosition(33, 33);
  birb.updatePosition(33, 33);
  
};

Game::checkInputs() {
  // if the dinosuar is jumping, do nothing.
  if (isJumping == false) {
    // if the jump button is pressed, jump takes priority
    if (digitalRead(JMP) == 1) {
      jump();
    }
    if (digitalRead(DCK) == 1) {
      duck();
    }else if(isDucking != false){
      isDucking = false;
    }
  }
};

Game::updateHitboxes(){
  // The player will only be able to interact with the left and bottom of the birb
  bHit[0] = birb.getRight() - 3;
  bHit[1] = birb.getBottom() - 3;
  bHit[2] = birb.getLeft();

  // The player will only be able to interact with the left and top of the cactus
  cHit[0] = cact.getTop();
  cHit[1] = cact.getRight() - 1;
  cHit[2] = cact.getLeft();

  /* 
   *  The right side of the dinosinosuar will be the only collision, but we need to factor in the legs of the dino, which is a seperate hitbox smaller than the body.
   *  We also have to take into account whether the player is ducking, because the top hitbox will be smaller to avoid the birbs.
   *  Same with jumping, the bottom of the dinosuar is elevated by one pixel in the jumping animation.
   */
  
  if (isDucking && !isJumping){      // If the dinosuar is ducking, we want to change the height of the hitbox.
    dHit[0] = dino.getTop() + 3;     // Dinosuar ducks by 3 pixels, we add 3 because y moves downwards
  }else{
    dHit[0] = dino.getTop();  
  }

  dHit[1] = dino.getRight() - 1;
  dHit[2] = dHit[0] + 5;              // The bottom of the top half of the dinosuar COULD come into collision with the cactus, so we account for this.
  dHit[3] = dHit[1] - 5;
  
  if (isJumping){
    dHit[4] = dino.getBottom() - 2;   // When jumping the bottom of the dino is elevated by 1, so we need to account for this.
  }else{
    dHit[4] = dino.getBottom()-1;  
  }

  dHit[5] = dino.getLeft() + 2;
  
}

Game::increaseSpeed(){
  if (gSpeed != 1){
    gSpeed--;
  }
  gDifficulty++;
  score += gDifficulty;
  tock = millis() / gSpeed;
}

bool Game::tick() {
  if (millis() > tock * gSpeed) {
    tock++;
    return true;
  }
  return false;
};

Game::gameUpdate() {
  if (tick() && gOver == false) {
    setDinoState();
    spawnEnemies();
    moveSprites();
    updateHitboxes();
    //checkCollision();
    displayAll();
  }
  else{
    gameOver();  
  }
};

Game::gameOver(){
  highScore = score;
  delay(3000);
  
};

Game::jump() {
  isJumping = true;
};

Game::duck() {
  isDucking = true;
};

Game::drawFloor(){
  matrix.drawRect(fHit[0], fHit[1], 32, 2, 255);  
};

Game::moveSprites(){
  // check if we're jumping
  if(isJumping == true){
      // do some magic, should be a smooth jump
      if(jumpDirection == false){
        jumpHeight = ((jumpFrame*jumpFrame*jumpFrame)/2400)+8;
      }else{
        jumpHeight = ((jumpFrame*jumpFrame*jumpFrame)/-2400)+8;
      }
      

      if(jumpHeight >= 8 && jumpDirection == false){
        if(jumpDelay < 16){
          jumpHeight = 8;
          jumpDelay++;
        }else{
          jumpDelay = 0;
          jumpDirection = true;
        }
      }else{
        jumpFrame++;
      }

      // if we've landed change the jump to false, and reset the variables
      if (jumpHeight < 0){
        isJumping = false;
        jumpHeight = 0;
        jumpDirection = false;
        jumpFrame = -26;  
      }
  }

  // update position based on whether we're jumping, relative to the floor
  if(isJumping == false){
    dino.updatePosition(2, fHit[1] - dino.getHeight());
  }else{
    dino.updatePosition(2, fHit[1] - jumpHeight - dino.getHeight());
  }

  if(enemyOnScreen){
    enemyPos--;
    if(randEnemy > 49){
      birb.updatePosition(enemyPos/3, 10);
      
      if(birb.getRight() < 0){
        enemyOnScreen = false;  
        randDelay = random(75) + 45;
        increaseSpeed();
      }
      
    }else{
      cact.updatePosition(enemyPos/3, 22);
      
      if(cact.getRight() < 0){
        enemyOnScreen = false;  
        randDelay = random(75) + 45;
        increaseSpeed();
      }
      
    }
  }
  
};

Game::spawnEnemies(){
  if(!enemyOnScreen){
    enemyCounter++;
    
    if(enemyCounter >= randDelay){
      randEnemy = random(80);
      enemyPos = 96;
      enemyOnScreen = true;  
    }
    
  }  
};

Game::setDinoState(){
  // switch states depending on the players state and the current sprite state
  char state = dino.getState();
  if (isJumping == true && dino.getState() != 'c') {
    dino.setState('c');
  }

  if (isJumping == false && isDucking == true && dino.getState() != 'b') {
    dino.setState('b');
  }

  if (isJumping == false && isDucking == false && dino.getState() != 'a') {
    dino.setState('a');
  }
};

Game::displayScore(){
  matrix.setTextSize(1);
  matrix.setCursor(1,1);
  matrix.print(score);
};

Game::hitboxDisplay(){
  // dinosuar hitbox
  matrix.drawLine(dHit[3], dHit[0], dHit[1], dHit[0], matrix.Color333(3,1,0));
  matrix.drawLine(dHit[1], dHit[0], dHit[1], dHit[2], matrix.Color333(3,1,0));
  matrix.drawLine(dHit[1], dHit[2], dHit[3], dHit[2], matrix.Color333(3,1,0));
  matrix.drawLine(dHit[3], dHit[2], dHit[3], dHit[4], matrix.Color333(3,1,0));
  matrix.drawLine(dHit[3], dHit[4], dHit[5], dHit[4], matrix.Color333(3,1,0));

  // cactus hitbox
  matrix.drawLine(cHit[1], cHit[0], cHit[2], cHit[0], matrix.Color333(3,1,0));

  // birb hitbox
  matrix.drawLine(bHit[0], bHit[1], bHit[2], bHit[1], matrix.Color333(3,1,0));
}

Game::displayAll() {
  matrix.fillScreen(matrix.Color333(0, 0, 0));  // fill the matrix with black
  birb.spriteDisplay();                         
  dino.spriteDisplay();                         
  cact.spriteDisplay();
  displayScore();

  hitboxDisplay();
                        
  drawFloor();                                  
  matrix.swapBuffers(false);                    // swap the buffers over for that smooth, smooth animation
};

/*
   TEST CODE FOR DISPLAYING SPRITES
   Do not use this with the game class, they use the same variables.
*/
void spriteTest() {
  if (millis() > tock * (1000 / 30)) {
    tock++;
    i++;
    if (i == 33) {
      dino.setState('b');
    }
    if (i == 66) {
      dino.setState('c');
    }
    if (i == 99) {
      dino.setState('a');
      i = 0;
    }
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    dino.spriteDisplay();
    cact.spriteDisplay();
    birb.spriteDisplay();
  }
}

/*
   CREATE NEW GAME, SETUP AND RUN A LOOP
*/

Game dinorun;

void setup() {
  matrix.begin();
  pinMode(JMP, INPUT);
  pinMode(DCK, INPUT);
  Serial.begin(9600);
}

void loop() {
  dinorun.checkInputs();
  dinorun.gameUpdate();
}
