//以下声明和全局变量
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<map>
#include<queue>
#include<set>
#include<algorithm>
#include "Python.h"
#include"head.h"
using namespace std;

int cnn_score(int color);

const char punctuation = ',';

extern int field[11][10];
extern vector<int> valid_moves;
int deep_limit = 4;
int cuttab_index = 0;
extern int turns;

int deep_limit_cnt = 0;
int eval = 0;

const bool debug = 0;
const bool debug2 = 1;
//const bool debug2 = 0;
const bool cache = 0;	//是否使用rectab_cache

const int time_limit_k = 3900000;
const int time_limit_t = 5;
float eat_k = 0.3;

const int cuttabR = 17;
const int cuttabJ = 10;
const float cuttab[cuttabR][13] = { 
 1, 1, 1.00, 1.00, 1.00, 0.04, 0.05, 0.05, 0.05, 0.35, 0.35, 0.45, 0.45,
 1, 1, 1.00, 1.00, 1.00, 0.06, 0.06, 0.06, 0.06, 0.35, 0.35, 0.45, 0.45,
 1, 1, 1.00, 1.00, 1.00, 0.08, 0.08, 0.08, 0.08, 0.35, 0.35, 0.45, 0.45,
 1, 1, 1.00, 1.00, 1.00, 0.10, 0.10, 0.10, 0.12, 0.35, 0.35, 0.45, 0.45,
 1, 1, 1.00, 1.00, 1.00, 0.12, 0.12, 0.15, 0.20, 0.35, 0.35, 0.45, 0.45,
 1, 1, 1.00, 1.00, 1.00, 0.17, 0.17, 0.20, 0.30, 0.35, 0.35, 0.45, 0.45,
 1, 1, 1.00, 1.00, 1.00, 0.22, 0.22, 0.25, 0.40, 0.50, 0.50, 0.70, 0.70,
 1, 1, 1.00, 1.00, 1.00, 0.30, 0.30, 0.30, 0.50, 0.55, 0.55, 0.75, 0.75,
 1, 1, 1.00, 1.00, 1.00, 0.40, 0.40, 0.40, 0.60, 0.60, 0.60, 0.80, 0.80,
 1, 1, 1.00, 1.00, 1.00, 0.50, 0.50, 0.50, 0.65, 0.65, 0.65, 0.82, 0.82,
 1, 1, 1.00, 1.00, 1.00, 0.60, 0.60, 0.60, 0.70, 0.70, 0.70, 0.85, 0.85,
 1, 1, 1.00, 1.00, 1.00, 0.70, 0.70, 0.70, 0.70, 0.70, 0.70, 0.85, 0.85,
 1, 1, 1.00, 1.00, 1.00, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.85, 0.85,
 1, 1, 1.00, 1.00, 1.00, 0.80, 0.80, 0.80, 0.80, 0.80, 0.80, 0.85, 0.85,
 1, 1, 1.00, 1.00, 1.00, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85,
 1, 1, 1.00, 1.00, 1.00, 0.90, 0.90, 0.90, 0.90, 0.90, 0.90, 0.90, 0.90,
 1, 1, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00
};

int decision = 0;
int deep_now = 0;
const int large_num = 1000000;
int maxmin[15];
int count_cal = 0;
int abcut = 0;	//剪枝系数，精确时取0
float cut_k = 0.2;	//剪枝系数，精确时取1
int hit_count = 0;

int score_king[11][10] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 5, 5, 5, 0, 0, 0,
	};

int score_bishop[11][10] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0,-3, 0, 0, 0, 6, 0, 0, 0,-3,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 2, 0, 0, 0, 2, 0, 0,
	};

int score_knight[11][10] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 4, 7, 8, 7, 6, 7, 8, 7, 4,
		0, 5, 9,15, 7, 7, 7,15, 9, 5,
		0, 7,10,12,14,12,14,12,10, 7,
		0, 4, 8, 6, 7, 5, 7, 6, 8, 4,
		0, 2, 8, 8, 8, 8, 8, 8, 8, 2,
		0, 2, 8, 9, 8, 8, 8, 9, 8, 2,
		0, 0, 7, 7, 6, 8, 6, 7, 7, 0,
		0, 0, 4, 3, 4, 4, 4, 3, 4, 0,
		0,-2, 2, 2, 2,-7, 2, 2, 2,-2,
		0,-3,-3, 0, 0, 0, 0, 0,-3,-3,
	};

int score_cannon[11][10] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0,12, 9, 3, 0, 0, 0, 3, 9,12,
		0, 3, 2, 2, 3, 1, 3, 2, 2, 3,
		0, 3, 2, 1, 1, 2, 1, 1, 2, 3,
		0, 2, 2, 1, 1,12, 1, 1, 2, 2,
		0, 1, 1, 1, 1,11, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 7, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 4, 1, 1, 1, 1,
		0, 5, 5, 5, 6, 7, 6, 5, 5, 5,
		0, 0, 1, 1, 0, 1, 0, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

