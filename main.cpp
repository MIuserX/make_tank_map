#include <graphics.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <list>
#include "classes.h"

// 宏
#define Enemy_bullet_rest_time 25  // 敌方坦克开炮冷却时间
#define Enemy_shoot_probability 71 // 敌方坦克开炮几率
#define Enemy_appear_time 100      // 敌方坦克出现时间间隔
#define Cake_appear_probability 1  // 奖励出现几率
#define Enemy_stop_time 400        // 定身术持续时间
#define Cake_twinkle_time 20       // 奖励闪烁时间
#define Smaller_len 4
#define Step_len 6

void reEditMap(OneMap&, OneMap&);
void saveToCache(OneMap&);
void drawGridline();
void MSG_operator(int mode, int floor);
void getPictures();
void processButton(OneMap&);
void drawMapElem(stone *mapElemPtr, int x = -1, int y = -1);
void Cartoonplayer(OneMap&);
void Game_operator();
int searchPic(int Class);
void readAMap(FILE *fp);

PIMAGE Picture[Map_NumberOfImg];

OneMap *mapArray = new OneMap[Map_Limit]; // 地图缓存区
int mapIdx = -1;                          // 缓存区最后一个元素下标



int main()
{
    OneMap editMap(170); // 地图编辑区

    setinitmode( 0 );
    initgraph(700, 390);
    setrendermode( RENDER_MANUAL );

    // 模式 1 读取关卡地形信息
    MSG_operator(1, 1);

    // 读取所需图片资源
    getPictures();

    // 游戏运行模块
    for( ; is_run(); )
    {
        // 画网格线
        drawGridline();

        // 处理按钮操作
        processButton(editMap);

        // 绘图
        Cartoonplayer(editMap);

        delay_fps( 50 );
        cleardevice();
    }
    for(int i = 0; i < 86; i++)
    {
        delimage( Picture[i] );
    }


    getch();
    closegraph(); // 关闭图形界面
    return 0;

}

/* 将编辑区保存到缓存区
 */
void saveToCache(OneMap &aMap) {
    int idx = mapIdx + 1;

    // 若编辑的是已存在的地图，则写回
    if (aMap.originMapId != -1) {
        mapArray[aMap.originMapId].cleanData();
        aMap.copyDataTo(mapArray[aMap.originMapId]);
        aMap.cleanAll();
    } else if (idx < Map_Limit) {
    // 若编辑的是个新地图，则添加到数组末尾
        mapIdx++;
        mapArray[idx].id = idx;
        aMap.copyDataTo(mapArray[idx]);
    }
}

/* 重新编辑已存在的地图
 *     就是将缓存区的地图复制给编辑区，然后切换为编辑状态
 */
void reEditMap(OneMap &originMap, OneMap &editMap) {
    editMap.cleanAll();
    originMap.copyDataTo(editMap);
    editMap.originMapId = originMap.id;
    programRuningMode = Mode_EDIT;
}

/* 将缓存区写入到文件
 *     文件的内容将会被刷新，老文件别想要了。
 */
void saveToFile() {
    bool app = false, last = false;

    for (int i = 0; i <= mapIdx; i++) {
        if (i == mapIdx) {
            last = true;
        }
        mapArray[i].saveToFile("newmap.txt", app, last);
        app = true;
    }
}


