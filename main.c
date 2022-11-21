#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <time.h>

#define num_pieces_horizontal 4
#define num_pieces_vertical 19
#define bag_maxpieces num_pieces_horizontal * num_pieces_vertical
#define LEVEL_GOAL 76
#define NUM_SPRITES 145
#define NUM_SOUNDS 6

enum sfx{ANSWER_CORRECT=0,ANSWER_TYPING,ANSWER_WRONG,PIECE_FLIP,MOVE_CURSOR,MENU_SELECT};

enum timefield{GAME_HOUR1=1,GAME_HOUR2=2,GAME_MINUTE1=3,GAME_MINUTE2=4,GAME_SECOND1=5,GAME_SECOND2=6};

enum tilenum{BLANK=0,DIVIDE,MINUS,MULTIPLY,PLUS};

enum timevars{START_TIME=0, CURRENT_TIME,PIECE_START_TIME,PIECE_CURRENT_TIME,COMBO_TOTAL_TIME, COMBO_START_TIME, COMBO_CURRENT_TIME,
ANIMATECOMBO_START_TIME,ANIMATECOMBO_CURRENT_TIME,TOTAL_TIME,SECOND,MINUTE,HOUR,TIMEVAR_SIZE};

enum gamelimits{SCORE_LIMIT=0, HIT_LIMIT, MISS_LIMIT, TOTAL_LIMIT, HOUR_LIMIT, MINUTE_LIMIT, SECOND_LIMIT, GAMELIMITS_SIZE};

enum currentmode{START=0,OPTION=1,CONFIG=2};

enum currentscreen{TITLE_SCREEN,GAME_SCREEN,OPTION_SCREEN,CONFIG_SCREEN,GAMEOVER_SCREEN};

enum labels{GAME_SCORE, GAME_SPEED, GAME_TIME, GAME_HIT, GAME_MISS, GAME_TOTAL};

enum actions{ACT_UP, ACT_DOWN, ACT_LEFT, ACT_RIGHT, ACT_0, ACT_1, ACT_2, ACT_3, ACT_4, ACT_5, ACT_6, ACT_7, ACT_8, ACT_9, 
ACT_CLEAR,ACT_SUBMIT, ACT_QUIT};

enum sprites{title_sprite = 0, start_sprite, options_sprite, controls_sprite, lcursor_sprite, scursor_sprite,
s0_sprite, s1_sprite, s2_sprite, s3_sprite,s4_sprite, s5_sprite, s6_sprite, s7_sprite, s8_sprite, s9_sprite, sblank_sprite, scolon_sprite, 
key0_sprite, key1_sprite, key2_sprite, key3_sprite, key4_sprite, key5_sprite, key6_sprite, key7_sprite, key8_sprite, key9_sprite, 
keya_sprite, keyb_sprite, keyc_sprite, keyd_sprite, keye_sprite, keyf_sprite, keyg_sprite, keyh_sprite, keyi_sprite, keyj_sprite, 
keyk_sprite, keyl_sprite, keym_sprite, keyn_sprite, keyo_sprite, keyp_sprite, keyq_sprite, keyr_sprite, keys_sprite, keyt_sprite, 
keyu_sprite, keyv_sprite, keyw_sprite, keyx_sprite, keyy_sprite, keyz_sprite, keypointer_sprite, keyblank_sprite, labelscore_sprite,
labelspeed_sprite, labeltime_sprite, labelhit_sprite, labelmiss_sprite, labeltotal_sprite, blockdivide0_sprite, blockdivide1_sprite,
blockdivide2_sprite, blockdivide3_sprite, blockdivide4_sprite, blockdivide5_sprite, blockdivide6_sprite, blockdivide7_sprite,
blockdivide8_sprite, blockdivide9_sprite, blockminus0_sprite, blockminus1_sprite, blockminus2_sprite, blockminus3_sprite,
blockminus4_sprite, blockminus5_sprite, blockminus6_sprite, blockminus7_sprite, blockminus8_sprite, blockminus9_sprite,
blockmultiply0_sprite, blockmultiply1_sprite, blockmultiply2_sprite, blockmultiply3_sprite, blockmultiply4_sprite, blockmultiply5_sprite,
blockmultiply6_sprite, blockmultiply7_sprite, blockmultiply8_sprite, blockmultiply9_sprite, blockplus0_sprite, blockplus1_sprite, 
blockplus2_sprite, blockplus3_sprite, blockplus4_sprite, blockplus5_sprite, blockplus6_sprite, blockplus7_sprite, blockplus8_sprite, 
blockplus9_sprite, blockselect_sprite, blockblank_sprite, blockmeter_sprite, blockmeterblank_sprite, blockmetertopbottom_sprite, 
eq0_sprite, eq1_sprite, eq2_sprite, eq3_sprite, eq4_sprite, eq5_sprite, eq6_sprite, eq7_sprite, eq8_sprite, eq9_sprite, 
eqplus_sprite, eqminus_sprite, eqdivide_sprite, eqmultiply_sprite, eqequal_sprite, bordertopleft_sprite, borderleftright_sprite,
bordertopright_sprite, borderdownleft_sprite, borderupdown_sprite, borderdownright_sprite, gameover_sprite, background0_sprite, 
background1_sprite, background2_sprite, background3_sprite, background4_sprite, combo0_sprite, combo1_sprite, combo2_sprite,
combo3_sprite, combo4_sprite, combo5_sprite, combo6_sprite, combo7_sprite, combo8_sprite, combo9_sprite, combo10_sprite};

struct data {
// Generic variables
	int screen;
	int menu;
	int exit;
	int screenWidth;
	int screenHeight;
// Game variables
	int time[TIMEVAR_SIZE];
	int timelimitreached;
	int grid[num_pieces_horizontal][num_pieces_vertical];
	int score;
	int hit;
	int miss;
	int total;
	int speed_lvl;
	int bag_equations[4][4];
	int baglimit;
	int piecelimit[4];
	int animate_piece[4];
	int playdrop;
	int guess[2];
	int guess_position;
	int speed;
	int selection;
	int option[4];
	int optionselection;
	int goal;
	int combo_accumulator;
	int animate_combo_frame;
	int animate_combo;
	int block_frame[4];
	int block_frame_count[4];
	int controls[17];
	int current_control;
	int timeoption;
	int timearray[7];
	int scoreoption;
	int scorearray[10];
	int hitoption;
	int hitarray[10];
	int missoption;
	int missarray[10];
	int totaloption;
	int totalarray[10];
	unsigned int limits[GAMELIMITS_SIZE];
	int spriteTextNum[50][25];
	int spriteGameTextNum[6][9];
// SDL variables
	SDL_Window* window;
	SDL_Surface* screenSurface;
	SDL_Surface* spriteSurface[NUM_SPRITES];
	SDL_Rect spriteGameTextScore[9];
	SDL_Rect spriteGameTextSpeed[9];
	SDL_Rect spriteGameTextTime[9];
	SDL_Rect spriteGameTextHit[9];
	SDL_Rect spriteGameTextMiss[9];
	SDL_Rect spriteGameTextTotal[9];
	SDL_Rect *spriteGameText[6];
	SDL_Event event;
// SDL Mixer
	Mix_Chunk *sfx[NUM_SOUNDS];
};

// Take two numbers and return a random number within these limits.
int rand_lim(int min, int max) {
	return rand() % (max - min + 1) + min;
}

// Take a random operation (addtion, substraction, multiplication, divison) and counter for a collection(bag) of equations.
// Generate an equation an equation with a number, a operation symbol, a number and the answer to the equation.
// To avoid a negative number for substraction, the first number will be larger than the second number.
// To avoid a divide by zero error for division, the second number will always be a minimum of one. 
int gen_equation(struct data *gamedata, int operator, int count) {
	int max_number = gamedata->option[0]; 
	gamedata->bag_equations[count][0] = rand_lim(0,max_number);

	switch (operator) {
		case PLUS:
			gamedata->bag_equations[count][1] = eqplus_sprite;
			gamedata->bag_equations[count][2] = rand_lim(0,max_number);
			gamedata->bag_equations[count][3] = gamedata->bag_equations[count][0] + gamedata->bag_equations[count][2];
			break;
		case MINUS:
			// avoid negative number
			gamedata->bag_equations[count][1] = eqminus_sprite;
			gamedata->bag_equations[count][2] = rand_lim(0,gamedata->bag_equations[count][0]);
			gamedata->bag_equations[count][3] = gamedata->bag_equations[count][0] - gamedata->bag_equations[count][2];
			break;
		case MULTIPLY:
			gamedata->bag_equations[count][1] = eqmultiply_sprite;
			gamedata->bag_equations[count][2] = rand_lim(0,max_number);
			gamedata->bag_equations[count][3] = gamedata->bag_equations[count][0] * gamedata->bag_equations[count][2];
			break;
		case DIVIDE:
			gamedata->bag_equations[count][1] = eqdivide_sprite;
			gamedata->bag_equations[count][2] = rand_lim(1,max_number);
			gamedata->bag_equations[count][3] = gamedata->bag_equations[count][0] / gamedata->bag_equations[count][2];
			break;
		default:
			break;
		}

	return 0;
}

// Take an array of integers and the size of the array.
// Convert each number by it's placevalue and add it to the total.
// Return the converted array as a single integer.
int process_arraytoint(const int input[], int size)  {
	int converted_array = 0;
	int placevalue = 1;
	size = size -1;

	for (int i = size; i > 0; i--) {
		converted_array += input[i] * placevalue;
		placevalue = placevalue * 10;
	}

	return converted_array;
}

// Take the index of one of the pre-loaded sounds.
// If the sound was loaded properly, and is still in memory, play the sound.
int process_sfx(struct data *gamedata, int sound) {
	if (gamedata->sfx[sound] != NULL) {
		Mix_PlayChannel(-1, gamedata->sfx[sound], 0);
	}

	return 0;
}

// Start at the bottom of the grid of tiles. 
// For each line in the grid, move each tile down by one. 
// Keep moving pieces down until the entire grid is processed.
// Get an equation for each of the four tiles on the bottom of the grid.
int process_grid(struct data *gamedata) {
	int line = 18;

	while (line >= 0) {
		for (int j = 17; j >= 0; j--) {
			for (int i = 0; i < 4; i++) {
				if (gamedata->grid[i][j+1] == BLANK) {
					gamedata->grid[i][j+1] = gamedata->grid[i][j];
					gamedata->grid[i][j] = BLANK;

					if (j+1 == 18) {
						gen_equation(gamedata,gamedata->grid[i][j+1],i);
					}
				}
			}
		}
		line--;
	}

	return 0;
}

// Check if a limit has been reached.
// Set the current game screen to game over if the limit has been reached.
// The limits are set in the options screen.
int process_limits(struct data *gamedata) {
	if (gamedata->limits[HOUR_LIMIT] !=0 || gamedata->limits[MINUTE_LIMIT] !=0 || gamedata->limits[SECOND_LIMIT] != 0) {
		if (gamedata->time[HOUR] >= gamedata->limits[HOUR_LIMIT] && gamedata->time[MINUTE] >= gamedata->limits[MINUTE_LIMIT] && gamedata->time[SECOND] >= gamedata->limits[SECOND_LIMIT]) {
			gamedata->screen = GAMEOVER_SCREEN;
		}

	} else if (gamedata->limits[SCORE_LIMIT] != 0) {
		if (gamedata->score >= gamedata->limits[SCORE_LIMIT]) {
			gamedata->screen = GAMEOVER_SCREEN;
		}
	} else if (gamedata->limits[HIT_LIMIT] != 0) {
		if (gamedata->hit >= gamedata->limits[HIT_LIMIT]) {
			gamedata->screen = GAMEOVER_SCREEN;
		}
	} else if (gamedata->limits[MISS_LIMIT] != 0) {
		if (gamedata->miss >= gamedata->limits[MISS_LIMIT]) {
			gamedata->screen = GAMEOVER_SCREEN;
		}
	} else if (gamedata->limits[TOTAL_LIMIT] != 0) {
		if (gamedata->total >= gamedata->limits[TOTAL_LIMIT]) {
			gamedata->screen = GAMEOVER_SCREEN;
		}
	}

	return 0;
}

// Check if the top line's spaces already has pieces taking room. 
// If so then the grid is too full, resulting in a game over.
// Otherwise, generate 4 random tiles. 
// There should be an attempt to make sure the tiles don't repeat much. 
// This check should be performed against the current line as well as the previous line. 
// Place the tiles into the grid and decrease the limit. 
int process_line(struct data *gamedata) {
	int piece_count = 0;
	int tile = -1;
	int previous_piece[2] = {0};
	int bag[4];

	if (gamedata->piecelimit[0] == 0 || gamedata->piecelimit[1] == 0 || gamedata->piecelimit[2] == 0 || gamedata->piecelimit[3] == 0) {
		gamedata->screen = GAMEOVER_SCREEN;
		gamedata->piecelimit[0] = 0;
		gamedata->piecelimit[1] = 0;
		gamedata->piecelimit[2] = 0;
		gamedata->piecelimit[3] = 0;

		return 0;
	}

	while (piece_count != 4) {
		tile = rand_lim(DIVIDE,PLUS);

		if (piece_count > 1) {
			previous_piece[0] = bag[piece_count - 2];
			previous_piece[1] = bag[piece_count - 1];
		}

		if ((previous_piece[0] != tile) && (previous_piece[1] != tile)) {
			bag[piece_count] = tile;
			++piece_count;
		}
	}

	gamedata->grid[0][0] = bag[0];
	gamedata->grid[1][0] = bag[1];
	gamedata->grid[2][0] = bag[2];
	gamedata->grid[3][0] = bag[3];
	gamedata->baglimit += 4;

	gamedata->piecelimit[0]--;
	gamedata->piecelimit[1]--;
	gamedata->piecelimit[2]--;
	gamedata->piecelimit[3]--;

	return 0;
}

// Setup th basic SDL VIDEO and AUDIO libraries.
int init_system(struct data *gamedata) {
	gamedata->screenWidth = 1280;
	gamedata->screenHeight = 720;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("Init error: %s\n", SDL_GetError());
	} else {
		gamedata->window = SDL_CreateWindow("SOLVER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gamedata->screenWidth, gamedata->screenHeight, 0);
		if (gamedata->window == NULL) {
			printf("Window creation error: %s\n", SDL_GetError());
		} else {
			gamedata->screenSurface = SDL_GetWindowSurface( gamedata->window );
		}

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT,2,2048) < 0) {
			printf("Init error: %s\n", Mix_GetError());
		}
	}

	return 0;
}

// Reset the variables needed to start a new game.
int reset_variables(struct data *gamedata) {
	gamedata->time[START_TIME] = 0;
	gamedata->time[CURRENT_TIME] = 0;
	gamedata->time[COMBO_TOTAL_TIME] = 0;
	gamedata->time[COMBO_START_TIME] = 0;
	gamedata->time[TOTAL_TIME] = 0;
	gamedata->time[SECOND] = 0;
	gamedata->time[MINUTE] = 0;
	gamedata->time[HOUR] = 0;
	gamedata->timelimitreached = 0;
	gamedata->score = 0;
	gamedata->hit = 0; 
	gamedata->miss = 0;
	gamedata->total = 0;
	gamedata->optionselection = 0;
	gamedata->speed_lvl = gamedata->option[1];
	gamedata->goal = LEVEL_GOAL;
	gamedata->speed = 13 - gamedata->speed_lvl;
	gamedata->selection = 0;
	gamedata->combo_accumulator = 0;
	gamedata->guess[0] = 0;
	gamedata->guess[1] = 0; 
	gamedata->guess_position = 1;
	
	gamedata->piecelimit[0] = 18;
	gamedata->piecelimit[1] = 18;
	gamedata->piecelimit[2] = 18;
	gamedata->piecelimit[3] = 18;

	gamedata->animate_piece[0] = 1;
	gamedata->animate_piece[1] = 1;
	gamedata->animate_piece[2] = 1;
	gamedata->animate_piece[3] = 1;

	gamedata->block_frame_count[0] = 9;
	gamedata->block_frame_count[1] = 9;
	gamedata->block_frame_count[2] = 9;
	gamedata->block_frame_count[3] = 9;

	gamedata->block_frame[0] = blockblank_sprite;
	gamedata->block_frame[1] = blockblank_sprite;
	gamedata->block_frame[2] = blockblank_sprite;
	gamedata->block_frame[3] = blockblank_sprite;

	gamedata->playdrop = 0;

	for (int i = 0; i<num_pieces_horizontal;i++) { 
		for (int j = 0; j<num_pieces_vertical;j++) {
			gamedata->grid[i][j] = BLANK;
		}
	}

	return 0;
}

