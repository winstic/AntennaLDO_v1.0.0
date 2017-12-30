#pragma execution_character_set("utf-8")
#include "wizardFreFarField.h"
#include "../Utility/parseJson.h"

wizardFreFarField::wizardFreFarField(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent) : QWizardPage(parent),
 _atn_problem(atn_problem), _obj(obj){    
	setTitle("辐射频率及远场范围参数设置");
	_frequency_widget = new frequencyTemplate(atn_problem, _obj);
	_theta_phi_widget = new thetaPhiTemplate(atn_problem, _obj);

	_hint = new QLabel(this);
	QFont font;
	font.setPixelSize(20);
	_hint->setFont(font);

	initLayout();
	connect(_frequency_widget, SIGNAL(signal_checkValid()), this, SIGNAL(completeChanged()));
	connect(_theta_phi_widget, SIGNAL(signal_checkValid()), this, SIGNAL(completeChanged()));
}

void wizardFreFarField::initLayout() {
	QGroupBox* frequency_group_box = new QGroupBox("频率设置");
	QGroupBox* far_field_group_box = new QGroupBox("远场范围设置");
	QLayout* frequency_layout = _frequency_widget->getLayout();
	frequency_group_box->setLayout(frequency_layout);
	QLayout* theta_phi_layout = _theta_phi_widget->getLayout();
	far_field_group_box->setLayout(theta_phi_layout);
	QVBoxLayout* v_layout = new QVBoxLayout;
	QHBoxLayout *hlayout = new QHBoxLayout;
	v_layout->addWidget(frequency_group_box);
	v_layout->addWidget(far_field_group_box);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
	hlayout->addWidget(_hint);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	v_layout->addLayout(hlayout);
	v_layout->setSpacing(50);
	v_layout->setContentsMargins(2, 20, 10, 2);
	setLayout(v_layout);
}

bool wizardFreFarField::isComplete() const {
	_hint->clear();
	QList<checkInfo*> cios;
	cios.append(_frequency_widget->checkInputValid());
	cios.append(_theta_phi_widget->checkInputValid());
	for (checkInfo* cio : cios) {
		if (cio->code != 0) {
			_hint->setText(cio->message);
			return false;
		}
	}
	return true;
}

QList<iTemplate*> wizardFreFarField::getTemplatesWidget() const {
	return QList<iTemplate*> {_frequency_widget, _theta_phi_widget};
}

/*wizardFreFarField::~wizardFreFarField(){
	delete _frequency_widget;
	_frequency_widget = nullptr;
	delete _theta_phi_widget;
	_theta_phi_widget = nullptr;
}*/