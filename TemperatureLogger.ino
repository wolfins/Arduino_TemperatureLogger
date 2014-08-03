/*
  Temperature Logger
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
  
  ToDo:
  Fix file format for csv
  include column headers
  parameterized sample rate (use RTC for timing)
  Fix backlight control (DONE?)
  upload to git
  Add backlight timer after button press
  
  
 Board Stack:
 
 Itead LCD Shield
 Ethernet Shield
 Arduino Uno
 
 Sensors:
 
  
 Arduino Uno Pin Assignments
 Pin	Board/Sensor	Function
 0  	Uno				UART 
 1		Uno				UART
 2
 3		LCD Shield		Backlight (rewired from 10)
 4		LCD Shield
 5		LCD Shield
 6		LCD Shield
 7		LCD Shield
 8		LCD Shield
 9		LCD Shield
 10   	SD Card  		SS 
 11   	SD Card			MOSI
 12   	SD Card			MISO
 13   	SD Card			SCLK
 SCL  	RTC    			I2C Clock
 SDA  	RTC    			I2C Data
 A0		LCD Shield		Pushbutton 
 A1		Steel Temp		Ambient temperature
 A2		Steel Temp		Ambient temperature
 A3		DHT22 Sensor	DHTPIN
 A4
 A5		
  
  

 This example code is in the public domain.
*/

// include the library code:


#include <LiquidCrystal.h>
#include "DHT.h"
#include <SPI.h>
#include <SD.h>
#include "Wire.h"
#include "RTClib.h"

	#define BACKLIGHT_ON_TIME 4000UL
		#define BACKLIGHT_PIN  10
#define DHTTYPE DHT21   // DHT 22  (AM2302)
#define DHTPIN A3     // what pin we're connected to
DHT dht(DHTPIN, DHTTYPE);
  LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
  RTC_DS1307 rtc;
  const int chipSelect = 4;
  const int backLightPin = 10;
  	long backlightTimer;
  int elapsedTime=0;
  	String fwDate = __DATE__;
	String fwTime = __TIME__;
// the setup routine runs once when you press reset:


//void displayDateTimeLCD(char startRow, char startColumn);

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(57600);
  pinMode(backLightPin,OUTPUT);
   lcd.begin(16, 2);
   	lcd.home();
	lcd.print("Temp Logger");
	lcd.setCursor(0,1);
	lcd.print("FW: ");
	fwDate = fwDate.substring(0,fwDate.lastIndexOf(' '));
	lcd.print(fwDate);
	lcd.print(" ");
	lcd.print(fwTime);
	delay(1000);
	
		//RTC initialization
	Wire.begin();
	rtc.begin();
	if (! rtc.isrunning()) {
		rtc.adjust(DateTime(__DATE__, __TIME__));
	}
	lcd.clear();
	displayDateTimeLCD(0,0);
	delay(1000);
	
   dht.begin();
    if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");  
   
}