void readAMap(FILE *fp) {
    char ch_num [4], ch4[5];
    int numOfMapElem = 0, temp = 0, i = 0, idx = 0;
    stone aStone;

    if (NULL != fp) {
        //-- 读取障碍个数
        fgets(ch_num, 4, fp);
        numOfMapElem = atoi(ch_num);

        idx = mapIdx + 1;
        if (idx < Map_Limit) {
            mapIdx++;
            mapArray[idx].id = idx;

            /* 读取该关布局信息 */
            for( i = 0; i < numOfMapElem; i++) {
                aStone.Class = MapElementType::mapNull;
                aStone.origin_x = -1;
                aStone.origin_y = -1;
                for (int j = 0; j < 4; j++) {
                    aStone.subStone[j] = MapElementType::mapNull;
                }

                fgets(ch_num, 4, fp);
                temp = atoi(ch_num);
                aStone.Class = temp;

                fgets(ch_num, 4, fp);
                temp = atoi(ch_num);
                aStone.origin_x = temp;

                fgets(ch_num, 4, fp);
                temp = atoi(ch_num);
                aStone.origin_y = temp;

                temp = 0;
                if (aStone.Class == MapElementType::mapStrong ||
                    aStone.Class == MapElementType::mapBrick ||
                    aStone.Class == MapElementType::mapStrong1 ||
                    aStone.Class == MapElementType::mapBrick1) {
                    if (aStone.Class == MapElementType::mapStrong1) {
                        aStone.Class = MapElementType::mapStrong;
                    }
                    if (aStone.Class == MapElementType::mapBrick1) {
                        aStone.Class = MapElementType::mapBrick;
                    }
                    fgets(ch4, 5, fp);
                    temp = atoi(ch4);
                }

                MapElementType::initMapElemInfo(&aStone, temp);

                mapArray[idx].addNew(&aStone);
            }
        }
    }
}

// 数据处理函数, *模式1读取，模式2写入
void MSG_operator(int mode, int floor)
{
    FILE *fp;
    char ch;

    fp = fopen("newmap.txt", "r");

    // 读取关卡信息
    do/* 将文件指针调至指定关卡前 */
    {
        ch = fgetc(fp);
        if('*' == ch) {
            readAMap(fp);
        }
        else if ('#' == ch) {
            break;
        }
    }
    while( 1 );

    fclose(fp);
}

// 接收用户按键函数
void processButton(OneMap &aMap)
{
    int x, y;
    int x1, y1, x2, y2, w, h, btn_space;

    //文字颜色
	setcolor(EGERGB(0x0, 0xFF, 0x0));

	//文字背景色（注意setbkcolor函数也会同时改变文字背景色）
	setfontbkcolor(EGERGB(0x80, 0x00, 0x80));

	//设置字体，第一个参数是字体的高度（像素），第二个参数是字体的宽度，第二个参数如果为0，就使用默认比例值
	//如果高度为12，即相当于小五号字，或者9磅字，实际的换算就自己完成吧
	setfont(12, 0, "宋体");

	//写文字，注意：outtextxy不支持\t \n这类格式化用的特殊字符，这类字符会被忽略
	//要使用特殊格式化字符请用outtextrect

	mousepos(&x, &y);
	xyprintf(420, 20, "x:%d", x);
	xyprintf(420, 40, "y:%d", y);

	xyprintf(470, 20, "format_x:%d", x / 30 * 30);
	xyprintf(470, 40, "format_y:%d", y / 30 * 30);

	if ( keystate( VK_LBUTTON ) && clickDelay == 0 ) {
        clickDelay = 7;
	}
    if (clickDelay > 0) {
        clickDelay--;
    }

	w = 70;
	h = 20;
	btn_space = 20;

	/*
    ********
    切换模式
    ********
    */
    x1 = 410;
    y1 = 70;
    x2 = x1 + w;
    OneButton switchMode(x1, y1, Mode_ALL, "切换模式");
    switchMode.myShowTime(x, y);
    if (switchMode.clickMe(x, y)) {
        if (programRuningMode == Mode_EDIT) {
            //-- 清理选中数据
            selectStatus = Operate_null;
            selectedMapElem = 0;

            //-- 初始化浏览模式
            programRuningMode = Mode_SEE;
            if (seeMapId == -1) {
                if (mapIdx >= 0) {
                    seeMapId = 0;
                }
            } else {
                if (mapIdx < 0) {
                    seeMapId = -1;
                }
            }
        }
        else if (programRuningMode == Mode_SEE) {
            programRuningMode = Mode_EDIT;
        }
    }

	/*
    ********
    上一关
    ********
    */
    x1 = x2 + btn_space;
    x2 = x1 + w;
    OneButton seeUp(x1, y1, Mode_SEE, "上一关");
    seeUp.myShowTime(x, y);
    if (seeUp.clickMe(x, y)) {
        if (programRuningMode == Mode_SEE && seeMapId > 0) {
            seeMapId--;
        }
    }

	/*
    ********
    下一关
    ********
    */
    x1 = x2 + btn_space;
    x2 = x1 + w;
    OneButton seeDown(x1, y1, Mode_SEE, "下一关");
    seeDown.myShowTime(x, y);
    if ( seeDown.clickMe(x, y) ) {
        if (programRuningMode == Mode_SEE && seeMapId < mapIdx) {
            seeMapId++;
        }
    }

    /*
    **********
    保存到缓存
    **********
    */
    x1 = 410;
    y1 = 100;
    x2 = x1 + w;
    y2 = y1 + h;
    if (programRuningMode == Mode_EDIT) {
        if( x >= x1 && x < x2 && y >= y1 && y < y2 )
        {
            setcolor(RED);
            if( keystate( VK_LBUTTON )&& clickDelay == 0 )
            {
                saveToCache(aMap);
                rectangle(x1+1, y1+1, x2+1, y2+1);
            }
            else
            {
                rectangle(x1, y1, x2, y2);
            }
        }
        else
        {
            setcolor(BLUE);
            rectangle(x1, y1, x2, y2);
        }
        setcolor(WHITE);
        if (aMap.originMapId != -1) {
            xyprintf(x1 + 5, y1 + 5, "写回到缓存");
        } else {
            xyprintf(x1 + 5, y1 + 5, "保存到缓存");
        }
    }

    /*
    *********
    编辑本关
    *********
    */
    OneButton editThis(x1, y1, Mode_SEE, "编辑本关");
    editThis.myShowTime(x, y);
    if ( editThis.clickMe(x, y) ) {
        reEditMap(mapArray[seeMapId], aMap);
    }

    /*
    **********
    清空编辑区
    **********
    */
    x1 = x2 + btn_space;
    x2 = x1 + w;
    OneButton cleanWorkspace(x1, y1, Mode_EDIT, "清空编辑区");
    cleanWorkspace.myShowTime(x, y);
    if ( cleanWorkspace.clickMe(x, y) ) {
        aMap.cleanAll();
    }

    /*
    ************
    缓存写入文件
    ************
    */
    x1 = x2 + btn_space;
    x2 = x1 + w;
    OneButton saveCacheToFile(x1, y1, Mode_EDIT, "缓存写入文件");
    saveCacheToFile.myShowTime(x, y);
    if ( saveCacheToFile.clickMe(x, y) ) {
        saveToFile();
    }
}

