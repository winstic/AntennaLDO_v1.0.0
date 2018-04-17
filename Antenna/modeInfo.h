#pragma once
#include <QtWidgets>
#include "../Utility/global.h"
#include "../Templates/tableTemplate.h"

class modelInfo : public QDialog {
	Q_OBJECT
public:
	modelInfo(parsProblem* atn_problem, QWidget *parent = 0);
	~modelInfo();

private:
	void initDefalultParametersRange();
	void initDefalultParameters();
	void initLayout();

	void setFirstTitle(QLabel *label);
	void setSecondTitle(QLabel *label);

private:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	tableTemplate *_vars_range_table;
	tableTemplate *_vars_table;
	QVBoxLayout* _main_layout;
};
