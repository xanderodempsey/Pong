#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

#define PADDLE_WIDTH (1)
#define PADDLE_HEIGHT (7)
#define BALL_WIDTH (1)
#define BALL_HEIGHT (1)
#define DELAY (20)

bool gameOver = false;
bool updateScreen = true; 
void levelOne();
void levelTwo();
void setupStats(void);
void setupBorder(void);
void setupPlayerPaddle(void);
void setupHelpScreen(void);
void updatePlayerPaddle(int key);
void setupAIPaddle();
void setupBall(void);
void countdown(void);
void updateTime(void);
void resumeTime(double pauseStart);
void levelNum(void);
void drawGame(void);
void setupAnomoly(void);
void updateAnomoly(void);
void drawRails (void);
void updateRails(void);
void setupRails(void);
void reboundBall(double, double);
double startTime;
double elapsedTime;
int minutes;
int seconds;
double secPaused;
int paddleH;
int score = 0;
int lives = 10;
int level = 1;
int anomolyCounterDelay = 0;

struct Rail{
    int x;
    int y;
    bool on_screen;
};
int numberOfRails;
struct Rail *rails;

char * paddleImage =
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|";

char * aiPaddleImage =
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|";

char * helpMessage =
/**/	"               Welcome to Pong!                 "
/**/    "         Created by Alexander O'Dempsey         "
/**/    "           Student number: n9662821             "
/**/    "  Press the W and S keys to control the paddle  "
/**/    "              Press 'q' to exit                 "
/**/    "          Press 'l' to change levels            "
/**/    "       Press 'h' to return to this screen       "
/**/	"            Press any key to play...            ";

char * gameOverImg = 
/**/	"                  Game over!                    "
/**/    "              Thanks for playing!               "
/**/    "             Press any key to exit...           ";

char * ballImage = 
/**/	"O";

char railImg = '=';

char * anomoloyImg = 
/**/    " \\ | / "
/**/    "  \\|/  "
/**/    " -- -- "
/**/    "  /|\\  "
/**/    " / | \\ ";

char * countdownThreeImg =
/**/	"+---------+"            
/**/    "|         |"        
/**/    "|    3    |"        
/**/    "|         |"        
/**/    "+---------+";

char * countdownTwoImg =
/**/	"+---------+"            
/**/    "|         |"        
/**/    "|    2    |"        
/**/    "|         |"        
/**/    "+---------+";

char * countdownOneImg =
/**/	"+---------+"            
/**/    "|         |"        
/**/    "|    1    |"        
/**/    "|         |"        
/**/    "+---------+";

sprite_id paddle;
sprite_id ball;
sprite_id aiPaddle;
sprite_id anomoloy;
sprite_id *topRail;
sprite_id *bottomRail;

void setup(void){
	setupHelpScreen();
	startTime = get_current_time();
	clear_screen();
	setupStats();
	setupBorder();
	setupPlayerPaddle();
	setupBall();
	setupRails();
	numberOfRails = (screen_width()+1)/2;
}

void setupBorder ( void ) {
	int left = 0, right = screen_width() - 1, top = 0, bottom = screen_height() - 1;
	int h2 = 3;
	//Draw the border
	draw_line(left,  top, right, top, '*');
	draw_line(left, h2, right, h2, '*');
	draw_line(right, bottom, left, bottom, '*');
	draw_line(left, top, left, bottom, '*');
	draw_line(right, top, right, bottom, '*');
}

void setupStats(void){
	int top = 1; int screenPos = screen_width();
	draw_formatted(screenPos/49, top, "Lives = %3d", lives);
    draw_formatted(screenPos/4, top, "* Score = %3d", score);
    draw_formatted(screenPos/2, top, "* Level = %3d", level);
    draw_formatted(screenPos/1.3, top, "* Time = %3d:%3d", minutes,seconds);	
}

