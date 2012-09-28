#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

/************************************************************************
 *	FUNCTION DEFINITIONS
 */
void LoadGame			( void );
uint8_t GenerateRoad	( void );
void MoveRoad			( uint8_t NewRoad );
void MoveCar			( uint8_t dir );
uint8_t CheckCollision	( void );
void ShowCar			( uint8_t row );
void ShowRoad			( uint8_t row );
void GameOver			( void );
void ShowScroll			( void );

	
#endif /* FUNCTIONS_H_ */