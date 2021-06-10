#include <arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"
#include "menu.h"
#include "tch.h"
#include "syogi.h"


extern int work_buf[5000];


struct sv{
	int moves[1000];
	unsigned long long hash[160];
	unsigned long long code[100];
	int buf[3480];
};


static const int tbl1[]={1,9,2,10,3,11,4,12,5,6,13,7,14,8,};	// koma to code
static const int tbl2[]={1,3,5,7,9,10,12,14,2,4,6,8,11,13,};	// code to koma


static struct sv *sv=(struct sv*)work_buf;
static int MCyomucnt;
static int yomiend;
static const int syo_ban_color=WHITE;				// 駒と盤の背景色
static int qgoban[160];	/* メインの碁盤		*/
static int teban;
static int tekazu;
static int white, black;					// 0:cpu, 1:human
static int start_teban;

static void syo_disp_waku();
static void syo_disp_koma(int x, int y, int code);	// 先手番駒表示
static void syo_disp_rev_koma(int x, int y, int code);	// 後手番駒表示
//static void cpyban1(int ban1[], const int ban2[]);
static int testmoveok(int *ban, int k, int move);
static int naricheck(int *ban, int k, int move);
static int yomiutu(int *ban, int k, int move);
static void makeallmoves(int *ban, int k, int *moves, int *movecnt);
static int MCyomu(int *ban, int k, int *move);
static int MCyomuk(int *ban, int k, int *move, int depth, int alpha, int beta, int buf_ptr);
//static int MCyomus(int *ban, int *move, int depth, int alpha, int beta);


/********************************************************************************/
/*		testmoveok																*/
/*		return 0:OK, error code													*/
/********************************************************************************/
static int testmoveoksub(int *ban, int k, int mv1, int mv2, int dir)
{
	int i;
	
	for(i=1; ; i++){
		if((mv1+dir*i)==mv2)
			return 1;
		if((ban[mv1+dir*i] & 0xf00)==ISHI_O || (ban[mv1+dir*i] & 0xf00)==k)
			return 0;
	}
}


static int testmoveok(int *ban, int k, int move)
{
	int mv1, mv2, i, km, x;
	const int *pti;
	
	mv1 = move & 0xff;					/* 移動元								*/
	mv2 = (move & 0xff00)>>8;			/* 移動先								*/
	if(mv2<21 || mv2>=110)				/* 移動先は盤面であること				*/
		return 2;
	if(mv1 >= 131){						/* 駒台から								*/
		if(mv1 >= 160)
			return 3;
		if(ban[mv2])
			return 4;
		if(ban[mv1] <= 0)				/* 駒台の駒数							*/
			return 5;
		if(move & 0x10000)				/* 成フラグ								*/
			return 6;
		if(k==ISHI_K){
			if(mv1-131 > 14)			/* 駒台の駒種類が上手ならNG				*/
				return 7;
		}
		else{
			if(mv1-131 <= 14)			/* 駒台の駒種類が下手ならNG				*/
				return 8;
		}
		km = mv1-131;
		if(km==1 || km==15){			/* 歩打ちなら２歩のチェックを行う		*/
			x = macrox(mv2);
			for(i=1; i<=9; i++){
				if((ban[macropos(x,i)] & 0xff)==km)
					return 9;			/* ２歩									*/
			}
		}
		return 0;						/* テストOK								*/
	}
	else{
		if(mv1<21 || mv1>=110)			/* 移動元は盤面であること				*/
			return 10;
		if((ban[mv1] & 0xf00) != k)		/* 移動元の駒は手番であること			*/
			return 11;
		if((ban[mv2] & 0xf00)==ISHI_O || (ban[mv2] & 0xf00)==k)	/* 移動先が番外や自駒でないこと	*/
			return 12;
		/*** 周辺の移動かどうかのチェック	***/
		pti = dir10[ban[mv1] & 0xff];	/* 移動元の駒種類						*/
		for(i=0; i<9; i++){
			if(pti[i]==0)				/* table end							*/
				break;
			if((mv1+pti[i])==mv2)		/* 元に移動量を加算したら移動先になること	*/
				return 0;				/* テストOK								*/
		}
		/*** 特殊駒の長距離移動テスト	***/
		switch(ban[mv1] & 0xff){		/* 移動元の駒種類						*/
		case 3:							/* 下手香								*/
			if(testmoveoksub(ban, k, mv1, mv2, -10))
				return 0;
			break;
		case 10:						/* 下手角/馬							*/
		case 11:
		case 24:						/* 上手角/馬							*/
		case 25:
			for(i=0; i<4; i++){
				if(testmoveoksub(ban, k, mv1, mv2, dir2[i]))
					return 0;
			}
			break;
		case 12:						/* 下手飛/竜							*/
		case 13:
		case 26:						/* 上手飛/竜							*/
		case 27:
			for(i=0; i<4; i++){
				if(testmoveoksub(ban, k, mv1, mv2, dir1[i]))
					return 0;
			}
			break;
		case 17:						/* 上手香								*/
			if(testmoveoksub(ban, k, mv1, mv2, 10))
				return 0;
			break;
		}
		return 13;						/* テストNG								*/
	}
}

