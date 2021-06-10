/*
* 横２ｘ縦４　の例
* 
*const struct menu main_menu[]={
*  {0,   0, 12, 1, "test"},
*  {0,  60, 12, 2, "draw"},
*  {0, 120, 12, 3, "f_mgr"},
*  {0, 180, 12, 4, "Game"},
*  
*  {157,  0, 12, 5, "Calc"},
*  {157, 60, 12, 6, "Oscillo"},
*  {157,120, 12, 7, "Spe_Ana"},
*  {157,180, 12, 8, "F.Resp."},
*  {0,0,0,0,0},
*};
*  
*    btn_back_color = GREEN;
*    cmd = menu_select0(main_menu); // 縦４のルーチン
*/

//
//  menu_select()
//    ボタンを表示しタッチしたら cmd 番号を返す
//    ボタンの大きさは縦40ドット、横(n+1)x12ドットの縁付きの文字
//    ボタンの色は btn_back_color で指定する
//    ボタンの位置は左上の座標で表す
//    ボタンを表示する時背景には手を加えない

struct menu{
  int x;
  int y;
  int n;
  int cmd;
  char *msg;
};

struct menu_pat{
  int x;
  int y;
  int w;                // w が 0 ならストッパー
  int h;
  int cmd;
  short *pat;             // pat==0 なら描画しない
};

extern int btn_back_color;


void keyin(char *buf, int maxlen);
void menu_pat_disp(const struct menu_pat *tbl);
int menu_pat_select(const struct menu_pat *tbl);
void menu_key_disp(const struct menu *tbl);
int menu_key(const struct menu *tbl);
void menu_disp0(const struct menu *tbl);
int menu_select0(const struct menu *tbl);
void menu_disp(const struct menu *tbl);
int menu_select(const struct menu *tbl);
void menu_disp2(const struct menu *tbl);
int menu_select2(const struct menu *tbl);
void menu_disp3(const struct menu *tbl);
int menu_select3(const struct menu *tbl);
