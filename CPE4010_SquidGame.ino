#include <Servo.h>
#include <Wire.h>

#define echoPin 2       // Echo Pin for ultrasound
#define trigPin 24      // Trigger Pin for ultrasound

int buttonpin = 22;
int buttonstate = 0;
int servo1pin = 9;
int servo2pin = 3;
Servo servo1;             //attached to ultrasonic
Servo servo2;             //swinging arm
int lcd_state = 6;
int gametimer = 60;       //use gametimer to count up to 60s
int led_red = 44;         //led pin
int led_green = 46;
int maximumRange = 50;    // Maximum range needed
int minimumRange = 0;     // Minimum range needed
long duration, distance;  // Duration used to calculate distance
int myangle;              // initialize angle variable
int pulsewidth;           // initialize width variable
int servo_val = 0;
int distance_val;
int buzzer = 42;
int playerposition;
bool gamewin = 0;

// set the IO pin for each segment
int a = 7;   // set digital pin 7 for segment a
int b = 6;   // set digital pin 6 for segment b
int c = 5;   // set digital pin 5 for segment c
int d = 10;  // set digital pin 10 for segment d
int e = 11;  // set digital pin 11 for segment e
int f = 8;   // set digital pin 8 for segment f
int g = 12;   // set digital pin 9 for segment g

