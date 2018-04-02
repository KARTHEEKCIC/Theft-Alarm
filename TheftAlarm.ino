#include <Keypad.h>

#include<LiquidCrystal.h>

#include<EEPROM.h>

const int alarmPin = 7;

const int ldrPin = A0;

// for usage of timer calculation
unsigned long start_time, end_time, time_elapsed;

LiquidCrystal liquid_crystal_display(13,12,11,10,9,8);

char password[4];

char initial_password[4], new_password[4];

int i=0;

bool disabled = false;

bool first_time = true;

int state_index = 5;

int relay_pin = 10;

char key_pressed=0;

const byte rows = 4; 

const byte columns = 4; 

char hexaKeys[rows][columns] = {

{'1','2','3','A'},

{'4','5','6','B'},

{'7','8','9','C'},

{'*','0','#','D'}

};

byte row_pins[rows] = {A1,A2,A3,A4};

byte column_pins[columns] = {A5,3,4,5};   

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);




void setup()

{
  
  pinMode(alarmPin, OUTPUT);

  // pin for ldr
  pinMode(ldrPin, INPUT);

  pinMode(relay_pin, OUTPUT);

  liquid_crystal_display.begin(16,2);

  liquid_crystal_display.print("CIC Security");
  
  liquid_crystal_display.setCursor(0,1);

  liquid_crystal_display.print("Electronic Lock");

  delay(2000);

  // if state is 2 change it to the 1 that is options menu
  if(EEPROM.read(state_index) == 2) {
    EEPROM.write(state_index, 1);
  }

//  EEPROM.read()

  liquid_crystal_display.clear();

//  liquid_crystal_display.print(EEPROM.read(state_index));

//  delay(3000);

  if(EEPROM.read(state_index) == 0) {
  
//    liquid_crystal_display.print("Set Password : ");
//  
//    liquid_crystal_display.setCursor(0,1);

    change();

//    initialpassword();

    EEPROM.write(state_index, 1);
  }

  liquid_crystal_display.clear();

}




void loop()

