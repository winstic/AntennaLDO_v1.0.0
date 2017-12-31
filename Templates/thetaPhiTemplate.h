#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"

class thetaPhiTemplate : public iTemplate {
	Q_OBJECT
public:
	thetaPhiTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent = 0);
	~thetaPhiTemplate();

public:
	QLayout* getLayout();
	//update _obj
	void updateJObj();	
	bool checkInputValid();
signals:
	void signal_checkValid();

private:
	void initRegex();
	void initDefaultData();
	void initLayout();


public:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	QLayout* _layout;
	QLabel* _theta_low_label;
	QLabel* _theta_up_label;
	QLabel* _theta_step_label;
	QLabel* _phi_low_label;
	QLabel* _phi_up_label;
	QLabel* _phi_step_label;
	QLineEdit* _theta_low_edit;
	QLineEdit* _theta_up_edit;
	QLineEdit* _theta_step_edit;
	QLineEdit* _phi_low_edit;
	QLineEdit* _phi_up_edit;
	QLineEdit* _phi_step_edit;
	bool _is_valid;
};