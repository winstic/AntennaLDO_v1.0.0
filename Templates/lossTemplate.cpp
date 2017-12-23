#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "lossTemplate.h"

lossTemplate::lossTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj) {
	_loss_table = new tableTemplate();
	_loss_table->setColumnCount(9);
	QStringList header;
	header << "Z0实部" << "Z0虚部" << "损失方式" << "优化方式" << "误差实部" << "误差虚部" << "值实部" << "值虚部" << "权值";
	_loss_table->setHorizontalHeaderLabels(header);
	_loss_table->resizeColumnToContents(6);
	_loss_table->resizeColumnToContents(7);
	initDefaultData();
	initLayout();
}


void lossTemplate::initDefaultData() {
	QJsonObject loss_obj = parseJson::getSubJsonObj(*_obj, "VSWRSetting");
	if (loss_obj.isEmpty()) {
		qCritical("get 'VSWRSetting' json object field.");
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
		_loss_table->insert2table(i, cz0real, strListR0Real[i]);
		_loss_table->insert2table(i, cz0imag, strListR0imag[i]);

		QComboBox* loss_type = new QComboBox;
		initLossTypeComBox(loss_type);
		loss_type->setCurrentIndex(QString(strListReturnLossType[i]).toInt());
		_loss_table->setCellWidget(i, closstype, loss_type);
		//map combobox signal
		connect(loss_type, SIGNAL(currentIndexChanged(int)), loss_signals_map, SLOT(map()));
		loss_signals_map->setMapping(loss_type, QString("%1-%2").arg(i).arg(closstype));

		QComboBox* optimal_type = new QComboBox;
		initOptimalTypeComBox(optimal_type);
		optimal_type->setCurrentText(strListOptimaltype[i]);
		_loss_table->setCellWidget(i, clossoptimaltype, optimal_type);
		//map combobox signal
		connect(optimal_type, SIGNAL(currentIndexChanged(int)), loss_signals_map, SLOT(map()));
		loss_signals_map->setMapping(optimal_type, QString("%1-%2").arg(i).arg(clossoptimaltype));

		_loss_table->insert2table(i, cdeltareal, strListDeltaReal[i]);
		_loss_table->insert2table(i, cdeltaimag, strListDeltaImag[i]);
		//setting cannot edit when optimize type is delta
		if (2 != optimal_type->currentIndex()) {
			_loss_table->item(i, cdeltareal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(i, cdeltaimag)->setFlags(Qt::NoItemFlags);
		}
		if (0 == loss_type->currentIndex()) {
			//loss type is vswr
			_loss_table->insert2table(i, cobjreal, strListVswrobj[i]);
			_loss_table->setItem(i, cobjimag, new QTableWidgetItem("None"));
			_loss_table->item(i, cobjimag)->setFlags(Qt::NoItemFlags);
		}
		else if (1 == loss_type->currentIndex()) {
			//loss type is S11
			_loss_table->insert2table(i, cobjreal, strListS11[i]);
			_loss_table->setItem(i, cobjimag, new QTableWidgetItem("None"));
			_loss_table->item(i, cobjimag)->setFlags(Qt::NoItemFlags);
		}
		else if (2 == loss_type->currentIndex()) {
			//loss type is R
			_loss_table->insert2table(i, cobjreal, strListR1Real[i]);
			_loss_table->insert2table(i, cobjimag, strListR1Imag[i]);
		}
		else {
			//loss type is None
			_loss_table->setItem(i, cobjreal, new QTableWidgetItem("None"));
			_loss_table->setItem(i, cobjimag, new QTableWidgetItem("None"));
			_loss_table->item(i, cobjreal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(i, cobjimag)->setFlags(Qt::NoItemFlags);
		}
		_loss_table->insert2table(i, clossweight, strListWeight[i]);
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

//update json obj
void lossTemplate::updateJObj() {
	QJsonObject mloss_obj;
	//update loss obj
	QStringList lossStr[11];
	for (int i = 0; i < _loss_table->rowCount(); i++) {
		lossStr[0] << _loss_table->item(i, cz0real)->text().trimmed();
		lossStr[1] << _loss_table->item(i, cz0imag)->text().trimmed();
		QComboBox *lossType = static_cast<QComboBox *>(_loss_table->cellWidget(i, closstype));
		lossStr[2] << QString::number(lossType->currentIndex());
		QComboBox *loType = static_cast<QComboBox *>(_loss_table->cellWidget(i, clossoptimaltype));
		if (3 == loType->currentIndex())
			lossStr[3] << loType->currentText().trimmed();
		else
			lossStr[3] << QString("'%1'").arg(loType->currentText().trimmed());
		lossStr[4] << _loss_table->item(i, cdeltareal)->text().trimmed();
		lossStr[5] << _loss_table->item(i, cdeltaimag)->text().trimmed();
		if (0 == lossType->currentIndex()) {
			lossStr[6] << _loss_table->item(i, cobjreal)->text().trimmed();
			lossStr[7] << "None";
			lossStr[8] << "None";
		}
		else if (1 == lossType->currentIndex()) {
			lossStr[6] << "None";
			lossStr[7] << _loss_table->item(i, cobjreal)->text().trimmed();
			lossStr[8] << "None";
		}
		else if (2 == lossType->currentIndex()) {
			lossStr[6] << "None";
			lossStr[7] << "None";
			lossStr[8] << _loss_table->item(i, cobjreal)->text().trimmed();
		}
		else {
			lossStr[6] << "None";
			lossStr[7] << "None";
			lossStr[8] << "None";
		}
		lossStr[9] << _loss_table->item(i, cobjimag)->text().trimmed();
		lossStr[10] << _loss_table->item(i, clossweight)->text().trimmed();
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
	QComboBox *selectCombox = (QComboBox *)_loss_table->cellWidget(row, col);
	//setting edit when optimize type is delta
	if (col == clossoptimaltype) {
		if (2 == selectCombox->currentIndex()) {
			_loss_table->item(row, cdeltareal)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			_loss_table->item(row, cdeltaimag)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		}
		else {
			_loss_table->item(row, cdeltareal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(row, cdeltaimag)->setFlags(Qt::NoItemFlags);
		}
	}
	if (col == closstype) {
		if (2 == selectCombox->currentIndex())
			_loss_table->item(row, cobjimag)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		else
			_loss_table->item(row, cobjimag)->setFlags(Qt::NoItemFlags);
	}
}


lossTemplate::~lossTemplate() {}