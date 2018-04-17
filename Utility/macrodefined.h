#pragma once

//keys
#define MAIN_WINDOW_WIDTH "WINDOW_WIDTH"
#define MAIN_WINDOW_HEIGHT "WINDOW_HEIGHT"
#define DEFAULT_PROJECT_PATH "DEFAULT_PATH"
#define PROBLEM_ID "PID"
#define PROBLEM_NAME "PROBLEM"
#define VARLOW "LOW"
#define VARUP "UP"

//size
#define SUBWINDOW_WIDTH 880
#define SUBWINDOW_HEIGHT 580
#define PROJECT_TREE_WIDTH 260
#define SEARCH_EDIT_WIDTH 260

//#define MBNTICON_WH 120
//#define MBNTICON_HT 100
//#define TOOLBNTSIZE 150
//#define MBNTDIS 15
//#define NUMOFANTENNA 60
#define ANTENNA_IMGWH 120
#define MIN_CELL_ATNWH 140
#define NUM_ANTENNA_COLUMN 6
#define INTRODUCEIMAGEPIX 440
//
//// 工程树右键菜单role设定
#define ROLE_MARK Qt::UserRole + 1          //用于区分根节点、文件夹节点、条目节点的角色
#define ROLE_MARK_NODE Qt::UserRole + 2   //用于区分优化设计文件夹、结果文件夹的角色
#define ROLE_MARK_ITEM Qt::UserRole + 3     //用于区分所有条目节点
#define ROLE_MARK_UNIT Qt::UserRole + 4     //unit role
//
//// 对应于ROLE_MARK，设定每种role的值
#define MARK_PROJECT 1
#define MARK_NODE 2
#define MARK_ITEM 3
//
//// 对应于ROLE_MARK_FOLDER，设定每种role的值
#define MARK_NODE_GENERAL 1  //通用值（保留）
#define MARK_NODE_DESIGN 2
#define MARK_NODE_PERFORMANCE 3
//
//
////对应于ROLE_MARK_ITEM，设定每种角色的值
#define MARK_ITEM_OPENFILE 1
#define MARK_ITEM_PERFORMANCEDESIGN 2
#define MARK_ITEM_ALGORITHMDESIGN 3
#define MARK_ITEM_GEOMETRYDESIGN 4
//
////对应于ROLE_MARK_UNIT，设定每种角色的值
#define MARK_UNIT_LAMBDA 0
#define MARK_UNIT_MM 1
#define MARK_UNIT_CM 2
#define MARK_UNIT_M 4
#define MARK_UNIT_KM 7
//

//slider value
#define MINSLIDERNUMBER 0
#define MAXSLIDERNUMBER 100
#define STEPSLIDERNUMBER 1

//
//
//enum pyFlag { proPy = 0, algPy };
//enum SWEEPTYPES { linearly = 0, logarithmically };
//enum ATNFLAG { flagDesign = 0, flagOptimize };

//2017-6-19 new add
//enum problemType { TEST = 0, HFSS, FEKO, NEC };

enum ERRORCODE {eNormal = 0, eNull, eInvalid, eOther};
//enum PROBLEMTYPE { TEST = 0, HFSS, FEKO, NEC };
enum GACOLUMN { cthetalower = 0, cthetaupper, cphilower, cphiupper, coptimaltype, cdelta, cobjvalue, cweight };
enum LOSSCOLUMN { cz0real = 0, cz0imag, closstype, clossoptimaltype, cdeltareal, cdeltaimag, cobjreal, cobjimag, clossweight };
enum NODESCOLUMN { nodeFlag = 0, coreFlag };
enum VARVALUECOLUMN { keyFlag = 0, valueFlag };
enum VARCOLUMN { varnote = 0, varmin, varmax, varunit, varvalue = 1 };	//since i want make varvalue at column-2
enum FARFIELDCOLUMN { fthetastart = 0, fthetaend, fthetastep, fphistart, fphiend, fphistep };
enum OPTIMIZETYPE {omax = 0, omin, odelta, onone};