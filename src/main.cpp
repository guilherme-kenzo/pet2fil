#include <Arduino.h>
#include <Stepper.h>
#include <FreeRTOS.h>

#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17
#define buttonPin 13

const int stepsPerRevolution = 4096/2;

Stepper stepper1(stepsPerRevolution, IN1, IN3, IN2, IN4);


bool buttonClicked = false;
int t1 = millis();

void buttonClick(){
  int t2 = millis();
  if ((t2 - t1) >= 50) {
    buttonClicked = digitalRead(buttonPin);
    delay(100);
  }
}

bool buttonStatus = false;
unsigned int initialTime = millis();
bool buttonUpandDown(){
    bool previousState = buttonStatus;
    buttonStatus = digitalRead(buttonPin);
    if (!buttonStatus && previousState) {
      return true;
    }
    else {
      return false;
    }
}

void setMotorSpeed(int speed){
  stepper1.setSpeed(speed);
}


TaskHandle_t xHandle = NULL;
bool currentButtonstate = false;
bool currentMotorState = true;
void buttonInterrupt(){
  bool previousValue = currentButtonstate;
  currentButtonstate = digitalRead(buttonPin);
  if (!currentButtonstate && previousValue) {
    delay(50);
    currentMotorState = !currentMotorState;
    Serial.println(currentMotorState);
    if (currentMotorState) {
      vTaskResume(xHandle);
    }
  }
}

void activateMotor(void * parameter) {
  for (;;) {
    Serial.println("running motor");
    stepper1.step(stepsPerRevolution/64);
    vTaskDelay(100/portTICK_PERIOD_MS);
    if (currentMotorState == 0) {
      Serial.println("Task suspended");
      vTaskSuspend(NULL);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Begining serial connection");
  setMotorSpeed(5);
  xTaskCreatePinnedToCore(
      activateMotor, // task function
      "Activate motor task", // Task name
      1024, // Stack size - this is more than enough for this function
      NULL, // params
      1, // Priority
      &xHandle, // Task Handle
      1 // Which core to use
  );
  configASSERT(xHandle);
}



void loop() {
  buttonInterrupt();
}

