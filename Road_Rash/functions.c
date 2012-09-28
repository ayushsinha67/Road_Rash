#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "Road_Rash.h"
#include "functions.h"

/************************************************************************
 *	GLOBAL VARIABLES
 */
/* Timer Variables */
extern volatile uint16_t car_speed_ticker;
extern uint16_t compare;

/* Game Variables */
extern uint8_t car_row, car_col;
extern uint8_t map[8];
uint8_t gen_state;
uint8_t road_head;
uint8_t chase_head;

/* Game Over */
extern uint8_t store[8];
extern const  uint8_t letter[10][8] PROGMEM;
/************************************************************************
 *	LOAD GAME
 */
void LoadGame		( void )
{
	for( uint8_t i = 0; i < 8; i++ ){								/* Clear Map */
		map[i] = 0xFF;
	}
	
	car_row = 7;													/* Initial Car Position */ 
	car_col = 4;
	
	gen_state = 0;													/* Generation State */
	
	road_head = (uint8_t) rand() % 6;

	map[0] = ( 0x07 << road_head );									/* Initial road */
}

/************************************************************************
 *	GENERATE ROAD
 */
uint8_t GenerateRoad	( void )
{	
	/* The Road head (pointer) chases a random point till they are equal.
	   New head to chase is then generated 
	*/
	
	if( gen_state == 0 ){											/* Create New Head to chase */
		
		chase_head = (uint8_t) rand() % 6;
		gen_state = 1;	
			
	}
	
	else if ( gen_state == 1 ){										/* Chase Head mode */
		
		if( road_head == chase_head ){
			
			gen_state = 0;
			return map[0];
		}
		
		if( road_head > chase_head ){
			
			road_head--;
			return ( map[0] >> 1 );
		}
		
		else if( road_head < chase_head ){
			
			road_head++;
			return ( map[0] << 1 );
		}
			
	}	
	
	return map[0];
}

/************************************************************************
 *	MOVE ROAD
 */
void MoveRoad	( uint8_t NewRoad )
{
	for( uint8_t i = 7; i > 0; i-- ){								/* Shift Road Down */
		map[i] = map[i-1];
	}
	
	map[0] = NewRoad;												/* Add New Road */
}

/************************************************************************
 *	MOVE CAR
 */
void MoveCar	( uint8_t dir )
{

	if ( ( dir == LEFT ) && ( ( car_col + 1 ) == 8 ) ){ }			/* Boundary Conditions */

	else if ( ( dir == RIGHT ) && ( ( car_col - 1 ) == -1 ) )	{ }
		
	else if ( ( dir == UP ) && ( ( car_row - 1 ) == -1 ) ) { }
		
	else if ( ( dir == DOWN ) && ( ( car_row + 1 ) == 8 ) ) { }
		
	else if ( dir == LEFT )
		car_col++;
		
	else if ( dir == RIGHT)
		car_col--;	
		
	else if ( dir == UP )
		car_row--;
	
	else if ( dir == DOWN )
		car_row++;
}

/************************************************************************
 *	CHECK COLLISION
 */
uint8_t CheckCollision ( void )
{
	if( ( 1 << car_col ) & ( ~map[ car_row ])  )					/* if Car Collides */
		return 1;
	else
		return 0;
}

/************************************************************************
 *	SHOW CAR
 */
void ShowCar( uint8_t row )
{
	if( row == car_row )
		PORT_COL |= ( 1 << car_col );								/* Car Column */
}

/************************************************************************
 *	SHOW ROAD
 */
void ShowRoad( uint8_t row )
{
	PORT_COL = ( ~map[row] );										/* Road Column */
}

/************************************************************************
 *	GAME OVER
 */
void GameOver ( void )
{
	PORT_ROW = 0x00;
	PORT_COL = 0x00;
	uint8_t i = 0, c, r, k;
	uint8_t n = 1;  
  
	while( n < 9 ){
	  
		while( i <= 7 ){
      
			ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){
				compare = car_speed_ticker; 						/* Check Timer */
			}	  
      
			if( compare >= SCROLL_TIME ){ 
      
				for( k = 0 ; k < 8; k++ ) {							/* Clearing */
					store[k] = 0x00; 
				}		
        
				for( c = 7; c > 0; c-- ){							/* Back of First Letter */
					for( r = 0; r < 8; r++ ){
																	/* Writing */
						if( CHK( pgm_read_byte( &letter[n-1][r] ) , ( 1 << c ) ) )	
							SET( store[r], 1 << ( c + i + 1) ); 
					}
				}
			
				for( c = 0; c <= i; c++ ){							/* Front of 2nd Letter */
					for( r = 0; r < 8; r++ ){
																	/* Writing */
						if( CHK( pgm_read_byte( &letter[n][r] ) , 1 << ( 7-c ) ) )	
							SET( store[r], 1 << ( i-c ) ); 
					}        
				} 
		
				i++;												/* Scroll */
				car_speed_ticker = 0;								/* Reset Timer */
			}
       
			ShowScroll();											/* Display */
		}
    
		n++; i=0;													/* Increment, Initialize */
	}
}

/************************************************************************
 *	SHOW SCROLL
 */
void ShowScroll ( void ){

	for( uint8_t i = 0; i < 8; i++ ){								/* Display Sweep */
    
		PORT_ROW = ( 1<<i );
      
		PORT_COL = store[i];
		
		_delay_ms( SWEEP_TIME );
		
	}  
}