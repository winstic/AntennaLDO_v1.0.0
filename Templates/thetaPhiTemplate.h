#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class thetaPhiTemplate : public iTemplate {
	Q_OBJECT
public:
	thetaPhiTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index = 0, iTemplate *parent = 0);
	~thetaPhiTemplate();

public:
	QLayout* getLayout();
	//update _obj
	void updateJObj();	
	bool checkInputValid();

signals:
	void signal_checkValid();
	//确定远场范围后触发信号，在设定的远场范围内设置增益轴比
	void signal_confirmFarField(QString);

public slots:
	void slot_confirmButton(bool);

private:
	void initReg();
	void initDefaultData();
	void initLayout();


public:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	QLayout* _layout;
	tableTemplate* _far_field_table;
	QPushButton* _confirm_button;

	QLineEdit* _theta_low_edit;
	QLineEdit* _theta_up_edit;
	QLineEdit* _theta_step_edit;
	QLineEdit* _phi_low_edit;
	QLineEdit* _phi_up_edit;
	QLineEdit* _phi_step_edit;
};