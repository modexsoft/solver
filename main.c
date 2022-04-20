#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vga.h>
#include <vgagl.h>
#include <time.h>
#include <vgakeyboard.h>
#include "rgb.h"
#include "font.h"
#include "sprites_l.h"
#include "sprites.h"
#include "background.h"
#include "wav.h"
#include "wavplay.h"

GraphicsContext *physicalscreen;
GraphicsContext *virtualscreen;
unsigned char *vbuf;
int writepage;

#define VWIDTH 640
#define VHEIGHT 480
#define WINWIDTH 320
#define WINHEIGHT 240
#define SCREEN G320x240x256
#define num_pieces_horizontal 4
#define num_pieces_vertical 19
#define bag_maxpieces num_pieces_horizontal * num_pieces_vertical
#define LEVEL_GOAL 76

char font_dst[64];
char spritesl_dst[1113];
char sprites_dst[256];
char background_dst[256];

char *asset_src[] = {
myfont,sprites_l,sprites,background
};

char *asset_dst[] = {font_dst,spritesl_dst,sprites_dst,background_dst};

enum sprite_font{
s8_0,s8_1,s8_2,s8_3,s8_4,s8_5,s8_6,s8_7,s8_8,s8_9,s8_a,s8_b,s8_c,s8_d,s8_e,s8_f,
s8_g,s8_h,s8_i,s8_j,s8_k,s8_l,s8_m,s8_n,s8_o,s8_p,s8_q,s8_r,s8_s,s8_t,s8_u,
s8_v,s8_w,s8_x,s8_y,s8_z,s8_equal,s8_plus,s8_minus,s8_multiply,s8_divide,
s8_colon,s8_percent,s8_slash,s8_comma,s8_left_arrow,s8_right_arrow,s8_enter,
s8_cursor1,s8_cursor2,s8_blank,bottomborder,leftborder,loleftborder,
lorightborder,rightborder,topborder,upleftborder,uprightborder,blank,clear};

enum sprite_32{
s32_s=0,s32_o,s32_l,s32_v,s32_e,s32_r,s32_divide=6,s32_minus,s32_multiply,
s32_plus,s32_blank};

enum sprite_16{
s16_0,s16_1,s16_2,s16_3,s16_4,s16_5,s16_6,s16_7,s16_8,s16_9,
s16_divide,s16_minus,s16_multiply,s16_plus,s16_equal,s16_blank};

enum currentmode{START=0,OPTION=1,CONFIG=2};
enum currentscreen{TITLE_SCREEN,GAME_SCREEN,OPTION_SCREEN,CONFIG_SCREEN,GAMEOVER_SCREEN};

enum controls{UP,DOWN,LEFT,RIGHT,ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,ACTION1,ACTION2,ESCAPE};

struct prgdata {
/* Generic variables */
	int gameover;
	int screen;
	int menu;
	char *buffer;
	int exit;
/* time related variables */
	time_t start_time;
	time_t current_time;
	int total_time;
	int second;
	int minute;
	int hour;
/* Game specific variables */
	int score;
	int hit;
	int miss;
	int total;
	int speed_lvl;
	int grid[num_pieces_horizontal][num_pieces_vertical];
	int bag_equations[4][4];
	int baglimit;
	int piecelimit[4];
	int animate_piece[4];
	int playdrop;
	int guess[2];
	int guess_position;
	int speed;
	int selection;
	int option[3];
	int optionselection;
	int goal;
	int combo_accumulator;
	time_t piece_start_time;
	time_t piece_current_time;
	int combo_total_time;
	time_t combo_start_time;
	time_t combo_current_time;
	struct timespec animate_start_time;
	struct timespec animate_current_time;
	int combo_xposition;
	int controls[17];
	int current_control;
};

// Create a virtual screen. Set the screen mode to MODE X-style (width 320 
// height 240 colors 256). Initialize page flipping by displaying page 0 and 
// writing to page 1.
void initialize_screen(void) {
	vbuf = malloc(VWIDTH * VHEIGHT);
	gl_setcontextvirtual(VWIDTH, VHEIGHT, 1, 8, vbuf);
	vga_setmode(G320x240x256);
	vga_setdisplaystart(2);
	writepage = 1;
}

// Copy the window to the screen. Flip the pages/screens and then perform 
// triple buffering.
void update_screen(void) {
	int pageoffset[3] = {
	0,
	320 * 240 / 4,
	2 * 320 * 240 / 4
	};

	vga_copytoplanar256(vbuf + 0 * WIDTH + 0, WIDTH,
			 pageoffset[writepage], 80, WINWIDTH, WINHEIGHT);

	vga_setdisplaystart(pageoffset[writepage] * 4);

	vga_waitretrace();
	writepage = (writepage + 1) % 3;
}

// Take two numbers and return a random number within these limits.
int rand_lim(int min, int max) {
	return rand() % (max - min + 1) + min;
}

// Generate an equation based on the max number. Get a random number first. 
// Based on a random operator (+,-,*,/) generate get the operator number, 
// another random number and the answer for the equation. For subtraction 
// operations, make sure the second number generated is smaller than or the 
// same as the first. This will avoid generating a negative number. For 
// division operations, make sure the second number generated is one(1) or 
// greater. This will avoid generating a divide by zero.
int gen_equation(struct prgdata *programdata, int operator, int count) {
	int max_number = programdata->option[0]; 

	programdata->bag_equations[count][0] = rand_lim(0,max_number);

	switch (operator) {
		case s32_plus:
			programdata->bag_equations[count][1] = 13;
			programdata->bag_equations[count][2] = rand_lim(0,max_number);
			programdata->bag_equations[count][3] = programdata->bag_equations[count][0] + programdata->bag_equations[count][2];
			break;
		case s32_minus:
			// avoid negative number
			programdata->bag_equations[count][1] = 11;
			programdata->bag_equations[count][2] = rand_lim(0,programdata->bag_equations[count][0]);
			programdata->bag_equations[count][3] = programdata->bag_equations[count][0] - programdata->bag_equations[count][2];
			break;
		case s32_multiply:
			programdata->bag_equations[count][1] = 12;
			programdata->bag_equations[count][2] = rand_lim(0,max_number);
			programdata->bag_equations[count][3] = programdata->bag_equations[count][0] * programdata->bag_equations[count][2];
			break;
		case s32_divide:
			programdata->bag_equations[count][1] = 10;
			programdata->bag_equations[count][2] = rand_lim(1,max_number);
			programdata->bag_equations[count][3] = programdata->bag_equations[count][0] / programdata->bag_equations[count][2];
			break;
		default:
			break;
		}

	return 0;
}

