#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class gainTemplate : public iTemplate {
	Q_OBJECT
public:
	gainTemplate(parsProblem* atn_problem, QJsonObject& obj, iTemplate *parent = 0);
	~gainTemplate();

	QLayout* getLayout();
	//update _obj
	void updateJObj();

public slots:
	void slot_ChangeOptimaltype(QString);

private:
	void initDefaultData();
	void initLayout();


public:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	QLayout* _layout;
	tableTemplate* _gain_table;
};