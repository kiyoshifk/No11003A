#include <Arduino.h>
#include "lcd.h"
#include "tch.h"
#include "menu.h"


#define SHIFT1 0x100
#define SHIFT2 0x101
#define SHIFT3 0x102
#define SHIFT4 0x103
#define KANA1  0x110
#define KANA2  0x111

const struct menu menu_key_4[]={
  {5,140,21-4,KANA2,"A"},
  {26,140,21-4,0,""},
  {47,140,21-4,0,""},
  {68,140,21-4,'ｧ',"ｧ"},
  {89,140,21-4,'ｩ',"ｩ"},
  {110,140,21-4,'ｪ',"ｪ"},
  {131,140,21-4,'ｫ',"ｫ"},
  {152,140,21-4,'ｬ',"ｬ"},
  {173,140,21-4,'ｭ',"ｭ"},
  {194,140,21-4,'ｮ',"ｮ"},
  {215,140,21-4,'ｦ',"ｦ"},
  {236,140,21-4,0,""},
  {257,140,21-4,0,""},
  {278,140,21-4,0,""},
  {299,140,21-4,'\b',""},
  
  {5,160,31-4,'\t',"Tb"},
  {36,160,21-4,0,""},
  {57,160,21-4,0,""},
  {78,160,21-4,'ｨ',"ｨ"},
  {99,160,21-4,0,""},
  {120,160,21-4,0,""},
  {141,160,21-4,0,""},
  {162,160,21-4,0,""},
  {183,160,21-4,0,""},
  {204,160,21-4,0,""},
  {225,160,21-4,0,""},
  {246,160,21-4,0,""},
  {267,160,21-4,'｢',"｢"},
  {288,160,32-4,'\n',"Et"},
  
  {5,180,37-4,0,""},
  {42,180,21-4,0,""},
  {63,180,21-4,0,""},
  {84,180,21-4,0,""},
  {105,180,21-4,0,""},
  {126,180,21-4,0,""},
  {147,180,21-4,0,""},
  {168,180,21-4,0,""},
  {189,180,21-4,0,""},
  {210,180,21-4,0,""},
  {231,180,21-4,0,""},
  {252,180,21-4,0,""},
  {273,180,21-4,'｣',"｣"},
  {294,180,26-4,0,""},
  
  {5,200,44-4,SHIFT4,"SFT"},
  {49,200,21-4,'ｯ',"ｯ"},
  {70,200,21-4,0,""},
  {91,200,21-4,0,""},
  {112,200,21-4,0,""},
  {133,200,21-4,0,""},
  {154,200,21-4,0,""},
  {175,200,21-4,0,""},
  {196,200,21-4,'､',"､"},
  {217,200,21-4,'｡',"｡"},
  {238,200,21-4,'･',"･"},
  {259,200,21-4,0,""},
  {280,200,40-4,SHIFT4,"SFT"},
  
  {5,  220,28-4,0,""},
  {100,220,120-4,' ',"Space"},
  {292,220,28-4,0,""},
  
  {0,0,0,0,0},
};

