#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"
#include "menu.h"
#include "tch.h"
#include "gomoku.h"


struct yomumoves{
	int pos;
	int result;
};


struct gv{
	struct yomumoves yomumoves[250];
	int buf[4500];
};

extern int work_buf[5000];

static struct gv *gv = (struct gv*)work_buf;
static int goban[274];					// ÉÅÉCÉìÇÃî’
static int yomiend;
static int ban_color;
static int black, white;
static int teban, tekazu;
//static int dir1[4]={-16,-1,1,16};
//static int dir2[4]={-17,-15,15,17};
//static int dir3[8]={-17,-16,-15,1,17,16,15,-1};
static int dir10[4]={1,15,16,17};


static int MCjoseki(int *ban);


/********************************************************************************/
/*		initban																	*/
/********************************************************************************/
static void initban(int *ban)
{
	int i, x, y;
	
	for(i=0; i<274; i++){
		ban[i] = ISHI_O;
	}
	for(y=1; y<=15; y++){
		for(x=1; x<=15; x++){
			ban[macropos(x,y)] = ISHI_N;
		}
	}
}
/********************************************************************************/
/*		cpyban1/2																*/
/********************************************************************************/
static void cpyban1(int *ban1, int *ban2)
{
	memcpy(ban1, ban2, 274*sizeof(int));
}

static void cpyban2(int *ban1, int *ban2)
{
	int i;
	
	for(i=0; i<274; i++){
		ban1[i] = ban2[i] & 3;
	}
}
/********************************************************************************/
/*		MCupdateban																*/
/*		ban Ç…ÉtÉâÉOÇïtÇØÇÈ													*/
/********************************************************************************/
//		MCupdates1
//		return  0: Ç“Ç¡ÇΩÇËé~Ç‹Ç¡ÇƒÇ¢ÇÈ
//		return -1: âƒé~Çﬂ
//		return -2: ÇQä‘ãÛÇ´
//		return -3: ÇQä‘ãÛÇ´ÇÃèHé~Çﬂ
//		return  n: ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒ
//		h:1 ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒå„Ç™é~Ç‹Ç¡ÇƒÇ¢ÇÈ
//		h:2 ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒå„Ç™èHé~Çﬂ
//		h:0 ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒå„Ç…ãÛÇ´Ç†ÇË

static int MCupdates1(int *ban, int p, int dir, int k, int *h)
{
	int n;
	int *banptr;
	
	banptr = &ban[p+dir];
	*h = 0;
	if(*banptr & 3){					/* ó◊Ç™êŒÇ©î’í[							*/
		return 0;						// Ç“Ç¡ÇΩÇËé~Ç‹Ç¡ÇƒÇ¢ÇÈ
	}
	//	ó◊Ç™É_ÉÅÇæÇ¡ÇΩ
	
	banptr += dir;
	if(*banptr & 3){					/* ÇPä‘Ç∆Ç—Ç≈êŒÇ©î’í[					*/
		if((*banptr & 3) != k)			// àÍå©îÚÇ—Ç™é©êŒÇ≈Ç»Ç©Ç¡ÇΩ
			return -1;					/* âƒé~Çﬂ								*/
		
		// àÍä‘îÚÇ—Ç™é©êŒÇæÇ¡ÇΩ
		n = 1;
		banptr += dir;
		for(;;){
			if ((*banptr & 3) == k)
				n++;
			else
				break;
			banptr += dir;
		}
		if(*banptr & 3){				// ëºêŒÇ©î’í[Ç≈é~Ç‹Ç¡ÇƒÇ¢ÇÈ
			*h = 1;						/* Ç∑ÇÆÇ…é~Ç‹Ç¡ÇƒÇ¢ÇÈ					*/
		}
		else{							/* ãÛÇ´									*/
			banptr += dir;
			if((*banptr & 3)==k)
				*h = 2;					/* èHé~Çﬂ								*/
			else
				*h = 0;					// àÍä‘îÚÇ—ÇÃêÊÇ…ãÛÇ´Ç†ÇË
		}
		return n;
	}
	//	ÇQä‘Ç∆Ç—
	if((ban[p+dir*3] & 3)){				/* ÇQä‘Ç∆Ç—Ç≈êŒÇ©î’í[					*/
		if((ban[p+dir*3] & 3) == k)
			return -3;					/* ÇQä‘Ç∆Ç—ÇÃèHé~Çﬂ						*/
	}
	return -2;							/* ÇQä‘ãÛÇ´								*/
}

//	p ÇÕòAÇÃí[ÅAdir ï˚å¸ÇÃÉ_ÉÅêî

static int MCupdates2(int *ban, int p, int dir)
{
	int cnt;
	int *banptr;
	
	cnt = 0;
	banptr = &ban[p+dir];
	for(;;){
		if((*banptr & 3)==ISHI_N){
			cnt++;
			if(cnt >= 3)
				return cnt;
		}
		else
			return cnt;
		banptr += dir;
	}
}

static void MCupdatebans(int *ban, int p)
{
	int i, d, banp, ren, p1, p2, h1, h2, hasi1, hasi2, flag, flag2, flag3;
	int *banptr;
	
	banp = ban[p];
	for(i=0; i<4; i++){
		d = dir10[i];
		ren = 1;
		p1 = p;								// òAÇÃí[Åié©êŒÅjà íu
		banptr = &ban[p+d];
		for(;;){
			if((*banptr & 3)==ISHI_S){
				ren++;
				p1 += d;
			}
			else
				break;
			banptr += d;
		}
		p2 = p;								// òAÇÃí[Åié©êŒÅjà íu
		banptr = &ban[p-d];
		for(;;){
			if((*banptr & 3)==ISHI_S){
				ren++;
				p2 -= d;
			}
			else
				break;
			banptr -= d;
		}
		if(ren >= 5){
			banp |= ISHI_S5;
		}
		else if(ren==4){
			if((ban[p1+d] & 3)==ISHI_N && (ban[p2-d] & 3)==ISHI_N){
				if(banp & (ISHI_S4X|ISHI_S4)){
					banp |= ISHI_S44;
				}
				banp |= ISHI_S4X;
			}
			else if((ban[p1+d] & 3)==ISHI_N){
				if(banp & (ISHI_S4X|ISHI_S4)){
					banp |= ISHI_S44;
				}
				banp |= ISHI_S4;
			}
			else if((ban[p2-d] & 3)==ISHI_N){
				if(banp & (ISHI_S4X|ISHI_S4)){
					banp |= ISHI_S44;
				}
				banp |= ISHI_S4;
			}
		}
		else{								// ren <= 3
			hasi1 = MCupdates1(ban, p1, d, ISHI_S, &h1);
			hasi2 = MCupdates1(ban, p2, -d, ISHI_S, &h2);
//			hasi  0: Ç“Ç¡ÇΩÇËé~Ç‹Ç¡ÇƒÇ¢ÇÈ
//			hasi -1: âƒé~Çﬂ
//			hasi -2: ÇQä‘ãÛÇ´
//			hasi -3: ÇQä‘ãÛÇ´ÇÃèHé~Çﬂ
//			hasi  n: ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒ
//			h:1 ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒå„Ç™é~Ç‹Ç¡ÇƒÇ¢ÇÈ
//			h:2 ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒå„Ç™èHé~Çﬂ
//			h:0 ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒå„Ç…ãÛÇ´Ç†ÇË
			flag2 = 0;						// ISHI_S2 Ç™óLÇ¡ÇΩÇÁ 1
			flag3 = 0;						// ISHI_S3 Ç™óLÇ¡ÇΩÇÁ 1
			if(ren==3){
				flag = 0;
				if(hasi1 >= 1){		/* ÇPä‘Ç∆Ç—Ç≈ÇPå¬à»è„ÇÃé©êŒ				*/
					if(banp & (ISHI_S4X|ISHI_S4)){
						banp |= ISHI_S44;
					}
					banp |= ISHI_S4;
					flag = 1;
				}
				if(hasi2 >= 1){		/* ÇPä‘Ç∆Ç—Ç≈ÇPå¬à»è„ÇÃé©êŒ				*/
					if(banp & (ISHI_S4X|ISHI_S4)){
						banp |= ISHI_S44;
					}
					banp |= ISHI_S4;
					flag = 1;
				}
				if(flag==0){
					if(hasi1 < 0 && hasi2 < 0){	/* Ç“Ç¡ÇΩÇËé~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢ÅAé©êŒÇ‡ÇªÇŒÇ…Ç»Ç¢	*/
						if(hasi1 != -1 || hasi2 != -1){	/* âƒé~ÇﬂÇ≈Ç»Ç¢			*/
							flag3 = 1;		// ISHI_S3
						}
					}
				}
				if(hasi1< -2 || hasi2< -2 || (hasi1 && hasi2))
					flag2 = 1;				// ISHI_S2;
			}
			else if(ren==2){
				flag = 0;
				if(hasi1 >= 2){		/* ÇPä‘Ç∆Ç—Ç≈ÇQå¬à»è„ÇÃé©êŒ				*/
					if(banp & (ISHI_S4X|ISHI_S4)){
						banp |= ISHI_S44;
					}
					banp |= ISHI_S4;
					flag = 1;
				}
				if(hasi2 >= 2){		/* ÇPä‘Ç∆Ç—Ç≈ÇQå¬à»è„ÇÃé©êŒ				*/
					if(banp & (ISHI_S4X|ISHI_S4)){
						banp |= ISHI_S44;
					}
					banp |= ISHI_S4;
					flag = 1;
				}
				if(flag==0){			// Ç±ÇÃóÒÇ…ÇSÇ™óLÇ¡ÇΩÇÁÇRÇÃÉ`ÉFÉbÉNÇÕïsóv
					if(hasi1==1){		/* ÇPä‘Ç∆Ç—Ç≈ÇPå¬ÇÃé©êŒ					*/
						if(h1 != 1){		/* ÇPä‘Ç∆Ç—ÇÃêÊÇ™é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢			*/
							if(hasi2 != 0){	/* Ç“Ç¡ÇΩÇËé~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢			*/
								flag3 = 1;	// ISHI_S3
							}
							else{
								flag2 = 1;
							}
						}
						else{
							if(hasi2)
								flag2 = 1;
						}
					}
					if(hasi2==1){		/* ÇPä‘Ç∆Ç—Ç≈ÇPå¬ÇÃé©êŒ					*/
						if(h2 != 1){		/* ÇPä‘Ç∆Ç—ÇÃêÊÇ™é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢			*/
							if(hasi1 != 0){	/* Ç“Ç¡ÇΩÇËé~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢			*/
								flag3 = 1;	// ISHI_S3
							}
							else{
								flag2 = 1;
							}
						}
						else{
							if(hasi1)
								flag2 = 1;
						}
					}
				}
				if((ban[p1+d] & 3)==ISHI_N && (ban[p2-d] & 3)==ISHI_N)
					flag2 = 1;
			}
			else if(ren==1){
				flag = 0;
				if(hasi1 >= 3){		/* ÇPä‘Ç∆Ç—Ç≈ÇRå¬à»è„ÇÃé©êŒ				*/
					if(banp & (ISHI_S4X|ISHI_S4)){
						banp |= ISHI_S44;
					}
					banp |= ISHI_S4;
					flag = 1;
				}
				if(hasi2 >= 3){		/* ÇPä‘Ç∆Ç—Ç≈ÇRå¬à»è„ÇÃé©êŒ				*/
					if(banp & (ISHI_S4X|ISHI_S4)){
						banp |= ISHI_S44;
					}
					banp |= ISHI_S4;
					flag = 1;
				}
				if(flag==0){			// Ç±ÇÃóÒÇ…ÇSÇ™óLÇ¡ÇΩÇÁÇRÇÃÉ`ÉFÉbÉNÇÕïsóv
					if(hasi1==2){		/* ÇPä‘Ç∆Ç—Ç≈ÇQå¬ÇÃé©êŒ					*/
						if(h1 != 1){		/* ÇPä‘Ç∆Ç—ÇÃêÊÇ™é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢			*/
							if(hasi2 != 0)	/* Ç“Ç¡ÇΩÇËé~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢			*/
								flag3 = 1;	// ISHI_S3
							else
								flag2 = 1;	// ISHI_S2
						}
						else{
							if(hasi2)
								flag2 = 1;
						}
					}
					if(hasi2==2){		/* ÇPä‘Ç∆Ç—Ç≈ÇQå¬ÇÃé©êŒ					*/
						if(h2 != 1){		/* ÇPä‘Ç∆Ç—ÇÃêÊÇ™é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢			*/
							if(hasi1 != 0)	/* Ç“Ç¡ÇΩÇËé~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢			*/
								flag3 = 1;	// ISHI_S3
							else
								flag2 = 1;
						}
						else{
							if(hasi1)
								flag2 = 1;
						}
					}
				}
				if(hasi1==1){
					if((h1 != 1) && (hasi2 != 0))
						banp |= ISHI_S2;
				}
				if(hasi2==1){
					if((h2 != 1) && (hasi1 != 0))
						banp |= ISHI_S2;
				}
			}
			if(flag3){
				if(banp & ISHI_S3)
					banp |= ISHI_S33;
				banp |= ISHI_S3;
			}
			if(flag2 && (banp & (ISHI_S4X|ISHI_S4|ISHI_S3))==0){
				banp |= ISHI_S2;
			}
		}
	}
	ban[p] = banp;
}