{
   if(EEPROM.read(state_index) == 1) {
       liquid_crystal_display.clear();
     // options menu for the user
     liquid_crystal_display.print("Enter Choice : ");
     delay(2000);
     liquid_crystal_display.clear();
     liquid_crystal_display.print("1.Change Pass");
     liquid_crystal_display.setCursor(0,1);
     liquid_crystal_display.print("2.Enable Lock");
     while(1) {
       char key = keypad_key.getKey();
       if(key) {
        if(key == '1') {
          EEPROM.write(state_index, 2);
        } else if(key == '2') {
          liquid_crystal_display.clear();
          EEPROM.write(state_index, 3);
        } else {
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Wrong Choice !!");
          delay(3000);
        }
        break;
       }
     }   
  } else if(EEPROM.read(state_index) == 2) {
     // change the password
     if(check(false)){
      change();
      EEPROM.write(state_index,1);
     } else {
       liquid_crystal_display.clear();

       liquid_crystal_display.print("Wrong Password");

       liquid_crystal_display.setCursor(0,1);

       liquid_crystal_display.print("Try Again");

       delay(2000);
     }
  } else if(EEPROM.read(state_index) == 3) {
     // Enable the theft alarm
     disabled = false;
     if(first_time) {
      liquid_crystal_display.clear(); 
      liquid_crystal_display.print("Theft Alarm Enabled");

      liquid_crystal_display.setCursor(0,1);

      liquid_crystal_display.print("Press # to disable");
      first_time = false;
     }
       char key = keypad_key.getKey();
       
       if(key == '#') {
         // go back to the main page
         if(check(false)) {
          liquid_crystal_display.clear();
          liquid_crystal_display.print("Alarm Disabled");
          disabled = true;
          first_time = true;
          delay(2000);
          EEPROM.write(state_index, 1);
         } else {
            disabled = false;
            liquid_crystal_display.clear();

            liquid_crystal_display.print("Wrong Password");

            liquid_crystal_display.setCursor(0,1);

            liquid_crystal_display.print("Try Again");
         }
       }
        
       int ldrStatus = analogRead(ldrPin);

       if (ldrStatus <=100 && !disabled) {

        // wait for 30 seconds for the user to enter the password 
        // if doesn't start the buzzer and inform the owner

        liquid_crystal_display.clear();
        liquid_crystal_display.print("Alarm Activated");
        delay(2000);
        
        liquid_crystal_display.clear();
        liquid_crystal_display.print("Enter Password ");
        liquid_crystal_display.setCursor(0,1);
        liquid_crystal_display.print("to Deactivate");
        delay(2000);        

        start_time = millis();
        end_time = millis();
        time_elapsed = end_time - start_time;
        int count = 0;
        while(time_elapsed < 30000) {
          if(check(true) == true) {
            disabled = true;
            break;
          } else {
            if(time_elapsed > 30000) {
              disabled = false;
              break;
            }
            if(count==3) {
              disabled = false;
              break;
              // inform the user
            }
            count++;
            liquid_crystal_display.clear();

            liquid_crystal_display.print("Wrong Password");

            liquid_crystal_display.setCursor(0,1);

            liquid_crystal_display.print("Try Again");
          }
          end_time = millis();
          time_elapsed = end_time - start_time;
        }
        if(!disabled) {

          // thief entered. Act Now
          digitalWrite(alarmPin, HIGH);

          liquid_crystal_display.clear();
          liquid_crystal_display.print("Initiate");
          liquid_crystal_display.setCursor(0,1);
          liquid_crystal_display.print("Protocols");

          delay(5000);
        } else if(disabled) {
           // correct password entered 
           // disable the alarm
           liquid_crystal_display.clear();
           liquid_crystal_display.print("Alarm Disabled");
           digitalWrite(alarmPin, LOW);
           first_time = true;
           delay(3000);
           EEPROM.write(state_index, 1);
        }
        
        digitalWrite(alarmPin, LOW);
        first_time = true;
        EEPROM.write(state_index, 1);
        
       }
  
//       else {
//
//        digitalWrite(alarmPin, LOW);
//
//        Serial.println("---------------");
//
//       }
   
//       EEPROM.write(state_index, 1);
  }

}

void clearNewPassword() {
  int j=0;
  while(j<4) {
    new_password[j++] = -1;
  }
}

bool check(bool timer) {
  int j=0;

  liquid_crystal_display.clear();

  liquid_crystal_display.print("Current Password");

  liquid_crystal_display.setCursor(0,1);

  clearNewPassword();
  
  while(j<4)

  {

    if(timer == true) {
      end_time = millis();
      time_elapsed = end_time - start_time;
      if(time_elapsed > 30000) {
        break;
      }
    }
    
    char key=keypad_key.getKey();

    if(key)

    {

      new_password[j++]=key;

      liquid_crystal_display.print(key);

      liquid_crystal_display.setCursor(j-1,1);
      delay(200);
      liquid_crystal_display.print('*');


    }

    key=0;

  }
  initialpassword();
//  delay(500);

  return !strncmp(new_password, initial_password, 4);
  
}

void change()

{

    int j=0;

    liquid_crystal_display.clear();

    liquid_crystal_display.print("Set Password:");

    liquid_crystal_display.setCursor(0,1);

    while(j<4)

    {

      char key=keypad_key.getKey();

      if(key)

      {

        initial_password[j]=key;

        liquid_crystal_display.print(key);
        liquid_crystal_display.setCursor(j,1);
        delay(200);
        liquid_crystal_display.print('*');

        EEPROM.write(j,key);

        j++;

     

      }

    }
  
    delay(500);
     
    liquid_crystal_display.clear();
  
    liquid_crystal_display.print("Pass Changed");

    delay(1000);

//  }

}




void initialpassword(){
  
  for(int j=0;j<4;j++)

    initial_password[j]=EEPROM.read(j);

} 
