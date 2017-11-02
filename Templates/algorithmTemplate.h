#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class algorithmTemplate : public iTemplate {
	Q_OBJECT
public:
	algorithmTemplate(parsProblem* atn_problem, QJsonObject algorithm_obj, parsAlgorithm* palgorithm = 0, iTemplate *parent = 0);
	~algorithmTemplate() {}

	QLayout* getLayout();
	//update _obj
	void updateJObj();

public slots:
	void slot_algName(const int index);

private:
	void initAlgComboItem();
	void initDefaultData();
	void initLayout();

public:
	parsProblem* _atn_problem;
	parsAlgorithm* _algorithm;
	QJsonObject _algorithm_obj;
	QLabel* _alg_label;
	QComboBox* _alg_combox;
	tableTemplate* _alg_vars_table;
	QLayout* _layout;
};