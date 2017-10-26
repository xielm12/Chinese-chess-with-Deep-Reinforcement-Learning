#include<iostream>
#include<fstream>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
//#include<windows.h>
#include<vector>
#include<algorithm>
#include"head.h"
#include <locale.h>
using namespace std;

int field[11][10]; //存储现在的棋局状态，从(1,1)开始
int record[300]; //棋谱，从1开始
int chase_record[300];	//用于记录是否有捉着，以便判断长捉
int turns = 0; //步数
extern int deep_limit;	//偶数步正常？？
int view = 0;
int change_view = 0;
vector<int> valid_moves;

void delay(int del){
	for(int i = 0; i < del; ++i){
		for(int j = 0; j < del; ++j){
			for(int k = 0; k < del; ++k)
				int num = 54321 * 321;
		}
	}
	return;
}

int read_file() {	//读盘
	char filename[30];
	cout << "请输入您之前保存的文件名，以“file”开始，以“.dat”结束，不超过30个字符。" << endl;
	scanf("%s", filename);
	ifstream fin(filename, ios::binary);
	for (int i = 0; i < 300; ++i)
		fin.read((char*)&record[i], sizeof(int));
	fin.close();
	return 0;
}

int write_file() {	//存盘
	char filename[30];
	cout << "请为您新建的文件命名，以“file”开始，以“.dat”结束，不要超过30个字符。" << endl;
	scanf("%s", filename);
	ofstream fout(filename, ios::binary);
	for (int i = 0; i < 300; ++i)
		fout.write((char*)&record[i], sizeof(int));
	fout.close();
	return 0;
}

int main(){
	cout << "本程序在linux下运行" << endl;
	setlocale(LC_ALL,"zh_CN.GBK"); 
	for (int i = 0; i < 300; ++i) {
		record[i] = 0;
		chase_record[i] = 0;
	}
	int mode = 0;
	system("cls");
	cout << endl << "            欢迎来到中国象棋游戏  w          " << endl << endl << endl;
	cout << "               0：双机对战               " << endl << endl;            
	cout << "               1：玩家执红               " << endl << endl;
	cout << "               2：玩家执黑               " << endl << endl;
	cout << "               3：读入存档               " << endl << endl;
	cout << "               4：写开局库               " << endl << endl;
	cout << "               5：对比测试               " << endl << endl;
	cout << "            6：生成训练数据              " << endl << endl;
	cout << endl << "欢迎联系：zhaotianyang@pku.edu.cn" << endl;
	cout << "请输入对应数字，以回车键结束" << endl;
	cin >> mode;
	if (mode == 6) {
		system("cls");
		for (int i = 1; i <= 65; ++i) {
			char filename[30];
			sprintf(filename, "%d", i);
			learning_gen_data(filename);
			usleep(2000000);
		}
		return 0;
	}
	if (mode == 7) {
		//one_hot_trans();
		//count();
		split();
		return 0;
	}
	int test_mode = 0;
	if (mode == 5) {
		cout << "输入1让procut执红，0让朴素alpha-beta执红" << endl;	
		cin >> test_mode;
	}
	ai_start_table_init();
	if (mode != 3) {
		initiate_field();
		print_field(field);
		print_field(field);
		int director = 0;
		bool start_table_hit_flag = 1;
		while (turns < 297) {
			//红方
			++turns;
			if ((mode == 1) || (mode == 4))
				director = players_turn(1);
			else {
				if (mode == 5) {
					if(turns <= 2)	//测试者指定开局
						director = players_turn(1);
					else {
						if (test_mode) {
							AI(1, 0);
							director = AI(1, 1);
						}
						else
							director = AI(1, 0);
					}
				}
				else if (start_table_hit_flag) {
					director = ai_start_table_search(director);
					if (!director) {
						director = AI(1);
						start_table_hit_flag = 0;
					}
				}
				else
					director = AI(1);
			}
			if (!director)
				break;
			if (move_with_info(director) == 20)
				chase_record[turns] = 1;
			record[turns] = director;
			if((turns <= 2) || (mode != 5))
				print_field(field);
			//黑方
			++turns;
			if ((mode == 2) || (mode == 4))
				director = players_turn(0);
			else {
				if (mode == 5) {
					if (turns <= 2)	//测试者指定开局
						director = players_turn(0);
					else {
						if (test_mode)
							director = AI(0, 0);
						else {
							AI(0, 0);
							director = AI(0, 1);
						}						
					}
				}
				else if (start_table_hit_flag) {
					director = ai_start_table_search(director);
					if (!director) {
						director = AI(0);
						start_table_hit_flag = 0;
					}
				}
				else
					director = AI(0);
			}
			if (!director)
				break;
			if (move_with_info(director) == 20)
				chase_record[turns] = 1;
			record[turns] = director;
			if ((turns <= 2) || (mode != 5))
				print_field(field);
		}
		cout << "您是否需要存盘？是请输入1，否则输入0。以回车键结束" << endl;
		int save_flag = 0;
		cin >> save_flag;
		if(save_flag)
			write_file();
	}
	else if (mode == 3)
		read_file();
	if (mode == 4)
		save_to_start_table();
	cout << "您是否需要复盘？是请输入1，否则输入0。以回车键结束" << endl;
	int replay_flag = 0;
	cin >> replay_flag;
	if (replay_flag)
		replay();
	cout << "您是否需要存盘？是请输入1，否则输入0。以回车键结束" << endl;
	int save_flag = 0;
	cin >> save_flag;
	if (save_flag)
		write_file();
	cout << "欢迎您下次再来！" << endl;
	system("pause");
	return 0;
}
