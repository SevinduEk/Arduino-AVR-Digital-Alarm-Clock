/*
Arduino code for input subsystem of digital alarm clock
(E.M.S.S.N. Ekanayake - 190164M)
*/

#include <LiquidCrystal.h>

unsigned long timenow = 0;
unsigned long timelast= 0;

//Starting time
int starthour = 12;
int startmins = 20;

//Time Variables
int secs = 0;
int mins = startmins;
int hours = starthour;

/*********************************************/
//LED
#define ledpin 13

//LCD
#define rs 7
#define en 6
#define d4 5
#define d5 4
#define d6 3
#define d7 2

//BUTTONS
#define upbutton 12
#define downbutton 11
#define okbutton 10
#define backbutton 9

//Debounce delay
int del = 300;

//LCD object
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Menu 
int currentmode = 0;
int maxmodes = 3;
String optionNames[] = {"<<  SET TIME  >>","<< VIEW TIME  >>","<<BUTTON CHECK>>"};

//Long press detect variables
unsigned long startTime;
unsigned long endTime;
unsigned long duration;
byte timerRunning = 0;

/*********************************************/

void setup()
{
  pinMode(upbutton,INPUT);
  pinMode(downbutton,INPUT);
  pinMode(okbutton,INPUT);
  pinMode(backbutton,INPUT);
  pinMode(ledpin,OUTPUT);
  
  lcd.begin(16,2);
  
  lcd.print("<<  Welcome!  >>");
  delay(1000);
  
  print_msg(optionNames[currentmode]);
  delay(500);
  set_mode(currentmode);
}

/*********************************************/

void loop()
{
  if (digitalRead(backbutton)==false)
  {
    menu_modes();
  }
}

/*********************************************/

void print_msg(String msg)  //prints single line message on LCD
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg);
}

void check_buttons()	//prints which a button is pressed. exits function when BACK button is long-pressed.
{
  while(true)
  {
    if (!digitalRead(upbutton) && (long_press(upbutton)==0 || long_press(upbutton)==1))
    {
      print_msg("  Pressed UP");
      delay(del);
    }
    else if (!digitalRead(downbutton)&& (long_press(downbutton)==0 || long_press(downbutton)==1))
    {
      print_msg("  Pressed DOWN");
      delay(del);
    }
    else if (!digitalRead(okbutton) && (long_press(okbutton)==0 || long_press(okbutton)==1))
    {
      print_msg("  Pressed OK");
      delay(del);
    }
    else if (!digitalRead(backbutton) &&(long_press(backbutton)==0))
    {
      print_msg("  Pressed BACK");
      delay(del);
    }
    else if ((long_press(backbutton)==1))
    {
      break;
    }
    else
    {
      lcd.clear();
    }
  }
  return;
}

void print_time()  //this prints the current time
{
  while(true)
  {
    lcd.setCursor(0,0);
    lcd.print("  Current time: ");
    lcd.setCursor(4,1);
    lcd.print(hours);
    lcd.setCursor(7,1);
    lcd.print(":");
    lcd.setCursor(9,1);
    lcd.print(mins);
    delay(del);
    
    if (!digitalRead(backbutton))
    {
      break;
    }
  }
}

int pressed_button()	//this detects which button is pressed and returns its pin no.
{
  while(true)
  {
    if(!digitalRead(upbutton))
    {
      return upbutton;
    }
    else if(!digitalRead(downbutton))
    {
      return downbutton;
    }
    else if(!digitalRead(okbutton))
    {
      return okbutton;
    }
    else if(!digitalRead(backbutton))
    {
      return backbutton;
    }
  }
}

void menu_modes()	//switches between menu modes
{
  while(true)
  {
    print_msg(optionNames[currentmode]);
    delay(del);
    
    int pressed=pressed_button();
    if(pressed==upbutton)
    {
      currentmode+=1;
      currentmode%=maxmodes;
    }
    else if (pressed==downbutton)
    {
      currentmode-=1;
      if (currentmode<0)
      {
        currentmode=maxmodes-1;
      }
    }
    else if(pressed==okbutton)
    {
      delay(del);
      set_mode(currentmode);
    }
    else if(pressed==backbutton)
    {
      delay(del);
      return;
    }
  }
}

void set_mode(int mode)		//this executes the mode which is selected by user
{
  if (mode==0)
  {
    while(true)
    {
      print_msg("Enter Hr:" + String(starthour));
      
      int pressed=pressed_button();
      if (pressed==upbutton)
      {
        starthour++;
        starthour%=24;
      }
      else if (pressed==downbutton)
      {
        starthour--;
        if (starthour<0)
        {
          starthour=23;
        }
      }
      else if (pressed==okbutton)
      {
        hours=starthour;
        break;
      }
      else if (pressed==backbutton)
      {
        return;
      }
      delay(del);
    }
    delay(del);
    while(true)
    {
      print_msg("Enter Mins:" + String(startmins));
      
      int pressed=pressed_button();
      if (pressed==upbutton)
      {
        startmins++;
        startmins%=60;
      }
      else if (pressed==downbutton)
      {
        startmins--;
        if (startmins<0)
        {
          startmins=59;
        }
      }
      else if (pressed==okbutton)
      {
        mins=startmins;
        break;
      } 
      else if (pressed==backbutton)
      {
        return;
      }
      delay(del);
    }
    //print_msg(" BACK for menu  ");
    lcd.setCursor(0,0);
    lcd.print(" Time is set!  ");
    lcd.setCursor(0,1);
    lcd.print(" Back for Menu  ");
    delay(del);
  }
  else if (mode==1)
  {
    print_time();
    delay(del);
  }
  else if (mode==2)
  {
    check_buttons();
    delay(del);
  }
}


void update_time()  //this function is used to set the current time. user has to set this at the start
{
  timenow = millis()/1000;
  secs = timenow - timelast;
  
  if (secs>=60)
  {
    timelast+=60;
    mins+=1;
  }
  if (mins==60)
  {
    mins=0;
    hours+=1;
    hours%=24;
  }
  if (timenow<timelast)
  {
    timelast=timenow;
  }
}
  
byte long_press(int button)		//this detects whether a button is long pressed or short pressed
{
  if (timerRunning == 0 && digitalRead(button) == LOW)
  {
    startTime = millis();
    timerRunning = 1;
  }
  if (timerRunning == 1 && digitalRead(button) == HIGH)
  {  
    endTime = millis();
    timerRunning = 0;
    duration = endTime - startTime;
  }
  if (duration>1000)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
  
  
  
/*********************************************/
  
  
  
  
  
  
