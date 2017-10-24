#pragma once

#include <QtWidgets>
#include "../Utility/global.h"

class antennaCell :public QWidget{
	Q_OBJECT

public:
    antennaCell(parsProblem* atn_problem, QWidget* parent = 0);
	/*void enterEvent(QEvent *);*/
    //void mouseMoveEvent(QMouseEvent* event);
	//QPushButton *m_detail_btn;
	parsProblem* getAntennaProblem() const;

private:
	parsProblem* _atn_problem;
    QLabel* _atn_photo;
    QLabel* _atn_name_label;	
};