// Take character array/string and the current text label being processed.
// Get the size of the string and fill the current label with blank sprites.
// For each character in the string, put the corresponding sprite into the label.
int assign_gamevalue(struct data *gamedata, int current_label, char input[]) {
	int size = strnlen(input, 25);

	for (int i = 0; i < 9; i++) {
		gamedata->spriteGameTextNum[current_label][i] = sblank_sprite;
	}

	for (int i = 0; i < size; i++) {
		switch(input[i]) {
			case '0':
				gamedata->spriteGameTextNum[current_label][i] = s0_sprite;
				break;
			case '1':
				gamedata->spriteGameTextNum[current_label][i] = s1_sprite;
				break;
			case '2':
				gamedata->spriteGameTextNum[current_label][i] = s2_sprite;
				break;
			case '3':
				gamedata->spriteGameTextNum[current_label][i] = s3_sprite;
				break;
			case '4':
				gamedata->spriteGameTextNum[current_label][i] = s4_sprite;
				break;
			case '5':
				gamedata->spriteGameTextNum[current_label][i] = s5_sprite;
				break;
			case '6':
				gamedata->spriteGameTextNum[current_label][i] = s6_sprite;
				break;
			case '7':
				gamedata->spriteGameTextNum[current_label][i] = s7_sprite;
				break;
			case '8':
				gamedata->spriteGameTextNum[current_label][i] = s8_sprite;
				break;
			case '9':
				gamedata->spriteGameTextNum[current_label][i] = s9_sprite;
				break;
			case ':':
				gamedata->spriteGameTextNum[current_label][i] = scolon_sprite;
				break;
			default:
				break;
		}
	}

	return 0;
}

// Take character array/string and the current control label being processed.
// Get the size of the string and fill the current label with blank sprites.
// For each character in the string, put the corresponding sprite into the label.
int assign_control(struct data *gamedata, int textNumber, char input[]) {
	int size = strnlen(input, 25);

	for (int i = 0; i < 25; i++) {
		gamedata->spriteTextNum[textNumber][i] = keyblank_sprite;
	}

	for (int i = 0; i < size; i++) {
		switch (input[i]) {
			case '0':
				gamedata->spriteTextNum[textNumber][i] = key0_sprite;
				break;
			case '1':
				gamedata->spriteTextNum[textNumber][i] = key1_sprite;
				break;
			case '2':
				gamedata->spriteTextNum[textNumber][i] = key2_sprite;
				break;
			case '3':
				gamedata->spriteTextNum[textNumber][i] = key3_sprite;
				break;
			case '4':
				gamedata->spriteTextNum[textNumber][i] = key4_sprite;
				break;
			case '5':
				gamedata->spriteTextNum[textNumber][i] = key5_sprite;
				break;
			case '6':
				gamedata->spriteTextNum[textNumber][i] = key6_sprite;
				break;
			case '7':
				gamedata->spriteTextNum[textNumber][i] = key7_sprite;
				break;
			case '8':
				gamedata->spriteTextNum[textNumber][i] = key8_sprite;
				break;
			case '9':
				gamedata->spriteTextNum[textNumber][i] = key9_sprite;
				break;
			case 'a':
				gamedata->spriteTextNum[textNumber][i] = keya_sprite;
				break;
			case 'b':
				gamedata->spriteTextNum[textNumber][i] = keyb_sprite;
				break;
			case 'c':
				gamedata->spriteTextNum[textNumber][i] = keyc_sprite;
				break;
			case 'd':
				gamedata->spriteTextNum[textNumber][i] = keyd_sprite;
				break;
			case 'e':
				gamedata->spriteTextNum[textNumber][i] = keye_sprite;
				break;
			case 'f':
				gamedata->spriteTextNum[textNumber][i] = keyf_sprite;
				break;
			case 'g':
				gamedata->spriteTextNum[textNumber][i] = keyg_sprite;
				break;
			case 'h':
				gamedata->spriteTextNum[textNumber][i] = keyh_sprite;
				break;
			case 'i':
				gamedata->spriteTextNum[textNumber][i] = keyi_sprite;
				break;
			case 'j':
				gamedata->spriteTextNum[textNumber][i] = keyj_sprite;
				break;
			case 'k':
				gamedata->spriteTextNum[textNumber][i] = keyk_sprite;
				break;
			case 'l':
				gamedata->spriteTextNum[textNumber][i] = keyl_sprite;
				break;
			case 'm':
				gamedata->spriteTextNum[textNumber][i] = keym_sprite;
				break;
			case 'n':
				gamedata->spriteTextNum[textNumber][i] = keyn_sprite;
				break;
			case 'o':
				gamedata->spriteTextNum[textNumber][i] = keyo_sprite;
				break;
			case 'p':
				gamedata->spriteTextNum[textNumber][i] = keyp_sprite;
				break;
			case 'q':
				gamedata->spriteTextNum[textNumber][i] = keyq_sprite;
				break;
			case 'r':
				gamedata->spriteTextNum[textNumber][i] = keyr_sprite;
				break;
			case 's':
				gamedata->spriteTextNum[textNumber][i] = keys_sprite;
				break;
			case 't':
				gamedata->spriteTextNum[textNumber][i] = keyt_sprite;
				break;
			case 'u':
				gamedata->spriteTextNum[textNumber][i] = keyu_sprite;
				break;
			case 'v':
				gamedata->spriteTextNum[textNumber][i] = keyv_sprite;
				break;
			case 'w':
				gamedata->spriteTextNum[textNumber][i] = keyw_sprite;
				break;
			case 'x':
				gamedata->spriteTextNum[textNumber][i] = keyx_sprite;
				break;
			case 'y':
				gamedata->spriteTextNum[textNumber][i] = keyy_sprite;
				break;
			case 'z':
				gamedata->spriteTextNum[textNumber][i] = keyz_sprite;
				break;
			default:
				gamedata->spriteTextNum[textNumber][i] = keyblank_sprite;
				break;
		}
	}

	return 0;
}