const struct menu menu_key_3[]={
  {5,140,21-4,KANA2,"A"},
  {26,140,21-4,'ﾇ',"ﾇ"},
  {47,140,21-4,'ﾌ',"ﾌ"},
  {68,140,21-4,'ｱ',"ｱ"},
  {89,140,21-4,'ｳ',"ｳ"},
  {110,140,21-4,'ｴ',"ｴ"},
  {131,140,21-4,'ｵ',"ｵ"},
  {152,140,21-4,'ﾔ',"ﾔ"},
  {173,140,21-4,'ﾕ',"ﾕ"},
  {194,140,21-4,'ﾖ',"ﾖ"},
  {215,140,21-4,'ﾜ',"ﾜ"},
  {236,140,21-4,'ﾎ',"ﾎ"},
  {257,140,21-4,'ﾍ',"ﾍ"},
  {278,140,21-4,'ｰ',"ｰ"},
  {299,140,21-4,'\b',""},
  
  {5,160,31-4,'\t',"Tb"},
  {36,160,21-4,'ﾀ',"ﾀ"},
  {57,160,21-4,'ﾃ',"ﾃ"},
  {78,160,21-4,'ｲ',"ｲ"},
  {99,160,21-4,'ｽ',"ｽ"},
  {120,160,21-4,'ｶ',"ｶ"},
  {141,160,21-4,'ﾝ',"ﾝ"},
  {162,160,21-4,'ﾅ',"ﾅ"},
  {183,160,21-4,'ﾆ',"ﾆ"},
  {204,160,21-4,'ﾗ',"ﾗ"},
  {225,160,21-4,'ｾ',"ｾ"},
  {246,160,21-4,'ﾞ',"ﾞ"},
  {267,160,21-4,'ﾟ',"ﾟ"},
  {288,160,32-4,'\n',"Et"},
  
  {5,180,37-4,0,""},
  {42,180,21-4,'ﾁ',"ﾁ"},
  {63,180,21-4,'ﾄ',"ﾄ"},
  {84,180,21-4,'ｼ',"ｼ"},
  {105,180,21-4,'ﾊ',"ﾊ"},
  {126,180,21-4,'ｷ',"ｷ"},
  {147,180,21-4,'ｸ',"ｸ"},
  {168,180,21-4,'ﾏ',"ﾏ"},
  {189,180,21-4,'ﾉ',"ﾉ"},
  {210,180,21-4,'ﾘ',"ﾘ"},
  {231,180,21-4,'ﾚ',"ﾚ"},
  {252,180,21-4,'ｹ',"ｹ"},
  {273,180,21-4,'ﾑ',"ﾑ"},
  {294,180,26-4,0,""},
  
  {5,200,44-4,SHIFT3,"SFT"},
  {49,200,21-4,'ﾂ',"ﾂ"},
  {70,200,21-4,'ｻ',"ｻ"},
  {91,200,21-4,'ｿ',"ｿ"},
  {112,200,21-4,'ﾋ',"ﾋ"},
  {133,200,21-4,'ｺ',"ｺ"},
  {154,200,21-4,'ﾐ',"ﾐ"},
  {175,200,21-4,'ﾓ',"ﾓ"},
  {196,200,21-4,'ﾈ',"ﾈ"},
  {217,200,21-4,'ﾙ',"ﾙ"},
  {238,200,21-4,'ﾒ',"ﾒ"},
  {259,200,21-4,'ﾛ',"ﾛ"},
  {280,200,40-4,SHIFT3,"SFT"},
  
  {5,  220,28-4,0,""},
  {100,220,120-4,' ',"Space"},
  {292,220,28-4,0,""},
  
  {0,0,0,0,0},
};

const struct menu menu_key_2[]={
  {5,140,21-4,KANA1,"ｶ"},
  {26,140,21-4,'!',"!"},
  {47,140,21-4,'\"',"\""},
  {68,140,21-4,'#',"#"},
  {89,140,21-4,'$',"$"},
  {110,140,21-4,'%',"%"},
  {131,140,21-4,'&',"&"},
  {152,140,21-4,'\'',"\'"},
  {173,140,21-4,'(',"("},
  {194,140,21-4,')',")"},
  {215,140,21-4,0,""},
  {236,140,21-4,'=',"="},
  {257,140,21-4,'~',"~"},
  {278,140,21-4,'|',"|"},
  {299,140,21-4,'\b',""},
  
  {5,160,31-4,'\t',"Tb"},
  {36,160,21-4,'Q',"Q"},
  {57,160,21-4,'W',"W"},
  {78,160,21-4,'E',"E"},
  {99,160,21-4,'R',"R"},
  {120,160,21-4,'T',"T"},
  {141,160,21-4,'Y',"Y"},
  {162,160,21-4,'U',"U"},
  {183,160,21-4,'I',"I"},
  {204,160,21-4,'O',"O"},
  {225,160,21-4,'P',"P"},
  {246,160,21-4,'`',"`"},
  {267,160,21-4,'{',"{"},
  {288,160,32-4,'\n',"Et"},
  
  {5,180,37-4,0,""},
  {42,180,21-4,'A',"A"},
  {63,180,21-4,'S',"S"},
  {84,180,21-4,'D',"D"},
  {105,180,21-4,'F',"F"},
  {126,180,21-4,'G',"G"},
  {147,180,21-4,'H',"H"},
  {168,180,21-4,'J',"J"},
  {189,180,21-4,'K',"K"},
  {210,180,21-4,'L',"L"},
  {231,180,21-4,'+',"+"},
  {252,180,21-4,'*',"*"},
  {273,180,21-4,'}',"}"},
  {294,180,26-4,0,""},
  
  {5,200,44-4,SHIFT2,"SFT"},
  {49,200,21-4,'Z',"Z"},
  {70,200,21-4,'X',"X"},
  {91,200,21-4,'C',"C"},
  {112,200,21-4,'V',"V"},
  {133,200,21-4,'B',"B"},
  {154,200,21-4,'N',"N"},
  {175,200,21-4,'M',"M"},
  {196,200,21-4,'<',"<"},
  {217,200,21-4,'>',">"},
  {238,200,21-4,'?',"?"},
  {259,200,21-4,'_',"_"},
  {280,200,40-4,SHIFT2,"SFT"},
  
  {5,  220,28-4,0,""},
  {100,220,120-4,' ',"Space"},
  {292,220,28-4,0,""},
  
  {0,0,0,0,0},
};

