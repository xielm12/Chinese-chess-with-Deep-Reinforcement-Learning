menu: menu2.cpp AI.o display.o learning.o move.o player.o table.o cnn_foruse.py
	g++ menu2.cpp -o menu -I ./ ./AI.o ./display.o ./learning.o ./table.o ./player.o ./move.o -I/usr/include/python2.7 -lpython2.7
AI.o: AI.cpp
	g++ -c AI.cpp -o AI.o -I/usr/include/python2.7/ -lpython2.7
display.o: display2.cpp
	g++ -c display2.cpp -o display.o
learning.o: learning.cpp
	g++ -c learning.cpp -o learning.o
move.o: move.cpp
	g++ -c move.cpp -o move.o
player.o: player2.cpp
	g++ -c player2.cpp -o player.o
table.o: table.cpp
	g++ -c table.cpp -o table.o
menu2.cpp: menu.cpp
	iconv -f gb2312 -t utf-8 menu.cpp > menu2.cpp
display2.cpp: display.cpp
	iconv -f gb2312 -t utf-8 display.cpp > display2.cpp
player2.cpp: player.cpp
	iconv -f gb2312 -t utf-8 player.cpp > player2.cpp
