#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "frequencyTemplate.h"

frequencyTemplate::frequencyTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj) {
	_frequency_low_label = new QLabel("频段上限:", this);
	_frequency_up_label = new QLabel("频段下限:", this);
	_frequency_num_label = new QLabel("频点个数:", this);
	_sweep_type_label = new QLabel("扫频方式:", this);
	_polarization_label = new QLabel("极化方式:", this);
	_frequency_low_edit = new QLineEdit(this);
	_frequency_up_edit = new QLineEdit(this);
	_frequency_num_edit = new QLineEdit(this);
	_sweep_type_combox = new QComboBox(this);
	_polarization_combox = new QComboBox(this);
	initSweepNDPMComBox();
	initRegex();
	initDefaultData(); 
	initLayout();
}

void frequencyTemplate::initSweepNDPMComBox() {
	_sweep_type_combox->addItem("等差");
	_sweep_type_combox->addItem("等比");

	_polarization_combox->addItem("总增益");
	_polarization_combox->addItem("左旋");
	_polarization_combox->addItem("右旋");
	_polarization_combox->addItem("水平");
	_polarization_combox->addItem("垂直");
}

void frequencyTemplate::initRegex() {
	//setting some input rules
	QRegExpValidator *nonNegFloatValid = getNonNegativeFloatReg();    //non negative float
	QRegExpValidator *posIntValid = getPositiveIntReg();		//positive int
	_frequency_low_edit->setValidator(nonNegFloatValid);
	_frequency_up_edit->setValidator(nonNegFloatValid);
	_frequency_num_edit->setValidator(posIntValid);
}

void frequencyTemplate::initDefaultData() {
	//setting default data
	QJsonObject frequency_obj = parseJson::getSubJsonObj(*_obj, "FreSetting");
	if (frequency_obj.isEmpty()) {
		qCritical("get 'FreSetting' json object field.");
		return;
	}
	QStringList str_list;
	str_list = dataPool::str2list(frequency_obj.value("FreStart").toString().trimmed());
	_frequency_low_edit->setText(str_list[0]);
	str_list = dataPool::str2list(frequency_obj.value("FreEnd").toString().trimmed());
	_frequency_up_edit->setText(str_list[0]);
	str_list = dataPool::str2list(frequency_obj.value("FreNumber").toString().trimmed());
	_frequency_num_edit->setText(str_list[0]);
	_sweep_type_combox->setCurrentIndex(0);
	_sweep_type_combox->setEnabled(false);
	_polarization_combox->setCurrentIndex(0);
	_polarization_combox->setEnabled(false);
}

void frequencyTemplate::initLayout() {
	//layout
	QGridLayout* frequency_layout = new QGridLayout;
	_frequency_low_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_frequency_low_label, 0, 0);
	frequency_layout->addWidget(_frequency_low_edit, 0, 1);
	QLabel* hz_label = new QLabel("MHz");
	frequency_layout->addWidget(hz_label, 0, 2);
	_frequency_up_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_frequency_up_label, 1, 0);
	frequency_layout->addWidget(_frequency_up_edit, 1, 1);
	QLabel* hz_label2 = new QLabel("MHz");
	frequency_layout->addWidget(hz_label2, 1, 2);
	_frequency_num_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_frequency_num_label, 2, 0);
	frequency_layout->addWidget(_frequency_num_edit, 2, 1);
	_sweep_type_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_sweep_type_label, 3, 0);
	frequency_layout->addWidget(_sweep_type_combox, 3, 1);
	_polarization_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_polarization_label, 4, 0);
	frequency_layout->addWidget(_polarization_combox, 4, 1);
	_layout = frequency_layout;
}

QLayout* frequencyTemplate::getLayout() {
	return _layout;
}

//update json obj
void frequencyTemplate::updateJObj() {
	QJsonObject mfrequency_obj;
	mfrequency_obj.insert("FreStart", QString("[%1]").arg(_frequency_low_edit->text().trimmed()));
	mfrequency_obj.insert("FreEnd", QString("[%1]").arg(_frequency_up_edit->text().trimmed()));
	mfrequency_obj.insert("FreNumber", QString("[%1]").arg(_frequency_num_edit->text().trimmed()));
	mfrequency_obj.insert("SweepType", QString("[%1]").arg(_sweep_type_combox->currentIndex()));
	mfrequency_obj.insert("PM", QString("[%1]").arg(_polarization_combox->currentIndex()));
	_obj->insert("FreSetting", mfrequency_obj);
}

frequencyTemplate::~frequencyTemplate(){}