const struct menu menu_key_1[]={
  {5,140,21-4,KANA1,"ｶ"},
  {26,140,21-4,'1',"1"},
  {47,140,21-4,'2',"2"},
  {68,140,21-4,'3',"3"},
  {89,140,21-4,'4',"4"},
  {110,140,21-4,'5',"5"},
  {131,140,21-4,'6',"6"},
  {152,140,21-4,'7',"7"},
  {173,140,21-4,'8',"8"},
  {194,140,21-4,'9',"9"},
  {215,140,21-4,'0',"0"},
  {236,140,21-4,'-',"-"},
  {257,140,21-4,'^',"^"},
  {278,140,21-4,'\\',"\\"},
  {299,140,21-4,'\b',""},
  
  {5,160,31-4,'\t',"Tb"},
  {36,160,21-4,'q',"q"},
  {57,160,21-4,'w',"w"},
  {78,160,21-4,'e',"e"},
  {99,160,21-4,'r',"r"},
  {120,160,21-4,'t',"t"},
  {141,160,21-4,'y',"y"},
  {162,160,21-4,'u',"u"},
  {183,160,21-4,'i',"i"},
  {204,160,21-4,'o',"o"},
  {225,160,21-4,'p',"p"},
  {246,160,21-4,'@',"@"},
  {267,160,21-4,'[',"["},
  {288,160,32-4,'\n',"Et"},
  
  {5,180,37-4,0,""},
  {42,180,21-4,'a',"a"},
  {63,180,21-4,'s',"s"},
  {84,180,21-4,'d',"d"},
  {105,180,21-4,'f',"f"},
  {126,180,21-4,'g',"g"},
  {147,180,21-4,'h',"h"},
  {168,180,21-4,'j',"j"},
  {189,180,21-4,'k',"k"},
  {210,180,21-4,'l',"l"},
  {231,180,21-4,';',";"},
  {252,180,21-4,':',":"},
  {273,180,21-4,']',"]"},
  {294,180,26-4,0,""},
  
  {5,200,44-4,SHIFT1,"SFT"},
  {49,200,21-4,'z',"z"},
  {70,200,21-4,'x',"x"},
  {91,200,21-4,'c',"c"},
  {112,200,21-4,'v',"v"},
  {133,200,21-4,'b',"b"},
  {154,200,21-4,'n',"n"},
  {175,200,21-4,'m',"m"},
  {196,200,21-4,',',","},
  {217,200,21-4,'.',"."},
  {238,200,21-4,'/',"/"},
  {259,200,21-4,'\\',"\\"},
  {280,200,40-4,SHIFT1,"SFT"},
  
  {5,  220,28-4,0,""},
  {100,220,120-4,' ',"Space"},
  {292,220,28-4,0,""},
  
  {0,0,0,0,0},
};

int btn_back_color = GREEN;

