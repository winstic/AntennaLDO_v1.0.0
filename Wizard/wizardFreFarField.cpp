﻿#pragma execution_character_set("utf-8")
#include "wizardFreFarField.h"
#include "../Utility/commonStyle.h"
#include "../Utility/parseJson.h"

wizardFreFarField::wizardFreFarField(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent) : QWizardPage(parent),
 _atn_problem(atn_problem), _obj(obj){    
	setTitle("辐射频率及远场范围参数设置");
	_frequency_widget = new frequencyTemplate(atn_problem, _obj);

	_hint = new QLabel(this);
	commonStyle::setHintStyle(_hint);

	initLayout();
	connect(_frequency_widget, SIGNAL(signal_checkValid()), this, SIGNAL(completeChanged()));
}

void wizardFreFarField::initLayout() {
	QLayout* frequency_layout = _frequency_widget->getLayout();

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addLayout(frequency_layout);
	layout->addStretch();
	layout->addWidget(_hint);
	
	layout->setContentsMargins(2, 20, 10, 2);
	setLayout(layout);
}

bool wizardFreFarField::isComplete() const {
	_hint->clear();
	/*QList<checkInfo*> cios;
	cios.append(_frequency_widget->checkInputValid());
	cios.append(_theta_phi_widget->checkInputValid());
	for (checkInfo* cio : cios) {
		if (cio->code != 0) {
			_hint->setText(cio->message);
			return false;
		}
	}*/
	QList<iTemplate*> templates = getTemplatesWidget();
	for (iTemplate* iter : templates) {
		if (!iter->checkInputValid()) {
			_hint->setText(iter->checkInfo->message);
			return false;
		}
	}
	return true;
}

QList<iTemplate*> wizardFreFarField::getTemplatesWidget() const {
	return QList<iTemplate*> {_frequency_widget};
}

/*wizardFreFarField::~wizardFreFarField(){
	delete _frequency_widget;
	_frequency_widget = nullptr;
	delete _theta_phi_widget;
	_theta_phi_widget = nullptr;
}*/