// Check if any of the bottom pieces are blank and set the above piece to
// be animated when drawn. Set a delay timer so the animation is not 
// immediate. Start at the bottom of the grid. For each line in the grid, 
// move each tile down by one. Keep moving pieces down until the entire grid is
// processed. Get an equation for each of the four tiles on the bottom of 
// the grid.
int process_grid(struct prgdata *programdata, struct wavfile * channel) {
	int line = 18;

	if (programdata->grid[0][18] == s32_blank) {
		programdata->animate_piece[0] = 1;
	}
	if (programdata->grid[1][18] == s32_blank) {
		programdata->animate_piece[1] = 1;
	}
	if (programdata->grid[2][18] == s32_blank) {
		programdata->animate_piece[2] = 1;
	}
	if (programdata->grid[3][18] == s32_blank) {
		programdata->animate_piece[3] = 1;
	}

	if (programdata->grid[0][18] == s32_blank || programdata->grid[1][18] == s32_blank || programdata->grid[2][18] == s32_blank || programdata->grid[3][18] == s32_blank) {
		clock_gettime(CLOCK_MONOTONIC,&(programdata->animate_start_time));
		clock_gettime(CLOCK_MONOTONIC,&(programdata->animate_current_time));
	}

	while (line >= 0) {
		for (int j = 17; j >= 0; j--) {
			for (int i = 0; i < 4; i++) {
				if (programdata->grid[i][j+1] == s32_blank) {
					programdata->grid[i][j+1] = programdata->grid[i][j];
					programdata->grid[i][j] = s32_blank;

					if (j+1 == 18) {
						gen_equation(programdata,programdata->grid[i][j+1],i);
					}
				}
			}
		}
		line--;
	}

	return 0;
}

// Check if the top line's spaces already has pieces taking room. If so then 
// the grid is too full, resulting in a game over. Otherwise, generate 4 random 
// tiles. There should be an attempt to make sure the tiles don't repeat much. 
// This check should be performed against the current line as well as the 
// previous line. Place the tiles into the grid and decrease the limit. 
int process_line(struct prgdata *programdata) {
	int piece_count = 0;
	int tile = -1;
	int previous_piece[2] = {0};
	int bag[4];

	if (programdata->piecelimit[0] == 1 || programdata->piecelimit[1] == 1 || programdata->piecelimit[2] == 1 || programdata->piecelimit[3] == 1) {
		programdata->screen = GAMEOVER_SCREEN;
		programdata->piecelimit[0] = 0;
		programdata->piecelimit[1] = 0;
		programdata->piecelimit[2] = 0;
		programdata->piecelimit[3] = 0;

		return 0;
	}

	while (piece_count != 4) {
		tile = rand_lim(6,9);

		if (piece_count > 1) {
			previous_piece[0] = bag[piece_count - 2];
			previous_piece[1] = bag[piece_count - 1];
		}

		if ((previous_piece[0] != tile) && (previous_piece[1] != tile)) {
			bag[piece_count] = tile;
			++piece_count;
		}
	}

	programdata->grid[0][0] = bag[0];
	programdata->grid[1][0] = bag[1];
	programdata->grid[2][0] = bag[2];
	programdata->grid[3][0] = bag[3];
	programdata->baglimit += 4;

	programdata->piecelimit[0]--;
	programdata->piecelimit[1]--;
	programdata->piecelimit[2]--;
	programdata->piecelimit[3]--;

	return 0;
}

// For every 60 seconds add a minute to the total game time. Reset the seconds 
// to zero. For every 60 minutes add an hour to the total time. Reset the 
// minutes to zero.
int game_timer(struct prgdata *programdata) {
	if (programdata->hour != 99  || programdata->minute != 59 || programdata->second != 59) {
		programdata->current_time = time(NULL);
		if (programdata->total_time != (int)(programdata->current_time - programdata->start_time)) {
			programdata->total_time = (int)(programdata->current_time - programdata->start_time);
			programdata->second = programdata->total_time;
			if (programdata->second == 60 && programdata->hour != 99) {
				programdata->start_time = time(NULL);
				programdata->minute++;
				programdata->second = 0;
				programdata->total_time = 0;
			}

			if (programdata->minute == 60 && programdata->hour != 99) {
				programdata->hour++;
				programdata->minute = 0;
			}
		}
	}

	return 0;
}

// Process the current guess for an equation. Return is a match is true/false.
int check_answer(struct prgdata *programdata) {
	int match = 0;
	int guess = (programdata->guess[1] *10) + programdata->guess[0];

	if (programdata->bag_equations[programdata->selection][3] == guess) {
		match = 1;
	}

	return match;
}

// Set all variables to the initial defaults for first run.
int init_variables(struct prgdata *programdata) {
	programdata->exit = 0;
	programdata->screen = TITLE_SCREEN; 
	programdata->option[0] = 3;
	programdata->option[1] = 0;
	programdata->option[2] = 0;
	programdata->menu = START; 
	programdata->buffer = keyboard_getstate();
	programdata->optionselection = 0;	
	programdata->current_time = 0;
	programdata->total_time = 0;
	programdata->second = 0;
	programdata->minute = 0;
	programdata->hour = 0;
	programdata->score = 0;
	programdata->speed_lvl = 0;
	programdata->hit = 0; 
	programdata->miss = 0;
	programdata->total = 0;
	programdata->speed = 13; 
	programdata->goal = LEVEL_GOAL;
	programdata->selection = 0; 
	programdata->combo_total_time = 0;
	programdata->combo_start_time = 0;
	programdata->combo_current_time = 0;
	programdata->combo_accumulator = 1;
	programdata->guess[0] = 0;
	programdata->guess[1] = 0; 
	programdata->guess_position = 1;

	programdata->piecelimit[0] = 18;
	programdata->piecelimit[1] = 18;
	programdata->piecelimit[2] = 18;
	programdata->piecelimit[3] = 18;

	programdata->animate_piece[0] = 0;
	programdata->animate_piece[1] = 0;
	programdata->animate_piece[2] = 0;
	programdata->animate_piece[3] = 0;

	programdata->playdrop = 0;

	for (int i = 0; i<num_pieces_horizontal;i++) { 
		for (int j = 0; j<num_pieces_vertical;j++) {
			programdata->grid[i][j] = s32_blank;
		}
	}

	programdata->controls[UP] = SCANCODE_CURSORBLOCKUP;
	programdata->controls[DOWN] = SCANCODE_CURSORBLOCKDOWN;
	programdata->controls[LEFT] = SCANCODE_CURSORBLOCKLEFT;
	programdata->controls[RIGHT] = SCANCODE_CURSORBLOCKRIGHT;
	programdata->controls[ZERO] = SCANCODE_0;
	programdata->controls[ONE] = SCANCODE_1;
	programdata->controls[TWO] = SCANCODE_2;
	programdata->controls[THREE] = SCANCODE_3;
	programdata->controls[FOUR] = SCANCODE_4;
	programdata->controls[FIVE] = SCANCODE_5;
	programdata->controls[SIX] = SCANCODE_6;
	programdata->controls[SEVEN] = SCANCODE_7;
	programdata->controls[EIGHT] = SCANCODE_8;
	programdata->controls[NINE] = SCANCODE_9;
	programdata->controls[ACTION1] = SCANCODE_SPACE;
	programdata->controls[ACTION2] = SCANCODE_ENTER;
	programdata->controls[ESCAPE] = SCANCODE_ESCAPE;
	programdata->current_control = UP;

	return 0;
}