int score_rook[11][10] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0,11,15, 6,15, 6,15, 6,15,11,
		0,10,15,12,16,10,16,12,15,10,
		0,10,11,11,12,10,12,11,11,10,
		0, 2,10, 2,10, 4,10, 2,10, 2,
		0, 2,10, 2,10, 2,10, 2,10, 2,
		0, 2,12, 2,15, 2,15, 2,12, 2,
		0, 2, 6, 2, 6, 2, 6, 2, 6, 2,
		0, 3, 7, 3, 7, 3, 7, 3, 7, 3,
		0, 4, 8, 4, 2, 2, 2, 4, 8, 4,
		0,-5, 6, 2, 8, 2, 8, 2, 6,-5,
	};

int score_pawn[11][10] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 3, 6,11, 6,11, 6, 3, 1,
		0,18,22,27,30,30,30,27,22,18,
		0,18,22,27,32,32,32,27,22,18,
		0,17,20,22,27,27,27,22,20,17,
		0,16,17,21,25,25,25,21,17,16,
		0, 1, 0, 7, 0,12, 0, 7, 0, 1,
		0, 0, 0,-2, 0,12, 0,-2, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

//以下函数原型和数据结构声明
int prejudge(int color, int deep, bool from_max);

/*以下V2.0新增的函数体和数据结构
	该版本更新旨在通过记录表的方式存储已经计算过的结点估值，
	从而尽量避免重复计算和结点重复的搜索*/

