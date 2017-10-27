#include "wizardFreFarField.h"
#include "../Utility/parseJson.h"

wizardFreFarField::wizardFreFarField(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QWizardPage(parent),
 _atn_problem(atn_problem), _obj(obj){    
	setTitle(tr("性能参数设置"));
    setSubTitle(tr("设置频率信息并指定远场范围"));
	_frequency_widget = new frequencyTemplate(atn_problem, obj);
	_theta_phi_widget = new thetaPhiTemplate(atn_problem, obj);
	//layout
	QGroupBox frequency_group_box(tr("频率设置"));
	QGroupBox far_field_group_box(tr("远场范围设置"));
	QLayout* frequency_layout = _frequency_widget->getLayout();
	frequency_group_box.setLayout(frequency_layout);
	QLayout* theta_phi_layout = _theta_phi_widget->getLayout();
	far_field_group_box.setLayout(theta_phi_layout);
	QVBoxLayout v_layout;
	v_layout.addWidget(&frequency_group_box);
	v_layout.addWidget(&far_field_group_box);
	v_layout.setSpacing(50);
	v_layout.setContentsMargins(2, 20, 2, 2);
	setLayout(&v_layout);
	//QHBoxLayout l_ayout;
	//l_ayout.addLayout(&v_layout);
	//setLayout(&l_ayout);
	//!
}

bool wizardFreFarField::isComplete() const {
	return true;
}

QList<iTemplate*> wizardFreFarField::getTemplatesWidget() const {
	return QList<iTemplate*> {_frequency_widget, _theta_phi_widget};
}

wizardFreFarField::~wizardFreFarField(){
	delete _frequency_widget;
	_frequency_widget = nullptr;
	delete _theta_phi_widget;
	_theta_phi_widget = nullptr;
}