/********************************************************************************/
/*    keyin                                 */
/*    enter で return する                         */
/********************************************************************************/
void keyin(char *buf, int maxlen)
{
  int c, pt, plane;
  
  pt = 0;
  buf[0] = '\n';
  plane = 1;
  for(;;){
    if(pt>=maxlen-1)
      return;
    if(plane==1)
      c = menu_key(menu_key_1);
    else if(plane==2)
      c = menu_key(menu_key_2);
    else if(plane==3)
      c = menu_key(menu_key_3);
    else if(plane==4)
      c = menu_key(menu_key_4);
    else
      c = menu_key(menu_key_1);
    if(c==SHIFT1){
      plane = 2;
    }
    else if(c==SHIFT2){
      plane = 1;
    }
    else if(c==SHIFT3){
      plane = 4;
    }
    else if(c==SHIFT4){
      plane = 3;
    }
    else if(c==KANA1){
      plane = 3;
    }
    else if(c==KANA2){
      plane = 1;
    }
    else if(c<0x80){        // 文字
      if(c=='\n'){
//        buf[pt++] = c;
//        buf[pt] = '\0';
        return;
      }
      else if(c=='\b'){     // backspace
        if(pt){
          --pt;
          if(lcd_x){
            --lcd_x;
            lcd_putc(' ');  // 消去
            --lcd_x;
          }
          else{
            if(lcd_y){
              --lcd_y;
              lcd_x = 25;
              lcd_putc(' ');  // 消去
              lcd_x = 25;
            }
          }
        }
      }
      else{
        lcd_putc(c);
        buf[pt++] = c;
        buf[pt] = '\0';
      }
    }
  }
}
/********************************************************************************/
/*    menu_pat_select                             */
/********************************************************************************/
void menu_pat_disp(const struct menu_pat *tbl)
{
  int i;

  lcd_clear_B();
  for(i=0; tbl[i].w; i++){
    if(tbl[i].pat==0)
      continue;
    lcd_write_square(tbl[i].x, tbl[i].y, tbl[i].w, tbl[i].h, tbl[i].pat);
  }
}

int menu_pat_select(const struct menu_pat *tbl)
{
  int i, x, y, x1, y1, x2, y2;
  
  /***  ボタン表示 ***/
  menu_pat_disp(tbl);
  
  /***  タッチ入力 ***/
  for(;;){
    mouse_updown(&x, &y);
    for(i=0; tbl[i].w; i++){
      x1 = tbl[i].x;
      y1 = tbl[i].y;
      x2 = x1 + tbl[i].w -1;
      y2 = y1 + tbl[i].h -1;
      if(x>=x1 && x<=x2 && y>=y1 && y<=y2){
        lcd_clearA(x1,y1,x2,y2, GREEN);
        delay(100);
//        if(tbl[i].cmd==0)   // cmd==0 は入力不可
//          break;
        return tbl[i].cmd;
      }
    }
    menu_pat_disp(tbl);
  }
}
/********************************************************************************/
/*    menu_key                                */
/*    key は文字列の表示(12x16)のみ、back_color text_color を使用する      */
/*    n はボタンの幅（ドット）                       */
/*    cmd==0 は表示のみで入力不可                       */
/********************************************************************************/
void menu_key_disp(const struct menu *tbl)
{
  int i, x1, y1, x2, y2, n;
  
  lcd_clear_B();
  /***  ボタン表示 ***/
  for(i=0; tbl[i].msg; i++){
    n  = tbl[i].n;
    x1 = tbl[i].x;
    y1 = tbl[i].y;
    x2 = x1 + n - 1;
    y2 = y1 + 16 -1;
    lcd_clearA(x1,y1,x2,y2, back_color);
    lcd_disp_str_x1(x1+(n-strlen(tbl[i].msg)*12)/2, tbl[i].y, tbl[i].msg);
  }
}

int menu_key(const struct menu *tbl)
{
  int i, x, y, x1, y1, x2, y2, n;
  
  /***  ボタン表示 ***/
  menu_key_disp(tbl);
  
  /***  タッチパネル入力  ***/
  for(;;){
    mouse_updown(&x, &y);
    for(i=0; tbl[i].msg; i++){
      n  = tbl[i].n;
      x1 = tbl[i].x;
      y1 = tbl[i].y;
      x2 = x1 + n - 1;
      y2 = y1 + 16 -1;
      if(x>=x1 && x<=x2 && y>=y1 && y<=y2){
        lcd_clearA(x1, y1, x2, y2, BRUE);
        delay(100);
//        if(tbl[i].cmd==0)   // cmd==0 は入力不可
//          break;
        return tbl[i].cmd;
      }
    }
    menu_key_disp(tbl);
  }
}
/********************************************************************************/
/*    menu_select0                              */
/*    ボタン高さ 60ドット、上下に 4ドットの縁、縁の下 18 ドット(btn_back_color)*/
/*    ボタン幅 = (n+1)*12 ドット ... n:最大文字数、左右に 4ドットの縁      */
/*    文字 m*12 ドット幅 ... m:表示文字数、16 ドット高            */
/********************************************************************************/
void menu_disp0(const struct menu *tbl)
{
  int i, j, k, x, y;
  
  lcd_clear_B();
  /***  ボタン表示 ***/
  for(i=0; tbl[i].msg; i++){
    lcd_clearA(tbl[i].x+4, tbl[i].y+4, tbl[i].x+(tbl[i].n+1)*12-4-1, tbl[i].y+60-4-1, btn_back_color);
    j = strlen(tbl[i].msg);
    if(j > tbl[i].n)
      j = tbl[i].n;
    x = tbl[i].x + ((tbl[i].n+1)*12 - j*12)/2;
    y = tbl[i].y + 4 + 18;
    for(k=0; k<j; k++)
      lcd_disp_char_x1(x+k*12, y, tbl[i].msg[k]);
  }
}

