#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class varsDefaultValueTemplate : public iTemplate {
	Q_OBJECT
public:
	varsDefaultValueTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent = 0);
	~varsDefaultValueTemplate();

public:
	QLayout* getLayout();
	//update _obj
	void updateJObj();
	bool checkInputValid();

signals:
	void signal_checkValid();

public slots:
	void slot_LinetextChange(QString);
	void slot_sliderValueChange(QString);
	void slot_unitChange(QString);

private:
	void initDefaultData();
	void initLayout();

public:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	QLayout* _layout;
	tableTemplate* _vars_table;
	QLabel* _atn_image_label;
	QMap<QString, QString> _default_vars;
	QMap<int, int> _vars_unit;				//save unit combobox 
	bool _is_valid;
};