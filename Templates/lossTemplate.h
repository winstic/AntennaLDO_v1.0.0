#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class lossTemplate : public iTemplate {
	Q_OBJECT
public:
	lossTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index = 0, iTemplate *parent = 0);
	~lossTemplate();

public:
	QLayout* getLayout();
	//update _obj
	void updateJObj();
	bool checkInputValid();

signals:
	void signal_checkValid();

public slots:
	void slot_ChangeOptimaltype(QString);

private:
	void initDefaultData();
	void initLayout();
	void initLossTypeComBox(QComboBox *combox);


public:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	QLayout* _layout;
	tableTemplate* _loss_table;
	bool _is_valid;
	unsigned int _index;
};