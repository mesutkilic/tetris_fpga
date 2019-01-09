/*
 * tetrislib.c
 *
 *  Created on: 26 Ara 2018
 *      Author: Mesut
 */
#include "tetrislib.h"

extern int game_status, dly_counter, score;

// Get these from xparamters.h
#define BRAMBASE XPAR_BRAM_0_BASEADDR
#define BRAMHIGH XPAR_BRAM_0_HIGHADDR
#define DEFAULT_COLOR 1

uint32_t row_bases[17]={ROW0_BASE,
		ROW1_BASE, ROW2_BASE, ROW3_BASE,
		ROW4_BASE, ROW5_BASE, ROW6_BASE,
		ROW7_BASE, ROW8_BASE, ROW9_BASE,
		ROW10_BASE, ROW11_BASE, ROW12_BASE,
		ROW13_BASE, ROW14_BASE, ROW15_BASE,
		ROW16_BASE};


uint32_t red = 0xFF000000U;
uint32_t green = 0x00FF0000U;
uint32_t blue = 0x0000FF00U;
uint32_t zero = 0x00000000U;
uint32_t white = 0xFFFFFF00U;
uint32_t yellow = 0xFFFF0000U;
uint32_t turkuaz = 0x00FFFF00U;
uint32_t orange = 0xFF993300U;
uint32_t purple = 0xCC00CC00U;

void set_cube(int row, int col, int status, int color){
	if(row>=0  && row<17 && col>=0 && col<10){}else{return ;}
	tetris_map[row][col]=status;
	tetris_color_map[row][col]=color;
	volatile uint32_t *ptr = (volatile uint32_t*)(row_bases[row]);
	volatile uint32_t ptr_addr, ptr_val, new_;
	for (int i=0; i<col; ++i){
		ptr++;
	}

	again:
	switch (color) {
		case 0: new_ = zero; break;
		case 1: new_ = red; break;
		case 2: new_ = green; break;
		case 3: new_ = blue; break;
		case 4: new_ = white; break;
		case 5: new_ = yellow; break;
		case 6: new_ = turkuaz; break;
		case 7: new_ = orange; break;
		case 8: new_ = purple; break;
		default: new_ = green; break;
	}
	if(status){
		new_ += 0xAA;
	}else {

	}
	*ptr = new_;
	ptr_addr = (volatile uint32_t)ptr;
	ptr_val = *ptr;
	if(!valid_addr(ptr_addr, ptr_val)){
		goto again;
	}
	//delay(10);
	if(ADDR_DEBUG){
		print("Reading 32-bit values from memory region:\n");
		xil_printf("0x%p: %x\n", ptr, *ptr);
	}
}

void set_zero(void){
	volatile uint32_t *ptr = (volatile uint32_t*)(EMPTY_BASE);
	*ptr = zero;
}

void set_full_map(int status, int color){
	for(int i=0; i<17; ++i){
		for(int j=0;j<10; ++j){
			set_cube(i,j,status,color);
		}
	}
}

void set_row(int row, int status, int color){
	for(int i=0; i<10;++i){
		set_cube(row,i,status,color);
		tetris_color_map[row][i]=color;
	}
}

void set_col(int col, int status, int color){
	for(int i=0; i<17;++i){
		set_cube(i,col,status,color);
		tetris_color_map[i][col]=color;
	}
}

int valid_addr(uint32_t addrr,uint32_t value){
	volatile uint32_t *ptr = (volatile uint32_t*)(addrr);
	if(value==*ptr){return 1;}
	else{return 0;}
}

void set_map(int data[17][10], int color){
	for(int i=0; i<17; ++i){
		for(int j=0;j<10; ++j){
			set_cube(i,j,data[i][j],color);
		}
	}
}

void delay(int miliseconds){
	usleep(1000*miliseconds);
}

int get_btns(XGpio *InstancePtr, int id){
	u32 b = XGpio_DiscreteRead(InstancePtr, 1);
	switch (b) {
		case 0: return 0; break;
		case 1: if(id==0){return 1;}else{return 0;} break;
		case 2: if(id==1){return 1;}else{return 0;} break;
		case 4: if(id==2){return 1;}else{return 0;} break;
		case 8: if(id==3){return 1;}else{return 0;} break;
		default: return 0; break;
	}
}

void init_btns(XGpio *InstancePtr){
	int status;
	print("Initializing Buttons & Switches\n");
	status = XGpio_Initialize(InstancePtr, XPAR_AXI_GPIO_0_DEVICE_ID);
	if (status != XST_SUCCESS) {
		//return XST_FAILURE;
		print("\nInitializing Buttons & Switches failed \n");
	}
	// set the direction to input
	XGpio_SetDataDirection(InstancePtr, 1, 0xFF); // Channel 1
}

