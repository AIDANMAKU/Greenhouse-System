//Greenhouse Management System by Aidan Makumbe RT 

#include <IRremote.h> // Responsible for interfacing with the IR Sensor VS1838B
#include "DHT.h" //Interfacing with Temperature and Humidity Sensor DHT11
#include <LiquidCrystal_I2C.h>//For interfacing with the LCD

//Defining pins and Variables
#define DHT_TYPE DHT11
const int DHT_PIN = 2;
const int RELAY_PIN = 3;
const int WATER_SENSOR_PIN = 5;  // Float switch pin
const int moistureSensor = A0;
const unsigned long BUTTON_CODE = 0x202F1329; //Put Activation code here 

const float maximumMoistureLevel = 156;
float currentMoistureLevel;
float moisture;
String state = ""; 



bool relayState = false;
bool waterOK = false;

const int RECV_PIN = 4;  // IR receiver data pin connected to D2
IRrecv irrecv(RECV_PIN);
decode_results results;
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x3F, 16, 2);


byte emptyDroplet[8] = {
  B00100,  //    ▓
  B00100,  //    ▓
  B01010,  //  ▓   ▓
  B10001,  // ▓     ▓
  B10001,  // ▓     ▓
  B10001,  // ▓     ▓
  B01110,  //   ▓▓▓   Empty 
  B00000   //
};

byte lowDroplet[8] = {
  B00100,  //    ▓
  B00100,  //    ▓
  B01010,  //   ▓ ▓
  B10001,  //  ▓   ▓
  B10001,  // ▓     ▓
  B10001,  // ▓     ▓
  B01110,  //   ▓▓▓
  B00100   //    ▓    Small low fill
};

byte mediumDroplet[8] = {
  B00100,  //   ▓
  B00100,  //   ▓
  B01010,  // ▓   ▓
  B10001,  //▓     ▓
  B10001,  //▓▓▓▓▓▓▓
  B01110,  // ▓▓▓▓▓ 
  B01110,  //   ▓     Mid fill line
  B00100
};

byte fullDroplet[8] = {
  B00100, //      ▓   
  B00100, //      ▓       
  B01110, //    ▓▓▓▓▓   
  B11111, //   ▓▓▓▓▓▓▓ 
  B11111, //  ▓▓▓▓▓▓▓▓▓
  B11111, //   ▓▓▓▓▓▓▓
  B01110, //    ▓▓▓▓▓
  B00100  //      ▓   Full Fill
};

void setup(){
  // Configuring I/O pins
  pinMode(moistureSensor, INPUT);
  pinMode( RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(WATER_SENSOR_PIN, INPUT_PULLUP); // Internal pull-up, active LOW



  // Starting up Essential Services
  Serial.begin(9600);
  dht.begin(); // Starting DHT services
  irrecv.enableIRIn(); // Enable IR Reciever

  // Initialize LCD
  lcd.init();         
  lcd.backlight();

  // Create custom characters in CGRAM (slots 0–3)
  lcd.createChar(0, emptyDroplet);
  lcd.createChar(1, lowDroplet);
  lcd.createChar(2, mediumDroplet);
  lcd.createChar(3, fullDroplet);
  
  lcd.setCursor(5, 0);
  lcd.print("Welcome");
  lcd.setCursor(7, 1);
  lcd.print("To");
  delay(2000);
  lcd.clear();

  lcd.setCursor(5, 0);
  lcd.print("Greenhouse");
  lcd.setCursor(3, 1);
  lcd.print("System");
  delay(2000);
  lcd.clear();

}
void waterplant(){

    if (waterOK) {
      relayState = !relayState;
      //digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
        //Activate Relay
      digitalWrite(RELAY_PIN, HIGH);

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Watering.");
      delay(1000);
      lcd.setCursor(0,0);
      lcd.print("Watering..");
      delay(1000);
      lcd.setCursor(0,0);
      lcd.print("Watering...");
      delay(2000);

      digitalWrite(RELAY_PIN, LOW);
      lcd.clear();
        //Serial.println(relayState ? "Relay ON" : "Relay OFF");
      } else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Cannot water...");
        lcd.setCursor(0,1);
        lcd.print("Water level low!");
        delay(3500);
        lcd.clear();
      }

}

void loop(){
  int temp = dht.readTemperature();
  int hum = dht.readHumidity();

  currentMoistureLevel = analogRead(moistureSensor);
  moisture = currentMoistureLevel / maximumMoistureLevel;
  
  waterOK = digitalRead(WATER_SENSOR_PIN) == LOW; // LOW = water present

  if(digitalRead(WATER_SENSOR_PIN) == LOW){
    waterOK = true;
    state = "F";
  }
  else{
    waterOK = false;
    state = "E";
  }

  if( moisture <= 0.1){
    //  Execute if the Moisture is below 10%
    lcd.setCursor(9,0);
    lcd.write(byte(0)); //Special Character for moisture 
    waterplant();
  }
  else if( moisture <= 0.3 && moisture >0.1 ){
    // Execute if the Moisture is between 10% and 30%
    lcd.setCursor(9,0);
    lcd.write(byte(1)); //Special Character for moisture 
    waterplant();
  }
  else if(moisture <= 0.6 && moisture > 0.3){
    // Execute if the Moisture is between 30% and 60%
    lcd.setCursor(9,0);
    lcd.write(byte(2)); //Special Character for moisture 
  }
  else{
    // Otherwise the moisture level is above 60% and therefore its good enough
    lcd.setCursor(9,0);
    lcd.write(byte(3)); //Special Character for moisture 
  }

  if (irrecv.decode(&results)) {
    if(results.value == BUTTON_CODE){
      waterplant();
    }
    irrecv.resume();  // Receive the next value
  }
  //Refreshing the LCD input
  
  //Row 1
  lcd.setCursor(0,0);
  lcd.print("Moist:");
  lcd.print(round(moisture*100));
  lcd.print("%");
  lcd.setCursor(11,0);
  lcd.print("Lvl:");
  lcd.print(state);


  //Row 2 of LCD
  lcd.setCursor(8,1);
  lcd.print("Temp:");
  lcd.print(temp);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Hum:");
  lcd.print(hum);
  lcd.print("%");


  //Output to Serial
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" ˚C | Humidity: ");
  Serial.print(hum);
  Serial.print(" % |");
  Serial.print(" Moisture: ");
  Serial.print(moisture*100);
  Serial.print("% |");
  Serial.print(" Water level: ");
  Serial.println(state);
  delay(2000);

}