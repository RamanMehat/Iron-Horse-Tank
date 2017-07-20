// This code moves the tracks the object horizontal and vertical
#include <Servo.h>
Servo horizontalservo;
Servo verticalservo;

long duration;
long distance;

int currentHorizontalServoPos;
int currentVerticalServoPos;
int leftshiftcheck;
int rightshiftcheck;
int upshiftcheck;
int downshiftcheck;

int echoPinL = 49;
int trigPinL = 47;
int echoPinR = 33; 
int trigPinR = 31; 
int trigPinB = 37;
int echoPinB = 39;
int laser_din = 7;
int firepin = 4;


boolean left;
boolean right;
//boolean bottom;
boolean laserstatus;
boolean sensor;
boolean targetengaged;
boolean setInitialServoPos;
boolean automaticTrace;
boolean manualControl = true;
boolean trace;

byte lastRead;
byte currentRead;
byte StatusLED = 0;  // initialize LED in OFF state

void setup() {

  Serial.begin(74880);
  Serial1.begin(9600);
  pinMode(laser_din, INPUT);
  pinMode(echoPinL, INPUT);
  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinR, INPUT);
  pinMode(echoPinB, INPUT);
  pinMode(trigPinB, OUTPUT);
  pinMode(firepin, OUTPUT);
  horizontalservo.attach(11); // attaches to pin 12 of MEGA
  verticalservo.attach(12); // attaches to pin 13 of MEGA
  pinMode(A0, OUTPUT); // A0 and A1 connects to bottom of H-bridge.
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT); // A2 and A3 connects to top of H-bridge.
  pinMode(A3, OUTPUT);
}

void loop() {

  // if there's any serial bytes available, read them:
  //checkManualandAutomatic();

  setlaserstatus();
  if (setInitialServoPos == false) {
    horizontalservo.write(100);
    //verticalservo.write(100);
    setInitialServoPos = true;
    currentHorizontalServoPos = 100;
    //currentVerticalServoPos = 100;
  }
  if (trace == true) {
    enableTraceMode();
    trace = false;
  }

  if (automaticTrace == true) {
    while (Serial1.available() > 0) {
      currentRead = Serial1.read();

      // Read all bytes until a zero is received, store the bytes received before the zero.
      if (currentRead != NULL) {
        lastRead = currentRead;
      }

      if (currentRead != 8) {
        Serial1.write(StatusLED);
      }
      if (currentRead == 8) {
        automaticTrace = false;
        manualControl = true;
        bluetooth();
      }


      // Horizontal shift
      if (laserstatus == true) {
        if (left == true) {
          ServoShiftLeft();

        }
        if (right == true) {
          ServoShiftRight();
        }
      }
      /*
        // vertical shift
        if (laserstatus == true && bottom == false) {
          // move up
          ServoShiftUp();
        }
        if (laserstatus == false && bottom == true) {
          // move down
          ServoShiftDown();
        }
      */
    }
  }
  if (manualControl == true) {
    //manualServoControl();       ///////////////////////////////while or if loop
    bluetooth();
  }
  leftshiftcheck = currentHorizontalServoPos + 10;
  rightshiftcheck = currentHorizontalServoPos - 10;
  upshiftcheck = currentVerticalServoPos + 10;
  downshiftcheck = currentVerticalServoPos - 10;
  setandPrintAllSensorStatus();

}

// Functions
void setandPrintAllSensorStatus() {
  int dleft = finddistance(trigPinL, echoPinL);
  int dright = finddistance(trigPinR, echoPinR);
  //int dbottom = finddistance(trigPinB, echoPinB);

  Serial.print("Left distance:");
  Serial.print(dleft, DEC);
  setultrasoundstatus(dleft, 'L');
  Serial.print(" " + getSensorStatusString());
  Serial.print("  Right distance:");
  Serial.print(dright, DEC);
  setultrasoundstatus(dright, 'R');
  Serial.print(" " + getSensorStatusString());
  //Serial.print("  Bottom distance: ");
  // Serial.print(dbottom, DEC);
  //setultrasoundstatus(dbottom, 'B');
  //Serial.print(" " + getSensorStatusString());
  Serial.print("  Horizontal Servo Pos:");
  Serial.print(currentHorizontalServoPos, DEC);
  //Serial.print("  Vertical Servo Pos:");
  //Serial.print(currentVerticalServoPos, DEC);
  Serial.print("  laser Status:");
  Serial.println(getlaserStatusString());

}

long finddistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return distance = duration / 58; // inches
}

// set the ultrasound status
void setultrasoundstatus(int sensorFeed, char individualsensor) {
  if (sensorFeed < 100) {
    sensor = true;
    switch (individualsensor) {
      case'L':
        left = true;
        break;
      case'R':
        right = true;
        break;
        //case'B':
        // bottom = true;
        //break;
    }

  }
  else {
    sensor = false;
    switch (individualsensor) {
      case'L':
        left = false;
        break;
      case'R':
        right = false;
        break;
        //case'B':
        /// bottom = false;
        // break;
    }
  }
}
// set the laser status
void setlaserstatus() {
  if (digitalRead(laser_din) == LOW)
  {
    laserstatus = true;
    // Serial.println("Obstacles!");
  }
  else
  {
    laserstatus = false;
    //Serial.println("No Obstacles!");
  }
}
// chages the status of the ultrasound sensor
String getSensorStatusString() {
  if (sensor == true) {
    return ("true");
  }
  else {
    return ("false");
  }
}
String getlaserStatusString() {
  if (laserstatus == true) {
    return ("true");
  }
  else {
    return ("false");
  }
}