// Take a keycode for a key that has been pressed. 
// Process which SDL key was pressed.
// Send the string of the key press to be processed and assigned as a control key.
// Increment which current control is currently selected.
// If the limit of current control keys has been met then reset the current control to the first control key.
int convert_keyname(struct data *gamedata, int key) {
	switch (key) {
		case SDLK_0:
			assign_control(gamedata, gamedata->current_control, "0");
			break;
		case SDLK_1:
			assign_control(gamedata, gamedata->current_control, "1");
			break;
		case SDLK_2:
			assign_control(gamedata, gamedata->current_control, "2");
			break;
		case SDLK_3:
			assign_control(gamedata, gamedata->current_control, "3");
			break;
		case SDLK_4:
			assign_control(gamedata, gamedata->current_control, "4");
			break;
		case SDLK_5:
			assign_control(gamedata, gamedata->current_control, "5");
			break;
		case SDLK_6:
			assign_control(gamedata, gamedata->current_control, "6");
			break;
		case SDLK_7:
			assign_control(gamedata, gamedata->current_control, "7");
			break;
		case SDLK_8:
			assign_control(gamedata, gamedata->current_control, "8");
			break;
		case SDLK_9:
			assign_control(gamedata, gamedata->current_control, "9");
			break;
		case SDLK_a:
			assign_control(gamedata, gamedata->current_control, "a");
			break;
		case SDLK_AC_BACK:
			assign_control(gamedata, gamedata->current_control, "ac back");
			break;
		case SDLK_AC_BOOKMARKS:
			assign_control(gamedata, gamedata->current_control, "ac bookmarks");
			break;
		case SDLK_AC_FORWARD:
			assign_control(gamedata, gamedata->current_control, "ac forward");
			break;
		case SDLK_AC_HOME:
			assign_control(gamedata, gamedata->current_control, "ac home");
			break;
		case SDLK_AC_REFRESH:
			assign_control(gamedata, gamedata->current_control, "ac refresh");
			break;
		case SDLK_AC_SEARCH:
			assign_control(gamedata, gamedata->current_control, "ac search");
			break;
		case SDLK_AC_STOP:
			assign_control(gamedata, gamedata->current_control, "ac stop");
			break;
		case SDLK_AGAIN:
			assign_control(gamedata, gamedata->current_control, "again");
			break;
		case SDLK_ALTERASE:
			assign_control(gamedata, gamedata->current_control, "alterase");
			break;
		case SDLK_QUOTE:
			assign_control(gamedata, gamedata->current_control, "quote");
			break;
		case SDLK_APPLICATION:
			assign_control(gamedata, gamedata->current_control, "application");
			break;
		case SDLK_AUDIOMUTE:
			assign_control(gamedata, gamedata->current_control, "audiomute");
			break;
		case SDLK_AUDIONEXT:
			assign_control(gamedata, gamedata->current_control, "audionext");
			break;
		case SDLK_AUDIOPLAY:
			assign_control(gamedata, gamedata->current_control, "audioplay");
			break;
		case SDLK_AUDIOPREV:
			assign_control(gamedata, gamedata->current_control, "audioprev");
			break;
		case SDLK_AUDIOSTOP:
			assign_control(gamedata, gamedata->current_control, "audiostop");
			break;
		case SDLK_b:
			assign_control(gamedata, gamedata->current_control, "b");
			break;
		case SDLK_BACKSLASH:
			assign_control(gamedata, gamedata->current_control, "backslash");
			break;
		case SDLK_BACKSPACE:
			assign_control(gamedata, gamedata->current_control, "backspace");
			break;
		case SDLK_BRIGHTNESSDOWN:
			assign_control(gamedata, gamedata->current_control, "brightnessdown");
			break;
		case SDLK_BRIGHTNESSUP:
			assign_control(gamedata, gamedata->current_control, "brightnessup");
			break;
		case SDLK_c:
			assign_control(gamedata, gamedata->current_control, "c");
			break;
		case SDLK_CALCULATOR:
			assign_control(gamedata, gamedata->current_control, "calculator");
			break;
		case SDLK_CANCEL:
			assign_control(gamedata, gamedata->current_control, "cancel");
			break;
		case SDLK_CAPSLOCK:
			assign_control(gamedata, gamedata->current_control, "capslock");
			break;
		case SDLK_CLEAR:
			assign_control(gamedata, gamedata->current_control, "clear");
			break;
		case SDLK_CLEARAGAIN:
			assign_control(gamedata, gamedata->current_control, "clearagain");
			break;
		case SDLK_COMMA:
			assign_control(gamedata, gamedata->current_control, "comma");
			break;
		case SDLK_COMPUTER:
			assign_control(gamedata, gamedata->current_control, "computer");
			break;
		case SDLK_COPY:
			assign_control(gamedata, gamedata->current_control, "copy");
			break;
		case SDLK_CRSEL:
			assign_control(gamedata, gamedata->current_control, "crsel");
			break;
		case SDLK_CURRENCYSUBUNIT:
			assign_control(gamedata, gamedata->current_control, "currencysubunit");
			break;
		case SDLK_CURRENCYUNIT:
			assign_control(gamedata, gamedata->current_control, "currencyunit");
			break;
		case SDLK_CUT:
			assign_control(gamedata, gamedata->current_control, "cut");
			break;
		case SDLK_d:
			assign_control(gamedata, gamedata->current_control, "d");
			break;
		case SDLK_DECIMALSEPARATOR:
			assign_control(gamedata, gamedata->current_control, "decimalseperator");
			break;
		case SDLK_DELETE:
			assign_control(gamedata, gamedata->current_control, "delete");
			break;
		case SDLK_DISPLAYSWITCH:
			assign_control(gamedata, gamedata->current_control, "displayswitch");
			break;
		case SDLK_DOWN:
			assign_control(gamedata, gamedata->current_control, "down");
			break;
		case SDLK_e:
			assign_control(gamedata, gamedata->current_control, "e");
			break;
		case SDLK_EJECT:
			assign_control(gamedata, gamedata->current_control, "eject");
			break;
		case SDLK_END:
			assign_control(gamedata, gamedata->current_control, "end");
			break;
		case SDLK_EQUALS:
			assign_control(gamedata, gamedata->current_control, "equals");
			break;
		case SDLK_ESCAPE:
			assign_control(gamedata, gamedata->current_control, "escape");
			break;
		case SDLK_EXECUTE:
			assign_control(gamedata, gamedata->current_control, "execute");
			break;
		case SDLK_EXSEL:
			assign_control(gamedata, gamedata->current_control, "exsel");
			break;
		case SDLK_f:
			assign_control(gamedata, gamedata->current_control, "f");
			break;
		case SDLK_F1:
			assign_control(gamedata, gamedata->current_control, "f1");
			break;
		case SDLK_F10:
			assign_control(gamedata, gamedata->current_control, "f10");
			break;
		case SDLK_F11:
			assign_control(gamedata, gamedata->current_control, "f11");
			break;
		case SDLK_F12:
			assign_control(gamedata, gamedata->current_control, "f12");
			break;
		case SDLK_F13:
			assign_control(gamedata, gamedata->current_control, "f13");
			break;
		case SDLK_F14:
			assign_control(gamedata, gamedata->current_control, "f14");
			break;
		case SDLK_F15:
			assign_control(gamedata, gamedata->current_control, "f15");
			break;
		case SDLK_F16:
			assign_control(gamedata, gamedata->current_control, "f16");
			break;
		case SDLK_F17:
			assign_control(gamedata, gamedata->current_control, "f17");
			break;
		case SDLK_F18:
			assign_control(gamedata, gamedata->current_control, "f18");
			break;
		case SDLK_F19:
			assign_control(gamedata, gamedata->current_control, "f19");
			break;
		case SDLK_F2:
			assign_control(gamedata, gamedata->current_control, "f2");
			break;
		case SDLK_F20:
			assign_control(gamedata, gamedata->current_control, "f20");
			break;
		case SDLK_F21:
			assign_control(gamedata, gamedata->current_control, "f21");
			break;
		case SDLK_F22:
			assign_control(gamedata, gamedata->current_control, "f22");
			break;
		case SDLK_F23:
			assign_control(gamedata, gamedata->current_control, "f23");
			break;
		case SDLK_F24:
			assign_control(gamedata, gamedata->current_control, "f24");
			break;
		case SDLK_F3:
			assign_control(gamedata, gamedata->current_control, "f3");
			break;
		case SDLK_F4:
			assign_control(gamedata, gamedata->current_control, "f4");
			break;
		case SDLK_F5:
			assign_control(gamedata, gamedata->current_control, "f5");
			break;
		case SDLK_F6:
			assign_control(gamedata, gamedata->current_control, "f6");
			break;
		case SDLK_F7:
			assign_control(gamedata, gamedata->current_control, "f7");
			break;
		case SDLK_F8:
			assign_control(gamedata, gamedata->current_control, "f8");
			break;
		case SDLK_F9:
			assign_control(gamedata, gamedata->current_control, "f9");
			break;
		case SDLK_FIND:
			assign_control(gamedata, gamedata->current_control, "find");
			break;
		case SDLK_g:
			assign_control(gamedata, gamedata->current_control, "g");
			break;
		case SDLK_BACKQUOTE:
			assign_control(gamedata, gamedata->current_control, "backquote");
			break;
		case SDLK_h:
			assign_control(gamedata, gamedata->current_control, "h");
			break;
		case SDLK_HELP:
			assign_control(gamedata, gamedata->current_control, "help");
			break;
		case SDLK_HOME:
			assign_control(gamedata, gamedata->current_control, "home");
			break;
		case SDLK_i:
			assign_control(gamedata, gamedata->current_control, "i");
			break;
		case SDLK_INSERT:
			assign_control(gamedata, gamedata->current_control, "insert");
			break;
		case SDLK_j:
			assign_control(gamedata, gamedata->current_control, "j");
			break;
		case SDLK_k:
			assign_control(gamedata, gamedata->current_control, "k");
			break;
		case SDLK_KBDILLUMDOWN:
			assign_control(gamedata, gamedata->current_control, "kbdillumdown");
			break;
		case SDLK_KBDILLUMTOGGLE:
			assign_control(gamedata, gamedata->current_control, "kdbillumtoggle");
			break;
		case SDLK_KBDILLUMUP:
			assign_control(gamedata, gamedata->current_control, "kdbillumup");
			break;
		case SDLK_KP_0:
			assign_control(gamedata, gamedata->current_control, "keypad 0");
			break;
		case SDLK_KP_00:
			assign_control(gamedata, gamedata->current_control, "keypad 00");
			break;
		case SDLK_KP_000:
			assign_control(gamedata, gamedata->current_control, "keypad 000");
			break;
		case SDLK_KP_1:
			assign_control(gamedata, gamedata->current_control, "keypad 1");
			break;
		case SDLK_KP_2:
			assign_control(gamedata, gamedata->current_control, "keypad 2");
			break;
		case SDLK_KP_3:
			assign_control(gamedata, gamedata->current_control, "keypad 3");
			break;
		case SDLK_KP_4:
			assign_control(gamedata, gamedata->current_control, "keypad 4");
			break;
		case SDLK_KP_5:
			assign_control(gamedata, gamedata->current_control, "keypad 5");
			break;
		case SDLK_KP_6:
			assign_control(gamedata, gamedata->current_control, "keypad 6");
			break;
		case SDLK_KP_7:
			assign_control(gamedata, gamedata->current_control, "keypad 7");
			break;
		case SDLK_KP_8:
			assign_control(gamedata, gamedata->current_control, "keypad 8");
			break;
		case SDLK_KP_9:
			assign_control(gamedata, gamedata->current_control, "keypad 9");
			break;
		case SDLK_KP_A:
			assign_control(gamedata, gamedata->current_control, "keypad a");
			break;
		case SDLK_KP_AMPERSAND:
			assign_control(gamedata, gamedata->current_control, "keypad apersand");
			break;
		case SDLK_KP_AT:
			assign_control(gamedata, gamedata->current_control, "keypad at");
			break;
		case SDLK_KP_B:
			assign_control(gamedata, gamedata->current_control, "keypad b");
			break;
		case SDLK_KP_BACKSPACE:
			assign_control(gamedata, gamedata->current_control, "keypad backspace");
			break;
		case SDLK_KP_BINARY:
			assign_control(gamedata, gamedata->current_control, "keypad binary");
			break;
		case SDLK_KP_C:
			assign_control(gamedata, gamedata->current_control, "keypad c");
			break;
		case SDLK_KP_CLEAR:
			assign_control(gamedata, gamedata->current_control, "keypad clear");
			break;
		case SDLK_KP_CLEARENTRY:
			assign_control(gamedata, gamedata->current_control, "keypad clearentry");
			break;
		case SDLK_KP_COLON:
			assign_control(gamedata, gamedata->current_control, "keypad colon");
			break;
		case SDLK_KP_COMMA:
			assign_control(gamedata, gamedata->current_control, "keypad comma");
			break;
		case SDLK_KP_D:
			assign_control(gamedata, gamedata->current_control, "keypad d");
			break;
		case SDLK_KP_DBLAMPERSAND:
			assign_control(gamedata, gamedata->current_control, "keypad dblampersand");
			break;
		case SDLK_KP_DECIMAL:
			assign_control(gamedata, gamedata->current_control, "keypad decimal");
			break;
		case SDLK_KP_DIVIDE:
			assign_control(gamedata, gamedata->current_control, "keypad divide");
			break;
		case SDLK_KP_E:
			assign_control(gamedata, gamedata->current_control, "keypad e");
			break;
		case SDLK_KP_ENTER:
			assign_control(gamedata, gamedata->current_control, "keypad enter");
			break;
		case SDLK_KP_EQUALS:
			assign_control(gamedata, gamedata->current_control, "keypad equals");
			break;
		case SDLK_KP_EQUALSAS400:
			assign_control(gamedata, gamedata->current_control, "keypad equalsas400");
			break;
		case SDLK_KP_EXCLAM:
			assign_control(gamedata, gamedata->current_control, "keypad exclam");
			break;
		case SDLK_KP_F:
			assign_control(gamedata, gamedata->current_control, "keypad f");
			break;
		case SDLK_KP_GREATER:
			assign_control(gamedata, gamedata->current_control, "keypad greater");
			break;
		case SDLK_KP_HASH:
			assign_control(gamedata, gamedata->current_control, "keypad hash");
			break;
		case SDLK_KP_HEXADECIMAL:
			assign_control(gamedata, gamedata->current_control, "keypad hexidecimal");
			break;
		case SDLK_KP_LEFTBRACE:
			assign_control(gamedata, gamedata->current_control, "keypad leftbrace");
			break;
		case SDLK_KP_LEFTPAREN:
			assign_control(gamedata, gamedata->current_control, "keypad leftparen");
			break;
		case SDLK_KP_LESS:
			assign_control(gamedata, gamedata->current_control, "keypad less");
			break;
		case SDLK_KP_MEMADD:
			assign_control(gamedata, gamedata->current_control, "keypad memadd");
			break;
		case SDLK_KP_MEMCLEAR:
			assign_control(gamedata, gamedata->current_control, "keypad memclear");
			break;
		case SDLK_KP_MEMDIVIDE:
			assign_control(gamedata, gamedata->current_control, "keypad memdivide");
			break;
		case SDLK_KP_MEMMULTIPLY:
			assign_control(gamedata, gamedata->current_control, "keypad memmultiply");
			break;
		case SDLK_KP_MEMRECALL:
			assign_control(gamedata, gamedata->current_control, "keypad memrecall");
			break;
		case SDLK_KP_MEMSTORE:
			assign_control(gamedata, gamedata->current_control, "keypad memstore");
			break;
		case SDLK_KP_MEMSUBTRACT:
			assign_control(gamedata, gamedata->current_control, "keypad memsubtract");
			break;
		case SDLK_KP_MINUS:
			assign_control(gamedata, gamedata->current_control, "keypad minus");
			break;
		case SDLK_KP_MULTIPLY:
			assign_control(gamedata, gamedata->current_control, "keypad multiply");
			break;
		case SDLK_KP_OCTAL:
			assign_control(gamedata, gamedata->current_control, "keypad octal");
			break;
		case SDLK_KP_PERCENT:
			assign_control(gamedata, gamedata->current_control, "keypad percent");
			break;
		case SDLK_KP_PERIOD:
			assign_control(gamedata, gamedata->current_control, "keypad period");
			break;
		case SDLK_KP_PLUS:
			assign_control(gamedata, gamedata->current_control, "keypad plus");
			break;
		case SDLK_KP_PLUSMINUS:
			assign_control(gamedata, gamedata->current_control, "keypad plusminus");
			break;
		case SDLK_KP_POWER:
			assign_control(gamedata, gamedata->current_control, "keypad power");
			break;
		case SDLK_KP_RIGHTBRACE:
			assign_control(gamedata, gamedata->current_control, "keypad rightbrace");
			break;
		case SDLK_KP_RIGHTPAREN:
			assign_control(gamedata, gamedata->current_control, "keypad rightparen");
			break;
		case SDLK_KP_SPACE:
			assign_control(gamedata, gamedata->current_control, "keypad space");
			break;
		case SDLK_KP_TAB:
			assign_control(gamedata, gamedata->current_control, "keypad tab");
			break;
		case SDLK_KP_VERTICALBAR:
			assign_control(gamedata, gamedata->current_control, "keypad verticalbar");
			break;
		case SDLK_KP_XOR:
			assign_control(gamedata, gamedata->current_control, "keypad xor");
			break;
		case SDLK_l:
			assign_control(gamedata, gamedata->current_control, "l");
			break;
		case SDLK_LALT:
			assign_control(gamedata, gamedata->current_control, "lalt");
			break;
		case SDLK_LCTRL:
			assign_control(gamedata, gamedata->current_control, "lcrtl");
			break;
		case SDLK_LEFT:
			assign_control(gamedata, gamedata->current_control, "left");
			break;
		case SDLK_LEFTBRACKET:
			assign_control(gamedata, gamedata->current_control, "leftbracket");
			break;
		case SDLK_LGUI:
			assign_control(gamedata, gamedata->current_control, "lgui");
			break;
		case SDLK_LSHIFT:
			assign_control(gamedata, gamedata->current_control, "lshift");
			break;
		case SDLK_m:
			assign_control(gamedata, gamedata->current_control, "m");
			break;
		case SDLK_MAIL:
			assign_control(gamedata, gamedata->current_control, "mail");
			break;
		case SDLK_MEDIASELECT:
			assign_control(gamedata, gamedata->current_control, "mediaselect");
			break;
		case SDLK_MENU:
			assign_control(gamedata, gamedata->current_control, "menu");
			break;
		case SDLK_MINUS:
			assign_control(gamedata, gamedata->current_control, "minus");
			break;
		case SDLK_MODE:
			assign_control(gamedata, gamedata->current_control, "mode");
			break;
		case SDLK_MUTE:
			assign_control(gamedata, gamedata->current_control, "mute");
			break;
		case SDLK_n:
			assign_control(gamedata, gamedata->current_control, "n");
			break;
		case SDLK_NUMLOCKCLEAR:
			assign_control(gamedata, gamedata->current_control, "numlock");
			break;
		case SDLK_o:
			assign_control(gamedata, gamedata->current_control, "o");
			break;
		case SDLK_OPER:
			assign_control(gamedata, gamedata->current_control, "oper");
			break;
		case SDLK_OUT:
			assign_control(gamedata, gamedata->current_control, "out");
			break;
		case SDLK_p:
			assign_control(gamedata, gamedata->current_control, "p");
			break;
		case SDLK_PAGEDOWN:
			assign_control(gamedata, gamedata->current_control, "pagedown");
			break;
		case SDLK_PAGEUP:
			assign_control(gamedata, gamedata->current_control, "pageup");
			break;
		case SDLK_PASTE:
			assign_control(gamedata, gamedata->current_control, "paste");
			break;
		case SDLK_PAUSE:
			assign_control(gamedata, gamedata->current_control, "pause");
			break;
		case SDLK_PERIOD:
			assign_control(gamedata, gamedata->current_control, "period");
			break;
		case SDLK_POWER:
			assign_control(gamedata, gamedata->current_control, "power");
			break;
		case SDLK_PRINTSCREEN:
			assign_control(gamedata, gamedata->current_control, "printscreen");
			break;
		case SDLK_PRIOR:
			assign_control(gamedata, gamedata->current_control, "prior");
			break;
		case SDLK_q:
			assign_control(gamedata, gamedata->current_control, "q");
			break;
		case SDLK_r:
			assign_control(gamedata, gamedata->current_control, "r");
			break;
		case SDLK_RALT:
			assign_control(gamedata, gamedata->current_control, "ralt");
			break;
		case SDLK_RCTRL:
			assign_control(gamedata, gamedata->current_control, "rcrtl");
			break;
		case SDLK_RETURN:
			assign_control(gamedata, gamedata->current_control, "return");
			break;
		case SDLK_RETURN2:
			assign_control(gamedata, gamedata->current_control, "return2");
			break;
		case SDLK_RGUI:
			assign_control(gamedata, gamedata->current_control, "rgui");
			break;
		case SDLK_RIGHT:
			assign_control(gamedata, gamedata->current_control, "right");
			break;
		case SDLK_RIGHTBRACKET:
			assign_control(gamedata, gamedata->current_control, "rightbracket");
			break;
		case SDLK_RSHIFT:
			assign_control(gamedata, gamedata->current_control, "rshift");
			break;
		case SDLK_s:
			assign_control(gamedata, gamedata->current_control, "s");
			break;
		case SDLK_SCROLLLOCK:
			assign_control(gamedata, gamedata->current_control, "scroll lock");
			break;
		case SDLK_SELECT:
			assign_control(gamedata, gamedata->current_control, "select");
			break;
		case SDLK_SEMICOLON:
			assign_control(gamedata, gamedata->current_control, "semicolon");
			break;
		case SDLK_SEPARATOR:
			assign_control(gamedata, gamedata->current_control, "seperator");
			break;
		case SDLK_SLASH:
			assign_control(gamedata, gamedata->current_control, "slash");
			break;
		case SDLK_SLEEP:
			assign_control(gamedata, gamedata->current_control, "sleep");
			break;
		case SDLK_SPACE:
			assign_control(gamedata, gamedata->current_control, "space");
			break;
		case SDLK_STOP:
			assign_control(gamedata, gamedata->current_control, "stop");
			break;
		case SDLK_SYSREQ:
			assign_control(gamedata, gamedata->current_control, "sysreq");
			break;
		case SDLK_t:
			assign_control(gamedata, gamedata->current_control, "t");
			break;
		case SDLK_TAB:
			assign_control(gamedata, gamedata->current_control, "tab");
			break;
		case SDLK_THOUSANDSSEPARATOR:
			assign_control(gamedata, gamedata->current_control, "thousandsseperator");
			break;
		case SDLK_u:
			assign_control(gamedata, gamedata->current_control, "u");
			break;
		case SDLK_UNDO:
			assign_control(gamedata, gamedata->current_control, "undo");
			break;
		case SDLK_UNKNOWN:
			assign_control(gamedata, gamedata->current_control, "unknown");
			break;
		case SDLK_UP:
			assign_control(gamedata, gamedata->current_control, "up");
			break;
		case SDLK_v:
			assign_control(gamedata, gamedata->current_control, "v");
			break;
		case SDLK_VOLUMEDOWN:
			assign_control(gamedata, gamedata->current_control, "volumedown");
			break;
		case SDLK_VOLUMEUP:
			assign_control(gamedata, gamedata->current_control, "volumeup");
			break;
		case SDLK_w:
			assign_control(gamedata, gamedata->current_control, "w");
			break;
		case SDLK_WWW:
			assign_control(gamedata, gamedata->current_control, "www");
			break;
		case SDLK_x:
			assign_control(gamedata, gamedata->current_control, "x");
			break;
		case SDLK_y:
			assign_control(gamedata, gamedata->current_control, "y");
			break;
		case SDLK_z:
			assign_control(gamedata, gamedata->current_control, "z");
			break;
		case SDLK_AMPERSAND:
			assign_control(gamedata, gamedata->current_control, "ampersand");
			break;
		case SDLK_ASTERISK:
			assign_control(gamedata, gamedata->current_control, "asterisk");
			break;
		case SDLK_AT:
			assign_control(gamedata, gamedata->current_control, "at");
			break;
		case SDLK_CARET:
			assign_control(gamedata, gamedata->current_control, "caret");
			break;
		case SDLK_COLON:
			assign_control(gamedata, gamedata->current_control, "colon");
			break;
		case SDLK_DOLLAR:
			assign_control(gamedata, gamedata->current_control, "dollar");
			break;
		case SDLK_EXCLAIM:
			assign_control(gamedata, gamedata->current_control, "exclaim");
			break;
		case SDLK_GREATER:
			assign_control(gamedata, gamedata->current_control, "greater");
			break;
		case SDLK_HASH:
			assign_control(gamedata, gamedata->current_control, "hash");
			break;
		case SDLK_LEFTPAREN:
			assign_control(gamedata, gamedata->current_control, "leftparen");
			break;
		case SDLK_LESS:
			assign_control(gamedata, gamedata->current_control, "less");
			break;
		case SDLK_PERCENT:
			assign_control(gamedata, gamedata->current_control, "percent");
			break;
		case SDLK_PLUS:
			assign_control(gamedata, gamedata->current_control, "plus");
			break;
		case SDLK_QUESTION:
			assign_control(gamedata, gamedata->current_control, "question");
			break;
		case SDLK_QUOTEDBL:
			assign_control(gamedata, gamedata->current_control, "quotedbl");
			break;
		case SDLK_RIGHTPAREN:
			assign_control(gamedata, gamedata->current_control, "rightparen");
			break;
		case SDLK_UNDERSCORE:
			assign_control(gamedata, gamedata->current_control, "underscore");
			break;
		default:
			break;
	}

	gamedata->current_control = gamedata->current_control+1;

	if (gamedata->current_control > 16) {
		gamedata->current_control = 0;
	}

	return 0;
}

int uninit_system(struct data *gamedata) {
	for (int i = 0; i < NUM_SPRITES; i++) {
		if (gamedata->spriteSurface[i] != NULL) {
			SDL_FreeSurface(gamedata->spriteSurface[i]);
		}
	}

	for (int i = 0; i < NUM_SOUNDS; i++) {
		if (gamedata->sfx[i] != NULL) {
			Mix_FreeChunk(gamedata->sfx[i]);
		}
	}

	SDL_DestroyWindow(gamedata->window);
	Mix_Quit();
	SDL_Quit();

	return 0;
}