void test_minimap_bram(uint32_t value){
	xil_printf("Testing MINIMAP memory region : 0x%x\n", MINIMAP_BASE);
	volatile uint32_t *ptr = (volatile uint32_t*)(MINIMAP_BASE);
	for (int h = 0; h < 16; h++) {
		//*ptr++ = 0xFFFFFFAA ;
		//*ptr++ = 0x00000000;
		*ptr++ = value;
	}

	print("Reading 32-bit values from memory region:\n");

	volatile uint32_t *ptr2 = (volatile uint32_t*)(MINIMAP_BASE);
	for (int h = 0; h < 16; h++) {
		xil_printf("0x%p: %x\n", ptr2, *ptr2);
		*ptr2++;
	}
}

void test_bram(int n, uint32_t value){
	xil_printf("Testing memory region : 0x%x - 0x%x\n", BRAMBASE, BRAMHIGH);
	xil_printf("Writing %d bytes to memory region:\n", n);
	volatile uint32_t *ptr = (volatile uint32_t*)(BRAMBASE);
	for (int h = 0; h < n; h++) {
		//*ptr++ = 0xFFFFFFAA ;
		//*ptr++ = 0x00000000;
		*ptr++ = value;
	}

	print("Reading 32-bit values from memory region:\n");

	volatile uint32_t *ptr2 = (volatile uint32_t*)(BRAMBASE);
	for (int h = 0; h < n; h++) {
		xil_printf("0x%p: %x\n", ptr2, *ptr2);
		*ptr2++;
	}

}

void init_seq(int wait_until){
	delay(wait_until);
	set_map(number_10,7);
	delay(1000);
	set_map(number_9,6);
	delay(1000);
	set_map(number_8,8);
	delay(1000);
	set_map(number_7,7);
	delay(1000);
	set_map(number_6,6);
	delay(1000);
	set_map(number_5,5);
	delay(1000);
	set_map(number_4,4);
	delay(1000);
	set_map(number_3,3);
	delay(1000);
	set_map(number_2,2);
	delay(1000);
	set_map(number_1,1);
	delay(1000);
	set_map(number_0,4);
	delay(1000);
	set_map(text_go,4);
	delay(1000);
}

void init_game(XGpio *InstancePtr){
	if(game_status){init_btns(InstancePtr);}
	// Create a pointer to the base of the starting address of the buffer
	volatile uint32_t *objs = (volatile uint32_t*)(XPAR_OBJECTBUFFER_0_S00_AXI_BASEADDR);
	objs[0] = 0x0000000; //dark

	set_zero();
	set_full_map(0,DEFAULT_COLOR);
	print("TETRIS Starting\n\r");

}

void update_map(){
	for(int i=0; i<17; ++i){
		for(int j=0;j<10; ++j){
			set_cube(i,j,tetris_map[i][j],tetris_color_map[i][j]);
		}
	}
}

void set_color(int i, int j){
	int color=2;
	switch (shape.id) {
		case 0:	 color=5; break;
		case 1:	 color=6; break;
		case 2:	 color=7; break;
		case 3:	 color=3; break;
		case 4:	 color=1; break;
		case 5:	 color=2; break;
		case 6:	 color=8; break;
		default: color=2; break;
	}
	shape.color=color;
	tetris_color_map[i][j]=color;
}


int get_coord(int i, int j){
	return shape.coord[i][j];
}
int get_coordx(int i){
	return shape.coord[i][1];
}
int get_coordy(int i){
	return shape.coord[i][0];
}
void set_coord(int i, int j, int val){
	shape.coord[i][j]=val;
	set_color(i,j);
}

int get_last_coord(int i, int j){
	return shape.last_coord[i][j];
}
int get_last_coordx(int i){
	return shape.last_coord[i][1];
}
int get_last_coordy(int i){
	return shape.last_coord[i][0];
}
void  set_last_coord(int i, int j, int val){
	shape.last_coord[i][j]=val;
}
void cop2last(){
	//save figure position
	for(int i=0; i<5; ++i){
		for(int j=0; j<2; ++j){
			set_last_coord(i,j,get_coord(i,j));
		}
	}
}