int menu_select0(const struct menu *tbl)
{
  int i, x, y, x1, y1, x2, y2;
  
  /***  ボタン表示 ***/
  menu_disp0(tbl);

  /***  タッチパネル入力  ***/
  for(;;){
    mouse_updown(&x, &y);
    for(i=0; tbl[i].msg; i++){
//      lcd_clearA(tbl[i].x+4, tbl[i].y+4, tbl[i].x+(tbl[i].n+1)*12-4-1, tbl[i].y+60-4-1, btn_back_color);
      x1 = tbl[i].x+4;
      y1 = tbl[i].y+4;
      x2 = tbl[i].x+(tbl[i].n+1)*12-4-1;
      y2 = tbl[i].y+60-4-1;
      if(x>=x1 && x<=x2 && y>=y1 && y<=y2){
        lcd_clearA(x1,y1,x2,y2, BRUE);
        delay(100);
//        if(tbl[i].cmd==0)   // cmd==0 は入力不可
//          break;
        return tbl[i].cmd;
      }
    }
    menu_disp0(tbl);
  }
}

/********************************************************************************/
/*    menu_select                               */
/*    ボタン高さ 40ドット、上下に 4ドットの縁、縁の下 8 ドット(btn_back_color)*/
/*    ボタン幅 = (n+1)*12 ドット ... n:最大文字数、左右に 4ドットの縁      */
/*    文字 m*12 ドット幅 ... m:表示文字数、16 ドット高            */
/********************************************************************************/
void menu_disp(const struct menu *tbl)
{
  int i, j, k, x, y;
  
  lcd_clear_B();
  /***  ボタン表示 ***/
  for(i=0; tbl[i].msg; i++){
    lcd_clearA(tbl[i].x+4, tbl[i].y+4, tbl[i].x+(tbl[i].n+1)*12-4-1, tbl[i].y+40-4-1, btn_back_color);
    j = strlen(tbl[i].msg);
    if(j > tbl[i].n)
      j = tbl[i].n;
    x = tbl[i].x + ((tbl[i].n+1)*12 - j*12)/2;
    y = tbl[i].y + 4 + 8;
    for(k=0; k<j; k++)
      lcd_disp_char_x1(x+k*12, y, tbl[i].msg[k]);
  }
}

int menu_select(const struct menu *tbl)
{
  int i, x, y, x1, y1, x2, y2;
  
  /***  ボタン表示 ***/
  menu_disp(tbl);

  /***  タッチパネル入力  ***/
  for(;;){
    mouse_updown(&x, &y);
    for(i=0; tbl[i].msg; i++){
//      lcd_clearA(tbl[i].x+4, tbl[i].y+4, tbl[i].x+(tbl[i].n+1)*12-4-1, tbl[i].y+40-4-1, btn_back_color);
      x1 = tbl[i].x+4;
      y1 = tbl[i].y+4;
      x2 = tbl[i].x+(tbl[i].n+1)*12-4-1;
      y2 = tbl[i].y+40-4-1;
      if(x>=x1 && x<=x2 && y>=y1 && y<=y2){
        lcd_clearA(x1,y1,x2,y2, BRUE);
        delay(100);
//        if(tbl[i].cmd==0)   // cmd==0 は入力不可
//          break;
        return tbl[i].cmd;
      }
    }
    menu_disp(tbl);
  }
}
/********************************************************************************/
/*    menu_select2                              */
/*    ボタン高さ 26ドット、上下に 3ドットの縁、縁の下 2 ドット(btn_back_color)*/
/*    ボタン幅 = (n+1)*12 ドット ... n:最大文字数、左右に 4ドットの縁      */
/*    文字 m*12 ドット幅 ... m:表示文字数、16 ドット高            */
/********************************************************************************/
void menu_disp2(const struct menu *tbl)
{
  int i, j, k, x, y;
  
  lcd_clear_B();
  /***  ボタン表示 ***/
  for(i=0; tbl[i].msg; i++){
    lcd_clearA(tbl[i].x+4, tbl[i].y+3, tbl[i].x+(tbl[i].n+1)*12-4-1, tbl[i].y+26-3-1, btn_back_color);
    j = strlen(tbl[i].msg);
    if(j > tbl[i].n)
      j = tbl[i].n;
    x = tbl[i].x + ((tbl[i].n+1)*12 - j*12)/2;
    y = tbl[i].y + 3 + 2;
    for(k=0; k<j; k++)
      lcd_disp_char_x1(x+k*12, y, tbl[i].msg[k]);
  }
}

