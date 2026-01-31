#include <Arduino.h>
#include <Bluepad32.h> //Controller
#include <MPU9250_asukiaaa.h> //IMU LIB


// INTRODUCTION  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
// 
// An general pupse drone program for WEMOS D1 MINI ESP32
// by Mathew van Duijvenbode, @Kaasijs
// 
// DEPENDENSIES & CREDITS  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
//
// - Ricardo Quesada: The Bluepad32 library
// - My version of flix's: 3D Design
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
// # VARIBLES
// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

//PINS
int Motor0 = 14;  //L onder
int Motor1 = 13;  //R onder
int Motor2 = 15;  //L boven
int Motor3 = 12;  //R boven

//MOTORS
int Motors[4] = {Motor0, Motor1, Motor2, Motor3}; //All motors array
float MotorFQ = 0; //temp Motorspeed

//IMU
MPU9250_asukiaaa imu;
float gX, gY, gZ;

#define SDA_PIN 23
#define SCL_PIN 18

float rollOffset = 0.0; // Calibration offsets
float pitchOffset = 0.0; // Calibration offsets
float yawOffset = 0.0; // Calibration offsets

float rollCal  = 0.0; //Gecalculeerde waarde
float pitchCal = 0.0; //Gecalculeerde waarde

float roll = 0.0;
float pitch = 0.0;

float normalizeAngle(float angle) {
  while (angle > 180.0) angle -= 360.0;
  while (angle < -180.0) angle += 360.0;
  return angle;
}

//STATES
int state = 0;
  //0 = standby & flying
  //1 = test motors
  //2 = test imu
  //3 = start

// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
// # SETUPS
// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

//SETUP: motors
void setup_motors() {
  Serial.begin(115200);

  pinMode(Motor0, OUTPUT);
  pinMode(Motor1, OUTPUT);
  pinMode(Motor2, OUTPUT);
  pinMode(Motor3, OUTPUT);
}

void setup_imu() {
  Wire.begin(SDA_PIN, SCL_PIN);
  imu.setWire(&Wire);

  imu.beginAccel();
  imu.beginGyro();
  imu.beginMag();
  Serial.println("Sensor is verteld op testarten!");

  delay(200);
  calibrateOrientation();
  Serial.println("Setup calibration");
}

void setup() {
  state = 3; //Start automaticly

  setup_controller(); //Setup Controller first
  
  setup_motors();

  setup_imu();

  delay(1000);
}

// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
// # LOOPS: Main
// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

void loop() {
  commands();

  loop_moters();
  loop_IMU();

  if (state == 1) {
    loop_testing_motors();
  }
  if (state == 2) {
    loop_testing_IMU(); //Print out data
    loop_controller();
  }
  if (state == 3) {
    loop_controller();
  }

}

float SmoothMotorFQ = 0;
const float alpha = 0.22;

float LO = 1;
float RO = 1;
float LB = 1;
float RB = 1;

void loop_IMU(){
  if (state == 4) {
    roll = 0;
    pitch = 0;
    rollCal = 0;
    pitchCal = 0;
    return;
  }
  imu.accelUpdate();
  imu.gyroUpdate();
  imu.magUpdate();

  //Raw accelerometer data
  float ax = imu.accelX();
  float ay = imu.accelY();
  float az = imu.accelZ();

  //pitch and roll
  roll  = atan2(ay, az);
  pitch = atan2(-ax, sqrt(ay * ay + az * az));

  //2pi naar 360 graden
  roll  = roll * 180.0 / PI;
  pitch = pitch * 180.0 / PI;

  //calibration offsets
  rollCal  = normalizeAngle(roll  - rollOffset);
  pitchCal = normalizeAngle(pitch - pitchOffset);
  return;
}

//roll pitch correction
float UpAngle(float value){
  if (value > 0.0){
    return value;
  }
  else{
    return 0.0;
  }
}
float DownAngle(float value){
  if (value < 0.0){
    return value;
  }
  else{
    return 0.0;
  }
}

float AngelDifrance = 360+6; //Totalen rotatie + fout marge
float AngelDevide = 360; //Totalen rotatie
float multi = 3; //agrisifitiet
float macht = 2; //agrisifitiet


float SMOOTH_ROLL_UP     = 0.0;
float SMOOTH_ROLL_DOWN   = 0.0;
float SMOOTH_PITCH_UP    = 0.0;
float SMOOTH_PITCH_DOWN  = 0.0;

