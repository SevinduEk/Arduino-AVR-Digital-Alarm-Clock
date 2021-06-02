
/*
AVR code for input subsystem of digital alarm clock
(E.M.S.S.N. Ekanayake - 190164M)
*/

#include <avr/io.h>
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>

#define LCD_Dir  DDRD		//define display data, register, enable pins for LCD 
#define LCD_Port PORTD
#define RS PD2
#define EN PD3

const int del=200;		//debounce delay

int starthour = 8;		//starting time
int startmins = 20;

int mins = startmins;	//time variables
int hours = starthour;

char str_hour[10];
char str_min[10];
char str_sthour[10];
char str_stmin[10];

int currentmode = 0;	//menu
int maxmodes = 3;
char *optionNames[3] = {"<<  SET TIME  >>","<< VIEW TIME  >>","<<BUTTON CHECK>>"};

void LCD_Command(unsigned char cmnd );		//functions 
void LCD_Char(unsigned char data );
void LCD_Init (void);
void LCD_String (char *str);
void LCD_Clear();
bool pressed_button (int btn);
void check_buttons();
void print_time();
void set_mode(int mode);
void menu_modes();


int main(void)
{
	DDRC = 0x00;
	
	LCD_Init();		//initialize LCD
	
	LCD_String("<<  Welcome!  >>");
	_delay_ms(1000);
	LCD_String(optionNames[currentmode]);
	_delay_ms(200);
	LCD_Clear();
	
	set_mode(currentmode);
	
	while (1)
	{
		if (pressed_button(PINC3))
		{
			menu_modes();
		}
	}
}

void LCD_Command(unsigned char cmnd )	//sends commands for LCD
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0);
	LCD_Port &= ~ (1<<RS);
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	
	_delay_us(200);
	
	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}

void LCD_Char( unsigned char data )		//prints a character on LCD
{
	LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0);
	LCD_Port |= (1<<RS);
	LCD_Port|= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (data << 4);
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}

void LCD_Init (void)	//Initializes LCD
{
	LCD_Dir = 0xFF;
	_delay_ms(20);
	
	LCD_Command(0x02);
	LCD_Command(0x28);
	LCD_Command(0x0c);
	LCD_Command(0x06);
	LCD_Command(0x01);
	_delay_ms(2);
}

void LCD_String (char *str)		//sends string to print on LCD
{
	//LCD_Clear();
	int i;
	for(i=0; str[i]!=0; i++)
	{
		LCD_Char (str[i]);
	}
	return;
}

void LCD_Clear()	//clear LCD screen
{
	LCD_Command (0x01);
	_delay_ms(2);
	LCD_Command (0x80);
}

bool pressed_button (int btn)		//determines whether the given button is pressed or not
{
	if (!(PINC & (1<<btn)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void check_buttons()		///prints which a button is pressed. exits when BACK is pressed
{
	while(true)
	{
		if (pressed_button(PINC0))
		{
			LCD_String("   Pressed UP   ");
			LCD_Command(0xC0);
			LCD_String("                ");
		}
		else if (pressed_button(PINC1))
		{
			LCD_String("  Pressed DOWN");
			LCD_Command(0xC0);
			LCD_String("                ");
		}
		else if (pressed_button(PINC2))
		{
			LCD_String("   Pressed OK");
			LCD_Command(0xC0);
			LCD_String("                ");
		}
		else if (pressed_button(PINC3))
		{
			LCD_String("  Pressed BACK");
			LCD_Command(0xC0);
			LCD_String("                ");
			_delay_ms(500);
			break;
		}
		/*else if()		//for long press
		{
			break;
		}*/
		else
		{
			LCD_Clear();
		}
	}
}

void print_time()		//prints the user given time
{
	while(true)
	{
		LCD_Command(0x02);
		sprintf(str_hour,"%d",hours);
		LCD_String("Current time:   ");
		LCD_Command(0xC0);
		LCD_String("    ");
		LCD_String(str_hour);
		LCD_String(" : ");
		sprintf(str_min,"%d",mins);
		LCD_String(str_min);
		_delay_ms(del);
		
		if (pressed_button(PINC3))
		{
			LCD_Clear();
			break;
		}
	}
}

void set_mode(int mode)			////this executes the mode which is selected by user
{
	if(mode==0)
	{
		while(true)
		{
			LCD_Command(0x02);
			LCD_String("Enter Hr:   ");
			sprintf(str_sthour,"%d",starthour);
			LCD_String(str_sthour);
			LCD_String(" ");
			
			if (pressed_button(PINC0))
			{
				starthour++;
				starthour%=24;
			}
			else if (pressed_button(PINC1))
			{
				starthour--;
				if (starthour<0)
				{
					starthour=23;
				}
			}
			else if (pressed_button(PINC2))
			{
				hours=starthour;
				break;
			}
			else if (pressed_button(PINC3))
			{
				return;
			}
			_delay_ms(del);
		}
		_delay_ms(del);
		while(true)
		{
			LCD_Command(0x02);
			LCD_String("Enter Mins : ");
			sprintf(str_stmin,"%d",startmins);
			LCD_String(str_stmin);
			LCD_String(" ");
			
			if (pressed_button(PINC0))
			{
				startmins++;
				startmins%=60;
			}
			else if (pressed_button(PINC1))
			{
				startmins--;
				if (startmins<0)
				{
					startmins=59;
				}
			}
			else if(pressed_button(PINC2))
			{
				mins=startmins;
				break;
			}
			else if (pressed_button(PINC3))
			{
				return;
			}
			_delay_ms(del);
		}
		LCD_Command(0x02);
		LCD_String("  Time is set!   ");
		_delay_ms(del);
	}
	else if (mode==1)
	{
		print_time();
		_delay_ms(del);
	}
	else if (mode==2)
	{
		check_buttons();
		_delay_ms(del);
	}
}

void menu_modes()		//switches between menu modes
{
	while(true)
	{
		LCD_Command(0x02);
		LCD_String(optionNames[currentmode]);
		_delay_ms(del);
		
		if (pressed_button(PINC0))
		{
			currentmode+=1;
			currentmode%=maxmodes;
		}
		else if(pressed_button(PINC1))
		{
			currentmode-=1;
			if (currentmode<0)
			{
				currentmode=maxmodes-1;
			}
		}
		else if(pressed_button(PINC2))
		{
			LCD_Clear();
			_delay_ms(del);
			set_mode(currentmode);
		}
		else if (pressed_button(PINC3))
		{
			_delay_ms(del);
			//return;
		}
	}
}