#define SETFLAG(flag) {if(banp & (flag))banp |= ISHI_KINSYU;else banp |= (flag);}

static void MCupdatebank(int *ban, int p)
{
	int i, d, banp, ren, p1, p2, hasi1, hasi2, h1, h2, flag, flag2, flag3;
	int *banptr;
	
	banp = ban[p];
	for(i=0; i<4; i++){
		ren = 1;
		p1 = p;
		d = dir10[i];
		banptr = &ban[p+d];
		for(;;){
			if((*banptr & 3)==ISHI_K){
				ren++;
				p1 += d;
			}
			else
				break;
			banptr += d;
		}
		p2 = p;								// òAÇÃí[Åié©êŒÅjà íu
		banptr = &ban[p-d];
		for(;;){
			if((*banptr & 3)==ISHI_K){
				ren++;
				p2 -= d;
			}
			else
				break;
			banptr -= d;
		}
		hasi1 = MCupdates1(ban, p1, d, ISHI_K, &h1);
		hasi2 = MCupdates1(ban, p2, -d, ISHI_K, &h2);
//			hasi  0: Ç“Ç¡ÇΩÇËé~Ç‹Ç¡ÇƒÇ¢ÇÈ
//			hasi -1: âƒé~Çﬂ
//			hasi -2: ÇQä‘ãÛÇ´
//			hasi -3: ÇQä‘ãÛÇ´ÇÃèHé~Çﬂ
//			hasi  n: ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒ
//			h:1 ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒå„Ç™é~Ç‹Ç¡ÇƒÇ¢ÇÈ
//			h:2 ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒå„Ç™èHé~Çﬂ
//			h:0 ÇPä‘ãÛÇ´Ç≈ n å¬ÇÃé©êŒå„Ç…ãÛÇ´Ç†ÇË
		if(ren > 5)
			banp |= ISHI_KINSYU;
		else if(ren==5)
			banp |= ISHI_K5;
		else if(ren==4){
			if(hasi1 < 0 && hasi2 < 0)
				SETFLAG(ISHI_K4X|ISHI_K4)
			else if(hasi1 < 0 || hasi2 < 0)	/* é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢(èHé~Çﬂä‹Çﬁ)			*/
				SETFLAG(ISHI_K4)
		}
		else{
			flag3 = 0;						// ISHI_K3
			flag2 = 0;						// ISHI_K2
			if(ren==3){
				flag = 0;
				if(hasi1==1){		/* ÇPä‘Ç∆Ç—Ç≈ÇPå¬ÇÃé©êŒ					*/
					SETFLAG(ISHI_K4)
					flag = 1;
				}
				if(hasi2==1){
					SETFLAG(ISHI_K4);
					flag = 1;
				}
				if(flag==0){
					if(hasi1 < 0 && hasi2 < 0){	/* é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢		*/
						if(hasi1==-2 || hasi2==-2)		/* ÇQä‘Ç–ÇÁÇ´			*/
							flag3 = 1;
						else
							flag2 = 1;
					}
					else{
						if(hasi1>=0 && hasi2== -2)
							flag2 = 1;
						if(hasi2>=0 && hasi1== -2)
							flag2 = 1;
					}
				}
			}
			else if(ren==2){
				flag = 0;
				if(hasi1==2){			/* ÇPä‘Ç∆Ç—Ç≈ÇQå¬ÇÃé©êŒ					*/
					SETFLAG(ISHI_K4)
					flag = 1;
				}
				if(hasi2==2){
					SETFLAG(ISHI_K4);
					flag = 1;
				}
				if(flag==0){
					if(hasi1==1){			/* ÇPä‘Ç∆Ç—Ç≈ÇPå¬ÇÃé©êŒ					*/
						if(h1==0){			/* ãÛÇ´Ç†ÇË								*/
							if(hasi2 < 0)	/* é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢					*/
								flag3 = 1;
							else
								flag2 = 1;
						}
						else{				// h1 ë§ÇÕé~Ç‹Ç¡ÇƒÇ¢ÇÈ
							if(hasi2 < 0)	// ÇPå¬äJÇ¢ÇƒÇ¢ÇÈ
								flag2 = 1;
						}
					}
					if(hasi2==1){			/* ÇPä‘Ç∆Ç—Ç≈ÇPå¬ÇÃé©êŒ					*/
						if(h2==0){			/* ãÛÇ´Ç†ÇË								*/
							if(hasi1 < 0)	/* é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢					*/
								flag3 = 1;
							else
								flag2 = 1;
						}
						else{				// h2 ë§ÇÕé~Ç‹Ç¡ÇƒÇ¢ÇÈ
							if(hasi1 < 0)	//ÇPå¬ãÛÇ¢ÇƒÇ¢ÇÈ
								flag2 = 1;
						}
					}
				}
				if((banp & (ISHI_K3|ISHI_K4|ISHI_K4X|ISHI_K5))==0){
					if(MCupdates2(ban, p1, dir10[i])+MCupdates2(ban, p2, -dir10[i]) >= 3)
						flag2 = 1;
				}
			}
			else if(ren==1){
				flag = 0;
				if(hasi1==3){			/* ÇPä‘Ç∆Ç—Ç≈ÇRå¬ÇÃé©êŒ					*/
					SETFLAG(ISHI_K4)
					flag = 1;
				}
				if(hasi2==3){
					SETFLAG(ISHI_K4)
					flag = 1;
				}
				if(flag==0){
					if(hasi1==2){			/* ÇPä‘Ç∆Ç—Ç≈ÇQå¬ÇÃé©êŒ					*/
						if(h1==0){			/* ãÛÇ´Ç†ÇË								*/
							if(hasi2 < 0)	/* é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢					*/
								flag3 = 1;
							else
								flag2 = 1;
						}
						else{
							if(hasi2 < 0)
								banp |= ISHI_K2;
						}
					}
					if(hasi2==2){			/* ÇPä‘Ç∆Ç—Ç≈ÇQå¬ÇÃé©êŒ					*/
						if(h2==0){			/* ãÛÇ´Ç†ÇË								*/
							if(hasi1 < 0)	/* é~Ç‹Ç¡ÇƒÇ¢Ç»Ç¢					*/
								flag3 = 1;
							else
								flag2 = 1;
						}
						else{
							if(hasi1 < 0)
								banp |= ISHI_K2;
						}
					}
					if(hasi1==1){
						if((h1 != 1) && (hasi2 != 0))
							flag2 = 1;
					}
					if(hasi2==1){
						if((h2 != 1) && (hasi1 != 0))
							flag2 = 1;
					}
				}
			}
			if(flag3)
				SETFLAG(ISHI_K3);
			if(flag2 && (banp & (ISHI_KINSYU|ISHI_K5|ISHI_K4X|ISHI_K4|ISHI_K3))==0)
				banp |= ISHI_K2;
		}
	}
	ban[p] = banp;
}

static void MCmakeban(int *ban)
{
	int p;
	
	cpyban2(ban, ban);					/* ÉtÉâÉOÉNÉäÉA							*/
	for(p=17; p<256; p++){
		if(ban[p]==ISHI_N){
			MCupdatebank(ban, p);
			MCupdatebans(ban, p);
		}
	}
}