// Set all variables to the initial defaults for first run.
// For all loaded files (images and sounds), check if the file loaded and is of the correct size.
// If anything fails, quick the game with the corresponding error.
int init_variables(struct data *gamedata) {
	const int sfxcheck[NUM_SOUNDS] = {27596, 20288, 58828, 20288, 20288, 23716};
	const int imgcheck[NUM_SPRITES][3] = {
	{1160, 322, 4640},
{184, 52, 736},
{238, 52, 952},
{292, 52, 1168},
{64, 64, 256},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{32, 32, 128},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{16, 32, 64},
{77, 32, 308},
{77, 32, 308},
{63, 32, 252},
{40, 32, 160},
{61, 32, 244},
{82, 32, 328},
{64, 64, 256},
{64, 64, 256},
{64, 62, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{8, 8, 32},
{8, 8, 24},
{8, 8, 24},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{64, 64, 256},
{16, 16, 48},
{16, 16, 48},
{16, 16, 48},
{16, 16, 48},
{16, 16, 48},
{16, 16, 48},
{115, 32, 460},
{64, 64, 192},
{64, 64, 192},
{64, 64, 192},
{64, 64, 256},
{64, 64, 192},
{96, 32, 384},
{96, 32, 384},
{96, 32, 384},
{96, 32, 384},
{96, 32, 384},
{96, 32, 384},
{96, 32, 384},
{96, 32, 384},
{96, 32, 384},
{96, 32, 384},
{96, 32, 384}};

/* Generic variables */
	gamedata->screen = TITLE_SCREEN;
	gamedata->menu = START;
	gamedata->exit = 0;
	gamedata->option[0] = 3;
	gamedata->option[1] = 0;
	gamedata->option[2] = 0;
	gamedata->optionselection = 0;
/* time related variables */
	gamedata->time[COMBO_TOTAL_TIME] = 0;
	gamedata->time[COMBO_START_TIME] = 0;
	gamedata->time[COMBO_CURRENT_TIME] = 0;
	gamedata->time[ANIMATECOMBO_START_TIME] = SDL_GetTicks64();
	gamedata->time[ANIMATECOMBO_CURRENT_TIME] = SDL_GetTicks64();
	gamedata->time[SECOND] = 0;
	gamedata->time[MINUTE] = 0;
	gamedata->time[HOUR] = 0;
	gamedata->timelimitreached = 0;
/* Game specific variables */
	gamedata->score = 0;
	gamedata->hit = 0 ;
	gamedata->miss = 0;
	gamedata->total = 0;
	gamedata->speed_lvl = 0;
	gamedata->baglimit = 0;
	gamedata->playdrop = 0;
	gamedata->guess[0] = 0;
	gamedata->guess[1] = 0;
	gamedata->guess_position = 1;
	gamedata->speed = 13;
	gamedata->selection = 0;
	gamedata->goal = LEVEL_GOAL;
	gamedata->combo_accumulator = 0;
	gamedata->animate_combo = 0;
	gamedata->animate_combo_frame = combo10_sprite;
	gamedata->timeoption = 0;
	gamedata->timearray[0] = 0;
	gamedata->timearray[GAME_HOUR1] = 0;
	gamedata->timearray[GAME_HOUR2] = 0;
	gamedata->timearray[GAME_MINUTE1] = 0;
	gamedata->timearray[GAME_MINUTE2] = 0;
	gamedata->timearray[GAME_SECOND1] = 0;
	gamedata->timearray[GAME_SECOND2] = 0;

	gamedata->scoreoption = 0;
	gamedata->scorearray[0] = 0;
	gamedata->scorearray[1] = 0;
	gamedata->scorearray[2] = 0;
	gamedata->scorearray[3] = 0;
	gamedata->scorearray[4] = 0;
	gamedata->scorearray[5] = 0;
	gamedata->scorearray[6] = 0;
	gamedata->scorearray[7] = 0;
	gamedata->scorearray[8] = 0;
	gamedata->scorearray[9] = 0;

	gamedata->hitoption = 0;
	gamedata->hitarray[0] = 0;
	gamedata->hitarray[1] = 0;
	gamedata->hitarray[2] = 0;
	gamedata->hitarray[3] = 0;
	gamedata->hitarray[4] = 0;
	gamedata->hitarray[5] = 0;
	gamedata->hitarray[6] = 0;
	gamedata->hitarray[7] = 0;
	gamedata->hitarray[8] = 0;
	gamedata->hitarray[9] = 0;

	gamedata->missoption = 0;
	gamedata->missarray[0] = 0;
	gamedata->missarray[1] = 0;
	gamedata->missarray[2] = 0;
	gamedata->missarray[3] = 0;
	gamedata->missarray[4] = 0;
	gamedata->missarray[5] = 0;
	gamedata->missarray[6] = 0;
	gamedata->missarray[7] = 0;
	gamedata->missarray[8] = 0;
	gamedata->missarray[9] = 0;

	gamedata->totaloption = 0;
	gamedata->totalarray[0] = 0;
	gamedata->totalarray[1] = 0;
	gamedata->totalarray[2] = 0;
	gamedata->totalarray[3] = 0;
	gamedata->totalarray[4] = 0;
	gamedata->totalarray[5] = 0;
	gamedata->totalarray[6] = 0;
	gamedata->totalarray[7] = 0;
	gamedata->totalarray[8] = 0;
	gamedata->totalarray[9] = 0;

	gamedata->limits[SCORE_LIMIT] = 0;
	gamedata->limits[HIT_LIMIT] = 0;
	gamedata->limits[MISS_LIMIT] = 0;
	gamedata->limits[TOTAL_LIMIT] = 0;

	gamedata->block_frame_count[0] = 9;
	gamedata->block_frame_count[1] = 9;
	gamedata->block_frame_count[2] = 9;
	gamedata->block_frame_count[3] = 9;

	gamedata->block_frame[0] = blockblank_sprite;
	gamedata->block_frame[1] = blockblank_sprite;
	gamedata->block_frame[2] = blockblank_sprite;
	gamedata->block_frame[3] = blockblank_sprite;

	gamedata->animate_piece[0] = 1;
	gamedata->animate_piece[1] = 1;
	gamedata->animate_piece[2] = 1;
	gamedata->animate_piece[3] = 1;

	gamedata->piecelimit[0] = 18;
	gamedata->piecelimit[1] = 18;
	gamedata->piecelimit[2] = 18;
	gamedata->piecelimit[3] = 18;

	gamedata->spriteSurface[title_sprite] = IMG_Load("/usr/local/share/solver/title.png");
	gamedata->spriteSurface[start_sprite] = IMG_Load("/usr/local/share/solver/start.png");
	gamedata->spriteSurface[options_sprite] = IMG_Load("/usr/local/share/solver/options.png");
	gamedata->spriteSurface[controls_sprite] = IMG_Load("/usr/local/share/solver/controls.png");
	gamedata->spriteSurface[lcursor_sprite] = IMG_Load("/usr/local/share/solver/large_cursor.png");
	gamedata->spriteSurface[scursor_sprite] = IMG_Load("/usr/local/share/solver/small_cursor.png");
	gamedata->spriteSurface[s0_sprite] = IMG_Load("/usr/local/share/solver/s0.png");
	gamedata->spriteSurface[s1_sprite] = IMG_Load("/usr/local/share/solver/s1.png");
	gamedata->spriteSurface[s2_sprite] = IMG_Load("/usr/local/share/solver/s2.png");
	gamedata->spriteSurface[s3_sprite] = IMG_Load("/usr/local/share/solver/s3.png");
	gamedata->spriteSurface[s4_sprite] = IMG_Load("/usr/local/share/solver/s4.png");
	gamedata->spriteSurface[s5_sprite] = IMG_Load("/usr/local/share/solver/s5.png");
	gamedata->spriteSurface[s6_sprite] = IMG_Load("/usr/local/share/solver/s6.png");
	gamedata->spriteSurface[s7_sprite] = IMG_Load("/usr/local/share/solver/s7.png");
	gamedata->spriteSurface[s8_sprite] = IMG_Load("/usr/local/share/solver/s8.png");
	gamedata->spriteSurface[s9_sprite] = IMG_Load("/usr/local/share/solver/s9.png");
	gamedata->spriteSurface[sblank_sprite] = IMG_Load("/usr/local/share/solver/sblank.png");
	gamedata->spriteSurface[scolon_sprite] = IMG_Load("/usr/local/share/solver/scolon.png");

	gamedata->spriteSurface[key0_sprite] = IMG_Load("/usr/local/share/solver/key_0.png");
	gamedata->spriteSurface[key1_sprite] = IMG_Load("/usr/local/share/solver/key_1.png");
	gamedata->spriteSurface[key2_sprite] = IMG_Load("/usr/local/share/solver/key_2.png");
	gamedata->spriteSurface[key3_sprite] = IMG_Load("/usr/local/share/solver/key_3.png");
	gamedata->spriteSurface[key4_sprite] = IMG_Load("/usr/local/share/solver/key_4.png");
	gamedata->spriteSurface[key5_sprite] = IMG_Load("/usr/local/share/solver/key_5.png");
	gamedata->spriteSurface[key6_sprite] = IMG_Load("/usr/local/share/solver/key_6.png");
	gamedata->spriteSurface[key7_sprite] = IMG_Load("/usr/local/share/solver/key_7.png");
	gamedata->spriteSurface[key8_sprite] = IMG_Load("/usr/local/share/solver/key_8.png");
	gamedata->spriteSurface[key9_sprite] = IMG_Load("/usr/local/share/solver/key_9.png");
	gamedata->spriteSurface[keya_sprite] = IMG_Load("/usr/local/share/solver/key_a.png");
	gamedata->spriteSurface[keyb_sprite] = IMG_Load("/usr/local/share/solver/key_b.png");
	gamedata->spriteSurface[keyc_sprite] = IMG_Load("/usr/local/share/solver/key_c.png");
	gamedata->spriteSurface[keyd_sprite] = IMG_Load("/usr/local/share/solver/key_d.png");
	gamedata->spriteSurface[keye_sprite] = IMG_Load("/usr/local/share/solver/key_e.png");
	gamedata->spriteSurface[keyf_sprite] = IMG_Load("/usr/local/share/solver/key_f.png");
	gamedata->spriteSurface[keyg_sprite] = IMG_Load("/usr/local/share/solver/key_g.png");
	gamedata->spriteSurface[keyh_sprite] = IMG_Load("/usr/local/share/solver/key_h.png");
	gamedata->spriteSurface[keyi_sprite] = IMG_Load("/usr/local/share/solver/key_i.png");
	gamedata->spriteSurface[keyj_sprite] = IMG_Load("/usr/local/share/solver/key_j.png");
	gamedata->spriteSurface[keyk_sprite] = IMG_Load("/usr/local/share/solver/key_k.png");
	gamedata->spriteSurface[keyl_sprite] = IMG_Load("/usr/local/share/solver/key_l.png");
	gamedata->spriteSurface[keym_sprite] = IMG_Load("/usr/local/share/solver/key_m.png");
	gamedata->spriteSurface[keyn_sprite] = IMG_Load("/usr/local/share/solver/key_n.png");
	gamedata->spriteSurface[keyo_sprite] = IMG_Load("/usr/local/share/solver/key_o.png");
	gamedata->spriteSurface[keyp_sprite] = IMG_Load("/usr/local/share/solver/key_p.png");
	gamedata->spriteSurface[keyq_sprite] = IMG_Load("/usr/local/share/solver/key_q.png");
	gamedata->spriteSurface[keyr_sprite] = IMG_Load("/usr/local/share/solver/key_r.png");
	gamedata->spriteSurface[keys_sprite] = IMG_Load("/usr/local/share/solver/key_s.png");
	gamedata->spriteSurface[keyt_sprite] = IMG_Load("/usr/local/share/solver/key_t.png");
	gamedata->spriteSurface[keyu_sprite] = IMG_Load("/usr/local/share/solver/key_u.png");
	gamedata->spriteSurface[keyv_sprite] = IMG_Load("/usr/local/share/solver/key_v.png");
	gamedata->spriteSurface[keyw_sprite] = IMG_Load("/usr/local/share/solver/key_w.png");
	gamedata->spriteSurface[keyx_sprite] = IMG_Load("/usr/local/share/solver/key_x.png");
	gamedata->spriteSurface[keyy_sprite] = IMG_Load("/usr/local/share/solver/key_y.png");
	gamedata->spriteSurface[keyz_sprite] = IMG_Load("/usr/local/share/solver/key_z.png");
	gamedata->spriteSurface[keypointer_sprite] = IMG_Load("/usr/local/share/solver/key_pointer.png");
	gamedata->spriteSurface[keyblank_sprite] = IMG_Load("/usr/local/share/solver/key_blank.png");

	gamedata->spriteSurface[labelscore_sprite] = IMG_Load("/usr/local/share/solver/label_score.png");
	gamedata->spriteSurface[labelspeed_sprite] = IMG_Load("/usr/local/share/solver/label_speed.png");
	gamedata->spriteSurface[labeltime_sprite] = IMG_Load("/usr/local/share/solver/label_time.png");
	gamedata->spriteSurface[labelhit_sprite] = IMG_Load("/usr/local/share/solver/label_hit.png");
	gamedata->spriteSurface[labelmiss_sprite] = IMG_Load("/usr/local/share/solver/label_miss.png");
	gamedata->spriteSurface[labeltotal_sprite] = IMG_Load("/usr/local/share/solver/label_total.png");

	gamedata->spriteSurface[blockdivide0_sprite] = IMG_Load("/usr/local/share/solver/block_divide0.png");
	gamedata->spriteSurface[blockdivide1_sprite] = IMG_Load("/usr/local/share/solver/block_divide1.png");
	gamedata->spriteSurface[blockdivide2_sprite] = IMG_Load("/usr/local/share/solver/block_divide2.png");
	gamedata->spriteSurface[blockdivide3_sprite] = IMG_Load("/usr/local/share/solver/block_divide3.png");
	gamedata->spriteSurface[blockdivide4_sprite] = IMG_Load("/usr/local/share/solver/block_divide4.png");
	gamedata->spriteSurface[blockdivide5_sprite] = IMG_Load("/usr/local/share/solver/block_divide5.png");
	gamedata->spriteSurface[blockdivide6_sprite] = IMG_Load("/usr/local/share/solver/block_divide6.png");
	gamedata->spriteSurface[blockdivide7_sprite] = IMG_Load("/usr/local/share/solver/block_divide7.png");
	gamedata->spriteSurface[blockdivide8_sprite] = IMG_Load("/usr/local/share/solver/block_divide8.png");
	gamedata->spriteSurface[blockdivide9_sprite] = IMG_Load("/usr/local/share/solver/block_divide9.png");

	gamedata->spriteSurface[blockminus0_sprite] = IMG_Load("/usr/local/share/solver/block_minus0.png");
	gamedata->spriteSurface[blockminus1_sprite] = IMG_Load("/usr/local/share/solver/block_minus1.png");
	gamedata->spriteSurface[blockminus2_sprite] = IMG_Load("/usr/local/share/solver/block_minus2.png");
	gamedata->spriteSurface[blockminus3_sprite] = IMG_Load("/usr/local/share/solver/block_minus3.png");
	gamedata->spriteSurface[blockminus4_sprite] = IMG_Load("/usr/local/share/solver/block_minus4.png");
	gamedata->spriteSurface[blockminus5_sprite] = IMG_Load("/usr/local/share/solver/block_minus5.png");
	gamedata->spriteSurface[blockminus6_sprite] = IMG_Load("/usr/local/share/solver/block_minus6.png");
	gamedata->spriteSurface[blockminus7_sprite] = IMG_Load("/usr/local/share/solver/block_minus7.png");
	gamedata->spriteSurface[blockminus8_sprite] = IMG_Load("/usr/local/share/solver/block_minus8.png");
	gamedata->spriteSurface[blockminus9_sprite] = IMG_Load("/usr/local/share/solver/block_minus9.png");

	gamedata->spriteSurface[blockmultiply0_sprite] = IMG_Load("/usr/local/share/solver/block_multiply0.png");
	gamedata->spriteSurface[blockmultiply1_sprite] = IMG_Load("/usr/local/share/solver/block_multiply1.png");
	gamedata->spriteSurface[blockmultiply2_sprite] = IMG_Load("/usr/local/share/solver/block_multiply2.png");
	gamedata->spriteSurface[blockmultiply3_sprite] = IMG_Load("/usr/local/share/solver/block_multiply3.png");
	gamedata->spriteSurface[blockmultiply4_sprite] = IMG_Load("/usr/local/share/solver/block_multiply4.png");
	gamedata->spriteSurface[blockmultiply5_sprite] = IMG_Load("/usr/local/share/solver/block_multiply5.png");
	gamedata->spriteSurface[blockmultiply6_sprite] = IMG_Load("/usr/local/share/solver/block_multiply6.png");
	gamedata->spriteSurface[blockmultiply7_sprite] = IMG_Load("/usr/local/share/solver/block_multiply7.png");
	gamedata->spriteSurface[blockmultiply8_sprite] = IMG_Load("/usr/local/share/solver/block_multiply8.png");
	gamedata->spriteSurface[blockmultiply9_sprite] = IMG_Load("/usr/local/share/solver/block_multiply9.png");

	gamedata->spriteSurface[blockplus0_sprite] = IMG_Load("/usr/local/share/solver/block_plus0.png");
	gamedata->spriteSurface[blockplus1_sprite] = IMG_Load("/usr/local/share/solver/block_plus1.png");
	gamedata->spriteSurface[blockplus2_sprite] = IMG_Load("/usr/local/share/solver/block_plus2.png");
	gamedata->spriteSurface[blockplus3_sprite] = IMG_Load("/usr/local/share/solver/block_plus3.png");
	gamedata->spriteSurface[blockplus4_sprite] = IMG_Load("/usr/local/share/solver/block_plus4.png");
	gamedata->spriteSurface[blockplus5_sprite] = IMG_Load("/usr/local/share/solver/block_plus5.png");
	gamedata->spriteSurface[blockplus6_sprite] = IMG_Load("/usr/local/share/solver/block_plus6.png");
	gamedata->spriteSurface[blockplus7_sprite] = IMG_Load("/usr/local/share/solver/block_plus7.png");
	gamedata->spriteSurface[blockplus8_sprite] = IMG_Load("/usr/local/share/solver/block_plus8.png");
	gamedata->spriteSurface[blockplus9_sprite] = IMG_Load("/usr/local/share/solver/block_plus9.png");

	gamedata->spriteSurface[blockselect_sprite] = IMG_Load("/usr/local/share/solver/block_select.png");
	gamedata->spriteSurface[blockblank_sprite] = IMG_Load("/usr/local/share/solver/block_blank.png");
	gamedata->spriteSurface[blockmeter_sprite] = IMG_Load("/usr/local/share/solver/block_meter.png");
	gamedata->spriteSurface[blockmeterblank_sprite] = IMG_Load("/usr/local/share/solver/block_meterblank.png");
	gamedata->spriteSurface[blockmetertopbottom_sprite] = IMG_Load("/usr/local/share/solver/block_topbottom.png");

	gamedata->spriteSurface[eq0_sprite] = IMG_Load("/usr/local/share/solver/eq_0.png");
	gamedata->spriteSurface[eq1_sprite] = IMG_Load("/usr/local/share/solver/eq_1.png");
	gamedata->spriteSurface[eq2_sprite] = IMG_Load("/usr/local/share/solver/eq_2.png");
	gamedata->spriteSurface[eq3_sprite] = IMG_Load("/usr/local/share/solver/eq_3.png");
	gamedata->spriteSurface[eq4_sprite] = IMG_Load("/usr/local/share/solver/eq_4.png");
	gamedata->spriteSurface[eq5_sprite] = IMG_Load("/usr/local/share/solver/eq_5.png");
	gamedata->spriteSurface[eq6_sprite] = IMG_Load("/usr/local/share/solver/eq_6.png");
	gamedata->spriteSurface[eq7_sprite] = IMG_Load("/usr/local/share/solver/eq_7.png");
	gamedata->spriteSurface[eq8_sprite] = IMG_Load("/usr/local/share/solver/eq_8.png");
	gamedata->spriteSurface[eq9_sprite] = IMG_Load("/usr/local/share/solver/eq_9.png");

	gamedata->spriteSurface[eqequal_sprite] = IMG_Load("/usr/local/share/solver/eq_equal.png");
	gamedata->spriteSurface[eqplus_sprite] = IMG_Load("/usr/local/share/solver/eq_plus.png");
	gamedata->spriteSurface[eqminus_sprite] = IMG_Load("/usr/local/share/solver/eq_minus.png");
	gamedata->spriteSurface[eqdivide_sprite] = IMG_Load("/usr/local/share/solver/eq_divide.png");
	gamedata->spriteSurface[eqmultiply_sprite] = IMG_Load("/usr/local/share/solver/eq_multiply.png");

	gamedata->spriteSurface[bordertopleft_sprite] = IMG_Load("/usr/local/share/solver/bordertopleft.png");
	gamedata->spriteSurface[borderleftright_sprite] = IMG_Load("/usr/local/share/solver/borderleftright.png");
	gamedata->spriteSurface[bordertopright_sprite] = IMG_Load("/usr/local/share/solver/bordertopright.png");
	gamedata->spriteSurface[borderdownleft_sprite] = IMG_Load("/usr/local/share/solver/borderdownleft.png");
	gamedata->spriteSurface[borderupdown_sprite] = IMG_Load("/usr/local/share/solver/borderupdown.png");
	gamedata->spriteSurface[borderdownright_sprite] = IMG_Load("/usr/local/share/solver/borderdownright.png");
	gamedata->spriteSurface[gameover_sprite] = IMG_Load("/usr/local/share/solver/gameover.png");

	gamedata->spriteSurface[background0_sprite] = IMG_Load("/usr/local/share/solver/bg0.png");
	gamedata->spriteSurface[background1_sprite] = IMG_Load("/usr/local/share/solver/bg1.png");
	gamedata->spriteSurface[background2_sprite] = IMG_Load("/usr/local/share/solver/bg2.png");
	gamedata->spriteSurface[background3_sprite] = IMG_Load("/usr/local/share/solver/bg3.png");
	gamedata->spriteSurface[background4_sprite] = IMG_Load("/usr/local/share/solver/bg4.png");

	gamedata->spriteSurface[combo0_sprite] = IMG_Load("/usr/local/share/solver/combo0.png");
	gamedata->spriteSurface[combo1_sprite] = IMG_Load("/usr/local/share/solver/combo1.png");
	gamedata->spriteSurface[combo2_sprite] = IMG_Load("/usr/local/share/solver/combo2.png");
	gamedata->spriteSurface[combo3_sprite] = IMG_Load("/usr/local/share/solver/combo3.png");
	gamedata->spriteSurface[combo4_sprite] = IMG_Load("/usr/local/share/solver/combo4.png");
	gamedata->spriteSurface[combo5_sprite] = IMG_Load("/usr/local/share/solver/combo5.png");
	gamedata->spriteSurface[combo6_sprite] = IMG_Load("/usr/local/share/solver/combo6.png");
	gamedata->spriteSurface[combo7_sprite] = IMG_Load("/usr/local/share/solver/combo7.png");
	gamedata->spriteSurface[combo8_sprite] = IMG_Load("/usr/local/share/solver/combo8.png");
	gamedata->spriteSurface[combo9_sprite] = IMG_Load("/usr/local/share/solver/combo9.png");
	gamedata->spriteSurface[combo10_sprite] = IMG_Load("/usr/local/share/solver/combo10.png");

	gamedata->controls[ACT_UP] = SDLK_UP;
	gamedata->controls[ACT_DOWN] = SDLK_DOWN;
	gamedata->controls[ACT_LEFT] = SDLK_LEFT;
	gamedata->controls[ACT_RIGHT] = SDLK_RIGHT;
	gamedata->controls[ACT_0] = SDLK_0;
	gamedata->controls[ACT_1] = SDLK_1;
	gamedata->controls[ACT_2] = SDLK_2;
	gamedata->controls[ACT_3] = SDLK_3;
	gamedata->controls[ACT_4] = SDLK_4;
	gamedata->controls[ACT_5] = SDLK_5;
	gamedata->controls[ACT_6] = SDLK_6;
	gamedata->controls[ACT_7] = SDLK_7;
	gamedata->controls[ACT_8] = SDLK_8;
	gamedata->controls[ACT_9] = SDLK_9;
	gamedata->controls[ACT_CLEAR] = SDLK_SPACE;
	gamedata->controls[ACT_SUBMIT] = SDLK_RETURN;
	gamedata->controls[ACT_QUIT] = SDLK_ESCAPE;
	gamedata->current_control = ACT_UP;

	for (int j = 0; j < 17;j++) {
		convert_keyname(gamedata, gamedata->controls[gamedata->current_control]);
		gamedata->current_control = gamedata->current_control + 1;
	}

	gamedata->current_control = ACT_UP;
	gamedata->spriteGameText[GAME_SCORE] = gamedata->spriteGameTextScore;
	gamedata->spriteGameText[GAME_SPEED] = gamedata->spriteGameTextSpeed;
	gamedata->spriteGameText[GAME_TIME] = gamedata->spriteGameTextTime;
	gamedata->spriteGameText[GAME_HIT] = gamedata->spriteGameTextHit;
	gamedata->spriteGameText[GAME_MISS] = gamedata->spriteGameTextMiss;
	gamedata->spriteGameText[GAME_TOTAL] = gamedata->spriteGameTextTotal;

	assign_gamevalue(gamedata, GAME_SCORE, "000000000");
	assign_gamevalue(gamedata, GAME_SPEED, "00");
	assign_gamevalue(gamedata, GAME_TIME, "00:00:00");
	assign_gamevalue(gamedata, GAME_HIT, "000000000");
	assign_gamevalue(gamedata, GAME_MISS, "000000000");
	assign_gamevalue(gamedata, GAME_TOTAL, "000000000");

	for (int i = 0; i<num_pieces_horizontal;i++) { 
		for (int j = 0; j<num_pieces_vertical;j++) {
			gamedata->grid[i][j] = BLANK;
		}
	}

	for (int i = 0; i < NUM_SOUNDS; i++) {
		gamedata->sfx[i] = NULL;
	}

	gamedata->sfx[ANSWER_CORRECT] = Mix_LoadWAV("/usr/local/share/solver/answer_correct.wav");
	gamedata->sfx[ANSWER_TYPING] = Mix_LoadWAV("/usr/local/share/solver/answer_typing.wav");
	gamedata->sfx[ANSWER_WRONG] = Mix_LoadWAV("/usr/local/share/solver/answer_wrong.wav");
	gamedata->sfx[PIECE_FLIP] = Mix_LoadWAV("/usr/local/share/solver/piece_flip.wav");
	gamedata->sfx[MOVE_CURSOR] = Mix_LoadWAV("/usr/local/share/solver/move_cursor.wav");
	gamedata->sfx[MENU_SELECT] = Mix_LoadWAV("/usr/local/share/solver/menu_select.wav");

	for (int i = 0; i < NUM_SPRITES; i++) {
		if (gamedata->spriteSurface[i] == NULL || gamedata->spriteSurface[i]->w != imgcheck[i][0] || gamedata->spriteSurface[i]->h != imgcheck[i][1] || gamedata->spriteSurface[i]->pitch != imgcheck[i][2]) {
			printf("Error loading images. %i\n", i);
			gamedata->exit = 1;
			return 0;
		}
	}

	for (int i = 0; i < NUM_SOUNDS; i++) {
		if (gamedata->sfx[i] == NULL || gamedata->sfx[i]->alen != sfxcheck[i]) {
			printf("Error loading sound effects.\n");
			gamedata->exit = 1;
			return 0;
		}
	}

	return 0;
}

// Take a number from a generated equation.
// Convert the number to the corresponding sprite and return the value.
int process_equationvalue(int value) {
	switch (value) {
		case 0:
			value = eq0_sprite;
			break;
		case 1:
			value = eq1_sprite;
			break;
		case 2:
			value = eq2_sprite;
			break;
		case 3:
			value = eq3_sprite;
			break;
		case 4:
			value = eq4_sprite;
			break;
		case 5:
			value = eq5_sprite;
			break;
		case 6:
			value = eq6_sprite;
			break;
		case 7:
			value = eq7_sprite;
			break;
		case 8:
			value = eq8_sprite;
			break;
		case 9:
			value = eq9_sprite;
			break;
		default:
			value = eq0_sprite;
			break;
	}

	return value;
}

// Display a notification of a combo being performed.
// After a certain amount of seconds display thenext frame in the animation.
// If the animation is on the last frame then reset the animation status, timers and current animate/sprite frame.
int display_combo(struct data *gamedata) {
	int borderspritesize = gamedata->spriteSurface[background0_sprite]->w;
	int x = (borderspritesize * 12) - 32;
	int y = (borderspritesize * 4) + 16;
	SDL_Rect combo;
	int animation_time = 0;

	combo.x = x;
	combo.y = y;
	gamedata->time[ANIMATECOMBO_CURRENT_TIME] = SDL_GetTicks64();
	animation_time = (gamedata->time[ANIMATECOMBO_CURRENT_TIME] - gamedata->time[ANIMATECOMBO_START_TIME]);

	if (gamedata->animate_combo == 1) {
		SDL_BlitSurface(gamedata->spriteSurface[gamedata->animate_combo_frame], NULL, gamedata->screenSurface, &combo);
		if (gamedata->animate_combo_frame > combo0_sprite) {
			if (animation_time > 1) {
				gamedata->time[ANIMATECOMBO_START_TIME] = SDL_GetTicks64();
				gamedata->time[ANIMATECOMBO_CURRENT_TIME] = SDL_GetTicks64();
				gamedata->animate_combo_frame--;
			}
		} else {
			if (animation_time > 500) {
				gamedata->animate_combo = 0;
				gamedata->time[ANIMATECOMBO_START_TIME] = SDL_GetTicks64();
				gamedata->time[ANIMATECOMBO_CURRENT_TIME] = SDL_GetTicks64();
				gamedata->animate_combo_frame = combo10_sprite;
			}
		}
	}

	return 0;
}

// Take a background sprite set in the options screen.
// Calculate the number of rows and cols needed to fill up the screen with the background sprite.
// Set the background in game to the sprite set in the options screen.
// If the background number is one of the valid backgrounds then fill the game screen with the background sprite.
int display_background(struct data *gamedata) {
	int x = 0;
	int y = 0;
	SDL_Rect background[221];
	int background_sprite = background1_sprite;
	int background_number = gamedata->option[2];
	int current_backgroundtile = 0;
	int maxrow = gamedata->screenHeight / gamedata->spriteSurface[background_sprite]->h;
	int maxcol = gamedata->screenWidth / gamedata->spriteSurface[background_sprite]->w;

	background[current_backgroundtile].x = x;
	background[current_backgroundtile].y = y;

	switch (background_number) {
		case 0:
			background_sprite = background0_sprite;
			break;
		case 1:
			background_sprite = background1_sprite;
			break;
		case 2:
			background_sprite = background2_sprite;
			break;
		case 3:
			background_sprite = background3_sprite;
			break;
		case 4:
			background_sprite = background4_sprite;
			break;
		default:
			background_sprite = background0_sprite;
			break;
	}

	if (background_number >= 0 && background_number < 5) {
		for (int row = 0; row < maxrow; row++) {
			x = 0;
			for (int col=0; col < maxcol; col++) {
				background[current_backgroundtile].x = x;
				background[current_backgroundtile].y = y;
				SDL_BlitSurface(gamedata->spriteSurface[background_sprite], NULL, gamedata->screenSurface, &background[current_backgroundtile]);
				current_backgroundtile++;
				x = x + gamedata->spriteSurface[background_sprite]->w;
			}
			y = y + gamedata->spriteSurface[background_sprite]->h;
		}
	}

	return 0;
}

// Display the controls header.
// Display the control labels for each of the controls.
// Display the control values for each of the controls.
// Display the cursor for the current control being set or changed.
int display_controlscreen(struct data *gamedata) {	
	int x = 0;
	int y = 0;
	int current_character = 0;
	int max = 851;
	SDL_Rect controls_label;
	SDL_Rect scursor;
	SDL_Rect controls[max];
	int align_text = (gamedata->screenWidth/2)-(gamedata->spriteSurface[controls_sprite]->w/2)+(gamedata->spriteSurface[keym_sprite]->w*10);

	controls_label.x = (gamedata->screenWidth/2)-(gamedata->spriteSurface[controls_sprite]->w/2);
	y = controls_label.y = y;

	assign_control(gamedata, 17, "up");
	assign_control(gamedata, 18, "down");
	assign_control(gamedata, 19, "left");
	assign_control(gamedata, 20, "right");
	assign_control(gamedata, 21, "0");
	assign_control(gamedata, 22, "1");
	assign_control(gamedata, 23, "2");
	assign_control(gamedata, 24, "3");
	assign_control(gamedata, 25, "4");
	assign_control(gamedata, 26, "5");
	assign_control(gamedata, 27, "6");
	assign_control(gamedata, 28, "7");
	assign_control(gamedata, 29, "8");
	assign_control(gamedata, 30, "9");
	assign_control(gamedata, 31, "clear");
	assign_control(gamedata, 32, "submit");
	assign_control(gamedata, 33, "quit");

	SDL_BlitSurface(gamedata->spriteSurface[controls_sprite], NULL, gamedata->screenSurface, &controls_label);

	x = align_text;
	y = (gamedata->screenHeight/8);

	for (int i = 0, j = 0; i < max; i++) {
		if (i == 17*25) {
			y = (gamedata->screenHeight/8);
		}

		controls[i].x = x;
		controls[i].y = y;
		x = x + gamedata->spriteSurface[key0_sprite]->w+1;
		j++;

		if (j == 25) {
			j = 0;
			if (i < 16*25) {
				x = align_text;
			} else {
				x = gamedata->spriteSurface[key0_sprite]->w*25;
			}
			y = y+gamedata->spriteSurface[key0_sprite]->h+2;
		}

	}

	scursor.x = controls[gamedata->current_control*25].x - 32;
	scursor.y = controls[gamedata->current_control*25].y;

	for (int i = 0; i < 34; i++) {
		for (int j = 0; j < 25; j++) {
			SDL_BlitSurface(gamedata->spriteSurface[(gamedata->spriteTextNum[i][j])], NULL, gamedata->screenSurface, &controls[current_character]);
			current_character = current_character+1;
		}
	}

	SDL_BlitSurface(gamedata->spriteSurface[scursor_sprite], NULL, gamedata->screenSurface, &scursor);

	return 0;
}

// Convert the text for the options into option labels
// Convert the option values from strings to integers.
// Display the options header.
// Display the options labels for each of the options.
// Display the options values for each of the options.
// Display the cursor for the current option being set or changed.
// If setting an option that can have a value greater than 9 display a smaller cursor to track which part of the option is selected.
int display_optionscreen(struct data *gamedata) {
	int x = 0;
	int y = 0;
	int current_character = 0;
	char *stringbuffer = malloc(25);
	int max = 400;
	SDL_Rect options[max];
	int align_text = (gamedata->screenWidth/2)-(gamedata->spriteSurface[keym_sprite]->w*12);
	int selection = gamedata->optionselection*25;
	SDL_Rect scursor;
	SDL_Rect sub_cursor;
	int show_subcursor = 0;
	SDL_Rect options_label;

	options_label.x = (gamedata->screenWidth/2)-(gamedata->spriteSurface[options_sprite]->w/2);
	y = options_label.y = y;

	assign_control(gamedata, 34, "max number");
	assign_control(gamedata, 35, "speed");
	assign_control(gamedata, 36, "background");
	assign_control(gamedata, 37, "time limit");
	assign_control(gamedata, 38, "score limit");
	assign_control(gamedata, 39, "hit limit");
	assign_control(gamedata, 40, "miss limit");
	assign_control(gamedata, 41, "total limit");

	x = align_text;
	y = (gamedata->screenHeight/8);

	snprintf(stringbuffer, 2, "%d", gamedata->option[0]);
	assign_control(gamedata, 42, stringbuffer);

	snprintf(stringbuffer, 2, "%d", gamedata->option[1]);
	assign_control(gamedata, 43, stringbuffer);

	snprintf(stringbuffer, 2, "%d", gamedata->option[2]);
	assign_control(gamedata, 44, stringbuffer);

	snprintf(stringbuffer, 9, "%d%d%d%d%d%d", gamedata->timearray[GAME_HOUR1], gamedata->timearray[GAME_HOUR2], gamedata->timearray[GAME_MINUTE1], gamedata->timearray[GAME_MINUTE2], gamedata->timearray[GAME_SECOND1], gamedata->timearray[GAME_SECOND2]);
	assign_control(gamedata, 45, stringbuffer);

	snprintf(stringbuffer, 10, "%d%d%d%d%d%d%d%d%d", gamedata->scorearray[1], gamedata->scorearray[2], gamedata->scorearray[3],
	gamedata->scorearray[4], gamedata->scorearray[5], gamedata->scorearray[6], gamedata->scorearray[7], gamedata->scorearray[8],
	gamedata->scorearray[9]);
	assign_control(gamedata, 46, stringbuffer);

	snprintf(stringbuffer, 10, "%d%d%d%d%d%d%d%d%d", gamedata->hitarray[1], gamedata->hitarray[2], gamedata->hitarray[3],
	gamedata->hitarray[4], gamedata->hitarray[5], gamedata->hitarray[6], gamedata->hitarray[7], gamedata->hitarray[8],
	gamedata->hitarray[9]);
	assign_control(gamedata, 47, stringbuffer);

	snprintf(stringbuffer, 10, "%d%d%d%d%d%d%d%d%d", gamedata->missarray[1], gamedata->missarray[2], gamedata->missarray[3],
	gamedata->missarray[4], gamedata->missarray[5], gamedata->missarray[6], gamedata->missarray[7], gamedata->missarray[8],
	gamedata->missarray[9]);
	assign_control(gamedata, 48, stringbuffer);

	snprintf(stringbuffer, 10, "%d%d%d%d%d%d%d%d%d", gamedata->totalarray[1],gamedata->totalarray[2],gamedata->totalarray[3],
	gamedata->totalarray[4],gamedata->totalarray[5], gamedata->totalarray[6],gamedata->totalarray[7],gamedata->totalarray[8],
	gamedata->totalarray[9]);
	assign_control(gamedata, 49, stringbuffer);

	free(stringbuffer);

	for (int i = 0,j = 0; i < max; i++) {
		if (i == 8*25) {
			y = (gamedata->screenHeight/8);
		}

		options[i].x = x;
		options[i].y = y;
		x = x + gamedata->spriteSurface[key0_sprite]->w+1;
		j++;

		if (j == 25) {
			j = 0;
			if (i < 7*25) {
				x = align_text;
			} else {
				x = align_text+gamedata->spriteSurface[key0_sprite]->w*25;
			}
			y = y+gamedata->spriteSurface[key0_sprite]->h*2;
		}

	}

	SDL_BlitSurface(gamedata->spriteSurface[options_sprite], NULL, gamedata->screenSurface, &options_label);

	for (int i = 34; i < 50; i++) {
		for (int j = 0; j < 25; j++) {
			SDL_BlitSurface(gamedata->spriteSurface[(gamedata->spriteTextNum[i][j])], NULL, gamedata->screenSurface, &options[current_character]);
			current_character = current_character+1;
		}
	}

	scursor.x = options[selection].x - gamedata->spriteSurface[scursor_sprite]->w*2;
	scursor.y = options[selection].y;
	SDL_BlitSurface(gamedata->spriteSurface[scursor_sprite], NULL, gamedata->screenSurface, &scursor);

	switch (gamedata->optionselection) {
		case 3:
			if (gamedata->timeoption > 0 && gamedata->timeoption <= 6) {
				show_subcursor = 1;
				sub_cursor.x = options[274+gamedata->timeoption].x;
				sub_cursor.y = options[274+gamedata->timeoption].y+gamedata->spriteSurface[key0_sprite]->h;
			}
			break;
		case 4:
			if (gamedata->scoreoption > 0 && gamedata->scoreoption <= 9) {
				show_subcursor = 1;
				sub_cursor.x = options[299+gamedata->scoreoption].x;
				sub_cursor.y = options[299+gamedata->scoreoption].y+gamedata->spriteSurface[key0_sprite]->h;
			}
			break;
		case 5:
			if (gamedata->hitoption > 0 && gamedata->hitoption <= 9) {
				show_subcursor = 1;
				sub_cursor.x = options[324+gamedata->hitoption].x;
				sub_cursor.y = options[324+gamedata->hitoption].y+gamedata->spriteSurface[key0_sprite]->h;
			}
			break;
		case 6:
			if (gamedata->missoption > 0 && gamedata->missoption <= 9) {
				show_subcursor = 1;
				sub_cursor.x = options[349+gamedata->missoption].x;
				sub_cursor.y = options[349+gamedata->missoption].y+gamedata->spriteSurface[key0_sprite]->h;
			}
			break;
		case 7:
			if (gamedata->totaloption > 0 && gamedata->totaloption <= 9) {
				show_subcursor = 1;
				sub_cursor.x = options[374+gamedata->totaloption].x;
				sub_cursor.y = options[374+gamedata->totaloption].y+gamedata->spriteSurface[key0_sprite]->h;
			}
			break;			
	}

	if (show_subcursor) {
		SDL_BlitSurface(gamedata->spriteSurface[keypointer_sprite], NULL, gamedata->screenSurface, &sub_cursor);
	}

	return 0;
}

// Display the title of the program.
// Display the title options.
// Display a cursor for which title option is selected currently.
int display_titlescreen(struct data *gamedata) {	
	int x = 0;
	int y = 0;
	SDL_Rect labels[4];
	SDL_Rect lcursor;

	x = labels[0].x = ((gamedata->screenWidth)/2)-((gamedata->spriteSurface[title_sprite]->w)/2);
	y = labels[0].y = ((gamedata->screenHeight)/3)-((gamedata->spriteSurface[title_sprite]->h)/2);

	labels[1].x = x+gamedata->spriteSurface[start_sprite]->w;
	y = labels[1].y = y+gamedata->spriteSurface[title_sprite]->h+gamedata->spriteSurface[start_sprite]->h;

	labels[2].x = x+gamedata->spriteSurface[start_sprite]->w;
	y = labels[2].y = y+gamedata->spriteSurface[start_sprite]->h;

	labels[3].x = x+gamedata->spriteSurface[start_sprite]->w;
	labels[3].y = y+gamedata->spriteSurface[options_sprite]->h;

	SDL_BlitSurface(gamedata->spriteSurface[title_sprite], NULL, gamedata->screenSurface, &labels[0]);
	SDL_BlitSurface(gamedata->spriteSurface[start_sprite], NULL, gamedata->screenSurface, &labels[1]);
	SDL_BlitSurface(gamedata->spriteSurface[options_sprite], NULL, gamedata->screenSurface, &labels[2]);
	SDL_BlitSurface(gamedata->spriteSurface[controls_sprite], NULL, gamedata->screenSurface, &labels[3]);

	lcursor.x = x + gamedata->spriteSurface[lcursor_sprite]->w;

	if (gamedata->menu == START) {
		lcursor.y = labels[1].y;
		SDL_BlitSurface(gamedata->spriteSurface[lcursor_sprite], NULL, gamedata->screenSurface, &lcursor);
	} else if (gamedata->menu == OPTION) {
		lcursor.y = labels[2].y;
		SDL_BlitSurface(gamedata->spriteSurface[lcursor_sprite], NULL, gamedata->screenSurface, &lcursor);
	} else if (gamedata->menu == CONFIG) {
		lcursor.y = labels[3].y;
		SDL_BlitSurface(gamedata->spriteSurface[lcursor_sprite], NULL, gamedata->screenSurface, &lcursor);
	}

	return 0;
}

// For the top or bottom of the meter set a sprite to denote the top/bottom.
// For each space in the grid mark it as blank or non-blank.
// Display the top, bottom, blank or non-blank sprite.
int display_gamepiecemeter(struct data *gamedata) {
	int borderspritesize = gamedata->spriteSurface[background0_sprite]->w;
	int x = 0;
	int startx = (borderspritesize * 10) - 32;
	int y = (borderspritesize * 5) - 32;
	int current_meterpiece = 0;
	SDL_Rect meter[81];
	int currentsprite = blockmeter_sprite;

	for (int i = 0; i < 20; i++) {
		x = startx - gamedata->spriteSurface[blockmeter_sprite]->w*8;
		for (int j = 0; j < 4; j++) {
			meter[current_meterpiece].x = x;
			meter[current_meterpiece].y = y;
			if (i == 0 || i == 19) {
				currentsprite = blockmetertopbottom_sprite;
			} else {
				if (gamedata->grid[j][i] == BLANK) {
					currentsprite = blockmeterblank_sprite;
				} else {
					currentsprite = blockmeter_sprite;
				}
			}
			SDL_BlitSurface(gamedata->spriteSurface[currentsprite], NULL, gamedata->screenSurface, &meter[current_meterpiece]);
			x = x + gamedata->spriteSurface[blockmeter_sprite]->w;
			current_meterpiece++;
		}
		y = y + gamedata->spriteSurface[blockmeter_sprite]->h;

	}

	return 0;
}

// Check if a piece is animating and has not finished.
// Decrease the frame count.
// Play a sound effect of the piece being flipped on the first frame.
// If the piece is blank, set the block to blank and reset the animation and frame count.
// For each of the four pieces, use the current frame count to calculate which piece sprite should be displayed.
int animate_gamepieces(struct data *gamedata) {
	for (int i = 0; i < 4; i++) {
		if (gamedata->animate_piece[i] == 1 && gamedata->block_frame_count[i] >= 0) {
			if (gamedata->block_frame_count[i] <= 9) {
				gamedata->block_frame_count[i] = gamedata->block_frame_count[i] - 1;
				if (gamedata->block_frame_count[i] == 0) {
					process_sfx(gamedata, PIECE_FLIP);
				}
			}
		} else if (gamedata->grid[i][18] == BLANK) {
			gamedata->block_frame[i] = blockblank_sprite;
			gamedata->animate_piece[i] = 1;
			gamedata->block_frame_count[i] = 9;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (gamedata->block_frame_count[i] >= 0 && gamedata->block_frame_count[i] <= 9) {
				switch (gamedata->grid[i][18]) {
					case DIVIDE:
						gamedata->block_frame[i] = blockdivide0_sprite + gamedata->block_frame_count[i];
						break;
					case MINUS:
						gamedata->block_frame[i] = blockminus0_sprite + gamedata->block_frame_count[i];
						break;
					case MULTIPLY:
						gamedata->block_frame[i] = blockmultiply0_sprite + gamedata->block_frame_count[i];
						break;
					case PLUS:
						gamedata->block_frame[i] = blockplus0_sprite + gamedata->block_frame_count[i];
						break;
					case BLANK:
						gamedata->block_frame[i] = blockblank_sprite;
						gamedata->animate_piece[i] = 1;
						gamedata->block_frame_count[i] = 9;
						break;
				}
		}
	}

	return 0;
}

// For each of the four pieces display the current sprite frame.
// Display a cursor for which of the four pieces is currently selected.
int display_gamepieces(struct data *gamedata) {
	int borderspritesize = gamedata->spriteSurface[background0_sprite]->w;
	int x = borderspritesize * 10;
	int y = (borderspritesize * 5) - 16;

	SDL_Rect selection;
	SDL_Rect blocks[4];

	blocks[0].x = x;
	blocks[0].y = y;

	x = blocks[1].x = x + gamedata->spriteSurface[blockplus0_sprite]->w*1.5;
	blocks[1].y = y;

	x = blocks[2].x = x + gamedata->spriteSurface[blockplus0_sprite]->w*1.5;
	blocks[2].y = y;

	blocks[3].x = x + gamedata->spriteSurface[blockplus0_sprite]->w*1.5;
	blocks[3].y = y;

	SDL_BlitSurface(gamedata->spriteSurface[gamedata->block_frame[0]], NULL, gamedata->screenSurface, &blocks[0]);
	SDL_BlitSurface(gamedata->spriteSurface[gamedata->block_frame[1]], NULL, gamedata->screenSurface, &blocks[1]);
	SDL_BlitSurface(gamedata->spriteSurface[gamedata->block_frame[2]], NULL, gamedata->screenSurface, &blocks[2]);
	SDL_BlitSurface(gamedata->spriteSurface[gamedata->block_frame[3]], NULL, gamedata->screenSurface, &blocks[3]);

	for (int i = 0; i < 4; i++) {
		if (gamedata->selection == i) {
			selection.x = blocks[i].x;
			selection.y = blocks[i].y;
			SDL_BlitSurface(gamedata->spriteSurface[blockselect_sprite], NULL, gamedata->screenSurface, &selection);
			break;
		}
	}

	return 0;
}

// Clear the game section of the gameplayscreen.
// Display the gameover message.
int display_gameover(struct data *gamedata) {
	int borderspritesize = gamedata->spriteSurface[background0_sprite]->w;
	SDL_Rect game;
	SDL_Rect labels[1];

	game.x = borderspritesize * 8;
	game.y = borderspritesize * 4;
	game.w = (borderspritesize * 8);
	game.h = (borderspritesize * 4) - 32;

	SDL_FillRect(gamedata->screenSurface, &game, 0x000000);

	labels[0].x = (game.x + (game.w/2)) - (gamedata->spriteSurface[gameover_sprite]->w/2);
	labels[0].y = (game.y + (game.h/2)) - (gamedata->spriteSurface[gameover_sprite]->h/2);

	SDL_BlitSurface(gamedata->spriteSurface[gameover_sprite], NULL, gamedata->screenSurface, &labels[0]);

	return 0;
}

// Display a border around the game section of the gameplay screen.
int display_gameborder(struct data *gamedata) {
	int i = 0;
	SDL_Rect border[89];
	int topleft = 0;
	int topright = 1;
	int bottomleft = 2;
	int bottomright = 3;
	int borderspritewidth = gamedata->spriteSurface[bordertopleft_sprite]->w;
	int borderspriteheight = gamedata->spriteSurface[bordertopleft_sprite]->h;
	int borderspritesize = gamedata->spriteSurface[background0_sprite]->w;
	int x = borderspritesize * 8;
	int y = borderspritesize * 4;

	border[topleft].x = x;
	border[topleft].y = y;
	border[topright].x = (borderspritesize * 16) - 16;
	border[topright].y = y;
	border[bottomleft].x = x;
	border[bottomleft].y = (borderspritesize * 7) + 16;
	border[bottomright].x = border[topright].x;
	border[bottomright].y = border[bottomleft].y;

	SDL_BlitSurface(gamedata->spriteSurface[bordertopleft_sprite], NULL, gamedata->screenSurface, &border[topleft]);
	SDL_BlitSurface(gamedata->spriteSurface[bordertopright_sprite], NULL, gamedata->screenSurface, &border[topright]);
	SDL_BlitSurface(gamedata->spriteSurface[borderdownleft_sprite], NULL, gamedata->screenSurface, &border[bottomleft]);
	SDL_BlitSurface(gamedata->spriteSurface[borderdownright_sprite], NULL, gamedata->screenSurface, &border[bottomright]);
	
	i = 4;

	// TOP Border Line
	border[i].x = border[topleft].x + borderspritewidth;
	y = border[i].y = border[topright].y;

	while (border[i].x < border[topright].x) {
		SDL_BlitSurface(gamedata->spriteSurface[borderupdown_sprite], NULL, gamedata->screenSurface, &border[i]);
		i = i + 1;
		border[i].x = border[i-1].x + borderspritewidth;
		border[i].y = y;
	}

	// BOTTOM Border Line
	border[i].x = border[bottomleft].x + borderspritewidth;
	y = border[i].y = border[bottomleft].y;

	while (border[i].x < border[bottomright].x) {
		SDL_BlitSurface(gamedata->spriteSurface[borderupdown_sprite], NULL, gamedata->screenSurface, &border[i]);
		i = i + 1;
		border[i].x = border[i-1].x + borderspritewidth;
		border[i].y = y;
	}

	//Left Border Line
	x = border[i].x = border[topleft].x;
	border[i].y = border[topleft].y + borderspriteheight;

	while (border[i].y < border[bottomleft].y) {
		SDL_BlitSurface(gamedata->spriteSurface[borderleftright_sprite], NULL, gamedata->screenSurface, &border[i]);
		i = i + 1;
		border[i].x = x;
		border[i].y = border[i-1].y + borderspriteheight;
	}

	//Right Border Line
	x = border[i].x = border[topright].x;
	border[i].y = border[topright].y + borderspriteheight;

	while (border[i].y < border[bottomleft].y) {
		SDL_BlitSurface(gamedata->spriteSurface[borderleftright_sprite], NULL, gamedata->screenSurface, &border[i]);
		i = i + 1;
		border[i].x = x;
		border[i].y = border[i-1].y + borderspriteheight;
	}

	return 0;
}

// Display a border around the status section of the gameplay screen.
int display_statsborder(struct data *gamedata) {
	int i = 0;
	SDL_Rect border[117];
	int topleft = 0;
	int topright = 1;
	int bottomleft = 2;
	int bottomright = 3;
	int borderspritewidth = gamedata->spriteSurface[bordertopleft_sprite]->w;
	int borderspriteheight = gamedata->spriteSurface[bordertopleft_sprite]->h;
	int x = 64 - (borderspritewidth*2);
	int y = x;

	border[topleft].x = x;
	border[topleft].y = y;
	border[topright].x = gamedata->spriteGameText[5][8].x+borderspritewidth*3;
	border[topright].y = y;
	border[bottomleft].x = x;
	border[bottomleft].y = gamedata->spriteGameText[5][8].y + borderspritewidth*3;
	border[bottomright].x = border[topright].x;
	border[bottomright].y = border[bottomleft].y;

	SDL_BlitSurface(gamedata->spriteSurface[bordertopleft_sprite], NULL, gamedata->screenSurface, &border[topleft]);
	SDL_BlitSurface(gamedata->spriteSurface[bordertopright_sprite], NULL, gamedata->screenSurface, &border[topright]);
	SDL_BlitSurface(gamedata->spriteSurface[borderdownleft_sprite], NULL, gamedata->screenSurface, &border[bottomleft]);
	SDL_BlitSurface(gamedata->spriteSurface[borderdownright_sprite], NULL, gamedata->screenSurface, &border[bottomright]);

	i = 4;

	// TOP Border Line
	border[i].x = border[topleft].x + borderspritewidth;
	y = border[i].y = border[topright].y;

	while (border[i].x < border[topright].x) {
		SDL_BlitSurface(gamedata->spriteSurface[borderupdown_sprite], NULL, gamedata->screenSurface, &border[i]);
		i = i + 1;
		border[i].x = border[i-1].x + borderspritewidth;
		border[i].y = y;
	}

	// BOTTOM Border Line
	border[i].x = border[bottomleft].x + borderspritewidth;
	y = border[i].y = border[bottomleft].y;

	while (border[i].x < border[bottomright].x) {
		SDL_BlitSurface(gamedata->spriteSurface[borderupdown_sprite], NULL, gamedata->screenSurface, &border[i]);
		i = i + 1;
		border[i].x = border[i-1].x + borderspritewidth;
		border[i].y = y;
	}

	//Left Border Line
	x = border[i].x = border[topleft].x;
	border[i].y = border[topleft].y + borderspriteheight;

	while (border[i].y < border[bottomleft].y) {
		SDL_BlitSurface(gamedata->spriteSurface[borderleftright_sprite], NULL, gamedata->screenSurface, &border[i]);
		i = i + 1;
		border[i].x = x;
		border[i].y = border[i-1].y + borderspriteheight;
	}

	//Right Border Line
	x = border[i].x = border[topright].x;
	border[i].y = border[topright].y + borderspriteheight;

	while (border[i].y < border[bottomright].y) {
		SDL_BlitSurface(gamedata->spriteSurface[borderleftright_sprite], NULL, gamedata->screenSurface, &border[i]);
		i = i + 1;
		border[i].x = x;
		border[i].y = border[i-1].y + borderspriteheight;
	}

	return 0;
}

// Clear the gameplay section of the gameplay screen.
// If the currently selected piece if not blank then display the parts of the equation.
int display_gameequation(struct data *gamedata) {
	int borderspritesize = gamedata->spriteSurface[background0_sprite]->w;
	int x = (borderspritesize * 10) - 32;
	int y = borderspritesize * 6;
	int equation[4];
	SDL_Rect game;
	SDL_Rect equationpart[6];

	equationpart[0].x = x;
	equationpart[0].y = y;
	x = equationpart[1].x = x + gamedata->spriteSurface[blockplus0_sprite]->w;
	equationpart[1].y = y;
	x = equationpart[2].x = x + gamedata->spriteSurface[blockplus0_sprite]->w;
	equationpart[2].y = y;
	x = equationpart[3].x = x + gamedata->spriteSurface[blockplus0_sprite]->w;
	equationpart[3].y = y;
	x = equationpart[4].x = x + gamedata->spriteSurface[blockplus0_sprite]->w;
	equationpart[4].y = y;
	equationpart[5].x = x + gamedata->spriteSurface[blockplus0_sprite]->w;
	equationpart[5].y = y;

	game.x = borderspritesize * 8;
	game.y = borderspritesize * 4;
	game.w = (borderspritesize * 8);
	game.h = (borderspritesize * 4) - 32;
	
	SDL_FillRect(gamedata->screenSurface, &game, 0x000000);

	if (gamedata->grid[gamedata->selection][18] != BLANK && gamedata->block_frame_count[gamedata->selection] <= 0) {
		equation[0] = process_equationvalue(gamedata->bag_equations[gamedata->selection][0]);
		equation[1] = process_equationvalue(gamedata->bag_equations[gamedata->selection][2]);
		equation[2] = process_equationvalue(gamedata->guess[1]);
		equation[3] = process_equationvalue(gamedata->guess[0]);

		SDL_BlitSurface(gamedata->spriteSurface[equation[0]], NULL, gamedata->screenSurface, &equationpart[0]);
		SDL_BlitSurface(gamedata->spriteSurface[gamedata->bag_equations[gamedata->selection][1]], NULL, gamedata->screenSurface, &equationpart[1]);
		SDL_BlitSurface(gamedata->spriteSurface[equation[1]], NULL, gamedata->screenSurface, &equationpart[2]);
		SDL_BlitSurface(gamedata->spriteSurface[eqequal_sprite], NULL, gamedata->screenSurface, &equationpart[3]);
		SDL_BlitSurface(gamedata->spriteSurface[equation[2]], NULL, gamedata->screenSurface, &equationpart[4]);
		SDL_BlitSurface(gamedata->spriteSurface[equation[3]], NULL, gamedata->screenSurface, &equationpart[5]);
	}

	return 0;
}

// Clear the stats area of the gameplay screen.
// Convert each of the current stats from an array to a single int.
// Display the stats labels.
// Display the stats values.
int display_gamestats(struct data *gamedata) {
	int x = 64;
	int y = 64;
	char *score= malloc(10);
	char *speed = malloc(3);
	char *hit = malloc(10);
	char *miss = malloc(10);
	char *total = malloc(10);
	char *time = malloc(9);
	SDL_Rect stats;
	SDL_Rect labels[6];

	stats.x = x - 16;
	stats.y = y-16;
	stats.w = gamedata->spriteSurface[labeltotal_sprite]->h*10;
	stats.h = gamedata->spriteSurface[labeltotal_sprite]->h*18;

	SDL_FillRect(gamedata->screenSurface, &stats, 0x000000);

	snprintf(score, 10, "%09d", gamedata->score);
	assign_gamevalue(gamedata, GAME_SCORE, score);

	snprintf(speed, 3, "%02d", gamedata->speed_lvl);
	assign_gamevalue(gamedata, GAME_SPEED, speed);

	snprintf(time, 9, "%02d:%02d:%02d", gamedata->time[HOUR], gamedata->time[MINUTE], gamedata->time[SECOND]);
	assign_gamevalue(gamedata, GAME_TIME, time);

	snprintf(hit, 10, "%09d", gamedata->hit);
	assign_gamevalue(gamedata, GAME_HIT, hit);

	snprintf(miss, 10, "%09d", gamedata->miss);
	assign_gamevalue(gamedata, GAME_MISS, miss);

	snprintf(total, 10, "%09d", gamedata->total);
	assign_gamevalue(gamedata, GAME_TOTAL, total);

	free(score);
	free(speed);
	free(hit);
	free(miss);
	free(total);
	free(time);

	x = labels[0].x = x;
	y = labels[0].y = y;
	x = labels[1].x = x;
	y = labels[1].y = y + gamedata->spriteSurface[labelscore_sprite]->h*3;
	x = labels[2].x = x;
	y = labels[2].y = y + gamedata->spriteSurface[labelscore_sprite]->h*3;
	x = labels[3].x = x;
	y = labels[3].y = y + gamedata->spriteSurface[labelscore_sprite]->h*3;
	x = labels[4].x = x;
	y = labels[4].y = y + gamedata->spriteSurface[labelscore_sprite]->h*3;
	x = labels[5].x = x;
	y = labels[5].y = y + gamedata->spriteSurface[labelscore_sprite]->h*3;

	SDL_BlitSurface(gamedata->spriteSurface[labelscore_sprite], NULL, gamedata->screenSurface, &labels[0]);
	SDL_BlitSurface(gamedata->spriteSurface[labelspeed_sprite], NULL, gamedata->screenSurface, &labels[1]);
	SDL_BlitSurface(gamedata->spriteSurface[labeltime_sprite], NULL, gamedata->screenSurface, &labels[2]);
	SDL_BlitSurface(gamedata->spriteSurface[labelhit_sprite], NULL, gamedata->screenSurface, &labels[3]);
	SDL_BlitSurface(gamedata->spriteSurface[labelmiss_sprite], NULL, gamedata->screenSurface, &labels[4]);
	SDL_BlitSurface(gamedata->spriteSurface[labeltotal_sprite], NULL, gamedata->screenSurface, &labels[5]);

	y = 64;
	y = y + gamedata->spriteSurface[labelscore_sprite]->h;

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 9; j++) {
			gamedata->spriteGameText[i][j].x = x;
			gamedata->spriteGameText[i][j].y = y;
			x = x + 32;
		}
		x = labels[0].x;
		y = y + gamedata->spriteSurface[labelscore_sprite]->h*3;
	}

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 9; j++) {
			SDL_BlitSurface(gamedata->spriteSurface[(gamedata->spriteGameTextNum[i][j])], NULL, gamedata->screenSurface, &gamedata->spriteGameText[i][j]);
		}
	}

	return 0;
}

