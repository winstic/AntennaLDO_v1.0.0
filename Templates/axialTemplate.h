#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class axialTemplate : public iTemplate {
	Q_OBJECT
public:
	axialTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent = 0);
	~axialTemplate();

	QLayout* getLayout();
	//update _obj
	void updateJObj();

public slots:
	void slot_changeOptimaltype(QString);

private:
	void initDefaultData();
	void initLayout();


public:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	QLayout* _layout;
	tableTemplate* _axial_table;
};