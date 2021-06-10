#define LEFT	0x01
#define MIDDLE	0x02
#define RIGHT	0x04
#define TOUCH	0x08


#define GetTickCount	millis
#define wait_ms			delay


extern int mouse_x;
extern int mouse_y;
//extern int mouse_press_flag;
extern int mouse_button;


void tch_drag_write(int x, int y, int w, int h, const short *pat, int toumei_color, short *save);
void tch_drag_recover(int x, int y, int w, int h, short *save);
int tch_int();
void tch_line(int x1, int y1, int x2, int y2, int color);
//void tch_input(int *x, int *y);
void delay_us(unsigned long us);
void mouse_cur_on();
void mouse_cur_off();
void SYS_Tasks();
int mouse_read();
void mouse_up();
int mouse_updown(int *x, int *y);
int MCrnd(int i);