// 获取图片函数
void getPictures()
{
    char dir[] = "pictue\\P00.bmp";
    // 地形图片资源
    for(int i = 0; i < Map_NumberOfImg; i++) {
        dir[8] = (char)(mapNameAndIdx[i][0] / 10) + '0';
        dir[9] = (char)(mapNameAndIdx[i][0] % 10) + '0';
        Picture[i] = newimage();
        getimage(Picture[i], dir);
        mapNameAndIdx[i][1] = i;
    }
}

int searchPic(int Class) {
    for (int i = 0; i < Map_NumberOfImg; i++) {
        if (
            (Class == MapElementType::mapCore && mapNameAndIdx[i][0] == MapElementType::mapCore1) ||
            (Class == MapElementType::mapForest && mapNameAndIdx[i][0] == MapElementType::mapForest1) ||
            (Class == MapElementType::mapRiver && mapNameAndIdx[i][0] == MapElementType::mapRiver1) ||
            ((Class == MapElementType::mapBrick || Class == MapElementType::mapBrick1) && mapNameAndIdx[i][0] == MapElementType::mapBrick1) ||
            ((Class == MapElementType::mapStrong || Class == MapElementType::mapStrong1)&& mapNameAndIdx[i][0] == MapElementType::mapStrong1) ||
            (Class == MapElementType::mapIce && mapNameAndIdx[i][0] == MapElementType::mapIce1) ) {
                return mapNameAndIdx[i][1];
        }
    }
    return 0;
}

