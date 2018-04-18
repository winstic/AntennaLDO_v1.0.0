#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class axialTemplate : public iTemplate {
	Q_OBJECT
public:
	axialTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index = 0, iTemplate *parent = 0);
	~axialTemplate();

	QLayout* getLayout();
	//update _obj
	void updateJObj();
	bool checkInputValid();

signals:
	void signal_checkValid();

public slots:
	void slot_changeOptimaltype(QString);
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
	tableTemplate* _axial_table;
	QPushButton* _add_button;
	QPushButton* _del_button;
	double _theta_start;
	double _theta_end;
	double _theta_step;
	double _phi_start;
	double _phi_end;
	double _phi_step;
	unsigned int _index;

	QStringList _theta_lower_lists;
	QStringList _theta_upper_lists;
	QStringList _phi_lower_lists;
	QStringList _phi_upper_lists;
	QStringList _optimal_type_lists;
	QStringList _delta_lists;
	QStringList _axial_lists;
	QStringList _weight_lists;
};