// Process the current guess for an equation.
// Return is a match is true/false.
int check_answer(struct data *gamedata) {
	int match = 0;
	int guess = (gamedata->guess[1] *10) + gamedata->guess[0];

	if (gamedata->bag_equations[gamedata->selection][3] == guess) {
		match = 1;
	}

	return match;
}

// Set the timers for generating a piece and starting the game.
int init_timers(struct data *gamedata) {
	gamedata->time[PIECE_START_TIME] = SDL_GetTicks64();
	gamedata->time[START_TIME] = SDL_GetTicks64();

	return 0;
}

// Search for an event type.
// If a quit event is detected then set the flag to exit the entire program. 
// If a key is pressed then check what screen we are on.
// Compare the key action to the key that was pressed.
// Set the various flags to move to a new screen, quit the game, move through the menus, set values etc.
int process_input(struct data *gamedata) {
	int process_key = 1;

	while(SDL_PollEvent(&gamedata->event) !=0) {
		if (gamedata->event.type == SDL_QUIT) {
			gamedata->exit = 1;
		} else if (gamedata->event.type == SDL_KEYDOWN) {
			if (gamedata->screen == TITLE_SCREEN) {
				if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_SUBMIT]) {
					if (gamedata->menu == START) {
						gamedata->screen = GAME_SCREEN;
						init_timers(gamedata);
					} else if (gamedata->menu == OPTION) {
						gamedata->screen = OPTION_SCREEN;
					} else if (gamedata->menu == CONFIG) {
						gamedata->screen = CONFIG_SCREEN;
					}
				} else if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_QUIT]) {
					gamedata->exit = 1;
				} else if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_UP]) {
					if (gamedata->menu > 0) {
						--gamedata->menu;
						process_sfx(gamedata, MENU_SELECT);
					}
				} else if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_DOWN]) {
					if (gamedata->menu < 2) {
						++gamedata->menu;
						process_sfx(gamedata, MENU_SELECT);
					}
				}
			} else if (gamedata->screen == OPTION_SCREEN) {
				if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_QUIT]) {
					gamedata->screen = TITLE_SCREEN;
					gamedata->limits[SCORE_LIMIT] = process_arraytoint(gamedata->scorearray, 10);
					gamedata->limits[HIT_LIMIT] = process_arraytoint(gamedata->hitarray, 10);
					gamedata->limits[MISS_LIMIT] = process_arraytoint(gamedata->missarray, 10);
					gamedata->limits[TOTAL_LIMIT] = process_arraytoint(gamedata->totalarray, 10);
					gamedata->limits[HOUR_LIMIT] = gamedata->timearray[GAME_HOUR1] * 10;
					gamedata->limits[HOUR_LIMIT] += gamedata->timearray[GAME_HOUR2] * 1;
					gamedata->limits[MINUTE_LIMIT] = gamedata->timearray[GAME_MINUTE1] * 10;
					gamedata->limits[MINUTE_LIMIT] += gamedata->timearray[GAME_MINUTE2] * 1;
					gamedata->limits[SECOND_LIMIT] = gamedata->timearray[GAME_SECOND1] * 10;
					gamedata->limits[SECOND_LIMIT] += gamedata->timearray[GAME_SECOND2] * 1;
					gamedata->optionselection = 0;
					gamedata->timeoption = 0;
					gamedata->scoreoption = 0;
					gamedata->hitoption = 0;
					gamedata->missoption = 0;
					gamedata->totaloption = 0;
				} else if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_UP]) {
					if (gamedata->optionselection == 3 && gamedata->timeoption > 0) {
						if (gamedata->timeoption == GAME_HOUR1 || gamedata->timeoption == GAME_HOUR2 || gamedata->timeoption == GAME_MINUTE2 || gamedata->timeoption == GAME_SECOND2) {
							if (gamedata->timearray[gamedata->timeoption] < 9) {
								gamedata->timearray[gamedata->timeoption]++;
							}
						} else {
							if (gamedata->timearray[gamedata->timeoption] < 5) {
								gamedata->timearray[gamedata->timeoption]++;
							}
						}
					} else if (gamedata->optionselection == 4 && gamedata->scoreoption > 0) {
						if (gamedata->scorearray[gamedata->scoreoption] < 9) {
							gamedata->scorearray[gamedata->scoreoption]++;
						}
					} else if (gamedata->optionselection == 5 && gamedata->hitoption > 0) {
						if (gamedata->hitarray[gamedata->hitoption] < 9) {
							gamedata->hitarray[gamedata->hitoption]++;
						}
					} else if (gamedata->optionselection == 6 && gamedata->missoption > 0) {
						if (gamedata->missarray[gamedata->missoption] < 9) {
							gamedata->missarray[gamedata->missoption]++;
						}
					} else if (gamedata->optionselection == 7 && gamedata->totaloption > 0) {
						if (gamedata->totalarray[gamedata->totaloption] < 9) {
							gamedata->totalarray[gamedata->totaloption]++;
						}
					} else if (gamedata->optionselection > 0) {
						--gamedata->optionselection;
						process_sfx(gamedata, MENU_SELECT);
					}
				} else if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_DOWN]) {
					if (gamedata->optionselection == 3 && gamedata->timeoption > 0) {
						if (gamedata->timearray[gamedata->timeoption] > 0) {
							gamedata->timearray[gamedata->timeoption]--;
						}
					} else if (gamedata->optionselection == 4 && gamedata->scoreoption > 0) {
						if (gamedata->scorearray[gamedata->scoreoption] > 0) {
							gamedata->scorearray[gamedata->scoreoption]--;
						}
					} else if (gamedata->optionselection == 5 && gamedata->hitoption > 0) {
						if (gamedata->hitarray[gamedata->hitoption] > 0) {
							gamedata->hitarray[gamedata->hitoption]--;
						}
					} else if (gamedata->optionselection == 6 && gamedata->missoption > 0) {
						if (gamedata->missarray[gamedata->missoption] > 0) {
							gamedata->missarray[gamedata->missoption]--;
						}
					} else if (gamedata->optionselection == 7 && gamedata->totaloption > 0) {
						if (gamedata->totalarray[gamedata->totaloption] > 0) {
							gamedata->totalarray[gamedata->totaloption]--;
						}
					} else if (gamedata->optionselection < 7) {
						++gamedata->optionselection;
						process_sfx(gamedata, MENU_SELECT);
					}
				} else if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_LEFT]) {
						if (gamedata->optionselection == 0) {
							if (gamedata->option[gamedata->optionselection] > 1) {
								gamedata->option[gamedata->optionselection] -= 1;
							}
						} else if (gamedata->optionselection == 3) {
							if (gamedata->timeoption > 0) {
								gamedata->timeoption -= 1;
							}
						} else if (gamedata->optionselection == 4 ) {
							if (gamedata->scoreoption > 0) {
								gamedata->scoreoption -= 1;
							}
						}  else if (gamedata->optionselection == 5 ) {
							if (gamedata->hitoption > 0) {
								gamedata->hitoption -= 1;
							}
						} else if (gamedata->optionselection == 6 ) {
							if (gamedata->missoption > 0) {
								gamedata->missoption -= 1;
							}
						} else if (gamedata->optionselection == 7 ) {
							if (gamedata->totaloption > 0) {
								gamedata->totaloption -= 1;
							}
						} else {
							if (gamedata->option[gamedata->optionselection] > 0) {
								gamedata->option[gamedata->optionselection] -= 1;
							}

							if (gamedata->optionselection == 1) {
								gamedata->speed_lvl = gamedata->option[gamedata->optionselection];
								gamedata->goal = (76 * gamedata->option[gamedata->optionselection]);
								gamedata->speed = 13 - gamedata->speed_lvl;
							}
						}
				} else if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_RIGHT]) {
						if (gamedata->optionselection == 0) {
							if (gamedata->option[gamedata->optionselection] < 9) {
								gamedata->option[gamedata->optionselection] += 1;
							}
						} else if (gamedata->optionselection == 1) {
							if (gamedata->option[gamedata->optionselection] < 9) {
								gamedata->option[gamedata->optionselection] += 1;
								gamedata->goal = (76 * gamedata->option[gamedata->optionselection]);
								gamedata->speed_lvl = gamedata->option[gamedata->optionselection];
								gamedata->speed = 13 - gamedata->speed_lvl;
							}
						} else if (gamedata->optionselection == 2) {
							if (gamedata->option[gamedata->optionselection] < 4) {
								gamedata->option[gamedata->optionselection] += 1;
							}
						} else if (gamedata->optionselection == 3) {
							if (gamedata->timeoption < 6) {
								gamedata->timeoption += 1;
							}
						} else if (gamedata->optionselection == 4) {
							if (gamedata->scoreoption < 9) {
								gamedata->scoreoption += 1;
							}
						} else if (gamedata->optionselection == 5) {
							if (gamedata->hitoption < 9) {
								gamedata->hitoption += 1;
							}
						} else if (gamedata->optionselection == 6) {
							if (gamedata->missoption < 9) {
								gamedata->missoption += 1;
							}
						} else if (gamedata->optionselection == 7) {
							if (gamedata->totaloption < 9) {
								gamedata->totaloption += 1;
							}
						}
				}
			} else if (gamedata->screen == CONFIG_SCREEN) {
				int key = gamedata->event.key.keysym.sym;
				if (gamedata->controls[ACT_QUIT] == key) {
					if (gamedata->current_control != 16) {
						gamedata->screen = TITLE_SCREEN;
					}
				} else if (process_key == 1 && gamedata->controls[ACT_QUIT] != key && strncmp(SDL_GetKeyName(key),"",1) != 0) {
					for (int i = 0; i < 17;i++) {
						if (gamedata->controls[i] == key) {
							process_key = 0;
							break;
						}
					}
					gamedata->controls[gamedata->current_control] = key;
					convert_keyname(gamedata, key);
				}
			} else if (gamedata->screen == GAME_SCREEN) {
				if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_QUIT]) {
					gamedata->screen = TITLE_SCREEN;
					reset_variables(gamedata);
				}
		
				if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_LEFT]) {
					if (gamedata->selection > 0) {
						--gamedata->selection;
						process_sfx(gamedata, MOVE_CURSOR);
					}
				} else if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_RIGHT]) {
					if (gamedata->selection < 3) {
						++gamedata->selection;
						process_sfx(gamedata, MOVE_CURSOR);
					}
				}

				if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_CLEAR]) {
					gamedata->guess[0] = 0;
					gamedata->guess[1] = 0;
					gamedata->guess_position = 1;
					process_sfx(gamedata, ANSWER_TYPING);
				}  else if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_SUBMIT]) {
					if (gamedata->grid[gamedata->selection][18] != BLANK) {
						if (check_answer(gamedata) == 1) {
							gamedata->grid[gamedata->selection][18] = BLANK;
							gamedata->animate_piece[gamedata->selection] = 1;
							gamedata->block_frame_count[gamedata->selection] = 9;
							gamedata->guess_position = 1;
							gamedata->guess[0] = 0;
							gamedata->guess[1] = 0;
							gamedata->hit++;
							gamedata->piecelimit[gamedata->selection]++;
							gamedata->time[COMBO_CURRENT_TIME] = SDL_GetTicks64();
							gamedata->time[COMBO_TOTAL_TIME] = (gamedata->time[COMBO_CURRENT_TIME] - gamedata->time[COMBO_START_TIME])/1000;
							if (gamedata->time[COMBO_TOTAL_TIME] >= 0 && gamedata->time[COMBO_TOTAL_TIME] <= 3) {
								if (gamedata->combo_accumulator < 5) {
									gamedata->combo_accumulator +=1;
								} else {
									gamedata->combo_accumulator = 5;
								}
								gamedata->time[COMBO_CURRENT_TIME] = SDL_GetTicks64();
								gamedata->time[COMBO_START_TIME] = SDL_GetTicks64();
								gamedata->animate_combo = 1;
								gamedata->time[ANIMATECOMBO_START_TIME] = SDL_GetTicks64();
								gamedata->time[ANIMATECOMBO_CURRENT_TIME] = SDL_GetTicks64();
							} else {
								if (gamedata->time[COMBO_TOTAL_TIME] == 0 || gamedata->time[COMBO_TOTAL_TIME] > 3) {							
									gamedata->time[COMBO_CURRENT_TIME] = SDL_GetTicks64();
									gamedata->time[COMBO_START_TIME] = SDL_GetTicks64();
									gamedata->combo_accumulator = 1;
								}
								gamedata->time[COMBO_CURRENT_TIME] = SDL_GetTicks64();
							}
							gamedata->score += gamedata->combo_accumulator * 1;
							process_grid(gamedata);
							process_sfx(gamedata, ANSWER_CORRECT);
						} else {
							gamedata->time[COMBO_CURRENT_TIME] = SDL_GetTicks64();
							gamedata->time[COMBO_START_TIME] = SDL_GetTicks64();
							gamedata->combo_accumulator = 0;
							gamedata->miss++;
							process_sfx(gamedata, ANSWER_WRONG);
						}
						gamedata->total++;
					}
				}

				for (int i = ACT_0, j = 0; i < ACT_9+1; i++,j++) {
					if (gamedata->event.key.keysym.sym == gamedata->controls[i]) {
						if (gamedata->guess_position == 1) {
							gamedata->guess[0] = 0;
							gamedata->guess[1] = 0;
						} else {
							gamedata->guess[1] = gamedata->guess[0];
						}
						gamedata->guess_position = 1 - gamedata->guess_position;
						gamedata->guess[0] = j;
						process_sfx(gamedata, ANSWER_TYPING);
						break;
					}
				}
			} else if (gamedata->screen == GAMEOVER_SCREEN) {
				if (gamedata->event.key.keysym.sym == gamedata->controls[ACT_QUIT]) {
					gamedata->screen = TITLE_SCREEN;
					reset_variables(gamedata);
				}
			}

		}
	}

	return 0;
}