void setupHelpScreen(void){
	double helpTimer = get_current_time();
	int w = screen_width(), h = screen_height();
	int helpHeight = 8;
	int helpWidth = strlen(helpMessage) / helpHeight;
	clear_screen();
	sprite_id help = sprite_create((w - helpWidth) / 2, (h - helpHeight) / 2, helpWidth, helpHeight, helpMessage);
	sprite_draw(help);
	show_screen();
	wait_char();
	resumeTime(helpTimer);
}
void setupPlayerPaddle(void){
	int paddleX = screen_width()-4;
	int paddleY = screen_height()/2;
	int paddleH = PADDLE_HEIGHT;

	paddle = sprite_create(paddleX, paddleY, PADDLE_WIDTH, paddleH, paddleImage);
	sprite_draw(paddle);
}
void updatePlayerPaddle(int key){
	int py = sprite_y(paddle);
	if (key == 'w' && py > 4){
		sprite_move(paddle, 0, -1);
		
	}
	if (key == 's' && py <= screen_height() - PADDLE_HEIGHT - 2){
		sprite_move(paddle, 0, +1);

	}
	if (key == 'W' && py > 4){
		sprite_move(paddle, 0, -1);
		
	}
	if (key == 'S' && py <= screen_height() - PADDLE_HEIGHT - 2){
		sprite_move(paddle, 0, +1);
	}
}
void setupAIPaddle(void){
	int paddleX = 4;
	int paddleY = screen_height()/2;
	int paddleH = PADDLE_HEIGHT;

	aiPaddle = sprite_create(paddleX, paddleY, PADDLE_WIDTH, paddleH, aiPaddleImage);
}
void updateAIPaddle(void){
	int ballY = round(sprite_y(ball));
	int aiPaddleYTop = round(sprite_y(aiPaddle));
	int aiPaddleC = aiPaddleYTop + (PADDLE_HEIGHT/2);
	int difference = ballY - aiPaddleC;
	
	// find difference between Y values of ball and paddle
	if ((difference > 0) && (aiPaddleYTop < (screen_height()-PADDLE_HEIGHT-1))){
		sprite_move(aiPaddle, 0, difference);
		
	}
	if ((difference < 0) && (aiPaddleYTop > 4)){
		sprite_move(aiPaddle, 0, difference);
	}
	// if it's > 0 and within the borders you need to move the paddle
	// if it's outside the border or the difference == 0 don't move the paddle
	

}
void setupBall(void){
	int ballX = screen_width()/2;
	int ballY = screen_height()/2;
	ball = sprite_create(ballX, ballY, BALL_WIDTH, BALL_HEIGHT, ballImage);
	sprite_turn_to(ball, 0.2, 0.4);
	int angle = rand() % 90;
	sprite_turn(ball, angle);

}
void updateBall(void){
	int x = round(sprite_x(ball));
	int y = round(sprite_y(ball));
	double dx = sprite_dx(ball);
	double dy = sprite_dy(ball);
	if (x <= 0) dx = -dx;
	if (x >= screen_width() - BALL_WIDTH){
		setupBall();
		lives--;
	}
	if ((x == screen_width() - 4) && (y < sprite_y(paddle) + PADDLE_HEIGHT+2) && (y > sprite_y(paddle))){
        dx = -dx;
		score++;
	}
	if ( (y <= 3) || (y >= screen_height() - BALL_HEIGHT) ) dy = -dy;
	if (level >1){
		if ((x == sprite_x(aiPaddle))&&(y < sprite_y(aiPaddle) + PADDLE_HEIGHT+2) && (y > sprite_y(aiPaddle))){
			dx = -dx;
		}
		if (x <= 0) setupBall();
	}


	if ( dx != sprite_dx(ball) || dy != sprite_dy(ball) ) {
		sprite_back(ball);
		sprite_turn_to(ball, dx, dy);
	}
	updateRails();
	}

void gameOverMessage(){
	int w = screen_width(), h = screen_height();
	int gOHeight = 3;
    int gOWidth = strlen(gameOverImg) / gOHeight;
	clear_screen();
	sprite_id gO = sprite_create((w - gOWidth) / 2, (h - gOHeight) / 2, gOWidth, gOHeight, gameOverImg);
	sprite_draw(gO);
	show_screen();
	wait_char();

	
}
void setupAnomoly(void){
	int xAnom = screen_width()/2;
	int yAnom = screen_height()/2;
	anomoloy = sprite_create(xAnom,yAnom,7,5,anomoloyImg);
}
void updateAnomoly(void){
	int xAnom = screen_width()/2;
	int yAnom = screen_height()/2;
	double xDiff = xAnom - sprite_x(ball); 
	double yDiff = yAnom - sprite_y(ball);
	double distSquared = (xDiff*xDiff) + (yDiff*yDiff);
	if (distSquared < 1e-10){
		distSquared = 1e-10;
	}
	double dist = sqrt(distSquared);
	double dx = sprite_dx(ball);
	double dy = sprite_dy(ball);
	double a = 0.1/distSquared;
	dx = dx + (a*xDiff/dist);
	dy = dy + (a*yDiff/dist);
	double v = sqrt(dx*dx + dy*dy);
	if (v > 1){
		dx = dx/v;
		dy = dy/v;
	}
	sprite_turn_to(ball,dx,dy);


}

