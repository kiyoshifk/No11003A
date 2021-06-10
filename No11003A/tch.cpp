#include <Arduino.h>
#include <address.h>
#include <adk.h>
#include <confdescparser.h>
#include <hid.h>
#include <hidboot.h>
#include <hidusagestr.h>
#include <KeyboardController.h>
#include <MouseController.h>
#include <parsetools.h>
#include <Usb.h>
#include <usb_ch9.h>

#include "lcd.h"
#include "tch.h"

extern USBHost usb;
extern MouseController mouse;
int mouse_x = 160;
int mouse_y = 120;
int mouse_button;

/********************************************************************************/
/*		delay_us																*/
/********************************************************************************/
void delay_us(unsigned long us)
{
	unsigned long time1 = micros();

	while((micros() - time1) < us)
		;
}
/********************************************************************************/
/*    tch_drag_write                              */
/*    tch_drag_recover                            */
/*                                        */
/*    tch_drag_write: pat(w * h) を書き込む、画面は save に保存する     */
/*    toumei_color: pat の透明色指定                      */
/*    tch_drag_recover: save に保存していた画像を書き戻す           */
/********************************************************************************/
void tch_drag_write(int x, int y, int w, int h, const short *pat, int toumei_color, short *save)
{
  lcd_read_mem(x, y, x+w-1, y+h-1, save); // 画面を save に保存する
  
  int i, color;
  
  lcd_area_set(x, y, x+w-1, y+h-1);
  lcd_write_cmd(MemoryWrite);
  for(i=0; i<w*h; i++){
    color = pat[i];
    if(color==toumei_color)
      color = save[i];
    lcd_write_color(color);
    }
}

