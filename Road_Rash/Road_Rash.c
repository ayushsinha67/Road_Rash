/*
 * Road_Rash.c
 *
 * Created: 27-09-2012 AM 2:00:55
 *  Author: Ayush Sinha
 */ 
#define F_CPU	16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include "Road_Rash.h"
#include "functions.h"

/************************************************************************
 *	GLOBAL VARIABLES
 */
/* Timer Variables */
volatile uint16_t car_speed_ticker = 0;
volatile uint16_t counter_right = 0;
volatile uint16_t counter_left  = 0;
volatile uint16_t counter_up = 0;
volatile uint16_t counter_down = 0;
uint16_t compare;

/* Game Variables */
uint8_t car_row, car_col;
uint8_t map[8];

/* GAME OVER */
uint8_t store[8];
const uint8_t letter[10][8] PROGMEM =	{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
										{ 0x00, 0x3c, 0x20, 0x20, 0x2c, 0x24, 0x3c, 0x00}, 
										{ 0x00, 0x3c, 0x24, 0x24, 0x3c, 0x24, 0x24, 0x00},
										{ 0x00, 0x44, 0x6c, 0x54, 0x44, 0x44, 0x44, 0x00},
										{ 0x00, 0x3c, 0x20, 0x20, 0x38, 0x20, 0x3c, 0x00},
										{ 0x00, 0x3c, 0x24, 0x24, 0x24, 0x24, 0x3c, 0x00},
										{ 0x00, 0x22, 0x22, 0x22, 0x22, 0x14, 0x08, 0x00},
										{ 0x00, 0x3c, 0x20, 0x20, 0x38, 0x20, 0x3c, 0x00},
										{ 0x00, 0x3c, 0x24, 0x24, 0x3c, 0x28, 0x24, 0x00},
										{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
										};

/************************************************************************
 *	MAIN
 */
int main(void)
{
	/* Local Variables */
	uint8_t NewRoad;
	
	/* Initialize Ports */
	DDR_ROW = 0xFF;										/* Row */
	DDR_COL = 0xFF;										/* Column */
	DDR_SEL	= 0x00;										/* Buttons */
  
	PORT_ROW = 0xFF;
	PORT_COL = 0xFF;
	PORT_SEL = 0xFF;									/* Pull Up */
		
	/* Initialize Timer : CTC mode, PS = 64, TOP = OCR1A, 1ms, 16 Mhz*/  
	TCCR1B |= ( ( 1 << WGM12 ) | (1 << CS10) | (1 << CS11) );  
	TIMSK = ( 1 << OCIE1A ); 
	OCR1A = 249;               
  
	sei();												/* Enable interrupts */ 
	
	LoadGame();											/* Load Game */
	
	while(1){ 
		
		if( ( ( PIN_SEL & SEL_LEFT ) == 0x00 ) && ( counter_left == 0 ) ){
			
			counter_left = SEL_WAIT;
			MoveCar( LEFT );							/* Go left  */
		}	
		
		if( ( ( PIN_SEL & SEL_RIGHT ) == 0x00 ) && ( counter_right == 0 ) ){
			
			counter_right = SEL_WAIT;
			MoveCar( RIGHT );							/* Go Right */
		}
		
		if( ( ( PIN_SEL & SEL_UP ) == 0x00 ) && ( counter_up == 0 ) ){
			
			counter_up = SEL_WAIT;
			MoveCar( UP );								/* Go Up */
		}	
		
		if( ( ( PIN_SEL & SEL_DOWN ) == 0x00 ) && ( counter_down == 0 ) ){
			
			counter_down = SEL_WAIT;
			MoveCar( DOWN );							/* Go Down */
		}				
		
		if( CheckCollision() == 1 ){
			
			GameOver();									/* Display Game Over */
			LoadGame();									/* Reload */
		} 
		
		ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){			/* Get Elapsed time */
			compare = car_speed_ticker;						
		}	
		
		if( compare >= CAR_SPEED ) {					/* Move Time */	
			
			NewRoad = GenerateRoad();					/* Create New path */
			MoveRoad( NewRoad );						/* Move Ahead */
			
			ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){		/* Reset Time */
				car_speed_ticker = 0;						
			}
		}	
		
		for( uint8_t i = 0; i < 8; i++ ){				/* Display Sweep */
    
			PORT_ROW = 0x00;
			PORT_ROW |= ( 1 << i );
			
			ShowRoad(i);								/* Display */
			ShowCar(i);
			
			_delay_ms( SWEEP_TIME );		
		}
	}		
}

/************************************************************************
 *	TIMER INTERRUPT
 */
ISR(TIMER1_COMPA_vect)
{
	car_speed_ticker++;  
	
	if( counter_left > 0 )								/* Auto Decrement for button press wait */
		counter_left--;
		
	if( counter_right > 0 )								
		counter_right--;
	
	if( counter_up > 0 )
		counter_up--;
	
	if( counter_down > 0 )
		counter_down--;
			
}