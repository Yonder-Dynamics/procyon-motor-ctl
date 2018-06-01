/*  
 *  MAE 156A - Interrupt Function for Measuring No-Load Speed
 *  Motor: Pololu Motor 3260
 *  Spec No-Load Speed: 5600 RPM
 *  For No-Load Speed
 *  
 *  Janaury 19th, 2017
 *  
 */
 
// Variables for motor and encoder operation
int encoder_input_A_pin = 3;       // Digital input pin
int encoder_input_B_pin = 43;       // Digital input pin
int potentiometer_input_pin = 0;        // Analog input pin
int cycles_per_rotation = 4320*3;
volatile int cycles = 0;
int input_analog = 0;
int direction_pin = 7;
int pwm_pin = 6;
int motor_value = 255; // 0 - 255
int motor_direction = 0;
// Variables for detecting encoder pulse and counting
int count = 0;
int input_encoder_A_prev = 0;  
int input_encoder_A_now = 0;
// Variables for timing
unsigned long int prev_time = 0;
unsigned long  int time_diff = 0;
unsigned long now = 0;
float encoder_pulse_frequency = 0;
volatile int a_last;
void setup() {
  Serial.begin(115200);              //serial output 
  pinMode(encoder_input_A_pin, INPUT);      // sets the digital pin as input
  pinMode(encoder_input_B_pin, INPUT);      // sets the digital pin as input
  // pinMode(potentiometer_input_pin, INPUT);      // sets the analog pin as input
  // pinMode(direction_pin, OUTPUT);       // Set the direction pin output
  // pinMode(pwm_pin, OUTPUT);       // Set the pwm pin output
  // digitalWrite(direction_pin, motor_direction);      // Set the motor in motion
  // analogWrite(pwm_pin, motor_value);      // Set the motor in motion
  attachInterrupt(digitalPinToInterrupt(encoder_input_A_pin),update_encoder_info,RISING);
  // attachInterrupt(digitalPinToInterrupt(encoder_input_B_pin),encoderB,CHANGE);
  
  a_last = digitalRead(encoder_input_A_pin);
  prev_time = micros();  
}
 
void loop() {
  // Serial.print(millis());           //time base for speed data
  // Serial.print(",");                //data separator character
  // Serial.println(count);            //counts for position and velocity calculation
  // Serial.print(",");              //data separator character
  // Serial.println(encoder_pulse_frequency, 2);         //time base for speed data
  // Serial.print(",");              //data separator character
  Serial.println(cycles / ((float)cycles_per_rotation) * 360,2);         //time base for speed data
  // Serial.println(cycles);         //time base for speed data
  // Serial.println(count);         //time base for speed data
  cycles = cycles%cycles_per_rotation;
}

void update_encoder_info(int step){
  int b = digitalRead(encoder_input_B_pin);
  if(!b){      //CW
    cycles++;
  } else {    //CCW
    cycles--;
  }
}