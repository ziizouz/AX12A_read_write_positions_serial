#include "Arduino.h"
#include "AX12A.h"

#define DirectionPin (2u)
#define BaudRate 1000000u
#define BaudRate_PC 9600
#define NUM_SERVOS 2
#define NUM_PARAMS 2
#define MAX_BUFF_SIZE 40
#define LED_P 13

const unsigned char ids[NUM_SERVOS] = {4u, 9u};
// const unsigned char ids[NUM_SERVOS] = {2u, 3u, 5u, 6u, 7u, 8u, 10u, 11u, 12u, 14u, 17u, 18u};
int pos = 0;
// Check whether you can print servo data in laptop

// Prototypes
void set_all_non_endless(void);
void print_initial_values(void);
void print_laptop(int val);
void send_from_laptop(int *params);

void setup() {
  // Configure pins
  pinMode(LED_P, OUTPUT);

  // Only needed once
  set_all_non_endless();

  // Always at start up, print all initial positions of all servos
  print_initial_values();

}

void loop() {

  int params[NUM_PARAMS];
  
  // Read config from user
  send_from_laptop(params);
  

  ax12a.begin(BaudRate, DirectionPin, &Serial); 
  ax12a.move((unsigned char) params[0], params[1]);
  delay(1000);
  
  ax12a.end();
  delay(1);
  print_initial_values();
  delay(1);
}

void print_laptop(int val){
  Serial.begin(BaudRate_PC);
  Serial.println(val, DEC);
  Serial.flush();
  Serial.end();
  return;
  }

void send_from_laptop(int *params){
  params[NUM_PARAMS] = {0};
  int i = 0;
  Serial.begin(BaudRate_PC);
  digitalWrite(DirectionPin, HIGH); // Flip directions-> Set servos to receive mode
  while(!Serial){;}
  Serial.println("Please enter ID and position");
  while(true)
    {
     if (Serial.available()>=4){
          params[i] = Serial.parseInt(); // ID, position, orientation
           i++;
        }

      if (i>=NUM_PARAMS)
        break;
    }
    Serial.println("Received data: ");
    Serial.println((unsigned char) params[0], HEX);
    Serial.println(params[1], DEC);
    Serial.println("\nEnd of reception!");
    
  digitalWrite(DirectionPin, LOW); // Reset directions
  Serial.end(); // 
  
  return;
  }

void set_all_non_endless(void){
  ax12a.begin(BaudRate, DirectionPin, &Serial);

  // Only needed once, the servos might be set to endless rotation movements initially
  for(unsigned char id : ids)
    ax12a.setEndless(id, OFF);  

  ax12a.end();
  return;
  }

void print_initial_values(void){
  int init_positions[NUM_SERVOS] = {0};
  char str[MAX_BUFF_SIZE] = "";
  
  ax12a.begin(BaudRate, DirectionPin, &Serial);
  
  for (int i =0; i<NUM_SERVOS; i++){
    init_positions[i] = ax12a.readPosition(ids[i]);
    delay(1);
    }

  ax12a.end();
  delay(1);
  
  Serial.begin(BaudRate_PC);
  while(!Serial){;}
  for(int i=0; i<NUM_SERVOS; i++){
    sprintf(str, "Servo ID: %d at position: %d", ids[i], init_positions[i]);
    Serial.println(str);
    }

  Serial.flush();
  Serial.end();
  
  return;
  }