class packed_board {	//压缩过的盘面，包含盘面现状和轮谁先手
private:
	int info[6];	/*	共6 * 32 = 192位，前96位记录红子的位置，后96位记录黑子的位置
			int[0]从高位到低位分别为黑车1、车2、马1、马2、将（7+7+7+7+4）
			int[1]从高位到低位分别为黑炮1、炮2、兵1、兵2、士1、红黑先手位（1为红）（7+7+7+7+3+1）
			int[2]从高位到低位分别为黑兵3、兵4、兵5、士2、相1、相2、空缺位（7+7+7+3+3+3+2）
			int[3]从高位到低位分别为红车1、车2、马1、马2、将（7+7+7+7+4）
			int[4]从高位到低位分别为红炮1、炮2、兵1、兵2、士1、空缺位（7+7+7+7+3+1）
			int[5]从高位到低位分别为红兵3、兵4、兵5、士2、相1、相2、空缺位（7+7+7+3+3+3+2）
			位置的表示法：
			任何子力，0表示已阵亡；
			车马炮兵：行数i，列数j，则位置被表示为(10 * i + j)；
			红将：(8,4),(8,5),(8,6),(9,4),(9,5),(9,6),(10,4),(10,5),(10,6)分别被表示为1-9；
			黑将：(1,4),(1,5),(1,6),(2,4),(2,5),(2,6),(3,4),(3,5),(3,6)分别被表示为1-9；
			红士：(8,4),(8,6),(9,5),(10,4),(10,6)分别被表示为1-5；
			黑士：(1,4),(1,6),(2,5),(3,4),(3,6)分别被表示为1-5；
			红相：(6,3),(6,7),(8,1),(8,5),(8,9),(10,3),(10,7)分别被表示为1-7；
			黑相：(1,3),(1,7),(3,1),(3,5),(3,9),(5,3),(5,7)分别被表示为1-7；
			*/
private:
	inline int shift_right(int val, int dgt) {	//右移正负位
		if (dgt >= 0)
			return val >> dgt;
		return val << (-dgt);
	};
	inline void put(int val, int len, int arr_idx, int dgt_idx) {	//把val的后len位放进info[arr_idx]的以dgt_idx为最低位的位集合里
		int tmp = ((int)0x80000000 >> (31 - dgt_idx)) ^ (shift_right((int)0x80000000, 31 - dgt_idx - len));
		info[arr_idx] |= tmp;
		info[arr_idx] &= ((~tmp) | (val << dgt_idx));
		return;
	};
	inline int get(int len, int arr_idx, int dgt_idx) {	//把info[arr_idx]的以dgt_idx为最低位的长为len的值读出
		int tmp = ((int)0x80000000 >> (31 - dgt_idx)) ^ (shift_right((int)0x80000000, 31 - dgt_idx - len));
		tmp &= info[arr_idx];
		return ((unsigned)tmp) >> dgt_idx;
	};
	inline int get_value(int type, int i, int j) {	//根据(i,j)把type类型棋子的位置编码返回
		switch (type) {
		case 1: return 3 * i + j - 27;	//红将
		case 2: return (3 * i + j - 26) / 2;	//红士
		case 3: return (4 * i + j) / 3 - 8;	//红相，取整除法！
		case 11: return 3 * i + j - 6;	//黑将
		case 12: return (3 * i + j - 5) / 2;	//黑士
		case 13: return (4 * i + j - 1) / 3 - 1;	//黑相
		default: return 10 * i + j;	//车，马，炮，兵
		}
	};
	inline int get_len(int type) {	//返回type类型棋子pack后占据位数
		int len[18] = {0, 4, 3, 3, 7, 7, 7, 7, 0, 0, 0, 4, 3, 3, 7, 7, 7, 7};
		return len[type];
	};
	inline int get_arridx(int type, int id) {	//返回type类型第id个(从1开始)棋子pack后的arr_idx
		int arridx[18] = { 0, 3, 4, 5, 3, 3, 4, 4, 0, 0, 0, 0, 1, 2, 0, 0, 1, 1 };
		int tmp = arridx[type];
		if (((type == 2) || (type == 12)) && (id == 2))	//第2个士
			++tmp;
		else if (((type == 7) || (type == 17)) && (id >= 3))	//第3，4，5个兵
			++tmp;
		return tmp;
	};
	inline int get_dgtidx(int type, int id) {	//返回type类型第id个(从1开始)棋子pack后的dgt_idx
		int dgtidx[17] = {0, 0, 1, 8, 5, 2, 25, 18, 11, 4, 25, 18, 11, 4, 25, 18, 11};
		return dgtidx[2 * (type % 10) + id - 3];
	};
public:
	void print() {
		for (int i = 0; i < 6; ++i)
			printf("%x ", info[i]);
		cout << endl;
		return;
	};
	void pack(const int board[][10], const int red_black) {	//从board压缩到这里，1为红
		for (int i = 0; i < 6; ++i)
			info[i] = 0;
		int cnt[18] = {0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1 };
		for (int i = 1; i <= 10; ++i) {
			for (int j = 1; j <= 9; ++j) {
				int tmp = board[i][j];
				if (tmp) {	//有棋子
					put(get_value(tmp, i, j), get_len(tmp), get_arridx(tmp, cnt[tmp]), get_dgtidx(tmp, cnt[tmp]));
					++cnt[tmp];
				}
			}
		}
		//红黑先手位（1为红）
		put(red_black, 1, 1, 0);
		return;
	};
	void checked_pack(const int board[][10], const int red_black) {	//做unpack检验，用于debug，效率很低，不过无所谓
		this->pack(board, red_black);
		//unpack
		int unpacked[11][10];
		for (int i = 0; i < 11; ++i) {
			for (int j = 0; j < 10; ++j)
				unpacked[i][j] = 0;
		}
		int cnt[18] = { 0, 1, 2, 2, 2, 2, 2, 5, 0, 0, 0, 1, 2, 2, 2, 2, 2, 5 };
		for (int type = 0; type < 18; ++type) {
			if (cnt[type] > 0) {
				int val = get(get_len(type), get_arridx(type, cnt[type]), get_dgtidx(type, cnt[type]));
				if (val) {	//未阵亡
					int i, j;
					switch (type) {
					case 1:
						switch (val) {
						case 1:	i = 8; j = 4; break;
						case 2:	i = 8; j = 5; break;
						case 3:	i = 8; j = 6; break;
						case 4:	i = 9; j = 4; break;
						case 5:	i = 9; j = 5; break;
						case 6:	i = 9; j = 6; break;
						case 7:	i = 10; j = 4; break;
						case 8:	i = 10; j = 5; break;
						case 9:	i = 10; j = 6; break;
						default: cout << "pack err1!" << endl; system("pause");
						}
						break;
					case 2:
						switch (val) {
						case 1:	i = 8; j = 4; break;
						case 2:	i = 8; j = 6; break;
						case 3:	i = 9; j = 5; break;
						case 4:	i = 10; j = 4; break;
						case 5:	i = 10; j = 6; break;
						default: cout << "pack err1!" << endl; system("pause");
						}
						break;
					case 3:
						switch(val) {
						case 1:	i = 6; j = 3; break;
						case 2:	i = 6; j = 7; break;
						case 3:	i = 8; j = 1; break;
						case 4:	i = 8; j = 5; break;
						case 5:	i = 8; j = 9; break;
						case 6:	i = 10; j = 3; break;
						case 7:	i = 10; j = 7; break;
						default: cout << "pack err1!" << endl; system("pause");
						}
						break;
					case 11:
						switch (val) {
						case 1:	i = 1; j = 4; break;
						case 2:	i = 1; j = 5; break;
						case 3:	i = 1; j = 6; break;
						case 4:	i = 2; j = 4; break;
						case 5:	i = 2; j = 5; break;
						case 6:	i = 2; j = 6; break;
						case 7:	i = 3; j = 4; break;
						case 8:	i = 3; j = 5; break;
						case 9:	i = 3; j = 6; break;
						default: cout << "pack err1!" << endl; system("pause");
						}
						break;
					case 12:
						switch (val) {
						case 1:	i = 1; j = 4; break;
						case 2:	i = 1; j = 6; break;
						case 3:	i = 2; j = 5; break;
						case 4:	i = 3; j = 4; break;
						case 5:	i = 3; j = 6; break;
						default: cout << "pack err1!" << endl; system("pause");
						}
						break;
					case 13:
						switch (val) {
						case 1:	i = 1; j = 3; break;
						case 2:	i = 1; j = 7; break;
						case 3:	i = 3; j = 1; break;
						case 4:	i = 3; j = 5; break;
						case 5:	i = 3; j = 9; break;
						case 6:	i = 5; j = 3; break;
						case 7:	i = 5; j = 7; break;
						default: cout << "pack err1!" << endl; system("pause");
						}
						break;
					default:
						i = val / 10;
						j = val % 10;
					}
					unpacked[i][j] = type;
				}
				--cnt[type];
				if (cnt[type])
					--type;
			}
		}
		//compare
		for (int i = 0; i < 11; ++i) {
			for (int j = 0; j < 10; ++j) {
				if (board[i][j] != unpacked[i][j]) {
					cout << "pack err2!" << endl;
					system("pause");
					this->print();
					cout << "ori: " << endl;
					print_field(field);
					system("pause");
					cout << "pack: " << endl;
					print_field(unpacked);
					system("pause");
				}
			}
		}
		return;
	};
	friend const bool operator < (const packed_board& b1, const packed_board& b2) {	//map重载小于号
		for (int i = 0; i < 6; ++i) {
			if (b1.info[i] < b2.info[i])
				return 1;
			if (b1.info[i] > b2.info[i])	//!!!!!!!!!
				return 0;
		}
		return 0;	//1的话，任何都不相等；应该是0
	};
};

