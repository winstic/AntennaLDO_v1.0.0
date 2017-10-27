#include "../Utility/parseJson.h"
#include "thetaPhiTemplate.h"

thetaPhiTemplate::thetaPhiTemplate(parsProblem* atn_problem, QJsonObject& obj, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj) {
	_theta_low_label = new QLabel("起始θ:", this);
	_theta_up_label = new QLabel("终止θ:", this);
	_theta_step_label = new QLabel("θ步长:", this);
	_phi_low_label = new QLabel("起始φ:", this);
	_phi_up_label = new QLabel("终止φ:", this);
	_phi_step_label = new QLabel("φ步长:", this);
	_theta_low_edit = new QLineEdit(this);
	_theta_up_edit = new QLineEdit(this);
	_theta_step_edit = new QLineEdit(this);
	_phi_low_edit = new QLineEdit(this);
	_phi_up_edit = new QLineEdit(this);
	_phi_step_edit = new QLineEdit(this);
	initRegex();
	initDefaultData();
	initLayout();
}

void thetaPhiTemplate::initRegex() {
	QRegExpValidator nonNegFloatValid(QRegExp("^(\\d+)(\\.\\d+)?$"));    //non negative float
	QRegExpValidator floatValid(QRegExp("^-?(180|1?[0-7]?\\d(\\.\\d+)?)$"));      //float [-180. 180]
	_theta_low_edit->setValidator(&floatValid);
	_theta_up_edit->setValidator(&floatValid);
	_theta_step_edit->setValidator(&nonNegFloatValid);
	_phi_low_edit->setValidator(&floatValid);
	_phi_up_edit->setValidator(&floatValid);
	_phi_step_edit->setValidator(&nonNegFloatValid);
}

void thetaPhiTemplate::initDefaultData() {
	QJsonObject far_field_obj = parseJson::getSubJsonObj(_obj, "ThetaPhiStep");
	if (far_field_obj.isEmpty()) {
		qCritical("get 'ThetaPhiStep' json object field.");
		return;
	}
	QStringList str_list;
	str_list = dataPool::str2list(far_field_obj.value("ThetaLower").toString().trimmed());
	_theta_low_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("ThetaUpper").toString().trimmed());
	_theta_up_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("ThetaStep").toString().trimmed());
	_theta_step_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("PhiLower").toString().trimmed());
	_phi_low_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("PhiUpper").toString().trimmed());
	_phi_up_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("PhiStep").toString().trimmed());
	_phi_step_edit->setText(str_list[0]);
}

void thetaPhiTemplate::initLayout() {
	QGridLayout far_field_layout;
	far_field_layout.addWidget(_theta_low_label, 0, 0);
	far_field_layout.addWidget(_theta_low_edit, 0, 1);
	far_field_layout.addWidget(_theta_up_label, 0, 2);
	far_field_layout.addWidget(_theta_up_edit, 0, 3);
	far_field_layout.addWidget(_theta_step_label, 0, 4);
	far_field_layout.addWidget(_theta_step_edit, 0, 5);
	far_field_layout.addWidget(_phi_low_label, 1, 0);
	far_field_layout.addWidget(_phi_low_edit, 1, 1);
	far_field_layout.addWidget(_phi_up_label, 1, 2);
	far_field_layout.addWidget(_phi_up_edit, 1, 3);
	far_field_layout.addWidget(_phi_step_label, 1, 4);
	far_field_layout.addWidget(_phi_step_edit, 1, 5);
	_layout = &far_field_layout;
}

QLayout* thetaPhiTemplate::getLayout() {
	return _layout;
}

//update json obj
void thetaPhiTemplate::updateJObj() {
	QJsonObject mfar_fiel_obj;
	mfar_fiel_obj.insert("ThetaLower", _theta_low_edit->text().trimmed());
	mfar_fiel_obj.insert("ThetaUpper", _theta_up_edit->text().trimmed());
	mfar_fiel_obj.insert("ThetaStep", _theta_step_edit->text().trimmed());
	mfar_fiel_obj.insert("PhiLower", _phi_low_edit->text().trimmed());
	mfar_fiel_obj.insert("PhiUpper", _phi_up_edit->text().trimmed());
	mfar_fiel_obj.insert("PhiStep", _phi_step_edit->text().trimmed());
	_obj.insert("ThetaPhiStep", mfar_fiel_obj);
}

thetaPhiTemplate::~thetaPhiTemplate(){}