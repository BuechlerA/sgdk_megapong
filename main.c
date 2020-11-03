#include <genesis.h>
#include "res/resources.h"
#include <string.h>

enum GAME_STATE {
	STATE_MENU,
	STATE_PLAY_SINGLE,
	STATE_PLAY_MULTI
};
enum GAME_STATE currentState;

/*The edges of the play field*/
const int LEFT_EDGE = 0;
const int RIGHT_EDGE = 320;
const int TOP_EDGE = 0;
const int BOTTOM_EDGE = 224;

int scrollSpeed = 0;
#define SFX_BEEP 64

typedef struct {
	u16 x; u16 y; char label[10];
} Option;
#define NUM_OPTIONS 3
Option options[NUM_OPTIONS] = {
	{15, 15, "ONE PLAYER"},
	{15, 16, "TWO PLAYER"},
	{15, 17, "OPTIONS"},
};
u8 currentIndex = 0;
Sprite *cursor;

Sprite *ball;
int ball_pos_x = 100;
int ball_pos_y = 100;
int ball_vel_x = 1;
int ball_vel_y = 1;
int ball_width = 8;
int ball_height = 8;

Sprite *player1;
Sprite *player2;

//single player stats
int player_pos_x = 144;
const int player_pos_y = 200;
int player_vel_x = 0;
const int player_width = 32;
const int player_height = 8;

int score = 0;
char label_score[6] = "SCORE\0";
char str_score[3] = "0";
// end - single player stats

//multi player stats
const int player1_pos_x = 4;
int player1_pos_y = 80;
int player1_vel_y = 0;

const int player2_pos_x = 307;
int player2_pos_y = 80;
int player2_vel_y = 0;

const int player_multi_width = 8;
const int player_multi_height = 32;

int scoreP1 = 0;
int scoreP2 = 0;
char label_scoreP1[10] = "P1 SCORE\0";
char label_scoreP2[10] = "P2 SCORE\0";
char str_scoreP1[3] = "0";
char str_scoreP2[3] = "0";
// end - multiplayer stats

int game_on = FALSE;
char msg_start[22] = "PRESS START TO BEGIN!\0";
char msg_reset[37] = "GAME OVER! PRESS START TO PLAY AGAIN.";

int sign(int x) {
	return (x > 0) - (x < 0);
}
void updateScoreDisplay()
{
	if (currentState == STATE_PLAY_SINGLE)
	{
		sprintf(str_score, "%d", score);
		VDP_clearText(1, 2, 3);
		VDP_drawText(str_score, 1, 2);
	}
	else if (currentState == STATE_PLAY_MULTI)
	{
		sprintf(str_scoreP1, "%d", scoreP1);
		sprintf(str_scoreP2, "%d", scoreP2);
		VDP_clearText(1, 2, 3);
		VDP_clearText(31, 2, 3);
		VDP_drawText(str_scoreP1, 1, 2);
		VDP_drawText(str_scoreP2, 31, 2);
	}
}
//Draws text in the center of the screen
void showText(char s[]) {
	VDP_drawText(s, 20 - strlen(s) / 2, 15);
}

//game event
void startGame() {
	score = 0;
	updateScoreDisplay();

	ball_pos_x = 80;
	ball_pos_y = 40;

	ball_vel_x = -1;
	ball_vel_y = 1;

	player1_pos_y = 80;
	player2_pos_y = 80;

	/*Clear the text from the screen*/
	VDP_clearTextArea(0, 10, 40, 10);

	game_on = TRUE;
}
void endGame() {
	showText(msg_reset);
	game_on = FALSE;
}
void playerScore(int num)
{
	if (num == 1)
	{
		showText("POINT FOR P1");
	}
	else
	{
		showText("POINT FOR P2");
	}
	game_on = FALSE;
}