static void MCupdateban(int *ban, int pos)
{
	int i, j;
	
//	time1 = GetTickCount();
//	update_cnt++;
	if((ban[pos] & 3)==ISHI_N){
		ban[pos] &= 3;				/* ÉtÉâÉOÉNÉäÉA							*/
		MCupdatebank(ban, pos);
		MCupdatebans(ban, pos);
	}
	for(i=0; i<4; i++){
		for(j=1; ; j++){
			if((ban[pos+dir10[i]*j] & 3)==ISHI_O)
				break;
			if((ban[pos+dir10[i]*j] & 3)==ISHI_N){
				ban[pos+dir10[i]*j] &= 3;			/* ÉtÉâÉOÉNÉäÉA							*/
				MCupdatebank(ban, pos+dir10[i]*j);
				MCupdatebans(ban, pos+dir10[i]*j);
			}
		}
		for(j=-1; ; j--){
			if((ban[pos+dir10[i]*j] & 3)==ISHI_O)
				break;
			if((ban[pos+dir10[i]*j] & 3)==ISHI_N){
				ban[pos+dir10[i]*j] &= 3;			/* ÉtÉâÉOÉNÉäÉA							*/
				MCupdatebank(ban, pos+dir10[i]*j);
				MCupdatebans(ban, pos+dir10[i]*j);
			}
		}
	}
//	update_time += GetTickCount()-time1;
}
/********************************************************************************/
/*		judge																	*/
/********************************************************************************/
static int judge(int *ban, int k)
{
	int i, b, ten, flagS5, flagK5, flagS4X, flagK4X, flagKINSYU;
	
	ten = 0;
	if(k==ISHI_K){							// çïî‘
		flagS5 = flagK5 = flagS4X = flagK4X = flagKINSYU = 0;
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & 3){
				continue;
			}
			if(b & ISHI_K5){				// é©ï™Ç…ÇSòAÇ™óLÇ¡ÇΩÇ©ÇÁèüÇø
				return 100000;
			}
			if(b & ISHI_KINSYU){			// ã÷éË
				if(b & ISHI_S5){			// ã÷éËÇë≈ÇΩÇ¥ÇÈÇìæÇ»Ç¢
					flagKINSYU = i;
				}
				continue;
			}
			if(b & ISHI_S5){
				flagS5 = i;					// ëäéËÇ…ÇSòAÇ™óLÇ¡ÇΩ
			}
			if(b & ISHI_K4X){
				flagK4X = i;				// é©ï™Ç…äàÇRÇ™óLÇ¡ÇΩ
			}
			if(b & ISHI_S4X){
				flagS4X = i;				// ëäéËÇ…äàÇRÇ™óLÇ¡ÇΩ
			}
		}
		if(flagKINSYU){						// ã÷éËïâÇØ
			for(i=17; i<256; i++){
				if((ban[i] & (3 | ISHI_KINSYU))==0){
					break;
				}
			}
			return -100000;					// ïâÇØ
		}
		if(flagK4X && flagS5==0){			// é©ï™Ç…äàÇRÇ™óLÇÈÅAëäéËÇ…ÇSòAñ≥Çµ
			return 100000;					// èüÇø
		}
		//		ÉmÅ[É}ÉãÇ»ì_êîïtÇØ
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & 3){
				continue;
			}
			if(b & ISHI_KINSYU){
				ten -= 1000;
			}
			
			if((b & ISHI_K4X) && (b & ISHI_KINSYU)==0){		// é©ï™ÇÃäàÇRÇ™óLÇÈ
				ten += 10000;
			}
			else if((b & (ISHI_K4|ISHI_K3))==(ISHI_K4|ISHI_K3) && (b & ISHI_KINSYU)==0){	// é©ï™ÇÃÇSÇR
				ten += 10000;
			}
			else if(b & ISHI_S5){				// ëäéËÇÃÇSòA
				ten -= 2000;
			}
			else if(b & ISHI_S4X){				// ëäéËÇÃäàÇR
				ten -= 2000;
			}
			else if(b & ISHI_S44){				// ëäéËÇÃÇSÇS
				ten -= 2000;
			}
			else if((b & (ISHI_S4|ISHI_S3))==(ISHI_S4|ISHI_S3)){	// ëäéËÇÃÇSÇR
				ten -= 2000;
			}
			else if(b & ISHI_S33){				// ëäéËÇÃÇRÇR
				ten -= 1000;
			}
			else if((b & (ISHI_K4|ISHI_K3)) && (b & ISHI_KINSYU)==0){
				ten += 200;
			}
			else if(b & (ISHI_S4|ISHI_S3)){
				ten -= 100;
			}
		}
	}
	else{									// îíî‘
		flagS5 = flagK5 = flagS4X = flagK4X = flagKINSYU = 0;
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & 3){
				continue;
			}
			if(b & ISHI_S5){				// é©ï™Ç…ÇSòAÇ™óLÇ¡ÇΩÇ©ÇÁèüÇø
				return 100000;
			}
			if(b & ISHI_K5){
				flagK5 = i;					// ëäéËÇ…ÇSòAÇ™óLÇ¡ÇΩ
			}
			if(b & ISHI_S4X){
				flagS4X = i;				// é©ï™Ç…äàÇRÇ™óLÇ¡ÇΩ
			}
			if(b & ISHI_K4X){
				flagK4X = i;				// ëäéËÇ…äàÇRÇ™óLÇ¡ÇΩ
			}
		}
		if(flagS4X && flagK5==0){			// é©ï™Ç…äàÇRÇ™óLÇÈÅAëäéËÇ…ÇSòAñ≥Çµ
			return 100000;
		}
		//		ÉmÅ[É}ÉãÇ»ì_êîïtÇØ
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & 3){
				continue;
			}
			if(b & ISHI_KINSYU){
				ten += 1000;
			}
			
			if(b & ISHI_S4X){				// é©ï™ÇÃäàÇRÇ™óLÇÈ
				ten += 10000;
			}
			else if(b & ISHI_S44){			// é©ï™ÇÃÇSÇS
				ten += 10000;
			}
			else if((b & (ISHI_S4|ISHI_S3))==(ISHI_S4|ISHI_S3)){	// é©ï™ÇÃÇSÇR
				ten += 10000;
			}
			else if(b & ISHI_K5){				// ëäéËÇÃÇSòA
				ten -= 2000;
			}
			else if(b & ISHI_K4X){				// ëäéËÇÃäàÇR
				ten -= 2000;
			}
			else if((b & (ISHI_K4|ISHI_K3))==(ISHI_K4|ISHI_K3) && (b & ISHI_KINSYU)==0){
				ten -= 2000;
			}
			else if(b & ISHI_S33){				// é©ï™ÇÃÇRÇR
				ten += 2000;
			}
			else if(b & (ISHI_S4|ISHI_S3)){
				ten += 200;
			}
			else if((b & (ISHI_K4|ISHI_K3)) && (b & ISHI_KINSYU)==0){
				ten -= 100;
			}
		}
	}
    return ten;
}
/********************************************************************************/
/*		makeallmoves															*/
/*		ÇPÅDé©ï™ÇÃÇSòAÇ™óLÇ¡ÇΩÇÁÇTòAÇ…ÇµÇƒèüÇø									*/
/*		ÇQÅDëäéËÇÃÇSòAÇ™óLÇ¡ÇΩÇÁé~ÇﬂÇÈéËÇÃÇ›ï‘Ç∑								*/
/*		    é©ï™Ç™çïÇ≈é~ÇﬂÇÈéËÇ™ã÷éËÇæÇ¡ÇΩÇÁïâÇØ								*/
/*		ÇRÅDé©ï™Ç…äàÇRÇ™óLÇ¡ÇƒëäéËÇ…ÇSòAÇ™ñ≥ÇØÇÍÇŒñ_ÇSÇ…ÇµÇƒèüÇø				*/
/*		ÇSÅDëäéËÇ…äàÇRÇ™óLÇÈÇ∆é~ÇﬂÇÈéËÇ∆ÇSòAÇçÏÇÈéËÇÃÇ›Ç…Ç∑ÇÈ					*/
/*		ÇTÅDéléOÇÃì_ÇçÇÇ≠Ç∑ÇÈ													*/
/********************************************************************************/
static void makeallmoves(int *ban, int k, struct yomumoves *moves, int *poscnt)
{
	int i, b, flagS5, flagK5, flagS4X, flagK4X, flagKINSYU;
	
	*poscnt = 0;
	if(k==ISHI_K){							// çïî‘
		flagS5 = flagK5 = flagS4X = flagK4X = flagKINSYU = 0;
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & 3){
				continue;
			}
			if(b & ISHI_K5){				// é©ï™Ç…ÇSòAÇ™óLÇ¡ÇΩÇ©ÇÁèüÇø
				moves[0].pos = i;
				moves[0].result = 100000;
				*poscnt = 1;
				return;
			}
			if(b & ISHI_KINSYU){			// ã÷éË
				if(b & ISHI_S5){			// ã÷éËÇë≈ÇΩÇ¥ÇÈÇìæÇ»Ç¢
					flagKINSYU = i;
				}
				continue;
			}
			if(b & ISHI_S5){
				flagS5 = i;					// ëäéËÇ…ÇSòAÇ™óLÇ¡ÇΩ
			}
			if(b & ISHI_K4X){
				flagK4X = i;				// é©ï™Ç…äàÇRÇ™óLÇ¡ÇΩ
			}
			if(b & ISHI_S4X){
				flagS4X = i;				// ëäéËÇ…äàÇRÇ™óLÇ¡ÇΩ
			}
		}
		if(flagKINSYU){						// ã÷éËïâÇØ
			for(i=17; i<256; i++){
				if((ban[i] & (3 | ISHI_KINSYU))==0){
					break;
				}
			}
			moves[0].pos = i;				// Ç¢Ç¢â¡å∏Ç»éËÇï‘Ç∑ÅAÇ«Ç§ÇπïâÇØÇæÇ©ÇÁ
			moves[0].result = -100000;
			*poscnt = 1;
			return;
		}
		if(flagS5){							// ëäéËÇ…ÇSòAÇ™óLÇ¡ÇΩÇÃÇ≈é~ÇﬂÇÈéËÇÃÇ›ï‘Ç∑
			moves[0].pos = flagS5;
			moves[0].result = 1000;
			*poscnt = 1;
			return;
		}
		if(flagK4X){						// é©ï™Ç…äàÇRÇ™óLÇÈÅAëäéËÇÃÇSòAÇÕÉ`ÉFÉbÉNçœÇ›
			moves[0].pos = flagK4X;
			moves[0].result = 100000;
			*poscnt = 1;
			return;
		}
		if(flagS4X){						// ëäéËÇ…äàÇRÇ™óLÇÈ
			for(i=17; i<256; i++){
				b = ban[i];
				if(b & (3 | ISHI_KINSYU)){
					continue;
				}
				else if((b & (ISHI_K4|ISHI_K3))==(ISHI_K4|ISHI_K3)){	// é©ï™ÇÃÇSÇR
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 220;
				}
				else if(b & ISHI_S4X){				// ëäéËÇÃäàÇRÇé~ÇﬂÇÈéË
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 210;
				}
				else if(b & (ISHI_K4|ISHI_K4X)){	// é©ï™ÇÃÇSÇçÏÇÈéË
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 200;
				}
			}
		}
		else{
			//		ÉmÅ[É}ÉãÇ»ì_êîïtÇØ
			for(i=17; i<256; i++){
				b = ban[i];
				if(b & (3 | ISHI_KINSYU)){
					continue;
				}
				else if((b & (ISHI_K4|ISHI_K3))==(ISHI_K4|ISHI_K3)){	// é©ï™ÇÃÇSÇR
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 80;
				}
				else if(b & ISHI_S44){				// ëäéËÇÃÇSÇS
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 75;
				}
				else if((b & (ISHI_S4|ISHI_S3))==(ISHI_S4|ISHI_S3)){	// ëäéËÇÃÇSÇR
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 70;
				}
				else if(b & ISHI_S33){				// ëäéËÇ…ÇRÇRÇçÏÇÁÇπÇ»Ç¢
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 60;
				}
				else if(b & ISHI_K3){				// é©ï™ÇÃÇRÇçÏÇÈ
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 50;
				}
				else if(b & ISHI_K4){				// é©ï™ÇÃÇSÇçÏÇÈ
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 40;
				}
				else if(b & ISHI_S4){				// ëäéËÇ…ÇSÇçÏÇÁÇπÇ»Ç¢
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 30;
				}
				else if(b & ISHI_S3){				// ëäéËÇ…ÇRÇçÏÇÁÇπÇ»Ç¢
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 20;
				}
				else if(b & ISHI_K2){
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 10;
				}
				else if(b & ISHI_S2){
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 5;
				}
			}
		}
	}
	else{									// îíî‘
		flagS5 = flagK5 = flagS4X = flagK4X = flagKINSYU = 0;
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & 3){
				continue;
			}
			if(b & ISHI_S5){				// é©ï™Ç…ÇSòAÇ™óLÇ¡ÇΩÇ©ÇÁèüÇø
				moves[0].pos = i;
				moves[0].result = 100000;
				*poscnt = 1;
				return;
			}
			if(b & ISHI_K5){
				flagK5 = i;					// ëäéËÇ…ÇSòAÇ™óLÇ¡ÇΩ
			}
			if(b & ISHI_S4X){
				flagS4X = i;				// é©ï™Ç…äàÇRÇ™óLÇ¡ÇΩ
			}
			if(b & ISHI_K4X){
				flagK4X = i;				// ëäéËÇ…äàÇRÇ™óLÇ¡ÇΩ
			}
		}
		if(flagK5){							// ëäéËÇ…ÇSòAÇ™óLÇ¡ÇΩÇÃÇ≈é~ÇﬂÇÈéËÇÃÇ›ï‘Ç∑
			moves[0].pos = flagK5;
			moves[0].result = 1000;
			*poscnt = 1;
			return;
		}
		if(flagS4X){						// é©ï™Ç…äàÇRÇ™óLÇÈÅAëäéËÇÃÇSòAÇÕÉ`ÉFÉbÉNçœÇ›
			moves[0].pos = flagS4X;
			moves[0].result = 100000;
			*poscnt = 1;
			return;
		}
		if(flagK4X){						// ëäéËÇ…äàÇRÇ™óLÇÈ
			for(i=17; i<256; i++){
				b = ban[i];
				if(b & 3){
					continue;
				}
				else if((b & (ISHI_S4|ISHI_S3))==(ISHI_S4|ISHI_S3)){	// é©ï™ÇÃÇSÇR
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 220;
				}
				else if(b & ISHI_K4X){				// ëäéËÇÃäàÇRÇé~ÇﬂÇÈéË
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 210;
				}
				else if(b & (ISHI_S4|ISHI_S4X)){	// é©ï™ÇÃÇSÇçÏÇÈéË
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 200;
				}
			}
		}
		else{
			//		ÉmÅ[É}ÉãÇ»ì_êîïtÇØ
			for(i=17; i<256; i++){
				b = ban[i];
				if(b & 3){
					continue;
				}
				else if(b & ISHI_S44){				// é©ï™ÇÃÇSÇS ëäéËÇ…ÇÕÇSÇÕñ≥Ç¢
					moves[0].pos    = i;
					moves[0].result = 100000;
					*poscnt = 1;
					return;
				}
				else if((b & (ISHI_S4|ISHI_S3))==(ISHI_S4|ISHI_S3)){	// é©ï™ÇÃÇSÇR
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 80;
				}
				else if((b & (ISHI_K4|ISHI_K3))==(ISHI_K4|ISHI_K3) && (b & ISHI_KINSYU)==0){
					moves[(*poscnt)].pos      = i;	// ëäéËÇÃÇSÇR
					moves[(*poscnt)++].result = 70;
				}
				else if(b & ISHI_S33){				// é©ï™ÇÃÇRÇR
					moves[(*poscnt)].pos      = i;
					moves[(*poscnt)++].result = 60;
				}
				else if((b & ISHI_KINSYU)==0){
					if(b & ISHI_S3){				// é©ï™ÇÃÇRÇçÏÇÈ
						moves[(*poscnt)].pos      = i;
						moves[(*poscnt)++].result = 50;
					}
					else if(b & ISHI_S4){				// é©ï™ÇÃÇSÇçÏÇÈ
						moves[(*poscnt)].pos      = i;
						moves[(*poscnt)++].result = 40;
					}
					else if(b & ISHI_K4){				// ëäéËÇ…ÇSÇçÏÇÁÇπÇ»Ç¢
						moves[(*poscnt)].pos      = i;
						moves[(*poscnt)++].result = 30;
					}
					else if(b & ISHI_K3){				// ëäéËÇ…ÇRÇçÏÇÁÇπÇ»Ç¢
						moves[(*poscnt)].pos      = i;
						moves[(*poscnt)++].result = 20;
					}
					else if(b & ISHI_S2){
						moves[(*poscnt)].pos      = i;
						moves[(*poscnt)++].result = 10;
					}
					else if(b & ISHI_K2){
						moves[(*poscnt)].pos      = i;
						moves[(*poscnt)++].result = 5;
					}
				}
				else{									// ëäéËÇ™ã÷éË
					if(b & (ISHI_K4|ISHI_K3|ISHI_K2|ISHI_S4|ISHI_S3|ISHI_S2)){
						moves[(*poscnt)].pos      = i;
						moves[(*poscnt)++].result = 3;
					}
				}
			}
		}
	}
}
/********************************************************************************/
/*		kosyu																	*/
/*		top åƒéËÇ2å¬ÇæÇØì_êîèCê≥Ç∑ÇÈ ì_êî 500, 490								*/
/********************************************************************************/
int kosyu_eva(int *ban1, int k)			// 3òA 4òAÇçÏÇÍÇÈêîÇÉJÉEÉìÉgÇ∑ÇÈ
{
	int i, ten;
	
	ten = 0;
	for(i=17; i<256; i++){
		if(ban1[i] & ISHI_KINSYU){
			ten -= 10;
			continue;
		}
		if(ban1[i] & (ISHI_K3 | ISHI_K4 | ISHI_K4X | ISHI_K5)){
			ten++;
		}
		if(ban1[i] & (ISHI_S3 | ISHI_S33 | ISHI_S4 | ISHI_S4X | ISHI_S5)){
			ten--;
		}
	}
	if(k==ISHI_K)
		return ten;
	return -ten;
}

