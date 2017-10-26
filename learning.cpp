#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<vector>
#include<string>
#include<algorithm>
//#include<windows.h>
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

extern int eval;	//from ai

/*以下为新增的机器学习类*/

const int dim = 1260;

class ts_item {	//training set item	
private:
	bool valid;
	int vector[dim];
	int label;
private:
	int transform() {	//把棋盘转换成训练用的vector
		for (int i = 0; i < dim; ++i)
			vector[i] = 0;
		int trans[18] = { 0,0,1,2,3,4,5,6,0,0,0,7,8,9,10,11,12,13 };
		for (int i = 1; i <= 10; ++i) {
			for (int j = 1; j <= 9; ++j) {
				if (field[i][j]) {
					int p = trans[field[i][j]] * 90 + 9 * (i - 1) + (j - 1);
					vector[p] = 1;
				}
			}
		}
		return 0;
	};
public:
	ts_item(): valid(0){};
	int clear() {
		valid = 0;
		return 0;
	};
	bool gen(bool valid_, int label_) {	//生成一个item
		valid = valid_;
		label = label_;
		transform();
		return valid_;
	};
	friend class ts;
};

class ts {	//training_set
private:
	ts_item data[200];
private:
	int write_to_file() {	//把数据写入文件
		ofstream fout("trainingset", ios::app | ios::binary);
		for (int i = 0; i < 200; ++i) {
			if (data[i].valid) {
				fout.write((char*)&data[i].label, sizeof(int));
				for (int j = 0; j < dim; ++j)
					fout.write((char*)&data[i].vector[j], sizeof(int));
			}
		}
		fout.close();
		return 0;
	}
public:
	int gen_data(char* filename) {
		//读入存盘数据	
		ifstream fin(filename, ios::binary);
		for (int i = 0; i < 300; ++i)
			fin.read((char*)&record[i], sizeof(int));
		fin.close();
		//复盘并计算label值，存入data中
		initiate_field();
		while (turns < 297) {
			++turns;
			if (!record[turns])
				break;
			int c = turns % 2;	//color
			AI(c);
			//用红的评价训练
			if(c)	//红
				data[turns].gen(1, eval);	//注意正负！！
			else
				data[turns].gen(1, -eval);	//注意正负！！
			move(record[turns]);
		}
		//打印至文件
		write_to_file();
		//清理内存
		for (int i = 0; i < 200; ++i)
			data[i].clear();
		return 0;
	};
	int gen_more_data(char* filename) {
		//读入存盘数据	
		ifstream fin(filename, ios::binary);
		for (int i = 0; i < 300; ++i)
			fin.read((char*)&record[i], sizeof(int));
		fin.close();
		//复盘并计算label值，存入data中
		initiate_field();
		while (turns < 297) {
			usleep(30000);
			int data_it = 0;
			++turns;
			if (!record[turns])
				break;
			int c = turns % 2;	//color
			//用红的评价训练
			AI(1);
			data[data_it++].gen(1, eval);	//注意正负！！
			//generate more
			get_valid_moves(c);
			int size = valid_moves.size();
			int save[11][10];
			for (int n = size - 1; n >= 0; --n) {
				for (int i = 0; i <= 10; ++i) {
					for (int j = 0; j <= 9; ++j)
						save[i][j] = field[i][j];
				}
				move(valid_moves[n]);
				//用红的评价训练
				AI(1);
				data[data_it++].gen(1, eval);	//注意正负！！
				for (int i = 0; i <= 10; ++i) {
					for (int j = 0; j <= 9; ++j)
						field[i][j] = save[i][j];
				}
			}
			move(record[turns]);
			//打印至文件
			write_to_file();
			//清理内存
			for (int i = 0; i < 200; ++i)
				data[i].clear();
		}
		return 0;
	};
};

ts a;

int learning_gen_data(char* filename) {
	//cout << sizeof(int) << endl;
	//cout << "正在根据第 " << filename << " 个棋谱生成训练集合..." << endl;
	cout << filename << endl;
	a.gen_more_data(filename);
	return 0;
}

int one_hot_trans() {
	//cout << sizeof(int) << endl;
	ifstream fin("trainingset", ios::binary);
	ofstream fout("10-trainingset", ios::app | ios::binary);
	int tmp;
	bool flag = 1;
	while (true) {
		for (int i = 0; i <= 1260; ++i) {
			if (!fin.read((char*)&tmp, sizeof(int))) {
				flag = 0;
				break;
			}			
			if (i == 0) {
				if (tmp > 200) 
					tmp = 10;
				else if (tmp > 130)
					tmp = 9;
				else if (tmp > 70)
					tmp = 8;
				else if (tmp > 30)
					tmp = 7;
				else if (tmp > 0)
					tmp = 6;
				else if (tmp > -30)
					tmp = 5;
				else if (tmp > -70)
					tmp = 4;
				else if (tmp > -130)
					tmp = 3;
				else if (tmp > -200)
					tmp = 2;
				else
					tmp = 1;
			}
			fout.write((char*)&tmp, sizeof(int));
		}
		if (!flag)
			break;
	}
	fin.close();
	fout.close();
	return 0;
}

int count() {
	ifstream fin("trainingset", ios::binary);
	int tmp;
	int count = 0;
	bool flag = 1;
	while (true) {
		for (int i = 0; i <= 1260; ++i) {
			if (!fin.read((char*)&tmp, sizeof(int))) {
				flag = 0;
				break;
			}		
		}
		if (!flag)
			break;
		count++;
	}
	fin.close();
	cout << count << endl;
	system("pause");
	return 0;
}

int split() {
	//cout << sizeof(int) << endl;
	ifstream fin("trainingset", ios::binary);
	ofstream fout1("trainingset-116693-1", ios::app | ios::binary);
	ofstream fout2("trainingset-116693-2", ios::app | ios::binary);
	int tmp;
	for (int i = 1; i <= 233386; ++i) {
		for (int j = 0; j <= 1260; ++j) {
			fin.read((char*)&tmp, sizeof(int));
			if(i <= 116693)
				fout1.write((char*)&tmp, sizeof(int));
			else if (i <= 233386)
				fout2.write((char*)&tmp, sizeof(int));
		}
	}
	fin.close();
	fout1.close();
	fout2.close();
	return 0;
}