//handle object movement
void moveBall() 
{
	if (ball_pos_x < LEFT_EDGE) {
		ball_pos_x = LEFT_EDGE;
		ball_vel_x = -ball_vel_x;
	}
	else if (ball_pos_x + ball_width > RIGHT_EDGE) {
		ball_pos_x = RIGHT_EDGE - ball_width;
		ball_vel_x = -ball_vel_x;
	}
	if (ball_pos_y < TOP_EDGE) {
		ball_pos_y = TOP_EDGE;
		ball_vel_y = -ball_vel_y;
	}
	else if (ball_pos_y + ball_height > BOTTOM_EDGE) 
	{
		endGame();
	}

	if (ball_pos_x < player_pos_x + player_width && ball_pos_x + ball_width > player_pos_x) 
	{
		if (ball_pos_y < player_pos_y + player_height && ball_pos_y + ball_height >= player_pos_y) 
		{
			ball_pos_y = player_pos_y - ball_height - 1;
			ball_vel_y = -ball_vel_y;

			score++;
			updateScoreDisplay();
			PlaySound();

			//Make ball faster on every 5th hit
			if (score % 5 == 0) 
			{
				ball_vel_x += sign(ball_vel_x);
				ball_vel_y += sign(ball_vel_y);
			}
		}
	}

	ball_pos_x += ball_vel_x;
	ball_pos_y += ball_vel_y;

	SPR_setPosition(ball, ball_pos_x, ball_pos_y);
}
void moveBallMulti()
{
	if (ball_pos_y < TOP_EDGE) {
		ball_pos_y = TOP_EDGE;
		ball_vel_y = -ball_vel_y;
	}
	else if (ball_pos_y + ball_width > BOTTOM_EDGE) {
		ball_pos_y = BOTTOM_EDGE - ball_width;
		ball_vel_y = -ball_vel_y;
	}
	if (ball_pos_x < LEFT_EDGE) {
		scoreP2++;
		playerScore(0);
	}
	else if(ball_pos_x + ball_width > RIGHT_EDGE) {
		scoreP1++;
		playerScore(1);
	}

	if (ball_pos_x < player1_pos_x + player_multi_width && ball_pos_x + ball_width > player1_pos_x)
	{
		if (ball_pos_y < player1_pos_y + player_multi_height && ball_pos_y + ball_height >= player1_pos_y)
		{
			//ball_pos_y = player1_pos_y - ball_height - 1;
			ball_vel_x = -ball_vel_x;
			PlaySound();

			//Make ball faster on every 5th hit
			//if (score % 5 == 0)
			//{
			//	ball_vel_x += sign(ball_vel_x);
			//	ball_vel_y += sign(ball_vel_y);
			//}
		}
	}
	if (ball_pos_x < player2_pos_x + player_multi_width && ball_pos_x + ball_width > player2_pos_x)
	{
		if (ball_pos_y < player2_pos_y + player_multi_height && ball_pos_y + ball_height >= player2_pos_y)
		{
			ball_vel_x = -ball_vel_x;
			PlaySound();
		}
	}

	ball_pos_x += ball_vel_x;
	ball_pos_y += ball_vel_y;

	SPR_setPosition(ball, ball_pos_x, ball_pos_y);
}

void movePlayer()
{
	player_pos_x += player_vel_x;

	if (player_pos_x < LEFT_EDGE) player_pos_x = LEFT_EDGE;
	if (player_pos_x + player_width > RIGHT_EDGE) player_pos_x = RIGHT_EDGE - player_width;

	SPR_setPosition(player1, player_pos_x, player_pos_y);
}
void movePlayerMulti()
{
	player1_pos_y += player1_vel_y;
	player2_pos_y += player2_vel_y;

	if (player1_pos_y < TOP_EDGE) player1_pos_y = TOP_EDGE;
	if (player1_pos_y + player_multi_height > BOTTOM_EDGE) player1_pos_y = BOTTOM_EDGE - player_multi_height;

	if (player2_pos_y < TOP_EDGE) player2_pos_y = TOP_EDGE;
	if (player2_pos_y + player_multi_height > BOTTOM_EDGE) player2_pos_y = BOTTOM_EDGE - player_multi_height;

	SPR_setPosition(player1, player1_pos_x, player1_pos_y);
	SPR_setPosition(player2, player2_pos_x, player2_pos_y);
}
void clearScreen()
{
	VDP_clearPlane(BG_A, TRUE);
}
void updateCursorPosition()
{
	SPR_setPosition(cursor, options[currentIndex].x * 8 - 12, options[currentIndex].y * 8);
}
void menuMoveUp() {
	if (currentIndex > 0)
	{
		currentIndex--;
		updateCursorPosition();
		PlaySound();
	}
}
void menuMoveDown() {
	if (currentIndex < NUM_OPTIONS - 1)
	{
		currentIndex++;
		updateCursorPosition();
		PlaySound();
	}
}
void menuSelectOption(u16 Option) 
{
	clearScreen();
	switch (Option)
	{
	case 0:
		currentState = STATE_PLAY_SINGLE;
		break;
	case 1:
		currentState = STATE_PLAY_MULTI;
		break;
	case 2:
		currentState = STATE_MENU;
		break;
	default:
		break;
	}
}

