#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

#define ONE_WIRE_BUS 6 //Pin termometru

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

const int Grzalka = 4;
const int Pompka = 3;

LiquidCrystal lcd(13,12,11, 10, 9, 8);

float temp_wytr,temp_aktu;
unsigned long start_czas,czas_wytr,czas_poz=0;
byte wybor;
bool start;
bool nagrzewanie;

//0-start/stop
//1-temp
//2-czas

const int przycisk_wybor=0;
const int przycisk_wgore=0;
const int przycisk_wdol=0;

int wybor_stan=0, wgore_stan=0, wdol_stan=0;
int wybor_pstan=0,wgore_pstan=0, wdol_pstan=0;

void setup() 
{

  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);
  

  pinMode(Grzalka, OUTPUT);
  pinMode(Pompka, OUTPUT);

  digitalWrite(Grzalka,LOW);
  digitalWrite(Pompka,LOW);

  start=false;
  nagrzewanie=false;
  wybor=2;
  czas_wytr=30;
  temp_wytr=30;
  
  Serial.begin(9600);
  sensors.begin();
  lcd.begin(16, 2);

  start_czas=millis();
}

void wyswietl()
{

  lcd.setCursor(0,0);
 
  if (start==false)
  {
  lcd.print("Start  ");

  lcd.setCursor(0,1);
  if(temp_wytr<10) lcd.print("0");
  lcd.print(temp_wytr);
  lcd.print("'C  ");

  lcd.setCursor(9,1);
  lcd.print("  ");
  if(czas_wytr<10) lcd.print("0");
  if(czas_wytr<1) lcd.print("0");
  lcd.print(czas_wytr);
  lcd.print(":00");
  }
  else
  {
  lcd.print("Stop           ");

  lcd.setCursor(0,1);
  lcd.print(temp_aktu);
  lcd.print("'C  ");

  lcd.setCursor(9,1);
  lcd.print("  00");
  lcd.setCursor(11,1);
  if(czas_poz/60000<10) lcd.print("0");
  lcd.print(czas_poz/60000);
  lcd.setCursor(13,1);
  lcd.print(":");
  lcd.setCursor(14,1);
  unsigned int sek=(czas_poz/1000)%60;
  if (sek<10)
  {
    lcd.print("0");
    lcd.print(sek);
  }else lcd.print(sek);
  }

  switch(wybor)
  {
      case 0:
      { 
          lcd.setCursor(5,0);
          lcd.print("<<");
          break;
      }
      case 1:
      { 
          lcd.setCursor(7,1);
          lcd.print("<<");
          break;
      }
      case 2:
      {
          lcd.setCursor(9,1);
          lcd.print(">>");
          break;
      }
  }
   
}

void rozpocznij()
{
    digitalWrite(Grzalka, HIGH);
    digitalWrite(Pompka, HIGH);

    nagrzewanie=true;
    
    while(temp_aktu < temp_wytr-5)
    {     
        lcd.setCursor(0,0);
        lcd.print("Nagrzewanie");
       
        dane();

        lcd.setCursor(0,1);
        lcd.print(temp_aktu);
        lcd.print("'C  ");
    }
    
    tone(7,880,2000);
    
    start_czas=millis();
    czas_poz=czas_wytr;
    start=true;
}

void zakoncz()
{
    start=false;
}

void finish()
{
  digitalWrite(Pompka,LOW);
  
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Koniec");

  lcd.setCursor(0,1);
  lcd.print("wytrawiania");

  tone(7,880,2000);
  delay(2000);

  while(wybor_stan!= HIGH)
  {
  int x;
  x=analogRead(0);
  if(x>800) wybor_stan=HIGH; else wybor_stan=LOW;
  }

  zakoncz();
}

void dane()
{
  if(start==true || nagrzewanie==true)
  {
  sensors.requestTemperatures();
  temp_aktu=sensors.getTempCByIndex(0);
  czas_poz=(czas_wytr*60000)-(millis()-start_czas);
  }
  
  int x;
  x=analogRead(3);
  if(x>800) wybor_stan=HIGH; else wybor_stan=LOW;

  x=analogRead(4);
  if(x>800) wgore_stan=HIGH; else wgore_stan=LOW;

  x=analogRead(5);
  if(x>800) wdol_stan=HIGH; else wdol_stan=LOW;
  

}

void sterowanie()
{


  if ( wybor_stan== LOW && wybor_stan!=wybor_pstan) 
  {
    wybor++;
    if (wybor==3) wybor=0;
  }
  wybor_pstan=wybor_stan;

  if (wgore_stan == LOW && wgore_stan!=wgore_pstan) 
  {
    switch(wybor)
    {
      case 0:{rozpocznij(); break;}
      case 1:{if(temp_wytr<99) temp_wytr++;break;}
      case 2:{if(czas_wytr<99) czas_wytr++;break;}
    }
   
  }
  wgore_pstan=wgore_stan;

  if (wdol_stan == LOW && wdol_stan!=wdol_pstan) 
  {
    switch(wybor)
    {
      case 0:{zakoncz() ;break;}
      case 1:{if(temp_wytr>0) temp_wytr--;break;}
      case 2:{if(czas_wytr>1) czas_wytr--;break;}
    }
  }
  wdol_pstan=wdol_stan; 
  
}

void wytrawianie()
{
  
  if(start==true) digitalWrite(Pompka, HIGH);

  if(start==true && temp_aktu < temp_wytr -5) digitalWrite(Grzalka, HIGH); else digitalWrite(Grzalka, LOW);
  
  if(start==true && czas_poz<=1000) finish();
}

void loop() 
{
 dane();
 wytrawianie();
 sterowanie();
 wyswietl();
 if (start==false) delay(100);
}