struct child_node {	//子节点链表结点，暂不启用。
public:
	short value;
	child_node* next;	//NULL表示链表尾，注意释放空间！
};

class board_info {	//盘面信息，暂不启用子节点链表
private:
	int info[3];	/*每个int，包括深度（直接求score，深度为1）（最高8位），分数（24位）
					val[0]为深度为1的值，val[1]为m，val[2]为M*/
public:
	board_info(){
		for(int i = 0; i < 3; ++i)
			info[i] = 0;
	};
	int read_depth(int mode) {	//直接求score，深度为1。
		int tmp = ((int)0x80000000) >> 7;
		return ((unsigned)(tmp & info[mode])) >> 24;
	};
	void write_depth(int mode, int val) {	//直接求score，深度为1。
		int tmp = ((int)0x80000000) >> 7;
		info[mode] &= (~tmp);
		info[mode] |= (val << 24);
		return;
	};
	int read_score(int mode) {
		int tmp = ((int)0x80000000) >> 7;
		int res = info[mode] & (~tmp);
		if (res & 0x800000)	//负数的处理！！！！！！！！！！
			res |= 0xff000000;
		return res;
	};
	void write_score(int mode, int val) {
		int tmp = ((int)0x80000000) >> 7;
		info[mode] &= tmp;
		info[mode] |= val;
		return;
	};
	void make_blank(int mode) {
		info[mode] = 0;
		return;
	};
	bool is_blank(int mode) {
		if (info[mode] == 0)
			return 1;
		return 0;
	};
};

class record_table {	//记录表
	//由于剪枝得不到精确值，但可以存储上下界！！
	//优化：红黑先手的分数判断需要更改；剪枝可以设定近似剪枝系数；应该避免重复计算score（预计算和计算），2步、3步之间share的数据可以提高剪枝效率；push，find合并避免降低效率？避免长捉？？debug........
private:
	map<packed_board, board_info> table;	//map是用红黑树实现的，时间复杂度O(lgn)
	queue<packed_board> table_seq;	//暂时用queue来实现替换策略，以后可以考虑用set
public:
	unsigned max_volume;	//最大容量
private:
	void traversemap() {
		map<packed_board, board_info>::iterator iter;
		cout << "traverse map: " << endl;
		for (iter = table.begin(); iter != table.end(); iter++) {
			packed_board tmp = iter->first;
			tmp.print();
		}
		cout << endl;
		return;
	};
	int power(int expo) {
		int res = 1;
		while (expo--)
			res *= 3;
		return res;
	};
public:
	record_table() {
		max_volume = cut_k * 250 * power(deep_limit);	//实验表明，6000000这个容量限度大概消耗内存800Mb
		table.clear();
		while (!table_seq.empty())
			table_seq.pop();
		return;
	};
	board_info find(const int board[][10], const int red_black) {	//找到返回board_info，否则返回空的board_info，以后优化的话可以分情况直接插入赋值，1为红先手
		packed_board obj;
		obj.pack(board, red_black);
		map<packed_board, board_info>::iterator it = table.find(obj);
		if (it != table.end()) {	//找到
			board_info tmp = it->second;
			return tmp;
		}
		//没找到
		board_info blank;
		return blank;
	};
	void push(const int board[][10], const int red_black, const int depth, const int score, int mode ) {	//压入记录表，也可用于更改已有的值，red-black统一为1
		//替换策略
		if (table.size() >= max_volume) {	//满
			map<packed_board, board_info>::iterator it;
			do {
				packed_board pb = table_seq.front();
				table_seq.pop();
				it = table.find(pb);
			} while (it == table.end());	//do-while考虑到有map已删除而seq未删除的情况：修改已有结点值！！
			table.erase(it);
		}
		//以下插入
		board_info info;
		info.write_depth(mode, depth);
		info.write_score(mode, score);
		packed_board pboard;
		pboard.pack(board, red_black);
		map<packed_board, board_info>::iterator it = table.find(pboard);
		if (it == table.end()) {	//本来没有
			table_seq.push(pboard);
			table[pboard] = info;
		}
		else {	//本来有，判断一下深度大小比较，决定是否更新
			if (it->second.read_depth(mode) < depth) {
				it->second.write_depth(mode, depth);
				it->second.write_score(mode, score);
			}
		}
		return;
	};
	int get_size() {
		return table.size();
	};
};

