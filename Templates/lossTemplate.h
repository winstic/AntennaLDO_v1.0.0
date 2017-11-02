#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class lossTemplate : public iTemplate {
	Q_OBJECT
public:
	lossTemplate(parsProblem* atn_problem, QJsonObject& obj, iTemplate *parent = 0);
	~lossTemplate();

	QLayout* getLayout();
	//update _obj
	void updateJObj();

	public slots:
	void slot_ChangeOptimaltype(QString);

private:
	void initDefaultData();
	void initLayout();
	void initLossTypeComBox(QComboBox *combox);


public:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	QLayout* _layout;
	tableTemplate* _loss_table;
};