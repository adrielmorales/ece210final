#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int PIN_VRX = A0;
int PIN_VRY = A1;
int PIN_SW  = 2;
int PIN_BUZZ = 8;

int gameDirections[100];
int level = 0;       
int score = 0;       
int bestScore = 0;   
int gameOver = 0;

void setup() {
  pinMode(PIN_BUZZ, OUTPUT);
  pinMode(PIN_SW, INPUT_PULLUP);
  pinMode(PIN_VRX, INPUT);
  pinMode(PIN_VRY, INPUT);

  Wire.begin();
  lcd.init();
  lcd.backlight();

  randomSeed(random(0, 299)); //neccessary for the random directions, without it it would lead to the same combination every restart

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Simon Says");
  lcd.setCursor(0, 1);
  lcd.print("Move to center");
}

void loop() {
  int xAxis, yAxis;

  // game over screen and a restart
  if (gameOver == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Over");
    lcd.setCursor(0, 1);
    lcd.print("Score:");
    lcd.print(score);
    lcd.print(" B:");
    lcd.print(bestScore);

    tone(PIN_BUZZ, 110, 700);
    delay(700);
    noTone(PIN_BUZZ);

    //wait until center button is pressed for new game
    while (digitalRead(PIN_SW) == HIGH) { //LOW = PRESSED
      delay(10);
    }

    // reset for new game
    level = 0;
    score = 0;
    gameOver = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("New Game");
    lcd.setCursor(0, 1);
    lcd.print("Center joystick");
    delay(1000);
  }

  if (level == 0) {
    while (1) {
     xAxis= analogRead(PIN_VRX);
     yAxis= analogRead(PIN_VRY);
      if (xAxis >= 400 && xAxis <= 624 && yAxis >= 400 && yAxis <= 624) { //joystick is centered
        break;
      }
      delay(10);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Starting...");
    delay(800);
  }

  // add one new random direction
  if (level < 100) {
    int randomDirection = random(0, 4);   // 0 = LEFT, 1 = RIGHT, 2 = UP, 3 = DOWN
    gameDirections[level] = randomDirection;
    level++;
  }

  // show level + best
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level:");
  lcd.print(level);
  lcd.setCursor(0, 1);
  lcd.print("Best:");
  lcd.print(bestScore);
  delay(600);

  // show the sequence
  for (int i = 0; i < level; i++) {
    int direction = gameDirections[i];

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WATCH");
    lcd.setCursor(0, 1);

    // print direction name
    if (direction == 0){
      lcd.print("LEFT ");
    }
    else if (direction == 1){
      lcd.print("RIGHT");
    }
    else if (direction == 2){
      lcd.print("UP   ");
    }
    else if (direction == 3){
      lcd.print("DOWN ");
    } 

    // beep direction
    int piezoFrequency = 0;
    if (direction == 0){
      piezoFrequency = 523;
    }
    if (direction == 1){
      piezoFrequency = 659;
    }
    if (direction == 2){
      piezoFrequency = 784;
    }
    if (direction == 3){
      piezoFrequency = 440;
    }

    if (piezoFrequency > 0) {
      tone(PIN_BUZZ, piezoFrequency, 400);
      delay(400);
      noTone(PIN_BUZZ);
    }

    delay(200);
  }

  // now gamer has to repeat
  for (int i = 0; i < level; i++) {
    int expected = gameDirections[i];

    // wait until gamer moves the joystick
    int joystickMove = 4; // none
    while (1) {
      xAxis = analogRead(PIN_VRX);
      yAxis = analogRead(PIN_VRY);

      if (xAxis < 200){
        joystickMove = 0; //LEFT
      }
      else if (xAxis > 800){
        joystickMove = 1; //RIGHT
      }     // RIGHT
      else if (yAxis < 200){
        joystickMove = 2; //UP
      }
      else if (yAxis > 800){
        joystickMove = 3;     // DOWN
      }
      else{
        joystickMove = 4; //No move
      } 

      if (joystickMove != 4){
        break;
      }
      delay(10);
    }

    // show what gamer did
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("YOU");
    lcd.setCursor(0, 1);

    if (joystickMove == 0){
      lcd.print("LEFT ");
    }
    else if (joystickMove == 1){
      lcd.print("RIGHT");
    }
    else if (joystickMove == 3){
      lcd.print("DOWN   ");
    }
    else if (joystickMove == 2){
      lcd.print("UP ");
    }

    // original beep like before
    int piezoFrequency2 = 0;
    if (joystickMove == 0){
      piezoFrequency2 = 523;
    }
    if (joystickMove == 1){
      piezoFrequency2 = 659;
    }
    if (joystickMove == 2){
      piezoFrequency2 = 784;
    }
    if (joystickMove == 3){
      piezoFrequency2 = 440;
    }
    
    if (piezoFrequency2 > 0) {
      tone(PIN_BUZZ, piezoFrequency2, 200);
      delay(200);
      noTone(PIN_BUZZ);
    }

    // wait until joystick is back in center
    while (1) {
     xAxis= analogRead(PIN_VRX);
     yAxis= analogRead(PIN_VRY);
      if (xAxis >= 400 && xAxis <= 624 && yAxis>= 400 && yAxis<= 624) break;
      delay(5);
    }
    delay(30);

    // check if wrong
    if (joystickMove != expected) {
      score = level - 1;
      if (score > bestScore) {
        bestScore = score;
      }
      gameOver = 1;
      break;
    }
  }

  if (gameOver == 0) { //correct
    score = level; // means that they beat this level
    if (score > bestScore) {
      bestScore = score;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Correct!");
    lcd.setCursor(0, 1);
    lcd.print("Score:");
    lcd.print(score);
    lcd.print(" B:");
    lcd.print(bestScore);

    tone(PIN_BUZZ, 1200, 100);
    delay(100);
    noTone(PIN_BUZZ);
    delay(400);
  }

  delay(300);
}