void set_minicube(int col, int status,  int color){
	volatile uint32_t *ptr = (volatile uint32_t*)(MINIMAP_BASE);
	volatile uint32_t new_;
	for (int i=0; i<col; ++i){
		ptr++;
	}
	uint32_t red = 0xFF000000U;
	uint32_t green = 0x00FF0000U;
	uint32_t blue = 0x0000FF00U;
	uint32_t zero = 0x00000000U;
	uint32_t white = 0xFFFFFF00U;
	uint32_t yellow = 0xFFFF0000U;
	uint32_t turkuaz = 0x00FFFF00U;
	uint32_t orange = 0xFF993300U;
	uint32_t purple = 0xCC00CC00U;
	switch (color) {
		case 0: new_ = zero; break;
		case 1: new_ = red; break;
		case 2: new_ = green; break;
		case 3: new_ = blue; break;
		case 4: new_ = white; break;
		case 5: new_ = yellow; break;
		case 6: new_ = turkuaz; break;
		case 7: new_ = orange; break;
		case 8: new_ = purple; break;
		default: new_ = green; break;
	}
	if(status){
		new_ += 0xAA;
	}else {

	}
	*ptr = new_;
	//delay(10);
	if(ADDR_DEBUG){
		print("Reading 32-bit values from memory region:\n");
		xil_printf("0x%p: %x\n", ptr, *ptr);
	}
}

void set_minimap(){
	switch ((shape.id+1)%7) {
		case 0:
			tetris_minimap[0] = 0;
			tetris_minimap[1] = 0;
			tetris_minimap[2] = 1;
			tetris_minimap[3] = 1;
			tetris_minimap[4] = 1;
			tetris_minimap[5] = 1;
			tetris_minimap[6] = 0;
			tetris_minimap[7] = 0;
		break;
		case 1:
			tetris_minimap[0] = 1;
			tetris_minimap[1] = 0;
			tetris_minimap[2] = 1;
			tetris_minimap[3] = 0;
			tetris_minimap[4] = 1;
			tetris_minimap[5] = 0;
			tetris_minimap[6] = 1;
			tetris_minimap[7] = 0;
		break;
		case 2:
			tetris_minimap[0] = 1;
			tetris_minimap[1] = 0;
			tetris_minimap[2] = 1;
			tetris_minimap[3] = 0;
			tetris_minimap[4] = 1;
			tetris_minimap[5] = 1;
			tetris_minimap[6] = 0;
			tetris_minimap[7] = 0;
		break;
		case 3:
			tetris_minimap[0] = 0;
			tetris_minimap[1] = 1;
			tetris_minimap[2] = 0;
			tetris_minimap[3] = 1;
			tetris_minimap[4] = 1;
			tetris_minimap[5] = 1;
			tetris_minimap[6] = 0;
			tetris_minimap[7] = 0;
		break;
		case 4:
			tetris_minimap[0] = 0;
			tetris_minimap[1] = 1;
			tetris_minimap[2] = 1;
			tetris_minimap[3] = 1;
			tetris_minimap[4] = 1;
			tetris_minimap[5] = 0;
			tetris_minimap[6] = 0;
			tetris_minimap[7] = 0;
		break;
		case 5:
			tetris_minimap[0] = 1;
			tetris_minimap[1] = 0;
			tetris_minimap[2] = 1;
			tetris_minimap[3] = 1;
			tetris_minimap[4] = 0;
			tetris_minimap[5] = 1;
			tetris_minimap[6] = 0;
			tetris_minimap[7] = 0;
		break;
		case 6:
			tetris_minimap[0] = 1;
			tetris_minimap[1] = 0;
			tetris_minimap[2] = 1;
			tetris_minimap[3] = 1;
			tetris_minimap[4] = 1;
			tetris_minimap[5] = 0;
			tetris_minimap[6] = 0;
			tetris_minimap[7] = 0;
		break;
	}

	int color=2;
	switch ((shape.id+1)%7) {
		case 0:	 color=5; break;
		case 1:	 color=6; break;
		case 2:	 color=7; break;
		case 3:	 color=3; break;
		case 4:	 color=1; break;
		case 5:	 color=2; break;
		case 6:	 color=8; break;
		default: color=2; break;
	}
	for(int i=0; i<8; ++i){
		set_minicube(i,tetris_minimap[i],color);
	}
}
void calculate_coord(){
	if(shape.id==0){
		set_coord(1,0,16);
		set_coord(1,1,4);
		set_coord(2,1,get_coordx(1)+1);
		set_coord(2,0,get_coordy(1));
		set_coord(3,1,get_coordx(1));
		set_coord(3,0,get_coordy(1)-1);
		set_coord(4,1,get_coordx(1)+1);
		set_coord(4,0,get_coordy(1)-1);
	}else if(shape.id==1){
		set_coord(1,0,16);
		set_coord(1,1,4);
		set_coord(2,1,get_coordx(1));
		set_coord(2,0,get_coordy(1)-1);
		set_coord(3,1,get_coordx(1));
		set_coord(3,0,get_coordy(1)-2);
		set_coord(4,1,get_coordx(1));
		set_coord(4,0,get_coordy(1)-3);
	}else if(shape.id==2){
		set_coord(1,0,16);
		set_coord(1,1,4);
		set_coord(2,1,get_coordx(1));
		set_coord(2,0,get_coordy(1)-1);
		set_coord(3,1,get_coordx(1));
		set_coord(3,0,get_coordy(1)-2);
		set_coord(4,1,get_coordx(1)+1);
		set_coord(4,0,get_coordy(1)-2);
	}else if(shape.id==3){
		set_coord(1,0,16);
		set_coord(1,1,5);
		set_coord(2,1,get_coordx(1));
		set_coord(2,0,get_coordy(1)-1);
		set_coord(3,1,get_coordx(1));
		set_coord(3,0,get_coordy(1)-2);
		set_coord(4,1,get_coordx(1)-1);
		set_coord(4,0,get_coordy(1)-2);
	}else if(shape.id==4){
		set_coord(1,0,16);
		set_coord(1,1,5);
		set_coord(2,1,get_coordx(1));
		set_coord(2,0,get_coordy(1)-1);
		set_coord(3,1,get_coordx(1)-1);
		set_coord(3,0,get_coordy(1)-1);
		set_coord(4,1,get_coordx(1)-1);
		set_coord(4,0,get_coordy(1)-2);
	}else if(shape.id==5){
		set_coord(1,0,16);
		set_coord(1,1,4);
		set_coord(2,1,get_coordx(1));
		set_coord(2,0,get_coordy(1)-1);
		set_coord(3,1,get_coordx(1)+1);
		set_coord(3,0,get_coordy(1)-1);
		set_coord(4,1,get_coordx(1)+1);
		set_coord(4,0,get_coordy(1)-2);
	}else if(shape.id==6){
		set_coord(1,0,16);
		set_coord(1,1,5);
		set_coord(2,1,get_coordx(1)-1);
		set_coord(2,0,get_coordy(1)-1);
		set_coord(3,1,get_coordx(1));
		set_coord(3,0,get_coordy(1)-1);
		set_coord(4,1,get_coordx(1)+1);
		set_coord(4,0,get_coordy(1)-1);
	}
	set_minimap();
}

