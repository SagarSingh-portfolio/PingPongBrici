#include <stm32f031x6.h>
#include "display.h"
#include "musical_notes.h"
#include <stdint.h>

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
volatile uint32_t milliseconds;
void initTimer(void);
void playNote(uint32_t Freq,uint32_t duration);

#define BW 16
#define BH 9
int BAT_WIDTH;
#define MAX_BRICI_LEVELS 10

uint32_t ball_x;
uint32_t ball_y;
uint32_t bat_x;
uint32_t bat_y;

typedef struct  {
	uint16_t colour;
	uint16_t x;
	uint16_t y;	
	uint16_t visible;
} block_t;

#define YELLBLK 0x1f0e
#define GREENBLK 0x001f
#define BLUEBLK 0x00f8
#define BLOCKCOUNT 24
block_t Blocks[BLOCKCOUNT] = {
{ YELLBLK, 0, 18, 0 }, { YELLBLK, 16, 18, 0 }, { YELLBLK, 32, 18, 0 }, { YELLBLK, 48, 18, 0 }, { YELLBLK, 64, 18, 0 }, { YELLBLK, 80, 18, 0}, { YELLBLK, 96, 18, 0 }, { YELLBLK, 112, 18, 0 }, 
{ GREENBLK, 0, 27, 0 }, { GREENBLK, 16, 27, 0 }, { GREENBLK, 32, 27, 0 }, { GREENBLK, 48, 27, 0 }, { GREENBLK, 64, 27, 0 }, { GREENBLK, 80, 27, 0 }, { GREENBLK, 96, 27, 0 }, { GREENBLK, 112, 27, 0 },
{ BLUEBLK, 0, 36, 0 }, { BLUEBLK, 16, 36, 0 }, { BLUEBLK, 32, 36, 0 }, { BLUEBLK, 48, 36, 0 }, { BLUEBLK, 64, 36, 0}, { BLUEBLK, 80, 36, 0 }, { BLUEBLK, 96, 36, 0 }, { BLUEBLK, 112, 36, 0 }, 
};
  
void hideBlock(uint32_t index);
void showBlock(uint32_t index);
void hideBall(void);
void showBall(void);
void moveBall(uint32_t newX, uint32_t newY);
void hideBat(void);
void showBat(int,int,int);
void moveBat(uint32_t newX, uint32_t newY, int colors[7][3], int i);
int blockTouching(int Index,uint16_t ball_x,uint16_t ball_y);
int UpPressed(void);
int DownPressed(void);
int LeftPressed(void);
int RightPressed(void);
void randomize(void);
uint32_t random(uint32_t lower, uint32_t upper);
void playBrici(void);
void redOn(void); // turns on the red LED without changing the others
void redOff(void); // turns off the red LED without changing the others
void yellowOn(void); // turns on the yellow LED without changing the others
void yellowOff(void); // turns off the yellow LED without changing the others
void greenOn(void); // turns on the green LED without changing the others
void greenOff(void); // turns off the green LED without changing the others
int keyPressed(void); //makes sure getchar isnt always running 

// Editted
#define SONGSIZE
void pause(void);
void eputchar(char c);
char egetchar(void);
void eputs(char *String);
void printDecimal(int32_t Value);
void SerialBegin(void);
void introSong(void);
void deathNoise(void);
void gameOverSong(void);
void rankUpNoise(void);



