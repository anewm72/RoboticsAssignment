#include <Servo.h>
#include <EEPROM.h> //imports EEPROM

#define LEFT_STOP 95 //defines left servo stop value
#define RIGHT_STOP 95 //defines right servo stop value

Servo leftServo;
Servo rightServo;

const int GREEN = 7;
const int YELLOW = 12;
const int RED = 13;
const int LEFT = 4;
const int RIGHT = 2;
const int IR_PIN = 3;
const int FREQUENCY = 38000;

int lightValues[3] = {655, 610, 686}; //global array for light averages
int darkValues[3] = {435, 442, 536}; //global array for dark averages
int greyValues[3] = {526, 509, 590}; //global array for grey averages

int lightSD[3] = {3.04795, 3.034798, 3.611094}; //global array for light standard deviation
int darkSD[3] = {3.666061, 3.001666, 5.640035}; //global array for dark standard deviation
int greySD[3] = {4.050926, 2.941088, 4.07431}; //global array for grey standard deviation

void setup() {
  Serial.begin(9600);
  pinMode(7, OUTPUT);   // setup green pin as output
  pinMode(12, OUTPUT);  // setup yellow pin as output
  pinMode(13, OUTPUT);  // setup red pin as output

  pinMode(LEFT, INPUT); //setup left remote button as input
  pinMode(RIGHT, INPUT); //setup right remote button as input

  pinMode(IR_PIN, OUTPUT);  // setup IR LED pin as output

  pinMode(A0, INPUT);  //setup LDR 0 as input
  pinMode(A1, INPUT); //setup LDR 1 as input
  pinMode(A2, INPUT); //setup LDR 2 as input

  leftServo.attach(5); //attach pin 5 to left servo
  rightServo.attach(6); //attach pin 6 to right servo

  /*
  int i = 0;
  int j = 0;
  for (i=0; i<3; i++) {
    Serial.print("LDR");
    Serial.println(i);
    for (j=0; j<10; j++) {
      Serial.println(analogRead(i));
    }
  }
  */
  /*
  //moveStraight(20);
  delay(1000);
  int initialNumber = 70;

  leftServo.write(LEFT_STOP);

  while (digitalRead(LEFT) == HIGH) {
    rightServo.write(initialNumber);
    leftServo.write(LEFT_STOP);
    delay(500);
    initialNumber = initialNumber + 1;
  }
  int rightNumber = initialNumber;
  Serial.println(rightNumber);

  // Code for finding out stop values (reusable)
  */
  
}

void waitKey(char pin) {  //Waits for a key to be pressed
  while (digitalRead(pin) == HIGH) {
  }
  delay(20);
  while (digitalRead(pin) == LOW) {
  }
  delay(20);
return;
}

bool keyState(char pin) {  //Checks the state of a key
  if (digitalRead(pin) == LOW) {
    delay(100);
    if (digitalRead(pin) == HIGH) {
      return true;
    }else {
      return false;
    }
  }
  else {
    return false;
  }
}

/*
String getKey() {
  if ((digitalRead(LEFT)== LOW) && (digitalRead(RIGHT)== LOW)) {
    return "GREY";
  }
  else if (digitalRead(LEFT) == LOW) {
    return "DARK";
  }
  else if (digitalRead(RIGHT) == LOW) {
    return "LIGHT"; 
  }
  else {
    return "NOKEY";
  }
}
*/

void setSpeed(signed int left_speed, signed int right_speed) {  //Sets the speed of the servos based on user input
  leftServo.write(LEFT_STOP - left_speed);
  rightServo.write(RIGHT_STOP + right_speed);  
}

void stop() {  //Sets the speed of both the servos to zero
  setSpeed(0, 0);
}

void stopKey(int pin) { //Waits for a key to be pressed
  while (digitalRead(pin) == HIGH) {    
  }
  delay(20);
  while (digitalRead(pin) == LOW){
  }
  delay(20);
return;
}

void turnAngle(int angle) {  //Turns the car a given degree
//13.7 milliseconds for one degree
  if (angle < 0) { 
    int angleTime = 13.7 * -(angle);
    setSpeed(20, -20);
    delay(angleTime);
    stop();
  }
  else {
    int angleTime = 13.7 * angle;
    setSpeed(-20, 20);
    delay(angleTime);
    stop();
  }
}

void moveStraight(int dist) {  //Moves the car straight for a given distance
//100 milliseconds for one centimetre
  if (dist < 0) {
    int moveTime = 100 * -(dist);
    setSpeed(-20, -20);
    delay(moveTime);
    stop();
  }
  else {
    int moveTime = 100 * dist;
    setSpeed(20, 20);
    delay(moveTime);
    stop();
  }
}