//input handlers for different states
void menuJoyHandler(u16 joy, u16 changed, u16 state) 
{
	if (joy == JOY_1)
	{
		if (state & BUTTON_START)
		{
			//select current menu option
			menuSelectOption(currentIndex);
		}
		if (state & BUTTON_UP)
		{
			//set cursor up
			menuMoveUp();
		}
		else if (state & BUTTON_DOWN)
		{
			//set cursor down
			menuMoveDown();
		}
		else
		{
			if ((changed & BUTTON_UP) | (changed & BUTTON_DOWN))
			{
				//reset cursor
			}
		}
	}
}
void playSingleJoyHandler(u16 joy, u16 changed, u16 state) 
{
	if (joy == JOY_1)
	{
		if (state & BUTTON_START) 
		{
			if (!game_on) 
			{
				startGame();
			}
		}
		if (state & BUTTON_RIGHT)
		{
			player_vel_x = 3;
		}
		else if(state & BUTTON_LEFT)
		{
			player_vel_x = -3;
		}
		else
		{
			if ((changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT))
			{
				player_vel_x = 0;
			}
		}
	}
}
void playMultiJoyHandler(u16 joy, u16 changed, u16 state) 
{
	if (joy == JOY_1)
	{
		if (state & BUTTON_START)
		{
			if (!game_on)
			{
				startGame();
			}
		}
		if (state & BUTTON_DOWN)
		{
			player1_vel_y = 3;
		}
		else if (state & BUTTON_UP) 
		{
			player1_vel_y = -3;
		}
		else
		{
			if ((changed & BUTTON_UP) | (changed & BUTTON_DOWN))
			{
				player1_vel_y = 0;
			}
		}
	}
	if (joy == JOY_2)
	{
		if (state & BUTTON_START)
		{
			if (!game_on)
			{
				startGame();
			}
		}
		if (state & BUTTON_DOWN)
		{
			player2_vel_y = 3;
		}
		else if (state & BUTTON_UP)
		{
			player2_vel_y = -3;
		}
		else
		{
			if ((changed & BUTTON_UP) | (changed & BUTTON_DOWN))
			{
				player2_vel_y = 0;
			}
		}
	}
}

//general inits
void basicInit() {
	JOY_init();
	currentState = STATE_MENU;
}

void PlaySound()
{
	XGM_startPlayPCM(SFX_BEEP, 1, SOUND_PCM_CH4);
}

//state processings
void stateMenu() {
	JOY_setEventHandler(&menuJoyHandler);

	XGM_setPCM(SFX_BEEP, beep, sizeof(beep));

	SPR_init();
	cursor = SPR_addSprite(&imgarrow, 0, 0, 0);

	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

	VDP_loadTileSet(bg_star.tileset, 1, DMA);
	VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, 1), 0, 0, 64, 32);

	XGM_setLoopNumber(-1);
	XGM_startPlay(&jam_music);

	u16 i = 0;
	for (i; i < NUM_OPTIONS; i++)
	{
		Option o = options[i];
		VDP_drawText(o.label, o.x, o.y);
	}

	VDP_drawText("MEGA PONG", 15, 10);
	VDP_drawText("Copyright 1989 LEXSOFT Ltd.", 3, 25);

	updateCursorPosition();

	while (currentState == STATE_MENU)
	{
		SPR_update();
		scrollSpeed++;
		VDP_setHorizontalScroll(BG_B, scrollSpeed);
		if (scrollSpeed >= 320)
		{
			scrollSpeed = 0;
		}
		VDP_waitVSync();
	}
}
void statePlaySingle() {
	JOY_setEventHandler(&playSingleJoyHandler);

	SPR_init();
	ball = SPR_addSprite(&imgball, 100, 100, TILE_ATTR(PAL1, 0, FALSE, FALSE));
	player1 = SPR_addSprite(&imgpaddle, player_pos_x, player_pos_y, TILE_ATTR(PAL1, 0, FALSE, FALSE));

	VDP_setTextPlane(BG_A);
	VDP_drawText(label_score, 1, 1);

	updateScoreDisplay();
	showText(msg_start);

	VDP_loadTileSet(bg.tileset, 1, DMA);
	VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, 1), 0, 0, 40, 30);

	while (currentState == STATE_PLAY_SINGLE)
	{
		if (game_on == TRUE)
		{
			moveBall();
			movePlayer();
		}

		SPR_update();
		VDP_waitVSync();
	}
}
void statePlayMulti() 
{
	JOY_setEventHandler(&playMultiJoyHandler);

	SPR_init();
	ball = SPR_addSprite(&imgball, 100, 100, TILE_ATTR(PAL1, 0, FALSE, FALSE));
	player1 = SPR_addSprite(&imgpaddlev, player1_pos_x, player1_pos_y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
	player2 = SPR_addSprite(&imgpaddlev, player2_pos_x, player2_pos_y, TILE_ATTR(PAL2, 0, FALSE, FALSE));

	VDP_setTextPlane(BG_A);
	VDP_drawText(label_scoreP1, 1, 1);
	VDP_drawText(label_scoreP2, 31, 1);

	updateScoreDisplay();
	showText(msg_start);

	VDP_loadTileSet(bg.tileset, 1, DMA);
	VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, 1), 0, 0, 40, 30);

	while (currentState == STATE_PLAY_MULTI)
	{
		if (game_on == TRUE)
		{
			moveBallMulti();
			movePlayerMulti();
		}

		SPR_update();
		VDP_waitVSync();
	}
}

int main()
{
	//initialize basic state und input
	basicInit();

	while (1)
	{
		switch (currentState)
		{
			case STATE_MENU:
			{
				stateMenu();
				break;
			}
			case STATE_PLAY_SINGLE:
			{
				statePlaySingle();
				break;
			}
			case STATE_PLAY_MULTI:
			{
				statePlayMulti();
				break;
			}
			default:
			{
				break;
			}
		}
	}

	return (0);
}