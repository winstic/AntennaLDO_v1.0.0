#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"

class frequencyTemplate : public iTemplate {
	Q_OBJECT
public:
	frequencyTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index = 0, iTemplate *parent = 0);
	~frequencyTemplate();
	
public:
	QLayout* getLayout();
	//update _obj
	void updateJObj();
	bool checkInputValid();

signals:
	void signal_checkValid();

public slots:
	void slot_textChanged(QString);

private:
	void initSweepNDPMComBox();
	void initRegex();
	void initDefaultData();
	void initLayout();


public:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	QLayout* _layout;
	QLabel* _frequency_low_label;
	QLabel* _frequency_up_label;
	QLabel* _frequency_num_label;
	QLabel* _sweep_type_label;
	QLabel* _polarization_label;
	QLineEdit* _frequency_low_edit;
	QLineEdit* _frequency_up_edit;
	QLineEdit* _frequency_num_edit;
	QComboBox* _sweep_type_combox;
	QComboBox* _polarization_combox;
	bool _is_valid;
	unsigned int _index;
};