// Reset the variables needed to start a new game.
int reset_variables(struct prgdata *programdata) {
	programdata->current_time = 0;
	programdata->total_time = 0;
	programdata->second = 0;
	programdata->minute = 0;
	programdata->hour = 0;
	programdata->score = 0;
	programdata->hit = 0; 
	programdata->miss = 0;
	programdata->total = 0;
	programdata->optionselection = 0;
	programdata->speed_lvl = programdata->option[1];
	programdata->goal = LEVEL_GOAL;
	programdata->speed = 13 - programdata->speed_lvl;
	programdata->selection = 0; 
	programdata->combo_total_time = 0;
	programdata->combo_accumulator = 1;
	programdata->guess[0] = 0;
	programdata->guess[1] = 0; 
	programdata->guess_position = 1;
	
	programdata->piecelimit[0] = 18;
	programdata->piecelimit[1] = 18;
	programdata->piecelimit[2] = 18;
	programdata->piecelimit[3] = 18;

	programdata->animate_piece[0] = 0;
	programdata->animate_piece[1] = 0;
	programdata->animate_piece[2] = 0;
	programdata->animate_piece[3] = 0;

	programdata->playdrop = 0;

	for (int i = 0; i<num_pieces_horizontal;i++) { 
		for (int j = 0; j<num_pieces_vertical;j++) {
			programdata->grid[i][j] = s32_blank;
		}
	}

	return 0;
}

// Set all the timers to start
int init_timers(struct prgdata *programdata) {
	programdata->piece_start_time = time(NULL);
	programdata->start_time = time(NULL);

	return 0;
}

// Free up any memory and close any libraries or files currently open.
int uninit_all(struct prgdata *programdata,struct wavfile * channel) {
	wav_uninit();
	keyboard_close();
	vga_setmode(TEXT);
	free(vbuf);

	return 0;
}

// Check the current screen and process the user inputs and system sounds.
int process_input(struct prgdata *programdata, struct wavfile * channel){
	int process_key = 1;

	if (programdata->screen == TITLE_SCREEN) {
		if (programdata->buffer[SCANCODE_ENTER] || programdata->buffer[SCANCODE_KEYPADENTER]) {
			if (programdata->menu == START) {
				programdata->screen = GAME_SCREEN;
				init_timers(programdata);
			} else if (programdata->menu == OPTION) {
				programdata->screen = OPTION_SCREEN;
			} else if (programdata->menu == CONFIG) {
				programdata->screen = CONFIG_SCREEN;
			}
		} else if (programdata->buffer[SCANCODE_ESCAPE] || programdata->buffer[programdata->controls[ESCAPE]]) {
			programdata->exit = 1;
		} else if (programdata->buffer[SCANCODE_CURSORUP] || programdata->buffer[SCANCODE_CURSORBLOCKUP]) {
			if (programdata->menu > 0) {
				--programdata->menu;
				wav_setstate(selection,PLAY);
			}
		} else if (programdata->buffer[SCANCODE_CURSORDOWN] || programdata->buffer[SCANCODE_CURSORBLOCKDOWN]) {
			if (programdata->menu < 3) {
				++programdata->menu;
				wav_setstate(selection,PLAY);
			}
		}
	} else if(programdata->screen == GAME_SCREEN) {
		if (programdata->buffer[SCANCODE_ESCAPE] || programdata->buffer[programdata->controls[ESCAPE]]) {
			programdata->screen = TITLE_SCREEN;
			reset_variables(programdata);
		}
		
		if (programdata->buffer[programdata->controls[LEFT]]) {
			if (programdata->selection > 0) {
				--programdata->selection;
				wav_setstate(move,PLAY);
			}
		} else if (programdata->buffer[programdata->controls[RIGHT]]) {
			if (programdata->selection < 3) {
				++programdata->selection;
				wav_setstate(move,PLAY);
			}
		}

		if (programdata->buffer[programdata->controls[ACTION1]]) {
				programdata->guess[0] = 0;
				programdata->guess[1] = 0;
				programdata->guess_position = 1;
		}  else if (programdata->buffer[programdata->controls[ACTION2]]) {
				if (programdata->grid[programdata->selection][18] != s32_blank) {
					if (check_answer(programdata) == 1) {
						programdata->grid[programdata->selection][18] = s32_blank;
						programdata->guess_position = 1;
						programdata->guess[0] = 0;
						programdata->guess[1] = 0;
						programdata->hit++;
						programdata->combo_current_time = time(NULL);
						programdata->combo_total_time = (int)(programdata->combo_current_time - programdata->combo_start_time);
						programdata->piecelimit[programdata->selection]++;
						if (programdata->combo_total_time > 0 && programdata->combo_total_time <= 3) {
							programdata->combo_xposition = 142;
							if (programdata->combo_accumulator < 5) {
								programdata->combo_accumulator +=1;
							}
						} else {
							programdata->combo_accumulator = 1;
						}
						programdata->score += programdata->combo_accumulator * 1;
						programdata->combo_start_time = time(NULL);
						process_grid(programdata,channel);
						wav_setstate(correct,PLAY);
					} else {
						programdata->combo_accumulator = 1;
						programdata->miss++;
						wav_setstate(wrong,PLAY);
					}
					programdata->total++;
				}
		}

		for (int i = ZERO, j = 0; i < NINE+1; i++,j++) {
			if (programdata->buffer[programdata->controls[i]]) {
				if (programdata->guess_position == 1) {
					programdata->guess[0] = 0;
					programdata->guess[1] = 0;
				} else {
					programdata->guess[1] = programdata->guess[0];
				}
				programdata->guess_position = 1 - programdata->guess_position;
				programdata->guess[0] = j;
				wav_setstate(typing,PLAY);
				break;
			}
		}
	} else if (programdata->screen == GAMEOVER_SCREEN) {
		if (programdata->buffer[SCANCODE_ESCAPE] || programdata->buffer[programdata->controls[ESCAPE]]) {
			programdata->screen = TITLE_SCREEN;
			reset_variables(programdata);
		}
	} else if (programdata->screen == CONFIG_SCREEN) {
		if (programdata->buffer[SCANCODE_ESCAPE] || programdata->buffer[programdata->controls[ESCAPE]]) {
			if (programdata->current_control != 16) {
				programdata->screen = TITLE_SCREEN;
			}
		} else {
			for (int i = 2; i < 108; i++) {
				process_key = 1;
				if (keyboard_keypressed(i)) {
					for(int j = 0; j < 17;j++) {
						if (programdata->controls[j] == i) {
							process_key = 0;
							break;
						}
					}
					if (process_key == 1) {
						programdata->controls[programdata->current_control] = i;
						wav_setstate(selection,PLAY);
					}
					programdata->current_control++;
					if (programdata->current_control == 17) {
						programdata->current_control = UP;
					}
				}
			}
		}
	}  else if (programdata->screen == OPTION_SCREEN) {
		if (programdata->buffer[SCANCODE_ESCAPE] || programdata->buffer[programdata->controls[ESCAPE]]) {
			programdata->screen = TITLE_SCREEN;
		}
		
		if (programdata->buffer[SCANCODE_CURSORUP] || programdata->buffer[SCANCODE_CURSORBLOCKUP]) {
			if (programdata->optionselection > 0) {
				--programdata->optionselection;
				wav_setstate(selection,PLAY);
			}
		} else if (programdata->buffer[SCANCODE_CURSORDOWN] || programdata->buffer[SCANCODE_CURSORBLOCKDOWN]) {
			if (programdata->optionselection < 2) {
				++programdata->optionselection;
				wav_setstate(selection,PLAY);
			}
		} else if (programdata->buffer[SCANCODE_CURSORLEFT] || programdata->buffer[SCANCODE_CURSORBLOCKLEFT]) {
				if (programdata->optionselection == 0) {
					if (programdata->option[programdata->optionselection] > 1) {
						programdata->option[programdata->optionselection] -= 1;
					}
				} else {
					if (programdata->option[programdata->optionselection] > 0) {
						programdata->option[programdata->optionselection] -= 1;
					}

					if (programdata->optionselection == 1) {
						programdata->speed_lvl = programdata->option[programdata->optionselection];
						programdata->goal = (76 * programdata->option[programdata->optionselection]);
						programdata->speed = 13 - programdata->speed_lvl;
					}
				}
		} else if (programdata->buffer[SCANCODE_CURSORRIGHT] || programdata->buffer[SCANCODE_CURSORBLOCKRIGHT]) {
				if (programdata->optionselection == 0) {
					if (programdata->option[programdata->optionselection] < 9) {
						programdata->option[programdata->optionselection] += 1;
					}
				} else if (programdata->optionselection == 1) {
					if (programdata->option[programdata->optionselection] < 5) {
						programdata->option[programdata->optionselection] += 1;
						programdata->goal = (76 * programdata->option[programdata->optionselection]);
						programdata->speed_lvl = programdata->option[programdata->optionselection];
						programdata->speed = 13 - programdata->speed_lvl;
					}
				} else if (programdata->optionselection == 2) {
					if (programdata->option[programdata->optionselection] < 8) {
						programdata->option[programdata->optionselection] += 1;
					}
				}
		}
	} 

	return 0;
}

