#include<iostream>
#include<stdio.h>
#include<vector>
#include<algorithm>
using namespace std;

extern int field[11][10];
extern int record[300];
extern int chase_record[300];	//用于记录是否有捉着，以便判断长捉
extern int turns;

extern vector<int> valid_moves; /*合法走子方法集合。
						 走子方法用一个6位整数表示：前2位为棋子类型代号，
						 3-4位为起始点坐标，5-6位为走子后坐标，
						 第一位可能是空位；坐标为“10”的记为“0”。*/

struct pos{ //位置结构
public:
	int i;
	int j;
	pos(int i_ = 0, int j_ = 0): i(i_), j(j_){};
	friend bool operator == (const pos& x, const pos& y){
		return (x.i == y.i) && (x.j == y.j) && (x.i != 0) && (x.j != 0);
	};
};

inline int get_color(int id){ //参数：兵种代号
	if(!id)
		return -1; //-1无子
	if(id < 10)
		return 1; //1红
	return 0; //0黑
}

inline int valid_pos(int i, int j){
	if(i > 0 && i <= 10 && j > 0 && j <= 9)
		return 1;
	return 0;
}

bool bailianjiang(){ //白脸将
	int black_i = 0, black_j = 0;
	for(int i = 1; i <= 3; ++i){
		for(int j = 4; j <= 6; ++j){
			if(field[i][j] == 11){
				black_i = i;
				black_j = j;
			}
		}
	}
	int red_i = 0, red_j = 0;
	for(int i = 8; i <= 10; ++i){
		for(int j = 4; j <= 6; ++j){
			if(field[i][j] == 1){
				red_i = i;
				red_j = j;
			}
		}
	}
	if(red_j != black_j)
		return 0;
	for(int i = black_i + 1; i < red_i; ++i){
		if(field[i][red_j])
			return 0;
	}
	return 1;
}