/********************************************************************************/
/*		naricheck																*/
/*		move の正当性は確認済みのこと											*/
/********************************************************************************/
static const struct menu nari_menu[]={
	{264, 192, 3, 1, "ﾅﾘ"},
	{264, 212, 3, 2, "ﾌﾅﾘ"},
	{0,0,0,0,0},
};

static int nari_input()
{
	int cmd;
	
	for(;;){
		cmd = menu_select3(nari_menu);
		switch(cmd){
			case 1:							// ナリ
			case 2:							// フナリ
				break;
			default:
				continue;
		}
		lcd_clear_square(264,192, 4*12, 48, WHITE);
		return cmd;						// 1:ナリ、2:フナリ
	}
}

static int naricheck(int *ban, int k, int move)
{
	int p1, p2;
	
	p1 = move & 0xff;
	p2 = (move & 0xff00)>>8;
	if(p1 > 110)
		return move;
	if(k==ISHI_K){
		if(macroy(p1)<=3 || macroy(p2)<=3){
			switch(ban[p1] & 0xff){
			case 1:case 3:case 5:case 7:case 10:case 12:
				if(nari_input()==1)			// 成り
					return move | 0x10000;
				break;
			}
		}
	}
	else{
		if(macroy(p1)>=7 || macroy(p2)>=7){
			switch(ban[p1] & 0xff){
			case 15:case 17:case 19:case 21:case 24:case 26:
				if(nari_input()==1)			// 成り
					return move | 0x10000;
				break;
			}
		}
	}
	return move;
}

/********************************************************************************/
/*		yomiutu																	*/
/*		move の正当性は確認済みのこと											*/
/*		return 1:王取り															*/
/********************************************************************************/
static int yomiutu(int *ban, int k, int move)
{
	int p1, p2, km1, km2;
	
	p1 = move & 0xff;
	p2 =(move & 0xff00)>>8;
	if(p1 < 110){
		km1 = ban[p1];
		km2 = ban[p2] & 0xff;
		if(km2){						/* 駒取り								*/
			switch(km2){				/* 成駒の取りか？						*/
			case 2:case 4:case 6:case 8:case 11:case 13:case 16:case 18:case 20:case 22:case 25:case 27:
				km2--;
				break;
			case 14:case 28:
				return 1;				/* 王取り								*/
			}
			if(km2 <= 14)
				km2+=14;
			else
				km2-=14;
			ban[km2+131]++;				/* 駒台のカウントアップ					*/
		}
		ban[p1] = 0;					/* 移動して駒が無くなった				*/
		if(move & 0x10000)				/* 成フラグ								*/
			km1++;
		ban[p2] = km1;					/* 駒が移動してきた						*/
	}
	else{
		km1 = (p1-131) | k;
		ban[p1]--;						/* 駒台から盤へ							*/
		ban[p2] = km1;
	}
	return 0;
}