// Draw sprites using the text, size, x and y position and spacing needed.
int display_sprite(struct prgdata *programdata, int *text, int WXH, int x, int y, int spacing){
	int imgsize = WXH * WXH;
	int asset_num = 0;

	if (WXH == 8) {
		asset_num = 0;
	} else if (WXH == 32) {
		asset_num = 1;
	} else if (WXH == 16) {
		asset_num = 2;
	} 

	for (int position = 1;position<(*text);position++) {
		gl_compileboxmask(WXH,WXH,asset_src[asset_num]+(imgsize*(*(text+position))),asset_dst[asset_num]);
		gl_putboxmaskcompiled(x+((WXH+spacing)*(position-1)),y,WXH,WXH,asset_dst[asset_num]);
	}

	return 0;
}

// Display a notification of a combo being performed.
int display_combo(struct prgdata *programdata) {
	int label_combo[] = {6,s8_c,s8_o,s8_m,s8_b,s8_0};
	int label_xvalue[] = {4,s8_blank,s8_x,programdata->combo_accumulator};
	int y = 215;

	if (programdata->combo_xposition > 82) {
		display_sprite(programdata,label_combo,8,programdata->combo_xposition,y,1);
		display_sprite(programdata,label_xvalue,8,programdata->combo_xposition+44,y,1);
		programdata->combo_xposition -= 30;
	} else {
		display_sprite(programdata,label_combo,8,programdata->combo_xposition,y,1);
		display_sprite(programdata,label_xvalue,8,programdata->combo_xposition+44,y,1);
	}

	return 0;
}

// Display options for the max number, piece speed, and the background.
int display_optionscreen(struct prgdata *programdata) {
	int label_options[] = {10,s8_minus,s8_o,s8_p,s8_t,s8_i,s8_o,s8_n,s8_s,s8_minus};
	int label_maxnumber[] = {11,s8_m,s8_a,s8_x,s8_blank,s8_n,s8_u,s8_m,s8_b,s8_e,s8_r};
	int label_maxnum_set[] = {2,programdata->option[0]};
	int label_speed[] = {6,s8_s,s8_p,s8_e,s8_e,s8_d};
	int label_speed_set[] = {2,programdata->option[1]};
	int label_background[] = {11,s8_b,s8_a,s8_c,s8_k,s8_g,s8_r,s8_o,s8_u,s8_n,s8_d};
	int label_background_number[] = {2,programdata->option[2]};
	int label_cursor[] = {2,s8_cursor2};
	int x = 127;
	int y = 0;

	display_sprite(programdata,label_options,8,x,y,1);

	x = 119;
	y = 87;

	display_sprite(programdata,label_maxnumber,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_maxnum_set,8,x+16,y,1);

	y += 16;

	display_sprite(programdata,label_speed,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_speed_set,8,x+16,y,1);

	y += 16;

	display_sprite(programdata,label_background,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_background_number,8,x+16,y,1);

	x = 103;
	y = 87;

	display_sprite(programdata,label_cursor,8,x,y+(programdata->optionselection*24),1);

	return 0;
}