void set_figure_positions(){
	for(int i=0; i<4; ++i){
		tetris_map[ get_coordy(i+1) ][ get_coordx(i+1) ]=1;
		set_color(get_coordy(i+1),get_coordx(i+1));
	}
	//update_map();
}

void set_last_figure_pos(){
	for(int i=0; i<4; ++i){
		tetris_map[ get_last_coordy(i+1) ][ get_last_coordx(i+1) ]=0;
		tetris_color_map[get_last_coordy(i+1)][get_last_coordx(i+1)]=0;
	}
	//update_map();
}


void shift_map_down(int row){
	for(int i=row; i<16; ++i){
		for(int j=0; j<10; ++j){
			tetris_map[i][j]=tetris_map[i+1][j];
		}
	}
	set_row(16,0,0);
}

void check_map(){
	int same_counter=0;
	for(int i=0; i<17; ++i){
		for(int j=0; j<10; ++j){
			if(tetris_map[i][j]==1){
				same_counter++;
			}
			if(i==16){
				if(tetris_map[i][j]==1 && (j==4 || j==5)){
					//game over
					game_status=0;
					print("\n\nGAME OVER\n\n");
					xil_printf("SCORE : %d\n", score);
					print("Press btn[0] to start the game.\n");
				}
			}
		}
		if(same_counter==10){
			//row delete
			shift_map_down(i);
			score++;
			xil_printf("SCORE : %d\n", score);
			check_map();
		}else{same_counter=0;}
	}
}

void new_figure(){
	shape.id++;
	shape.id=shape.id%7;
	shape.convert_id=0;
	calculate_coord();
	cop2last();
	check_map();
}
void new_figure_pos(){
	//calculate new figure positions
	for(int i=0; i<4; ++i){
		set_coord(i+1,0,get_last_coord(i+1,0)-1);
	}
}

int get_map_val(int i){//for y axis
	return tetris_map[get_coordy(i)-1][get_coordx(i)];
}
int get_map_valx(int i){//for shift lef
	return tetris_map[get_coordy(i)][get_coordx(i)-1];
}
int get_map_valxr(int i){//for shift right
	return tetris_map[get_coordy(i)][get_coordx(i)+1];
}