/********************************************************************************/
/*		makeallmoves															*/
/********************************************************************************/
static int makeallmoves_sub(int *ban, int s, int p, int p2, int *moves, int *movecnt, int kyoo)
{
	if((ban[p2] & 0xf00)==ISHI_N){
		moves[(*movecnt)++] = (p2<<8) | p;
		if(s==ISHI_S){
			if(macroy(p)<=3 || macroy(p2)<=3){
				switch(ban[p] & 0xff){
				case 1:case 5:case 7:case 10:case 12:	/* 成れる	*/
					moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
					break;
				case 3:					/* 香									*/
					if(macroy(p2)>=2)
						moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
					else
						moves[(*movecnt)-1] = (p2<<8) | p | 0x10000;
					break;
				}
			}
		}
		else{
			if(macroy(p)>=7 || macroy(p2)>=7){
				switch(ban[p] & 0xff){
				case 15:case 19:case 21:case 24:case 26:
					moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
					break;
				case 17:					/* 香									*/
					if(macroy(p2)<=8)
						moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
					else
						moves[(*movecnt)-1] = (p2<<8) | p | 0x10000;
					break;
				}
			}
		}
		return 0;
	}
	if((ban[p2] & 0xf00)==s){
		moves[(*movecnt)++] = (p2<<8) | p;
		if(s==ISHI_S){
			if(macroy(p)<=3 || macroy(p2)<=3){
				switch(ban[p] & 0xff){
				case 1:case 5:case 7:case 10:case 12:	/* 成れる	*/
					moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
					break;
				case 3:					/* 香									*/
					if(macroy(p2)>=2)
						moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
					else
						moves[(*movecnt)-1] = (p2<<8) | p | 0x10000;
					break;
				}
			}
		}
		else{
			if(macroy(p)>=7 || macroy(p2)>=7){
				switch(ban[p] & 0xff){
				case 15:case 19:case 21:case 24:case 26:
					moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
					break;
				case 17:					/* 香									*/
					if(macroy(p2)<=8)
						moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
					else
						moves[(*movecnt)-1] = (p2<<8) | p | 0x10000;
					break;
				}
			}
		}
		return 1;
	}
	return 1;
}

