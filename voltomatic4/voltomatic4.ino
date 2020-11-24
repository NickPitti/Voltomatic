//Librerías
#include <DHT.h>
//#include <LiquidCrystal.h>//Libreria LCD no se usa por el shift register
#include <ShiftedLCD.h>
#include <SPI.h>
#include <Wire.h> // include Wire library code (needed for I2C protocol devices)
/*
 *The UNO board has 6 analog inputs, labeled A0 through A5.
Can also used as digital pins, A0=D14, A1=D15, A2=D16, A3=D17, A4=D18, A5=D19.
*/
//Definición de variables
#define LDR A1
#define DHTPIN A0 //NECESITA SER ANALOGO
#define DHTTYPE DHT11
#define RELAY1 9
#define RELAY2 2
int swmo=12;//pin switch
int swval=16;
int swvent=7;
int swlum=8;
int togmo;//variable para lectura de estado de switch modo manual/auto
int togval,toglum,togvent;// estado de switch valor,luminacion manual y ventilacion manual
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(0);

//
 
char Time[]     = "TIME:  :  :  ";
char Calendar[] = "DATE:  /  /20  ";
byte i, second, minute, hour, date, month, year;
 
void DS3231_display(){
  // Convert BCD to decimal
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour   = (hour >> 4)   * 10 + (hour & 0x0F);
  date   = (date >> 4)   * 10 + (date & 0x0F);
  month  = (month >> 4)  * 10 + (month & 0x0F);
  year   = (year >> 4)   * 10 + (year & 0x0F);
  // End conversion
  Time[12]     = second % 10 + 48;
  Time[11]     = second / 10 + 48;
  Time[9]      = minute % 10 + 48;
  Time[8]      = minute / 10 + 48;
  Time[6]      = hour   % 10 + 48;
  Time[5]      = hour   / 10 + 48;
  Calendar[14] = year   % 10 + 48;
  Calendar[13] = year   / 10 + 48;
  Calendar[9]  = month  % 10 + 48;
  Calendar[8]  = month  / 10 + 48;
  Calendar[6]  = date   % 10 + 48;
  Calendar[5]  = date   / 10 + 48;
  lcd.setCursor(0, 0);
  lcd.print(Time);                               // Display time
  lcd.setCursor(0, 1);
  lcd.print(Calendar);                           // Display calendar
}
void blink_parameter(){
  byte j = 0;
  while(j < 10 && digitalRead(5) && digitalRead(6)){
    j++;
    delay(10);
  }
}
byte edit(byte x, byte y, byte parameter){
  char text[3];
  while(!digitalRead(5));                        // Wait until button (pin #5) released
  while(true){
    while(!digitalRead(6)){                      // If button (pin #6) is pressed
      parameter++;
      if(i == 0 && parameter > 23)               // If hours > 23 ==> hours = 0
        parameter = 0;
      if(i == 1 && parameter > 59)               // If minutes > 59 ==> minutes = 0
        parameter = 0;
      if(i == 2 && parameter > 31)               // If date > 31 ==> date = 1
        parameter = 1;
      if(i == 3 && parameter > 12)               // If month > 12 ==> month = 1
        parameter = 1;
      if(i == 4 && parameter > 99)               // If year > 99 ==> year = 0
        parameter = 0;
      sprintf(text,"%02u", parameter);
      lcd.setCursor(x, y);
      lcd.print(text);
      delay(200);                                // Wait 200ms      /////**************
    }
    lcd.setCursor(x, y);
    lcd.print("  ");                             // Display two spaces
    blink_parameter();
    sprintf(text,"%02u", parameter);
    lcd.setCursor(x, y);
    lcd.print(text);
    blink_parameter();
    if(!digitalRead(5)){                         // If button (pin #5) is pressed
      i++;                                       // Increament 'i' for the next parameter
      return parameter;                          // Return parameter value and exit
    }
  }
}
 
//
  bool menu=true;
  float MinLum=120;//variable para calibracion de minima luminacion de respuesta en el menu de entrada
  float MaxTemp=25;//variable para calibracion de maxima temperatura de respuesta en el menu
  
void setup() {
  Wire.begin();   
  lcd.begin(20,4);// cantidad de columnas y filas del lcd 20x4
  Serial.begin(9600);
  dht.begin();
  pinMode(5, INPUT_PULLUP);                      // button1 is connected to pin 5
  pinMode(6, INPUT_PULLUP);                      // button2 is connected to pin 6
  
  pinMode(swmo,INPUT);
  pinMode(swval,INPUT);
  pinMode(swlum,INPUT);
  pinMode(swvent,INPUT);
  
  pinMode(LDR,OUTPUT); //photoresistor
  pinMode(3,OUTPUT); // led verde
  pinMode(4,OUTPUT); // led rojo
  /*
  pinMode(7,OUTPUT);//debe ser input 
  pinMode(8,OUTPUT);//antes eran botones
  */
  pinMode(RELAY1,OUTPUT);
  pinMode(RELAY2,OUTPUT);
  digitalWrite(RELAY1,LOW);
  digitalWrite(RELAY2,LOW);
  
  digitalWrite(3,HIGH);//verde parpadeo al encender
  delay(100);
  digitalWrite(3,LOW);//
  delay(100);
  digitalWrite(3,HIGH);
  delay(100);
  digitalWrite(3,LOW);
  delay(100);
  digitalWrite(3,HIGH);
  delay(100);
  digitalWrite(3,LOW);
}
 