int main()
{
	initClock();
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	initSysTick();
	initTimer();
	display_begin();
	//invertDisplay();// Uncomment this line if you have a display with a blue PCB
	pinMode(GPIOB,0,1);
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,0,1);
	pinMode(GPIOA,1,1);
	pinMode(GPIOB,3,1);
	
	enablePullUp(GPIOB,0);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
	enablePullUp(GPIOA,1);
	enablePullUp(GPIOA,0);
	enablePullUp(GPIOB,3);
	
	
	

	SerialBegin();//editted
	
	
	
	

	while(1)
	{
		playBrici();
	}
}
void playBrici()
{
	
	// Rank and difficulty 
	int rankColors[7][3]={{205,127,50},
												{192,192,192},
												{212,175,55},
												{243,243,243},
												{185,242,255},
												{91,73,101},
												{192,22,25}
												};
	int rankScr[7]={2,3,4,5,10,15,20};
	
	int rankIndex = 0;
	int tempRankIndex = 0;
	int difSelect = 1;
	int currentScrMulti = 1;
	int currentBallSpd = 1;
	int score = 0;
	int speed = 0;


	//list of tunes
	int bgIndex=0;
  int bgFreq[SONGSIZE]={100,200,300,500};
	int bgDuration[SONGSIZE]={10,20,30,40};
  int bgCurrentNote=0;
  int bgCount = bgDuration[0];
												
												
  int Level = MAX_BRICI_LEVELS;
  int LevelComplete = 0;
  unsigned int BallCount = 5;
  unsigned int Index;
  float BallXVelocity = 1;
  float BallYVelocity = 1;
  int brokenBlocks = 0;
	int hitCheck = 0;

	// upgrades
	int upgrade =0;
	int tempTime = 0;
	int upChoice =-1;


  // Blank the screen to start with
  fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  bat_x = 20;
  bat_y = SCREEN_HEIGHT - 20;	
  
	printText("Welcome to .....", 10, 50 , RGBToWord(0xff, 0xff, 0xff) , 0);
	delay(1000);
	fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	
	printText(" __  __    __    ",10, 40 , RGBToWord(0xff, 0xff, 0xff) , 0);
	printText("||_)||_)||// `|| ",10, 50 , RGBToWord(0xff, 0xff, 0xff) , 0);
	printText("||_)|| \\||\\\\_,|| ",10, 60 , RGBToWord(0xff, 0xff, 0xff) , 0);
	introSong();
	fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	
	
  printText("Brici", 48, 40, RGBToWord(243, 159, 14), RGBToWord(0, 0, 0));
	printText("Press UP to Play", 10, 70, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));					
	printText("Select Difficulty", 10, 50, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
	printText("<---        ---> ", 10, 60, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
	
	eputs(" ________  ________  ___  ________  ___         \r\n");
	eputs("|\\   __  \\|\\   __  \\|\\  \\|\\   ____\\|\\  \\        \r\n");
	eputs("\\ \\  \\|\\ /\\ \\  \\|\\  \\ \\  \\ \\  \\___|\\ \\  \\       \r\n");
	eputs(" \\ \\   __  \\ \\   _  _\\ \\  \\ \\  \\    \\ \\  \\      \r\n");
	eputs("  \\ \\  \\|\\  \\ \\  \\\\  \\\\ \\  \\ \\  \\____\\ \\  \\    \r\n");
	eputs("   \\ \\_______\\ \\__\\\\ _\\\\ \\__\\ \\_______\\ \\__\\    \r\n");
	eputs("    \\|_______|\\|__|\\|__|\\|__|\\|_______|\\|__|    \r\n");
	
	
	
	
	eputs("Starting Menu \r\n");
	eputs("press W to start the game\r\n");
	eputs("You can use S to pause the game during play\r\n");
	eputs("\r\n");
	
	while (!UpPressed()){
		if (RightPressed()){
			difSelect++;
			playNote(1000,200);
			if (difSelect == 4){
				difSelect =1;

				}
		}

		else if (LeftPressed()){
			difSelect--;
			playNote(1000,200);
			if (difSelect == 0){
				difSelect =3;
				}
			}
		
			if (difSelect == 1){ //sets difficulty to Easy
				printText("      ", 50, 60, RGBToWord(50, 205, 50), RGBToWord(0, 0, 0));
				printText("Easy", 50, 60, RGBToWord(50, 205, 50), RGBToWord(0, 0, 0));
				currentScrMulti = 1;
				BAT_WIDTH = 30;
				
			}
			if (difSelect == 2){ //sets difficulty to Medium
				printText("      ", 50, 60, RGBToWord(50, 205, 50), RGBToWord(0, 0, 0));
				printText("Medium", 50, 60, RGBToWord(255,255,0), RGBToWord(0, 0, 0));
				currentScrMulti = 2;
				BAT_WIDTH = 20;
				
			}
			if (difSelect == 3){ //sets difficulty to Hard
				printText("      ", 50, 60, RGBToWord(50, 205, 50), RGBToWord(0, 0, 0));
				printText("Hard", 50, 60, RGBToWord(255,0,0), RGBToWord(0, 0, 0));
				currentScrMulti = 4;
				BAT_WIDTH = 10;
				;
			}
		delay(150);
	}

	eputs("difficulty selected: ");
	eputs("\r\n");
	if (difSelect == 1)
	{
		greenOn();
		delay(500);
		greenOff();
		eputs("Easy");
		eputs("\r\n");
		speed = 20;
	}
	else if (difSelect == 2)
	{
		yellowOn();
		delay(500);
		yellowOff();
		eputs("Medium");
		eputs("\r\n");
		speed = 15;
	}
	else
	{
		redOn();
		delay(500);
		redOff();
		eputs("Hard");
		eputs("\r\n");
		speed = 10;
	}

	milliseconds=0;

	randomize();  
	ball_x = random(0,SCREEN_WIDTH);
  ball_y = random(50,(bat_y - 10));  	
	// draw the red squares indicating the number of remaining lives.
	for (Index = BallCount; Index > 0; Index--)
		fillRectangle(SCREEN_WIDTH - Index * 8, SCREEN_HEIGHT-10, 7, 7, RGBToWord(0xff, 0xf, 0xf));
	
	
	//ball velocity !!!!!  (change to difficulty selection)
  while (Level > 0)
  {
    moveBall(random(10, SCREEN_WIDTH-10), SCREEN_HEIGHT/2);
    if (random(0,2) > 0) // "flip a coin" to choose ball x velocity
      BallXVelocity = currentBallSpd;
    else
      BallXVelocity = -currentBallSpd;
    LevelComplete = 0;
    BallYVelocity = -currentBallSpd;  // initial ball motion is up the screen
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, 0); // clear the screen
		// draw the blocks.
    for (Index=0;Index<BLOCKCOUNT;Index++)
		{
			showBlock(Index);
		}
		showBall();
		showBat(rankColors[rankIndex][0],rankColors[rankIndex][1], rankColors[rankIndex][2]);
    printText("Score: ", 5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
    printNumber(score, 45, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
		
		//end of velocityy!!!!
		
		
    while (!LevelComplete)
    {
		
		tempRankIndex = rankIndex;
		
		//Rank display
		if(brokenBlocks<10)
		{
			printText("                       ", 45, 10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0,0,0));
			rankIndex=0;
			printText("Bronze", 45, 10, RGBToWord(rankColors[rankIndex][0],rankColors[rankIndex][1], rankColors[rankIndex][2] ), 0);
		}
		else if (brokenBlocks<20)
		{
			printText("                    ", 45, 10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0,0,0));
			rankIndex=1;
			printText("Silver", 45, 10, RGBToWord(rankColors[rankIndex][0],rankColors[rankIndex][1], rankColors[rankIndex][2] ), 0 );
		}
		else if (brokenBlocks<50)
		{
			printText("                     ", 45, 10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0,0,0));
			rankIndex=2;
			printText("Gold", 45, 10, RGBToWord(rankColors[rankIndex][0],rankColors[rankIndex][1], rankColors[rankIndex][2] ), 0);
		}
		else if (brokenBlocks<75)
		{
			printText("                         ", 45, 10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0,0,0));
			rankIndex=3;
			printText("Platinum", 45, 10, RGBToWord(rankColors[rankIndex][0],rankColors[rankIndex][1], rankColors[rankIndex][2] ), 0 );
		}
		else if (brokenBlocks<100)
		{
			printText("                       ", 45, 10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0,0,0));
			rankIndex=4;
			printText("Diamond", 45, 10, RGBToWord(rankColors[rankIndex][0],rankColors[rankIndex][1], rankColors[rankIndex][2] ), 0);
		}
		else if (brokenBlocks<150)
		{
			printText("                     ", 45, 10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0,0,0));
			rankIndex=5;
			printText("Obsidian", 45, 10, RGBToWord(rankColors[rankIndex][0],rankColors[rankIndex][1], rankColors[rankIndex][2] ), 0);
		}
		else
		{
			printText("                      ", 45, 10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0,0,0));
			rankIndex=6;
			printText("GRAND MASTER", 45, 10, RGBToWord(rankColors[rankIndex][0],rankColors[rankIndex][1], rankColors[rankIndex][2] ), 0);
		}

		//LED for rank change
		if (tempRankIndex < rankIndex)
		{
			eputs("Rank Up!");
			eputs("\r\n");
			
			printText("Rank Up!", 50, 100, RGBToWord(123,252,34), 0);

			delay(200);
			printText("Rank Up!", 50, 100, RGBToWord(0,252,34), 0);

			delay(200);
			printText("Rank Up!", 50, 100, RGBToWord(250,60,0), 0);

			delay(200);
			printText("Rank Up!", 50, 100, RGBToWord(150,0,100), 0);

			delay(100);

			greenOn();
			delay(200);
			yellowOn();
			delay(200);
			redOn();
			delay(200);
			greenOff();
			yellowOff();
			redOff();
			rankUpNoise();
			
			printText("        ", 50, 100, RGBToWord(0xff,0xff, 0xff), 0);
		}
		
		
		//Pause
		if(DownPressed())
		{
			eputs("Game Paused");
			eputs("\r\n");
			pause();
			delay(300);
		}
	
	  if (RightPressed())
      {
        // Move right
        if (bat_x < (SCREEN_WIDTH - BAT_WIDTH))
        {
          moveBat(bat_x + 2, bat_y, rankColors, rankIndex); // Move the bat faster than the ball
        }
      }

      if (LeftPressed())
      {
        // Move left
        if (bat_x > 0)
        {
          moveBat(bat_x - 2, bat_y, rankColors, rankIndex); // Move the bat faster than the ball
        }
      }
      if ((ball_y == bat_y) && (ball_x >= bat_x) && (ball_x <= bat_x + BAT_WIDTH))
      {
        BallYVelocity = -BallYVelocity;
				playNote(600,15);
				eputs("Bounce");
				eputs("\r\n");
      }
      showBat(rankColors[rankIndex][0],rankColors[rankIndex][1], rankColors[rankIndex][2]); // redraw bat as it might have lost a pixel due to collisions

			moveBall(ball_x+BallXVelocity,ball_y+BallYVelocity);
      
     // if (ball_x == 2)
      //  BallXVelocity = -BallXVelocity;
      if (ball_x == SCREEN_WIDTH - 2  || ball_x == 2)
			{
        BallXVelocity = -BallXVelocity;
				eputs("Bounce");
				eputs("\r\n");
			}
      if (ball_y == 2)
			{
        BallYVelocity = -BallYVelocity;
				eputs("Bounce");
				eputs("\r\n");
			}

      if (ball_y >= bat_y+3)  // has the ball pass the bat vertically?
      {
        BallCount--;
				redOn();
				delay(300);
				redOff();
				eputs("Life Lost");
				eputs("\r\n");
				deathNoise();
				brokenBlocks-=10;// lose life, reduce "streak" by 10
				if (brokenBlocks<0)
				{
					brokenBlocks=0;
				}

						if (BallCount == 0)
						{
							fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
							printText("GAME OVER", 40, 100, RGBToWord(0xff, 0xff, 0xff), 0);
							printText("Up to restart", 18, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0));
							eputs("All lives lost. Game over");
							eputs(" __     __    _      ____      ___   _      ____  ___      \r\n");
							eputs("/ /`_  / /\\  | |\\/| | |_      / / \\ \\ \\  / | |_  | |_)     \r\n");
							eputs("\\_\\_/ /_/--\\ |_|  | |_|__     \\_\\_/  \\_\\/  |_|__ |_| \\     \r\n");
							eputs("		\r\n");
					//RedOnOff
							redOn();
							delay(200);
							redOff();
							delay(200);
							redOn();
							delay(200);
							redOff();
							gameOverSong();
							while (!UpPressed())
								return;
						}
						
        if (random(0,100) & 1)
          BallXVelocity = currentBallSpd;
        else
          BallXVelocity = -currentBallSpd;

					BallYVelocity = -currentBallSpd;
					moveBall(random(10, SCREEN_WIDTH - 10), random(90, 120));
					fillRectangle(SCREEN_WIDTH-5*15, SCREEN_HEIGHT-10, 120, 10, 0);
        for (Index = BallCount; Index > 0; Index--)
					fillRectangle(SCREEN_WIDTH - Index * 8, SCREEN_HEIGHT-10, 7, 7, RGBToWord(0xff, 0xf, 0xf));
      }
			
			
			
			
			
			// check for ball hitting blocks and if this level is done.
			LevelComplete = 1;
      for (Index = 0; Index < BLOCKCOUNT; Index++)
      {
        int touch = blockTouching(Index,ball_x,ball_y);
        if (touch) // ball touches block
        {
          eputs("Block broken");
					eputs("\r\n");
					hideBlock(Index);
					playNote(C6,10);
          if ( (touch == 1) || (touch == 3) )
            BallYVelocity = -BallYVelocity;
          if ( (touch == 2) || (touch == 4) )
            BallXVelocity = -BallXVelocity;

					score+=rankScr[rankIndex]*currentScrMulti;
					eputs("Score: ");
					printDecimal(score);
					eputs("\r\n");
					
			printNumber(score, 45, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));

			brokenBlocks++;
        }
				if (Blocks[Index].visible) // any blocks left?  //checks if level complete
          LevelComplete = 0;
				
				
      }
		
		
      // No Blocks left, Move to next level.
      if ((LevelComplete == 1) && (Level > 0))
      {
        Level--;
		// green, yellow, red on off
				
				greenOn();
				delay(200);
				yellowOn();
				delay(200);
				redOn();
				delay(200);
				greenOff();
				yellowOff();
				redOff();
				
				
				

		eputs("Level cleared.");
				eputs("\r\n");
		eputs("New level: ");
				eputs("\r\n");
		printDecimal(MAX_BRICI_LEVELS-Level+1);

        printText("Score: ",5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
				printNumber(score, 45, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
        

      }
      delay(speed); // Slow the game to human speed and make it level dependant.
    }
  }
  eputs("Victory!");
	eputs("\r\n");
  fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, RGBToWord(0, 0, 0xff));
  printText("VICTORY!",40, 100, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  printText("Up to restart", 18, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  while (!UpPressed());
  return;
}

void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
	milliseconds++;
}
void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
  // Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
        // inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	*/
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}
void initTimer()
{
	// Power up the timer module
	RCC->APB1ENR |= (1 << 8);
	pinMode(GPIOB,1,2); // Assign a non-GPIO (alternate) function to GPIOB bit 1
	GPIOB->AFR[0] &= ~(0x0fu << 4); // Assign alternate function 0 to GPIOB 1 (Timer 14 channel 1)
	TIM14->CR1 = 0; // Set Timer 14 to default values
	TIM14->CCMR1 = (1 << 6) + (1 << 5);
	TIM14->CCER |= (1 << 0);
	TIM14->PSC = 48000000UL/65536UL; // yields maximum frequency of 21kHz when ARR = 2;
	TIM14->ARR = (48000000UL/(uint32_t)(TIM14->PSC))/((uint32_t)440);
	TIM14->CCR1 = TIM14->ARR/2;	
	TIM14->CNT = 0;
}
void playNote(uint32_t Freq,uint32_t duration)
{
	TIM14->CR1 = 0; // Set Timer 14 to default values
	TIM14->CCMR1 = (1 << 6) + (1 << 5);
	TIM14->CCER |= (1 << 0);
	TIM14->PSC = 48000000UL/65536UL; // yields maximum frequency of 21kHz when ARR = 2;
	TIM14->ARR = (48000000UL/(uint32_t)(TIM14->PSC))/((uint32_t)Freq);
	TIM14->CCR1 = TIM14->ARR/2;	
	TIM14->CNT = 0;
	TIM14->CR1 |= (1 << 0);
	uint32_t end_time=milliseconds+duration;
	while(milliseconds < end_time);
	TIM14->CR1 &= ~(1u << 0);
}

