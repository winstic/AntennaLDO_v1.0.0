#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "lossTemplate.h"

lossTemplate::lossTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj), _is_valid(true) {
	_loss_table = new tableTemplate();
	_loss_table->setColumnCount(9);
	QStringList header;
	header << "Z0实部" << "Z0虚部" << "损失方式" << "优化方式" << "误差实部" << "误差虚部" << "值实部" << "值虚部" << "权值";
	_loss_table->setHorizontalHeaderLabels(header);
	_loss_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_loss_table->setShowGrid(false);                               //setting no grid line

	_loss_table->resizeColumnToContents(6);
	_loss_table->resizeColumnToContents(7);
	initDefaultData();
	initLayout();
}


void lossTemplate::initDefaultData() {
	QJsonObject loss_obj = parseJson::getSubJsonObj(*_obj, "VSWRSetting");
	if (loss_obj.isEmpty()) {
		qCritical("get 'VSWRSetting' json object field.");
		checkInfo->code = eOther;
		checkInfo->message = "问题json文件格式不正确。";
		_is_valid = false;
		emit signal_checkValid();
		return;
	}
	QSignalMapper* loss_signals_map = new QSignalMapper;
	QStringList strListR0Real = dataPool::str2list(loss_obj.value("R0_real").toString());
	QStringList strListR0imag = dataPool::str2list(loss_obj.value("R0_imag").toString());
	QStringList strListReturnLossType = dataPool::str2list(loss_obj.value("ReturnLossType").toString());
	QStringList strListOptimaltype = dataPool::str2list(loss_obj.value("optimaltype_vswr").toString());
	QStringList strListDeltaReal = dataPool::str2list(loss_obj.value("delta_real").toString());
	QStringList strListDeltaImag = dataPool::str2list(loss_obj.value("delta_imag").toString());
	QStringList strListVswrobj = dataPool::str2list(loss_obj.value("vswrobj").toString());
	QStringList strListS11 = dataPool::str2list(loss_obj.value("S11").toString());
	QStringList strListR1Real = dataPool::str2list(loss_obj.value("R1_real").toString());
	QStringList strListR1Imag = dataPool::str2list(loss_obj.value("R1_imag").toString());
	QStringList strListWeight = dataPool::str2list(loss_obj.value("weight_vswr").toString());
	_loss_table->setRowCount(strListR0Real.length());
	for (int i = 0; i < strListR0Real.length(); i++) {
		QRegExpValidator* floatValidReg = getFloatReg();    //float
		QLineEdit* Z0_real_edit = new QLineEdit;
		Z0_real_edit->setValidator(floatValidReg);
		Z0_real_edit->setText(strListR0Real[i]);
		QLineEdit* Z0_image_edit = new QLineEdit;
		Z0_image_edit->setValidator(floatValidReg);
		Z0_image_edit->setText(strListR0imag[i]);
		_loss_table->setCellWidget(i, cz0real, Z0_real_edit);
		_loss_table->setCellWidget(i, cz0imag, Z0_image_edit);

		QComboBox* loss_type = new QComboBox;
		initLossTypeComBox(loss_type);
		loss_type->setCurrentIndex(QString(strListReturnLossType[i]).toInt());
		_loss_table->setCellWidget(i, closstype, loss_type);
		//map combobox signal
		connect(loss_type, SIGNAL(currentIndexChanged(int)), loss_signals_map, SLOT(map()));
		loss_signals_map->setMapping(loss_type, QString("%1-%2").arg(i).arg(closstype));

		QComboBox* optimal_type = new QComboBox;
		initOptimalTypeComBox(optimal_type);
		if(optimal_type->findText(strListOptimaltype[i]) != -1)
			optimal_type->setCurrentText(strListOptimaltype[i]);
		_loss_table->setCellWidget(i, clossoptimaltype, optimal_type);
		//map combobox signal
		connect(optimal_type, SIGNAL(currentIndexChanged(int)), loss_signals_map, SLOT(map()));
		loss_signals_map->setMapping(optimal_type, QString("%1-%2").arg(i).arg(clossoptimaltype));

		QLineEdit* delta_real_edit = new QLineEdit;
		delta_real_edit->setValidator(floatValidReg);		
		QLineEdit* delta_image_edit = new QLineEdit;
		delta_image_edit->setValidator(floatValidReg);		
		if (2 != optimal_type->currentIndex()) {
			delta_real_edit->setText("----");
			delta_real_edit->setEnabled(false);
			delta_image_edit->setText("----");
			delta_image_edit->setEnabled(false);
		}
		else {
			delta_real_edit->setText(strListDeltaReal[i]);
			delta_image_edit->setText(strListDeltaImag[i]);
		}
		_loss_table->setCellWidget(i, cdeltareal, delta_real_edit);
		_loss_table->setCellWidget(i, cdeltaimag, delta_image_edit);
		//setting cannot edit when optimize type is delta

		QLineEdit* obj_real_edit = new QLineEdit;
		obj_real_edit->setValidator(floatValidReg);
		QLineEdit* obj_image_edit = new QLineEdit;
		obj_image_edit->setValidator(floatValidReg);
		if (0 == loss_type->currentIndex()) {
			//loss type is vswr
			obj_real_edit->setText(strListVswrobj[i]);
			obj_image_edit->setText("----");
			obj_image_edit->setEnabled(false);
		}
		else if (1 == loss_type->currentIndex()) {
			//loss type is S11
			obj_real_edit->setText(strListS11[i]);
			obj_image_edit->setText("----");
			obj_image_edit->setEnabled(false);
		}
		else if (2 == loss_type->currentIndex()) {
			//loss type is R
			obj_real_edit->setText(strListR1Real[i]);
			obj_image_edit->setText(strListR1Imag[i]);
		}
		else {
			//loss type is None
			obj_real_edit->setText("----");
			obj_real_edit->setEnabled(false);
			obj_image_edit->setText("----");
			obj_image_edit->setEnabled(false);
		}
		_loss_table->setCellWidget(i, cobjreal, obj_real_edit);
		_loss_table->setCellWidget(i, cobjimag, obj_real_edit);

		QLineEdit* weight_edit = new QLineEdit;
		weight_edit->setValidator(floatValidReg);
		weight_edit->setText(strListWeight[i]);
		_loss_table->setCellWidget(i, clossweight, weight_edit);
	}
	connect(loss_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_ChangeOptimaltype(QString)));
}

