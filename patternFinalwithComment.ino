/*
 * Controller code for Haptic Cushion
 * by KyodaiHaptics, Matsuno Lab, Kyoto University
 * 
 */


// map motor rows to controller pins
int m1 = 3;      // motor row 1
int m2 = 5; 
int m3 = 6; 
int m4 = 9; 
int m5 = 10; 
int m6 = 11;

// values used to regulate vibration magnitude
unsigned long val = 180; //limit at 102 for 9V sources, main row, 180 for 5V. Maximum 255.
unsigned long val2 = 50;

unsigned long delayTime = 120; //duration of a vibration within a pattern. i.e., duration in which vibration magnitude of each row is not changed
unsigned long interPatternDelay = 1000; // interval between complete patterns
unsigned long currrentTime;
unsigned long previousTime;

  
int state = 0;
int previousState = 0;
int stateChangeFlag = 0; 
// 0: no state change
// 1: some change occured

int incomingByte; 
//this value will be converted to state
/*  0 green, running
 *  1 red, running  
 *  2 green, stopped
 *  3 red, stopped
 *  others are ignored
 */

void setup()
{
  Serial.begin(9600);  // opens serial port, sets data rate to 9600 bps
  pinMode(m1, OUTPUT); // sets the pins connected to motor rows as output
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT);
  pinMode(m5, OUTPUT);
}

 
void preparePattern(int scale) //set background vibration
{
  analogWrite(m1, val2/scale);
  analogWrite(m2, val2/scale);
  analogWrite(m3, val2/scale);
  analogWrite(m4, val2/scale);
  analogWrite(m5, val2/scale);
}


void clearOutput() //terminate vibration
{
  analogWrite(m1,0);
  analogWrite(m2,0);
  analogWrite(m3,0);
  analogWrite(m4,0);
  analogWrite(m5,0);
}

void myDelayMs(unsigned long delaytime) 
//when this function is called, the the same set of value of vibration magnitude on each motor row will be maintained for <delaytime> millisecond
//state is updated at roughly 1000Hz
{
  if(stateChangeFlag) return;// come back to loop, start new pattern
  previousTime = millis();
  while((millis() - previousTime <= delaytime))  //wait until there is a state change or a certain period of time elapses
  {  
      int nextState = checkState(state);
      if(state!= nextState) //in the event of state change
      {
        state = nextState;
        stateChangeFlag = 1; //mark state change
        return; //exit (start new pattern)
      }
  }
}

void pattern2() // 5 to 1
{
  preparePattern(1); analogWrite(m5, val); myDelayMs(delayTime); clearOutput();
  preparePattern(1.1); analogWrite(m4, val-5); myDelayMs(delayTime); clearOutput();
  preparePattern(1.2); analogWrite(m3, val-10); myDelayMs(delayTime); clearOutput();
  preparePattern(1.3); analogWrite(m2, val-15); myDelayMs(delayTime); clearOutput();
  preparePattern(1.4); analogWrite(m1, val-20); myDelayMs(delayTime); clearOutput();
  if(stateChangeFlag) return;
  myDelayMs(interPatternDelay);
}

void pattern1() // 1 to 5
{
  preparePattern(1); analogWrite(m1, val); myDelayMs(delayTime); clearOutput();
  preparePattern(1.1); analogWrite(m2, val-5); myDelayMs(delayTime); clearOutput();
  preparePattern(1.2); analogWrite(m3, val-10); myDelayMs(delayTime); clearOutput();
  preparePattern(1.3); analogWrite(m4, val-25); myDelayMs(delayTime); clearOutput();
  preparePattern(1.4); analogWrite(m5, val-20); myDelayMs(delayTime); clearOutput();
  if(stateChangeFlag) return;
  myDelayMs(interPatternDelay);
}

int checkState(int prevState)
{ 
  int serialData;
  if (Serial.available()) {
     serialData = Serial.read(); 
    switch (serialData) {
      case '0': return 0; break;
      case '1': return 1; break;
      case '2': return 2; break;
      case '3': return 3; break;
      default:  return prevState; break;
    }
  }
  return prevState;
}


void loop()
{
  stateChangeFlag = 0;
  state = checkState(state); //check state and execute corresponding pattern
  
  switch(state)
  {
    case 0:  
    clearOutput();
    while(state == 0)
    {
       state = checkState(state);
       //Serial.println(state);
    }
    break;

    case 1: // green, not running
    while(state == 1)  // tell the driver to start
    {
      pattern1();
     // Serial.println(state);
    }
    break;

    case 2: // green, not running
    while(state == 2)  // tell the driver to start
    {
      pattern2();
      //Serial.println(state);
    }
    break;
    
    case 3:  
    clearOutput();
    while(state == 3)
    {
       state = checkState(state);
       //Serial.println(state);
    } 
    break;
    default: 
    break;
  }
}