// Display the currently configured controls. Scancodes are translated to text.
int display_configscreen(struct prgdata *programdata) {
	int label_config[] = {9,s8_minus,s8_c,s8_o,s8_n,s8_f,s8_i,s8_g,s8_minus};
	int label_up[] = {3,s8_u,s8_p};
	int label_down[] = {5,s8_d,s8_o,s8_w,s8_n};
	int label_left[] = {5,s8_l,s8_e,s8_f,s8_t};
	int label_right[] = {6,s8_r,s8_i,s8_g,s8_h,s8_t};
	int label_0[] = {2,s8_0};
	int label_1[] = {2,s8_1};
	int label_2[] = {2,s8_2};
	int label_3[] = {2,s8_3};
	int label_4[] = {2,s8_4};
	int label_5[] = {2,s8_5};
	int label_6[] = {2,s8_6};
	int label_7[] = {2,s8_7};
	int label_8[] = {2,s8_8};
	int label_9[] = {2,s8_9};
	int label_clear[] = {6,s8_c,s8_l,s8_e,s8_a,s8_r};
	int label_submit[] = {7,s8_s,s8_u,s8_b,s8_m,s8_i,s8_t};
	int label_quit[] = {5,s8_q,s8_u,s8_i,s8_t};
	int scancode_0[] = {0};
	int scancode_1[] = {7,s8_e,s8_s,s8_c,s8_a,s8_p,s8_e};
	int scancode_2[] = {2,s8_1};
	int scancode_3[] = {2,s8_2};
	int scancode_4[] = {2,s8_3};
	int scancode_5[] = {2,s8_4};
	int scancode_6[] = {2,s8_5};
	int scancode_7[] = {2,s8_6};
	int scancode_8[] = {2,s8_7};
	int scancode_9[] = {2,s8_8};
	int scancode_10[] = {2,s8_9};
	int scancode_11[] = {2,s8_0};
	int scancode_12[] = {6,s8_m,s8_i,s8_n,s8_u,s8_s};
	int scancode_13[] = {6,s8_e,s8_q,s8_u,s8_a,s8_l};
	int scancode_14[] = {10,s8_b,s8_a,s8_c,s8_k,s8_s,s8_p,s8_a,s8_c,s8_e};
	int scancode_15[] = {4,s8_t,s8_a,s8_b};
	int scancode_16[] = {2,s8_q};
	int scancode_17[] = {2,s8_w};
	int scancode_18[] = {2,s8_e};
	int scancode_19[] = {2,s8_r};
	int scancode_20[] = {2,s8_t};
	int scancode_21[] = {2,s8_y};
	int scancode_22[] = {2,s8_u};
	int scancode_23[] = {2,s8_i};
	int scancode_24[] = {2,s8_o};
	int scancode_25[] = {2,s8_p};
	int scancode_26[] = {13,s8_b,s8_r,s8_a,s8_c,s8_k,s8_e,s8_t,s8_blank,s8_l,s8_e,s8_f,s8_t};
	int scancode_27[] = {14,s8_b,s8_r,s8_a,s8_c,s8_k,s8_e,s8_t,s8_blank,s8_r,s8_i,s8_g,s8_h,s8_t};
	int scancode_28[] = {6,s8_e,s8_n,s8_t,s8_e,s8_r};
	int scancode_29[] = {9,s8_l,s8_e,s8_f,s8_t,s8_c,s8_t,s8_r,s8_l};
	int scancode_30[] = {2,s8_a};
	int scancode_31[] = {2,s8_s}; 
	int scancode_32[] = {2,s8_d}; 
	int scancode_33[] = {2,s8_f}; 
	int scancode_34[] = {2,s8_g}; 
	int scancode_35[] = {2,s8_h}; 
	int scancode_36[] = {2,s8_j}; 
	int scancode_37[] = {2,s8_k}; 
	int scancode_38[] = {2,s8_l}; 
	int scancode_39[] = {10,s8_s,s8_e,s8_m,s8_i,s8_c,s8_o,s8_l,s8_o,s8_n};
	int scancode_40[] = {11,s8_a,s8_p,s8_o,s8_s,s8_t,s8_r,s8_o,s8_p,s8_h,s8_e};
	int scancode_41[] = {6,s8_g,s8_r,s8_a,s8_v,s8_e};
	int scancode_42[] = {10,s8_l,s8_e,s8_f,s8_t,s8_s,s8_h,s8_i,s8_f,s8_t};
	int scancode_43[] = {10,s8_b,s8_a,s8_c,s8_k,s8_s,s8_l,s8_a,s8_s,s8_h};
	int scancode_44[] = {2,s8_z};
	int scancode_45[] = {2,s8_x};
	int scancode_46[] = {2,s8_c};
	int scancode_47[] = {2,s8_v};
	int scancode_48[] = {2,s8_b};
	int scancode_49[] = {2,s8_n};
	int scancode_50[] = {2,s8_m};
	int scancode_51[] = {6,s8_c,s8_o,s8_m,s8_m,s8_a};
	int scancode_52[] = {7,s8_p,s8_e,s8_r,s8_i,s8_o,s8_d};
	int scancode_53[] = {6,s8_s,s8_l,s8_a,s8_s,s8_h};
	int scancode_54[] = {11,s8_r,s8_i,s8_g,s8_h,s8_t,s8_s,s8_h,s8_i,s8_f,s8_t};
	int scancode_55[] = {15,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_m,s8_u,s8_l,s8_t,s8_i,s8_p,s8_l,s8_y};
	int scancode_56[] = {8,s8_l,s8_e,s8_f,s8_t,s8_a,s8_l,s8_t};
	int scancode_57[] = {6,s8_s,s8_p,s8_a,s8_c,s8_e};
	int scancode_58[] = {9,s8_c,s8_a,s8_p,s8_s,s8_l,s8_o,s8_c,s8_k};
 	int scancode_59[] = {3,s8_f,s8_1};
	int scancode_60[] = {3,s8_f,s8_2};
	int scancode_61[] = {3,s8_f,s8_3};
	int scancode_62[] = {3,s8_f,s8_4};
	int scancode_63[] = {3,s8_f,s8_5};
	int scancode_64[] = {3,s8_f,s8_6};
	int scancode_65[] = {3,s8_f,s8_7};
	int scancode_66[] = {3,s8_f,s8_8};
	int scancode_67[] = {3,s8_f,s8_9};
	int scancode_68[] = {4,s8_f,s8_1,s8_0};
	int scancode_69[] = {8,s8_n,s8_u,s8_m,s8_l,s8_o,s8_c,s8_k};
	int scancode_70[] = {12,s8_s,s8_c,s8_r,s8_o,s8_l,s8_l,s8_blank,s8_l,s8_o,s8_c,s8_k};
	int scancode_71[] = {8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_7};
	int scancode_72[] = {8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_8};
	int scancode_73[] = {8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_9};
	int scancode_74[] = {12,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_m,s8_i,s8_n,s8_u,s8_s};
	int scancode_75[] = {8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_4};
	int scancode_76[] ={8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_5};
	int scancode_77[] = {8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_6};
	int scancode_78[] = {11,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_p,s8_l,s8_u,s8_s};
	int scancode_79[] = {8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_1};
	int scancode_80[] = {8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_2};
	int scancode_81[] = {8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_3};
	int scancode_82[] = {8,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_0};
	int scancode_83[] = {11,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_blank,s8_d,s8_o,s8_t};
	int scancode_84[] = {0};
	int scancode_85[] = {0};
	int scancode_86[] = {5,s8_l,s8_e,s8_s,s8_s};
	int scancode_87[] = {4,s8_f,s8_1,s8_1};
	int scancode_88[] = {4,s8_f,s8_1,s8_2}; 
	int scancode_89[] = {12,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_e,s8_n,s8_t,s8_e,s8_r};
	int scancode_90[] = {10,s8_r,s8_i,s8_g,s8_h,s8_t,s8_c,s8_t,s8_r,s8_l};
	int scancode_91[] = {13,s8_k,s8_e,s8_y,s8_p,s8_a,s8_d,s8_d,s8_i,s8_v,s8_i,s8_d,s8_e};
	int scancode_92[] = {12,s8_p,s8_r,s8_i,s8_n,s8_t,s8_s,s8_c,s8_r,s8_e,s8_e,s8_n};
	int scancode_93[] = {9,s8_r,s8_i,s8_g,s8_h,s8_t,s8_a,s8_l,s8_t};
	int scancode_94[] = {5,s8_h,s8_o,s8_m,s8_e};
	int scancode_95[] = {3,s8_u,s8_p};
	int scancode_96[] = {7,s8_p,s8_a,s8_g,s8_e,s8_u,s8_p};
	int scancode_97[] = {5,s8_l,s8_e,s8_f,s8_t};
	int scancode_98[] = {6,s8_r,s8_i,s8_g,s8_h,s8_t};
	int scancode_99[] = {4,s8_e,s8_n,s8_d};
	int scancode_100[] = {5,s8_d,s8_o,s8_w,s8_n};
	int scancode_101[] = {9,s8_p,s8_a,s8_g,s8_e,s8_d,s8_o,s8_w,s8_n};
	int scancode_102[] = {7,s8_i,s8_n,s8_s,s8_e,s8_r,s8_t};
	int scancode_103[] = {7,s8_r,s8_e,s8_m,s8_o,s8_v,s8_e};
	int scancode_104[] = {6,s8_b,s8_r,s8_e,s8_a,s8_k};
	int scancode_105[] = {5,s8_w,s8_i,s8_n,s8_1};
	int scancode_106[] = {5,s8_w,s8_i,s8_n,s8_2};
	int scancode_107[] = {5,s8_c,s8_r,s8_t,s8_l};

	int *codes[] = {
	scancode_0,scancode_1,scancode_2,scancode_3,scancode_4,	scancode_5,scancode_6,scancode_7,scancode_8,scancode_9,
	scancode_10,scancode_11,scancode_12,scancode_13,scancode_14,scancode_15,scancode_16,scancode_17,scancode_18,scancode_19,
	scancode_20,scancode_21,scancode_22,scancode_23,scancode_24,scancode_25,scancode_26,scancode_27,scancode_28,scancode_29,
	scancode_30,scancode_31,scancode_32,scancode_33,scancode_34,scancode_35,scancode_36,scancode_37,scancode_38,scancode_39,
	scancode_40,scancode_41,scancode_42,scancode_43,scancode_44,scancode_45,scancode_46,scancode_47,scancode_48,scancode_49,
	scancode_50,scancode_51,scancode_52,scancode_53,scancode_54,scancode_55,scancode_56,scancode_57,scancode_58,scancode_59,
	scancode_60,scancode_61,scancode_62,scancode_63,scancode_64,scancode_65,scancode_66,scancode_67,scancode_68,scancode_69,
	scancode_70,scancode_71,scancode_72,scancode_73,scancode_74,scancode_75,scancode_76,scancode_77,scancode_78,scancode_79,
	scancode_80,scancode_81,scancode_82,scancode_83,scancode_84,scancode_85,scancode_86,scancode_87,scancode_88,scancode_89,
	scancode_90,scancode_91,scancode_92,scancode_93,scancode_94,scancode_95,scancode_96,scancode_97,scancode_98,scancode_99,
	scancode_100,scancode_101,scancode_102,scancode_103,scancode_104,scancode_105,scancode_106,scancode_107
	};
	int label_cursor[] = {2,s8_cursor2};

	int x = 127;
	int y = 0;

	display_sprite(programdata,label_config,8,x,y,1);

	x = 112;
	y = 56;

	display_sprite(programdata,label_up,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_down,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_left,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_right,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_0,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_1,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_2,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_3,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_4,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_5,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_6,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_7,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_8,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_9,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_clear,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_submit,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_quit,8,x,y,1);

	x = 168;
	y = 56;

	display_sprite(programdata,codes[programdata->controls[UP]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[DOWN]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[LEFT]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[RIGHT]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[ZERO]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[ONE]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[TWO]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[THREE]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[FOUR]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[FIVE]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[SIX]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[SEVEN]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[EIGHT]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[NINE]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[ACTION1]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[ACTION2]],8,x,y,1);

	y += 8;

	display_sprite(programdata,codes[programdata->controls[ESCAPE]],8,x,y,1);

	x = 104;
	y = 56;
	
	y += (8*programdata->current_control);

	display_sprite(programdata,label_cursor,8,x,y,1);

	return 0;
}

