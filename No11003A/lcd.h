//
//  Windwos:RGB(r,g,b)= 0xbbggrr
//
//  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
// |©   BRUE   ¨|©    GREEN    ¨|©    RED   ¨|
//	R:0`255, G:0`255, B:0`255 ... ãˆÊ 5–”‚Í6bit g—p
#define RGB(R,G,B) ((((B) & 0xf8)<<8) | (((G) & 0xfc)<<3) | (((R) & 0xf8)>>3))
#define RGB2R(RGB) (((RGB)<<3) & 0xf8)
#define RGB2G(RGB) (((RGB)>>3) & 0xfc)
#define RGB2B(RGB) (((RGB)>>8) & 0xf8)

#define BLACK RGB(0,0,0)
#define WHITE RGB(255,255,255)
#define GRAY  RGB(192,192,192)
#define RED   RGB(255,0,0)        // ï¿½Eï¿½ï¿½Eï¿½ï¿½Eï¿½ï¿½Eï¿½ï¿½Eï¿½Ü‚ï¿½
#define GREEN RGB(0,255,0)
#define BRUE  RGB(0,0,255)
#define CYAN  RGB(0,255,255)
#define MAGENTA RGB(255,0,255)
#define YELLOW  RGB(255,255,0)
#define CYAN_D  RGB(0,128,128)
#define RED_D   RGB(128,0,0)
#define GREEN_D RGB(0,128,0)


#define BLACK1    0
#define WHITE1    0xff
#define RED1      0x18
#define BRUE1     0xe0
#define GREEN1    0x07
#define YELLOW1   0x1f
#define CYAN1     0xee

#define LCD_RST   58
#define LCD_CS    57
#define LCD_RS    56
#define LCD_WR    55
#define LCD_RD    54
#define LCD_D0    8
#define LCD_D1    9
#define LCD_D2    2
#define LCD_D3    3
#define LCD_D4    4
#define LCD_D5    5
#define LCD_D6    6
#define LCD_D7    7

#define ColumnAddressSet 0x2a
#define PageAddressSet   0x2b
#define MemoryRead       0x2e
#define MemoryWrite      0x2c

#define printf lcd_printf

extern int back_color;
extern int text_color;
extern int keisen_color;
extern int graph_color;
extern int lcd_x;                  // 0ï¿½Eï¿½Eï¿½Eï¿½`25
extern int lcd_y;                  // 0ï¿½Eï¿½Eï¿½Eï¿½`14
extern int lcd_noofline;            // ï¿½Eï¿½Eï¿½Eï¿½Pï¿½Eï¿½Eï¿½Eï¿½ï¿½Eï¿½Eï¿½Eï¿½Ê‚Ìsï¿½Eï¿½Eï¿½Eï¿½ï¿½Eï¿½Eï¿½Eï¿½
extern char lcd_text_buf[15][26];
extern const short font[256][16];

void lcd_scroll();
void lcd_nl();
int lcd_disp_char_x1(int x, int y, char c);
int lcd_disp_char_x2(int x, int y, char c);
void lcd_disp_str_x1(int x, int y, const char *str);
void lcd_disp_str_x2(int x, int y, const char *str);
void lcd_cdisp_str_x1(int y, const char *str);
void lcd_cdisp_str_x2(int y, const char *str);
int sjis_disp_char_x1(int x, int y, int c);
int sjis_disp_char_reverse_x1(int x, int y, int c);
void lcd_init();
void lcd_printf(const char *fmt, ...);
void lcd_puts(const char *str);
void lcd_putc(char c);
int lcd_clear(int color);
int lcd_clearA(int x1, int y1, int x2, int y2, int color);
void lcd_clear_square(int x, int y, int w, int h, int color);
int lcd_area_set(int x1, int y1, int x2, int y2);
int lcd_area_square(int x, int y, int w, int h);
void lcd_write_data(int data);
void lcd_write_cmd(int data);
void lcd_write_color(int color);
void lcd_clear1(char color);
void lcd_clear1_square(int x, int y, int w, int h, char color);
void lcd_clear_B();
void lcd_clear_W();
void lcd_read_mem(int x1, int y1, int x2, int y2, short *buf);
void lcd_read_square(int x, int y, int w, int h, short *buf);
void lcd_write_square(int x, int y, int w, int h, const short *pat);
int message(const char *fmt, ...);
void fatal(const char *msg1, const char *msg2, const char *msg3);
void circle1(int x0, int y0, int r, int color);
void circlefill(int x0, int y0, int r, int c);
