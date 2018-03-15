#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "thetaPhiTemplate.h"

thetaPhiTemplate::thetaPhiTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj){

	_far_field_table = new tableTemplate(this);
	_far_field_table->setColumnCount(6);
	QStringList header;
	header << "起始θ" << "终止θ" << "θ步长" << "起始φ" << "终止φ" << "φ步长";
	_far_field_table->setHorizontalHeaderLabels(header);
	_far_field_table->verticalHeader()->setVisible(false);              //setting no vertical header
	_far_field_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_far_field_table->setFixedHeight(60);
	_far_field_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	_far_field_table->setShowGrid(false);                               //setting no grid line

	_confirm_button = new QPushButton("确认", this); 

	_theta_low_edit = new QLineEdit(this);
	_theta_up_edit = new QLineEdit(this);
	_theta_step_edit = new QLineEdit(this);
	_phi_low_edit = new QLineEdit(this);
	_phi_up_edit = new QLineEdit(this);
	_phi_step_edit = new QLineEdit(this);

	initReg();
	initDefaultData();	
	initLayout();

	connect(_confirm_button, SIGNAL(clicked(bool)), this, SLOT(slot_confirmButton(bool)));
	//当文本框输入改变时，触发校验信号；
	connect(_theta_low_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_theta_up_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_phi_low_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_phi_up_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_theta_step_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
	connect(_phi_step_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));

	//当文本框输入改变时，触发远场校验信号；
	connect(_theta_low_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_editFarField()));
	connect(_theta_up_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_editFarField()));
	connect(_phi_low_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_editFarField()));
	connect(_phi_up_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_editFarField()));
	connect(_theta_step_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_editFarField()));
	connect(_phi_step_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_editFarField()));
}

void thetaPhiTemplate::initReg() {
	//设置正则表达式
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
		QMessageBox::critical(0, QString("警告"), QString("读取问题配置文件失败！"));
		return;
	}
	_far_field_table->setRowCount(1);

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
	_far_field_table->setCellWidget(0, fthetastart, _theta_low_edit);
	_far_field_table->setCellWidget(0, fthetaend, _theta_up_edit);
	_far_field_table->setCellWidget(0, fthetastep, _theta_step_edit);
	_far_field_table->setCellWidget(0, fphistart, _phi_low_edit);
	_far_field_table->setCellWidget(0, fphiend, _phi_up_edit);
	_far_field_table->setCellWidget(0, fphistep, _phi_step_edit);
}

void thetaPhiTemplate::initLayout() {
	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addWidget(_far_field_table);
	hlayout->addWidget(_confirm_button);
	_layout = hlayout;
}

QLayout* thetaPhiTemplate::getLayout() {
	return _layout;
}

bool thetaPhiTemplate::checkInputValid() {
	QString theta_low = _theta_low_edit->text().trimmed();
	QString theta_up = _theta_up_edit->text().trimmed();
	QString theta_step = _theta_step_edit->text().trimmed();
	QString phi_low = _phi_low_edit->text().trimmed();
	QString phi_up = _phi_up_edit->text().trimmed();
	QString phi_step = _phi_step_edit->text().trimmed();
	QList<QLineEdit *> angles{ _theta_low_edit, _theta_up_edit, _theta_step_edit, _phi_low_edit, _phi_up_edit, _phi_step_edit };
	for (QLineEdit* item : angles) {
		if (item->text().trimmed().isEmpty() || item->text().trimmed().isNull()) {
			checkInfo->code = eNull;
			checkInfo->message = "设置参数不能为空。";
			commonStyle::setLineEditWarningStyle(item);
			return false;
		}
	}
	for (QLineEdit* item : angles) {
		if (item->text().trimmed() == "-") {
			checkInfo->code = eOther;
			checkInfo->message = "设置参数输入不完整。";
			commonStyle::setLineEditWarningStyle(item);
			return false;
		}
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
		commonStyle::setLineEditWarningStyle(_theta_low_edit);
		commonStyle::setLineEditWarningStyle(_theta_up_edit);
		return false;
	}
	if (theta_step_d > (theta_up_d - theta_low_d)) {
		checkInfo->code = eInvalid;
		checkInfo->message = "theta角步长设置过大。";
		commonStyle::setLineEditWarningStyle(_theta_step_edit);
		return false;
	}
	if (phi_low_d > phi_up_d) {
		checkInfo->code = eInvalid;
		checkInfo->message = "phi角范围设置有误。";
		commonStyle::setLineEditWarningStyle(_phi_low_edit);
		commonStyle::setLineEditWarningStyle(_phi_up_edit);
		return false;
	}
	if (phi_step_d > (phi_up_d - phi_low_d)) {
		checkInfo->code = eInvalid;
		checkInfo->message = "phi角步长设置过大。";
		commonStyle::setLineEditWarningStyle(_phi_step_edit);
		return false;
	}
	commonStyle::clearLineEditWarningStyle(_theta_low_edit);
	commonStyle::clearLineEditWarningStyle(_theta_up_edit);
	commonStyle::clearLineEditWarningStyle(_theta_step_edit);
	commonStyle::clearLineEditWarningStyle(_phi_low_edit);
	commonStyle::clearLineEditWarningStyle(_phi_up_edit);
	commonStyle::clearLineEditWarningStyle(_phi_step_edit);
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


//slots
void thetaPhiTemplate::slot_confirmButton(bool){
	QString info;
	if (!checkInputValid()) {
		QMessageBox::critical(0, QString("警告"), checkInfo->message);
		info = "";
	}
	else {
		info = QString("%1#%2#%3#%4#%5#%6").arg(_theta_low_edit->text().trimmed()).arg(_theta_up_edit->text().trimmed())
			.arg(_theta_step_edit->text().trimmed()).arg(_phi_low_edit->text().trimmed())
			.arg(_phi_up_edit->text().trimmed()).arg(_phi_step_edit->text().trimmed());

		QMessageBox *tip = new QMessageBox;
		QTimer::singleShot(1000, tip, SLOT(close()));
		tip->setWindowTitle("提示");
		tip->setIcon(QMessageBox::Information);
		tip->setText("正在更新远场范围......");
		tip->show();
	}

	emit signal_confirmFarField(info);
}

void thetaPhiTemplate::slot_editFarField() {
	emit signal_confirmFarField("");
}

thetaPhiTemplate::~thetaPhiTemplate(){}