void calibrateServos(){  //Calibrates for each LDR, for any given light source.  Links to calibrateSensors()
  int calibrationComplete = 0;
  int calibrate = 0;
  while (calibrationComplete == 0) {  //if the calibration isn't complete the loop will run
    delay(500);
    int i = 0;
    Serial.println("What light source do you want to calibrate for?");
    Serial.println("1) Dark  2) Light  3) Grey");
    while (Serial.available() == 0) {
    }
    int choice = Serial.parseInt();  //user chooses from the menu
    String key_value = "";
    if (choice == 1) {
      key_value = "DARK";
    }else if (choice == 2) {
      key_value = "LIGHT";
    }else if (choice == 3) {
      key_value = "GREY";
    }else{
      key_value = "NOKEY";
    }
    for (i=0; i<3; i++) {
      Serial.println("");
      Serial.print("LDR ");
      Serial.print(i);
      Serial.println(":");
      if (key_value != "NOKEY") { //cannot pass to calibrateSensors() if there is no lighting environment
        calibrateSensors(key_value, i);    
      }
      delay(1000);
    }
    Serial.print(key_value);
    Serial.println(" calibration complete.");
    if (key_value != "NOKEY") {  //if the calibration isn't DARK, LIGHT, or GREY it will be ignored
      calibrate = calibrate + 1;
    }
    if (calibrate == 3) { //once the three calibrations are complete the calibration routine will end
      calibrationComplete = 1;
    }
  }
}


void calibrateSensors(String value, char ldr) { //Gets the mean and standard deviation of the LDR values for any given LDR and prints them to the serial monitor
  int sensorValues[10];
  int total = 0;
  int i = 0;
  for (i=0; i<10; i++) {  //Reads the LDR values 10 times and adds them all up
    delay(500);
    int ldrValue = analogRead(ldr);
    sensorValues[i] = ldrValue;
    total = total + sensorValues[i];
  }
  int average = total / 10; //Calculates the average
  int totalNum = 0;
  for (i=0; i<10; i++) {  //Takes every sensor value and takes away the average value from them all to calculate the standard deviation
    sensorValues[i] = (sensorValues[i] - average) * (sensorValues[i] - average);
    totalNum = totalNum + sensorValues[i];
  }
  int standardDeviation = sqrt(totalNum /10); //Final step to calculate standard deviation
  if (value == "DARK") {  //Prints the values if dark and adds mean and standard deviation to the global arrays
    Serial.println("DARK");
    darkValues[ldr] = average;
    darkSD[ldr] = standardDeviation;
  }
  else if (value == "LIGHT") { //Prints the values if light and adds mean and standard deviation to the global arrays
    Serial.println("LIGHT");
    lightValues[ldr] = average;
    lightSD[ldr] = standardDeviation;
  }
  else {
    Serial.println("GREY"); //Prints the values if grey and adds mean and standard deviation to the global arrays
    greyValues[ldr] = average;
    greySD[ldr] = standardDeviation;
  }
  Serial.print("Average: "); //Prints average and standard deviation
  Serial.println(average);
  Serial.print("Standard deviation: ");
  Serial.println(standardDeviation);
  Serial.println("");
}