void kosyu(int *ban, int k, struct yomumoves *yomumoves, int poscnt)
{
	int ban1[274];
	int i, ten, top_i, top_ten, second_i, second_ten;
	
	//	200 ì_à»è„ñîÇÕ - ì_Ç™óLÇÍÇŒåƒéËÇÕë≈ÇΩÇ»Ç¢
	for(i=0; i<poscnt; i++){
		if(yomumoves[i].result >= 200){	// äàÇRà»è„Ç™óLÇ¡ÇΩÇ©ÇÁåƒéËÇÕë≈ÇΩÇ»Ç¢
			return;
		}
		if(yomumoves[i].result < 0){
			return;
		}
	}
	
	top_i = top_ten = second_i = second_ten = -100;
	for(i=0; i< poscnt; i++){
		cpyban1(ban1, ban);
		ban1[yomumoves[i].pos] = k;
		MCupdateban(ban1, yomumoves[i].pos);
		ten = kosyu_eva(ban1, k);
		if(ten > top_ten){
			second_ten = top_ten;
			second_i   = top_i;
			top_ten    = ten;
			top_i      = i;
		}
		else if(ten > second_ten){
			second_ten = ten;
			second_i   = i;
		}
	}
	if(top_i >= 0){
		yomumoves[top_i].result = 500;
	}
	if(second_i >= 0){
		yomumoves[second_i].result = 490;
	}
}
/********************************************************************************/
/*		MCyomu																	*/
/********************************************************************************/
#define TEN_INFINIT	100000000

static int MCcompk(const void *elem1, const void *elem2)
{
	return ((struct yomumoves *)elem2)->result - ((struct yomumoves *)elem1)->result;
}

static int MCyomuk(int *ban, int k, int *pos, int depth, int alpha, int beta, int buf_ptr)
{
	int i, max, ret, mv, poscnt;
	int ban1[274];
	struct yomumoves *ymoves = (struct yomumoves*)&gv->buf[buf_ptr];
	int s = macroinv(k);
	
	if(depth >= yomiend)
		return judge(ban, k);
	makeallmoves(ban, k, gv->yomumoves, &poscnt);
	if(depth < 2){
		kosyu(ban, k, gv->yomumoves, poscnt);
	}
	memcpy(ymoves, gv->yomumoves, poscnt*sizeof(struct yomumoves));
	qsort(ymoves, poscnt, sizeof(struct yomumoves), MCcompk);
	if(poscnt >= 6){
		ret = ymoves[5].result;
		for(i=6; i<poscnt; i++){
			if(ret > ymoves[i].result){
				break;
			}
		}
		poscnt = i;
	}
	max = -TEN_INFINIT;
	for(i=0; i<poscnt; i++){
		if(ymoves[i].result==100000){		// 5òAäÆê¨
			*pos = ymoves[i].pos;
			return 100000;
		}
		if(ymoves[i].result < 0){			// ã÷éËïâÇØ
			*pos = ymoves[i].pos;
			return -100000;
		}
		cpyban1(ban1, ban);
		ban1[ymoves[i].pos] = k;
		MCupdateban(ban1, ymoves[i].pos);

		ret = -MCyomuk(ban1, s, &mv, depth+1, -beta, -alpha, buf_ptr+poscnt*2);

		if(ret >= beta){
			return (ret+1);
		}
		if(max < ret){
			max = ret;
			*pos = ymoves[i].pos;
			if(max > alpha)
				alpha = max;
		}
	}
	return max;
}


static int MCyomu(int *ban, int k, int *pos)
{
	int ret, p, tmp;
	
	for(p=17; p<256; p++){
		tmp = ban[p] & 3;
		if(tmp!=ISHI_O && tmp!=ISHI_N){
			break;
		}
	}
	if(p==256){								// êŒÇ»Çµ
		*pos = macropos(8,8);
		return 0;
	}

    *pos = MCjoseki(ban);
    if(*pos){
        if((ban[*pos]&3)==0){
			return 0;
        }
    }
    
	//		í èÌì«Ç›
	MCmakeban(ban);
	
//	yomiend = 5;
	*pos = 0;
	
	ret = MCyomuk(ban, k, pos, 0, -TEN_INFINIT, TEN_INFINIT, 0);
	
	if(k != ISHI_K)
		ret = -ret;
	return ret;
}
/********************************************************************************/
/*		dispban																	*/
/********************************************************************************/
static void dispban_null()
{
	int x, y;
	
	//		îwåiêFìhÇËÇ¬Ç‘Çµ
	lcd_clear_square(0,0, 16*15, 16*15, RGB(255,192,64));
	
	//		ècê¸
	for(x=1; x<=15; x++){
		lcd_clear_square((x-1)*16+8, 8, 1, 16*15-16, BLACK);
	}
	
	//		â°ê¸
	for(y=1; y<=15; y++){
		lcd_clear_square(8, (y-1)*16+8, 16*15-16, 1, BLACK);
	}
	
	//		êØ
	for(y=4; y<=12; y+=4){
		for(x=4; x<=12; x+=4){
			lcd_clear_square((x-1)*16+6, (y-1)*16+6, 5, 5, BLACK);
		}
	}
}

static void disp_koma_K(int x, int y)
{
	lcd_clear_square((x-1)*16, (y-1)*16, 16,16, ban_color);
	circlefill((x-1)*16+8, (y-1)*16+8, 7, BLACK);
}

static void disp_koma_S(int x, int y)
{
	lcd_clear_square((x-1)*16, (y-1)*16, 16,16, ban_color);
	circlefill((x-1)*16+8, (y-1)*16+8, 7, WHITE);
	circle1((x-1)*16+8, (y-1)*16+8, 7, BLACK);
}
/********************************************************************************/
/*		human_input																*/
/********************************************************************************/
int human_input()
{
	int x, y;
	
	for(;;){
		mouse_updown(&x, &y);
		x = x/16+1;
		y = y/16+1;
		if(x<1 || x>15 || y<1 || y>15){
			continue;
		}
		return macropos(x,y);
	}
}
/********************************************************************************/
/*		game																	*/
/********************************************************************************/
static void game()
{
	int pos, pos_mae, player, ten, x, y;
	char buf[20];
	
	lcd_clear(WHITE);
	dispban_null();							// ãÛÇÃî’Çï`Ç≠
	if(yomiend==5){
		lcd_disp_str_x1(240, 48, "Strong ");
	}
	else{
		lcd_disp_str_x1(240, 48, "Weak   ");
	}
	initban(goban);
	teban = ISHI_K;
	tekazu = 1;
	pos = pos_mae = 0;
	ten = 0;
	for(;;){
		//		ÇPéËëOÇÃéËÇÃÉJÅ[É\ÉãÇè¡Ç∑
		if(pos_mae){
			ban_color = RGB(255,192,64);
			if(teban==ISHI_K){
				disp_koma_K(macrox(pos_mae), macroy(pos_mae));
			}
			else{
				disp_koma_S(macrox(pos_mae), macroy(pos_mae));
			}
		}
		//		ç°âÒÇÃéËÇÉJÅ[É\ÉãïtÇ´Ç≈ï\é¶Ç∑ÇÈ
		if(pos){
			ban_color = CYAN;
			if(teban==ISHI_S){
				disp_koma_K(macrox(pos), macroy(pos));
			}
			else{
				disp_koma_S(macrox(pos), macroy(pos));
			}
		}
        pos_mae = pos;
		sprintf(buf, "%d", ten);
		lcd_disp_str_x1(240,240-16, buf);
		player = teban==ISHI_K ? black : white;
		if(player==0){						// cpu
			lcd_disp_str_x1(240,16,"CPU   ");
			ten = MCyomu(goban, teban, &pos);
			
//			buzzer(1000, 50);
			MCmakeban(goban);
			//		èüóòîªíË
			ban_color = CYAN;
			if(teban==ISHI_K && (goban[pos] & ISHI_K5)){
				disp_koma_K(macrox(pos), macroy(pos));
//				buzzer(500,1000);
				for(;;)
					;
			}
			if(teban==ISHI_S && (goban[pos] & ISHI_S5)){
				disp_koma_S(macrox(pos), macroy(pos));
//				buzzer(500,1000);
				for(;;)
					;
			}
			goban[pos] = teban;
		}
		else{								// human
			lcd_disp_str_x1(240,16,"human ");
			MCmakeban(goban);
			for(;;){
				pos = human_input();
				if(goban[pos]&3){
//					buzzer(500, 1000);
					continue;
				}
				if(teban==ISHI_K && (goban[pos]&ISHI_KINSYU) && (goban[pos]&ISHI_K5)==0){
//					buzzer(500, 1000);
					continue;
				}
				if(tekazu==1 && pos!=macropos(8,8)){
//					buzzer(500, 1000);
					continue;
				}
				break;
			}
			if((teban==ISHI_K && (goban[pos]&ISHI_K5)) || (teban==ISHI_S && (goban[pos]&ISHI_S5))){
//				buzzer(1000, 1000);
				lcd_cdisp_str_x2(100, "±≈¿…∂¡");
				mouse_updown(&x, &y);
			}
			goban[pos] = teban;
		}
		tekazu++;
		teban = macroinv(teban);
	}
}
/********************************************************************************/
/*		gomoku_main																*/
/********************************************************************************/
static const struct menu gomoku_menu[]={
	{0,  0,25, 1,"Black:cpu,   White:cpu"},
	{0, 60,25, 2,"Black:human, White:human"},
	{0,120,25, 3,"Black:cpu,   white:human"},
	{0,180,25, 4,"Black:hunam, White:cpu"},
	{0,0,0,0,0}
};

static const struct menu level_menu[]={
	{0, 60, 10,  5, "Strong"},
	{0,120, 10,  2, "Weak"},
	{0,0,0,0,0}
};


void gomoku_main()
{
	int x, y, cmd;
	
	lcd_clear(CYAN_D);
	lcd_cdisp_str_x2( 90, "∫ﬁ”∏≈◊Õﬁ");
	lcd_cdisp_str_x1(130, "K.Fukumoto");
	mouse_updown(&x, &y);
	
	lcd_clear(BLACK);
	btn_back_color = GRAY;
	for(;;){
		cmd = menu_select0(gomoku_menu);
		switch(cmd){
			case 1:							// cpu - cpu
				black = white = 0;
				break;
			case 2:							// human - human
				black = white = 1;
				break;
			case 3:							// cpu black
				black = 0;
				white = 1;
				break;
			case 4:							// cpu white
				black = 1;
				white = 0;
				break;
			default:
				continue;
		}
		break;
	}
	
	lcd_clear(BLACK);
	for(;;){
		yomiend = menu_select0(level_menu);
		if(yomiend > 0){
			break;
		}
	}
	
	game();
}
/********************************************************************/
/*		zyocnv1s		                                            */
/********************************************************************/
#define BANSIZE	15

static int  zyocnv1(int cnvflg, int pos)
{
    int  x, y, tmp;

    x = macrox(pos);
    y = macroy(pos);
    if(x<0 || x>BANSIZE || y<0 || y>BANSIZE)
    	return pos;

    if(cnvflg & 4)
		y = (BANSIZE+1)-y;
    if(cnvflg & 2)
		x = (BANSIZE+1)-x;
    if(cnvflg & 1){
		tmp = x;
		x   = y;
		y   = tmp;
    }
    return(macropos(x,y));
}
/****************************************************************************/
/*		zyocnv2                                                             */
/*		íËêŒî’à íuÇé¿î’à íuÇ…ïœä∑Ç∑ÇÈ										*/
/****************************************************************************/
static int  zyocnv2(int cnvflg, int pos)
{
    int  x, y, tmp;

    x = macrox(pos);
    y = macroy(pos);
    if(x<0 || x>BANSIZE || y<0 || y>BANSIZE)
    	return pos;

    if(cnvflg & 1){
		tmp = x;
		x   = y;
		y   = tmp;
    }
    if(cnvflg & 2)
		x = (BANSIZE+1) - x;
    if(cnvflg & 4)
		y = (BANSIZE+1) - y;
    return(macropos(x,y));
}
/***************************************************************************/
/*		zyoban1                                                            */
/***************************************************************************/
static void zyoban1(int cnvflg, int ban1[], int ban2[])
{
    int  pos;

    cpyban1(ban1, ban2);
    for(pos=17; pos<256; pos++)
		ban1[zyocnv1(cnvflg, pos)] = ban2[pos];
}
/********************************************************************************/
/*		MCjoseki																*/
/********************************************************************************/
#define MCrndx		MCrnd(100)

