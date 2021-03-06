//
//		gomoku.h
//

//#include <stdio.h>
//#include <memory.h>
//#include <string.h>
//#include <fcntl.h>
//#include <sys/stat.h>
//#include <time.h>
//#include <ctype.h>
//#include <stdlib.h>
//#include <stdarg.h>
//#include <io.h>
//#include <math.h>
//#include <windows.h>
//#include <share.h>
//#include <direct.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;


#define ISHI_N    0x0000		/* 石ナシ					*/
#define ISHI_K    0x0001		/* 黒石						*/
#define ISHI_S    0x0002		/* 白石						*/
#define ISHI_O    0x0003		/* 盤外						*/
//#define ISHI_SELF 0x0008
#define ISHI_K2   0x0010		// ここに黒を打つと２連になる
#define ISHI_K3   0x0020		// ここに黒を打つと活３になる
#define ISHI_K4   0x0040		// ここに黒を打つと活４になる
#define ISHI_K4X  0x0080		// ここに黒を打つと棒４になる
#define ISHI_K5   0x0100		// ここに黒を打つと５連になる
#define ISHI_S2   0x0200		// ここに白を打つと２連になる
#define ISHI_S3   0x0400		// ここに白を打つと活３になる
#define ISHI_S33  0x0800		// ここに白を打つと３３になる
#define ISHI_S4   0x1000		// ここに白を打つと活４になる
#define ISHI_S4X  0x2000		// ここに白を打つと棒４になる
#define ISHI_S5   0x4000		// ここに白を打つと５連になる
#define ISHI_KINSYU 0x8000		// ここに黒を打つと禁手
#define ISHI_S44  0x10000		// ここに白を打つと４４になる


#define macrox(pos) ((pos)%16)	/* pos からＸ座標への変換	*/
#define macroy(pos) ((pos)/16)	/* pos からＹ座標への変換	*/
#define macropos(x,y) ((x)+(y)*16)	/* X,Y 座標から pos への変換	*/
//#define is_stone(k) (((k)&7)==ISHI_K || ((k)&7)==ISHI_S)
//#define ishazi(x,y) ((x)==1 || (x)==19 || (y)==1 || (y)==19)
//#define kk2k(kk) ((kk & IBAN_MASK)==0 ? ISHI_N : ((kk & IBAN_MASK) >= 0x200 ? ISHI_S : ISHI_K))
//#define k2kk(k) ((k & 7)==ISHI_N ? 0 :((k & 7)==ISHI_K ? 1 :((k & 7)==ISHI_S ? 0x201 : IBAN_STOP)))
#define macroinv(koma) ((koma)==ISHI_K ? ISHI_S : ISHI_K)
//#define abs1(x) ((x)>=0?(x):-(x))
#define min1(x,y) ((x)>(y)?(y):(x))
#define max1(x,y) ((x)>(y)?(x):(y))