static void makeallmoves(int *ban, int k, int *moves, int *movecnt)
{
	int s, p, p2, k1, i, j, x, y;
	const int *pti;

	*movecnt = 0;
	s = macroinv(k);
	if(k==ISHI_K){
		for(p=21; p<110; p++){
			if((ban[p] & 0xf00)==k){
				k1 = ban[p] & 0xff;
				pti = dir10[k1];
				for(i=0; i<9; i++){
					if(pti[i]==0)
						break;
					p2 = p + pti[i];
					if((ban[p2] & 0xf00)==0 || (ban[p2] & 0xf00)==s){	/* 打てる	*/
						moves[(*movecnt)++] = (p2<<8) | p;
						if(macroy(p)<=3 || macroy(p2)<=3){
							switch(k1){
							case 7:case 10:case 12:	/* 成れる	*/
								moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
								break;
							case 1:case 3:		/* 歩 香								*/
								if(macroy(p2)>=2)
									moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
								else
									moves[(*movecnt)-1] = (p2<<8) | p | 0x10000;
								break;
							case 5:				/* 桂									*/
								if(macroy(p2)>=3)
									moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
								else
									moves[(*movecnt)-1] = (p2<<8) | p | 0x10000;
								break;
							}
						}
					}
				}
				/*** 特殊駒の長距離移動テスト	***/
				switch(k1){						/* 移動元の駒種類						*/
				case 3:							/* 下手香								*/
					for(j=1; ; j++){
						p2 = p-10*j;
						if(makeallmoves_sub(ban, s, p, p2, moves, movecnt, 1))
							break;
					}
					break;
				case 10:						/* 下手角/馬							*/
				case 11:
				case 24:						/* 上手角/馬							*/
				case 25:
					for(i=0; i<4; i++){
						for(j=1; ; j++){
							p2 = p + dir2[i]*j;
							if(makeallmoves_sub(ban, s, p, p2, moves, movecnt, 0))
								break;
						}
					}
					break;
				case 12:						/* 下手飛/竜							*/
				case 13:
				case 26:						/* 上手飛/竜							*/
				case 27:
					for(i=0; i<4; i++){
						for(j=1; ; j++){
							p2 = p + dir1[i]*j;
							if(makeallmoves_sub(ban, s, p, p2, moves, movecnt, 0))
								break;
						}
					}
					break;
				case 17:						/* 上手香								*/
					for(j=1; ; j++){
						p2 = p+10*j;
						if(makeallmoves_sub(ban, s, p, p2, moves, movecnt, 1))
							break;
					}
					break;
				}
			}
		}
		/* 打つ																			*/
		for(k1=1; k1<=14; k1++){
			if(ban[131+k1]){
				if(k1==1){						/* 歩									*/
					for(x=1; x<=9; x++){
						for(y=1; y<=9; y++){
							if((ban[macropos(x,y)] & 0xff)==1){	/* 2歩					*/
								break;
							}
						}
						if(y==9+1){				/* 2歩でない							*/
							for(y=2; y<=9; y++){
								p2 = macropos(x,y);
								if((ban[p2] & 0xf00)==ISHI_N){
									moves[(*movecnt)++] = (p2<<8) | (131+k1);   // 歩を打つ
								}
							}
						}
					}
				}
				else if(k1==3){					/* 香									*/
					for(x=1; x<=9; x++){
						for(y=2; y<=9; y++){
							p2 = macropos(x,y);
							if((ban[p2] & 0xf00)==ISHI_N){
								moves[(*movecnt)++] = (p2<<8) | (131+k1);   // 香を打つ
							}
						}
					}
				}
				else if(k1==5){					/* 桂									*/
					for(x=1; x<=9; x++){
						for(y=3; y<=9; y++){
							p2 = macropos(x,y);
							if((ban[p2] & 0xf00)==ISHI_N){
								moves[(*movecnt)++] = (p2<<8) | (131+k1);   // 桂を打つ
							}
						}
					}
				}
				else{
					for(p2=21; p2<110; p2++){
						if((ban[p2] & 0xf00)==ISHI_N){
							moves[(*movecnt)++] = (p2<<8) | (131+k1);
						}
					}
				}
			}
		}
	}
	else{
		for(p=21; p<110; p++){
			if((ban[p] & 0xf00)==k){
				k1 = ban[p] & 0xff;
				pti = dir10[k1];
				for(i=0; i<9; i++){
					if(pti[i]==0)
						break;
					p2 = p + pti[i];
					if((ban[p2] & 0xf00)==0 || (ban[p2] & 0xf00)==s){	/* 打てる	*/
						moves[(*movecnt)++] = (p2<<8) | p;
						if(macroy(p)>=7 || macroy(p2)>=7){
							switch(k1){
							case 21:case 24:case 26:
								moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
								break;
							case 15:case 17:		/* 歩 香								*/
								if(macroy(p2)<=8)
									moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
								else
									moves[(*movecnt)-1] = (p2<<8) | p | 0x10000;
								break;
							case 19:				/* 桂									*/
								if(macroy(p2)<=7)
									moves[(*movecnt)++] = (p2<<8) | p | 0x10000;
								else
									moves[(*movecnt)-1] = (p2<<8) | p | 0x10000;
								break;
							}
						}
					}
				}
				/*** 特殊駒の長距離移動テスト	***/
				switch(k1){						/* 移動元の駒種類						*/
				case 3:							/* 下手香								*/
					for(j=1; ; j++){
						p2 = p-10*j;
						if(makeallmoves_sub(ban, s, p, p2, moves, movecnt, 1))
							break;
					}
					break;
				case 10:						/* 下手角/馬							*/
				case 11:
				case 24:						/* 上手角/馬							*/
				case 25:
					for(i=0; i<4; i++){
						for(j=1; ; j++){
							p2 = p + dir2[i]*j;
							if(makeallmoves_sub(ban, s, p, p2, moves, movecnt, 0))
								break;
						}
					}
					break;
				case 12:						/* 下手飛/竜							*/
				case 13:
				case 26:						/* 上手飛/竜							*/
				case 27:
					for(i=0; i<4; i++){
						for(j=1; ; j++){
							p2 = p + dir1[i]*j;
							if(makeallmoves_sub(ban, s, p, p2, moves, movecnt, 0))
								break;
						}
					}
					break;
				case 17:						/* 上手香								*/
					for(j=1; ; j++){
						p2 = p+10*j;
						if(makeallmoves_sub(ban, s, p, p2, moves, movecnt, 1))
							break;
					}
					break;
				}
			}
		}
		/* 打つ																			*/
		for(k1=15; k1<=28; k1++){
			if(ban[131+k1]){
				if(k1==15){						/* 歩									*/
					for(x=1; x<=9; x++){
						for(y=1; y<=9; y++){
							if((ban[macropos(x,y)] & 0xff)==15){	/* 2歩				*/
								break;
							}
						}
						if(y==9+1){				/* 2歩でない							*/
							for(y=1; y<=8; y++){
								p2 = macropos(x,y);
								if((ban[p2] & 0xf00)==ISHI_N){
									moves[(*movecnt)++] = (p2<<8) | (131+k1);   // 歩を打つ
								}
							}
						}
					}
				}
				else if(k1==17){					/* 香									*/
					for(x=1; x<=9; x++){
						for(y=1; y<=8; y++){
							p2 = macropos(x,y);
							if((ban[p2] & 0xf00)==ISHI_N){
								moves[(*movecnt)++] = (p2<<8) | (131+k1);   // 香を打つ
							}
						}
					}
				}
				else if(k1==19){					/* 桂									*/
					for(x=1; x<=9; x++){
						for(y=1; y<=7; y++){
							p2 = macropos(x,y);
							if((ban[p2] & 0xf00)==ISHI_N){
								moves[(*movecnt)++] = (p2<<8) | (131+k1);   // 桂を打つ
							}
						}
					}
				}
				else{
					for(p2=21; p2<110; p2++){
						if((ban[p2] & 0xf00)==ISHI_N){
							moves[(*movecnt)++] = (p2<<8) | (131+k1);
						}
					}
				}
			}
		}
	}
}

