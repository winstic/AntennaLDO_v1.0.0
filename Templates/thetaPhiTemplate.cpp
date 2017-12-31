#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "thetaPhiTemplate.h"

thetaPhiTemplate::thetaPhiTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj), _is_valid(true) {
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

	connect(_theta_low_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_theta_up_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_theta_step_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_phi_low_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_phi_up_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_phi_step_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
}

void thetaPhiTemplate::initRegex() {
	QRegExpValidator* nonNegFloatValid = getNonNegativeFloatReg();    //non negative float
	QRegExpValidator* rangeFloatValid = getAngleReg();     //float [-180, 180]
	_theta_low_edit->setValidator(rangeFloatValid);
	_theta_up_edit->setValidator(rangeFloatValid);
	_phi_low_edit->setValidator(rangeFloatValid);
	_phi_up_edit->setValidator(rangeFloatValid);
	_theta_step_edit->setValidator(nonNegFloatValid);	
	_phi_step_edit->setValidator(nonNegFloatValid);
}

void thetaPhiTemplate::initDefaultData() {
	QJsonObject far_field_obj = parseJson::getSubJsonObj(*_obj, "ThetaPhiStep");
	if (far_field_obj.isEmpty()) {
		qCritical("get 'ThetaPhiStep' json object field.");
		checkInfo->code = eOther;
		checkInfo->message = "问题json文件格式不正确。";
		_is_valid = false;
		emit signal_checkValid();
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
	QGridLayout* far_field_layout = new QGridLayout;
	far_field_layout->addWidget(_theta_low_label, 0, 0);
	far_field_layout->addWidget(_theta_low_edit, 0, 1);
	far_field_layout->addWidget(_theta_up_label, 0, 2);
	far_field_layout->addWidget(_theta_up_edit, 0, 3);
	far_field_layout->addWidget(_theta_step_label, 0, 4);
	far_field_layout->addWidget(_theta_step_edit, 0, 5);
	far_field_layout->addWidget(_phi_low_label, 1, 0);
	far_field_layout->addWidget(_phi_low_edit, 1, 1);
	far_field_layout->addWidget(_phi_up_label, 1, 2);
	far_field_layout->addWidget(_phi_up_edit, 1, 3);
	far_field_layout->addWidget(_phi_step_label, 1, 4);
	far_field_layout->addWidget(_phi_step_edit, 1, 5);
	_layout = far_field_layout;
}

QLayout* thetaPhiTemplate::getLayout() {
	return _layout;
}

bool thetaPhiTemplate::checkInputValid() {
	if (!_is_valid) return false;
	QString theta_low = _theta_low_edit->text().trimmed();
	QString theta_up = _theta_up_edit->text().trimmed();
	QString theta_step = _theta_step_edit->text().trimmed();
	QString phi_low = _phi_low_edit->text().trimmed();
	QString phi_up = _phi_up_edit->text().trimmed();
	QString phi_step = _phi_step_edit->text().trimmed();
	if (theta_low.isEmpty() || theta_low.isNull() || theta_up.isEmpty() || theta_up.isNull() || theta_step.isEmpty() || 
		theta_step.isNull() || phi_low.isEmpty() || phi_low.isNull() || phi_up.isEmpty() || phi_up.isNull() ||
		phi_step.isEmpty() || phi_step.isNull()) {
		checkInfo->code = eNull;
		checkInfo->message = "设置参数不能为空。";
		return false;
	}
	if (theta_low == "-" || theta_up == "-" || phi_low == "-" || phi_up == "-") {
		checkInfo->code = eOther;
		checkInfo->message = "设置参数输入不完整。";
		return false;
	}
	double theta_low_d, theta_up_d, theta_step_d, phi_low_d, phi_up_d, phi_step_d;
	theta_low_d = theta_low.toDouble();
	theta_up_d = theta_up.toDouble();
	theta_step_d = theta_step.toDouble();
	phi_low_d = phi_low.toDouble();
	phi_up_d = phi_up.toDouble();
	phi_step_d = phi_step.toDouble();
	if (theta_low_d > theta_up_d) {
		checkInfo->code = eInvalid;
		checkInfo->message = "theta角范围设置有误。";
		return false;
	}
	if (theta_step_d > (theta_up_d - theta_low_d)) {
		checkInfo->code = eInvalid;
		checkInfo->message = "theta角步长设置过大。";
		return false;
	}
	if (phi_low_d > phi_up_d) {
		checkInfo->code = eInvalid;
		checkInfo->message = "phi角范围设置有误。";
		return false;
	}
	if (phi_step_d > (phi_up_d - phi_low_d)) {
		checkInfo->code = eInvalid;
		checkInfo->message = "phi角步长设置过大。";
		return false;
	}
	return true;
}

//update json obj
void thetaPhiTemplate::updateJObj() {
	QJsonObject mfar_fiel_obj;
	mfar_fiel_obj.insert("ThetaLower", QString("[%1]").arg(_theta_low_edit->text().trimmed()));
	mfar_fiel_obj.insert("ThetaUpper", QString("[%1]").arg(_theta_up_edit->text().trimmed()));
	mfar_fiel_obj.insert("ThetaStep", QString("[%1]").arg(_theta_step_edit->text().trimmed()));
	mfar_fiel_obj.insert("PhiLower", QString("[%1]").arg(_phi_low_edit->text().trimmed()));
	mfar_fiel_obj.insert("PhiUpper", QString("[%1]").arg(_phi_up_edit->text().trimmed()));
	mfar_fiel_obj.insert("PhiStep", QString("[%1]").arg(_phi_step_edit->text().trimmed()));
	_obj->insert("ThetaPhiStep", mfar_fiel_obj);
}

thetaPhiTemplate::~thetaPhiTemplate(){}