// Process the current time in hours, minutes and seconds.
// If 60 seconds is reached, add a minute to the total time.
// If 60 minutes is reached, add a hour to the total time.
// If the time has reached 99 hours, 59 minutes and 59 seconds, then mark the max time limited as reached.
int game_timer(struct data *gamedata) {
	if (gamedata->timelimitreached != 1) {
		gamedata->time[CURRENT_TIME] = SDL_GetTicks64();
		if (gamedata->time[TOTAL_TIME] != (gamedata->time[CURRENT_TIME] - gamedata->time[START_TIME])) {
			gamedata->time[TOTAL_TIME] = (gamedata->time[CURRENT_TIME] - gamedata->time[START_TIME])/1000;
			gamedata->time[SECOND] = gamedata->time[TOTAL_TIME];

			if (gamedata->time[SECOND] == 60) {
				gamedata->time[START_TIME] = SDL_GetTicks64();
				gamedata->time[MINUTE]++;
				gamedata->time[SECOND] = 0;
				gamedata->time[TOTAL_TIME] = 0;
			}

			if (gamedata->time[MINUTE] == 60) {
				gamedata->time[HOUR]++;
				gamedata->time[MINUTE] = 0;
				gamedata->time[SECOND] = 0;
				gamedata->time[TOTAL_TIME] = 0;
			}

			if(gamedata->time[HOUR] == 99 && gamedata->time[MINUTE] == 59 && gamedata->time[SECOND] == 59) {
				gamedata->timelimitreached = 1;
			}
		}
	}

	return 0;
}