int UpPressed(void)
{
	if (keyPressed()) //allows W or w to be used as UP button input
	{
		if (egetchar()=='W' || egetchar()=='w')
			return 1;
	}
	
	
	if ( (GPIOA->IDR & (1<<8)) == 0)
		return 1;
	else
		return 0;
}
int DownPressed(void)
{
	if (keyPressed()) //allows S or s to be used as DOWN button input
	{
		if (egetchar()=='S' || egetchar()=='s')
			return 1;
	}
	
	
	if ( (GPIOA->IDR & (1<<11)) == 0)
		return 1;
	else
		return 0;
}
int LeftPressed(void)
{
	if (keyPressed()) //allows A or a to be used as LEFT button input
	{
		if (egetchar()=='A' || egetchar()=='a')
			return 1;
	}
	
	
	if ( (GPIOB->IDR & (1<<5)) == 0)
		return 1;
	else
		return 0;
}
int RightPressed(void)
{
	if (keyPressed()) //allows D or d to be used as RIGHT button input
	{
		if (egetchar()=='D' || egetchar()=='d')
			return 1;
	}
	
	
	
	if ( (GPIOB->IDR & (1<<4)) == 0)
		return 1;
	else
		return 0;
}



void hideBlock(uint32_t index)
{
	fillRectangle(Blocks[index].x,Blocks[index].y,BW,BH,0);
	Blocks[index].visible = 0;
}
void showBlock(uint32_t index)
{	
	fillRectangle(Blocks[index].x,Blocks[index].y,BW,BH,Blocks[index].colour);
	Blocks[index].visible = 1;
}
void hideBall(void)
{
	fillRectangle(ball_x,ball_y,2,2,0);
}
void showBall(void)
{
	fillRectangle(ball_x,ball_y,2,2,RGBToWord(255,255,0));
}
void moveBall(uint32_t newX, uint32_t newY)
{
	hideBall();
	ball_x = newX;
	ball_y = newY;
	showBall();
}