void robotMode() { //Reads input from LDR and IR sensors and then makes decisions to move, turn or stop based on this input. Also determines if there is a sign (barcode) in front of it.
  Serial.println("Unplug your robot and then press the right button."); //Tells the user to unplug their robot so it can travel freely
  waitKey(RIGHT); //Waits for the right button to be pressed
  delay(500);
  tone(IR_PIN, FREQUENCY); //Now checking for obstacles
  delay(200);
  int time = 0; //Counts the time while robot on a bar
  int bars = 0; //Counts number of bars
  int timeBetweenBars = 0; //Counts the time between the first and second bar
  bool signStart = false; //Determines if robot is on a bar or not
  while (keyState(LEFT) == false) {
    if (digitalRead(RIGHT) == HIGH) {
      stop();
      int i = 0;
      bool i_values[3];
      for (i=0; i<3; i++) {   //Comparing the value read from analog against the grey average for the LDR + the standard deviation
        if (analogRead(i) <= (greyValues[i] + greySD[i])) {
          i_values[i] = true;
        } else {
          i_values[i] = false;
        }
      }
      digitalWrite(RED, LOW); //Red light turns on when there's an obstacle
      digitalWrite(YELLOW, LOW); //Yellow light turns on when everything is registered as false...ie. white
      digitalWrite(GREEN, LOW); //Green light turns on when the robot is turning due to a barcode
      if (i_values[0] == true){
        if (i_values[2] == true) {  //If the leftmost and rightmost LDRs are dark the robot is currently over a bar
          if (signStart == false){
            signStart = true;
          }
          setSpeed(10, 10); //less speed to give the robot time to register bars properly
          if (signStart == true) {
            time = time + 1; //time increases when on a bar
          }
          Serial.println(time);
          delay(100);
        }
        else { //if the leftmost LDR is dark then the robot will have to turn toward it
          setSpeed(-10, 10);
          delay(200);
        }
      }
      else if (i_values[2] == true) {  //if the rightmost LDR is dark then the robot will have to turn toward it
        setSpeed(10, -10);
        delay(200);
      } 
      else if (i_values[1] == true) { //if the middle LDR alone is dark then the robot can go straight forward
        setSpeed(10,10);   
        delay(200);
        if (bars == 2) { //if the number of bars are two then the robot can determine whether to go left or right
          if (timeBetweenBars <= 2) { //if the time is less than two between the first and second bars then there are three bars and the robot turns right
            digitalWrite(GREEN, HIGH);
            setSpeed(10, 10);
            delay(500);
            setSpeed(-10, 10);
            delay(500);
            bars = 0;
            Serial.println("RIGHT");
            bars = 0;
          }
          else {
            digitalWrite(GREEN, HIGH); //if the time is greater than two between the first and second bars then there are two bars and the robot turns left
            setSpeed(10, -10);
            delay(500);
            bars = 0;
            Serial.println("LEFT");
            bars = 0;
          }
          time = 0;  //resets the time to zero when the result of the sign is complete
          timeBetweenBars = 0; //resets the time between the first and second bar to zero when the result of the sign is complete
        }        
        if (signStart == false) {
          if (bars == 1) {
            timeBetweenBars = timeBetweenBars + 1; //if the robot is not going over a sign and there has been a bar already the time between bars will increase
            Serial.println(timeBetweenBars);                      
          }
        }
        if (signStart == true) {
          if (time > 1){  //precautionary to protect against false positives
            bars = bars + 1; //increases the number of bars
          }          
          signStart = false;
        }
        //Serial.println("Forward");   
      }
      else {
        digitalWrite(YELLOW, HIGH); //the robot believes that everything is light
        delay(200);
      }
    } else {
      digitalWrite(YELLOW, LOW);
      digitalWrite(RED, HIGH); //the robot is detecting an obstacle
      stop();
    }
    delay(100);
  }
  noTone(IR_PIN); //stops checking for obstacles as the user wishes to return to the menu
}

void loop() {  //Menu, uses switch...case statements to let the user switch between calibration and fully autonomous.  Deals with any errors.
  stop();
  Serial.println("1) Calibrate  2)Robot mode");
  while (Serial.available() == 0) {
  }
  int a[3][10]; //multidimensional array that stores all the 30 values read from the LDRs
  int i = 0;
  int j = 0;
  int total = 0;
  int average = 0;
  long totalNum = 0;
  float standardDeviation = 0;
  int choice = Serial.parseInt();
  switch (choice) {
    case 1: //activates calibration mode
      Serial.println("");
      Serial.println("---CALIBRATION MODE ACTIVATED---");
      calibrateServos(); 
      break;

    case 2: //activates robot mode (autonomous mode)
      Serial.println("");
      Serial.println("---ROBOT MODE ACTIVATED---");
      Serial.println("Initial calibrations..."); //starts Task 1
      moveStraight(10);
      moveStraight(-10);
      turnAngle(45);
      turnAngle(-45);
      for (i=0; i<10; i++) {
        for (j=0; j<3; j++) {        
          a[j][i] = analogRead(j); //adds every value to the multidimensional array
          total = total + a[j][i]; //adds everything to the total
        }
        tone(IR_PIN, FREQUENCY); //starts checking for obstacles
        delay(1000); //checks for an obstacle once per second (ten in ten seconds)
        Serial.print("Obstacle detected: ");
        if (digitalRead(RIGHT) == LOW) { //reads the IR sensor
          Serial.println("YES"); 
        } else {
          Serial.println("NO");
        }
        noTone(IR_PIN); //stops checking for obstacles
      }
      average = total / 30; //calculates average of all 30 values
      for (i=0; i<10; i++) { //calculating standard deviation as done in calibrateSensors()
        for (j=0; j<3; j++) {
          a[j][i] = (a[j][i] - average) * (a[j][i] - average);
          totalNum = totalNum + a[j][i];
        } 
      }
      standardDeviation = sqrt(totalNum / 30); //calculates standard deviation
      Serial.println("");
      Serial.print("Average light level: ");
      Serial.println(average); //prints average
      Serial.print("Standard deviation: ");
      Serial.println(standardDeviation); //prints standard deviation
      robotMode();
      break;

    default: //used if the user types anything other than 1 or 2
      Serial.println("");
      Serial.println("NO MODE SELECTED. PLEASE CHOOSE A MODE.");
      Serial.println("");
      break;
  }
}
