#include "wizardFreFarField.h"
#include "../Utility/parseJson.h"

wizardFreFarField::wizardFreFarField(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QWizardPage(parent),
 _atn_problem(atn_problem), _obj(obj), _temp_widget(atn_problem, obj){
    
	setTitle(tr("性能参数设置"));
    setSubTitle(tr("设置频率信息并指定远场范围"));
    
	registerField("frequency_low*", problemTemplate::_frequency_low_edit);
	registerField("frequency_up*", problemTemplate::_frequency_up_edit);
	registerField("frequency_num*", problemTemplate::_frequency_num_edit);
	registerField("sweep_type", problemTemplate::_sweep_type_combox);
	registerField("polarization", problemTemplate::_polarization_combox);
	registerField("theta_low*", problemTemplate::_theta_low_edit);
	registerField("theta_up*", problemTemplate::_theta_up_edit);
	registerField("theta_step*", problemTemplate::_theta_step_edit);
	registerField("phi_low*", problemTemplate::_phi_low_edit);
	registerField("phi_up*", problemTemplate::_phi_up_edit);
	registerField("phi_step*", problemTemplate::_phi_step_edit);

	//layout
	QGroupBox frequency_group_box(tr("频率设置"));
	QGroupBox far_field_group_box(tr("远场范围设置"));
	QGridLayout frequency_layout;
	_temp_widget.initFrequencyWidget(&frequency_layout);
	frequency_group_box.setLayout(&frequency_layout);
	QGridLayout far_field_layout;
	_temp_widget.initFarFieldWidget(&far_field_layout);
	far_field_group_box.setLayout(&far_field_layout);
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

wizardFreFarField::~wizardFreFarField(){}