record_table rectab;

//以下V1中的函数体，在V2.0中做了更新
//V2.1中进行迭代深度预判剪枝(probcut)

//const int power_score[8] = {0, 100000, 21, 20, 110, 45, 45, 3};
//const int power_score[8] = { 0, 100000, 32, 30, 165, 69, 69, 8 };
const int power_score[9] = { 0, 100000, 22, 20, 165, 69, 69, 8, 0 };	//最后一项是捉子分

int single_score(int color) {	//单边
	++count_cal;	//for trial
	int score = 0;
	/*
	for (int i = 1; i <= 10; ++i) {
		for (int j = 1; j <= 9; ++j) {
			if (get_color(field[i][j]) == color) {
				int type = field[i][j] % 10;
				int ii = i;
				if (color == 0)
					ii = 11 - i;
				score += power_score[type];
				switch (type) {
				case 1: score += score_king[ii][j]; break;
				case 2: break;
				case 3: score += score_bishop[ii][j]; break;
				case 4: score += score_rook[ii][j]; break;
				case 5: score += score_knight[ii][j]; break;
				case 6: score += score_cannon[ii][j]; break;
				case 7: int plus = (turns >> 3); if (plus > 16) plus = 16; score += plus; score += score_pawn[ii][j]; break;	//与turns有关，考虑rectab？？！！
				}
			}
		}
	}
	return score;
	*/
	//V4.0 Update
	return cnn_score(color);
}

int score(int color) {	//双边，红-黑
	//board_info info = rectab.find(field, 1);
	//if (info.is_blank(0)) {	//没有这条记录
		int red_score = single_score(1) - single_score(0);
	//	rectab.push(field, 1, 1, red_score, 0);
		if (color)	//红
			return red_score;
		else	//黑
			return -red_score;
	//}
	//有这条记录
	//int red_score = info.read_score(0);
	//if (color)	//红
	//	return red_score;
	//else	//黑
	//	return -red_score;
}

int random(int color) {
	get_valid_moves(color);
	int size = valid_moves.size();
	//reference: http://zhidao.baidu.com/link?url=Vmc3IgT1G_Houa7--AGtE_16BPl8d5SbzGOspv9B-lSAcL8PY_QTjh5pthya1aE-EkZN9XKjG828AW0E9smwg6mhX5rQJr-npYGOlb_SKsO
	srand((unsigned)time(NULL)); //用时间做种，每次产生随机数不一样 
	int num = rand() % (size - 1);  //产生随机数 
	move(valid_moves[num]);
	return valid_moves[num];
}

int greedy(int color) {
	get_valid_moves(color);
	int size = valid_moves.size();
	int num = 0;
	int save[11][10];
	int max_score = -1000000;
	int max_num = size - 1;
	for (num = size - 1; num >= 0; --num) {
		for (int i = 0; i <= 10; ++i) {
			for (int j = 0; j <= 9; ++j)
				save[i][j] = field[i][j];
		}
		move(valid_moves[num]);
		int tmp = score(color);
		if (tmp > max_score) {
			max_score = tmp;
			max_num = num;
		}
		for (int i = 0; i <= 10; ++i) {
			for (int j = 0; j <= 9; ++j)
				field[i][j] = save[i][j];
		}
	}
	move(valid_moves[max_num]);
	return valid_moves[max_num];
}

struct move_info {
public:
	int director;
	int score;
	friend bool operator < (const move_info& x, const move_info& y) {
		return x.score < y.score;
	};
	move_info(int dir, int sco) : director(dir), score(sco) {};
};

