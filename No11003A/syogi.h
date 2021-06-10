
#define draw_lineA	tch_line

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

/****************************************************************************/
/*		define                                                              */
/****************************************************************************/

//#define BANSIZE		9
//#define VERSION "2.08-No3"
//#define SINGLETHREAD
//#define MAXTHREAD 16

//#define BAN_TOP		80
//#define BAN_LEFT	80
//#define BAN_STEP	36


#define ISHI_N		0x0000		/* �΃i�V					*/
#define ISHI_K		0x0100		/* ����						*/
#define ISHI_S		0x0200		/* ����						*/
#define ISHI_O		0x0300		/* �ՊO						*/

#define TEN_INFINIT 0x70000000

#define MV_NARI		0x10000
#define MV_TORI		0x20000

//	int move;	0x000000ff:source, 0x0000ff00:destination, 0x00010000:���t���O


//	1:	�����
//	2:	�����
//	3:	���荁
//	4:	���萬��
//	5:	����j
//	6:	���萬�j
//	7:	�����
//	8:	���萬��
//	9:	�����
//	10:	����p
//	11:	����n
//	12:	�����
//	13:	���藳
//	14:	�����

//	15:	����
//	16:	����
//	17:	��荁
//	18:	��萬��
//	19:	���j
//	20:	��萬�j
//	21:	����
//	22:	��萬��
//	23:	����
//	24:	���p
//	25:	���n
//	26:	����
//	27:	��藳
//	28:	��艤


//		int ban[131+29], �Ֆ� for(i=21; i<110; i++)

static const int startpos[160]={
	ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,
	ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,
	ISHI_O		,ISHI_S+17	,ISHI_S+19	,ISHI_S+21	,ISHI_S+23	,ISHI_S+28	,ISHI_S+23	,ISHI_S+21	,ISHI_S+19	,ISHI_S+17	,
	ISHI_O		,0			,ISHI_S+26	,0			,0			,0			,0			,0			,ISHI_S+24	,0			,
	ISHI_O		,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,ISHI_S+15	,
	ISHI_O		,0			,0			,0			,0			,0			,0			,0			,0			,0			,
	ISHI_O		,0			,0			,0			,0			,0			,0			,0			,0			,0			,
	ISHI_O		,0			,0			,0			,0			,0			,0			,0			,0			,0			,
	ISHI_O		,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,ISHI_K+1	,
	ISHI_O		,0			,ISHI_K+10	,0			,0			,0			,0			,0			,ISHI_K+12	,0			,
	ISHI_O		,ISHI_K+3	,ISHI_K+5	,ISHI_K+7	,ISHI_K+9	,ISHI_K+14	,ISHI_K+9	,ISHI_K+7	,ISHI_K+5	,ISHI_K+3	,
	ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,
	ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,ISHI_O		,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static const int dir1[5]={-10,-1,1,10,0};				//	�㉺���E
static const int dir2[5]={-11,-9,9,11,0};				//	�ȂȂ�
static const int dir3[9]={-11,-10,-9,-1,1,9,10,11,0};	//	�W����
static const int dir10[28+1][9]={
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },
	{-10,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//1		�����
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//2		�����
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//3		���荁
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//4		���萬��
	{-21,-19,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//5		����j
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//6		���萬�j
	{-11,-10,-9 ,9  ,11 ,0  ,0  ,0  ,0  },	//7		�����
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//8		���萬��
	{-11,-10,-9 ,-1 ,1  ,10 ,0  ,0  ,0  },	//9		�����
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//10	����p
	{-10,-1 ,1  ,10 ,0  ,0  ,0  ,0  ,0  },	//11	����n
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//12	�����
	{-11,-9 ,9  ,11 ,0  ,0  ,0  ,0  ,0  },	//13	���藳
	{-11,-10,-9 ,-1 ,1  ,9  ,10 ,11 ,0  },	//14	�����
	{10 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//15	����
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//16	����
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//17	��荁
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//18	��萬��
	{19 ,21 ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//19	���j
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//20	��萬�j
	{9  ,10 ,11 ,-11,-9 ,0  ,0  ,0  ,0  },	//21	����
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//22	��萬��
	{9  ,10 ,11 ,-1 ,1  ,-10,0  ,0  ,0  },	//23	����
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//24	���p
	{-10,-1 ,1  ,10 ,0  ,0  ,0  ,0  ,0  },	//25	���n
	{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  },	//26	����
	{-11,-9 ,9  ,11 ,0  ,0  ,0  ,0  ,0  },	//27	��藳
	{-11,-10,-9 ,-1 ,1  ,9  ,10 ,11 ,0  },	//28	��艤
};

static const int tenall[29]={
	0,
	87,			//��	1
	534,		//��	2
	232,		//��	3
	489,		//����	4
	257,		//�j	5
	510,		//���j	6
	369,		//��	7
	495,		//����	8
	444,		//��	9
	569,		//�p	10
	827,		//�n	11
	642,		//��	12
	945,		//��	13
	100000,		//��	14
	-87,		//��	15
	-534,		//��	16
	-232,		//��	17
	-489,		//����	18
	-257,		//�j	19
	-510,		//���j	20
	-369,		//��	21
	-495,		//����	22
	-444,		//��	23
	-569,		//�p	24
	-827,		//�n	25
	-642,		//��	26
	-945,		//��	27
	-100000,	//��	28
};
	
#define macrox(pos) ((pos)%10)			/* pos ����w���W�ւ̕ϊ�	*/
#define macroy(pos) ((pos)/10-1)		/* pos ����x���W�ւ̕ϊ�	*/
#define macropos(x,y) ((x)+(y)*10+10)	/* X,Y ���W���� pos �ւ̕ϊ�	*/

					/* �����̔��]			*/
#define macroinv(koma) ((koma)==ISHI_K ? ISHI_S : ISHI_K)
//#define abs1(x) ((x)>=0?(x):-(x))
#define min1(x,y) ((x)>(y)?(y):(x))
#define max1(x,y) ((x)>(y)?(x):(y))

/****************************************************************************/
/*	struct	                                                            */
/****************************************************************************/
//struct Ten {
//    double  ten;
//    int  pos;
//    int  rot;
//};
//
//
//#define MCHASHSIZE    10000
//
//struct MCchild {
//	int pos;
//	int ten;
//	float value;
//	int games;							/* ���̎��T��������					*/
//	double win;							/* ��������							*/
//};
//
//struct MChash {
//	__int64 code;
//	struct MChash *next;
//	int ban[274];
//	int child_num;
//	struct MCchild child[225];
//	int games_sum;
//	int count;
//	int k;
//	CRITICAL_SECTION lock;
//};