void setupRails(void){

    rails = malloc(numberOfRails*2*sizeof(*rails));

    int startX = screen_width()/4;
    int startY = screen_height()/3;

    // Top rails
    for (int i = 0; i < numberOfRails; i++){
        rails[i].x = startX+i;
        rails[i].y = startY;
        rails[i].on_screen = true;
    }

    // Bottom rails
    for (int i = numberOfRails; i < numberOfRails*2; i++){
        rails[i].x = startX+i-numberOfRails;
        rails[i].y = startY*2;
        rails[i].on_screen = true;
    }
}

void drawRails(void){
    for (int i = 0; i < numberOfRails*2; i++){
        if (rails[i].on_screen){
            draw_char(rails[i].x, rails[i].y, railImg);
        }
    }
}

void reboundBall(double xRebound, double yRebound){
    sprite_back(ball);
    double dx = sprite_dx(ball);
    double dy = sprite_dy(ball);
    sprite_turn_to(ball, dx*xRebound, dy*yRebound);
}

void updateRails(void){
 bool rebound = false;
    if(level == 4){
        for (int i = 0; i < numberOfRails*2; i++){
            if (rails[i].on_screen){
                if(sprite_x(ball) >= rails[i].x-2 && sprite_x(ball) <= rails[i].x+1){ 
                    if(sprite_y(ball) >= rails[i].y-1 && sprite_y(ball) <= rails[i].y){
                        rails[i].on_screen = false;
      rebound = true;
                    }
                }
            }
        }
    }

 if(rebound){
  reboundBall(1.0, -1.0);
}
}
void process (void){
	int key = get_char();
	updatePlayerPaddle(key);
	updateBall();
	updateTime();
	if (key == 'l'){
		levelNum();
	}

	
	if (level > 1){
		updateAIPaddle();
	}
	if (level == 3){
		updateAnomoly();
		anomolyCounterDelay += DELAY;
	}

	if (key == 'q'){
		exit(0);
	}
	if (key == 'Q'){
		exit(0);
	}
	if (key == 'h'){
		setupHelpScreen();
	}
	if (key == 'H'){
		setupHelpScreen();
	}
	drawGame();
}

void drawGame(void){
	clear_screen();
	setupStats();
	setupBorder();
	sprite_draw(paddle);
	sprite_draw(ball);
	sprite_step(ball);
	
	if (level > 1){
		sprite_draw(aiPaddle);
	}
	if (level == 3){
		if (anomolyCounterDelay >= 5000){
            sprite_draw(anomoloy);
        }
	}
	if (level == 4){
		drawRails();
	}
}

void levelNum(void){
	if (level < 4){
		level++;
	} else{
		level = 1;
	}
	
	if (level == 2){
		setupAIPaddle();
	}
	
	if (level == 3){
		setupAnomoly();
	}
	
	if(level == 4){
		setupRails();
	}
}

void countdown(void){
	int w = screen_width();
	int h = screen_height();
	int msgHeight = 5;
	int msgWidth = 11;
	sprite_id cd3 = sprite_create((w - msgWidth) / 2, (h - msgHeight) / 2, msgWidth, msgHeight, countdownThreeImg);
	sprite_id cd2 = sprite_create((w - msgWidth) / 2, (h - msgHeight) / 2, msgWidth, msgHeight, countdownTwoImg);
	sprite_id cd1 = sprite_create((w - msgWidth) / 2, (h - msgHeight) / 2, msgWidth, msgHeight, countdownOneImg);
	
	sprite_draw(cd3);
	show_screen();
	timer_pause(1000);
	sprite_draw(cd2);
	show_screen();
	timer_pause(1000);
	sprite_draw(cd1);
	show_screen();
	timer_pause(1000);
	setupBall();
}

void resumeTime(double pauseStart){
	double currentTime = get_current_time();
	double pauseTime = currentTime - pauseStart;
	
	secPaused = (int) pauseTime + secPaused;
}

void updateTime(void){
	double currentTime = get_current_time();
	if ((minutes <= 0) && (seconds <=0)){
		elapsedTime = currentTime - startTime;
		secPaused = 0;
	} else {
		elapsedTime = currentTime - startTime - secPaused;
	}
	int wholeSeconds = (int) elapsedTime;
	minutes = wholeSeconds/60;
	seconds = wholeSeconds % 60;
}

int main() {
	srand(get_current_time());
	setup_screen();
	setup();
	countdown();
	startTime = get_current_time();
	show_screen();
	while ( !gameOver ) {
		process();

		if ( updateScreen ) {
			show_screen();
		}
		timer_pause(DELAY);
		if (lives == 0){
			gameOver = true;
			gameOverMessage();
		}

	}
	return 0;
}


