#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

#include <graphics.h>
#include <iomanip>
#include <fstream>

using namespace std;

#define Game_YES 8
#define Game_NO 7

// ͼƬ��������
#define Map_NumberOfImg 6
// ����ͼ����
#define Map_Limit 100
#define Map_MaxElem 170


#define Mode_SEE 1
#define Mode_EDIT 2
#define Mode_ALL 3
#define Operate_selected 3
#define Operate_null 4

// �������
#define Game_JiDiNumber 5
int coodi[5][6] = {
        {150, 360, 0, 1, 0, 1},
        {150, 330, 0, 0, 0, 1},
        {180, 330, 0, 0, 1, 1},
        {210, 330, 0, 0, 1, 0},
        {210, 360, 1, 0, 1, 0}
    };

int programRuningMode = Mode_EDIT; // ��������ģʽ���༭ or ���
int selectStatus = Operate_null;   // ѡ��״̬��ѡ�� or δѡ��
int selectedMapElem = 0; // ѡ�еĵ�ͼԪ������
int seeMapId = -1;       // ��ǰ����Ĺؿ���
int clickDelay = 0;

typedef struct stone {
    int Class;
    //int x;
    //int y;
    int origin_x;
    int origin_y;
    int subStone[4];
} stone;


/*
// ����שǽ 3
// ��שǽ 31
// ��שǽ 32
// ȱ���� 33
// ȱ���� 34
// ȱ���� 35
// ȱ���� 36
*/

void setMapArray(int a, int b, int c, int d, int arr[4], int Class);

class MapElementType {
public:
    static int mapNull;
    static int mapCore;
    static int mapJiDi;
    static int mapBrick;
    static int mapStrong;
    static int mapRiver;
    static int mapForest;
    static int mapShamo;
    static int mapIce;

    static int mapCore1;
    static int mapBrick1;
    static int mapStrong1;
    static int mapRiver1;
    static int mapForest1;
    static int mapIce1;

    static void initMapElemInfo(stone *elemPtr, int x) {
        switch (elemPtr->Class) {
        case 100:
            setMapArray(0, 0, 0, 0, elemPtr->subStone, MapElementType::mapJiDi);
            break;
        case 2:
            setMapArray(x / 1000,
                        x / 100 % 10,
                        x / 10 % 10,
                        x / 1 % 10,
                        elemPtr->subStone,
                        MapElementType::mapStrong);
            break;
        case 3:
            setMapArray(x / 1000,
                        x / 100 % 10,
                        x / 10 % 10,
                        x / 1 % 10,
                        elemPtr->subStone,
                        MapElementType::mapBrick);
        }
    }
};

int MapElementType::mapNull = 0;   //
int MapElementType::mapJiDi = 100; // ������Χ��ǽ�����ͣ�������ǽ���ߣ�ֻ�ڳ������е�ʱ�����
int MapElementType::mapForest = 1; // ɭ��
int MapElementType::mapStrong = 2; // ��ǽ
int MapElementType::mapBrick = 3;  // שǽ
int MapElementType::mapRiver = 4;  // ����
int MapElementType::mapCore = 5;   // ���غ���
int MapElementType::mapShamo = 6;  // ɳĮ(ʵ��δ֧��)
int MapElementType::mapIce = 7;    // ����

// ��¼ͼƬ�����еı��
int MapElementType::mapCore1 = 74;
int MapElementType::mapBrick1 = 82;
int MapElementType::mapStrong1 = 81;
int MapElementType::mapRiver1 = 72;
int MapElementType::mapForest1 = 57;
int MapElementType::mapIce1 = 87;
int mapNameAndIdx[Map_NumberOfImg][2] = {
    {74, -1},
    {82, -1},
    {81, -1},
    {72, -1},
    {57, -1},
    {87, -1}
    };

class OneButton{
private:
    int x, y, w, h; // ���꣬����
    const char *msg;
    int visibleMode;

public:
    OneButton(int _x, int _y, int _v, const char *_m, int _w=70, int _h=20) {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
        visibleMode = _v;
        msg = _m;
    }