void tch_drag_recover(int x, int y, int w, int h, short *save)
{
  lcd_write_square(x, y, w, h, save);
}
/********************************************************************************/
/*    tch_int                                 */
/*    return 1: touch                             */
/*                                        */
/*    Y-:LCD_RS, Y+:LCD_D0, X-:LCD_CS, X+:LCD_D1                */
/********************************************************************************/
int tch_int()
{
  int i, tmp;
  
//  LCD_RS_IN;              // LCD_RS:input
//  LCD_D0_IN;              // LCD_D0:input
//  LCD_D0_PULLDOWN;          // LCD_D0:pulldown
//  LCD_CS_S;             // LCD_CS:High
//  LCD_OUTPUT(2);            // LCD_D1:High

	pinMode(LCD_RS, INPUT);
	pinMode(LCD_D0, INPUT_PULLUP);
	digitalWrite(LCD_CS, LOW);
	digitalWrite(LCD_D1, LOW);

	delay_us(10);
	tmp  = digitalRead(LCD_D0);
	delay_us(10);
	tmp |= digitalRead(LCD_D0);
	delay_us(10);
	tmp |= digitalRead(LCD_D0);
	delay_us(10);
	tmp |= digitalRead(LCD_D0);
  
//  LCD_D0_NONPULLDOWN;
//  LCD_DIR_OUT;
//  LCD_RS_OUT;

	pinMode(LCD_RS, OUTPUT);
	pinMode(LCD_D0, OUTPUT);
	digitalWrite(LCD_CS, HIGH);

    return !tmp;
}
/********************************************************************************/
/*    tch_read_x                                */
/*    Y-:LCD_RS, Y+:LCD_D0, X-:LCD_CS, X+:LCD_D1                */
/********************************************************************************/
int tch_read_x()
{
  int i, sum;
  
//  LCD_RS_IN;
//  LCD_RS_ANA;
//  LCD_D0_IN;
//  LCD_CS_C;             // X-
//  LCD_OUTPUT(0xff);         // X+

	pinMode(LCD_RS, INPUT);		// RS: analog input
	analogReference((eAnalogReference)DEFAULT);
	pinMode(LCD_D0, INPUT);
	digitalWrite(LCD_CS, LOW);
	digitalWrite(LCD_D1, HIGH);

//  PR3 = 40*5-1;           // 40MHz/40*5=200KHz
//    TMR3 = 0;
//    T3CON = 0;
//    T3CONbits.ON = 1;
//  AD1CHSbits.CH0SA = 21;        // positive input is AN21;
//  AD1CHSbits.CH0SB = 21;        // positive input is AN21;
//    AD1CON1bits.ON = 1;

  sum = 0;
  for(i=0; i<10; i++){
//    while(AD1CON1bits.DONE==0)
//      ;
//        AD1CON1bits.DONE = 0;
//    sum += ADC1BUF0;
		sum += analogRead(LCD_RS);
  }
//  LCD_DIR_OUT;
//  LCD_RS_DIGI;
//  LCD_RS_OUT;

	pinMode(LCD_RS, OUTPUT);
	pinMode(LCD_D0, OUTPUT);

  return sum / 10;
}
/********************************************************************************/
/*    tch_read_y                                */
/*    Y-:LCD_RS, Y+:LCD_D0, X-:LCD_CS, X+:LCD_D1                */
/********************************************************************************/
int tch_read_y()
{
  int i, sum;
  
//  LCD_CS_IN;
//  LCD_CS_ANA;
//  LCD_D1_IN;
//  LCD_RS_C;             // Y-
//  LCD_OUTPUT(0xff);         // Y+
//  PR3 = 40*5-1;           // 40MHz/40*5=200KHz
//    TMR3 = 0;
//    T3CON = 0;
//    T3CONbits.ON = 1;
//  AD1CHSbits.CH0SA = 20;        // positive input is AN20;
//  AD1CHSbits.CH0SB = 20;        // positive input is AN20;
//    AD1CON1bits.ON = 1;

	pinMode(LCD_CS, INPUT);		// CS: analog input
	analogReference((eAnalogReference)DEFAULT);
	pinMode(LCD_D1, INPUT);
	digitalWrite(LCD_RS, LOW);
	digitalWrite(LCD_D0, HIGH);

  sum = 0;
  for(i=0; i<10; i++){
//    while(AD1CON1bits.DONE==0)
//      ;
//        AD1CON1bits.DONE = 0;
//    sum += ADC1BUF0;
    sum += analogRead(LCD_CS);
  }
//  LCD_DIR_OUT;
//  LCD_CS_DIGI;
//  LCD_CS_OUT;

	pinMode(LCD_CS, OUTPUT);
	pinMode(LCD_D1, OUTPUT);

  return sum / 10;
}
/********************************************************************************/
/*    tch_read                                */
/*    *x=0～319, *y=0～239                          */
/*      return 1: success                                                       */
/********************************************************************************/
#define LEFTx  77
#define RIGHTx 860
#define UPx    120
#define DOWNx  890

int tch_read(int *x, int *y)
{
  int i, tmp, t;
  
    tmp = 0;
    for(i=0; i<100; i++){
        t = tch_read_x();
        if(t<=0 || t>=1023)
            return 0;                   // error
        tmp += t;
        if(tch_int()==0)
            return 0;                   // error
    }
  tmp /= 100;
  tmp = (tmp-LEFTx)*319/(RIGHTx-LEFTx);
  if(tmp < 0) tmp = 0;
  if(tmp > 319) tmp = 319;
  *x = tmp;
  
    tmp = 0;
    for(i=0; i<100; i++){
        t = tch_read_y();
        if(t<=0 || t>=1023)
            return 0;                   // error
        tmp += t;
        if(tch_int()==0)
            return 0;                   // error
    }
  tmp /= 100;
  tmp = (tmp-UPx)*239/(DOWNx-UPx);
  if(tmp < 0) tmp = 0;
  if(tmp > 239) tmp = 239;
  *y = tmp;
    return 1;                           // success
}
/********************************************************************************/
/*    tch_input                               */
/********************************************************************************/
//  入力されるまで待つ(ブロック)、50ms up が続いてから、入力待ち
//  flag==1 なら sw2 で break

