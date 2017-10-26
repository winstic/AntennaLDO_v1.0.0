#pragma once

//#define MBNTICON_WH 120
//#define MBNTICON_HT 100
//#define TOOLBNTSIZE 150
//#define MBNTDIS 15
//#define NUMOFANTENNA 60
#define CELLATNWH 182
#define CELLATNHT 140
#define INTRODUCEIMAGEPIX 440
//
//// �������Ҽ��˵�role�趨
#define ROLE_MARK Qt::UserRole + 1          //�������ָ��ڵ㡢�ļ��нڵ㡢��Ŀ�ڵ�Ľ�ɫ
#define ROLE_MARK_NODE Qt::UserRole + 2   //���������Ż�����ļ��С�����ļ��еĽ�ɫ
#define ROLE_MARK_ITEM Qt::UserRole + 3     //��������������Ŀ�ڵ�
#define ROLE_MARK_UNIT Qt::UserRole + 4     //unit role
//
//// ��Ӧ��ROLE_MARK���趨ÿ��role��ֵ
#define MARK_PROJECT 1
#define MARK_NODE 2
#define MARK_ITEM 3
//
//// ��Ӧ��ROLE_MARK_FOLDER���趨ÿ��role��ֵ
#define MARK_NODE_GENERAL 1  //ͨ��ֵ��������
#define MARK_NODE_DESIGN 2
#define MARK_NODE_OPTIMIZE 3
#define MARK_NODE_RESULT 4
//
//
////��Ӧ��ROLE_MARK_ITEM���趨ÿ�ֽ�ɫ��ֵ
#define MARK_ITEM_OPENFILE 1
#define MARK_ITEM_PERDESIGN 2
#define MARK_ITEM_ALGDESIGN 3
#define MARK_ITEM_ATNDESIGN 4
#define MARK_ITEM_ATNOPTIMIZE 5
//
////��Ӧ��ROLE_MARK_UNIT���趨ÿ�ֽ�ɫ��ֵ
#define MARK_UNIT_LAMBDA 0
#define MARK_UNIT_MM 1
#define MARK_UNIT_CM 2
#define MARK_UNIT_M 4
#define MARK_UNIT_KM 7
//
//
//
//enum pyFlag { proPy = 0, algPy };
//enum SWEEPTYPES { linearly = 0, logarithmically };
//enum ATNFLAG { flagDesign = 0, flagOptimize };

//2017-6-19 new add
//enum problemType { TEST = 0, HFSS, FEKO, NEC };

enum GACOLUMN { cthetalower = 0, cthetaupper, cphilower, cphiupper, coptimaltype, cdelta, cobjvalue, cweight };
enum LOSSCOLUMN { cz0real = 0, cz0imag, closstype, clossoptimaltype, cdeltareal, cdeltaimag, cobjreal, cobjimag, clossweight };
enum NODESCOLUMN { nodeFlag = 0, coreFlag };
enum VARCOLUMN { keyFlag = 0, valueFlag };
enum VARCOLUMN { varnote = 0, varmin, varmax, varunit, varvalue = 1 };	//since i want make varvalue at column-2