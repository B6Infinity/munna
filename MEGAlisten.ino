#define MotorLeftDir 4
#define MotorRightDir 2
#define MotorLeftSpeed A1
#define MotorRightSpeed A2


int incomingByte = 0;  // for incoming serial data
char c;

// char buff[30] = "";
String buff = "";
float lin;
int lin_i;
float ang;
int ang_i;
float seconds;
int leftSpeed, leftVal;
int rightSpeed, rightVal;

bool incomingCommand = false;
int recvd = 0;

void setup() {
  Serial.begin(9600);  // opens serial port, sets data rate to 9600 bps
  Serial.println("Started listenin'...");

  pinMode(MotorLeftDir, OUTPUT);
  pinMode(MotorRightDir, OUTPUT);
  pinMode(MotorLeftSpeed, OUTPUT);
  pinMode(MotorRightSpeed, OUTPUT);


  Serial.println("Giving a 255 kick for 100ms");

  analogWrite(MotorLeftSpeed, 255);
  analogWrite(MotorRightSpeed, 255);
  delay(100);
  analogWrite(MotorLeftSpeed, 0);
  analogWrite(MotorRightSpeed, 0);

  Serial.println("Starting loop...");

}

void move() {
  /*
  CONVENTION:
  The values of 'lin' and 'ang' should be between -2.0 and 2.0. Time maximum allowed should be 30 seconds.
  */

  // Process data
  lin = max(-2.0, min(2.0, lin));
  ang = max(-2.0, min(2.0, ang));

  // Integer values should range from -2000 to 2000;
  lin_i = lin * 1000;
  ang_i = ang * 1000;

  // Serial.println("HERE:");
  // Serial.println(lin_i);
  // Serial.println(ang_i);
  // Serial.println(seconds);


  // BIG BRAIN 🧠
  leftSpeed = lin_i - ang_i;
  rightSpeed = lin_i + ang_i;
  // BIG BRAIN END 🐮


  Serial.println("Calculated Speed Numbers:");
  Serial.println(leftSpeed);
  Serial.println(rightSpeed);

  leftVal = abs(map(abs(leftSpeed), 0, 4000, 0, 1023));
  rightVal = abs(map(abs(rightSpeed), 0, 4000, 0, 1023));

  Serial.println("Final Values:");
  Serial.println(leftVal);
  Serial.println(rightVal);

  if (leftSpeed > rightSpeed) {
    // Clockwise
    Serial.println("CLOCKWISE");
    digitalWrite(MotorLeftDir, HIGH);
    digitalWrite(MotorRightDir, LOW);


    if (leftVal < 130){
      leftVal = 130;
    }
  } else if (leftSpeed < rightSpeed) {
    // Anti Clockwise
    Serial.println("ANTI-CLOCKWISE");
    digitalWrite(MotorLeftDir, LOW);
    digitalWrite(MotorRightDir, HIGH);


    if (rightVal < 130){
      rightVal = 130;
    }

  } else if (leftSpeed < 0) {  // LINEAR only!
    // Backward
    digitalWrite(MotorLeftDir, LOW);
    digitalWrite(MotorRightDir, LOW);


    if (leftVal < 130){
      leftVal = 130;
    }
    if (rightVal < 130){
      rightVal = 130;
    }

  } else {
    // Forward
    digitalWrite(MotorLeftDir, HIGH);
    digitalWrite(MotorRightDir, HIGH);
  

    if (leftVal < 130){
      leftVal = 130;
    }
    if (rightVal < 130){
      rightVal = 130;
    }
  }

  // Motor wont run before 130 PWM value;
  analogWrite(MotorLeftSpeed, leftVal);
  analogWrite(MotorRightSpeed, rightVal);
  delay(seconds * 1000);



  analogWrite(MotorLeftSpeed, 0);
  analogWrite(MotorRightSpeed, 0);
  Serial.print("MOVED @ ");
  Serial.print(leftVal);
  Serial.println(leftVal);
}


void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();  // THIS IS ASCII VALUE of incoming string...
    c = incomingByte;

    if (c == '\n') {
      return;
    }

    // Serial.println(incomingByte, DEC);
    // Serial.println(c);


    if (incomingCommand) {
      if (c == ',') {
        // Trace and clear buff

        if (recvd == 0) {
          // Store in lin
          lin = buff.toFloat();
          recvd++;
        } else if (recvd == 1) {
          // Store in ang
          ang = buff.toFloat();
          recvd++;
        }

        buff = "";

      } else if (c == ']') {
        incomingCommand = false;
        // Final frisk

        // Store in seconds
        seconds = buff.toFloat();
        recvd = 0;

        buff = "";
        // ACT
        move();

      } else {
        // Numbers and dots
        buff += c;
      }
    }


    if (c == '[') {
      incomingCommand = true;
    }
  }
}