/********************************************************************************/
/*		MCyomuk																	*/
/********************************************************************************/
struct yomumoves{
	int move;
	int result;
};

static int MCcompk(const void *elem1, const void *elem2)
{
	return ((struct yomumoves *)elem2)->result - ((struct yomumoves *)elem1)->result;
}

//static int MCcomps(const void *elem1, const void *elem2)
//{
//	return -((struct yomumoves *)elem2)->result + ((struct yomumoves *)elem1)->result;
//}

static int judge(int *ban, int k);

//static int MCyomuk(int *ban, int k, int *move, int depth, int alpha, int beta);
//static int MCyomus(int *ban, int *move, int depth, int alpha, int beta);
//static int MCyomuk_sub(int *ban, int *move, int depth, int alpha, int beta);
//static int MCyomus_sub(int *ban, int *move, int depth, int alpha, int beta);


static int MCyomu(int *ban, int k, int *move)
{
	int ret, retsv, mv;
	
	yomiend = 4;
	MCyomucnt = 0;
	ret = MCyomuk(qgoban, k, &mv, 0, -TEN_INFINIT, TEN_INFINIT, 0);
	if(k != ISHI_K)
		ret = -ret;
	retsv = ret;
	*move = mv;
	return retsv;
}

static int MCyomuk(int *ban, int k, int *move, int depth, int alpha, int beta, int buf_ptr)
{
	int i, max, ret, mv, /*moves[1000],*/ movecnt, ban1[160];
	struct yomumoves *yomumoves = (struct yomumoves*)&sv->buf[buf_ptr];
	int s = macroinv(k);
	
//	if(GetTickCount()-MCtime > 10000)	/* １０秒打ち切り						*/
//		return TEN_INFINIT+1;
	if(depth >= yomiend)
		return judge(ban, k);
	makeallmoves(ban, k, sv->moves, &movecnt);
	for(i=0; i<movecnt; i++)
		yomumoves[i].move = sv->moves[i];
	if(yomiend-depth >= 3){
		for(i=0; i<movecnt; i++){
			memcpy(ban1, ban, 160*sizeof(int));
//			cpyban1(ban1, ban);
			if(yomiutu(ban1, k, yomumoves[i].move))
				yomumoves[i].result = 100000+1;	// 王取り
			else
				yomumoves[i].result = -MCyomuk(ban1, s, move, depth+2, -TEN_INFINIT, TEN_INFINIT, buf_ptr+movecnt*2);
		}
		qsort(yomumoves, movecnt, sizeof(struct yomumoves), MCcompk);
		if(yomiend-depth >= 4){
			if(movecnt > 10)
				movecnt = 10;
		}
	}
	max = -TEN_INFINIT;
	for(i=0; i<movecnt; i++){
		memcpy(ban1, ban, 160*sizeof(int));
		if(yomiutu(ban1, k, yomumoves[i].move)){
			*move = yomumoves[i].move;
			return 100000;				/* 王取り								*/
		}

		ret = -MCyomuk(ban1, s, &mv, depth+1, -beta, -alpha, buf_ptr+movecnt*2);

		if(depth==0 && tekazu <= 15){
			ret += MCrnd(20)-10;		// 乱数
		}
		if(ret >= beta)
			return (ret+1);
		if(max < ret){
			max = ret;
			*move = yomumoves[i].move;
			if(max > alpha)
				alpha = max;
		}
	}
	return max;
}