int menu_select2(const struct menu *tbl)
{
  int i, x, y, x1, y1, x2, y2;
  
  /***  ボタン表示 ***/
  menu_disp2(tbl);
  
  /***  タッチパネル入力  ***/
  for(;;){
    mouse_updown(&x, &y);
    for(i=0; tbl[i].msg; i++){
//      lcd_clearA(tbl[i].x+4, tbl[i].y+3, tbl[i].x+(tbl[i].n+1)*12-4-1, tbl[i].y+26-3-1, btn_back_color);
      x1 = tbl[i].x+4;
      y1 = tbl[i].y+3;
      x2 = tbl[i].x+(tbl[i].n+1)*12-4-1;
      y2 = tbl[i].y+26-3-1;
      if(x>=x1 && x<=x2 && y>=y1 && y<=y2){
        lcd_clearA(x1,y1,x2,y2, BRUE);
        delay(100);
//        if(tbl[i].cmd==0)   // cmd==0 は入力不可
//          break;
        return tbl[i].cmd;
      }
    }
    menu_disp2(tbl);
  }
}
/********************************************************************************/
/*    menu_select3                              */
/*    ボタン高さ 20ドット、上下に 1ドットの縁、縁の下 1 ドット(btn_back_color)*/
/*    ボタン幅 = (n+1)*12 ドット ... n:最大文字数、左右に 4ドットの縁      */
/*    文字 m*12 ドット幅 ... m:表示文字数、16 ドット高            */
/********************************************************************************/
void menu_disp3(const struct menu *tbl)
{
  int i, j, k, x, y;
  
  lcd_clear_B();
  /***  ボタン表示 ***/
  for(i=0; tbl[i].msg; i++){
    lcd_clearA(tbl[i].x+4, tbl[i].y+1, tbl[i].x+(tbl[i].n+1)*12-4-1, tbl[i].y+20-1-1, btn_back_color);
    j = strlen(tbl[i].msg);
    if(j > tbl[i].n)
      j = tbl[i].n;
    x = tbl[i].x + ((tbl[i].n+1)*12 - j*12)/2;
    y = tbl[i].y + 1 + 1;
    for(k=0; k<j; k++)
      lcd_disp_char_x1(x+k*12, y, tbl[i].msg[k]);
  }
}

int menu_select3(const struct menu *tbl)
{
  int i, x, y, x1, y1, x2, y2;
  
  /***  ボタン表示 ***/
  menu_disp3(tbl);
  
  /***  タッチパネル入力  ***/
  for(;;){
    mouse_updown(&x, &y);
    for(i=0; tbl[i].msg; i++){
//      lcd_clearA(tbl[i].x+4, tbl[i].y+1, tbl[i].x+(tbl[i].n+1)*12-4-1, tbl[i].y+20-1-1, btn_back_color);
      x1 = tbl[i].x+4;
      y1 = tbl[i].y+1;
      x2 = tbl[i].x+(tbl[i].n+1)*12-4-1;
      y2 = tbl[i].y+20-1-1;
      if(x>=x1 && x<=x2 && y>=y1 && y<=y2){
        lcd_clearA(x1,y1,x2,y2, BRUE);
        delay(100);
//        if(tbl[i].cmd==0)   // cmd==0 は入力不可
//          break;
        return tbl[i].cmd;
      }
    }
    menu_disp3(tbl);
  }
}
