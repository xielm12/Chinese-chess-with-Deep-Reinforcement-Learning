#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<string>
#include<algorithm>
#include"head.h"
using namespace std;

extern int field[11][10];
extern vector<int> valid_moves;
extern int record[300];
extern int chase_record[300];
extern int turns;
extern int change_view;

extern int deep_limit;
extern int cuttab_index;

int initiate_field(){
	/*棋子类型的代号：
	红将　1，        黑将　11，
	红士　2，        黑士　12，
	红相　3，        黑相　13，
	红车　4，        黑车　14，
	红马　5，        黑马　15，
	红炮　6，        黑炮　16，
	红兵　7。        黑兵　17， */
	int tmp[11][10] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0,14,15,13,12,11,12,13,15,14,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,16, 0, 0, 0, 0, 0,16, 0,
		0,17, 0,17, 0,17, 0,17, 0,17,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 7, 0, 7, 0, 7, 0, 7, 0, 7,
		0, 0, 6, 0, 0, 0, 0, 0, 6, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 4, 5, 3, 2, 1, 2, 3, 5, 4,
	};
	for(int i = 0; i <= 10; ++i){
		for(int j = 0; j <= 9; ++j)
			field[i][j] = tmp[i][j];
	}
	turns = 0;
	return 0;
}

bool undo(){
	int temp = turns;
	initiate_field();
	while(turns < temp - 3){
		++turns;
		move(record[turns]);
	}
	turns++;
	return 0;
}

int players_turn(int color){ //玩家回合
	if(turns < 5){
		cout << "请您走子，您可以输入一个4位数，其各位数（从左向右）分别表示" << endl;
		cout <<	"要移动子力现在所在的行数（棋盘左侧已经标明）、列数（棋盘上方标明）、" << endl;
		cout <<	"要移动到的位置的行数、列数。" << endl;
		cout <<	"例如，红将五进一：0595（以回车键结束）；红炮八平五：8885（以回车键结束）" << endl;
		cout <<	"如果您希望悔棋，请不要输入上述四位数，直接输入“undo（以回车键结束），开局阶段不要悔棋”即可" << endl;
		cout <<	"悔棋不限制步数，您可以一直悔到第一手棋" << endl;
		cout <<	"如果您希望更换视角（红黑方视角对换），请直接输入“view（以回车键结束）”" << endl;
		cout << "如果您希望结束游戏，请直接输入“quit（以回车键结束）”" << endl;
		cout << "如果您希望结束游戏并存盘，请直接输入“save（以回车键结束）”，";
		cout <<	"存盘后，可以进入复盘模式" << endl;
	}
	else
		cout << "请您输入：";
	string inpu;
	cin >> inpu;
	if ((inpu.length() == 4) && (inpu[0] >= '0') && (inpu[0] <= '9') && (inpu[1] >= '1') && (inpu[1] <= '9') && (inpu[2] >= '0') && (inpu[2] <= '9') && (inpu[3] >= '1') && (inpu[3] <= '9')) {
		int cmd = 1000 * (inpu[0] - '0') + 100 * (inpu[1] - '0') + 10 * (inpu[2] - '0') + (inpu[3] - '0');
		int director = player_move(cmd);
		get_valid_moves(color, 0);
		vector<int>::iterator it = find(valid_moves.begin(), valid_moves.end(), director);
		if (it != valid_moves.end()) { //找到
			//cout << "director = " << director;	//for debug
			//system("pause");
			return director;
		}
		else {
			cout << "非法走子！请重新走棋" << endl;
			return players_turn(color);
		}
	}
	else if ((inpu.length() == 4) && (inpu[0] == 'u') && (inpu[1] == 'n') && (inpu[2] == 'd') && (inpu[3] == 'o')) {
		undo();
		system("cls");
		print_field(field);
		return players_turn(color);
	}
	else if ((inpu.length() == 4) && (inpu[0] == 'v') && (inpu[1] == 'i') && (inpu[2] == 'e') && (inpu[3] == 'w')) {
		change_view = 1;
		system("cls");
		print_field(field);
		return players_turn(color);
	}
	else if ((inpu.length() == 4) && (inpu[0] == 'f') && (inpu[1] == 'a') && (inpu[2] == 's') && (inpu[3] == 't')) {
		--cuttab_index;
		return players_turn(color);
	}
	else if ((inpu.length() == 4) && (inpu[0] == 's') && (inpu[1] == 'l') && (inpu[2] == 'o') && (inpu[3] == 'w')) {
		++cuttab_index;
		return players_turn(color);
	}
	else if ((inpu.length() == 4) && (inpu[0] == 's') && (inpu[1] == 'a') && (inpu[2] == 'v') && (inpu[3] == 'e')) {
		return 0;
	}
	else if ((inpu.length() == 4) && (inpu[0] == 'q') && (inpu[1] == 'u') && (inpu[2] == 'i') && (inpu[3] == 't'))
		exit(0);
	else{
		cout << "非法输入！请重新输入" << endl;
		return players_turn(color);
	}
}

void replay(){
	cout << "是否开启调试模式？1为是，0为否。" << endl;
	int mode = 0;
	cin >> mode;
	int ai_turn = 500;
	if (mode) {
		cout << "在第几步切换为AI？" << endl;
		cin >> ai_turn;
		cout << "deep_limit = ？" << endl;
		cin >> deep_limit;
		cout << "cuttab_index = ？" << endl;
		cin >> cuttab_index;
	}
	initiate_field();
	print_field(field);
	cout << "复盘开始" << endl;
	while(turns < 297){
		++turns;
		if (turns >= ai_turn) {
			//system("pause");
			int director = AI(turns % 2);
			if (move_with_info(director) == 20)
				chase_record[turns] = 1;
			record[turns] = director;
			print_field(field);
		}
		else {
			if (!record[turns])
				break;
			if (!mode)
				system("pause");
			move(record[turns]);
			print_field(field);
		}
	}
	cout << "复盘到此结束" << endl;
	system("pause");
	return;
}