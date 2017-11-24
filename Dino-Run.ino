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
   uint8_t arrays define the characteristics of each sprite.
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
    spriteUpdatePos(int, int);          // positional update for the sprites location
    spriteDisplay();                    // method to display sprites on the LED matrix
    setState(char);                     // state characteristics for each sprite
    char getState();                    // get current state of sprite
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

Sprite::spriteUpdatePos(int x, int y) {
  posX = x;
  posY = y;

};

Sprite::setState(char s) {
  state = s;
  flip = true;
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

  // return what the sprite is supposed to be.
  return cSprite;
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
Sprite cact(13, 0, 5, 8, 'c');      // create a cactus sprite
Sprite birb(18, 0, 11, 8, 'b');     // create a birb

class Game {
    int gSpeed, score, screenRate, highScore, height;   // game variables
    int dBottomHit[2], dTopHit[2], bHit[2], cHit[2];    // Int arrays for hitboxes
    bool isJumping, isDucking;                          // state check for dinosuar.

  public:
    Game();           // game constructor
    gameUpdate();     // update the game
    checkInputs();    // check what the current inputs are

  private:
    displayAll();             // display all the sprites on the matrix
    displayScore(int, int);   // display the current score on the matrix
    updateHitboxes();         // update the position of all hitboxes
    setDinoState();           // set the state of the dinosuar
    moveSprites();            // move the sprites on the screen
    jump();                   // jump the dinosuar
    duck();                   // duck the dinosuar
    bool checkCollision();    // check for any collisions
    reset();                  // reset the game
    bool tick();              // virtual timing for the game

};

Game::Game() {
  gSpeed = 33;            // set the game speed, how many seconds before a game update
  score = 0;              // score is 0 when the game starts
  highScore = 0;          // hi-score is also 0, do not reset or you'll lose your stuff!
  isJumping = false;
  isDucking = false;

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
    }
  }
};

bool Game::tick() {
  if (millis() > tock * gSpeed) {
    tock++;
    return true;
  }
  return false;
};

Game::gameUpdate() {
  if (tick()) {
    setDinoState();
    displayAll();
  }
} ;

Game::jump() {
  isJumping = true;
};

Game::duck() {
  isDucking = true;
};

Game::setDinoState() {
  if (isJumping == true && dino.getState() != 'c') {
    dino.setState('c');
  }

  if (isJumping == false && isDucking == true && dino.getState() != 'b') {
    dino.setState('b');
  }

  if (isJumping == false && dino.getState() != 'a') {
    dino.setState('a');
  }
};

Game::displayAll() {
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  birb.spriteDisplay();
  dino.spriteDisplay();
  cact.spriteDisplay();
  matrix.swapBuffers(false);
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
}

void loop() {
  dinorun.checkInputs();
  dinorun.gameUpdate();
}