void hideBat(void)
{
	fillRectangle(bat_x,bat_y,BAT_WIDTH,3,0);
}
void showBat(int r,int g,int b)
{
	fillRectangle(bat_x,bat_y,BAT_WIDTH,3,RGBToWord(r,g,b));
}
void moveBat(uint32_t newX, uint32_t newY, int colors[7][3], int i)
{
	hideBat();
	bat_x = newX;
	bat_y = newY;
	showBat(colors[i][0], colors[i][1],colors[i][2]);
}
int blockTouching(int Index,uint16_t x,uint16_t y)
{
	
	  // This function returns a non zero value if the object at x,y touches the sprite
    // The sprite is assumed to be rectangular and returns a value as follows:
    // 0 : not hit
    // 1 : touching on top face (lesser Y value)
    // 2 : touching on left face (lesser X value)
    // 3 : touching on bottom face (greater Y value)    
    // 4 : touching on right face (greater X value)
    if (Blocks[Index].visible == 0)
        return 0;
    if ( Blocks[Index].y == ball_y  )
    {  // top face?
      if ( (x>=Blocks[Index].x) && ( x < (Blocks[Index].x+BW) ) )
        return 1;      
    }
    if ( x == Blocks[Index].x )
    {
      // left face
      if ( (y>=Blocks[Index].y) && ( y < (Blocks[Index].y+BH) ) )
        return 2;
    }
    if ( y == (Blocks[Index].y+BH-1)  )
    {  // bottom face?
      if ( (x>=Blocks[Index].x) && ( x < (Blocks[Index].x+BW) ) )
        return 3;      
    }
    if ( x == (Blocks[Index].x + BW-1) )
    {
      // right face
      if ( (y>=Blocks[Index].y) && ( y < (Blocks[Index].y+BH) ) )
        return 4;
    }

    return 0; // not touching
}
static uint32_t prbs_shift_register=0;
void randomize(void)
{
	while(prbs_shift_register ==0) // can't have a value of zero here
		prbs_shift_register=milliseconds;
	
}
uint32_t random(uint32_t lower, uint32_t upper)
{
	uint32_t new_bit=0;	
	uint32_t return_value;
	new_bit= ((prbs_shift_register & (1<<27))>>27) ^ ((prbs_shift_register & (1<<30))>>30);
	new_bit= ~new_bit;
	new_bit = new_bit & 1;
	prbs_shift_register=prbs_shift_register << 1;
	prbs_shift_register=prbs_shift_register | (new_bit);
	return_value = prbs_shift_register;
	return_value = (return_value)%(upper-lower)+lower;
	return return_value;
}