#if 0
void tch_input(int *x, int *y)
{

	int i;
	
	mouse_cur_on();
	for(;;){
		for(i=0; i<10; i++){
			//	マウス処理
			SYS_Tasks();
			if(mouse_button & LEFT){
				*x = mouse_x;
				*y = mouse_y;
				mouse_cur_off();
				return;
			}
			//	タッチパネル処理
			if(tch_int()==0)
				i = 0;
			delay(1);
		}
		if(tch_read(x, y)){
			mouse_cur_off();
			return;
		}
	}
}
#endif

/********************************************************************************/
/*		mouse_read																*/
/********************************************************************************/
#if 0
int mouse_read()
{
	int sw;
	
	SYS_Tasks();
	
	sw = mouse_button;
	if(tch_int())
		sw |= TOUCH;
	return sw;
}

void mouse_up()
{
	unsigned int time1;
	
	//	sw up 確認 50ms
	time1 = GetTickCount();
	for(;;){
		if(GetTickCount()-time1 >= 50){
			return;
		}
		if(mouse_read()){				// up NG
			time1 = GetTickCount();
		}
	}
}
#endif

int mouse_updown(int *x, int *y)
{
//	int sw;
	unsigned int time1;
	
	mouse_cur_on();
//	//	sw up 確認 50ms
//	mouse_up();
	
//	//	sw down 確認 20ms
	mouse_button = 0;
//	sw = 0;
    for(;;){
		time1 = GetTickCount();
//		for(;;){
//			if(GetTickCount()-time1 >= 20){
//				break;
//			}
//			sw = mouse_read();
//			if(sw==0){							// down NG
//				time1 = GetTickCount();
//			}
//		}
		SYS_Tasks();
		if(mouse_button){		// mouse クリック
			*x = mouse_x;
			*y = mouse_y;
			mouse_cur_off();
			return mouse_button;
		}
		//		タッチパネルタッチ
		if(tch_read(x, y)){
			mouse_cur_off();
			return TOUCH;
		}
	}
}
/********************************************************************************/
/*    tch_line                                */
/********************************************************************************/
void tch_line(int x1, int y1, int x2, int y2, int color)
{
  int i, tmp, x_dist, y_dist;
  
  x_dist = abs(x1-x2);
  y_dist = abs(y1-y2);
  if(x_dist >= y_dist){
    if(x_dist==0)
      return;
    if(x1 <= x2){
      for(i=x1; i<=x2; i++){
        tmp = y1 + ((y2-y1)*(i-x1)+x_dist/2)/x_dist;
        lcd_clearA(i,tmp, i,tmp, color);
      }
    }
    else{
      for(i=x2; i<=x1; i++){
        tmp = y2 + ((y1-y2)*(i-x2)+x_dist/2)/x_dist;
        lcd_clearA(i,tmp, i,tmp, color);
      }
    }
  }
  else{
    if(y_dist==0)
      return;
    if(y1 <= y2){
      for(i=y1; i<=y2; i++){
        tmp = x1 + ((x2-x1)*(i-y1)+y_dist/2)/y_dist;
        lcd_clearA(tmp,i, tmp,i, color);
      }
    }
    else{
      for(i=y2; i<=y1; i++){
        tmp = x2 + ((x1-x2)*(i-y2)+y_dist/2)/y_dist;
        lcd_clearA(tmp,i, tmp,i, color);
      }
    }
  }
}




const char ms_cursor[9*6]={
1,1,0,0,0,0,
1,2,1,0,0,0,
1,2,2,1,0,0,
1,2,2,2,1,0,
1,2,2,2,1,1,
1,2,2,1,0,0,
1,2,1,2,1,0,
1,1,0,1,2,1,
1,0,0,0,1,0,
};

short ms_cur_buf[9*6];
int ms_cur_flag;

