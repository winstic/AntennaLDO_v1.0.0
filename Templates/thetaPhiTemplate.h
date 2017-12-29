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
	inline QLineEdit* getThetaLowEdit() const { return _theta_low_edit; }
	inline QLineEdit* getThetaUpEdit() const { return _theta_up_edit; }
	inline QLineEdit* getThetaStepEdit() const { return _theta_step_edit; }
	inline QLineEdit* getPhiLowEdit() const { return _phi_low_edit; }
	inline QLineEdit* getPhiUpEdit() const { return _phi_up_edit; }
	inline QLineEdit* getPhiStepEdit() const { return _phi_step_edit; }

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
};