static int MCjoseki(int *ban)
{
	int ban1[274], i, cnt, cnvflg, tmp;
	
	cnt = 0;
	for(i=0; i<274; i++){
		if(ban[i]==ISHI_K || ban[i]==ISHI_S)
			cnt++;
	}
	for(cnvflg=0; cnvflg<8; cnvflg++){
		zyoban1(cnvflg, ban1, ban);
		if(ban1[macropos(8,8)]==ISHI_K){	/* ìVå≥Ç…çïóL						*/
			if(cnt==1){					/* ìVå≥ÇÃêŒÇÃÇ›ÇæÇ¡ÇΩ					*/
				if((tmp=MCrndx)<50)
					return zyocnv2(cnvflg, macropos(9,7));
				else
					return zyocnv2(cnvflg, macropos(8,7));
			}

			//Ñ©Ñ©Ñ©Ñ©Ñ©
			//Ñ©Ñ©Ñ©ÅõÑ©
			//Ñ©Ñ©ÅúÑ©Ñ©
			//Ñ©Ñ©Ñ©Ñ©Ñ©
			//Ñ©Ñ©Ñ©Ñ©Ñ©
			if(ban1[macropos(9,7)]==ISHI_S){	/* ä‘ê⁄Ç«Çﬂ						*/
				if(cnt==2){
//					if(MCrndx < 70)
						return zyocnv2(cnvflg, macropos(9,9));
//					else
//						return 0;
				}

				//Ñ©Ñ©Ñ©Ñ©Ñ©
				//Ñ©Ñ©Ñ©ÅõÑ©
				//Ñ©Ñ©ÅúÅúÑ©
				//Ñ©Ñ©Ñ©Ñ©Ñ©
				//Ñ©Ñ©Ñ©Ñ©Ñ©
				if(ban1[macropos(9,8)]==ISHI_K){
					if(cnt==3){
						tmp = MCrndx;
						if(tmp < 40)
							return zyocnv2(cnvflg, macropos(8,9));
						else if(tmp < 70)
							return zyocnv2(cnvflg, macropos(10,8));
						else
							return zyocnv2(cnvflg, macropos(7,8));
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©ÅõÑ©
					//Ñ©Ñ©ÅúÅúÅõ
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(10,8)]==ISHI_S){

						//Ñ©Ñ©ÅúÑ©Ñ©
						//Ñ©Ñ©Ñ©ÅõÑ©
						//Ñ©Ñ©ÅúÅúÅõ
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(8,6)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(8,9));
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©ÅõÑ©
					//Ñ©ÅõÅúÅúÑ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(7,8)]==ISHI_S){

						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©ÅõÅú
						//Ñ©ÅõÅúÅúÑ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(10,7)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(10,9));
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©ÅõÑ©
					//Ñ©Ñ©ÅúÅúÑ©
					//Ñ©Ñ©ÅõÑ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(8,9)]==ISHI_S){

						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©ÅõÑ©
						//Ñ©ÅúÅúÅúÑ©
						//Ñ©Ñ©ÅõÑ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(7,8)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(6,8));
						}
					}
				}

				//Ñ©Ñ©Ñ©Ñ©Ñ©
				//Ñ©Ñ©Ñ©ÅõÑ©
				//Ñ©Ñ©ÅúÑ©Ñ©
				//Ñ©Ñ©Ñ©ÅúÑ©
				//Ñ©Ñ©Ñ©Ñ©Ñ©
				if(ban1[macropos(9,9)]==ISHI_K){	/* âYåéíËêŒ					*/
					if(cnt==3){
						tmp = MCrndx;
						if(tmp < 40)
							return zyocnv2(cnvflg, macropos(7,7));
						else if(tmp < 60)
							return zyocnv2(cnvflg, macropos(10,10));
						else if(tmp < 80)
							return zyocnv2(cnvflg, macropos(8,9));
						else
							return zyocnv2(cnvflg, macropos(8,10));
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©ÅõÑ©ÅõÑ©
					//Ñ©Ñ©ÅúÑ©Ñ©
					//Ñ©Ñ©Ñ©ÅúÑ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(7,7)]==ISHI_S){
						if(cnt==4){
							if(MCrndx < 70)
								return zyocnv2(cnvflg, macropos(10,9));
							else
								return zyocnv2(cnvflg, macropos(10,8));
						}

						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©ÅõÑ©ÅõÑ©
						//Ñ©Ñ©ÅúÑ©Ñ©
						//Ñ©Ñ©Ñ©ÅúÅú
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(10,9)]==ISHI_K){
							if(cnt==5){
								tmp = MCrndx;
								if(tmp < 20)
									return zyocnv2(cnvflg, macropos(7,8));
								else if(tmp < 40)
									return zyocnv2(cnvflg, macropos(8,9));
								else if(tmp < 60)
									return zyocnv2(cnvflg, macropos(10,7));
								else if(tmp < 80)
									return zyocnv2(cnvflg, macropos(11,9));
								else
									return zyocnv2(cnvflg, macropos(8,6));
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©ÅõÑ©ÅõÑ©
							//Ñ©ÅõÅúÑ©Ñ©
							//Ñ©Ñ©Ñ©ÅúÅú
							//Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(7,8)]==ISHI_S){

								//Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©ÅõÑ©ÅõÅú
								//Ñ©ÅõÅúÑ©Ñ©
								//Ñ©Ñ©Ñ©ÅúÅú
								//Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(10,7)]==ISHI_K){
									if(cnt==7){
										if(MCrndx < 70)
											return zyocnv2(cnvflg, macropos(7,9));
										else
											return 0;
									}

									//Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©ÅõÑ©ÅõÅú
									//Ñ©ÅõÅúÑ©Ñ©
									//Ñ©ÅõÑ©ÅúÅú
									//Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(7,9)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(7,6));

										//Ñ©ÅúÑ©Ñ©Ñ©
										//Ñ©ÅõÑ©ÅõÅú
										//Ñ©ÅõÅúÑ©Ñ©
										//Ñ©ÅõÑ©ÅúÅú
										//Ñ©Ñ©Ñ©Ñ©Ñ©
										if(ban1[macropos(7,6)]==ISHI_K){

											//Ñ©ÅúÑ©Ñ©Ñ©
											//Ñ©ÅõÑ©ÅõÅú
											//Ñ©ÅõÅúÑ©Ñ©
											//Ñ©ÅõÑ©ÅúÅú
											//Ñ©ÅõÑ©Ñ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©Ñ©
											if(ban1[macropos(7,10)]==ISHI_S){

												//Ñ©ÅúÑ©Ñ©Ñ©
												//Ñ©ÅõÑ©ÅõÅú
												//Ñ©ÅõÅúÑ©Ñ©
												//Ñ©ÅõÑ©ÅúÅú
												//Ñ©ÅõÑ©Ñ©Ñ©
												//Ñ©ÅúÑ©Ñ©Ñ©
												if(ban1[macropos(7,11)]==ISHI_K){

													//Ñ©ÅúÑ©Ñ©Ñ©
													//Ñ©ÅõÅõÅõÅú
													//Ñ©ÅõÅúÑ©Ñ©
													//Ñ©ÅõÑ©ÅúÅú
													//Ñ©ÅõÑ©Ñ©Ñ©
													//Ñ©ÅúÑ©Ñ©Ñ©
													if(ban1[macropos(8,7)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(10,11));
													}
												}
											}
										}
									}

									//Ñ©ÅõÑ©Ñ©Ñ©
									//Ñ©ÅõÑ©ÅõÅú
									//Ñ©ÅõÅúÑ©Ñ©
									//Ñ©Ñ©Ñ©ÅúÅú
									//Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(7,6)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(7,9));
									}
								}
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©ÅõÑ©ÅõÅõ
							//Ñ©Ñ©ÅúÑ©Ñ©
							//Ñ©Ñ©Ñ©ÅúÅú
							//Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(10,7)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(8,7));

								//Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©ÅõÅúÅõÅõ
								//Ñ©Ñ©ÅúÑ©Ñ©
								//Ñ©Ñ©Ñ©ÅúÅú
								//Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(8,7)]==ISHI_K){

									//Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©ÅõÅúÅõÅõ
									//Ñ©Ñ©ÅúÅõÑ©
									//Ñ©Ñ©Ñ©ÅúÅú
									//Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(9,8)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,10));
									}
								}
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©ÅõÑ©ÅõÑ©Ñ©
							//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©ÅúÅúÅõ
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(11,9)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(8,7));

								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©ÅõÅúÅõÑ©Ñ©
								//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©ÅúÅúÅõ
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(8,7)]==ISHI_K){

									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©ÅõÅúÅõÑ©Ñ©
									//Ñ©Ñ©ÅúÅõÑ©Ñ©
									//Ñ©Ñ©Ñ©ÅúÅúÅõ
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(9,8)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(10,8));

										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©ÅõÅúÅõÑ©Ñ©
										//Ñ©Ñ©ÅúÅõÅúÑ©
										//Ñ©Ñ©Ñ©ÅúÅúÅõ
										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										if(ban1[macropos(10,8)]==ISHI_K){

											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©ÅõÅúÅõÑ©Ñ©
											//Ñ©Ñ©ÅúÅõÅúÑ©
											//Ñ©Ñ©Ñ©ÅúÅúÅõ
											//Ñ©Ñ©Ñ©Ñ©ÅõÑ©
											if(ban1[macropos(10,10)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(8,9));
											}
										}
									}

									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©ÅõÅúÅõÑ©Ñ©
									//Ñ©Ñ©ÅúÑ©ÅõÑ©
									//Ñ©Ñ©Ñ©ÅúÅúÅõ
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(10,8)]==ISHI_S){

										//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
										//Ñ©ÅõÅúÅõÑ©Ñ©
										//Ñ©Ñ©ÅúÑ©ÅõÑ©
										//Ñ©Ñ©Ñ©ÅúÅúÅõ
										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										if(ban1[macropos(8,6)]==ISHI_K){

											//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
											//Ñ©ÅõÅúÅõÑ©Ñ©
											//Ñ©Ñ©ÅúÑ©ÅõÑ©
											//Ñ©Ñ©ÅõÅúÅúÅõ
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											if(ban1[macropos(8,9)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(7,6));
											}
										}
									}
								}
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©ÅõÑ©ÅõÑ©
							//Ñ©Ñ©ÅúÑ©Ñ©
							//Ñ©Ñ©ÅõÅúÅú
							//Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(8,9)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,8));

								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©ÅõÑ©ÅõÑ©Ñ©Ñ©
								//Ñ©Ñ©ÅúÑ©ÅúÑ©Ñ©
								//Ñ©Ñ©ÅõÅúÅúÑ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(10,8)]==ISHI_K){

									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©ÅõÑ©ÅõÅõÑ©Ñ©
									//Ñ©Ñ©ÅúÑ©ÅúÑ©Ñ©
									//Ñ©Ñ©ÅõÅúÅúÑ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(10,7)]==ISHI_S){

										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©ÅõÑ©ÅõÅõÅúÑ©
										//Ñ©Ñ©ÅúÑ©ÅúÑ©Ñ©
										//Ñ©Ñ©ÅõÅúÅúÑ©Ñ©
										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										if(ban1[macropos(11,7)]==ISHI_K){

											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
											//Ñ©ÅõÑ©ÅõÅõÅúÑ©
											//Ñ©Ñ©ÅúÑ©ÅúÑ©Ñ©
											//Ñ©Ñ©ÅõÅúÅúÑ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											if(ban1[macropos(12,6)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(9,8));

												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
												//Ñ©ÅõÑ©ÅõÅõÅúÑ©
												//Ñ©Ñ©ÅúÅúÅúÑ©Ñ©
												//Ñ©Ñ©ÅõÅúÅúÑ©Ñ©
												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
												if(ban1[macropos(9,8)]==ISHI_K){

													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
													//Ñ©ÅõÑ©ÅõÅõÅúÑ©
													//Ñ©Ñ©ÅúÅúÅúÅõÑ©
													//Ñ©Ñ©ÅõÅúÅúÑ©Ñ©
													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
													if(ban1[macropos(11,8)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(10,10));
													}
												}
											}
										}
									}
								}
							}

							//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©
							//Ñ©ÅõÑ©ÅõÑ©Ñ©Ñ©
							//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©ÅúÅúÑ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(8,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(11,9));

								//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©
								//Ñ©ÅõÑ©ÅõÑ©Ñ©Ñ©
								//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©ÅúÅúÅúÑ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(11,9)]==ISHI_K){

									//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©
									//Ñ©ÅõÑ©ÅõÑ©Ñ©Ñ©
									//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©ÅúÅúÅúÅõ
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(12,9)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,10));
									}

									//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©
									//Ñ©ÅõÑ©ÅõÑ©Ñ©Ñ©
									//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©ÅõÅúÅúÅúÑ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(8,9)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(11,11));
									}
								}
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©ÅõÑ©ÅõÑ©
							//ÅõÑ©ÅúÑ©Ñ©
							//Ñ©Ñ©Ñ©ÅúÅú
							//Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(6,8)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,8));
							}
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©ÅõÑ©
					//Ñ©Ñ©ÅúÑ©Ñ©
					//Ñ©Ñ©Ñ©ÅúÑ©
					//Ñ©Ñ©ÅõÑ©Ñ©
					if(ban1[macropos(8,10)]==ISHI_S){
						if(cnt==4)
							return zyocnv2(cnvflg, macropos(7,7));

						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©ÅúÑ©ÅõÑ©
						//Ñ©Ñ©ÅúÑ©Ñ©
						//Ñ©Ñ©Ñ©ÅúÑ©
						//Ñ©Ñ©ÅõÑ©Ñ©
						if(ban1[macropos(7,7)]==ISHI_K){

							//ÅõÑ©Ñ©Ñ©Ñ©
							//Ñ©ÅúÑ©ÅõÑ©
							//Ñ©Ñ©ÅúÑ©Ñ©
							//Ñ©Ñ©Ñ©ÅúÑ©
							//Ñ©Ñ©ÅõÑ©Ñ©
							if(ban1[macropos(6,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(7,8));
							}
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©ÅõÑ©
					//Ñ©Ñ©ÅúÑ©Ñ©
					//Ñ©Ñ©ÅõÅúÑ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(8,9)]==ISHI_S){
						if(cnt==4)
							return zyocnv2(cnvflg, macropos(7,7));

						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©ÅúÑ©ÅõÑ©
						//Ñ©Ñ©ÅúÑ©Ñ©
						//Ñ©Ñ©ÅõÅúÑ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(7,7)]==ISHI_K){

							//ÅõÑ©Ñ©Ñ©Ñ©
							//Ñ©ÅúÑ©ÅõÑ©
							//Ñ©Ñ©ÅúÑ©Ñ©
							//Ñ©Ñ©ÅõÅúÑ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(6,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,10));

								//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©ÅúÑ©ÅõÑ©Ñ©
								//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
								//Ñ©Ñ©ÅõÅúÑ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(10,10)]==ISHI_K){

									//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©ÅúÑ©ÅõÑ©Ñ©
									//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
									//Ñ©Ñ©ÅõÅúÑ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
									if(ban1[macropos(11,11)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,10));

										//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©ÅúÑ©ÅõÑ©Ñ©
										//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
										//Ñ©Ñ©ÅõÅúÑ©Ñ©
										//Ñ©Ñ©ÅúÑ©ÅúÑ©
										//Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
										if(ban1[macropos(8,10)]==ISHI_K){
											if(cnt==9)
												return zyocnv2(cnvflg, macropos(7,11));

											//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©ÅúÑ©ÅõÑ©Ñ©
											//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
											//Ñ©Ñ©ÅõÅúÑ©Ñ©
											//Ñ©Ñ©ÅúÑ©ÅúÑ©
											//Ñ©ÅõÑ©Ñ©Ñ©Åõ
											if(ban1[macropos(7,11)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(9,11));

												//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©ÅúÑ©ÅõÑ©Ñ©
												//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
												//Ñ©Ñ©ÅõÅúÑ©Ñ©
												//Ñ©Ñ©ÅúÑ©ÅúÑ©
												//Ñ©ÅõÑ©ÅúÑ©Åõ
												if(ban1[macropos(9,11)]==ISHI_K){

													//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©ÅúÑ©ÅõÑ©Ñ©
													//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
													//Ñ©Ñ©ÅõÅúÑ©Ñ©
													//Ñ©Ñ©ÅúÅõÅúÑ©
													//Ñ©ÅõÑ©ÅúÑ©Åõ
													if(ban1[macropos(9,10)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(10,8));

														//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
														//Ñ©ÅúÑ©ÅõÑ©Ñ©
														//Ñ©Ñ©ÅúÑ©ÅúÑ©
														//Ñ©Ñ©ÅõÅúÑ©Ñ©
														//Ñ©Ñ©ÅúÅõÅúÑ©
														//Ñ©ÅõÑ©ÅúÑ©Åõ
														if(ban1[macropos(10,8)]==ISHI_K){

															//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
															//Ñ©ÅúÑ©ÅõÑ©Ñ©
															//Ñ©ÅõÅúÑ©ÅúÑ©
															//Ñ©Ñ©ÅõÅúÑ©Ñ©
															//Ñ©Ñ©ÅúÅõÅúÑ©
															//Ñ©ÅõÑ©ÅúÑ©Åõ
															if(ban1[macropos(7,8)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(10,11));

																//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																//Ñ©ÅúÑ©ÅõÑ©Ñ©
																//Ñ©ÅõÅúÑ©ÅúÑ©
																//Ñ©Ñ©ÅõÅúÑ©Ñ©
																//Ñ©Ñ©ÅúÅõÅúÑ©
																//Ñ©ÅõÑ©ÅúÅúÅõ
																if(ban1[macropos(10,11)]==ISHI_K){

																	//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																	//Ñ©ÅúÑ©ÅõÑ©Ñ©
																	//Ñ©ÅõÅúÑ©ÅúÑ©
																	//Ñ©Ñ©ÅõÅúÅõÑ©
																	//Ñ©Ñ©ÅúÅõÅúÑ©
																	//Ñ©ÅõÑ©ÅúÅúÅõ
																	if(ban1[macropos(10,9)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(12,8));
																	}
																}
															}
														}
													}

													//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©ÅúÑ©ÅõÑ©Ñ©
													//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
													//Ñ©ÅõÅõÅúÑ©Ñ©
													//Ñ©Ñ©ÅúÑ©ÅúÑ©
													//Ñ©ÅõÑ©ÅúÑ©Åõ
													if(ban1[macropos(7,9)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(9,8));

														//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
														//Ñ©ÅúÑ©ÅõÑ©Ñ©
														//Ñ©Ñ©ÅúÅúÑ©Ñ©
														//Ñ©ÅõÅõÅúÑ©Ñ©
														//Ñ©Ñ©ÅúÑ©ÅúÑ©
														//Ñ©ÅõÑ©ÅúÑ©Åõ
														if(ban1[macropos(9,8)]==ISHI_K){

															//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
															//Ñ©ÅúÑ©ÅõÑ©Ñ©
															//Ñ©Ñ©ÅúÅúÑ©Ñ©
															//Ñ©ÅõÅõÅúÑ©Ñ©
															//Ñ©Ñ©ÅúÑ©ÅúÅõ
															//Ñ©ÅõÑ©ÅúÑ©Åõ
															if(ban1[macropos(11,10)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(10,8));

																//ÅõÑ©Ñ©Ñ©Ñ©Ñ©Ñ©
																//Ñ©ÅúÑ©ÅõÑ©Ñ©Ñ©
																//Ñ©Ñ©ÅúÅúÅúÑ©Ñ©
																//Ñ©ÅõÅõÅúÑ©Ñ©Ñ©
																//Ñ©Ñ©ÅúÑ©ÅúÅõÑ©
																//Ñ©ÅõÑ©ÅúÑ©ÅõÑ©
																if(ban1[macropos(10,8)]==ISHI_K){

																	//ÅõÑ©Ñ©Ñ©Ñ©Ñ©Ñ©
																	//Ñ©ÅúÑ©ÅõÑ©Ñ©Ñ©
																	//Ñ©Ñ©ÅúÅúÅúÅõÑ©
																	//Ñ©ÅõÅõÅúÑ©Ñ©Ñ©
																	//Ñ©Ñ©ÅúÑ©ÅúÅõÑ©
																	//Ñ©ÅõÑ©ÅúÑ©ÅõÑ©
																	if(ban1[macropos(11,8)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(11,7));

																		//ÅõÑ©Ñ©Ñ©Ñ©Ñ©Ñ©
																		//Ñ©ÅúÑ©ÅõÑ©ÅúÑ©
																		//Ñ©Ñ©ÅúÅúÅúÅõÑ©
																		//Ñ©ÅõÅõÅúÑ©Ñ©Ñ©
																		//Ñ©Ñ©ÅúÑ©ÅúÅõÑ©
																		//Ñ©ÅõÑ©ÅúÑ©ÅõÑ©
																		if(ban1[macropos(11,7)]==ISHI_K){

																			//ÅõÑ©Ñ©Ñ©Ñ©Ñ©Åõ
																			//Ñ©ÅúÑ©ÅõÑ©ÅúÑ©
																			//Ñ©Ñ©ÅúÅúÅúÅõÑ©
																			//Ñ©ÅõÅõÅúÑ©Ñ©Ñ©
																			//Ñ©Ñ©ÅúÑ©ÅúÅõÑ©
																			//Ñ©ÅõÑ©ÅúÑ©ÅõÑ©
																			if(ban1[macropos(12,6)]==ISHI_S){
																				if(cnt==18)
																					return zyocnv2(cnvflg, macropos(9,5));
																			}
																		}
																	}
																}
															}
														}
													}

													//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©ÅúÑ©ÅõÑ©Ñ©
													//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
													//Ñ©Ñ©ÅõÅúÑ©Ñ©
													//Ñ©Ñ©ÅúÑ©ÅúÑ©
													//Ñ©ÅõÑ©ÅúÑ©Åõ
													//Ñ©Ñ©Ñ©Ñ©ÅõÑ©
													if(ban1[macropos(10,12)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(9,8));

														//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
														//Ñ©ÅúÑ©ÅõÑ©Ñ©
														//Ñ©Ñ©ÅúÅúÑ©Ñ©
														//Ñ©Ñ©ÅõÅúÑ©Ñ©
														//Ñ©Ñ©ÅúÑ©ÅúÑ©
														//Ñ©ÅõÑ©ÅúÑ©Åõ
														//Ñ©Ñ©Ñ©Ñ©ÅõÑ©
														if(ban1[macropos(9,8)]==ISHI_K){

															//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
															//Ñ©ÅúÑ©ÅõÑ©Ñ©
															//Ñ©Ñ©ÅúÅúÑ©Ñ©
															//Ñ©Ñ©ÅõÅúÑ©Ñ©
															//Ñ©Ñ©ÅúÑ©ÅúÅõ
															//Ñ©ÅõÑ©ÅúÑ©Åõ
															//Ñ©Ñ©Ñ©Ñ©ÅõÑ©
															if(ban1[macropos(11,10)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(10,8));
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©ÅõÑ©
					//Ñ©Ñ©ÅúÑ©Ñ©
					//Ñ©Ñ©Ñ©ÅúÑ©
					//Ñ©Ñ©Ñ©Ñ©Åõ
					if(ban1[macropos(10,10)]==ISHI_S){

						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©ÅúÑ©ÅõÑ©
						//Ñ©Ñ©ÅúÑ©Ñ©
						//Ñ©Ñ©Ñ©ÅúÑ©
						//Ñ©Ñ©Ñ©Ñ©Åõ
						if(ban1[macropos(7,7)]==ISHI_K){

							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©ÅúÑ©ÅõÑ©
							//Ñ©Ñ©Ñ©ÅúÑ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
							//Ñ©Ñ©Ñ©ÅõÑ©Åõ
							if(ban1[macropos(8,10)]==ISHI_S){

								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©ÅúÑ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©ÅúÑ©ÅõÑ©
								//Ñ©Ñ©Ñ©ÅúÑ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
								//Ñ©Ñ©Ñ©ÅõÑ©Åõ
								if(ban1[macropos(6,6)]==ISHI_K){

									//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©ÅúÑ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©ÅúÑ©ÅõÑ©
									//Ñ©Ñ©Ñ©ÅúÑ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
									//Ñ©Ñ©Ñ©ÅõÑ©Åõ
									if(ban1[macropos(5,5)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(6,8));

										//ÅõÑ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©ÅúÑ©Ñ©Ñ©Ñ©
										//Ñ©Ñ©ÅúÑ©ÅõÑ©
										//Ñ©ÅúÑ©ÅúÑ©Ñ©
										//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
										//Ñ©Ñ©Ñ©ÅõÑ©Åõ
										if(ban1[macropos(6,8)]==ISHI_K){

											//ÅõÑ©Ñ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©ÅúÑ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
											//Ñ©ÅúÑ©ÅúÑ©Ñ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©ÅúÑ©Ñ©
											//Ñ©Ñ©Ñ©ÅõÅõÅõÑ©
											if(ban1[macropos(9,10)]==ISHI_S){

												//ÅõÑ©Ñ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©ÅúÑ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
												//Ñ©ÅúÑ©ÅúÑ©Ñ©Ñ©
												//Ñ©Ñ©Ñ©Ñ©ÅúÑ©Ñ©
												//Ñ©Ñ©Ñ©ÅõÅõÅõÅú
												if(ban1[macropos(11,10)]==ISHI_K){

													//ÅõÑ©Ñ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©ÅúÑ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
													//Ñ©ÅúÅõÅúÑ©Ñ©Ñ©
													//Ñ©Ñ©Ñ©Ñ©ÅúÑ©Ñ©
													//Ñ©Ñ©Ñ©ÅõÅõÅõÅú
													if(ban1[macropos(7,8)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(6,7));
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			//Ñ©Ñ©Ñ©Ñ©Ñ©
			//Ñ©Ñ©ÅõÑ©Ñ©
			//Ñ©Ñ©ÅúÑ©Ñ©
			//Ñ©Ñ©Ñ©Ñ©Ñ©
			//Ñ©Ñ©Ñ©Ñ©Ñ©
			else if(ban1[macropos(8,7)]==ISHI_S){	/* íºê⁄Ç«Çﬂ					*/
				if(cnt==2){
//					if(MCrndx < 70)
						return zyocnv2(cnvflg, macropos(9,7));
//					else
//						return 0;
				}

				//Ñ©Ñ©Ñ©Ñ©Ñ©
				//Ñ©Ñ©ÅõÑ©Ñ©
				//Ñ©Ñ©ÅúÅúÑ©
				//Ñ©Ñ©Ñ©Ñ©Ñ©
				//Ñ©Ñ©Ñ©Ñ©Ñ©
				if(ban1[macropos(9,8)]==ISHI_K){
					if(cnt==3){
						tmp = MCrndx;
						if(tmp < 40)
							return zyocnv2(cnvflg, macropos(9,9));
						if(tmp < 70)
							return zyocnv2(cnvflg, macropos(7,8));
						else
							return zyocnv2(cnvflg, macropos(10,8));
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©ÅõÑ©Ñ©
					//Ñ©ÅõÅúÅúÑ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(7,8)]==ISHI_S){

						//Ñ©Ñ©Ñ©ÅúÑ©
						//Ñ©Ñ©ÅõÑ©Ñ©
						//Ñ©ÅõÅúÅúÑ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(9,6)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(9,9));
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©ÅõÑ©Ñ©
					//Ñ©Ñ©ÅúÅúÅõ
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(10,8)]==ISHI_S){

						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©ÅúÅõÑ©Ñ©
						//Ñ©Ñ©ÅúÅúÅõ
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(7,7)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(7,9));
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©ÅõÑ©Ñ©
					//Ñ©Ñ©ÅúÅúÑ©
					//Ñ©Ñ©Ñ©ÅõÑ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(9,9)]==ISHI_S){

						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©ÅõÑ©Ñ©
						//Ñ©ÅúÅúÅúÑ©
						//Ñ©Ñ©Ñ©ÅõÑ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(7,8)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(6,8));
						}
					}
				}

				//Ñ©Ñ©Ñ©Ñ©Ñ©
				//Ñ©Ñ©ÅõÅúÑ©
				//Ñ©Ñ©ÅúÑ©Ñ©
				//Ñ©Ñ©Ñ©Ñ©Ñ©
				//Ñ©Ñ©Ñ©Ñ©Ñ©
				if(ban1[macropos(9,7)]==ISHI_K){	/* â‘åéíËêŒ					*/
					if(cnt==3){
						tmp = MCrndx;
						if(tmp < 35)
							return zyocnv2(cnvflg, macropos(7,9));
						else if(tmp < 70)
							return zyocnv2(cnvflg, macropos(9,8));
						else
							return zyocnv2(cnvflg, macropos(9,9));
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©ÅõÅúÑ©
					//Ñ©Ñ©ÅúÑ©Ñ©
					//Ñ©ÅõÑ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(7,9)]==ISHI_S){
						if(cnt==4){
							return zyocnv2(cnvflg, macropos(10,6));
						}

						//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
						//Ñ©Ñ©ÅõÅúÑ©Ñ©
						//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
						//Ñ©ÅõÑ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(10,6)]==ISHI_K){

							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
							//Ñ©Ñ©ÅõÅúÑ©Ñ©
							//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
							//Ñ©ÅõÑ©ÅõÑ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(9,9)]==ISHI_S){
								if(cnt==6){
									tmp = MCrndx;
									if(tmp<50)
										return zyocnv2(cnvflg, macropos(9,5));
									else
										return zyocnv2(cnvflg, macropos(9,6));
								}

								//Ñ©Ñ©Ñ©ÅúÑ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
								//Ñ©Ñ©ÅõÅúÑ©Ñ©
								//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
								//Ñ©ÅõÑ©ÅõÑ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(9,5)]==ISHI_K){

									//Ñ©Ñ©Ñ©ÅúÑ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
									//Ñ©Ñ©ÅõÅúÑ©Ñ©
									//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
									//Ñ©ÅõÅõÅõÑ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(8,9)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(10,9));
									}
								}
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©ÅõÅúÑ©
							//Ñ©Ñ©ÅõÅúÑ©Ñ©
							//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
							//Ñ©ÅõÑ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(9,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,5));

								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©ÅúÑ©Ñ©
								//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
								//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
								//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
								//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(10,5)]==ISHI_K){

									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©ÅúÅõÑ©
									//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
									//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
									//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
									//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(11,5)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(10,8));

										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©Ñ©Ñ©Ñ©ÅúÅõÑ©
										//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
										//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
										//Ñ©Ñ©ÅúÑ©ÅúÑ©Ñ©
										//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										if(ban1[macropos(10,8)]==ISHI_K){

											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©ÅúÅõÑ©
											//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
											//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
											//Ñ©Ñ©ÅúÑ©ÅúÑ©Ñ©
											//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											if(ban1[macropos(10,7)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(11,8));

												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©Ñ©Ñ©Ñ©ÅúÅõÑ©
												//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
												//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
												//Ñ©Ñ©ÅúÑ©ÅúÅúÑ©
												//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
												if(ban1[macropos(11,8)]==ISHI_K){

													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©Ñ©Ñ©Ñ©ÅúÅõÑ©
													//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
													//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
													//Ñ©Ñ©ÅúÅõÅúÅúÑ©
													//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
													if(ban1[macropos(9,8)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(11,9));

														//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
														//Ñ©Ñ©Ñ©Ñ©ÅúÅõÑ©
														//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
														//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
														//Ñ©Ñ©ÅúÅõÅúÅúÑ©
														//Ñ©ÅõÑ©Ñ©Ñ©ÅúÑ©
														//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
														if(ban1[macropos(11,9)]==ISHI_K){

															//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
															//Ñ©Ñ©Ñ©Ñ©ÅúÅõÑ©
															//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
															//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
															//Ñ©Ñ©ÅúÅõÅúÅúÑ©
															//Ñ©ÅõÑ©Ñ©Ñ©ÅúÑ©
															//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
															if(ban1[macropos(12,10)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(11,10));
															}
														}
													}
												}
											}
										}
									}

									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©ÅúÑ©Ñ©
									//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
									//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
									//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
									//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(10,7)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,5));

										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©Ñ©ÅúÑ©ÅúÑ©Ñ©
										//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
										//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
										//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
										//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										if(ban1[macropos(8,5)]==ISHI_K){

											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©ÅúÅõÅúÑ©Ñ©
											//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
											//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
											//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
											//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											if(ban1[macropos(9,5)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(9,4));

												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©Ñ©Ñ©ÅúÑ©Ñ©Ñ©
												//Ñ©Ñ©ÅúÅõÅúÑ©Ñ©
												//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
												//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
												//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
												//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
												if(ban1[macropos(9,4)]==ISHI_K){

													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
													//Ñ©Ñ©ÅúÅõÅúÑ©Ñ©
													//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
													//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
													//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
													//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
													if(ban1[macropos(10,4)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(8,3));

														//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
														//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
														//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
														//Ñ©Ñ©ÅúÅõÅúÑ©Ñ©
														//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
														//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
														//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
														//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
														//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
														if(ban1[macropos(8,3)]==ISHI_K){

															//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
															//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
															//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
															//Ñ©Ñ©ÅúÅõÅúÑ©Ñ©
															//Ñ©Ñ©Ñ©ÅõÅúÅõÑ©
															//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
															//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
															//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
															//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
															if(ban1[macropos(11,6)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(7,6));

																//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
																//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
																//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
																//Ñ©Ñ©ÅúÅõÅúÑ©Ñ©
																//Ñ©ÅúÑ©ÅõÅúÅõÑ©
																//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
																//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
																//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
																if(ban1[macropos(7,6)]==ISHI_K){

																	//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
																	//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
																	//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
																	//Ñ©Ñ©ÅúÅõÅúÑ©Ñ©
																	//Ñ©ÅúÑ©ÅõÅúÅõÑ©
																	//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
																	//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
																	//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																	//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
																	if(ban1[macropos(10,3)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(7,4));
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Åõ
					//Ñ©Ñ©ÅõÅúÑ©
					//Ñ©Ñ©ÅúÑ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(10,6)]==ISHI_S){
						if(cnt==4){
							return zyocnv2(cnvflg, macropos(7,9));
						}

						//Ñ©Ñ©Ñ©Ñ©Åõ
						//Ñ©Ñ©ÅõÅúÑ©
						//Ñ©Ñ©ÅúÑ©Ñ©
						//Ñ©ÅúÑ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(7,9)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(9,8));

							//Ñ©Ñ©Ñ©Ñ©ÅõÑ©
							//Ñ©Ñ©ÅõÅúÑ©Ñ©
							//Ñ©Ñ©ÅúÅõÑ©Ñ©
							//Ñ©ÅúÑ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(9,8)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,9));

								//Ñ©Ñ©Ñ©Ñ©ÅõÑ©
								//Ñ©Ñ©ÅõÅúÑ©Ñ©
								//Ñ©Ñ©ÅúÅõÑ©Ñ©
								//Ñ©ÅúÑ©Ñ©ÅúÑ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(10,9)]==ISHI_K){
									if(cnt==7)
										return zyocnv2(cnvflg, macropos(11,6));

									//Ñ©Ñ©Ñ©Ñ©ÅõÅõ
									//Ñ©Ñ©ÅõÅúÑ©Ñ©
									//Ñ©Ñ©ÅúÅõÑ©Ñ©
									//Ñ©ÅúÑ©Ñ©ÅúÑ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(11,6)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,9));

										//Ñ©Ñ©Ñ©Ñ©ÅõÅõ
										//Ñ©Ñ©ÅõÅúÑ©Ñ©
										//Ñ©Ñ©ÅúÅõÑ©Ñ©
										//Ñ©ÅúÅúÑ©ÅúÑ©
										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										if(ban1[macropos(8,9)]==ISHI_K){

											//Ñ©Ñ©Ñ©Ñ©ÅõÅõ
											//Ñ©Ñ©ÅõÅúÑ©Ñ©
											//Ñ©Ñ©ÅúÅõÑ©Ñ©
											//Ñ©ÅúÅúÅõÅúÑ©
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											if(ban1[macropos(9,9)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(7,11));

												//Ñ©Ñ©Ñ©Ñ©ÅõÅõ
												//Ñ©Ñ©ÅõÅúÑ©Ñ©
												//Ñ©Ñ©ÅúÅõÑ©Ñ©
												//Ñ©ÅúÅúÅõÅúÑ©
												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©ÅúÑ©Ñ©Ñ©Ñ©
												if(ban1[macropos(7,11)]==ISHI_K){

													//Ñ©Ñ©Ñ©ÅõÅõÅõ
													//Ñ©Ñ©ÅõÅúÑ©Ñ©
													//Ñ©Ñ©ÅúÅõÑ©Ñ©
													//Ñ©ÅúÅúÅõÅúÑ©
													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©ÅúÑ©Ñ©Ñ©Ñ©
													if(ban1[macropos(9,6)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(8,6));

														//Ñ©Ñ©ÅúÅõÅõÅõ
														//Ñ©Ñ©ÅõÅúÑ©Ñ©
														//Ñ©Ñ©ÅúÅõÑ©Ñ©
														//Ñ©ÅúÅúÅõÅúÑ©
														//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
														//Ñ©ÅúÑ©Ñ©Ñ©Ñ©
														if(ban1[macropos(8,6)]==ISHI_K){

															//Ñ©Ñ©ÅúÅõÅõÅõ
															//Ñ©Ñ©ÅõÅúÑ©Ñ©
															//Ñ©ÅõÅúÅõÑ©Ñ©
															//Ñ©ÅúÅúÅõÅúÑ©
															//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
															//Ñ©ÅúÑ©Ñ©Ñ©Ñ©
															if(ban1[macropos(7,8)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(6,10));

																//Ñ©Ñ©Ñ©ÅúÅõÅõÅõ
																//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
																//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
																//Ñ©Ñ©ÅúÅúÅõÅúÑ©
																//Ñ©ÅúÑ©Ñ©Ñ©Ñ©Ñ©
																//Ñ©Ñ©ÅúÑ©Ñ©Ñ©Ñ©
																if(ban1[macropos(6,10)]==ISHI_K){

																	//Ñ©Ñ©Ñ©ÅúÅõÅõÅõ
																	//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
																	//Ñ©Ñ©ÅõÅúÅõÑ©Ñ©
																	//Ñ©Ñ©ÅúÅúÅõÅúÑ©
																	//Ñ©ÅúÑ©Ñ©Ñ©Ñ©Ñ©
																	//ÅõÑ©ÅúÑ©Ñ©Ñ©Ñ©
																	if(ban1[macropos(5,11)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(6,9));
																	}
																
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©ÅõÑ©Åõ
							//Ñ©Ñ©Ñ©ÅõÅúÑ©
							//Ñ©Ñ©Ñ©ÅúÑ©Ñ©
							//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(8,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(7,6));

								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©ÅúÅõÑ©Åõ
								//Ñ©Ñ©Ñ©ÅõÅúÑ©
								//Ñ©Ñ©Ñ©ÅúÑ©Ñ©
								//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(7,6)]==ISHI_K){

									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©ÅúÅõÑ©Åõ
									//Ñ©Ñ©Ñ©ÅõÅúÑ©
									//Ñ©Ñ©ÅõÅúÑ©Ñ©
									//Ñ©Ñ©ÅúÑ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(7,8)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(6,10));
									}
								}
							}
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©ÅõÅúÑ©
					//Ñ©Ñ©ÅúÅõÑ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(9,8)]==ISHI_S){
						if(cnt==4){
							return zyocnv2(cnvflg, macropos(10,9));
						}

						//Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©ÅõÅúÑ©
						//Ñ©Ñ©ÅúÅõÑ©
						//Ñ©Ñ©Ñ©Ñ©Åú
						//Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(10,9)]==ISHI_K){
							if(cnt==5){
								tmp = MCrndx;
								if(tmp < 30)
									return zyocnv2(cnvflg, macropos(7,9));
								else if(tmp < 55)
									return zyocnv2(cnvflg, macropos(8,9));
								else if(tmp <80)
									return zyocnv2(cnvflg, macropos(9,9));
								else
									return zyocnv2(cnvflg, macropos(10,10));
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©ÅõÅúÑ©Ñ©
							//Ñ©Ñ©ÅúÅõÑ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
							//Ñ©Ñ©Ñ©Ñ©ÅõÑ©
							if(ban1[macropos(10,10)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(11,9));

								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©ÅõÅúÑ©Ñ©
								//Ñ©Ñ©ÅúÅõÑ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©ÅúÅú
								//Ñ©Ñ©Ñ©Ñ©ÅõÑ©
								if(ban1[macropos(11,9)]==ISHI_K){

									//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
									//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
									//Ñ©Ñ©ÅúÅõÑ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©ÅúÅúÑ©
									//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
									if(ban1[macropos(10,6)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(11,8));

										//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
										//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
										//Ñ©Ñ©ÅúÅõÑ©ÅúÑ©
										//Ñ©Ñ©Ñ©Ñ©ÅúÅúÑ©
										//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
										if(ban1[macropos(11,8)]==ISHI_K){

											//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
											//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
											//Ñ©Ñ©ÅúÅõÑ©ÅúÑ©
											//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõ
											//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
											if(ban1[macropos(12,9)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(9,10));

												//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
												//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
												//Ñ©Ñ©ÅúÅõÑ©ÅúÑ©
												//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõ
												//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©1
												if(ban1[macropos(9,10)]==ISHI_K){

													//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
													//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
													//Ñ©Ñ©ÅúÅõÑ©ÅúÑ©
													//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõ
													//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
													//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©1
													if(ban1[macropos(8,11)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(11,7));

														//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
														//Ñ©Ñ©ÅõÅúÑ©ÅúÑ©
														//Ñ©Ñ©ÅúÅõÑ©ÅúÑ©
														//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõ
														//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
														//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©
														if(ban1[macropos(11,7)]==ISHI_K){

															//Ñ©Ñ©Ñ©Ñ©ÅõÅõÑ©
															//Ñ©Ñ©ÅõÅúÑ©ÅúÑ©
															//Ñ©Ñ©ÅúÅõÑ©ÅúÑ©
															//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõ
															//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
															//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©
															if(ban1[macropos(11,6)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(10,8));

																//Ñ©Ñ©Ñ©Ñ©ÅõÅõÑ©
																//Ñ©Ñ©ÅõÅúÑ©ÅúÑ©
																//Ñ©Ñ©ÅúÅõÅúÅúÑ©
																//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõ
																//Ñ©Ñ©Ñ©ÅúÅõÑ©Ñ©
																//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©
																if(ban1[macropos(10,8)]==ISHI_K){

																	//Ñ©Ñ©Ñ©Ñ©ÅõÅõÑ©
																	//Ñ©Ñ©ÅõÅúÑ©ÅúÑ©
																	//Ñ©Ñ©ÅúÅõÅúÅúÑ©
																	//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõ
																	//Ñ©Ñ©Ñ©ÅúÅõÑ©Åõ
																	//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©
																	if(ban1[macropos(12,10)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(12,6));

																		//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
																		//Ñ©Ñ©Ñ©Ñ©ÅõÅõÅúÑ©
																		//Ñ©Ñ©ÅõÅúÑ©ÅúÑ©Ñ©
																		//Ñ©Ñ©ÅúÅõÅúÅúÑ©Ñ©
																		//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõÑ©
																		//Ñ©Ñ©Ñ©ÅúÅõÑ©ÅõÑ©
																		//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																		if(ban1[macropos(12,6)]==ISHI_K){

																			//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
																			//Ñ©Ñ©Ñ©Ñ©ÅõÅõÅúÑ©
																			//Ñ©Ñ©ÅõÅúÑ©ÅúÑ©Ñ©
																			//Ñ©Ñ©ÅúÅõÅúÅúÑ©Ñ©
																			//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõÑ©
																			//Ñ©Ñ©Ñ©ÅúÅõÑ©ÅõÑ©
																			//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																			if(ban1[macropos(13,5)]==ISHI_S){
																				if(cnt==18)
																					return zyocnv2(cnvflg, macropos(12,7));

																				//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
																				//Ñ©Ñ©Ñ©Ñ©ÅõÅõÅúÑ©
																				//Ñ©Ñ©ÅõÅúÑ©ÅúÅúÑ©
																				//Ñ©Ñ©ÅúÅõÅúÅúÑ©Ñ©
																				//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõÑ©
																				//Ñ©Ñ©Ñ©ÅúÅõÑ©ÅõÑ©
																				//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																				if(ban1[macropos(12,7)]==ISHI_K){

																					//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
																					//Ñ©Ñ©Ñ©Ñ©ÅõÅõÅúÅõ
																					//Ñ©Ñ©ÅõÅúÑ©ÅúÅúÑ©
																					//Ñ©Ñ©ÅúÅõÅúÅúÑ©Ñ©
																					//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõÑ©
																					//Ñ©Ñ©Ñ©ÅúÅõÑ©ÅõÑ©
																					//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																					if(ban1[macropos(13,6)]==ISHI_S){
																						if(cnt==20)
																							return zyocnv2(cnvflg, macropos(13,7));

																						//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
																						//Ñ©Ñ©Ñ©Ñ©ÅõÅõÅúÅõ
																						//Ñ©Ñ©ÅõÅúÑ©ÅúÅúÅú
																						//Ñ©Ñ©ÅúÅõÅúÅúÑ©Ñ©
																						//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõÑ©
																						//Ñ©Ñ©Ñ©ÅúÅõÑ©ÅõÑ©
																						//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																						if(ban1[macropos(13,7)]==ISHI_K){

																							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
																							//Ñ©Ñ©Ñ©Ñ©ÅõÅõÅúÅõ
																							//Ñ©Ñ©ÅõÅúÅõÅúÅúÅú
																							//Ñ©Ñ©ÅúÅõÅúÅúÑ©Ñ©
																							//Ñ©Ñ©Ñ©Ñ©ÅúÅúÅõÑ©
																							//Ñ©Ñ©Ñ©ÅúÅõÑ©ÅõÑ©
																							//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																							if(ban1[macropos(10,7)]==ISHI_S){
																								if(cnt==22)
																									return zyocnv2(cnvflg, macropos(9,9));

																								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
																								//Ñ©Ñ©Ñ©Ñ©ÅõÅõÅúÅõ
																								//Ñ©Ñ©ÅõÅúÅõÅúÅúÅú
																								//Ñ©Ñ©ÅúÅõÅúÅúÑ©Ñ©
																								//Ñ©Ñ©Ñ©ÅúÅúÅúÅõÑ©
																								//Ñ©Ñ©Ñ©ÅúÅõÑ©ÅõÑ©
																								//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																								if(ban1[macropos(9,9)]==ISHI_K){

																									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
																									//Ñ©Ñ©Ñ©Ñ©ÅõÅõÅúÅõ
																									//Ñ©Ñ©ÅõÅúÅõÅúÅúÅú
																									//Ñ©Ñ©ÅúÅõÅúÅúÑ©Ñ©
																									//Ñ©Ñ©Ñ©ÅúÅúÅúÅõÑ©
																									//Ñ©Ñ©ÅõÅúÅõÑ©ÅõÑ©
																									//Ñ©Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
																									if(ban1[macropos(8,10)]==ISHI_S){
																										if(cnt==24)
																											return zyocnv2(cnvflg, macropos(8,9));
																									}
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©ÅõÅúÑ©
							//Ñ©Ñ©ÅúÅõÑ©
							//Ñ©Ñ©Ñ©ÅõÅú
							//Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(9,9)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,6));

								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
								//Ñ©Ñ©ÅõÅúÑ©Ñ©
								//Ñ©Ñ©ÅúÅõÑ©Ñ©
								//Ñ©Ñ©Ñ©ÅõÅúÑ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(10,6)]==ISHI_K){

									//Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
									//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
									//Ñ©Ñ©ÅõÅúÑ©Ñ©
									//Ñ©Ñ©ÅúÅõÑ©Ñ©
									//Ñ©Ñ©Ñ©ÅõÅúÑ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(11,5)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(10,10));

										//Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
										//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
										//Ñ©Ñ©ÅõÅúÑ©Ñ©
										//Ñ©Ñ©ÅúÅõÑ©Ñ©
										//Ñ©Ñ©Ñ©ÅõÅúÑ©
										//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
										if(ban1[macropos(10,10)]==ISHI_K){

											//Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
											//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
											//Ñ©Ñ©ÅõÅúÅõÑ©
											//Ñ©Ñ©ÅúÅõÑ©Ñ©
											//Ñ©Ñ©Ñ©ÅõÅúÑ©
											//Ñ©Ñ©Ñ©Ñ©ÅúÑ©
											if(ban1[macropos(10,7)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(8,10));
											}
										}
									}
								}
							}

							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
							//Ñ©Ñ©ÅúÅõÑ©Ñ©Ñ©
							//Ñ©ÅõÑ©Ñ©ÅúÑ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(7,9)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(11,9));

								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
								//Ñ©Ñ©ÅúÅõÑ©Ñ©Ñ©
								//Ñ©ÅõÑ©Ñ©ÅúÅúÑ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(11,9)]==ISHI_K){

									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©
									//Ñ©Ñ©ÅúÅõÅõÑ©Ñ©
									//Ñ©ÅõÑ©Ñ©ÅúÅúÑ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(10,8)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(12,9));

										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©Ñ©
										//Ñ©Ñ©ÅúÅõÅõÑ©Ñ©Ñ©
										//Ñ©ÅõÑ©Ñ©ÅúÅúÅúÑ©
										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										if(ban1[macropos(12,9)]==ISHI_K){

											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©ÅõÅúÑ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©ÅúÅõÅõÑ©Ñ©Ñ©
											//Ñ©ÅõÑ©Ñ©ÅúÅúÅúÅõ
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											if(ban1[macropos(13,9)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(10,10));
											}
										}
									}
								}
							}
						}
					}

					//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
					//Ñ©Ñ©Ñ©ÅúÑ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
					//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
					if(ban1[macropos(9,9)]==ISHI_S){
						if(cnt==4)
							return zyocnv2(cnvflg, macropos(7,9));

						//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
						//Ñ©Ñ©Ñ©ÅúÑ©Ñ©Ñ©
						//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
						//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
						if(ban1[macropos(7,9)]==ISHI_K){

							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
							//Ñ©Ñ©Ñ©ÅúÑ©Ñ©Ñ©
							//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
							//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
							//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
							if(ban1[macropos(6,10)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,6));

								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©ÅúÑ©
								//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
								//Ñ©Ñ©Ñ©ÅúÑ©Ñ©Ñ©
								//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
								//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
								//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
								if(ban1[macropos(10,6)]==ISHI_K){

									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
									//Ñ©Ñ©Ñ©Ñ©Ñ©ÅúÑ©
									//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
									//Ñ©Ñ©Ñ©ÅúÑ©Ñ©Ñ©
									//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
									//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
									//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
									if(ban1[macropos(11,5)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,6));

										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
										//Ñ©Ñ©Ñ©ÅúÑ©ÅúÑ©
										//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
										//Ñ©Ñ©Ñ©ÅúÑ©Ñ©Ñ©
										//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
										//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
										//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
										if(ban1[macropos(8,6)]==ISHI_K){

											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
											//Ñ©Ñ©Ñ©ÅúÅõÅúÑ©
											//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
											//Ñ©Ñ©Ñ©ÅúÑ©Ñ©Ñ©
											//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
											//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
											//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
											if(ban1[macropos(9,6)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(7,8));

												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
												//Ñ©Ñ©Ñ©ÅúÅõÅúÑ©
												//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
												//Ñ©Ñ©ÅúÅúÑ©Ñ©Ñ©
												//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
												//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
												//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
												if(ban1[macropos(7,8)]==ISHI_K){

													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
													//Ñ©Ñ©Ñ©ÅúÅõÅúÑ©
													//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
													//Ñ©Ñ©ÅúÅúÑ©ÅõÑ©
													//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
													//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
													//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
													if(ban1[macropos(10,8)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(6,8));

														//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
														//Ñ©Ñ©Ñ©ÅúÅõÅúÑ©
														//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
														//Ñ©ÅúÅúÅúÑ©ÅõÑ©
														//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
														//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
														//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
														if(ban1[macropos(6,8)]==ISHI_K){

															//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
															//Ñ©Ñ©Ñ©ÅúÅõÅúÑ©
															//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
															//ÅõÅúÅúÅúÑ©ÅõÑ©
															//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
															//Ñ©ÅõÑ©Ñ©Ñ©Ñ©Ñ©
															//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
															if(ban1[macropos(5,8)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(8,10));

																//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
																//Ñ©Ñ©Ñ©ÅúÅõÅúÑ©
																//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
																//ÅõÅúÅúÅúÑ©ÅõÑ©
																//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
																//Ñ©ÅõÑ©ÅúÑ©Ñ©Ñ©
																//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
																if(ban1[macropos(8,10)]==ISHI_K){

																	//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©
																	//Ñ©Ñ©Ñ©Ñ©Ñ©Ñ©Åõ
																	//Ñ©Ñ©Ñ©ÅúÅõÅúÑ©
																	//Ñ©Ñ©Ñ©ÅõÅúÑ©Ñ©
																	//ÅõÅúÅúÅúÑ©ÅõÑ©
																	//Ñ©Ñ©ÅúÑ©ÅõÑ©Ñ©
																	//Ñ©ÅõÑ©ÅúÑ©Ñ©Ñ©
																	//Ñ©Ñ©Ñ©Ñ©ÅõÑ©Ñ©
																	if(ban1[macropos(9,11)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(6,4));
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