/********************************************************************************/
/*		judge																	*/
/********************************************************************************/
static int judge(int *ban, int k)
{
	int sum, p, km;
	
	MCyomucnt++;
	sum = 0;
	for(p=21; p<110; p++){
		km = ban[p] & 0xff;
		sum += tenall[km];
		if(km){
			sum += -macroy(p)+5;
		}
	}
	for(p=1; p<29; p++){
		sum += ban[131+p]*tenall[p];
	}
	if(k==ISHI_K)
		return sum;
	else
		return -sum;
}




/********************************************************************************/
/*		ut_test																	*/
/********************************************************************************/
void ut_test()
{
}
/********************************************************************************/
/*		syo_disp_koma															*/
/*		x: 1～9 が盤面、-1～11 まで OK											*/
/*		y: 1～9 が盤面、 1～9  まで OK											*/
/********************************************************************************/
static void syo_disp_clr(int x, int y)	// クリア
{
	int xx, yy;
	
	if(x< -1 || x>11 || y<1 || y>9){
		return;
	}
	xx = (x-1)*24+24*2;
	yy = (y-1)*24+12;
	lcd_clear_square(xx+1, yy+1, 24-1, 24-1, syo_ban_color);	// バックを塗りつぶし
}

static void syo_disp_waku()
{
	int x, y;
	
	for(y=0; y<=9; y++){
		lcd_clear_square(24*2, y*24+12, 24*9+1, 1, BLACK);
	}
	for(x=0; x<=9; x++){
		lcd_clear_square(x*24+24*2, 12, 1, 24*9+1, BLACK);
	}
}

static void syo_disp_koma(int x, int y, int code)	// 先手番駒表示
{
	int xx, yy;
	
	if(x< -1 || x>11 || y<1 || y>9){
		return;
	}
	xx = (x-1)*24+24*2;
	yy = (y-1)*24+12;
	lcd_clear_square(xx+1, yy+1, 24-1, 24-1, syo_ban_color);	// バックを塗りつぶし
//	//	周囲の正方形を描く
//	lcd_clear_square(xx,    yy,    24+1, 1,    BLACK);
//	lcd_clear_square(xx,    yy+24, 24+1, 1,    BLACK);
//	lcd_clear_square(xx,    yy,    1,    24+1, BLACK);
//	lcd_clear_square(xx+24, yy,    1,    24+1, BLACK);
	//	駒の周囲の線を描く
	draw_lineA(xx+3,  yy+22, xx+5,  yy+6,  BLACK);
	draw_lineA(xx+5,  yy+6,  xx+12, yy+1,  BLACK);
	draw_lineA(xx+12, yy+1,  xx+19, yy+6,  BLACK);
	draw_lineA(xx+19, yy+6,  xx+21, yy+22, BLACK);
	draw_lineA(xx+21, yy+22, xx+3,  yy+22, BLACK);
	//	駒の文字を書く
	sjis_disp_char_x1(xx+7, yy+6, code);
}

