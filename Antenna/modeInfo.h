#pragma once

#include <QtWidgets>
#include "../Utility/global.h"

class modelInfo : public QDialog {
	Q_OBJECT
public:
	modelInfo(parsProblem* atn_problem, QWidget *parent = 0);
	~modelInfo();

private:
	parsProblem* _atn_problem;
	QTextEdit* _atn_info;
};
