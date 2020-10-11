#include "simpletools.h"                      // Include simple tools 
#include "servo.h"                            // Include servo library

#define ButtonPin 6
#define ButtonPinB 7
#define TrigPin 10
#define EchoPin 11
#define PIRPin 5
#define ServoPin 15
#define PinA 14
#define PinB 13
#define Safedistance 200
#define LEDPin 1

int main()
{
  set_directions(14, 13, 0b00); // ensure pin 13 and 14 are all input pins
  set_direction(PIRPin, 0b0);
  set_directions(ButtonPinB, ButtonPin, 0b00);
  int pin_last = input(PinA);
  long duration;
  int distance;
  int flag = 0;
  int state = 0; // initial state
  servo_angle(ServoPin, 0); // servo initial pos
  int last_state = 0; 
  int last_state_b = 0; 
  int button = 0;
  int count = 0;
  int set_degree = 0;
  int re_counter = 0;
  int US_last = 0;
  while (1)
  {
    int PIR = input(PIRPin);
    int button_state = input(ButtonPin);
    int button_state_b = input(ButtonPinB);
    if (last_state_b == 0 && button_state_b == 1)
    {
      state = 0;
      low(LEDPin);
    }    
    last_state_b = button_state_b; 
    if (last_state == 0 && button_state == 1)
    {
      count = count + 1;
      button = 1;
      printf("Button state changed, button = %d\n", button); 
      printf("count = %d\n", count);
    }
    else
    {
      button = 0;
    }      
    last_state = button_state;  
    duration = ping(TrigPin,EchoPin);
    distance = duration/29/2;  
    if (distance > 2000)
    {
      distance = US_last;
    }         
    if (state != 0)
    {
      if (distance - US_last < 20)
      {
        PIR = 0;
      }        
    }
    print("PIR = %d\n", PIR);
    US_last = distance;   
    printf("%d\n", distance);  
   
    switch(state)
    {
      case 0:
      
      printf("initial state\n");
      if (count == 1)
      {
        printf("set the position of the mask\n");
        state = 1;
        high(LEDPin);
      }
      break;
              
      case 1:
      
      if (count == 1)
      {
        int aVal = input(PinA);
        if (aVal != pin_last)
        {
          printf("Updated!\n");
          if (aVal != input(PinB))
          {
            if (set_degree < 90)
            {
              re_counter = re_counter + 1;
              //printf("++%d\n", re_counter);
              set_degree = set_degree + 10; 
            }          
          }
          else
          {
            if (set_degree > 0)
            {
              re_counter = re_counter - 1;
              set_degree = set_degree - 10;
            }           
          }
          printf("%d\n", re_counter);
          servo_angle(ServoPin,10*(set_degree));
        } 
        pin_last = aVal;
      }
      else{
        count = 0;
        state = 2;
        servo_angle(ServoPin, 0);
        printf("state 0\n");
        //high(1);
        high(26);
        pause(3000);
        servo_disable(ServoPin);
      }                     
      break;
      
      case 2: //initial state 
       
      if (button == 1 && PIR == 0)
      {
        printf("Button pushed\n");
        servo_angle(ServoPin, set_degree*10);
        pause(1000); 
        servo_disable(ServoPin);
        flag = 1;
        state = 3;
        PIR = 0;
        printf("Mask On\n");
        low(26);
        high(27);
        pause(5000); 
      }  
      else if (button == 0 && PIR == 1)
      {          
        if (distance < Safedistance)
        {
          printf("Person Behind Detected, distance = %d\n", distance);
          servo_angle(ServoPin, set_degree*10);
          pause(1000); 
          servo_disable(ServoPin);
          state = 3;
          printf("Mask On\n"); 
          low(26);
          high(27);
          pause(5000);  
        }         
      }
      printf("%d\n", count); 
       
      break;
      
      case 3: // Mask off 
       
      if (PIR == 0 && button == 1)
      {
        printf("Button pressed to lift the mask\n");
        servo_angle(ServoPin, 0);
        pause(1000); 
        servo_disable(ServoPin);
        state = 2;
        PIR = 0;
        printf("Mask Off\n");
        flag = 0;
        low(27);
        high(26);
      }
      else if(PIR == 1 && distance > Safedistance && flag == 0)
      {
        servo_angle(ServoPin, 0); 
        pause(1000); 
        servo_disable(ServoPin);
        state = 2;
        printf("Mask Off\n");
        low(27);
        high(26);
      }
      printf("%d\n", count);;                   
      break;
    }     
  }    
}

int ping(int trig, int echo)//trig is trigger pin, echo is echo pin
{
  low(trig);//set trig low for start pulse
  low(echo);//set echo low to be safe
  pulse_out(trig, 10);//send the minimum 10 ms pulse on trig to start a ping
  return pulse_in(echo, 1);//get the pulse duration back on echo pin
}