// Display the title and menu for the startup screen.
int display_titlescreen(struct prgdata *programdata) {
	int label_start[] = {6,s8_s,s8_t,s8_a,s8_r,s8_t};
	int label_options[] = {8,s8_o,s8_p,s8_t,s8_i,s8_o,s8_n,s8_s};
	int label_config[] = {7,s8_c,s8_o,s8_n,s8_f,s8_i,s8_g};
	int title_solver[] = {7,s32_s,s32_o,s32_l,s32_v,s32_e,s32_r};
	int label_cursor[] = {2,s8_cursor2};
	int x = 63;
	int y = 80;

	gl_clearscreen(0);

	display_sprite(programdata,title_solver,32,63,80,1);

	x += 64;
	y += 64;	
	
	display_sprite(programdata,label_start,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_options,8,x,y,1);

	y += 8;

	display_sprite(programdata,label_config,8,x,y,1);

	y += 8;

	x = 127;
	y = 144;

	if (programdata->menu == START) {
		display_sprite(programdata,label_cursor,8,x-8,y,1);
	} else if (programdata->menu == OPTION) {
		y += 8;
		display_sprite(programdata,label_cursor,8,x-8,y,1);
	} else if (programdata->menu == CONFIG) {
		y += 16;
		display_sprite(programdata,label_cursor,8,x-8,y,1);
	}

	return 0;
}

// Set the background the user has chosen in the options.
int display_background(struct prgdata *programdata){
	int background_number = programdata->option[2];
	if (background_number >= 0 && background_number < 8) {
		gl_compileboxmask(16,16,asset_src[3]+(256*background_number),asset_dst[3]);
		for (int row = 0; row < 15; row++) {
			for (int col=0; col<20; col++) {
				gl_putboxmaskcompiled(col*16,row*16,16,16,background_dst);
			}
		}
	} else {
		gl_clearscreen(0);
	}

	return 0;
}