static void syo_disp_rev_koma(int x, int y, int code)	// 後手番駒表示
{
	int xx, yy;
	
	if(x< -1 || x>11 || y<1 || y>9){
		return;
	}
	xx = (x-1)*24+24*2;
	yy = (y-1)*24+12;
	lcd_clear_square(xx+1, yy+1, 24-1, 24-1, syo_ban_color);	// バックを塗りつぶし
	//	駒の周囲の線を描く
	draw_lineA(xx+3,  yy+2,  xx+5,  yy+18, BLACK);
	draw_lineA(xx+5,  yy+18, xx+12, yy+23, BLACK);
	draw_lineA(xx+12, yy+23, xx+19, yy+18, BLACK);
	draw_lineA(xx+19, yy+18, xx+21, yy+2,  BLACK);
	draw_lineA(xx+21, yy+2,  xx+3,  yy+2,  BLACK);
	//	駒の文字を書く
	sjis_disp_char_reverse_x1(xx+6, yy+3, code);
}
/********************************************************************************/
/*		dispban																	*/
/********************************************************************************/
static void dispban(int *ban, int move)
{
	int x, y, pos, pos_cur, koma, code;
	char buf[20];
	
	//	枠表示
	syo_disp_waku();
	//	盤面表示
	//	カーソル位置
	if(move){
		pos_cur = (move>>8) & 0xff;
	}
	else{
		pos_cur = 0;
	}
	back_color = YELLOW;
	text_color = BLACK;
	for(y=1; y<=9; y++){
		for(x=1; x<=9; x++){
			pos = macropos(x,y);
			if(pos==pos_cur){
				back_color = CYAN;
			}
			koma = ban[pos] & 0xff;
			if(koma>=1 && koma<=14){
				syo_disp_koma(x, y, tbl1[koma-1]);
			}
			else if(koma>=15 && koma<=28){
				syo_disp_rev_koma(x, y, tbl1[koma-15]);
			}
			else{
				syo_disp_clr(x, y);
			}
			back_color = YELLOW;
		}
	}
	//	持ち駒表示
	lcd_clear_square(  0,0, 48,240, WHITE);	// クリア
	lcd_clear_square(265,0, 48,240, WHITE);
	for(koma=1; koma<=14; koma++){		// 先手
		if(ban[131+koma]){				// koma の持ち駒有り
			code = tbl1[koma-1];
			syo_disp_koma(11, code, code);
			sprintf(buf, "%2d", ban[131+koma]);	// 駒数
			lcd_disp_str_x1(265, (code-1)*24+12+6, buf);
		}
	}
	for(koma=15; koma<=28; koma++){		// 後手
		if(ban[131+koma]){				// koma の持ち駒有り
			code = tbl1[koma-15];
			syo_disp_rev_koma(-1, code, code);
			sprintf(buf, "%d", ban[131+koma]);	// 駒数
			lcd_disp_str_x1(24, (code-1)*24+12+6, buf);
		}
	}
}
/********************************************************************************/
/*		human_input																*/
/********************************************************************************/
static int human_input(int *ban, int k, int disp_move)
{
	int x, y, move, p1, p2;
	
	for(;;){
		dispban(ban, disp_move);
		
		//	get source pos
		mouse_updown(&x, &y);
		x = x/24 - 1;
		y = (y+12)/24;
		back_color = RED;
		if(y<1 || y>9){
//			buzzer(500,1000);
			continue;
		}
		if(x>=1 && x<=9){
			p1 = macropos(x,y);
			if((ban[p1] & 0xf00) != k){		// 自分の駒ではなかった
//				buzzer(500,1000);
				continue;
			}
			if(k==ISHI_K){
				syo_disp_koma(x, y, tbl1[(ban[p1]&0xff)-1]);
			}
			else{
				syo_disp_rev_koma(x, y, tbl1[(ban[p1]&0xff)-15]);
			}
		}
		else if(k==ISHI_K){					// 先手番
			if(x!=11 || y>7){				// 持ち駒ではなかった
//				buzzer(500,1000);
				continue;
			}
			p1 = 131 + tbl2[y-1];
			if(ban[p1]==0){					// 持ち駒 0個
//				buzzer(500,1000);
				continue;
			}
			syo_disp_koma(x, y, y);
		}
		else{								// 後手番
			if(x!= -1 || y>7){				// 持ち駒ではなかった
//				buzzer(500,1000);
				continue;
			}
			p1 = 131 + (tbl2[y-1] -1+15);
			if(ban[p1]==0){					// 持ち駒 0個
//				buzzer(500,1000);
				continue;
			}
			syo_disp_rev_koma(x, y, y);
		}
		
		//	get dst pos
		mouse_updown(&x, &y);
		x = x/24 - 1;
		y = (y+12)/24;
		if(x<1 || x>9 || y<1 || y>9){
//			buzzer(500,1000);
			continue;
		}
		p2 = macropos(x,y);					// 動かせるかのチェックは game() に帰ってからやる
		move = p1 | (p2<<8);
		return move;
	}
}
/********************************************************************************/
/*		hash_setup																*/
/********************************************************************************/
#define MCrndA() MCrnd(0x7fffffff)

#define RND8BYTE (((unsigned long long)MCrndA()<<61)+((unsigned long long)MCrndA()<<55)+ \
			((unsigned long long)MCrndA()<<48)+((unsigned long long)MCrndA()<<43)+ \
			((unsigned long long)MCrndA()<<36)+((unsigned long long)MCrndA()<<31)+ \
			((unsigned long long)MCrndA()<<24)+((unsigned long long)MCrndA()<<19)+ \
			((unsigned long long)MCrndA()<<13)+((unsigned long long)MCrndA()<<7)+ \
			((unsigned long long)MCrndA())+((unsigned long long)MCrndA()>>5)+ \
			((unsigned long long)MCrndA()>>12))+((unsigned long long)MCrndA()>>19)