    void myShowTime(int mx, int my) {
        int x1 = x, x2 = x + w, y1 = y, y2 = y1 + h;

        if (programRuningMode == visibleMode || visibleMode == Mode_ALL) {
            if( mx >= x1 && mx < x2 && my >= y1 && my < y2 )
            {
                setcolor(RED);
                if( keystate( VK_LBUTTON )&& clickDelay == 0 )
                {
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
            xyprintf(x1 + 5, y1 + 5, msg);
        }
    }

    bool clickMe(int mx, int my) {
        int x1 = x, x2 = x + w, y1 = y, y2 = y1 + h;
        if ((programRuningMode == visibleMode || visibleMode == Mode_ALL) &&
            mx >= x1 && mx < x2 && my >= y1 && my < y2 &&
            keystate( VK_LBUTTON )&& clickDelay == 0) {
                return true;
        }
        return false;
    }
};


class OneMap{
private:
    int numberOfMapElem;
    stone *mapElemList;
    int last_idx;

    bool isSameClass(int Class1, int Class2) {
        if ((Class1 == MapElementType::mapBrick || Class1 == MapElementType::mapBrick1) &&
            (Class2 == MapElementType::mapBrick || Class2 == MapElementType::mapBrick1)) {
            return true;
        }
        else if ((Class1 == MapElementType::mapStrong || Class1 == MapElementType::mapStrong1) &&
                 (Class2 == MapElementType::mapStrong || Class2 == MapElementType::mapStrong1)) {
            return true;
        }
        else if (Class1 == Class2) {
            return true;
        }

        return false;
    }

    bool isClassValid(int Class) {
        if (Class == MapElementType::mapCore ||
            Class == MapElementType::mapForest ||
            Class == MapElementType::mapRiver ||
            Class == MapElementType::mapIce ||
            Class == MapElementType::mapStrong ||
            Class == MapElementType::mapBrick
            ) {
            return true;
        }
        return false;
    }

public:
    int id;
    int originMapId; // ���༭�Ѵ��ڵĵ�ͼ���˱�����¼��ԴID

    OneMap(int _n=Map_MaxElem) {
        numberOfMapElem = _n;
        last_idx = -1;
        id = -1;
        originMapId = -1;
        mapElemList = (struct stone *)calloc(numberOfMapElem, sizeof(struct stone));
        for (int i = 0; i < numberOfMapElem; i++) {
            mapElemList[i].Class = MapElementType::mapNull;
            mapElemList[i].origin_x = -1;
            mapElemList[i].origin_y = -1;
            for (int j = 0; j < 4; j++) {
                mapElemList[i].subStone[j] = MapElementType::mapNull;
            }
        }
    }

    ~OneMap() {
        free((void *)mapElemList);
    }

    void copyDataTo(OneMap &aMap) {
        aMap.last_idx = last_idx;
        if (last_idx >= 0) {
            memcpy((void *)(aMap.mapElemList), (void *)mapElemList, sizeof(struct stone) * (last_idx + 1));
        }
    }

    void addNew(stone *mapElemPtr) {
        int idx = last_idx + 1;
        if ( isClassValid(mapElemPtr->Class) ) {
            mapElemList[idx].Class = mapElemPtr->Class;
            mapElemList[idx].origin_x = mapElemPtr->origin_x;
            mapElemList[idx].origin_y = mapElemPtr->origin_y;
            for (int i = 0; i < 4; i++) {
                mapElemList[idx].subStone[i] = mapElemPtr->subStone[i];
            }
            last_idx++;
        }
    }

    void addMapElem(stone *mapElemPtr) {
        int idx = last_idx + 1;
        int add_one = 1;

        // �����������нڵ�
        for (int i = 0; i <= last_idx; i++) {
            if (mapElemList[i].Class == MapElementType::mapBrick1) {
                mapElemList[i].Class = MapElementType::mapBrick;
            }
            if (mapElemList[i].Class == MapElementType::mapStrong1) {
                mapElemList[i].Class = MapElementType::mapStrong;
            }

            // ���Ҫ��ӵĵ�ͼԪ�ص��������Ѵ���Ԫ��
            if (mapElemList[i].origin_x == mapElemPtr->origin_x && mapElemList[i].origin_y == mapElemPtr->origin_y) {
                add_one = 0;

                if (mapElemPtr->Class == MapElementType::mapStrong1 || mapElemPtr->Class == MapElementType::mapBrick1) {
                    if ( isSameClass(mapElemPtr->Class, mapElemList[i].Class) ) {
                        // ����¼ӵĵ�ͼԪ���ǣ�Ԫ��������Ԫ�� Class һ������׷���µ�Ԫ
                        for (int j = 0; j < 4; j++) {
                            if (mapElemPtr->subStone[j] != MapElementType::mapNull) {
                                mapElemList[i].subStone[j] = mapElemPtr->subStone[j];
                            }
                        }
                    }
                    else {
                        // ����¼ӵĵ�ͼԪ���ǣ�Ԫ��������Ԫ�� Class ��һ�������滻���е�Ԫ
                        mapElemList[i].Class = mapElemPtr->Class;
                        for (int j = 0; j < 4; j++) {
                            mapElemList[i].subStone[j] = mapElemPtr->subStone[j];
                        }
                    }
                } else {
                    // ����¼ӵĵ�ͼԪ���ǣ����غ��ġ�������ɭ�֡����ء�������ǽ������שǽ�����滻��Ԫ��
                    mapElemList[i].Class = mapElemPtr->Class;
                    for (int j = 0; j < 4; j++) {
                        mapElemList[i].subStone[j] = mapElemPtr->subStone[j];
                    }
                }
            }
        }

        if (add_one == 1) {
            // ����Class����
            if (mapElemPtr->Class == MapElementType::mapStrong1) {
                mapElemPtr->Class = MapElementType::mapStrong;
            }
            else if (mapElemPtr->Class == MapElementType::mapBrick1) {
                mapElemPtr->Class = MapElementType::mapBrick;
            }

            // ��Class���ԷǷ���ܾ����
            if ( isClassValid(mapElemPtr->Class) ) {
                mapElemList[idx].Class = mapElemPtr->Class;
                mapElemList[idx].origin_x = mapElemPtr->origin_x;
                mapElemList[idx].origin_y = mapElemPtr->origin_y;
                for (int i = 0; i < 4; i++) {
                    mapElemList[idx].subStone[i] = mapElemPtr->subStone[i];
                }
                last_idx++;
            }
        }
    }

    stone *getAMapElem(int idx) {
        if (idx >= 0 && idx <= last_idx) {
            return mapElemList + idx;
        }
        return NULL;
    }

    int getLastIdx() {
        return last_idx;
    }

    int getMapElemLimit() {
        return numberOfMapElem;
    }

    void cleanAll() {
        for (int i = 0; i <= last_idx; i++) {
            mapElemList[i].Class = MapElementType::mapNull;
            mapElemList[i].origin_x = -1;
            mapElemList[i].origin_y = -1;
            for (int j = 0; j < 4; j++) {
                mapElemList[i].subStone[j] = MapElementType::mapNull;
            }
        }
        last_idx = -1;
        id = -1;
        originMapId = -1;
    }

    void cleanData() {
        if (last_idx >= 0) {
            memset((void *)mapElemList, 0, sizeof(struct stone) * (last_idx + 1));
        }
        last_idx = -1;
    }

    bool saveToFile(const char *filePath, bool app=true, bool mapEnd=false) {
        /* �ú���Ĭ�ϰ�һ����ͼ����׷�ӵ��ļ����
         * ������ļ����Ѵ��ڵ������Ƿ���ȷ��
         */
        char ch[8];
        int x = 0, i = 0;

        ofstream aFile;
        if ( app ) {
            aFile.open(filePath, ios::app|ios::out);
        } else {
            aFile.open(filePath, ios::out);
        }

        // �����ļ����� && map �������ݲ�д���ļ�
        if ( aFile && last_idx >= 0 ) {
            aFile << "*";
            snprintf(ch, 8, "%03d", last_idx + 1);
            aFile << ch;

            for (i = 0; i <= last_idx; i++) {
                snprintf(ch, 4, "%03d", mapElemList[i].Class);
                aFile << ch;
                snprintf(ch, 4, "%03d", mapElemList[i].origin_x);
                aFile << ch;
                snprintf(ch, 4, "%03d", mapElemList[i].origin_y);
                aFile << ch;

                x = 0;
                if (mapElemList[i].Class == MapElementType::mapStrong ||
                    mapElemList[i].Class == MapElementType::mapBrick) {
                    if (mapElemList[i].subStone[0] != MapElementType::mapNull) {
                        x += 1000;
                    }
                    if (mapElemList[i].subStone[1] != MapElementType::mapNull) {
                        x += 100;
                    }
                    if (mapElemList[i].subStone[2] != MapElementType::mapNull) {
                        x += 10;
                    }
                    if (mapElemList[i].subStone[3] != MapElementType::mapNull) {
                        x += 1;
                    }
                    snprintf(ch, 8, "%04d", x);
                    aFile << ch;
                }
            }

            if ( mapEnd ) {
                aFile << "#";
            }

            aFile.close();

            return true;
        }

        return false;
    }
};


void setMapArray(int a, int b, int c, int d, int arr[4], int Class) {
    if (Class == MapElementType::mapJiDi) {
        for (int i = 0; i < 4; i++)
            arr[i] = MapElementType::mapNull;
    }
    else if (Class == MapElementType::mapStrong) {
        if (a == 1) {
            arr[0] = MapElementType::mapStrong1;
        } else {
            arr[0] = MapElementType::mapNull;
        }
        if (b == 1) {
            arr[1] = MapElementType::mapStrong1;
        } else {
            arr[1] = MapElementType::mapNull;
        }
        if (c == 1) {
            arr[2] = MapElementType::mapStrong1;
        } else {
            arr[2] = MapElementType::mapNull;
        }
        if (d == 1) {
            arr[3] = MapElementType::mapStrong1;
        } else {
            arr[3] = MapElementType::mapNull;
        }
    }
    else if (Class == MapElementType::mapBrick) {
        if (a == 1) {
            arr[0] = MapElementType::mapBrick1;
        } else {
            arr[0] = MapElementType::mapNull;
        }
        if (b == 1) {
            arr[1] = MapElementType::mapBrick1;
        } else {
            arr[1] = MapElementType::mapNull;
        }
        if (c == 1) {
            arr[2] = MapElementType::mapBrick1;
        } else {
            arr[2] = MapElementType::mapNull;
        }
        if (d == 1) {
            arr[3] = MapElementType::mapBrick1;
        } else {
            arr[3] = MapElementType::mapNull;
        }
    }
}

#endif // CLASSES_H_INCLUDED
