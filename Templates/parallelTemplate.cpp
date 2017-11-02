#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "parallelTemplate.h"


parallelTemplate::parallelTemplate(QJsonObject& obj, iTemplate *parent) : iTemplate(parent), _obj(obj) {
	_thread_label = new QLabel("启动核数", this);
	_thread_label->setFixedWidth(80);
	_thread_edit = new QLineEdit(this);
	_single_thread_checkbox = new QCheckBox("单机计算", this);
	_single_thread_checkbox->setCheckState(Qt::Checked);
	_multi_thread_checkbox = new QCheckBox("多机计算", this);
	_multi_thread_checkbox->setCheckState(Qt::Unchecked);
	_node_label = new QLabel("node", this);
	_node_edit = new QLineEdit(this);
	_node_vars_table = new tableTemplate();
	_core_label = new QLabel("cores", this);
	_core_edit = new QLineEdit(this);
	_single_group = new QGroupBox(this);
	_multi_group = new QGroupBox(this);
	_add_button = new QPushButton("添加", this);
	_del_button = new QPushButton("删除", this);
	_node_vars_table->setColumnCount(2);
	QStringList header;
	header << "IP地址/主机名" << "启动核数";
	_node_vars_table->setHorizontalHeaderLabels(header);

	initDefaultData();
	initLayout();
	connect(_single_thread_checkbox, SIGNAL(stateChanged(int)), this, SLOT(slot_singleCheckBoxStateChange(int)));
	connect(_multi_thread_checkbox, SIGNAL(stateChanged(int)), this, SLOT(slot_multiCheckBoxStateChange(int)));
	connect(_add_button, SIGNAL(clicked(bool)), this, SLOT(slot_addNodeButton()));
	connect(_del_button, SIGNAL(clicked(bool)), this, SLOT(slot_delNodeButton()));
}

void parallelTemplate::initDefaultData() {
	//setting default data
	//get thread num from global_conf.
	//QString global_json_path = QString("%1/global_conf.json").arg(dataPool::global::getGDEA4ADPath());
	//QJsonObject global_obj = parseJson::getJsonObj(global_json_path);
	//_thread_edit->setText(QString::number(global_obj.value("ThreadNum").toString().trimmed().toInt() - 1));
	_thread_edit->setText(QString::number(_obj.value("ThreadNum").toString().trimmed().toInt() - 1));

}

void parallelTemplate::initLayout() {
	QHBoxLayout h_layout1, h_layout2, h_layout3;
	QVBoxLayout v_layout, v_layout1;
	h_layout1.addWidget(_thread_label);
	h_layout1.addWidget(_thread_edit);
	_single_group->setLayout(&h_layout1);

	h_layout2.addWidget(_node_label);
	h_layout2.addWidget(_node_edit);
	h_layout2.addWidget(_core_label);
	h_layout2.addWidget(_core_edit);
	h_layout2.addWidget(_add_button);
	v_layout1.addLayout(&h_layout2);


	h_layout3.addWidget(_node_vars_table);
	h_layout3.addWidget(_del_button);
	v_layout1.addLayout(&h_layout3);
	_multi_group->setLayout(&v_layout1);

	_single_group->setEnabled(true);
	_multi_group->setEnabled(false);

	v_layout.addWidget(_single_thread_checkbox);
	v_layout.addWidget(_single_group);
	v_layout.addWidget(_multi_thread_checkbox);
	v_layout.addWidget(_multi_group);
	_layout = &v_layout;
}

QLayout* parallelTemplate::getLayout() {
	return _layout;
}
//update _obj
void parallelTemplate::updateJObj() {

}

//slots function
void parallelTemplate::slot_singleCheckBoxStateChange(const int state) {
	//state: enum explain{Qt::Unchecked=0, Qt::PartiallyChecked, Qt::Checked}
	//so usually using 0(unchecked), 2(checked);
	//qDebug() << state;
	if (state == 0) {
		_single_group->setEnabled(false);
		_multi_thread_checkbox->setCheckState(Qt::Checked);
	}
	else if (state == 2) {
		//set another checkBox(multiCheckBox unChecked)
		_single_group->setEnabled(true);
		_multi_thread_checkbox->setCheckState(Qt::Unchecked);
	}
}

void parallelTemplate::slot_multiCheckBoxStateChange(const int state) {
	if (state == 0) {
		//set another checkBox(multiCheckBox Checked)
		_multi_group->setEnabled(false);
		_single_thread_checkbox->setCheckState(Qt::Checked);
	}
	else if (state == 2) {
		//set another checkBox(multiCheckBox unChecked)
		_multi_group->setEnabled(true);
		_single_thread_checkbox->setCheckState(Qt::Unchecked);
	}
}

void parallelTemplate::slot_addNodeButton() {
	QString nodeinfo, coreinfo;
	int rowIndex = _node_vars_table->rowCount();
	//qDebug() << rowIndex;
	_node_vars_table->insertRow(rowIndex);
	nodeinfo = _node_edit->text().trimmed();
	coreinfo = _core_edit->text().trimmed();
	_node_vars_table->insert2table(rowIndex, nodeFlag, nodeinfo);
	_node_vars_table->insert2table(rowIndex, coreFlag, coreinfo);
}

void parallelTemplate::slot_delNodeButton() {
	int selectRow = _node_vars_table->currentRow();
	//qDebug() << selectRow;
	if (selectRow != -1)
		_node_vars_table->removeRow(selectRow);
}