// Draw the border for the game screen.
int display_border(struct prgdata *programdata){
	int x = 71;
	int y = 15;
	int width = 22;
	int height = 26;
	int border_1[] = {2,upleftborder};
	int border_2[] = {2,uprightborder};
	int border_3[] = {2,loleftborder};
	int border_4[] = {2,lorightborder};
	int border_5[] = {2,topborder};
	int border_6[] = {2,bottomborder};
	int border_7[] = {2,leftborder};
	int border_8[] = {2,blank};
	int border_9[] = {2,rightborder};
	int WXH = 8;

	display_sprite(programdata,border_1,8,x,y,0);

	display_sprite(programdata,border_2,8,x+(WXH*width),y,0);

	display_sprite(programdata,border_3,8,x,y+(WXH*height),0);

	display_sprite(programdata,border_4,8,x+(WXH*width),y+(WXH*height),0);

	for ( int i = 1; i < width; i++) {
		display_sprite(programdata,border_5,8,x+(WXH*i),y,0);
	}

	for ( int i = 1; i < width; i++) {
		display_sprite(programdata,border_6,8,x+(WXH*i),y+(WXH*height),0);
	}

	for ( int i = 1; i < height; i++) {
		display_sprite(programdata,border_7,8,x,y+(WXH*i),0);

		for (int j = 1; j < width; j++) {
			display_sprite(programdata,border_8,8,x+(WXH*j),y+(WXH*i),0);
		}

		display_sprite(programdata,border_9,8,x+(WXH*width),y+(WXH*i),0);
	}

	return 0;
}

// Draw the gameover sign.
int display_gameover(struct prgdata *programdata){
	int x = 124;
	int y = 116;
	int gameover_label[] = {10,s8_g,s8_a,s8_m,s8_e,s8_blank,s8_o,s8_v,s8_e,s8_r};

	if (programdata->screen == GAMEOVER_SCREEN) {
		display_sprite(programdata,gameover_label,8,x,y,0);
	}

	return 0;
}

// Draw all the sprites other than the border or the gameover sign.
int display_gamestats(struct prgdata *programdata, struct wavfile * channel){
	int WXH = 8;
	int x = 71;
	int y = 15;
	int piece_y[4] = {0,0,0,0};
	int temp = 0;
	uint64_t animation_time = 0;
	int score_label[] = {6,s8_s,s8_c,s8_o,s8_r,s8_e};
	int score_value[] = {10,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0};
	int time_label[] = {5,s8_t,s8_i,s8_m,s8_e};
	int time_value[] = {9,s8_0,s8_0,s8_colon,s8_0,s8_0,s8_colon,s8_0,s8_0};
	int speed_label[] = {6,s8_s,s8_p,s8_e,s8_e,s8_d};
	int speed_value[] = {3,s8_0,s8_0};
	int hit_label[] = {4,s8_h,s8_i,s8_t};
	int hit_value[] = {10,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0};
	int miss_label[] = {5,s8_m,s8_i,s8_s,s8_s};
	int miss_value[] = {10,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0};
	int total_label[] = {6,s8_t,s8_o,s8_t,s8_a,s8_l};
	int total_value[] = {10,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0,s8_0};
	int line_label[] = {22,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus,s8_minus};
	int piece1_label[] = {2,programdata->grid[0][18]};
	int piece2_label[] = {2,programdata->grid[1][18]};
	int piece3_label[] = {2,programdata->grid[2][18]};
	int piece4_label[] = {2,programdata->grid[3][18]};
	int piecelimit1_label[] = {3,s8_0,s8_0};
	int piecelimit2_label[] = {3,s8_0,s8_0};
	int piecelimit3_label[] = {3,s8_0,s8_0};
	int piecelimit4_label[] = {3,s8_0,s8_0};

	int cursor1_label[] = {2,s8_cursor1};
	int equation_label[] = {7,
		programdata->bag_equations[programdata->selection][0],
		programdata->bag_equations[programdata->selection][1],
		programdata->bag_equations[programdata->selection][2],
		s16_equal,
		programdata->guess[1],
		programdata->guess[0]};

	x += WXH;
	y += WXH;

	display_sprite(programdata,score_label,8,x,y,0);

	y += WXH;

	temp = programdata->score;
	score_value[9] = temp % 10;
	temp = temp / 10;
	score_value[8] = temp % 10;
	temp = temp / 10;
	score_value[7] = temp % 10;
	temp = temp / 10;
	score_value[6] = temp % 10;
	temp = temp / 10;
	score_value[5] = temp % 10;
	temp = temp / 10;
	score_value[4] = temp % 10;
	temp = temp / 10;
	score_value[3] = temp % 10;
	temp = temp / 10;
	score_value[2] = temp % 10;
	temp = temp / 10;
	score_value[1] = temp % 10;

	display_sprite(programdata,score_value,8,x,y,0);

	y+= WXH*2;

	display_sprite(programdata,speed_label,8,x,y,0);

	y += WXH;

	temp = programdata->speed_lvl;
	speed_value[2] = temp % 10;
	temp = temp / 10;
	speed_value[1] = temp % 10;

	display_sprite(programdata,speed_value,8,x,y,0);

	y+= WXH*2;

	display_sprite(programdata,time_label,8,x,y,0);

	y += WXH;

	temp = programdata->second;
	time_value[8] = temp % 10;
	temp = temp / 10;
	time_value[7] = temp % 10;

	temp = programdata->minute;
	time_value[5] = temp % 10;
	temp = temp / 10;
	time_value[4] = temp % 10;

	temp = programdata->hour;
	time_value[2] = temp % 10;
	temp = temp / 10;
	time_value[1] = temp % 10;
	
	display_sprite(programdata,time_value,8,x,y,0);

	x = 176;
	y = 23;

	display_sprite(programdata,hit_label,8,x,y,0);

	y += WXH;

	temp = programdata->hit;
	hit_value[9] = temp % 10;
	temp = temp / 10;
	hit_value[8] = temp % 10;
	temp = temp / 10;
	hit_value[7] = temp % 10;
	temp = temp / 10;
	hit_value[6] = temp % 10;
	temp = temp / 10;
	hit_value[5] = temp % 10;
	temp = temp / 10;
	hit_value[4] = temp % 10;
	temp = temp / 10;
	hit_value[3] = temp % 10;
	temp = temp / 10;
	hit_value[2] = temp % 10;
	temp = temp / 10;
	hit_value[1] = temp % 10;

	display_sprite(programdata,hit_value,8,x,y,0);

	y += WXH*2;

	display_sprite(programdata,miss_label,8,x,y,0);

	y += WXH;

	temp = programdata->miss;
	miss_value[9] = temp % 10;
	temp = temp / 10;
	miss_value[8] = temp % 10;
	temp = temp / 10;
	miss_value[7] = temp % 10;
	temp = temp / 10;
	miss_value[6] = temp % 10;
	temp = temp / 10;
	miss_value[5] = temp % 10;
	temp = temp / 10;
	miss_value[4] = temp % 10;
	temp = temp / 10;
	miss_value[3] = temp % 10;
	temp = temp / 10;
	miss_value[2] = temp % 10;
	temp = temp / 10;
	miss_value[1] = temp % 10;

	display_sprite(programdata,miss_value,8,x,y,0);

	y += WXH*2;

	display_sprite(programdata,total_label,8,x,y,0);

	y += WXH;

	temp = programdata->total;
	total_value[9] = temp % 10;
	temp = temp / 10;
	total_value[8] = temp % 10;
	temp = temp / 10;
	total_value[7] = temp % 10;
	temp = temp / 10;
	total_value[6] = temp % 10;
	temp = temp / 10;
	total_value[5] = temp % 10;
	temp = temp / 10;
	total_value[4] = temp % 10;
	temp = temp / 10;
	total_value[3] = temp % 10;
	temp = temp / 10;
	total_value[2] = temp % 10;
	temp = temp / 10;
	total_value[1] = temp % 10;

	display_sprite(programdata,total_value,8,x,y,0);

	x = 79;
	y = 15+(WXH*9);

	display_sprite(programdata,line_label,8,x,y,0);

	animation_time = (programdata->animate_current_time.tv_sec - programdata->animate_start_time.tv_sec) * 1000000 + (programdata->animate_current_time.tv_nsec - programdata->animate_start_time.tv_nsec) / 1000;

	for (int i = 0;i<4;i++) {
		if (programdata->animate_piece[i] == 1 && animation_time < 250000) {
			piece_y[i] = 100;
			clock_gettime(CLOCK_MONOTONIC,&(programdata->animate_current_time));
		} else {
			piece_y[i] = 132;
			if (programdata->animate_piece[i] == 1 && programdata->piecelimit[i] != 18) {
				programdata->playdrop = 1;
				programdata->animate_piece[i] = 0;
			}
		}
	}

	x = 99;
	display_sprite(programdata,piece1_label,32,x,piece_y[0],0);
	display_sprite(programdata,piece2_label,32,x+32,piece_y[1],0);
	display_sprite(programdata,piece3_label,32,x+64,piece_y[2],0);
	display_sprite(programdata,piece4_label,32,x+96,piece_y[3],0);

	if ( piece_y[0] == 132 && piece_y[1] == 132 && piece_y[2] == 132 && piece_y[3] == 132) {
		if (programdata->playdrop == 1) {
			programdata->playdrop = 0;
			wav_setstate(drop,PLAY);
		}
	}

	x = 99+(WXH*2);
	y = 20+(WXH*18);

	temp = programdata->piecelimit[0];
	piecelimit1_label[2] = temp % 10;
	temp = temp / 10;
	piecelimit1_label[1] = temp % 10;
	temp = programdata->piecelimit[1];
	piecelimit2_label[2] = temp % 10;
	temp = temp / 10;
	piecelimit2_label[1] = temp % 10;
	temp = programdata->piecelimit[2];
	piecelimit3_label[2] = temp % 10;
	temp = temp / 10;
	piecelimit3_label[1] = temp % 10;
	temp = programdata->piecelimit[3];
	piecelimit4_label[2] = temp % 10;
	temp = temp / 10;
	piecelimit4_label[1] = temp % 10;

	display_sprite(programdata,piecelimit1_label,8,x,y,0);
	x += (WXH*4);
	display_sprite(programdata,piecelimit2_label,8,x,y,0);
	x += (WXH*4);
	display_sprite(programdata,piecelimit3_label,8,x,y,0);
	x += (WXH*4);
	display_sprite(programdata,piecelimit4_label,8,x,y,0);

	x = 107;
	y = 20+(WXH*18);

	display_sprite(programdata,cursor1_label,8,x+(32*programdata->selection),y,0);

	x = 115;
	y = 15+(WXH*22);

	if (programdata->grid[programdata->selection][18] != s32_blank) {
		display_sprite(programdata,equation_label,16,x,y,0);
	}

	return 0;
}