void lossTemplate::initLossTypeComBox(QComboBox *combox) {
	combox->addItem("驻波比");
	combox->addItem("S11(dB)");
	combox->addItem("阻抗");
	combox->addItem("None");
	combox->setCurrentIndex(3);
}


void lossTemplate::initLayout() {
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(_loss_table);
	_layout = layout;
}

QLayout* lossTemplate::getLayout() {
	return _layout;
}

void lossTemplate::checkNull(QLineEdit* edit) {	
	checkInfo->code = eInvalid;
	checkInfo->message = "设置参数不能为空";
	_is_valid = false;
	commonStyle::setLineEditWarningStyle(edit);
	emit signal_checkValid();
}
bool lossTemplate::checkInputValid() {
	if (!_is_valid) return false;
	for (int i = 0; i < _loss_table->rowCount(); i++) {
		QLineEdit* z0_real_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cz0real));
		QLineEdit* z0_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cz0imag));
		QComboBox *lossType = qobject_cast<QComboBox *>(_loss_table->cellWidget(i, closstype));
		QComboBox *loType = qobject_cast<QComboBox *>(_loss_table->cellWidget(i, clossoptimaltype));
		QLineEdit* delta_real_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cdeltareal));
		QLineEdit* delta_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cdeltaimag));
		QLineEdit* obj_real_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cobjreal));
		QLineEdit* obj_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cobjimag));
		QLineEdit* weight_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, clossweight));

		if (z0_real_edit->text().isEmpty()) {
			qCritical("z0 real value is null.");
			checkNull(z0_real_edit);
			return false;
		}
		if (z0_image_edit->text().isEmpty()) {
			qCritical("z0 image value is null.");
			checkNull(z0_image_edit);
			return false;
		}

		QString delta_real_value = delta_real_edit->text().trimmed();
		QString delta_image_value = delta_image_edit->text().trimmed();
		QString obj_real_value = obj_real_edit->text().trimmed();
		QString obj_image_value = obj_image_edit->text().trimmed();
		QString weight_value = weight_edit->text().trimmed();

		if (loType->currentIndex() == odelta) {
			if (delta_real_value.isEmpty() || delta_real_value == "----") {
				qCritical("delta real value is invalid.");
				checkNull(delta_real_edit);
				return false;
			}
			if (delta_image_value.isEmpty() || delta_image_value == "----") {
				qCritical("delta image value is invalid.");
				checkNull(delta_image_edit);
				return false;
			}
		}
		if (obj_real_value.isEmpty() || obj_real_value.isNull()) {
			qCritical("loss object value is invalid.");
			checkNull(obj_real_edit);
			return false;
		}
		if (lossType->currentIndex() == 2) {
			//R
			if (obj_image_value.isEmpty() || obj_image_value.isNull()) {
				qCritical("R image value is invalid.");
				checkNull(obj_image_edit);
				return false;
			}
		}
		if (weight_value.isEmpty() || weight_value.isNull()) {
			qCritical("weight value is invalid.");
			checkNull(weight_edit);
			return false;
		}
	}
	return true;
}


