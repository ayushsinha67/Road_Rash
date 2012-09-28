#ifndef ROAD_RASH_H_
#define ROAD_RASH_H_

/************************************************************************
 *	DEFINES
 */
/* Bit Manipulation */
#define CHK(x,b)		(x&b)
#define CLR(x,b)		(x&=~b)
#define SET(x,b)		(x|=b)
#define TOG(a,b)		(a^=b)
#define TOG(a,b)		(a^=b)

/* PORTS */
#define DDR_ROW			DDRA
#define DDR_COL			DDRC
#define DDR_SEL			DDRD
#define PORT_ROW		PORTA
#define PORT_COL		PORTC
#define PORT_SEL		PORTD
#define PIN_SEL			PIND

/* Selection */
#define SEL_UP			0x01
#define SEL_DOWN		0x02
#define SEL_RIGHT		0x04
#define SEL_LEFT		0x08
#define SEL_GAME_CHNG	0x10

/* Direction */
#define UP				2
#define DOWN			3
#define LEFT			0
#define RIGHT			1

/* Timer - in ms */
#define CAR_SPEED		200		
#define SWEEP_TIME		1					
#define SCROLL_TIME		50
#define SEL_WAIT		150

/* Road Bend: 1 is most difficult */
#define ROAD_BEND		05

#endif /* ROAD_RASH_H_ */