void digital_0(void)  // display number 0
{
  unsigned char j;
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, LOW);
}
void digital_1(void)  // display number 1
{
  unsigned char j;
  digitalWrite(c, HIGH);     // set level as “high” for pin 5, turn on segment c
  digitalWrite(b, HIGH);     // turn on segment b
  for (j = 7; j <= 11; j++)  // turn off other segments
    digitalWrite(j, LOW);
}
void digital_2(void)  // display number 2
{
  unsigned char j;
  digitalWrite(b, HIGH);
  digitalWrite(a, HIGH);
  for (j = 9; j <= 11; j++)
    digitalWrite(j, HIGH);
  digitalWrite(c, LOW);
  digitalWrite(f, LOW);
}
void digital_3(void)  // display number 3
{
  digitalWrite(g, HIGH);
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(f, LOW);
  digitalWrite(e, LOW);
}
void digital_4(void)  // display number 4
{
  digitalWrite(c, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
  digitalWrite(a, LOW);
  digitalWrite(e, LOW);
  digitalWrite(d, LOW);
}
void digital_5(void)  // display number 5
{
  unsigned char j;
  digitalWrite(a, HIGH);
  digitalWrite(b, LOW);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}
void digital_6(void)  // display number 6
{
  unsigned char j;
  for (j = 7; j <= 11; j++)
    digitalWrite(j, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(b, LOW);
}


//FUNCTIONS FOR GAME

int movementChecker(int distance, int i) {
  //distance = measured distance from ultrasonic sensor
  //i is iterator from for loop
  if (playerposition == 0 && distance != -1) {
    //if no player position has been set & distance is valid reading, set the player's initial position
    playerposition = distance;
    Serial.print("playerposition = ");
    Serial.println(playerposition);
  } else if (distance <= 5 && distance != -1) {
    //player has won, end the game
      gametimer = 0;
      Serial.println("YOU WIN!!");
      gamewin = true;
      return 20;  // to escape for loop & end game
    } else if (distance != -1) {
    //if player's position changes by 5 or more, kill the player & end the game
    if (distance >= (playerposition + 4) || (playerposition - 4) >= distance) {
      gametimer = 0;
      Serial.println("PLAYER SENSED, KILL");
      return 20;  // to escape for loop & end game
    }
  }
  return i;  //to continue for loop onto next iteration
}

int measDist() {
  //check the distance from ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  return duration;
}

void countSeconds(int del, int red) {
  //use a function (countSeconds) to change 7-seg and read the distance sensor
  //while also having time elapse in the game via delay functions
  //arg "int del" is the amount of seconds you want to delay for
  for (int j = 1; j <= del; j++) {
    if (gametimer != 0) {
      int displayTime = (gametimer - 1) / 10 + 1;
      Serial.println(gametimer);
      //using switch statement, change the LCD to the proper reading
      switch (displayTime) {
        case 6:
          digital_6();
          break;
        case 5:
          digital_5();
          break;
        case 4:
          digital_4();
          break;
        case 3:
          digital_3();
          break;
        case 2:
          digital_2();
          break;
        case 1:
          digital_1();
          break;
        case 0:
          digital_0();
          break;
      }
      //decrement gametimer by 1sec and delay for 1sec thru sensor
      gametimer -= 1;

      //IF RED LIGHT CHECK DISTANCE WHILE COUNTING
      //ELSE (GREEN LIGHT) THEN DELAY FOR 1S
      if (red == 1) {
        playerposition = 0;
        int sensTime;
        for (int i = 0; i < 14; i++) {
          // check distance 14 times per second
          sensTime = measDist();
          distance = sensTime / 58.2;
          //make sure the distance sensed is within range
          if (distance >= maximumRange || distance <= minimumRange) {
            distance = -1;  // send negative number if reading isnt in range
          }

          i = movementChecker(distance, i);  // for if player is sensed, to skip to end of for loop & check movement
          delay(50);
        }
        Serial.print("Finished sensing for ");
        Serial.println(j);
      } else if (red == 0) {
        //ELSE IF GREEN, COUNT BUZZER FOR 1 SECOND
        delay(1000);
      }
    }
  }
}

void greenLight() {
  servo1.write(0);
  tone(buzzer, 432);
  Serial.println("Green Light!");
  digitalWrite(led_green, HIGH);
  digitalWrite(led_red, LOW);
  //if less than 2 seconds are left in the game, only count remaining time)
  //   else count for 2 seconds for green light
  if (gametimer - 2 <= 0) {
    countSeconds(gametimer, 0);
  } else {
    countSeconds(2, 0);
  }
}

void redLight() {
  servo1.write(180);
  delay(900);
  tone(buzzer, 369);
  Serial.println("Red Light!");
  digitalWrite(led_green, LOW);
  digitalWrite(led_red, HIGH);
  //if remaining time<3 secs, only count the remaining time
  //   else count 3 seconds for red light
  if (gametimer - 3 <= 0) {
    countSeconds(gametimer, 1);
  } else {
    countSeconds(3, 1);
  }
}

void gameReset() {
  digital_0();
  Serial.println("Game completed");
  //different tones from buzzer depending on win or loss
  if (gamewin == true){
    for(int i=700;i<1000;i+=50){
      tone(buzzer,i);
      digitalWrite(led_green, LOW);
      digitalWrite(led_red, HIGH);
      delay(250);
      digitalWrite(led_green, HIGH);
      digitalWrite(led_red, LOW);
      delay(250);
    }
    delay(3000);
    noTone(buzzer);
    gamewin = false;
  } else {
    servo2.write(180);
    tone(buzzer,75);
    delay(3000);
    servo2.write(90);
  }
  //reset LEDs & buzzer for next game
  digitalWrite(led_green, HIGH);
  digitalWrite(led_red, HIGH);
  noTone(buzzer);
}

void gameFlow() {
  gametimer = 60;
  Serial.println("Game is starting..");
  //loop through green and red light cycles until gametimer is 0
  do {
    greenLight();
    redLight();
  } while (gametimer > 0);
  //perform some cleanup after game ends
  gameReset();
}

void setup() {
  //setup serial monitor
  Serial.begin(9600);
  //setup lcd gpio
  for (int i = 4; i <= 11; i++) {
    pinMode(i, OUTPUT);  // set pin 4-11as “output”
  }
  //gpio config
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(buttonpin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(servo1pin, OUTPUT);
  pinMode(servo2pin, OUTPUT);
  servo2.attach(servo2pin);  //set servo2 pin
  servo1.attach(servo1pin);
  pinMode(buzzer, OUTPUT);

  Serial.println("Starting Button Reading");
  noTone(buzzer);
}


void loop() {
  //read button state forever while powered
  //if button is pressed, then start the game
  buttonstate = digitalRead(buttonpin);
  if (buttonstate == 1) {
    gameFlow();
  }
}