//update json obj
void lossTemplate::updateJObj() {
	QJsonObject mloss_obj;
	//update loss obj
	QStringList lossStr[11];
	for (int i = 0; i < _loss_table->rowCount(); i++) {
		QLineEdit* z0_real_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cz0real));
		QLineEdit* z0_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cz0imag));
		lossStr[0] << z0_real_edit->text().trimmed();
		lossStr[1] << z0_image_edit->text().trimmed();

		QComboBox *lossType = qobject_cast<QComboBox *>(_loss_table->cellWidget(i, closstype));
		lossStr[2] << QString::number(lossType->currentIndex());
		QComboBox *loType = qobject_cast<QComboBox *>(_loss_table->cellWidget(i, clossoptimaltype));
		if (3 == loType->currentIndex())
			lossStr[3] << loType->currentText().trimmed();
		else
			lossStr[3] << QString("'%1'").arg(loType->currentText().trimmed());

		QLineEdit* delta_real_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cdeltareal));
		QLineEdit* delta_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cdeltaimag));
		QString delta_real = delta_real_edit->text().trimmed();
		QString delta_image = delta_image_edit->text().trimmed();
		if (odelta != loType->currentIndex()) {
			delta_real = "None";
			delta_image = "None";
		}
		lossStr[4] << delta_real;
		lossStr[5] << delta_image;

		QLineEdit* obj_real_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cobjreal));
		QLineEdit* obj_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, cobjimag));
		QString obj_real = obj_real_edit->text().trimmed();
		QString obj_image = obj_image_edit->text().trimmed();
		if (0 == lossType->currentIndex()) {
			//vswr
			lossStr[6] << obj_real;
			lossStr[7] << "None";
			lossStr[8] << "None";
			lossStr[9] << "None";
		}
		else if (1 == lossType->currentIndex()) {
			//s11
			lossStr[6] << "None";
			lossStr[7] << obj_real;
			lossStr[8] << "None";
			lossStr[9] << "None";
		}
		else if (2 == lossType->currentIndex()) {
			//R
			lossStr[6] << "None";
			lossStr[7] << "None";
			lossStr[8] << obj_real;
			lossStr[9] << obj_image;
		}
		else {
			//None
			lossStr[6] << "None";
			lossStr[7] << "None";
			lossStr[8] << "None";
			lossStr[9] << "None";
		}
		QLineEdit* weight_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(i, clossweight));
		lossStr[10] << weight_edit->text().trimmed();
	}
	mloss_obj.insert("R0_real", QString("[[%1]]").arg(lossStr[0].join(",")));
	mloss_obj.insert("R0_imag", QString("[[%1]]").arg(lossStr[1].join(",")));
	mloss_obj.insert("ReturnLossType", QString("[[%1]]").arg(lossStr[2].join(",")));
	mloss_obj.insert("optimaltype_vswr", QString("[[%1]]").arg(lossStr[3].join(",")));
	mloss_obj.insert("delta_real", QString("[[%1]]").arg(lossStr[4].join(",")));
	mloss_obj.insert("delta_imag", QString("[[%1]]").arg(lossStr[5].join(",")));
	mloss_obj.insert("vswrobj", QString("[[%1]]").arg(lossStr[6].join(",")));
	mloss_obj.insert("S11", QString("[[%1]]").arg(lossStr[7].join(",")));
	mloss_obj.insert("R1_real", QString("[[%1]]").arg(lossStr[8].join(",")));
	mloss_obj.insert("R1_imag", QString("[[%1]]").arg(lossStr[9].join(",")));
	mloss_obj.insert("weight_vswr", QString("[[%1]]").arg(lossStr[10].join(",")));
	_obj->insert("VSWRSetting", mloss_obj);
}

void lossTemplate::slot_ChangeOptimaltype(QString pos) {
	QStringList coordinates = pos.split("-");
	int row = coordinates[0].toInt();
	int col = coordinates[1].toInt();
	QComboBox *selectCombox = qobject_cast<QComboBox *>(_loss_table->cellWidget(row, col));
	QLineEdit* delta_real_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(row, cdeltareal));
	QLineEdit* delta_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(row, cdeltaimag));
	QLineEdit* obj_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(row, cobjimag));
	//setting edit when optimize type is delta
	if (col == clossoptimaltype) {
		if (odelta == selectCombox->currentIndex()) {
			delta_real_edit->setEnabled(true);
			delta_image_edit->setEnabled(true);
		}
		else {
			delta_real_edit->setEnabled(false);
			delta_image_edit->setEnabled(false);
		}
	}
	if (col == closstype) {
		if (2 == selectCombox->currentIndex())
			obj_image_edit->setEnabled(true);
		else
			obj_image_edit->setEnabled(false);
	}
}


lossTemplate::~lossTemplate() {}