/********************************************************************************/
/*		ms_cur_draw																*/
/********************************************************************************/
void ms_cur_draw()
{
	int i;
	
	lcd_area_set(mouse_x-1, mouse_y-1, mouse_x+4, mouse_y+7);
	lcd_write_cmd(MemoryWrite);
	for(i=0; i<6*9; i++){
		if(ms_cursor[i]==0)
			lcd_write_color(ms_cur_buf[i]);
		else if(ms_cursor[i]==1)
			lcd_write_color(0);
		else
			lcd_write_color(0xffff);
	}
}
/********************************************************************************/
/*		mouse_cur_on																*/
/********************************************************************************/
void mouse_cur_on()
{
	if(ms_cur_flag){
//        ms_cur_draw();
        return;		 					// 既に ON している
    }
    ms_cur_flag = 1;
	lcd_read_square(mouse_x-1, mouse_y-1, 6, 9, ms_cur_buf);
	ms_cur_draw();
}
/********************************************************************************/
/*		mouse_cur_off																*/
/********************************************************************************/
void mouse_cur_off()
{
	if(ms_cur_flag==0)
		return;								// 既に OFF している
    ms_cur_flag = 0;
	lcd_write_square(mouse_x-1, mouse_y-1, 6, 9, ms_cur_buf);
}
/********************************************************************************/
/*		mouseMoved																*/
/*		システムから呼び出される関数											*/
/********************************************************************************/
void mouseMoved()
{
	static int x_sum, y_sum;
	int save = ms_cur_flag;

//  printf("=== mouseMoved\n");//AAAAA
	if(ms_cur_flag){					// カーソル表示中
		mouse_cur_off();
	}
	
	mouse_x += mouse.getXChange();
	mouse_y += mouse.getYChange();
	
	if(mouse_x < 1) mouse_x = 1;
	if(mouse_x > 315) mouse_x = 315;
	if(mouse_y < 1) mouse_y = 1;
	if(mouse_y > 232) mouse_y = 232;
	
	if(save){
		mouse_cur_on();
	}
}

void mouseDragged()
{
	static int x_sum, y_sum;
	int save = ms_cur_flag;

//  printf("=== mouseMoved\n");//AAAAA
	if(ms_cur_flag){					// カーソル表示中
		mouse_cur_off();
	}
	
	mouse_x += mouse.getXChange();
	mouse_y += mouse.getYChange();
	
	if(mouse_x < 1) mouse_x = 1;
	if(mouse_x > 315) mouse_x = 315;
	if(mouse_y < 1) mouse_y = 1;
	if(mouse_y > 232) mouse_y = 232;
	
	if(save){
		mouse_cur_on();
	}
}

void mousePressed()
{
	mouse_button = 0;
  if (mouse.getButton(LEFT_BUTTON)) {
	mouse_button |= LEFT;
  }
  if (mouse.getButton(MIDDLE_BUTTON)) {
	mouse_button |= MIDDLE;
  }
  if (mouse.getButton(RIGHT_BUTTON)) {
	mouse_button |= RIGHT;
  }
}

#if 0
void mouseReleased()
{
  if (!mouse.getButton(LEFT_BUTTON)) {
	mouse_button &= ~LEFT;
  }
  if (!mouse.getButton(MIDDLE_BUTTON)) {
	mouse_button &= ~MIDDLE;
  }
  if (!mouse.getButton(RIGHT_BUTTON)) {
	mouse_button &= ~RIGHT;
  }
}
#endif

/********************************************************************************/
/*		SYS_Tasks																*/
/********************************************************************************/
void SYS_Tasks()
{
	static unsigned int time1;
	
	if(millis()-time1 < 10)
		return;
	time1 = millis();
	usb.Task();
}
/********************************************************************************/
/*		dbg																		*/
/********************************************************************************/
void dbg(const char *msg)
{
	lcd_x = lcd_y = 0;
	printf("%s   ", msg);
}
