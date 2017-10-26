#include<iostream>
#include<stdio.h>
#include<stdlib.h>
//#include<windows.h>
using namespace std;

extern int field[11][10];
extern int turns;
extern int view;
extern int change_view;

int change(){
	if(change_view){
		view = 1 - view;
		change_view = 0;
	}
	if(view){
		int tmp[11][10];
		for(int i = 0; i < 11; ++i){
			for(int j = 0; j < 10; ++j)
				tmp[i][j] = field[i][j];
		}
		for(int i = 1; i <= 10; ++i){
			for(int j = 1; j <= 9; ++j)
				field[i][j] = tmp[11 - i][10 - j];
		}
	}
	return 0;
}

void color_cout(const int color1){ //指定打印字符的颜色函数
//reference: http://www.52pojie.cn/thread-315693-1-1.html
/*颜色对应值： 
　　0=黑色                8=灰色　　
  　1=蓝色                9=淡蓝色                　　                        
	2=绿色                10=淡绿色        0xa        　　
	3=湖蓝色			  11=淡浅绿色      0xb　
	4=红色                12=淡红色        0xc　　
	5=紫色                13=淡紫色        0xd        　　
	6=黄色                14=淡黄色        0xe        　　
	7=白色                15=亮白色        0xf 
	也可以吧这些值设置成常量。
*/
	/*仅限改变0-15的颜色;如果在0-15那么实现他的颜色   
	因为如果超过15后面的改变的是文本背景色。*/
	//if( color1 >= 0 && color1 <= 15 )
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color1);
	/*如果不在0-15的范围颜色，那么改为默认的颜色白色；*/
	//else
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
/*
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),BACKGROUND_BLUE);
	switch(color1){
		case 12: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED); break;
		case 7: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); break;
		case 10: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN); break;
	}
*/
}

int print_field(const int field[][10]){
	change();
	system("clear");
	cout << "***************第 " << turns << " 手***************" << endl << endl;
	color_cout(7);
	int print_field[20][18] = {
		18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
		18,23,21,28,21,28,21,28,21,28,21,28,21,28,21,28,21,24,
		18,22,18,22,18,22,18,22,19,22,20,22,18,22,18,22,18,22,
		18,27,21,31,21,31,21,31,21,31,21,31,21,31,21,31,21,29,
		18,22,18,22,18,22,18,22,20,22,19,22,18,22,18,22,18,22,
		18,27,21,36,21,31,21,31,21,31,21,31,21,31,21,36,21,29,
		18,22,18,22,18,22,18,22,18,22,18,22,18,22,18,22,18,22,
		18,32,21,31,21,36,21,31,21,36,21,31,21,36,21,31,21,34,
		18,22,18,22,18,22,18,22,18,22,18,22,18,22,18,22,18,22,
		18,27,21,30,21,30,21,30,21,30,21,30,21,30,21,30,21,29,
		18,22,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,22,
		18,27,21,28,21,28,21,28,21,28,21,28,21,28,21,28,21,29,
		18,22,18,22,18,22,18,22,18,22,18,22,18,22,18,22,18,22,
		18,32,21,31,21,36,21,31,21,36,21,31,21,36,21,31,21,34,
		18,22,18,22,18,22,18,22,18,22,18,22,18,22,18,22,18,22,
		18,27,21,36,21,31,21,31,21,31,21,31,21,31,21,36,21,29,
		18,22,18,22,18,22,18,22,19,22,20,22,18,22,18,22,18,22,
		18,27,21,31,21,31,21,31,21,31,21,31,21,31,21,31,21,29,
		18,22,18,22,18,22,18,22,20,22,19,22,18,22,18,22,18,22,
		18,25,21,30,21,30,21,30,21,30,21,30,21,30,21,30,21,26
	};
	for(int i = 1; i <= 10; ++i){
		for(int j = 1; j <= 9; ++j){
			if(field[i][j])
				print_field[2 * i - 1][2 * j - 1] = field[i][j];
		}
	}
	int k = 0;
	if(view)
		cout << " 　９　８　７　６　５　４　３　２　１" << endl;
	else
		cout << " 　１　２　３　４　５　６　７　８　９" << endl;
	for(int i = 0; i < 20; ++i){
		if(i % 2){
			++k;
			if(view)
				cout << ((11 - k) % 10);
			else
				cout << (k % 10);
		}
		else
			cout << " ";
		for(int j = 0; j < 18; ++j){
			switch(print_field[i][j]){
				case 1: color_cout(12); printf("\033[31m帅\033[0m"); color_cout(7); break;
				case 2: color_cout(12); printf("\033[31m仕\033[0m"); color_cout(7); break;
				case 3: color_cout(12); printf("\033[31m相\033[0m"); color_cout(7); break;
				case 4: color_cout(12); printf("\033[31m车\033[0m"); color_cout(7); break;
				case 5: color_cout(12); printf("\033[31m马\033[0m"); color_cout(7); break;
				case 6: color_cout(12); printf("\033[31m炮\033[0m"); color_cout(7); break;
				case 7: color_cout(12); printf("\033[31m兵\033[0m"); color_cout(7); break;
				case 11: color_cout(10); printf("\033[32m将\033[0m"); color_cout(7); break;
				case 12: color_cout(10); printf("\033[32m士\033[0m"); color_cout(7); break;
				case 13: color_cout(10); printf("\033[32m象\033[0m"); color_cout(7); break;
				case 14: color_cout(10); printf("\033[32m车\033[0m"); color_cout(7); break;
				case 15: color_cout(10); printf("\033[32m马\033[0m"); color_cout(7); break;
				case 16: color_cout(10); printf("\033[32m炮\033[0m"); color_cout(7); break;
				case 17: color_cout(10); printf("\033[32m卒\033[0m"); color_cout(7); break;
				case 18: cout << "  "; break;
				case 19: cout << "` "; break;
				case 20: cout << "/ "; break;
				case 21: cout << "─ "; break;
				case 22: cout << "│ "; break;
				case 23: cout << "┌ "; break;
				case 24: cout << "┐ "; break;
				case 25: cout << "└ "; break;
				case 26: cout << "┘ "; break;
				case 27: cout << "├ "; break;
				case 28: cout << "┬ "; break;
				case 29: cout << "┤ "; break;
				case 30: cout << "┴ "; break;
				case 31: cout << "┼ "; break;
				case 32: cout << "┣ "; break;
				case 34: cout << "┫ "; break;
				case 36: cout << "╋ "; break;
			}
		}
		cout << endl;
	}
	cout << endl;
	change();
	return 0;
}