int search_min(int color)	//a-b减枝DFS搜索算法
{
	int search_max(int color);
	//是否结束搜索
	int king = 0;
	for (int i = 1; i <= 10; ++i) {
		for (int j = 1; j <= 9; ++j) {
			if (field[i][j] % 10 == 1)
				++king;
		}
	}
	if (king < 2 || deep_now >= deep_limit)
		return score(color);
	//如果不结束搜索
	int min = large_num; //已搜索到的最佳走法的估值
	//读入rectab
	if (cache) {
		board_info info = rectab.find(field, 1);
		if (color) {	//红
			if ((!info.is_blank(2)) && (info.read_depth(2) == 10 + deep_limit - deep_now)) {	//为什么>=不对。。。
				min = info.read_score(2);
				++hit_count;
			}
		}
		else {	//黑
			if ((!info.is_blank(1)) && (info.read_depth(1) == 10 + deep_limit - deep_now)) {
				min = -info.read_score(1);
				++hit_count;
			}
		}
	}
	//存储初始状态
	int save[11][10]; //用于存储下电脑没有落子时的局面
	for (int i = 1; i <= 10; ++i) {
		for (int j = 1; j <= 9; ++j)
			save[i][j] = field[i][j];
	}
	//以下进行预评价局面，以优化搜索顺序，从而提升剪枝效率，以后最后一层应该利用数据！！
	vector<move_info> list;
	get_valid_moves(1 - color);
	int size = valid_moves.size(); //一共可行的走子数量
	for (int p = 0; p < size; ++p)
		list.push_back(move_info(valid_moves[p], 0));
	valid_moves.clear();
	for (int p = 0; p < size; ++p) {
		int eat = move(list[p].director); //假设落子
		list[p].score = prejudge(color, deep_limit - deep_now, 0) - eat_k * power_score[eat % 10];	//用于避免一些bug？？，不是+ !!!!!
		for (int i = 1; i <= 10; ++i) { //回溯
			for (int j = 1; j <= 9; ++j)
				field[i][j] = save[i][j];
		}
	}
	//排序
	sort(list.begin(), list.end());
	//以下寻找最佳落子点
	float cut = cuttab[cuttab_index][deep_limit - deep_now];
	for (int p = 0; p < cut * (float)size; ++p)
	{
		move(list[p].director);
		maxmin[deep_now] = min; //存储节点数据，准备剪枝
		++deep_now;
		int temp = search_max(color); //调用假想敌函数
		--deep_now; //回溯
		if (temp < min) //更优的走子，小于等于号是不对的，会使剪枝出错
			min = temp;
		for (int i = 1; i <= 10; ++i) { //回溯
			for (int j = 1; j <= 9; ++j)
				field[i][j] = save[i][j];
		}
		for (int step = deep_now - 1; step >= 0; step = step - 2) { //是0的话给出的解是精确的，但对每个点给出的估值不准确	
			if (min <= maxmin[step] + abcut) { //alpha剪枝
				//存入rectab
				if (cache) {
					if (color)	//红
						rectab.push(field, 1, 10 + deep_limit - deep_now, min, 2);	//不是从1开始的depth
					else	//黑
						rectab.push(field, 1, 10 + deep_limit - deep_now, -min, 1);
				}
				return min;
			}
		}
	}
	//存入rectab
	if (cache) {
		if (color) {	//红
			rectab.push(field, 1, 10 + deep_limit - deep_now, min, 2);	//不是从1开始的depth
			rectab.push(field, 1, 10 + deep_limit - deep_now, min, 1);
		}
		else {	//黑
			rectab.push(field, 1, 10 + deep_limit - deep_now, -min, 1);
			rectab.push(field, 1, 10 + deep_limit - deep_now, -min, 2);
		}
	}
	return min;
}

int search_max(int color)
{
	//是否结束搜索
	int king = 0;
	for (int i = 1; i <= 10; ++i) {
		for (int j = 1; j <= 9; ++j) {
			if (field[i][j] % 10 == 1)
				++king;
		}
	}
	if (king < 2 || deep_now >= deep_limit)
		return score(color);
	//如果不结束搜索
	int max = -large_num; //已搜索到的最佳走法的估值
	//读入rectab
	if (cache) {
		board_info info = rectab.find(field, 1);
		if (color) {	//红
			if ((!info.is_blank(1)) && (info.read_depth(1) == 10 + deep_limit - deep_now)) {
				max = info.read_score(1);
				++hit_count;
			}
		}
		else {	//黑
			if ((!info.is_blank(2)) && (info.read_depth(2) == 10 + deep_limit - deep_now)) {
				max = -info.read_score(2);
				++hit_count;
			}
		}
	}
	//存储初始状态
	int save[11][10]; //用于存储下电脑没有落子时的局面
	for (int i = 1; i <= 10; ++i) {
		for (int j = 1; j <= 9; ++j)
			save[i][j] = field[i][j];
	}
	//以下进行预评价局面，以优化搜索顺序，从而提升剪枝效率
	vector<move_info> list;
	get_valid_moves(color);
	int size = valid_moves.size(); //一共可行的走子数量
	for (int p = 0; p < size; ++p)
		list.push_back(move_info(valid_moves[p], 0));
	valid_moves.clear();
	for (int p = 0; p < size; ++p) {
		int eat = move(list[p].director); //假设落子
		list[p].score = prejudge(color, deep_limit - deep_now, 1) + eat_k * power_score[eat % 10];
		for (int i = 1; i <= 10; ++i) { //回溯
			for (int j = 1; j <= 9; ++j)
				field[i][j] = save[i][j];
		}
	}
	sort(list.begin(), list.end()); //排序
									//以下寻找最佳落子点
	float cut = cuttab[cuttab_index][deep_limit - deep_now];	//不能用全局cut，递归返回时会被修改！！！
	int best_move = 0;
	for (int p = size - 1; p >= ((float)1 - cut) * ((float)size - (float)1); --p)	//不是cut * (size - 1)!!!!!!!!!
	{
		move(list[p].director);
		maxmin[deep_now] = max; //存储节点数据，准备剪枝
		++deep_now;
		int temp = search_min(color); //调用假想敌函数
		--deep_now; //回溯
		if (temp > max) { //更优的走子，大于等于号是不对的，会使剪枝出错
			max = temp;
			best_move = p;
		}
		for (int i = 1; i <= 10; ++i) { //回溯
			for (int j = 1; j <= 9; ++j)
				field[i][j] = save[i][j];
		}
		for (int step = deep_now - 1; step >= 0; step = step - 2) { //是0的话给出的解是精确的，但对每个点给出的估值不准确
			if (max >= maxmin[step] - abcut) { //beta剪枝
				//存入rectab
				if (cache) {
					if (color)	//红
						rectab.push(field, 1, 10 + deep_limit - deep_now, max, 1);	//不是从1开始的depth
					else	//黑
						rectab.push(field, 1, 10 + deep_limit - deep_now, -max, 2);
				}
				decision = list[best_move].director;
				return max;
			}
		}
	}
	//存入rectab
	if (cache) {
		if (color) {	//红
			rectab.push(field, 1, 10 + deep_limit - deep_now, max, 1);	//不是从1开始的depth
			rectab.push(field, 1, 10 + deep_limit - deep_now, max, 2);
		}
		else {	//黑
			rectab.push(field, 1, 10 + deep_limit - deep_now, -max, 2);
			rectab.push(field, 1, 10 + deep_limit - deep_now, -max, 1);
		}
	}
	decision = list[best_move].director;
	return max;
}