void shift_left_all(void){
	cop2last();
	for(int i=0; i<5; ++i){
		set_coord(i,1,get_coordx(i)-1);
	}
	set_last_figure_pos();
	set_figure_positions();
}
void shift_left(void){
	int cont=0;
	switch (shape.id) {
	case 0:
		if(shape.convert_id==0){
			if(get_coordx(1)==0){cont=0;}
			else if(get_map_valx(1)==1 || get_map_valx(3)==1){cont=0;}else {cont=1; }
		}
	break;
	case 1:
		if(shape.convert_id==0){
			if(get_coordx(1)==0){cont=0;}
			else if(get_map_valx(4)==1 || get_map_valx(3)==1 ||
					get_map_valx(2)==1 || get_map_valx(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valx(4)==1){cont=0;}else {cont=1; }
		}
	break;
	case 2:
		if(shape.convert_id==0){
			if(get_coordx(1)==0){cont=0;}
			else if(get_map_valx(3)==1 || get_map_valx(2)==1 ||
					get_map_valx(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valx(3)==1 || get_map_valx(4)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==2){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valx(1)==1 || get_map_valx(2)==1 ||
					get_map_valx(4)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==3){
			if(get_coordx(1)==0){cont=0;}
			else if(get_map_valx(1)==1 || get_map_valx(4)==1){cont=0;}else {cont=1; }
		}
	break;
	case 3:
		if(shape.convert_id==0){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valx(4)==1 || get_map_valx(2)==1 ||
					get_map_valx(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valx(3)==1 || get_map_valx(4)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==2){
			if(get_coordx(1)==0){cont=0;}
			else if(get_map_valx(1)==1 || get_map_valx(2)==1 ||
					get_map_valx(3)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==3){
			if(get_coordx(1)==0){cont=0;}
			else if(get_map_valx(1)==1 || get_map_valx(4)==1){cont=0;}else {cont=1; }
		}
	break;
	case 4:
		if(shape.convert_id==0){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valx(3)==1 || get_map_valx(4)==1 ||
					get_map_valx(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valx(2)==1 || get_map_valx(4)==1){cont=0;}else {cont=1; }
		}
	break;
	case 5:
		if(shape.convert_id==0){
			if(get_coordx(1)==0){cont=0;}
			else if(get_map_valx(4)==1 || get_map_valx(2)==1 ||
					get_map_valx(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valx(2)==1 || get_map_valx(4)==1){cont=0;}else {cont=1; }
		}
	break;
	case 6:
		if(shape.convert_id==0){
			if(get_coordx(2)==0){cont=0;}
			else if(get_map_valx(1)==1 || get_map_valx(2)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(2)==0){cont=0;}
			else if(get_map_valx(3)==1 || get_map_valx(4)==1 ||
					get_map_valx(2)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==2){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valx(1)==1 || get_map_valx(4)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==3){
			if(get_coordx(1)==0){cont=0;}
			else if(get_map_valx(1)==1 || get_map_valx(4)==1 ||
					get_map_valx(2)==1){cont=0;}else {cont=1; }
		}
	break;
	default:cont=0;break;
	}

	if(cont){
		shift_left_all();
	}

}

void shift_right_all(void){
	cop2last();
	for(int i=0; i<5; ++i){
		set_coord(i,1,get_coordx(i)+1);
	}
	set_last_figure_pos();
	set_figure_positions();
}
void shift_right(void){
	int cont=0;
	switch (shape.id) {
	case 0:
		if(shape.convert_id==0){
			if(get_coordx(2)==9){cont=0;}
			else if(get_map_valxr(2)==1 || get_map_valxr(4)==1){cont=0;}else {cont=1; }
		}
	break;
	case 1:
		if(shape.convert_id==0){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(4)==1 || get_map_valxr(3)==1 ||
					get_map_valxr(2)==1 || get_map_valxr(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(1)==1){cont=0;}else {cont=1; }
		}
	break;
	case 2:
		if(shape.convert_id==0){
			if(get_coordx(4)==9){cont=0;}
			else if(get_map_valxr(4)==1 || get_map_valxr(2)==1 ||
					get_map_valxr(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(1)==1 || get_map_valxr(4)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==2){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(1)==1 || get_map_valxr(2)==1 ||
					get_map_valxr(3)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==3){
			if(get_coordx(4)==9){cont=0;}
			else if(get_map_valxr(3)==1 || get_map_valxr(4)==1){cont=0;}else {cont=1; }
		}
	break;
	case 3:
		if(shape.convert_id==0){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(3)==1 || get_map_valxr(2)==1 ||
					get_map_valxr(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(1)==1 || get_map_valxr(4)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==2){
			if(get_coordx(4)==9){cont=0;}
			else if(get_map_valxr(1)==1 || get_map_valxr(2)==1 ||
					get_map_valxr(4)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==3){
			if(get_coordx(4)==9){cont=0;}
			else if(get_map_valxr(3)==1 || get_map_valxr(4)==1){cont=0;}else {cont=1; }
		}
	break;
	case 4:
		if(shape.convert_id==0){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(2)==1 || get_map_valxr(4)==1 ||
					get_map_valxr(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(1)==1 || get_map_valxr(3)==1){cont=0;}else {cont=1; }
		}
	break;
	case 5:
		if(shape.convert_id==0){
			if(get_coordx(4)==9){cont=0;}
			else if(get_map_valxr(3)==1 || get_map_valxr(4)==1 ||
					get_map_valxr(1)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(1)==1 || get_map_valxr(3)==1){cont=0;}else {cont=1; }
		}
	break;
	case 6:
		if(shape.convert_id==0){
			if(get_coordx(4)==9){cont=0;}
			else if(get_map_valxr(1)==1 || get_map_valxr(4)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==1){
			if(get_coordx(1)==9){cont=0;}
			else if(get_map_valxr(1)==1 || get_map_valxr(4)==1 ||
					get_map_valxr(2)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==2){
			if(get_coordx(2)==9){cont=0;}
			else if(get_map_valxr(1)==1 || get_map_valxr(2)==1){cont=0;}else {cont=1; }
		}else if(shape.convert_id==3){
			if(get_coordx(4)==0){cont=0;}
			else if(get_map_valxr(3)==1 || get_map_valxr(4)==1 ||
					get_map_valxr(2)==1){cont=0;}else {cont=1; }
		}
	break;
	default:cont=0;break;
	}

	if(cont){
		shift_right_all();
	}

}

void shift_down(){

	dly_counter+=20;
	//delay(10);
}

void rotate_shape(){
	int x, y;
	switch (shape.id) {
		case 1:if(shape.convert_id==0){//convert to 1
			cop2last();
			x=get_last_coordx(1);
			y=get_last_coordy(1);
			if(tetris_map[y][x-1]==1 || tetris_map[y][x-2]==1 ||
					tetris_map[y][x-3]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x);
			set_coord(2,1,x-1);
			set_coord(2,0,y);
			set_coord(3,1,x-2);
			set_coord(3,0,y);
			set_coord(4,1,x-3);
			set_coord(4,0,y);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=1;
		}else if(shape.convert_id==1){//convert to 0
			cop2last();
			x=get_last_coordx(1);
			y=get_last_coordy(1);
			if(tetris_map[y-1][x]==1 || tetris_map[y-2][x]==1 ||
					tetris_map[y-3][x]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x);
			set_coord(2,1,x);
			set_coord(2,0,y-1);
			set_coord(3,1,x);
			set_coord(3,0,y-2);
			set_coord(4,1,x);
			set_coord(4,0,y-3);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=0;
		}
		break;
		case 2:if(shape.convert_id==0){//convert to 1
			cop2last();
			x=get_last_coordx(1);
			y=get_last_coordy(1);
			if(tetris_map[y][x+1]==1 || tetris_map[y][x+2]==1 ||
					tetris_map[y+1][x]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x+2);
			set_coord(2,1,x+1);
			set_coord(2,0,y);
			set_coord(3,1,x);
			set_coord(3,0,y);
			set_coord(4,1,x);
			set_coord(4,0,y-1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=1;
		}else if(shape.convert_id==1){//convert to 2
			cop2last();
			x=get_last_coordx(1);
			y=get_last_coordy(1);
			if(tetris_map[y-2][x]==1 || tetris_map[y-1][x]==1 ||
					tetris_map[y][x-1]==1){
				break;
			}
			set_coord(1,0,y-2);
			set_coord(1,1,x);
			set_coord(2,1,x);
			set_coord(2,0,y-1);
			set_coord(3,1,x);
			set_coord(3,0,y);
			set_coord(4,1,x-1);
			set_coord(4,0,y);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=2;
		}else if(shape.convert_id==2){//convert to 3
			cop2last();
			x=get_last_coordx(3);
			y=get_last_coordy(3);
			if(tetris_map[y-1][x]==1 || tetris_map[y][x-2]==1 ||
				tetris_map[y][x-1]==1){
				break;
			}
			set_coord(1,0,y-1);
			set_coord(1,1,x-2);
			set_coord(2,1,x-1);
			set_coord(2,0,y-1);
			set_coord(3,1,x);
			set_coord(3,0,y-1);
			set_coord(4,1,x);
			set_coord(4,0,y);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=3;
		}else if(shape.convert_id==3){//convert to 0
			cop2last();
			x=get_last_coordx(3);
			y=get_last_coordy(3);
			if(tetris_map[y-1][x]==1 || tetris_map[y-2][x]==1 ||
				tetris_map[y][x-1]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x);
			set_coord(2,1,x);
			set_coord(2,0,y-1);
			set_coord(3,1,x);
			set_coord(3,0,y-2);
			set_coord(4,1,x-1);
			set_coord(4,0,y-1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=0;
		}
		break;
		case 3:if(shape.convert_id==0){//convert to 1
			cop2last();
			x=get_last_coordx(2);
			y=get_last_coordy(2);
			if(tetris_map[y][x-1]==1 || tetris_map[y][x+1]==1 ||
					tetris_map[y+1][x]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x+1);
			set_coord(2,1,x);
			set_coord(2,0,y);
			set_coord(3,1,x-1);
			set_coord(3,0,y);
			set_coord(4,1,x-1);
			set_coord(4,0,y+1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=1;
		}else if(shape.convert_id==1){//convert to 2
			cop2last();
			x=get_last_coordx(2);
			y=get_last_coordy(2);
			if(tetris_map[y+1][x]==1 || tetris_map[y-1][x]==1 ||
				tetris_map[y][x+1]==1){
				break;
			}
			set_coord(1,0,y-1);
			set_coord(1,1,x);
			set_coord(2,1,x);
			set_coord(2,0,y);
			set_coord(3,1,x);
			set_coord(3,0,y+1);
			set_coord(4,1,x+1);
			set_coord(4,0,y+1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=2;
		}else if(shape.convert_id==2){//convert to 3
			cop2last();
			x=get_last_coordx(3);
			y=get_last_coordy(3);
			if(tetris_map[y-1][x]==1 || tetris_map[y][x+1]==1 ||
				tetris_map[y][x-1]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x-1);
			set_coord(2,1,x);
			set_coord(2,0,y);
			set_coord(3,1,x+1);
			set_coord(3,0,y);
			set_coord(4,1,x+1);
			set_coord(4,0,y-1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=3;
		}else if(shape.convert_id==3){//convert to 0
			cop2last();
			x=get_last_coordx(3);
			y=get_last_coordy(3);
			if(tetris_map[y-2][x]==1 || tetris_map[y-1][x]==1 ||
				tetris_map[y][x-1]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x);
			set_coord(2,1,x);
			set_coord(2,0,y-1);
			set_coord(3,1,x);
			set_coord(3,0,y-2);
			set_coord(4,1,x-1);
			set_coord(4,0,y-2);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=0;
		}
		break;
		case 4:if(shape.convert_id==0){//convert to 1
			cop2last();
			x=get_last_coordx(2);
			y=get_last_coordy(2);
			if(tetris_map[y][x-1]==1 || tetris_map[y][x-2]==1 ||
				tetris_map[y+1][x]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x);
			set_coord(2,1,x-1);
			set_coord(2,0,y);
			set_coord(3,1,x-1);
			set_coord(3,0,y+1);
			set_coord(4,1,x-2);
			set_coord(4,0,y+1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=1;
		}else if(shape.convert_id==1){//convert to 0
			cop2last();
			x=get_last_coordx(1);
			y=get_last_coordy(1);
			if(tetris_map[y+1][x]==1 || tetris_map[y-1][x]==1 ||
				tetris_map[y][x-1]==1){
				break;
			}
			set_coord(1,0,y+1);
			set_coord(1,1,x);
			set_coord(2,1,x);
			set_coord(2,0,y);
			set_coord(3,1,x-1);
			set_coord(3,0,y);
			set_coord(4,1,x-1);
			set_coord(4,0,y-1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=0;
		}
		break;
		case 5:if(shape.convert_id==0){//convert to 1
			cop2last();
			x=get_last_coordx(1);
			y=get_last_coordy(1);
			if(tetris_map[y][x+1]==1 || tetris_map[y][x-1]==1 ||
				tetris_map[y-1][x]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x+1);
			set_coord(2,1,x);
			set_coord(2,0,y);
			set_coord(3,1,x);
			set_coord(3,0,y-1);
			set_coord(4,1,x-1);
			set_coord(4,0,y-1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=1;
		}else if(shape.convert_id==1){//convert to 0
			cop2last();
			x=get_last_coordx(2);
			y=get_last_coordy(2);
			if(tetris_map[y-2][x]==1 || tetris_map[y-1][x]==1 ||
				tetris_map[y][x+1]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x);
			set_coord(2,1,x);
			set_coord(2,0,y-1);
			set_coord(3,1,x+1);
			set_coord(3,0,y-1);
			set_coord(4,1,x+1);
			set_coord(4,0,y-2);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=0;
		}
		break;
		case 6:if(shape.convert_id==0){//convert to 1
			cop2last();
			x=get_last_coordx(3);
			y=get_last_coordy(3);
			if(tetris_map[y][x+1]==1 || tetris_map[y+1][x]==1 ||
				tetris_map[y-1][x]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x+1);
			set_coord(2,1,x);
			set_coord(2,0,y+1);
			set_coord(3,1,x);
			set_coord(3,0,y);
			set_coord(4,1,x);
			set_coord(4,0,y-1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=1;
		}else if(shape.convert_id==1){//convert to 2
			cop2last();
			x=get_last_coordx(3);
			y=get_last_coordy(3);
			if(tetris_map[y-1][x]==1 || tetris_map[y][x+1]==1 ||
				tetris_map[y][x-1]==1){
				break;
			}
			set_coord(1,0,y-1);
			set_coord(1,1,x);
			set_coord(2,1,x+1);
			set_coord(2,0,y);
			set_coord(3,1,x);
			set_coord(3,0,y);
			set_coord(4,1,x-1);
			set_coord(4,0,y);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=2;
		}else if(shape.convert_id==2){//convert to 3
			cop2last();
			x=get_last_coordx(3);
			y=get_last_coordy(3);
			if(tetris_map[y+1][x]==1 || tetris_map[y-1][x]==1 ||
				tetris_map[y][x-1]==1){
				break;
			}
			set_coord(1,0,y);
			set_coord(1,1,x-1);
			set_coord(2,1,x);
			set_coord(2,0,y-1);
			set_coord(3,1,x);
			set_coord(3,0,y);
			set_coord(4,1,x);
			set_coord(4,0,y+1);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=3;
		}else if(shape.convert_id==3){//convert to 0
			cop2last();
			x=get_last_coordx(3);
			y=get_last_coordy(3);
			if(tetris_map[y+1][x]==1 || tetris_map[y][x-1]==1 ||
				tetris_map[y][x+1]==1){
				break;
			}
			set_coord(1,0,y+1);
			set_coord(1,1,x);
			set_coord(2,1,x-1);
			set_coord(2,0,y);
			set_coord(3,1,x);
			set_coord(3,0,y);
			set_coord(4,1,x+1);
			set_coord(4,0,y);
			set_last_figure_pos();
			set_figure_positions();
			shape.convert_id=0;
		}
		break;

		default:
			break;
	}
}

int isDownPossible(){
	int cont=0;
	switch (shape.id) {
	case 0:
		if(shape.convert_id==0){
			if(get_coordy(3)<=0){
				cont=0;
			}else if(get_map_val(3)==1 || get_map_val(4)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}
	break;
	case 1:
		if(shape.convert_id==0){
			if(get_coordy(4)<=0){
				cont=0;
			}else if(get_map_val(4)==1){
			//}else if(tetris_map[get_coordy(4)-1][get_coordx(4)]==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==1){
			if(get_coordy(4)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(3)==1 ||
					get_map_val(2)==1 || get_map_val(1)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}
	break;
	case 2:
		if(shape.convert_id==0){
			if(get_coordy(4)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(3)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==1){
			if(get_coordy(4)<=0){
				cont=0;
			}else if(get_map_val(4)==1 ||
					get_map_val(2)==1 || get_map_val(1)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==2){
			if(get_coordy(1)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(1)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==3){
			if(get_coordy(1)<=0){
				cont=0;
			}else if(get_map_val(3)==1 || get_map_val(2)==1 || get_map_val(1)==1 ){
				cont=0;
			}else {
				cont = 1;
			}
		}
	break;
	case 3:
		if(shape.convert_id==0){
			if(get_coordy(4)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(3)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==1){
			if(get_coordy(3)<=0){
				cont=0;
			}else if(get_map_val(3)==1 ||
					get_map_val(2)==1 || get_map_val(1)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==2){
			if(get_coordy(1)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(1)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==3){
			if(get_coordy(1)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(2)==1 || get_map_val(1)==1 ){
				cont=0;
			}else {
				cont = 1;
			}
		}
	break;
	case 4:
		if(shape.convert_id==0){
			if(get_coordy(4)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(2)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==1){
			if(get_coordy(2)<=0){
				cont=0;
			}else if(get_map_val(4)==1 ||
					get_map_val(2)==1 || get_map_val(1)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}
	break;
	case 5:
		if(shape.convert_id==0){
			if(get_coordy(4)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(2)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==1){
			if(get_coordy(3)<=0){
				cont=0;
			}else if(get_map_val(3)==1 ||
					get_map_val(4)==1 || get_map_val(1)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}
	break;
	case 6:
		if(shape.convert_id==0){
			if(get_coordy(4)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(3)==1 || get_map_val(2)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==1){
			if(get_coordy(3)<=0){
				cont=0;
			}else if(get_map_val(4)==1 || get_map_val(1)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==2){
			if(get_coordy(1)<=0){
				cont=0;
			}else if(get_map_val(2)==1 ||get_map_val(1)==1 || get_map_val(4)==1){
				cont=0;
			}else {
				cont = 1;
			}
		}else if(shape.convert_id==3){
			if(get_coordy(2)<=0){
				cont=0;
			}else if(get_map_val(2)==1 || get_map_val(1)==1 ){
				cont=0;
			}else {
				cont = 1;
			}
		}
	break;
	default:cont = 1;break;
	}
	return cont;
}



