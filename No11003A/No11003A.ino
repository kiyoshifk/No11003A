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
#include "menu.h"


USBHost usb;
MouseController mouse(usb);


int work_buf[5000];


void pentomino(int fast);
void gomoku_main();
void othe_main();
void syogi_main();


const struct menu main_menu[]={
  {0,   0, 12, 4, "gomoku"},
  {0,  60, 12, 5, "othello"},
  {0, 120, 12, 6, "syogi"},
  {0, 180, 12, 7, ""},
  
  {157,  0, 12, 8, ""},
  {157, 60, 12, 1, "pentomino 1"},
  {157,120, 12, 2, "pentomino 2"},
  {157,180, 12, 3, "test"},
  {0,0,0,0,0},
};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
	lcd_init();
}

void loop() {
  int cmd, x, y;

    btn_back_color = GREEN;
    cmd = menu_select0(main_menu);
    switch(cmd){
      case 1:
        pentomino(1);
        break;
      case 2:
        pentomino(0);
        break;
      case 3:                 // test
        for(y=0; y<16; y++){
          for(x=0; x<16; x++){
            lcd_clear1_square(x*20, y*15, 20, 15, x+y*16);
          }
        }
        mouse_updown(&x, &y);
        break;
      case 4:				// ŒÜ–Ú•À‚×
		gomoku_main();
        break;
      case 5:				// ƒIƒZƒ
		othe_main();
        break;
      case 6:				// «Šû
		syogi_main();
        break;
      case 7:
        break;
      case 8:
        break;
    }
}

int MCrnd(int i)
{
  static int flag;

  if(flag==0){
    flag = 1;
    randomSeed(micros());
  }
  return random(i);
}
