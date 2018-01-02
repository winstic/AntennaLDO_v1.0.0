#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class gainTemplate : public iTemplate {
	Q_OBJECT
public:
	gainTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent = 0);
	~gainTemplate();

public:
	QLayout* getLayout();
	//update _obj
	void updateJObj();
	bool checkInputValid();

signals:
	void signal_checkValid();

public slots:
	void slot_ChangeOptimaltype(QString);
	void slot_confirmFarField(QString);
	void slot_addSetting(bool);
	void slot_delSetting(bool);
	void slot_selectChanged();

private:
	void initDefaultData();
	void initLayout();

public:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	QLayout* _layout;
	tableTemplate* _gain_table;
	QPushButton* _add_button;
	QPushButton* _del_button;
	double _theta_start;
	double _theta_end;
	double _theta_step;
	double _phi_start;
	double _phi_end;
	double _phi_step;
	bool _is_valid;
};