void drawGridline() {
    setcolor(RED);

	//画一直线，从(100,100)到(500,200)
	//特别注意的是端点，(100,100)这个点会被画上颜色，但(500,200)不会画上颜色
	//以下所有的矩形（或者说两点）描述方式，总是前一个点取得到，但后一个点取不到
	for (int x = 0; x < 390; x += 15) {
        line(x, 0, x, 390);
        line(0, x, 390, x);
        line(x + 29, 0, x + 29, 390);
        line(0, x + 29, 390, x + 29);
	}


    setcolor(GREEN);

	//画一直线，从(100,100)到(500,200)
	//特别注意的是端点，(100,100)这个点会被画上颜色，但(500,200)不会画上颜色
	//以下所有的矩形（或者说两点）描述方式，总是前一个点取得到，但后一个点取不到
	for (int x = 0; x < 390; x += 30) {
        line(x, 0, x, 390);
        line(0, x, 390, x);
        line(x + 29, 0, x + 29, 390);
        line(0, x + 29, 390, x + 29);
	}

}

// 绘图函数
void Cartoonplayer(OneMap &aMap)
{
    // 铁元，砖元，森林，基地，河流，冰地，整铁，整砖
    int mapYuan[] = {81, 82, 57, 74, 72, 87, 58, 65};
    int yuanLen = 8;
    int idx = 0;
    stone aMapElem;
    int base_x = 410, draw_x = 0;
    int base_y = 200, draw_y = 0;
    int mx = 0, my = 0, small_x, small_y;
    stone *aMapElemPtr;

    for(int i = 0; i < 3; i++) {
        for (int j = 0; j < 7; j++) {

            for (int k = 0; k < 4; k++) {
                aMapElem.subStone[k] = MapElementType::mapNull;
            }

            //---- 初始化 Class 属性
            switch (mapYuan[idx]) {
            case 81:
                aMapElem.Class = MapElementType::mapStrong1;
                aMapElem.subStone[0] = MapElementType::mapStrong1;
                break;
            case 82:
                aMapElem.Class = MapElementType::mapBrick1;
                aMapElem.subStone[0] = MapElementType::mapBrick1;
                break;
            case 57:
                aMapElem.Class = MapElementType::mapForest;
                break;
            case 74:
                aMapElem.Class = MapElementType::mapCore;
                break;
            case 72:
                aMapElem.Class = MapElementType::mapRiver;
                break;
            case 87:
                aMapElem.Class = MapElementType::mapIce;
                break;
            case 58:
                aMapElem.Class = MapElementType::mapStrong;
                for (int k = 0; k < 4; k++) {
                    aMapElem.subStone[k] = MapElementType::mapStrong1;
                }
                break;
            case 65:
                aMapElem.Class = MapElementType::mapBrick;
                for (int k = 0; k < 4; k++) {
                    aMapElem.subStone[k] = MapElementType::mapBrick1;
                }
                break;
            }

            //---- 下面这部分代码绘制可选中图片
            draw_x = base_x + 40 * j;
            draw_y = base_y + 40 * i;
            drawMapElem(&aMapElem, draw_x, draw_y );

            mousepos(&mx, &my);
            //---- 如果处于选中状态且选中的就是我，则展示
            if (programRuningMode == Mode_EDIT) {
                if (selectStatus == Operate_selected && aMapElem.Class == selectedMapElem ) {
                    // 展示选中元素
                    xyprintf(base_x + 80, base_y - 60, "选中元素：");
                    drawMapElem(&aMapElem, base_x + 160, base_y - 60);

                    // 若发生了点击地图区域事件则添加到地图中
                    if (mx >= 0 && mx < 390 && my >= 0 && my < 390 && keystate( VK_LBUTTON ) && clickDelay == 0) {
                        aMapElem.origin_x = mx / 30 * 30;
                        aMapElem.origin_y = my / 30 * 30;
                        if (aMapElem.Class == MapElementType::mapStrong1 ||
                            aMapElem.Class == MapElementType::mapBrick1) {
                            // 计算元所在的位置
                            small_x = mx / 15 * 15;
                            small_y = my / 15 * 15;
                            aMapElem.subStone[0] = MapElementType::mapNull;
                            if (small_x > aMapElem.origin_x) {
                                if (small_y > aMapElem.origin_y) {
                                    aMapElem.subStone[3] = aMapElem.Class;
                                } else {
                                    aMapElem.subStone[1] = aMapElem.Class;
                                }
                            } else {
                                if (small_y > aMapElem.origin_y) {
                                    aMapElem.subStone[2] = aMapElem.Class;
                                } else {
                                    aMapElem.subStone[0] = aMapElem.Class;
                                }
                            }
                        }
                        aMap.addMapElem(&aMapElem);
                    }
                }
                else if( mx >= draw_x && mx < draw_x + 30
                   && my >= draw_y && my < draw_y + 30
                   && keystate( VK_LBUTTON ) && clickDelay == 0) {
                    if (selectStatus == Operate_selected && aMapElem.Class == selectedMapElem ) {
                        selectStatus = Operate_null;
                        selectedMapElem = 0;
                    }
                    else {
                        selectStatus = Operate_selected;
                        selectedMapElem = aMapElem.Class;
                    }
                }
            }

            if (programRuningMode == Mode_SEE) {
                xyprintf(560, 20, "当前模式：浏览");
            }
            else if (programRuningMode == Mode_EDIT) {
                xyprintf(560, 20, "当前模式：编辑");
                if (selectStatus == Operate_selected) {
                    xyprintf(base_x, base_y - 60, "是否选中：是");
                }
                else if (selectStatus == Operate_null) {
                    xyprintf(base_x, base_y - 60, "是否选中：否");
                }
            }


            idx += 1;

            if (idx >= yuanLen) {
                break;
            }
        }
        if (idx >= yuanLen) {
            break;
        }
    }

    // 绘制当前制作的地图
    if (programRuningMode == Mode_EDIT) {
        for (int i = 0; i <= aMap.getLastIdx(); i++) {
            aMapElemPtr = aMap.getAMapElem(i);
            drawMapElem(aMapElemPtr);
        }
        aMapElemPtr = NULL;
        // 输出一些地图的信息
        xyprintf(410, 300, "地图元素数：%d", aMap.getLastIdx() + 1);
    }
    else if (programRuningMode == Mode_SEE && mapIdx >= 0 && seeMapId >= 0 && seeMapId <= mapIdx) {
    // 如果是: 浏览模式 && 加载的地图数大于0 && 浏览下标合法
        for (int i = 0; i <= mapArray[seeMapId].getLastIdx(); i++) {
            aMapElemPtr = mapArray[seeMapId].getAMapElem(i);
            drawMapElem(aMapElemPtr);
        }
        aMapElemPtr = NULL;
        // 输出一些地图的信息
        xyprintf(410, 300, "地图元素数：%d", mapArray[seeMapId].getLastIdx() + 1);
    }

    // 信息展示
    xyprintf(410, 320, "缓存地图数量：%d", mapIdx + 1);
    //xyprintf(410, 340, "缓存地图数量：%d", mapIdx + 1);
    if (programRuningMode == Mode_SEE) {
        xyprintf(410, 360, "当前浏览关数：%d", seeMapId + 1);
    }
}


