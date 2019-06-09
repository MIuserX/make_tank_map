#include <graphics.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <list>
#include "classes.h"

// ��
#define Enemy_bullet_rest_time 25  // �з�̹�˿�����ȴʱ��
#define Enemy_shoot_probability 71 // �з�̹�˿��ڼ���
#define Enemy_appear_time 100      // �з�̹�˳���ʱ����
#define Cake_appear_probability 1  // �������ּ���
#define Enemy_stop_time 400        // ����������ʱ��
#define Cake_twinkle_time 20       // ������˸ʱ��
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

OneMap *mapArray = new OneMap[Map_Limit]; // ��ͼ������
int mapIdx = -1;                          // ���������һ��Ԫ���±�



int main()
{
    OneMap editMap(170); // ��ͼ�༭��

    setinitmode( 0 );
    initgraph(700, 390);
    setrendermode( RENDER_MANUAL );

    // ģʽ 1 ��ȡ�ؿ�������Ϣ
    MSG_operator(1, 1);

    // ��ȡ����ͼƬ��Դ
    getPictures();

    // ��Ϸ����ģ��
    for( ; is_run(); )
    {
        // ��������
        drawGridline();

        // ����ť����
        processButton(editMap);

        // ��ͼ
        Cartoonplayer(editMap);

        delay_fps( 50 );
        cleardevice();
    }
    for(int i = 0; i < 86; i++)
    {
        delimage( Picture[i] );
    }


    getch();
    closegraph(); // �ر�ͼ�ν���
    return 0;

}

/* ���༭�����浽������
 */
void saveToCache(OneMap &aMap) {
    int idx = mapIdx + 1;

    // ���༭�����Ѵ��ڵĵ�ͼ����д��
    if (aMap.originMapId != -1) {
        mapArray[aMap.originMapId].cleanData();
        aMap.copyDataTo(mapArray[aMap.originMapId]);
        aMap.cleanAll();
    } else if (idx < Map_Limit) {
    // ���༭���Ǹ��µ�ͼ������ӵ�����ĩβ
        mapIdx++;
        mapArray[idx].id = idx;
        aMap.copyDataTo(mapArray[idx]);
    }
}

/* ���±༭�Ѵ��ڵĵ�ͼ
 *     ���ǽ��������ĵ�ͼ���Ƹ��༭����Ȼ���л�Ϊ�༭״̬
 */
void reEditMap(OneMap &originMap, OneMap &editMap) {
    editMap.cleanAll();
    originMap.copyDataTo(editMap);
    editMap.originMapId = originMap.id;
    programRuningMode = Mode_EDIT;
}

/* ��������д�뵽�ļ�
 *     �ļ������ݽ��ᱻˢ�£����ļ�����Ҫ�ˡ�
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
        //-- ��ȡ�ϰ�����
        fgets(ch_num, 4, fp);
        numOfMapElem = atoi(ch_num);

        idx = mapIdx + 1;
        if (idx < Map_Limit) {
            mapIdx++;
            mapArray[idx].id = idx;

            /* ��ȡ�ùز�����Ϣ */
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