const float IMU_SMOOTHING_ALPHA = 0.9; // 0.9 = strong smoothing

void loop_moters(){
  //Calculate apropiate motor speed
  float IMU_ROLL_UP    = constrain( ( AngelDifrance - UpAngle(rollCal)    * multi * 1 ) /AngelDevide , 0 ,1 );
  float IMU_ROLL_DOWN  = constrain( ( AngelDifrance - DownAngle(rollCal)  * multi *-1 ) /AngelDevide , 0 ,1 );
  float IMU_PITCH_UP   = constrain( ( AngelDifrance - UpAngle(pitchCal)   * multi * 1 ) /AngelDevide , 0 ,1 );
  float IMU_PITCH_DOWN = constrain( ( AngelDifrance - DownAngle(pitchCal) * multi *-1 ) /AngelDevide , 0 ,1 );

  SMOOTH_ROLL_UP     = (IMU_SMOOTHING_ALPHA * SMOOTH_ROLL_UP)    + ((1.0 - IMU_SMOOTHING_ALPHA) * IMU_ROLL_UP)    ;
  SMOOTH_ROLL_DOWN   = (IMU_SMOOTHING_ALPHA * SMOOTH_ROLL_DOWN)  + ((1.0 - IMU_SMOOTHING_ALPHA) * IMU_ROLL_DOWN)  ;
  SMOOTH_PITCH_UP    = (IMU_SMOOTHING_ALPHA * SMOOTH_PITCH_UP)   + ((1.0 - IMU_SMOOTHING_ALPHA) * IMU_PITCH_UP)   ;
  SMOOTH_PITCH_DOWN  = (IMU_SMOOTHING_ALPHA * SMOOTH_PITCH_DOWN) + ((1.0 - IMU_SMOOTHING_ALPHA) * IMU_PITCH_DOWN) ;

  //Smothout motorfunc
  SmoothMotorFQ += (MotorFQ - SmoothMotorFQ) * alpha;

  //Versuur motor singal
  analogWrite(Motor0, round(MotorFQ) * LO * ( pow(SMOOTH_ROLL_UP, macht)   * pow(SMOOTH_PITCH_UP, macht)   ));
  analogWrite(Motor1, round(MotorFQ) * RO * ( pow(SMOOTH_ROLL_UP, macht)   * pow(SMOOTH_PITCH_DOWN, macht) ));
  analogWrite(Motor2, round(MotorFQ) * LB * ( pow(SMOOTH_ROLL_DOWN, macht) * pow(SMOOTH_PITCH_DOWN, macht) ));
  analogWrite(Motor3, round(MotorFQ) * RB * ( pow(SMOOTH_ROLL_DOWN, macht) * pow(SMOOTH_PITCH_UP, macht)   ));
}

// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
// # LOOPS: Test
// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

void loop_testing_IMU(){
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("=== ORIENTATION ===");
    Serial.print("Raw Roll: ");  Serial.print(roll, 2);
    Serial.print("° | Calibrated: "); Serial.print(rollCal, 2); 
    Serial.print("° | Smooth UP: "); Serial.print(SMOOTH_ROLL_UP, 2); 
    Serial.print("° | Smooth DOWN: "); Serial.print(SMOOTH_ROLL_DOWN, 2); 
    Serial.println("x");

    Serial.print("Raw Pitch: "); Serial.print(pitch, 2);
    Serial.print("° | Calibrated: "); Serial.print(pitchCal, 2);
    Serial.print("° | Smooth UP: "); Serial.print(SMOOTH_PITCH_UP, 2); 
    Serial.print("° | Smooth DOWN: "); Serial.print(SMOOTH_PITCH_DOWN, 2); 
    Serial.println("x");

    delay(200);
}

void loop_testing_motors(){
}

// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
// # COMMANDS
// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

