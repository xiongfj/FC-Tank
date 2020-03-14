#include "graphics.h"
#include <windows.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include "Map.h"
using namespace std;

/* 项目说明
* 目的生成 map.dat 文件，复制到 Tank.sln 的 /Tank/res/data/map.dat 中使用。
* 可以完全不使用该项目生成，直接将数字输入到 map.dat。
* 地图标记约定：
	0 空地
	1 森林
	2 冰
	3 墙
	4 河流
	5 石头
	- 每一行数字对应游戏窗口的每行的块（游戏窗口同样被分为 26*26 的区域块，一个块占 4*4 像素）
*/



void writeMap();
void TestRead();

void main()
{
	writeMap();



	//TestRead();	// 测试读取地图
	_getch();
}



void writeMap()
{
	MAP map[35];
	map[0].map_1();
	map[1].map_2();
	map[2].map_3();
	map[3].map_4();
	map[4].map_5();
	map[5].map_6();
	map[6].map_7();
	map[7].map_8();
	map[8].map_9();
	map[9].map_10();
	map[10].map_11();
	map[11].map_12();
	map[12].map_13();
	map[13].map_14();
	map[14].map_15();
	map[15].map_16();
	map[16].map_17();
	map[17].map_18();
	map[18].map_19();
	map[19].map_20();
	map[20].map_21();
	map[21].map_22();
	map[22].map_23();
	map[23].map_24();
	map[24].map_25();
	map[25].map_26();
	map[26].map_27();
	map[27].map_28();
	map[28].map_29();
	map[29].map_30();
	map[30].map_31();
	map[31].map_32();
	map[32].map_33();
	map[33].map_34();
	map[34].map_35();
	FILE* fp = NULL;
	fopen_s(&fp, "map.dat", "ab+");
	for (int i = 0; i < 35; i++)
		fwrite(&map[i], sizeof(MAP), 1, fp);
	fclose(fp);

	cout << "map.dat 已生成到项目根路径下。" << endl;
}







void TestRead()
{
	MAP map;
	FILE* fp = NULL;
	fopen_s(&fp, "map.dat", "rb");

	for (int m = 0; m < 35; m++)
	{
		fread(&map, sizeof(MAP), 1, fp);
		for (int i = 0; i < 26; i++)
		{
			cout << map.buf[i] << endl;
		}
		cout << endl;
	}
	fclose(fp);


	cout << "地图读取，正确。" << endl;
}














