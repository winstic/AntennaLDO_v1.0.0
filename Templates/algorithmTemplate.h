#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class algorithmTemplate : public iTemplate {
	Q_OBJECT
public:
	algorithmTemplate(parsProblem* atn_problem, QJsonObject* global_obj, parsAlgorithm** palgorithm, QJsonObject** algorithm_obj, iTemplate *parent = 0);
	~algorithmTemplate();

	QLayout* getLayout();
	//update _obj
	void updateJObj();
	bool checkInputValid();

signals:
	void signal_checkValid();

public slots:
	void slot_algName(const int index);
	void slot_textChanged(QString);

private:
	void initAlgComboItem();
	void initDefaultData();
	void initLayout();
	bool undateDefaultGlobalJson();

public:
	parsProblem* _atn_problem;
	QJsonObject* _global_obj;
	parsAlgorithm** _algorithm;
	QJsonObject** _algorithm_obj;

	QLabel* _alg_label;
	QComboBox* _alg_combox;
	tableTemplate* _alg_vars_table;
	QLayout* _layout;

	QLineEdit* _thread_number_edit;
	QLineEdit* _max_time_edit;
};