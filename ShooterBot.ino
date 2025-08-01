#include <Servo.h>
#include <PS2X_lib.h>

#define PS2_DAT 46  // purple          5v: yellow
#define PS2_CMD 45  // red             ground: orange
#define PS2_ATT 44  // green
#define PS2_CLK 40  // blue

PS2X ps2x;
byte type = 0;
byte vibrate = 0;

int RX = 0, RY = 0, LX = 0, LY = 0;

Servo frontLeft;
Servo frontRight;
Servo backLeft;
Servo backRight;

Servo cannon;

int lastGoodValue = 127;
int hold = 3;
int framecount = 0;

int alpha = 0.1;
int smoothed = 0;

void setup() {
  Serial.begin(57600);

  ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, true, true);
  type = ps2x.readType();

  if (type == 1) {
    Serial.println("Dualshock controller found!");
    delay(1000);
  }

  frontLeft.attach(8);
  frontRight.attach(12);
  backLeft.attach(11);
  backRight.attach(10);
  cannon.attach(9);

  pinMode(8, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);

  frontLeft.writeMicroseconds(1500);
  frontRight.writeMicroseconds(1500);
  backLeft.writeMicroseconds(1500);
  backRight.writeMicroseconds(1500);
  cannon.writeMicroseconds(1500);
}

void loop() {
  ps2x.read_gamepad(false, vibrate);
 
  int Y = 0;
  int X = 0;
  int Z = 0;

  if (ps2x.Button(PSB_PAD_UP)) {
    Y = -100;
  } else if (ps2x.Button(PSB_PAD_LEFT)) {
    X = -100;
  } else if (ps2x.Button(PSB_PAD_RIGHT)) {
    X = 100;
  } else if (ps2x.Button(PSB_PAD_DOWN)) {
    Y = 100;
  }
  if (ps2x.Button(PSB_L2)) {
    Z = -100;
  } else if (ps2x.Button(PSB_R2)) {
    Z = 100;
  }

  Serial.println(Y);

  drive(Y, X, Z);
  
  if (ps2x.Button(PSB_GREEN)) {
    cannon.writeMicroseconds(1000);
  } else {
    cannon.writeMicroseconds(1500);
  }
}

void setFrontLeft(float power) {
  frontLeft.writeMicroseconds(map(power, -100, 100, 1700, 1300));
}
void setFrontRight(float power) {
  frontRight.writeMicroseconds(map(power, -100, 100, 1300, 1700));
}
void setBackLeft(float power) {
  backLeft.writeMicroseconds(map(power, -100, 100, 1700, 1300));
}
void setBackRight(float power) {
  backRight.writeMicroseconds(map(power, -100, 100, 1300, 1700));
}

void drive(float drive, float strafe, float turn) {
  float frontLeftPower = -drive + strafe + turn;
  float frontRightPower = -drive + strafe - turn;
  float backLeftPower = -drive - strafe + turn;
  float backRightPower = -drive - strafe - turn;

  setFrontLeft(frontLeftPower);
  setFrontRight(frontRightPower);
  setBackLeft(backLeftPower);
  setBackRight(backRightPower);
}

int filter(int newReading) {
  if (framecount < hold) {
    framecount++;
    return lastGoodValue;
  }

  if (newReading - lastGoodValue > delay) {
    lastGoodValue = newReading;

    return lastGoodValue;
  }

  lastGoodValue = newReading;

  return newReading;
}