int main(void) {
	if (vga_hasmode(6) < 1) {
		printf("Screen does not support 320x240x256.\n");
		return 0;
	}

	vga_init();
	keyboard_init();

	int randomvalue = 0;
	FILE *fp;
	fp = fopen("/dev/urandom", "rb");
	if (fp != NULL) {
		fread(&randomvalue, sizeof(int),1,fp);
		fclose(fp);
	}

	unsigned int rval = abs(randomvalue);

	if (rval > 0) {
		srand(rval);
	} else {
		srand(time(NULL));
	}

	struct prgdata programdata;
	struct wavfile channel[MAX_SOUNDS];

	wav_init();

	initialize_screen();
	init_variables(&programdata);

  	gl_setpalettecolors(0,256,&palette);
	gl_clearscreen(1);

	while (1) {
		keyboard_update();
		display_background(&programdata);

		if (programdata.exit == 1) {
			break;
		}

		wav_process();

		if ((programdata.goal == programdata.hit) && programdata.speed_lvl < 5) {
			programdata.speed_lvl += 1;
			programdata.goal += programdata.goal;
			programdata.speed -= 1;
		}

		if (programdata.screen == GAME_SCREEN || programdata.screen == GAMEOVER_SCREEN) {

			process_input(&programdata,channel);

			if (programdata.screen == GAME_SCREEN) {
				game_timer(&programdata);
				programdata.piece_current_time = time(NULL);
				if (programdata.grid[0][18] == s32_blank &&
				programdata.grid[1][18] == s32_blank && 
				programdata.grid[2][18] == s32_blank &&
				programdata.grid[3][18] == s32_blank) {
					process_line(&programdata);
					process_grid(&programdata,channel);
				}

				if (programdata.speed == (int)(programdata.piece_current_time - programdata.piece_start_time)) {
					process_line(&programdata);
					process_grid(&programdata,channel);
					programdata.piece_start_time = time(NULL);
					programdata.piece_current_time = time(NULL);
				}
				display_border(&programdata);
				
				if (programdata.combo_accumulator > 1) {
					display_combo(&programdata);
				}
				display_gamestats(&programdata,channel);
			} else if (programdata.screen == GAMEOVER_SCREEN) {
				display_border(&programdata);
				display_gamestats(&programdata,channel);
				display_gameover(&programdata);
			}
		} else if (programdata.screen == TITLE_SCREEN) {
			process_input(&programdata,channel);
			display_titlescreen(&programdata);
		} else if (programdata.screen == OPTION_SCREEN) {
			process_input(&programdata,channel);
			display_optionscreen(&programdata);
		} else if (programdata.screen == CONFIG_SCREEN) {
			process_input(&programdata,channel);
			display_configscreen(&programdata);
		}

		update_screen();
		gl_clearscreen(240);
		keyboard_clearstate();
	}

	uninit_all(&programdata,channel);

	return EXIT_SUCCESS;
}
