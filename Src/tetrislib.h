/*
 * tetrislib.h
 *
 *  Created on: 26 Ara 2018
 *      Author: Mesut
 */

#include <stdlib.h>
#include "stdint.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "sleep.h"
#include "numbers.h"

#ifndef SRC_TETRISLIB_H_
#define SRC_TETRISLIB_H_

#define ADDR_DEBUG 0

//Row address
#define EMPTY_BASE 0x40000400U
#define ROW0_BASE 0x40000000U
#define ROW1_BASE 0x40000028
#define ROW2_BASE 0x40000050
#define ROW3_BASE 0x40000078
#define ROW4_BASE 0x400000A0
#define ROW5_BASE 0x400000C8
#define ROW6_BASE 0x400000F0
#define ROW7_BASE 0x40000118
#define ROW8_BASE 0x40000140
#define ROW9_BASE 0x40000168
#define ROW10_BASE 0x40000190
#define ROW11_BASE 0x400001B8
#define ROW12_BASE 0x400001E0
#define ROW13_BASE 0x40000208
#define ROW14_BASE 0x40000230
#define ROW15_BASE 0x40000258
#define ROW16_BASE 0x40000280
#define MINIMAP_BASE 0x400002A8U

struct shape_struct{
	int id;
	int convert_id;
	int coord[5][2];
	int last_coord[5][2];
	int color;
}shape;


volatile int tetris_map[17][10];
volatile int tetris_color_map[17][10];
volatile int tetris_minimap[8];


void set_cube(int row, int col, int status,  int color);
void set_zero(void);
void set_full_map(int status, int color);
void set_row(int row, int status, int color);
void set_col(int col, int status, int color);
int valid_addr(uint32_t addrr,uint32_t value);
void set_map(int data[17][10], int color);
void delay(int miliseconds);
void init_btns(XGpio *InstancePtr);
int get_btns(XGpio *InstancePtr, int id);
void test_minimap_bram(uint32_t value);
void test_bram(int n, uint32_t value);
void init_seq(int wait_until);
void init_game(XGpio *InstancePtr);
void update_map();

void set_color(int i, int j);
int get_coord(int i, int j);
int get_coordx(int i);
int get_coordy(int i);
void set_coord(int i, int j, int val);

int get_last_coord(int i, int j);
int get_last_coordx(int i);
int get_last_coordy(int i);
void  set_last_coord(int i, int j, int val);

void cop2last();

void set_minicube(int col, int status,  int color);
void set_minimap();

void calculate_coord();
void set_figure_positions();
void set_last_figure_pos();

void shift_map_down(int row);

void check_map();

void new_figure();
void new_figure_pos();

int get_map_val(int i);
int get_map_valx(int i);
int get_map_valxr(int i);

void shift_left_all(void);
void shift_left(void);

void shift_right_all(void);
void shift_right(void);

void shift_down();

void rotate_shape();

int isDownPossible();


#endif /* SRC_TETRISLIB_H_ */
