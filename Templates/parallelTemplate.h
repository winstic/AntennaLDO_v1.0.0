#pragma once

#include "../Utility/global.h"
#include "iTemplate.h"
#include "tableTemplate.h"

class parallelTemplate : public iTemplate {
	Q_OBJECT
public:
	parallelTemplate(QJsonObject& obj, iTemplate *parent = 0);
	~parallelTemplate() {}

	QLayout* getLayout();
	//update _obj
	void updateJObj();

public slots:
	void slot_singleCheckBoxStateChange(const int state);
	void slot_multiCheckBoxStateChange(const int state);
	void slot_addNodeButton(/*bool check*/);
	void slot_delNodeButton(/*bool check*/);

private:
	void initDefaultData();
	void initLayout();

private:
	QJsonObject _obj;
	//threads
	QLabel* _thread_label;
	QLineEdit* _thread_edit;
	QCheckBox* _single_thread_checkbox;
	QCheckBox* _multi_thread_checkbox;
	//nodes
	QLabel* _node_label;
	QLabel* _core_label;
	tableTemplate* _node_vars_table;
	QLineEdit* _node_edit;
	QLineEdit* _core_edit;
	//button
	QPushButton* _add_button;
	QPushButton* _del_button;
	//group
	QGroupBox* _single_group;
	QGroupBox* _multi_group;
	QLayout* _layout;
};