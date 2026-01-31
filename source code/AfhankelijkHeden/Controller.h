#ifndef Controller_h
#define Controller_h

#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);

// ========= SEE CONTROLLER VALUES IN SERIAL MONITOR ========= //
void dumpGamepad(ControllerPtr ctl);
void processGamepad(ControllerPtr ctl);
void processControllers();

// Arduino setup/loop function. Runs in CPU 1
void setup_Controller();
void loop_Controller();

#endif