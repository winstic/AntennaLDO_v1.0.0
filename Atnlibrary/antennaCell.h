#pragma once

#include <QtWidgets>
#include "../Utility/global.h"

class antennaCell :public QFrame
{
public:
    antennaCell(parsProblem* atn_problem, QFrame* parent = 0);
	~antennaCell();
	parsProblem* getAntennaProblem() const;

private:
	void paintEvent(QPaintEvent *event) override;

private:
	parsProblem* _atn_problem;
    QLabel* _atn_name_label;	
	QLabel* _atn_photo;
	//layout
	QVBoxLayout* _vlayout;
	QHBoxLayout* _hlayout;
};