// ���ݴ�����, *ģʽ1��ȡ��ģʽ2д��
void MSG_operator(int mode, int floor)
{
    FILE *fp;
    char ch;

    fp = fopen("newmap.txt", "r");

    // ��ȡ�ؿ���Ϣ
    do/* ���ļ�ָ�����ָ���ؿ�ǰ */
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

// �����û���������
void processButton(OneMap &aMap)
{
    int x, y;
    int x1, y1, x2, y2, w, h, btn_space;

    //������ɫ
	setcolor(EGERGB(0x0, 0xFF, 0x0));

	//���ֱ���ɫ��ע��setbkcolor����Ҳ��ͬʱ�ı����ֱ���ɫ��
	setfontbkcolor(EGERGB(0x80, 0x00, 0x80));

	//�������壬��һ������������ĸ߶ȣ����أ����ڶ�������������Ŀ�ȣ��ڶ����������Ϊ0����ʹ��Ĭ�ϱ���ֵ
	//����߶�Ϊ12�����൱��С����֣�����9���֣�ʵ�ʵĻ�����Լ���ɰ�
	setfont(12, 0, "����");

	//д���֣�ע�⣺outtextxy��֧��\t \n�����ʽ���õ������ַ��������ַ��ᱻ����
	//Ҫʹ�������ʽ���ַ�����outtextrect

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
    �л�ģʽ
    ********
    */
    x1 = 410;
    y1 = 70;
    x2 = x1 + w;
    OneButton switchMode(x1, y1, Mode_ALL, "�л�ģʽ");
    switchMode.myShowTime(x, y);
    if (switchMode.clickMe(x, y)) {
        if (programRuningMode == Mode_EDIT) {
            //-- ����ѡ������
            selectStatus = Operate_null;
            selectedMapElem = 0;

            //-- ��ʼ�����ģʽ
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
    ��һ��
    ********
    */
    x1 = x2 + btn_space;
    x2 = x1 + w;
    OneButton seeUp(x1, y1, Mode_SEE, "��һ��");
    seeUp.myShowTime(x, y);
    if (seeUp.clickMe(x, y)) {
        if (programRuningMode == Mode_SEE && seeMapId > 0) {
            seeMapId--;
        }
    }

	/*
    ********
    ��һ��
    ********
    */
    x1 = x2 + btn_space;
    x2 = x1 + w;
    OneButton seeDown(x1, y1, Mode_SEE, "��һ��");
    seeDown.myShowTime(x, y);
    if ( seeDown.clickMe(x, y) ) {
        if (programRuningMode == Mode_SEE && seeMapId < mapIdx) {
            seeMapId++;
        }
    }

    /*
    **********
    ���浽����
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
            xyprintf(x1 + 5, y1 + 5, "д�ص�����");
        } else {
            xyprintf(x1 + 5, y1 + 5, "���浽����");
        }
    }

    /*
    *********
    �༭����
    *********
    */
    OneButton editThis(x1, y1, Mode_SEE, "�༭����");
    editThis.myShowTime(x, y);
    if ( editThis.clickMe(x, y) ) {
        reEditMap(mapArray[seeMapId], aMap);
    }

    /*
    **********
    ��ձ༭��
    **********
    */
    x1 = x2 + btn_space;
    x2 = x1 + w;
    OneButton cleanWorkspace(x1, y1, Mode_EDIT, "��ձ༭��");
    cleanWorkspace.myShowTime(x, y);
    if ( cleanWorkspace.clickMe(x, y) ) {
        aMap.cleanAll();
    }

    /*
    ************
    ����д���ļ�
    ************
    */
    x1 = x2 + btn_space;
    x2 = x1 + w;
    OneButton saveCacheToFile(x1, y1, Mode_EDIT, "����д���ļ�");
    saveCacheToFile.myShowTime(x, y);
    if ( saveCacheToFile.clickMe(x, y) ) {
        saveToFile();
    }
}

// ��ȡͼƬ����
void getPictures()
{
    char dir[] = "pictue\\P00.bmp";
    // ����ͼƬ��Դ
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

	//��һֱ�ߣ���(100,100)��(500,200)
	//�ر�ע����Ƕ˵㣬(100,100)�����ᱻ������ɫ����(500,200)���ử����ɫ
	//�������еľ��Σ�����˵���㣩������ʽ������ǰһ����ȡ�õ�������һ����ȡ����
	for (int x = 0; x < 390; x += 15) {
        line(x, 0, x, 390);
        line(0, x, 390, x);
        line(x + 29, 0, x + 29, 390);
        line(0, x + 29, 390, x + 29);
	}


    setcolor(GREEN);

	//��һֱ�ߣ���(100,100)��(500,200)
	//�ر�ע����Ƕ˵㣬(100,100)�����ᱻ������ɫ����(500,200)���ử����ɫ
	//�������еľ��Σ�����˵���㣩������ʽ������ǰһ����ȡ�õ�������һ����ȡ����
	for (int x = 0; x < 390; x += 30) {
        line(x, 0, x, 390);
        line(0, x, 390, x);
        line(x + 29, 0, x + 29, 390);
        line(0, x + 29, 390, x + 29);
	}

}

// ��ͼ����
void Cartoonplayer(OneMap &aMap)
{
    // ��Ԫ��שԪ��ɭ�֣����أ����������أ���������ש
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

            //---- ��ʼ�� Class ����
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

            //---- �����ⲿ�ִ�����ƿ�ѡ��ͼƬ
            draw_x = base_x + 40 * j;
            draw_y = base_y + 40 * i;
            drawMapElem(&aMapElem, draw_x, draw_y );

            mousepos(&mx, &my);
            //---- �������ѡ��״̬��ѡ�еľ����ң���չʾ
            if (programRuningMode == Mode_EDIT) {
                if (selectStatus == Operate_selected && aMapElem.Class == selectedMapElem ) {
                    // չʾѡ��Ԫ��
                    xyprintf(base_x + 80, base_y - 60, "ѡ��Ԫ�أ�");
                    drawMapElem(&aMapElem, base_x + 160, base_y - 60);

                    // �������˵����ͼ�����¼�����ӵ���ͼ��
                    if (mx >= 0 && mx < 390 && my >= 0 && my < 390 && keystate( VK_LBUTTON ) && clickDelay == 0) {
                        aMapElem.origin_x = mx / 30 * 30;
                        aMapElem.origin_y = my / 30 * 30;
                        if (aMapElem.Class == MapElementType::mapStrong1 ||
                            aMapElem.Class == MapElementType::mapBrick1) {
                            // ����Ԫ���ڵ�λ��
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
                xyprintf(560, 20, "��ǰģʽ�����");
            }
            else if (programRuningMode == Mode_EDIT) {
                xyprintf(560, 20, "��ǰģʽ���༭");
                if (selectStatus == Operate_selected) {
                    xyprintf(base_x, base_y - 60, "�Ƿ�ѡ�У���");
                }
                else if (selectStatus == Operate_null) {
                    xyprintf(base_x, base_y - 60, "�Ƿ�ѡ�У���");
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

    // ���Ƶ�ǰ�����ĵ�ͼ
    if (programRuningMode == Mode_EDIT) {
        for (int i = 0; i <= aMap.getLastIdx(); i++) {
            aMapElemPtr = aMap.getAMapElem(i);
            drawMapElem(aMapElemPtr);
        }
        aMapElemPtr = NULL;
        // ���һЩ��ͼ����Ϣ
        xyprintf(410, 300, "��ͼԪ������%d", aMap.getLastIdx() + 1);
    }
    else if (programRuningMode == Mode_SEE && mapIdx >= 0 && seeMapId >= 0 && seeMapId <= mapIdx) {
    // �����: ���ģʽ && ���صĵ�ͼ������0 && ����±�Ϸ�
        for (int i = 0; i <= mapArray[seeMapId].getLastIdx(); i++) {
            aMapElemPtr = mapArray[seeMapId].getAMapElem(i);
            drawMapElem(aMapElemPtr);
        }
        aMapElemPtr = NULL;
        // ���һЩ��ͼ����Ϣ
        xyprintf(410, 300, "��ͼԪ������%d", mapArray[seeMapId].getLastIdx() + 1);
    }

    // ��Ϣչʾ
    xyprintf(410, 320, "�����ͼ������%d", mapIdx + 1);
    //xyprintf(410, 340, "�����ͼ������%d", mapIdx + 1);
    if (programRuningMode == Mode_SEE) {
        xyprintf(410, 360, "��ǰ���������%d", seeMapId + 1);
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