//Pauses game
void pause(void)
{
	delay(200);
	printText("PAUSED", 50, 100, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
	while(!DownPressed());

	printText("       ", 50, 100, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
	return;
}

//Serial diagnostics 
void SerialBegin()
{
	/* On the nucleo board, TX is on PA2 while RX is on PA15 */
	RCC->AHBENR |= (1 << 17); // enable GPIOA
	RCC->APB2ENR |= (1 << 14); // enable USART1
	pinMode(GPIOA,2,2); // enable alternate function on PA2
	pinMode(GPIOA,15,2); // enable alternate function on PA15
	// AF1 = USART1 TX on PA2
	GPIOA->AFR[0] &= 0xfffff0ff;
	GPIOA->AFR[0] |= (1 << 8);
	// AF1 = USART1 RX on PA2
	GPIOA->AFR[1] &= 0x0fffffff;
	GPIOA->AFR[1] |= (1 << 28);
	// De-assert reset of USART1 
	RCC->APB2RSTR &= ~(1u << 14);
	
	USART1->CR1 = 0; // disable before configuration
	USART1->CR3 |= (1 << 12); // disable overrun detection
	USART1->BRR = 48000000/9600; // assuming 48MHz clock and 9600 bps data rate
	USART1->CR1 |= (1 << 2) + (1 << 3); // enable Transmistter and receiver
	USART1->CR1 |= 1; // enable the UART

}
void eputchar(char c)
{
	while( (USART1->ISR & (1 << 6)) == 0); // wait for any ongoing
	USART1->ICR=0xffffffff;
	// transmission to finish
	USART1->TDR = c;
}
char egetchar()
{
	while( (USART1->ISR & (1 << 5)) == 0); // wait for a character
	return (char)USART1->RDR;
}
void eputs(char *String)
{
	while(*String) // keep printing until a NULL is found
	{
		eputchar(*String);
		String++;
	}
}
void printDecimal(int32_t Value)
{
	char DecimalString[12]; // a 32 bit value range from -2 billion to +2 billion approx
												// That's 10 digits
												// plus a null character, plus a sign
	DecimalString[11] = 0; // terminate the string;
	if (Value < 0)
	{
		DecimalString[0]='-';
		Value = -Value;
	}
	else
	{
		DecimalString[0]='+';
	}
	int index = 10;

	while(index > 0)
	{
		DecimalString[index]=(Value % 10) + '0';
		Value = Value / 10;
		index--;
	}
	eputs(DecimalString);
}

void redOn(void) //Set Bit 0
{
	GPIOB->ODR = GPIOB->ODR | (1<<3);
}
void redOff(void) //Clear Bit 0
{
	GPIOB->ODR = GPIOB->ODR & ~(1u<<3);
}

void yellowOn(void) //Set Bit 1
{
	GPIOA->ODR = GPIOA->ODR | (1<<0);
}
void yellowOff(void) //Clear Bit 1
{
	GPIOA->ODR = GPIOA->ODR & ~(1u<<0);
}

void greenOn(void) //Set Bit 2
{
	GPIOA->ODR = GPIOA->ODR | (1<<1);
}
void greenOff(void) //Clear Bit 2
{
	GPIOA->ODR = GPIOA->ODR & ~(1u<<1);
}

int keyPressed(void) //doesnt wait for input, allows code to run
{
	
	//returns a 1 if key is pressed 
	if ( (USART1->ISR & (1 << 5)) == 0)
		return 0;
	else
		return 1;
}

void introSong(void) //intro tune
{
	playNote(600,200);
	playNote(1000,400);
	playNote(800,500);
	playNote(600,200);
	playNote(1200,400);
	playNote(1000,300);
	playNote(400,700);
}

void deathNoise(void) //death noise
{
	playNote(1000,300);
	playNote(1200,300);
	playNote(800,300);
	playNote(600,600);
	
}

void gameOverSong(void) //game over song
{
	playNote(500,500);
	playNote(300,600);
	playNote(250,1000);
}

void rankUpNoise(void)
{
	playNote(300,100);
	playNote(400,100);
	playNote(300,100);
	playNote(500,300);

}