void drawMapElem(stone *mapElemPtr, int x, int y) {
    int idx = 0;

    if (x == -1 && y == -1) {
        x = mapElemPtr->origin_x;
        y = mapElemPtr->origin_y;
    }

    if (mapElemPtr->Class == MapElementType::mapCore ||
        mapElemPtr->Class == MapElementType::mapIce ||
        mapElemPtr->Class == MapElementType::mapRiver) {
        putimage( x, y, Picture[searchPic(mapElemPtr->Class)]);
    }
    else if (MapElementType::mapForest == mapElemPtr->Class) {
        putimage_transparent(NULL, Picture[searchPic(mapElemPtr->Class)], x, y, BLACK);
    }
    else if (mapElemPtr->Class == MapElementType::mapBrick ||
             mapElemPtr->Class == MapElementType::mapStrong ||
             mapElemPtr->Class == MapElementType::mapBrick1 ||
             mapElemPtr->Class == MapElementType::mapStrong1){
        idx = searchPic(mapElemPtr->Class);
        if (mapElemPtr->subStone[0] != MapElementType::mapNull) {
            putimage(x, y, Picture[idx]);
        }
        if (mapElemPtr->subStone[1] != MapElementType::mapNull) {
            putimage(x + 15, y, Picture[idx]);
        }
        if (mapElemPtr->subStone[2] != MapElementType::mapNull) {
            putimage(x, y +15, Picture[idx]);
        }
        if (mapElemPtr->subStone[3] != MapElementType::mapNull) {
            putimage(x + 15, y + 15, Picture[idx]);
        }
    }
}
