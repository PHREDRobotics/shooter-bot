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

float launch_windup = 0;
float launch_increment = 0.005;

void setup() {
  Serial.begin(57600);

  ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, true, true);
  type = ps2x.readType();

  if (type == 1) {
    Serial.println("Dualshock controller found!");
    delay(1000);
  }

  frontLeft.attach(13);
  frontRight.attach(12);
  backLeft.attach(11);
  backRight.attach(10);
  cannon.attach(9);

  frontLeft.writeMicroseconds(1500);
  frontRight.writeMicroseconds(1500);
  backLeft.writeMicroseconds(1500);
  backRight.writeMicroseconds(1500);
  cannon.writeMicroseconds(1500);
}

void loop() {
  ps2x.read_gamepad(false, vibrate);

  LY = ps2x.Analog(PSS_LY);
  LX = ps2x.Analog(PSS_LX);
  RY = ps2x.Analog(PSS_RY);
  RX = ps2x.Analog(PSS_RX);

  drive(LY, LX, RX);

  if (ps2x.Button(PSB_GREEN) && (launch_windup + launch_increment) < 1) {
    launch_windup += launch_increment;
  } else if (launch_windup > 0) {
    shoot(launch_windup);
    launch_windup = 0;
  }
}

void set(Servo motor, float power) {
  motor.writeMicroseconds(map(power, -1, 1, 1000, 2000));
}

void drive(float x, float y, float theta) {
  set(frontLeft, x + y + theta);
  set(frontRight, x - y - theta);
  set(backLeft, x - y + theta);
  set(backRight, x + y - theta);
}

void shoot(float power) {
  set(cannon, power);

  delay(1000);

  set(cannon, 0);
}
