//Greenhouse Management System by Aidan Makumbe RT 

#include <IRremote.h> // Responsible for interfacing with the IR Sensor VS1838B
#include "DHT.h" //Interfacing with Temperature and Humidity Sensor DHT11
#include <LiquidCrystal_I2C.h>//For interfacing with the LCD

//Defining pins and Variables
#define DHT_TYPE DHT11
const int DHT_PIN = 2;
const int moistureSensor = A0;
const int RELAY_PIN = 3;
const unsigned long BUTTON_CODE = 0x202F1329; //Put Activation code here 

const float maximumMoistureLevel = 156;
float currentMoistureLevel;
float moisture;

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
}

void loop(){
  int temp = dht.readTemperature();
  int hum = dht.readHumidity();

  currentMoistureLevel = analogRead(moistureSensor);
  moisture = currentMoistureLevel / maximumMoistureLevel;
  

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
  lcd.setCursor(0,0);
  lcd.print("Moist:");
  lcd.print(round(moisture*100));
  lcd.print("%");
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
  Serial.println("%");
  delay(2000);

}