void commands() {
  if (Serial.available()){
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "/help") {
      Serial.println("--- Serial monitor cmd: Mathew van duijvenbode ---");
      Serial.println("/start");
      Serial.println("/stop");
      Serial.println("/test motors | tests the motors at full speed");
      Serial.println("/test imu | outputs imu data to serial monitor");
      Serial.println("/imu reset | calibrates/overides the imu's offset");
      Serial.println("/imu stop | stops it");
      delay(100);
    }

    if (input == "/test motors") {
      Serial.println("testing...");
      delay(100);

      //MotorFQ = 255;
      state = 1;

      analogWrite(Motor0,255);
      delay(1000);
      analogWrite(Motor0,0);

      analogWrite(Motor1,255);
      delay(1000);
      analogWrite(Motor1,0);

      analogWrite(Motor2,255);
      delay(1000);
      analogWrite(Motor2,0);

      analogWrite(Motor3,255);
      delay(1000);
      analogWrite(Motor0,255);
      analogWrite(Motor1,255);
      analogWrite(Motor2,255);
      analogWrite(Motor3,255);
      
      delay(1000);
      analogWrite(Motor0,0);
      analogWrite(Motor1,0);
      analogWrite(Motor2,0);
      analogWrite(Motor3,0);
    }

    if (input == "/test imu") {
      Serial.println("testing...");
      delay(100);

      MotorFQ = 0;
      state = 2;
    }

    if (input == "/imu reset") {
      Serial.println("calibrated imu...");
      delay(100);

      calibrateOrientation();

      MotorFQ = 0;
    }
    if (input == "/imu stop") {
      Serial.println("stopped imu...");
      delay(100);

      MotorFQ = 0;
      state = 4;

      roll = 0;
      pitch = 0;
    }

    if (input == "/start") {
      Serial.println("starting...");
      delay(100);

      MotorFQ = 0;
      state = 3;
    }

    if (input == "/stop") {
      Serial.println("ending...");
      delay(100);

      MotorFQ = 0;
      state = 0;
    }
  }
}

void calibrateOrientation(){
  imu.accelUpdate();
  float ax = imu.accelX();
  float ay = imu.accelY();
  float az = imu.accelZ();

  float roll  = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));

  rollOffset  = roll * 180.0 / PI;
  pitchOffset = pitch * 180.0 / PI;
  //yawOffset = yaw * 180.0 / PI;

  Serial.println("Calibration done!");
  Serial.print("New Roll Offset: "); Serial.println(rollOffset, 2);
  Serial.print("New Pitch Offset: "); Serial.println(pitchOffset, 2);
  Serial.println();
}

// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
// # CONTROLLER
// # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
      }
    }

    if (!foundEmptySlot) {
      Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

    if (!foundController) {
      Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

// ========= SEE CONTROLLER VALUES IN SERIAL MONITOR ========= //

void dumpGamepad(ControllerPtr ctl) {
  if (false) {
    Serial.printf(
    "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
    ctl->index(),        // Controller Index
    ctl->dpad(),         // D-pad
    ctl->buttons(),      // bitmask of pressed buttons
    ctl->axisX(),        // (-511 - 512) left X Axis
    ctl->axisY(),        // (-511 - 512) left Y axis
    ctl->axisRX(),       // (-511 - 512) right X axis
    ctl->axisRY(),       // (-511 - 512) right Y axis
    ctl->brake(),        // (0 - 1023): brake button
    ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    ctl->gyroX(),        // Gyro X
    ctl->gyroY(),        // Gyro Y
    ctl->gyroZ(),        // Gyro Z
    ctl->accelX(),       // Accelerometer X
    ctl->accelY(),       // Accelerometer Y
    ctl->accelZ()        // Accelerometer Z
    );
  }
}

// ========= GAME CONTROLLER ACTIONS SECTION ========= //
float slowspeedmulti = 1;

void processGamepad(ControllerPtr ctl) {
  // There are different ways to query whether a button is pressed.
  // By query each button individually:
  //  a(), b(), x(), y(), l1(), etc...
 
  //== PS4 X button = 0x0001 ==//
  if (ctl->buttons() == 0x0001) {
    calibrateOrientation();
  }
  if (ctl->buttons() != 0x0001) {

  }

  //== PS4 Square button = 0x0004 ==//
  if (ctl->buttons() == 0x0004) {
    // code for when square button is pushed
  }
  if (ctl->buttons() != 0x0004) {
  // code for when square button is released
  }

  //== PS4 Triangle button = 0x0008 ==//
  if (ctl->buttons() == 0x0008) {
    // code for when triangle button is pushed
  }
  if (ctl->buttons() != 0x0008) {
    // code for when triangle button is released
  }

  //== PS4 Circle button = 0x0002 ==//
  if (ctl->buttons() == 0x0002) {
    // code for when circle button is pushed
  }
  if (ctl->buttons() != 0x0002) {
    // code for when circle button is released
  }

  //== PS4 Dpad UP button = 0x01 ==//
  if (ctl->buttons() == 0x01) {
    // code for when dpad up button is pushed
  }
  if (ctl->buttons() != 0x01) {
    // code for when dpad up button is released
  }

  //==PS4 Dpad DOWN button = 0x02==//
  if (ctl->buttons() == 0x02) {
    // code for when dpad down button is pushed
  }
  if (ctl->buttons() != 0x02) {
    // code for when dpad down button is released
  }

  //== PS4 Dpad LEFT button = 0x08 ==//
  if (ctl->buttons() == 0x08) {
    // code for when dpad left button is pushed
  }
  if (ctl->buttons() != 0x08) {
    // code for when dpad left button is released
  }

  //== PS4 Dpad RIGHT button = 0x04 ==//
  if (ctl->buttons() == 0x04) {
    // code for when dpad right button is pushed
  }
  if (ctl->buttons() != 0x04) {
    // code for when dpad right button is released
  }

  //== PS4 R1 trigger button = 0x0020 ==//
  if (ctl->buttons() == 0x0020) {
    // code for when R1 button is pushed
  }
  if (ctl->buttons() != 0x0020) {
    // code for when R1 button is released
  }

  //== PS4 R2 trigger button = 0x0080 ==//
  if (ctl->buttons() == 0x0080) {
    // code for when R2 button is pushed
  }
  if (ctl->buttons() != 0x0080) {
    // code for when R2 button is released
  }

  //== PS4 L1 trigger button = 0x0010 ==//
  if (ctl->buttons() == 0x0010) {
    // code for when L1 button is pushed
  }
  if (ctl->buttons() != 0x0010) {
    // code for when L1 button is released
  }

  //== PS4 L2 trigger button = 0x0040 ==//
  if (ctl->buttons() == 0x0040) {
    // code for when L2 button is pushed
  }
  if (ctl->buttons() != 0x0040) {
    // code for when L2 button is released
  }

  //== LEFT JOYSTICK - UP ==//
  if (ctl->axisY() <= -25) {
    float value = ctl->axisY();
    MotorFQ = ( (value*-1) / 512 ) *255*1;
    //Serial.println(MotorFQ);
    }
  else {
    MotorFQ = 0;
  }
  //== LEFT JOYSTICK - DOWN ==//
  if (ctl->axisY() >= 25) {
    //
  }

  //== LEFT JOYSTICK - LEFT ==//
  if (ctl->axisX() <= -25) {
    // code for when left joystick is pushed left
  }

  //== LEFT JOYSTICK - RIGHT ==//
  if (ctl->axisX() >= 25) {
    // code for when left joystick is pushed right
  }

  //== LEFT JOYSTICK DEADZONE ==//
  if (ctl->axisY() > -25 && ctl->axisY() < 25 && ctl->axisX() > -25 && ctl->axisX() < 25) {
    //
  }
  
  //== RIGHT JOYSTICK - X AXIS ==//
  if (ctl->axisRX() <= -25) {
    float value = ctl->axisRX();

    LO = (1 - (value/-512));
  }
  else {
    LO = 1;
  }

  if (ctl->axisRX() >= 25) {
    float value = ctl->axisRX();

    RO = (1 - (value/512));
  }
  else {
    RO = 1;
  }

  //== RIGHT JOYSTICK - Y AXIS (up) ==//
  if (ctl->axisRY() <= -25) {
    float value = ctl->axisRY();

    LB = (1 - (value/-512));
  }
  else {
    LB = 1;
  }

  if (ctl->axisRY() >= 25) {
    float value = ctl->axisRY();

    RB = (1 - (value/512));
  }
  else {
    RB = 1;
  }

  dumpGamepad(ctl);
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
         processGamepad(myController);
      }
      else {
        Serial.println("Unsupported controller");
      }
    }
  }
}

// Arduino setup function. Runs in CPU 1
void setup_controller() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But it might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();

  // Enables mouse / touchpad support for gamepads that support them.
  // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
  // - First one: the gamepad
  // - Second one, which is a "virtual device", is a mouse.
  // By default, it is disabled.
  BP32.enableVirtualDevice(false);
}

// Arduino loop function. Runs in CPU 1.
void loop_controller() {
  // This call fetches all the controllers' data.
  // Call this function in your main loop.
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    // vTaskDelay(1);
  delay(150);
}