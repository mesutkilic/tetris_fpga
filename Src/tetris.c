/*
 * Tetris Game
 * Mesut KILIC
 * 10.01.2019
 *
 */

#include <stdio.h>
#include "platform.h"

#include "tetrislib.h"
#include "numbers.h"

int dly=75, dly_counter=0;
int game_status=1, score=0;


int main()
{
	init_platform();
	XGpio btn;
	init_game(&btn);

	init_seq(10);
	//set_map(text_go,4);
	//delay(1000);

	update_map();
	set_full_map(0,0); //clear map
	delay(100);
	shape.id=1;
	shape.convert_id=0;
	calculate_coord();
	cop2last();

	int left_btn, right_btn, down_btn, rotate_btn;

	while(1) {
		left_btn=get_btns(&btn,1);
		right_btn=get_btns(&btn,0);
		down_btn=get_btns(&btn,2);
		rotate_btn=get_btns(&btn,3);

		while(game_status==0){//game over
			right_btn=get_btns(&btn,0);
			if(right_btn){
				init_game(&btn);
				shape.id=0;
				new_figure();
				score=0;
				game_status=1;
			}
		}


		if(left_btn){
			shift_left();
		}
		if(right_btn){
			shift_right();
		}
		if(down_btn){
			shift_down();
		}
		if(rotate_btn){
			rotate_shape();
		}

		//set figure positions
		set_last_figure_pos();
		//set figure positions
		set_figure_positions();
		//check_map();
		update_map();
		delay(dly);
		++dly_counter;

		//check down possible
		if(isDownPossible() && dly_counter>=500/dly){
			//continue
			dly_counter=0;
			//save figure position
			cop2last();
			//set last figure positions 0
			set_last_figure_pos();
			new_figure_pos();
		}else if(dly_counter>=500/dly) {
			dly_counter=0;
			//new figure
			new_figure();
		}

	}

	cleanup_platform();
	return 0;
}