void ServoShiftLeft() {

  if (leftshiftcheck <= 180) {
    int leftPos;
    // Serial.println("servoshiftleft works!!");
    for (leftPos = currentHorizontalServoPos; leftPos <= currentHorizontalServoPos + 10; leftPos += 10) {
      horizontalservo.write(leftPos);
    }
    currentHorizontalServoPos = leftPos;
  }

}

void ServoShiftRight() {
  if (rightshiftcheck >= 0) {
    //Serial.println("servoshiftright works!!");
    int rightPos;
    for (rightPos = currentHorizontalServoPos; rightPos >= currentHorizontalServoPos - 10; rightPos -= 10) {
      horizontalservo.write(rightPos);
    }
    currentHorizontalServoPos = rightPos;
  }

}

void ServoShiftUp() {
  Serial.println("servo up works 2");
  if (upshiftcheck <= 180) {
    int upPos;
    for (upPos = currentVerticalServoPos; upPos <= currentVerticalServoPos + 10; upPos += 10) {
      verticalservo.write(upPos);
    }
  }
}
void ServoShiftDown() {
  Serial.print("servo down works");
  if (downshiftcheck >= 100) {
    int downPos;
    for (downPos = currentVerticalServoPos; downPos >= currentVerticalServoPos - 10; downPos -= 10) {
      verticalservo.write(downPos);
    }
  }
}

void enableTraceMode() {
  int HTraceServoPos = 0;
  verticalservo.write(100);
  horizontalservo.write(0);
  currentHorizontalServoPos = HTraceServoPos;

  while (laserstatus == false) {
    followTracePattern();
  }
}

void followTracePattern() {
  int pos;
  for ( pos = 0; pos <= 180; pos += 10) { // horizontal tracing only.
    horizontalservo.write(pos);
    currentHorizontalServoPos = pos;
    setlaserstatus();
  }

  for (pos = 180; pos >= 0; pos -= 10) {
    horizontalservo.write(pos);
    currentHorizontalServoPos = pos;
    setlaserstatus();
  }
}

void motorstop() {
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
}

void ServoStop() {
  horizontalservo.write(currentHorizontalServoPos);
  verticalservo.write(currentVerticalServoPos);
}
void motorShiftLeft() {
  analogWrite(A0, 225);
  digitalWrite(A1, LOW);
  digitalWrite(A3, LOW);
  analogWrite(A2, 215);
}
void motorShiftRight() {
  analogWrite(A1, 225);
  digitalWrite(A0, LOW);
  digitalWrite(A2, LOW);
  analogWrite(A3, 215);
}

void bluetooth() {
  while (Serial1.available() > 0) {
    currentRead = Serial1.read();

    // Read all bytes until a zero is received, store the bytes received before the zero.
    if (currentRead != NULL) {
      lastRead = currentRead;
    }

    if (currentRead == NULL) {
      switch (lastRead) {
        case 255:
          Serial1.write(StatusLED);
          break;
        case 8:            // manualmode
          automaticTrace = false;
          manualControl = true;
          Serial1.write(StatusLED);
          break;
        case 18:         //automaticmode
          automaticTrace = true;
          manualControl = false;
          Serial1.write(StatusLED);
          break;
        case 35:                    // servoLeft
          Serial.println("servo left works1 ");
          ServoShiftLeft();
          Serial1.write(StatusLED);
          break;
        case 135:
          Serial.println("Servo right feedback works");
          ServoStop();
          Serial1.write(StatusLED);
          break;
        case 45:                    // servoRight
          ServoShiftRight();
          Serial1.write(StatusLED);
          break;
        case 145:
          ServoStop();
          Serial1.write(StatusLED);
          break;
        case 10:            // front
          analogWrite(A0, 225);
          digitalWrite(A1, LOW);
          digitalWrite(A2, LOW);
          analogWrite(A3, 215);
          Serial1.write(StatusLED);
          break;
        case 110:
          motorstop();
          Serial1.write(StatusLED);
          break;
        case 20:  // back
          analogWrite(A1, 225);
          digitalWrite(A0, LOW);
          digitalWrite(A3, LOW);
          analogWrite(A2, 215);
          Serial1.write(StatusLED);
          break;
        case 120:
          motorstop();
          Serial1.write(StatusLED);
          break;
        case 30:   // left
          analogWrite(A0, 225);
          digitalWrite(A1, LOW);
          digitalWrite(A3, LOW);
          analogWrite(A2, 215);
          Serial1.write(StatusLED);
          break;
        case 130:
          motorstop();
          Serial1.write(StatusLED);
          break;
        case 40: // right
          analogWrite(A1, 225);
          digitalWrite(A0, LOW);
          digitalWrite(A2, LOW);
          analogWrite(A3, 215);
          Serial1.write(StatusLED);
          break;
        case 140:
          motorstop();
          Serial1.write(StatusLED);
          break;
        case 69:
          fire();
          Serial1.write(StatusLED);
        case 79:
          releaseFire();
          Serial1.write(StatusLED);
        case 28:
          trace = true;
          Serial1.write(StatusLED);
      }
    }
  }
}
void fire() {
  digitalWrite(firepin, HIGH);
}
void releaseFire() {
  digitalWrite(firepin, LOW);
}