static void hash_setup()
{
	int i;
	
	memset(sv, 0, sizeof(work_buf));
	for(i=0; i<160; i++){
		sv->hash[i] = RND8BYTE;
	}
}
/********************************************************************************/
/*		hashcode																*/
/********************************************************************************/
unsigned long long hashcode(int *ban, int teban)
{
	unsigned long long code = teban;
	int i;
	
	for(i=0; i<160; i++){
		code += sv->hash[i] * (ban[i] & 0xff);
	}
	return code;
}
/********************************************************************************/
/*		sennichite																*/
/********************************************************************************/
int sennichite(int *ban, int k)
{
	unsigned long long code;
	int i, c;
	static int ptr;
	
	code = hashcode(ban, k);
	sv->code[ptr] = code;
	if(++ptr >= 100){
		ptr = 0;
	}
	c = 0;
	for(i=0; i<100; i++){
		if(sv->code[i]==code)
			c++;
	}
	if(c>=5)
		return 1;							// 千日手になった
	return 0;
}
/********************************************************************************/
/*		game																	*/
/********************************************************************************/
static void game()
{
	int player, move, move_tmp, ret, x, y, ten;
	char buf[20];
	
	lcd_clear(WHITE);
	syo_disp_waku();						//	枠表示
	hash_setup();
	memcpy(qgoban, startpos, 160*sizeof(int));	// initban
	teban = start_teban;
	tekazu = 1;
	move = 0;
	ten = 0;
	for(;;){
		dispban(qgoban, move);
		sprintf(buf, "%d ", ten);
		lcd_disp_str_x1(0,224, buf);	// 点数表示
		if(sennichite(qgoban, teban)){
//			buzzer(700,1000);
			lcd_cdisp_str_x2(100, "sennichite");
			mouse_updown(&x, &y);
			return;
		}
		player = teban==ISHI_K ? black : white;
		if(player==0){						// cpu
			ten = MCyomu(qgoban, teban, &move);
			
//			buzzer(1000, 50);
			if(testmoveok(qgoban, teban, move)){	// 打てない
				fatal("ｳﾃﾅｲﾃﾃﾞｽ",0,0);
			}
			ret = yomiutu(qgoban, teban, move);
			if(ret){						// 玉取り
//				buzzer(500, 1000);
				lcd_cdisp_str_x2(100, "CPU win");
				mouse_updown(&x, &y);
				return;
			}
		}
		else{								// human
			move_tmp = human_input(qgoban, teban, move);
			ret = testmoveok(qgoban, teban, move_tmp);
			if(ret){                        // 打てない
//				buzzer(500,1000);
				continue;
			}
			move = move_tmp;
			move = naricheck(qgoban, teban, move);	// 成りの処理
			ret = yomiutu(qgoban, teban, move);
			if(ret){						// 玉取り
//				buzzer(1000, 1000);
				lcd_cdisp_str_x2(100, "you win");
				mouse_updown(&x, &y);
				return;
			}
		}
		
		tekazu++;
		teban = macroinv(teban);
	}
}
/********************************************************************************/
/*		syogi_main																*/
/********************************************************************************/
static const struct menu syo_play_menu[]={
	{0,  0,25, 1,"Black:cpu,   White:cpu"},
	{0, 60,25, 2,"Black:human, White:human"},
	{0,120,25, 3,"Black:cpu,   white:human"},
	{0,180,25, 4,"Black:hunam, White:cpu"},
	{0,0,0,0,0}
};

void syogi_main()
{
	int x, y, cmd;
	
	lcd_clear(RED_D);
	lcd_cdisp_str_x2( 90, "Syogi");
	lcd_cdisp_str_x1(130, "K.Fukumoto");
	mouse_updown(&x, &y);
	
	lcd_clear(BLACK);
	btn_back_color = GREEN_D;
	for(;;){
		cmd = menu_select0(syo_play_menu);
		switch(cmd){
			case 1:						// cpu - cpu
				start_teban = ISHI_K;
				black = white = 0;
				break;
			case 2:						// human - human
				start_teban = ISHI_K;
				black = white = 1;
				break;
			case 3:						// cpu black
				start_teban = ISHI_S;
				black = 1;
				white = 0;
				break;
			case 4:						// cpu white
				start_teban = ISHI_K;
				black = 1;
				white = 0;
				break;
			default:
				continue;
		}
		break;
	}
	game();
	back_color = WHITE;
	text_color = BLACK;
}