int recur(int director, bool king){ //循环着法，例如长将，长捉。此函数有缺陷。
	if (king)
		return 0;
	if(turns > 16){
		if((record[turns - 1] == record[turns - 5])
			//&& (record[turns - 1] == record[turns - 9])
			&& (record[turns - 2] == record[turns - 6])
			//&& (record[turns - 2] == record[turns - 10])
			&& (record[turns - 3] == record[turns - 7])
			//&& (record[turns - 3] == record[turns - 11])
			&& (director == record[turns - 4]))
			//&& (director == record[turns - 8]))
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

int push(int color, int type, int from_i, int from_j, int to_i, int to_j, bool king, bool check_long_chase){
	int move_with_info(int director);
	int move(int director);
	if(valid_pos(to_i,to_j) && get_color(field[to_i][to_j]) != color){
		int save[11][10];
		for(int i = 0; i <= 10; ++i){
			for(int j = 0; j <= 9; ++j)
				save[i][j] = field[i][j];
		}
		int director = (from_i << 12) + (from_j << 8) + (to_i << 4) + to_j;
		move(director);
		bool invalid = 0;
		if(bailianjiang())
			invalid = 1;
		for(int i = 0; i <= 10; ++i){
			for(int j = 0; j <= 9; ++j)
				field[i][j] = save[i][j];
		}
		if(!invalid){
			if(recur(director, king))
				return -1;
			if ((check_long_chase) && (turns > 18)){
				if (chase_record[turns - 2] && chase_record[turns - 4] &&
					chase_record[turns - 6] && chase_record[turns - 8] &&
					chase_record[turns - 10] && chase_record[turns - 12] &&
					chase_record[turns - 14]) {
					if ((move_with_info(director) == 24) && (!king)) {
						for (int i = 0; i <= 10; ++i) {
							for (int j = 0; j <= 9; ++j)
								field[i][j] = save[i][j];
						}
						return -1;
					}
					for (int i = 0; i <= 10; ++i) {
						for (int j = 0; j <= 9; ++j)
							field[i][j] = save[i][j];
					}
				}
			}
			valid_moves.push_back(director);
		}
		return invalid;
	}
	return -1;
}

void king(int i, int j, int color, bool check_long_chase){
	int di[4] = {-1,0,1,0};
	int dj[4] = {0,1,0,-1};
	for(int k = 0; k < 4; ++k){
		int to_i = i + di[k], to_j = j + dj[k];
		if(to_i > 3 && to_i < 8 || to_j < 4 || to_j > 6)
			continue;
		push(color,1,i,j,to_i,to_j,1,check_long_chase);
	}
	return;		
}

void guard(int i, int j, int color, bool check_long_chase){
	int di[4] = {-1,-1,1,1};
	int dj[4] = {-1,1,1,-1};
	for(int k = 0; k < 4; ++k){
		int to_i = i + di[k], to_j = j + dj[k];
		if(to_i > 3 && to_i < 8 || to_j < 4 || to_j > 6)
			continue;
		push(color,2,i,j,to_i,to_j,0,check_long_chase);
	}
	return;	
}

void bishop(int i, int j, int color, bool check_long_chase){
	int di[4] = {-2,-2,2,2};
	int dj[4] = {-2,2,2,-2};
	int obi[4] = {-1,-1,1,1}; //obstruct
	int obj[4] = {-1,1,1,-1};
	for(int k = 0; k < 4; ++k){
		int to_i = i + di[k], to_j = j + dj[k];
		int by_i = i + obi[k], by_j = j + obj[k]; //come by
		if(valid_pos(by_i,by_j) && !field[by_i][by_j]){
			if(color && to_i > 5 || !color && to_i < 6)
				push(color,3,i,j,to_i,to_j,0, check_long_chase);
		}
	}
	return;
}

void rook(int i, int j, int color, bool check_long_chase){
	int di[4] = {-1,0,1,0};
	int dj[4] = {0,1,0,-1};
	for(int k = 0; k < 4; ++k){
		int to_i = i + di[k], to_j = j + dj[k];
		while(valid_pos(to_i,to_j)){
			if(get_color(field[to_i][to_j]) == color)
				break;
			if(get_color(field[to_i][to_j]) == 1 - color){
				push(color,4,i,j,to_i,to_j,0, check_long_chase);
				break;
			}
			push(color,4,i,j,to_i,to_j,0, check_long_chase);
			to_i += di[k];
			to_j += dj[k];
		}
	}
	return;	
}

void knight(int i, int j, int color, bool check_long_chase){
	int di[8] = {-2,-1,1,2,2,1,-1,-2};
	int dj[8] = {1,2,2,1,-1,-2,-2,-1};
	int obi[8] = {-1,0,0,1,1,0,0,-1}; //obstruct
	int obj[8] = {0,1,1,0,0,-1,-1,0};
	for(int k = 0; k < 8; ++k){
		int to_i = i + di[k], to_j = j + dj[k];
		int by_i = i + obi[k], by_j = j + obj[k]; //come by
		if(valid_pos(by_i,by_j) && !field[by_i][by_j])
			push(color,5,i,j,to_i,to_j,0, check_long_chase);
	}
	return;
}

void cannon(int i, int j, int color, bool check_long_chase){
	int di[4] = {-1,0,1,0};
	int dj[4] = {0,1,0,-1};
	for(int k = 0; k < 4; ++k){
		int to_i = i + di[k], to_j = j + dj[k];
		int by = 0; //come by
		while(valid_pos(to_i,to_j)){
			if(by == 0){
				if(get_color(field[to_i][to_j]) == -1)
					push(color,6,i,j,to_i,to_j,0, check_long_chase);
				else
					by = 1;
			}
			else if(by == 1){
				if(get_color(field[to_i][to_j]) == 1 - color){
					push(color,6,i,j,to_i,to_j,0, check_long_chase);
					break;
				}
				else if(get_color(field[to_i][to_j]) == color)
					break;
			}
			to_i += di[k];
			to_j += dj[k];
		}
	}
	return;
}

void pawn(int i, int j, int color, bool check_long_chase){
	if(color){
		if(i > 5)
			push(color,7,i,j,i - 1,j,0, check_long_chase);
		else{
			push(color,7,i,j,i - 1,j,0, check_long_chase);
			push(color,7,i,j,i,j + 1,0, check_long_chase);
			push(color,7,i,j,i,j - 1,0, check_long_chase);
		}
	}
	else{
		if(i < 6)
			push(color,7,i,j,i + 1,j,0, check_long_chase);
		else{
			push(color,7,i,j,i + 1,j,0, check_long_chase);
			push(color,7,i,j,i,j + 1,0, check_long_chase);
			push(color,7,i,j,i,j - 1,0, check_long_chase);
		}
	}
	return;
}

int get_valid_moves(int color, bool check_long_chase = 1){ //参数：1红；0黑
	valid_moves.clear();
	for(int i = 1; i <= 10; ++i){
		for(int j = 1; j <= 9; ++j){
			if(field[i][j]){
				if(color == get_color(field[i][j])){
					int type = field[i][j] % 10;
					switch(type){
						case 1: king(i,j,color, check_long_chase); break;
						case 2: guard(i,j,color, check_long_chase); break;
						case 3: bishop(i,j,color, check_long_chase); break;
						case 4: rook(i,j,color, check_long_chase); break;
						case 5: knight(i,j,color, check_long_chase); break;
						case 6: cannon(i,j,color, check_long_chase); break;
						case 7: pawn(i,j,color, check_long_chase); break;
					}
				}
			}
		}
	}
	return 0;
}

int move(int director){ //参数：走子方法代号，返回吃子代号；
	int i = 4;
	int dgt[4] = {0, 0, 0, 0};
	while(i){
		--i;
		dgt[i] = (director >> ((3 - i) << 2)) & 15;
	}
	int ret = field[dgt[2]][dgt[3]];	//吃子代号
	field[dgt[2]][dgt[3]] = field[dgt[0]][dgt[1]];
	field[dgt[0]][dgt[1]] = 0;
	return ret;
}

int move_with_info(int director) { //参数：走子方法代号，返回吃子代号；不吃子但是捉子，返回代号20
	int i = 4;
	int dgt[4] = { 0, 0, 0, 0 };
	while (i) {
		--i;
		dgt[i] = (director >> ((3 - i) << 2)) & 15;
	}
	int ret = field[dgt[2]][dgt[3]];	//吃子代号
	field[dgt[2]][dgt[3]] = field[dgt[0]][dgt[1]];
	field[dgt[0]][dgt[1]] = 0;
	if (!ret) {	//未吃子，判断是否捉子
		//保存棋局
		int save[11][10];
		for (int i = 0; i < 11; ++i) {
			for (int j = 1; j < 10; ++j)
				save[i][j] = field[i][j];
		}
		vector<int> save_valid_moves = valid_moves;
		valid_moves.clear();
		//遍历延伸走子
		switch (field[dgt[2]][dgt[3]]) {
		case 1: king(dgt[2], dgt[3], 1, 0); break;
		case 2: guard(dgt[2], dgt[3], 1, 0); break;
		case 3: bishop(dgt[2], dgt[3], 1, 0); break;
		case 4: rook(dgt[2], dgt[3], 1, 0); break;
		case 5: knight(dgt[2], dgt[3], 1, 0); break;
		case 6: cannon(dgt[2], dgt[3], 1, 0); break;
		case 7: pawn(dgt[2], dgt[3], 1, 0); break;
		case 11: king(dgt[2], dgt[3], 0, 0); break;
		case 12: guard(dgt[2], dgt[3], 0, 0); break;
		case 13: bishop(dgt[2], dgt[3], 0, 0); break;
		case 14: rook(dgt[2], dgt[3], 0, 0); break;
		case 15: knight(dgt[2], dgt[3], 0, 0); break;
		case 16: cannon(dgt[2], dgt[3], 0, 0); break;
		case 17: pawn(dgt[2], dgt[3], 0, 0); break;
		}
		//遍历延伸走子
		while (!valid_moves.empty()) {
			int director = valid_moves.back();
			valid_moves.pop_back();
			if (move(director)) {
				ret = 20;
				break;
			}	//不回溯成原始棋局，最后统一回溯
		}
		//回溯棋局
		for (int i = 0; i < 11; ++i) {
			for (int j = 1; j < 10; ++j)
				field[i][j] = save[i][j];
		}
		valid_moves = save_valid_moves;
	}
	return ret;
}

int player_move(int cmd){
	int i = 4;
	int dgt[4] = {0, 0, 0, 0};
	bool b = 0;
	if(cmd < 1000)
		b = 1;
	while(i){
		--i;
		dgt[i] = cmd % 10;
		cmd -= dgt[i];
		cmd /= 10;
		if(!dgt[i])
			dgt[i] = 10;
	}
	if(b)
		dgt[0] = 10;
	int color = get_color(field[dgt[0]][dgt[1]]);
	int type = field[dgt[0]][dgt[1]] % 10;
	int director = (dgt[0] << 12) + (dgt[1] << 8) + (dgt[2] << 4) + dgt[3];
	return director;
}