// the loop routine runs over and over again forever:
void loop() {
  String dataString = "";
  
  
  	int buttonState = analogRead(A0);
	
    if(buttonState > 800) {
		if( millis() > (backlightTimer + BACKLIGHT_ON_TIME) ) {
			digitalWrite(BACKLIGHT_PIN,LOW);
		}
	}//buttonstate
	//TODO: wrapper function for keypress (return 0,1,2,3,4,5 enumerated) (check for existing code)
	else if (buttonState > 600) { //select switch pressed
		backlightTimer = millis();
		digitalWrite(BACKLIGHT_PIN,HIGH);
	}
	else if (buttonState > 400) {lcd.print("Left  ");}
	else if (buttonState > 200) {
		digitalWrite(BACKLIGHT_PIN,HIGH);
		backlightTimer = millis();
		lcd.setCursor(0,0);
		lcd.print("Down: Stop logging");
		delay(3000);
		lcd.clear();
		//systemState=SYS_STATE_STANDBY;	
	}
	else if (buttonState > 100) {
		digitalWrite(BACKLIGHT_PIN,HIGH);
		backlightTimer = millis();
		lcd.setCursor(0,0);
		lcd.print("Up: Start logging"); 
		delay(3000);
		lcd.clear();
		//systemState=SYS_STATE_RUN;
	}
	else {
		digitalWrite(BACKLIGHT_PIN,HIGH);
		backlightTimer = millis();
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Rt: List Files");
		lcd.setCursor(0,1);	
		//root = SD.open("/");
		//l/cd.print(printDirectory(root,0));
		//lcd.print(" files");
		delay(2000);
		}
 
 
 
  
  // read the input on analog pin 0:
  int sensorOffset0 = 0;
  int sensorOffset1 = 28;
  int sensorOffset2 = 22;
  int sensorValue0 = analogRead(A0);
  int sensorValue1 = analogRead(A1)-sensorOffset1;
  int sensorValue2 = analogRead(A2)-sensorOffset2;
  
  float DHThumid = dht.readHumidity();
  // Read temperature as Celsius
  float DHTtemp = dht.readTemperature();
  
  
  // print out the value you read:
  Serial.print(elapsedTime);
  Serial.print(" secs ");
  dataString +=String(elapsedTime);
  dataString +=" secs ";
  Serial.print(sensorValue0);
  dataString += String(sensorValue0);
  Serial.print(" V\t");
  dataString +=" V\t";
  Serial.print("Sensor 1: ");
  dataString += "Sensor 1: ";
  Serial.print(sensorValue1);
  dataString += String(sensorValue1);
  
  Serial.print(" = ");
  dataString += " = ";
  //y = 4E-05x2 + 0.0544x - 13.908
  Serial.print(4e-5*sensorValue1*sensorValue1+0.0544*sensorValue1-13.908);
  dataString += String(4e-5*sensorValue1*sensorValue1+0.0544*sensorValue1-13.908);
  Serial.print(" C\t");
  Serial.print("Sensor 2: ");
  dataString += " C\tSensor 2: ";
  
  Serial.print(sensorValue2);
  dataString += String(sensorValue2);
  Serial.print(" = ");
  dataString += " = ";
  Serial.print(4e-5*sensorValue2*sensorValue2+0.0544*sensorValue2-13.908);
  dataString += String(4e-5*sensorValue2*sensorValue2+0.0544*sensorValue2-13.908);
  Serial.print(" C\t");
  Serial.print("Humidity: ");
  dataString += " C\tHumidity: ";
  Serial.print(DHThumid);
  dataString += String(DHThumid);
  Serial.print("% Ambient Temp: ");
  dataString += "% Ambient Temp: ";
  Serial.print(DHTtemp);
    dataString += String(DHTtemp);
  Serial.print(" C\r\n");
  dataString += " C\r\n";
  
  lcd.clear();
  lcd.home();
  /*
  if(sensorValue0 > 800) {
    lcd.print("      ");
    //digitalWrite(backLightPin,LOW);
  }
  else if (sensorValue0 > 600) {
    lcd.print("Select");
   //digitalWrite(backLightPin,HIGH);
  }
  else if (sensorValue0 > 400) {lcd.print("Left  ");}
  else if (sensorValue0 > 200) {lcd.print("Down  ");}
  else if (sensorValue0 > 100) {lcd.print("Up    ");}
  else {lcd.print("Right");}
  lcd.print(" ");
  lcd.print(sensorValue0);
  lcd.print(" V ");
  */
  lcd.print(DHThumid);
  lcd.print("% ");
  lcd.print(DHTtemp);
  lcd.print("C");
  
  lcd.setCursor(0,1);
  lcd.print(4e-5*sensorValue1*sensorValue1+0.0544*sensorValue1-13.908);
  lcd.print(" C ");
  
  lcd.print(4e-5*sensorValue2*sensorValue2+0.0544*sensorValue2-13.908);
  lcd.print(" C");
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
   // if the file is available, write to it:
  if (dataFile) {
  dataFile.print(dataString);
    
    
    dataFile.close();

  }//if dataFile
  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
  
  delay(15000);        // delay in between reads for stability
  elapsedTime += 15;
}//end of loop


/****************************************************************
\brief displays current date and time on LCD
\param startRow LCD row to start printing
\param startColumn  LCD column to start printing
\return none
****************************************************************/
void displayDateTimeLCD(char startRow, char startColumn){
	DateTime now = rtc.now(); 
	lcd.setCursor(startColumn,startRow);
	if(now.year() < 2020){
		lcd.print(now.month(), DEC);
		lcd.print('/');
		lcd.print(now.day(), DEC);
		//lcd.print('/');
		//lcd.print(now.yr(), DEC);
		lcd.print(' ');
		if(now.hour() < 10) lcd.print("0");
		lcd.print(now.hour(), DEC);
		lcd.print(':');
		if(now.minute() < 10) lcd.print("0");
		lcd.print(now.minute(), DEC);
		lcd.print(':');
		if(now.second() < 10) lcd.print("0");
		lcd.print(now.second(), DEC);
	}
	else{
		lcd.print("RTC Error");
	}

}//end of displayTimeLCD