int AI_probcut(int color) {
	//输出调试信息
	if (debug2) {
		cout << "count_cal = " << count_cal << ";    ";
		cout << "rectab = " << rectab.get_size() << ";    ";
		cout << "hit_count = " << hit_count << ";    ";
		cout << "deep_limit = " << deep_limit << ";    ";
		cout << "cuttab_index = " << cuttab_index << endl;
		cout << "eval = " << eval << ";    " << endl;
	}
	hit_count = 0;
	count_cal = 0;
	//cout << "您的对手probcut正在思考，请等待 5 ~ 15 秒..." << endl;
	deep_now = 0;
	eval = search_max(color);
	if (debug) {
		//freopen("out.csv", "a+", stdout);
		cout << "turns = " << turns << punctuation;
		cout << "eval = " << eval << punctuation;
		cout << "count_cal = " << count_cal << punctuation;
		cout << "rectab = " << rectab.get_size() << punctuation;
		cout << "hit_count = " << hit_count << punctuation;
		cout << "deep_limit = " << deep_limit << punctuation;
		cout << "cuttab_index = " << cuttab_index << endl;
		cout << turns << punctuation;
		cout << eval << punctuation;
		cout << count_cal << punctuation;
		cout << rectab.get_size() << punctuation;
		cout << hit_count << punctuation;
		cout << deep_limit << punctuation;
		cout << cuttab_index << endl;
		system("pause");
	}
	/*自适应搜索深度、广度
	if (turns <= 6)
		deep_limit = 8;	//运行时也可以为6
	else if (turns <= 8)
		deep_limit = 8;
	else if ((count_cal < time_limit_k) && (turns > 1))
		++deep_limit_cnt;
	if (deep_limit_cnt >= time_limit_t) {
		if (cuttab_index == cuttabR - 1) {
			cuttab_index = cuttabJ;
			deep_limit += 1;
			rectab.max_volume *= 3;
			eat_k = 0;
		}
		else {
			++cuttab_index;
		}
		deep_limit_cnt = 0;
	}*/
	return decision;
}

int AI_alphabeta(int color) {
	//save and reset
	int save_deep_limit = deep_limit;
	deep_limit = 6;
	int save_cuttab_index = cuttab_index;
	cuttab_index = cuttabR - 1;
	//输出调试信息
	if (debug2) {
		cout << "count_cal = " << count_cal << ";    ";
		cout << "rectab = " << rectab.get_size() << ";    ";
		cout << "hit_count = " << hit_count << ";    ";
		cout << "deep_limit = " << deep_limit << ";    ";
		cout << "cuttab_index = " << cuttab_index << endl;
		cout << "eval = " << eval << ";    ";
	}
	hit_count = 0;
	count_cal = 0;
	//cout << "您的对手alpha-beta正在思考，请等待 5 ~ 15 秒..." << endl;
	deep_now = 0;
	eval = search_max(color);
	if (debug) {
		//freopen("out.csv", "a+", stdout);
		/*cout << "turns = " << turns << punctuation;
		cout << "eval = " << eval << punctuation;
		cout << "count_cal = " << count_cal << punctuation;
		cout << "rectab = " << rectab.get_size() << punctuation;
		cout << "hit_count = " << hit_count << punctuation;
		cout << "deep_limit = " << deep_limit << punctuation;
		cout << "cuttab_index = " << cuttab_index << endl;*/
		cout << turns << punctuation;
		cout << eval << punctuation;
		cout << count_cal << punctuation;
		cout << rectab.get_size() << punctuation;
		cout << hit_count << punctuation;
		cout << deep_limit << punctuation;
		cout << cuttab_index << endl;
		system("pause");
	}
	//restore
	deep_limit = save_deep_limit;
	cuttab_index = save_cuttab_index;
	return decision;
}

