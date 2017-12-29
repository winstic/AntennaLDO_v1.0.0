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

	//register data field, with asterisk(*) means its a mandatory field
	registerField("frequency_low*", _frequency_widget->getFrequencyLowEdit());
	registerField("frequency_up*", _frequency_widget->getFrequencyUpEdit());
	registerField("frequency_num*", _frequency_widget->getFrequencyNumEdit());
	registerField("theta_low*", _theta_phi_widget->getThetaLowEdit());
	registerField("theta_up*", _theta_phi_widget->getThetaUpEdit());
	registerField("theta_step*", _theta_phi_widget->getThetaStepEdit());
	registerField("phi_low*", _theta_phi_widget->getPhiLowEdit());
	registerField("phi_up*", _theta_phi_widget->getPhiUpEdit());
	registerField("phi_step*", _theta_phi_widget->getPhiStepEdit());

	initLayout();
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
	QString frequency_low = field("frequency_low").toString();
	QString frequency_up = field("frequency_up").toString();
	QString frequency_num = field("frequency_num").toString();
	QString theta_low = field("theta_low").toString();
	QString theta_up = field("theta_up").toString();
	QString theta_step = field("theta_step").toString();
	QString phi_low = field("phi_low").toString();
	QString phi_up = field("phi_up").toString();
	QString phi_step = field("phi_step").toString();
	if (frequency_low.isEmpty() || frequency_low.isNull() || frequency_up.isEmpty() || frequency_up.isNull() ||
		frequency_num.isEmpty() || frequency_num.isNull() || theta_low.isEmpty() || theta_low.isNull() ||
		theta_up.isEmpty() || theta_up.isNull() || theta_step.isEmpty() || theta_step.isNull() ||
		phi_low.isEmpty() || phi_low.isNull() || phi_up.isEmpty() || phi_up.isNull() ||
		phi_step.isEmpty() || phi_step.isNull()) {
		_hint->setText("设置参数不能为空。");
		return false;
	}
	unsigned int frequency_low_int = frequency_low.toInt();
	unsigned int frequency_up_int = frequency_up.toInt();
	int theta_low_int, theta_up_int, theta_step_int, phi_low_int, phi_up_int, phi_step_int;
	theta_low_int = theta_low.toInt();
	theta_up_int = theta_up.toInt();
	theta_step_int = theta_step.toInt();
	phi_low_int = phi_low.toInt();
	phi_up_int = phi_up.toInt();
	phi_step_int = phi_step.toInt();
	if (frequency_low_int > frequency_up_int) {
		_hint->setText("频率范围设置有误。");
		return false;
	}
	if (theta_low_int > theta_up_int) {
		_hint->setText("theta角范围设置有误。");
		return false;
	}
	if (theta_step_int > (theta_up_int - theta_low_int)) {
		_hint->setText("theta角步长设置过大。");
		return false;
	}
	if (phi_low_int > phi_up_int) {
		_hint->setText("phi角范围设置有误。");
		return false;
	}
	if (phi_step_int > (phi_up_int - phi_low_int)) {
		_hint->setText("phi角步长设置过大。");
		return false;
	}
	//实时保存设置的最大频率
	_atn_problem->max_frequency = frequency_up_int;
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