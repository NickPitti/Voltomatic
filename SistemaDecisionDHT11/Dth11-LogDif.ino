#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2); // generates an instance in the lcd

//PH Start
const int analogInPin = A0; 
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10],temp;
//PH End

//Water temperature sensor Start
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float Celsius = 0;
// Water temperature sensor End

//Humidity & temperature Start
#include "dht.h"
#define dht_apin A1 // Analog Pin sensor is connected to
dht DHT;
//Humidity & temperature End
//////////////////////////////////////////////////////////////////////
#include <Fuzzy.h>
float humedad;
Fuzzy *fuzzy = new Fuzzy();

void setup()
{
  lcd.begin(16,2);
  sensors.begin();
  Serial.begin(9600);  
  randomSeed(analogRead(0));
  Serial.println("Salida superior a 0 encienda bomba de agua");

// FuzzyInput - Humidity
FuzzySet *low = new FuzzySet(0, 15, 15, 30);
FuzzySet *medium = new FuzzySet(25, 40, 40, 55);
FuzzySet *high = new FuzzySet(50, 65, 65, 80);
FuzzySet *exhigh = new FuzzySet(75, 88, 88, 100);

// FuzzyOutput - Pump
FuzzySet *pumpoff = new FuzzySet(0, 0, 0, 0);
FuzzySet *pumpon = new FuzzySet(0, 1, 1, 1);

  // FuzzyInput
  FuzzyInput *humid = new FuzzyInput(1);

  humid->addFuzzySet(low);
  humid->addFuzzySet(medium);
  humid->addFuzzySet(high);
  humid->addFuzzySet(exhigh);
  fuzzy->addFuzzyInput(humid);

  // FuzzyOutput
  FuzzyOutput *pump = new FuzzyOutput(1);

  pump->addFuzzySet(pumpoff);
  pump->addFuzzySet(pumpon);
  fuzzy->addFuzzyOutput(pump);

  // Building FuzzyRule - 1
  FuzzyRuleAntecedent *HlowOrHmed = new FuzzyRuleAntecedent();
  HlowOrHmed->joinWithOR(low,medium);
  FuzzyRuleAntecedent *HLMOrHhigh = new FuzzyRuleAntecedent();    //HLMOrHhigh
  HLMOrHhigh->joinWithOR(HlowOrHmed,high);
  FuzzyRuleConsequent *thenHNormal = new FuzzyRuleConsequent();
  thenHNormal->addOutput(pumpon);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, HLMOrHhigh, thenHNormal);
  fuzzy->addFuzzyRule(fuzzyRule1);
  
  // Building FuzzyRule - 2
  FuzzyRuleAntecedent *Hexhigh = new FuzzyRuleAntecedent();
  Hexhigh->joinSingle(exhigh);
  FuzzyRuleConsequent *thenMuyHumedo = new FuzzyRuleConsequent();
  thenMuyHumedo->addOutput(pumpoff);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, Hexhigh, thenMuyHumedo);
  fuzzy->addFuzzyRule(fuzzyRule2);
}

void loop(){
  //PH Start
for(int i=0;i<10;i++) 
 { 
  buf[i]=analogRead(analogInPin);
  delay(10);
 }
 for(int i=0;i<9;i++)
 {
  for(int j=i+1;j<10;j++)
  {
   if(buf[i]>buf[j])
   {
    temp=buf[i];
    buf[i]=buf[j];
    buf[j]=temp;
   }
  }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 float pHVol=(float)avgValue*5.0/1024/6;
 float phValue = -5.70 * pHVol + 21.34;

      lcd.setCursor(0,0);
      lcd.print("Valor de PH: ");
      lcd.setCursor(0,1);
      lcd.print(phValue);
      
 delay(3000);
 lcd.clear();
//PH End

//Water temperature sensor Start
sensors.requestTemperatures();
  Celsius = sensors.getTempCByIndex(0);
      lcd.setCursor(0,0);
      lcd.print("Temp de Agua: ");
      lcd.setCursor(0,1);
      lcd.print(Celsius);
      lcd.print(" C ");   
      delay(3000);
      lcd.clear();
// Water temperature sensor End

//Humidity & temperature Start
      DHT.read11(dht_apin);
      lcd.setCursor(0,0);
      lcd.print("Humedad: ");
      lcd.setCursor(0,1);
      lcd.print(DHT.humidity);
      lcd.print("% ");   
      delay(3000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Temperatura: ");
      lcd.setCursor(0,1);
      lcd.print(DHT.temperature);
      lcd.print("C ");   
      delay(3000);
      lcd.clear();
//Humidity & temperature End

  humedad=DHT.humidity
  Serial.println("\n\n\nEntrada: ");
  Serial.print("\t\t\tHumedade: ");
  Serial.println(humedad);
  
  // Set the random value as an input
  fuzzy->setInput(1, humedad);
  // Running the Fuzzification
  fuzzy->fuzzify();
  // Running the Defuzzification
  float output = fuzzy->defuzzify(1);
  // Printing something
  
  Serial.println("Salida: ");
  Serial.print("\t\t\tPump: ");
  Serial.println(output);
  
  delay(3000);//Wait 5 seconds before accessing sensor again.
}