int AI(int color, int test_mode) {	//test_mode为1，执行procut否则执行朴素alpha-beta 
	if (test_mode)
		return AI_probcut(color);
	else
		return AI_alphabeta(color);
}

//以下V2.1新增数据结构和函数体
int prejudge(int color, int deep, bool from_max) {	//预判函数,deep表示待展开层数,deep为1直接即直接求score
	if (deep == 1)	//不求值，避免重复计算!!!!!
		return 0;
	if (deep <= 4)	//4.直接求值,代码齐全后应该为3，改为100?
		return score(color);
	//预判深度大于0
	//保存全局数据结构
	int save_field[11][10];
	for (int i = 0; i < 11; ++i) {
		for (int j = 0; j < 10; ++j)
			save_field[i][j] = field[i][j];
	}
	int save_deep_limit = deep_limit;
	int save_turns = turns;
	int save_deep_now = deep_now;
	int save_maxmin[15];
	for (int i = 0; i < 15; ++i)
		save_maxmin[i] = maxmin[i];
	int save_abcut = abcut;
	//进行迭代预判的alpha-beta剪枝搜索
	int res = 0;

	deep_limit = deep - 3;	//可以尝试deep - 1，2，3，4；理论上应该是2
	deep_now = 0;
	for (int i = 0; i < 15; ++i)
		maxmin[i] = 0;
	//abcut = 0;
	//cut = 1;
	if (from_max)
		res = search_min(color);
	else
		res = search_max(color);

	//恢复全局数据结构
	for (int i = 0; i < 11; ++i) {
		for (int j = 0; j < 10; ++j)
			field[i][j] = save_field[i][j];
	}
	deep_limit = save_deep_limit;
	turns = save_turns;
	deep_now = save_deep_now;
	for (int i = 0; i < 15; ++i)
		maxmin[i] = save_maxmin[i];
	abcut = save_abcut;
	//返回运算结果
	return res;
}

/*********************************************************************
		V4.0 Update: Scored with CNNs
*********************************************************************/

/*******declarations*******/
int state[1260];

int cnn();
void state_transfer();

/*******functions*******/
int cnn_score(int color){
	Py_Initialize();
	state_transfer();

	cout << "transfer succeed!" << endl;

	if(color == 1)
		return cnn();
	else
		return -cnn();
	Py_Finalize();
}

void state_transfer(){	//write field to array: state[1260]
	for (int i = 0; i < 1260; ++i)
		state[i] = 0;
	int trans[18] = { 0,0,1,2,3,4,5,6,0,0,0,7,8,9,10,11,12,13 };
	for (int i = 1; i <= 10; ++i) {
		for (int j = 1; j <= 9; ++j) {
			if (field[i][j]) {
				int p = trans[field[i][j]] * 90 + 9 * (i - 1) + (j - 1);
				state[p] = 1;
			}
		}
	}
	return;
}

/*
本函数修改自知乎：
作者：Jerry Jho
链接：https://www.zhihu.com/question/23003213/answer/56121859
*/
int cnn() {
    int res = 0;
    PyObject *pModule,*pFunc;
    PyObject *pArgs, *pValue, *pName, *pDict;

	PyRun_SimpleString("import sys");  
    //PyRun_SimpleString("print '---import sys---'");   
 
    PyRun_SimpleString("sys.path.append('./')"); 

	PyRun_SimpleString("import numpy");  
    PyRun_SimpleString("print '---import numpy---'");  

	//PyRun_SimpleString("import tensorflow");  
    //PyRun_SimpleString("print '---import tensorflow---'"); 

	cout << "file preparing!" << endl;
    
	//http://www.cnblogs.com/apexchu/p/5015961.html
    // 载入名为pytest的脚本  
    pName = PyString_FromString("cnn_foruse");  
    pModule = PyImport_Import(pName);  
    if ( !pModule ) {  
        printf("can't find cnn_foruse.py");  
        getchar();  
        return -1;  
    }  
    pDict = PyModule_GetDict(pModule);  
    if ( !pDict ) {  
        return -1;  
    }  
  
    // 找出函数名为add的函数  
    printf("----------------------\n");  
    pFunc = PyDict_GetItemString(pDict, "cnn");  
    if ( !pFunc || !PyCallable_Check(pFunc) ) {  
        printf("can't find function [cnn]");  
        getchar();  
        return -1;  
    }  
    
    /* build args */

	cout << "arg preparing!" << endl;

    pArgs = PyTuple_New(1260);
	for(int i = 0; i < 1260; ++i)
    	PyTuple_SetItem(pArgs,i, PyInt_FromLong(state[i]));
	
	cout << "arg succeed!" << endl;

    /* call */
    pValue = PyObject_CallObject(pFunc, pArgs);

	cout << "call py succeed!" << endl;
    
    res = PyInt_AsLong(pValue);
    return res;
}