void loop() {
  togmo=digitalRead(swmo); //SwitchPara salir de menu de calibracion y Toggle de Modo:Manual/Auto
  togval=digitalRead(swval); //Toggle de valor de Luz o Temperatura en MenuPrincipal y En Funcionamiento Normal
  if(menu==true){
    lcd.setCursor(0,0);  
    lcd.print("Menu Calibracion");
    if(togval==HIGH){
      lcd.setCursor(0,1);
      lcd.print("Min Lum");
      if(!digitalRead(5)){                           // If button (pin #5) is pressed
        MinLum--;
      }
      if(!digitalRead(6)){                           // If button (pin #6) is pressed
        MinLum++;
      }
      lcd.setCursor(0,2);
      lcd.print(MinLum);
    }else{
      lcd.setCursor(0,1);
      lcd.print("Max Temp");
      if(!digitalRead(5)){                           // If button (pin #5) is pressed
        MaxTemp--;
      }
      if(!digitalRead(6)){                           // If button (pin #6) is pressed
        MaxTemp++;
      }
      lcd.setCursor(0,2);
      lcd.print(MaxTemp);
    }  
    lcd.setCursor(0,3);  
    lcd.print("Salir Con Switch");    
    if(togmo==HIGH)    menu=false; //para salir de menu usar switch de modo
  }
    else{
    if(!digitalRead(5)){                           // If button (pin #5) is pressed
        i = 0;
        hour   = edit(5, 0, hour);
        minute = edit(8, 0, minute);
        date   = edit(5, 1, date);
        month  = edit(8, 1, month);
        year   = edit(13, 1, year);
        // Convert decimal to BCD
        minute = ((minute / 10) << 4) + (minute % 10);
        hour = ((hour / 10) << 4) + (hour % 10);
        date = ((date / 10) << 4) + (date % 10);
        month = ((month / 10) << 4) + (month % 10);
        year = ((year / 10) << 4) + (year % 10);
        // End conversion
        // Write data to DS3231 RTC
        Wire.beginTransmission(0x68);               // Start I2C protocol with DS3231 address
        Wire.write(0);                              // Send register address
        Wire.write(0);                              // Reset sesonds and start oscillator
        Wire.write(minute);                         // Write minute
        Wire.write(hour);                           // Write hour
        Wire.write(1);                              // Write day (not used)
        Wire.write(date);                           // Write date
        Wire.write(month);                          // Write month
        Wire.write(year);                           // Write year
        Wire.endTransmission();                     // Stop transmission and release the I2C bus
        delay(200);                                 // Wait 200ms   /////////////////////////////**********************
      }
    Wire.beginTransmission(0x68);                 // Start I2C protocol with DS3231 address
    Wire.write(0);                                // Send register address
    Wire.endTransmission(false);                  // I2C restart
    Wire.requestFrom(0x68, 7);                    // Request 7 bytes from DS3231 and release I2C bus at end of reading
    second = Wire.read();                         // Read seconds from register 0
    minute = Wire.read();                         // Read minuts from register 1
    hour   = Wire.read();                         // Read hour from register 2
    Wire.read();                                  // Read day from register 3 (not used)
    date   = Wire.read();                         // Read date from register 4
    month  = Wire.read();                         // Read month from register 5
    year   = Wire.read();                         // Read year from register 6
    DS3231_display(); 
      
    toglum=digitalRead(swlum); //Encender o apagar luz manual
    togvent=digitalRead(swvent); //Encender o apagar ventilacion manual
      
    float temp;
    float nluz=0;
   
    temp = dht.readTemperature();
    nluz = analogRead(LDR);  
  
    delay(200);
    lcd.clear();        
    
    if(togval==HIGH){
      lcd.setCursor(0,2);  
      lcd.print("Luz: ");
      lcd.println(nluz);
     }else{
      lcd.setCursor(0,2);
      lcd.print("Temperatura");
      //lcd.setCursor(0,2) ; //sets cursor to second line first row
      lcd.print(temp);
      lcd.print("-C");  
     }
  
    if((togmo==HIGH)||(hour<7)||(hour>=17)){
      lcd.setCursor(0,3);  
      lcd.print("Modo Manual");  
      digitalWrite(4,HIGH);//red
      if(toglum==HIGH) {
        digitalWrite(RELAY1,HIGH);
      }else{
        digitalWrite(RELAY1,LOW);
      }
      if(togvent==HIGH) {
        digitalWrite(RELAY2,HIGH);
      }else{
        digitalWrite(RELAY2,LOW);
      }
    }else{ 
      digitalWrite(4,LOW);//red
      lcd.setCursor(0,3);  
      lcd.print("Modo Automatico");  
        if(temp > MaxTemp)
        {
          digitalWrite(RELAY1,HIGH);
        }
        else
        {
          digitalWrite(RELAY1,LOW);
        }
        if(nluz > MinLum)
        {
          digitalWrite(RELAY2,LOW);
        }
        else
        {
          digitalWrite(RELAY2,HIGH);     
        }
    }
  }   
}