// Setup the gamedata structure.
// Set up the pseudo-random number generator
// Initialize the SDL system and variables for the program.
// Delay the start of the game by a few milliseconds to give the program to properly load.
// Enter the game loop:
// For each frame, clear the screen.
// Check for the title, option, configuration, gameplay and gameover screens.
// Call the relevant functions for the given screen selected.
// For any screen, process any input from the keyboard and update the entire screen.
// If we get an exit signal during the loop then uninitialize all files/memory and close the program.
int main() {
	unsigned int randomvalue = 0;
	FILE *fp;
	struct data gamedata;

	const int SCREEN_FPS = 60;
	const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
	int starting_tick = SDL_GetTicks64();

	if ((fp = fopen("/dev/urandom", "rb")) != NULL) {
		fread(&randomvalue, sizeof(int),1,fp);
		fclose(fp);
	}

	if (randomvalue > 0) {
		srand(time(NULL));
	} else {
		srand(randomvalue);
	}

	init_system(&gamedata);
	init_variables(&gamedata);
	
	SDL_Delay(150);

	while (!gamedata.exit) {
		starting_tick = SDL_GetTicks64();
		
		SDL_FillRect(gamedata.screenSurface, NULL, 0x000000);

 		if (gamedata.screen == TITLE_SCREEN) {
			display_titlescreen(&gamedata);
		} else if (gamedata.screen == OPTION_SCREEN) {
			display_optionscreen(&gamedata);
		} else if (gamedata.screen == CONFIG_SCREEN) {
			display_controlscreen(&gamedata);
		} else if (gamedata.screen == GAME_SCREEN || gamedata.screen == GAMEOVER_SCREEN) {
			process_limits(&gamedata);
			display_background(&gamedata);
			display_gamestats(&gamedata);
			if (gamedata.screen == GAME_SCREEN) {
				game_timer(&gamedata);
				gamedata.time[PIECE_CURRENT_TIME] = SDL_GetTicks64();
				if (gamedata.grid[0][18] == BLANK && gamedata.grid[1][18] == BLANK && gamedata.grid[2][18] == BLANK && gamedata.grid[3][18] == BLANK) {
					process_line(&gamedata);
					process_grid(&gamedata);
				}
				if (gamedata.speed == (gamedata.time[PIECE_CURRENT_TIME] - gamedata.time[PIECE_START_TIME]) / 1000) {
					process_line(&gamedata);
					process_grid(&gamedata);
					gamedata.time[PIECE_START_TIME] = SDL_GetTicks64();
					gamedata.time[PIECE_CURRENT_TIME] = SDL_GetTicks64();
				}
				display_gameequation(&gamedata);
				display_gamepieces(&gamedata);
				animate_gamepieces(&gamedata);
				display_gamepiecemeter(&gamedata);
				if (gamedata.combo_accumulator > 1) {
					gamedata.time[COMBO_CURRENT_TIME] = SDL_GetTicks64();
					display_combo(&gamedata);
				}
			} else if (gamedata.screen == GAMEOVER_SCREEN) {
				display_gameover(&gamedata);
			}
			display_statsborder(&gamedata);
			display_gameborder(&gamedata);
		}
		process_input(&gamedata);
		SDL_UpdateWindowSurface(gamedata.window);

		if (SCREEN_TICKS_PER_FRAME > SDL_GetTicks64() - starting_tick) {
			SDL_Delay(SCREEN_TICKS_PER_FRAME - (SDL_GetTicks64() - starting_tick));
		}
	}

	uninit_system(&gamedata);

	return 0;
}

