#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class variablesTemplate : public iTemplate {
	Q_OBJECT
public:
	variablesTemplate(parsProblem* atn_problem, QJsonObject& obj, iTemplate *parent = 0);
	~variablesTemplate();

	QLayout* getLayout();
	//update _obj
	void updateJObj();

public slots:
	void slot_unitChange(QString);

private:
	void initDefaultData();
	void initLayout();

public:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	QLayout* _layout;
	tableTemplate* _vars_table;
	QLabel* _atn_image_label;
	QMap<int, int> _vars_unit;				//save combobox unit
};