#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "lineEditDelegateTemplate.h"
#include "lossTemplate.h"

lossTemplate::lossTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj), _index(index) {
	_loss_table = new tableTemplate();
	_loss_table->setColumnCount(9);
	QStringList header;
	header << "Z0实部" << "Z0虚部" << "回波损失类型" << "优化方式" << "误差实部" << "误差虚部" << "值实部" << "值虚部" << "权值";
	_loss_table->setHorizontalHeaderLabels(header);
	_loss_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//_loss_table->setShowGrid(false);                               //setting no grid line

	_loss_table->resizeColumnToContents(6);
	_loss_table->resizeColumnToContents(7);
	initDefaultData();
	initLayout();
}


void lossTemplate::initDefaultData() {
	QJsonObject loss_obj = parseJson::getSubJsonObj(*_obj, "VSWRSetting");
	if (loss_obj.isEmpty()) {
		qCritical("get 'VSWRSetting' json object field.");
		QMessageBox::critical(0, QString("警告"), QString("读取问题配置文件失败！"));
		return;
	}
	QSignalMapper* loss_signals_map = new QSignalMapper;
	QStringList R0_real_lists, R0_image_lists, return_loss_type_lists, optimal_type_lists, delta_real_lists, delta_image_lists,
		vswr_obj_lists, s11_obj_lists, R1_real_lists, R1_image_lists,weight_lists;

	R0_real_lists = dataPool::strlist2list(loss_obj.value("R0_real").toString());
	R0_image_lists = dataPool::strlist2list(loss_obj.value("R0_imag").toString());
	return_loss_type_lists = dataPool::strlist2list(loss_obj.value("ReturnLossType").toString());
	optimal_type_lists = dataPool::strlist2list(loss_obj.value("optimaltype_vswr").toString());
	delta_real_lists = dataPool::strlist2list(loss_obj.value("delta_real").toString());
	delta_image_lists = dataPool::strlist2list(loss_obj.value("delta_imag").toString());
	vswr_obj_lists = dataPool::strlist2list(loss_obj.value("vswrobj").toString());
	s11_obj_lists = dataPool::strlist2list(loss_obj.value("S11").toString());
	R1_real_lists = dataPool::strlist2list(loss_obj.value("R1_real").toString());
	R1_image_lists = dataPool::strlist2list(loss_obj.value("R1_imag").toString());
	weight_lists = dataPool::strlist2list(loss_obj.value("weight_vswr").toString());

	QStringList strListR0Real, strListR0imag, strListReturnLossType, strListOptimaltype, strListDeltaReal, strListDeltaImag,
		strListVswrobj, strListS11, strListR1Real, strListR1Imag, strListWeight; 
	if (_index < R0_real_lists.size())
		strListR0Real = dataPool::str2list(R0_real_lists[_index]);
	if (_index < R0_image_lists.size())
		strListR0imag = dataPool::str2list(R0_image_lists[_index]);
	if (_index < return_loss_type_lists.size())
		strListReturnLossType = dataPool::str2list(return_loss_type_lists[_index]);
	if (_index < optimal_type_lists.size())
		strListOptimaltype = dataPool::str2list(optimal_type_lists[_index]);
	if (_index < delta_real_lists.size())
		strListDeltaReal = dataPool::str2list(delta_real_lists[_index]);
	if (_index < delta_image_lists.size())
		strListDeltaImag = dataPool::str2list(delta_image_lists[_index]);
	if (_index < vswr_obj_lists.size())
		strListVswrobj = dataPool::str2list(vswr_obj_lists[_index]);
	if (_index < s11_obj_lists.size())
		strListS11 = dataPool::str2list(s11_obj_lists[_index]);
	if (_index < R1_real_lists.size())
		strListR1Real = dataPool::str2list(R1_real_lists[_index]);
	if (_index < R1_image_lists.size())
		strListR1Imag = dataPool::str2list(R1_image_lists[_index]);
	if (_index < weight_lists.size())
		strListWeight = dataPool::str2list(weight_lists[_index]);

	_loss_table->setRowCount(strListR0Real.length());
	QList<int> line_edit_columns{ cz0real, cz0imag, cdeltareal, cdeltaimag, cobjreal, cobjimag, clossweight };
	_loss_table->setItemDelegate(new lineEditDelegate(line_edit_columns));

	for (int i = 0; i < strListR0Real.length(); i++) {
		_loss_table->insert2table(i, cz0real, strListR0Real[i]);
		_loss_table->insert2table(i, cz0imag, strListR0imag[i]);

		QComboBox* loss_type = new QComboBox;
		initLossTypeComBox(loss_type);
		if (loss_type->findText(strListReturnLossType[i]) != -1);
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

		_loss_table->insert2table(i, cdeltareal, strListDeltaReal[i]);
		_loss_table->insert2table(i, cdeltaimag, strListDeltaImag[i]);
		//setting cannot edit when optimize type is delta
		if (odelta != optimal_type->currentIndex()) {
			//不可编辑
			_loss_table->setCannotEdit(_loss_table->item(i, cdeltareal));
			_loss_table->setCannotEdit(_loss_table->item(i, cdeltaimag));
		}

		_loss_table->insert2table(i, cobjimag, strListR1Imag[i]);
		QTableWidgetItem* item_objimage = _loss_table->item(i, cobjimag);
		//image 默认不可编辑
		_loss_table->setCannotEdit(item_objimage);
		if (0 == loss_type->currentIndex()) {
			//loss type is vswr
			_loss_table->insert2table(i, cobjreal, strListVswrobj[i]);			
		}
		else if (1 == loss_type->currentIndex()) {
			//loss type is S11
			_loss_table->insert2table(i, cobjreal, strListS11[i]);
		}
		else if (2 == loss_type->currentIndex()) {
			//loss type is R
			_loss_table->insert2table(i, cobjreal, strListR1Real[i]);
			//image可编辑
			_loss_table->setCanEdit(item_objimage);
		}
		else {
			//loss type is None
			_loss_table->insert2table(i, cobjreal, "None");
			_loss_table->setCannotEdit(_loss_table->item(i, cobjreal));
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

bool lossTemplate::checkInputValid() {
	for (int i = 0; i < _loss_table->rowCount(); i++) {
		QComboBox *lossType = qobject_cast<QComboBox *>(_loss_table->cellWidget(i, closstype));
		QComboBox *loType = qobject_cast<QComboBox *>(_loss_table->cellWidget(i, clossoptimaltype));
		QString z0_real_value = _loss_table->item(i, cz0real)->text().trimmed();
		QString z0_image_value = _loss_table->item(i, cz0imag)->text().trimmed();
		QString delta_real_value = _loss_table->item(i, cdeltareal)->text().trimmed();
		QString delta_image_value = _loss_table->item(i, cdeltaimag)->text().trimmed();
		QString obj_real_value = _loss_table->item(i, cobjreal)->text().trimmed();
		QString obj_image_value = _loss_table->item(i, cobjimag)->text().trimmed();
		QString weight_value = _loss_table->item(i, clossweight)->text().trimmed();


		if (z0_real_value.isEmpty() || z0_real_value.isNull()) {
			qCritical("z0 real value is null.");
			checkInfo->code = eNull;
			checkInfo->message = "请设置阻抗实部";
			_loss_table->item(i, cz0real)->setSelected(true);
			emit signal_checkValid();
			return false;
		}
		if (z0_image_value.isEmpty() || z0_image_value.isNull()) {
			qCritical("z0 image value is null.");
			checkInfo->code = eNull;
			checkInfo->message = "请设置阻抗虚部";
			_loss_table->item(i, cz0imag)->setSelected(true);
			emit signal_checkValid();
			return false;
		}
		if (loType->currentIndex() == odelta) {
			if (delta_real_value.isEmpty() || delta_real_value.isNull() || delta_real_value == "None") {
				qCritical("delta real value is invalid.");
				checkInfo->code = eNull;
				checkInfo->message = "请设置delta实部";
				_loss_table->item(i, cdeltareal)->setSelected(true);
				emit signal_checkValid();
				return false;
			}
			if (delta_image_value.isEmpty() || delta_image_value.isNull() || delta_image_value == "None") {
				qCritical("delta image value is invalid.");
				checkInfo->code = eNull;
				checkInfo->message = "请设置delta虚部";
				_loss_table->item(i, cdeltaimag)->setSelected(true);
				emit signal_checkValid();
				return false;
			}
		}
		if (obj_real_value.isEmpty() || obj_real_value.isNull()) {
			qCritical("loss object value is invalid.");
			checkInfo->code = eNull;
			checkInfo->message = "请设置回波损失的目标值";
			_loss_table->item(i, cobjreal)->setSelected(true);
			emit signal_checkValid();
			return false;
		}
		if (lossType->currentIndex() == 2) {
			//R
			if (obj_image_value.isEmpty() || obj_image_value.isNull() || obj_image_value == "None") {
				qCritical("R image value is invalid.");
				checkInfo->code = eNull;
				checkInfo->message = "请设置回波损失的目标值虚部";
				_loss_table->item(i, cobjimag)->setSelected(true);
				emit signal_checkValid();
				return false;
			}
		}
		if (weight_value.isEmpty() || weight_value.isNull()) {
			qCritical("weight value is invalid.");
			checkInfo->code = eNull;
			checkInfo->message = "请设置回波损失的权值";
			_loss_table->item(i, clossweight)->setSelected(true);
			emit signal_checkValid();
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
		lossStr[0] << _loss_table->item(i, cz0real)->text().trimmed();
		lossStr[1] << _loss_table->item(i, cz0imag)->text().trimmed();

		QComboBox *lossType = qobject_cast<QComboBox *>(_loss_table->cellWidget(i, closstype));
		lossStr[2] << QString::number(lossType->currentIndex());
		QComboBox *loType = qobject_cast<QComboBox *>(_loss_table->cellWidget(i, clossoptimaltype));
		if (3 == loType->currentIndex())
			lossStr[3] << loType->currentText().trimmed();
		else
			lossStr[3] << QString("'%1'").arg(loType->currentText().trimmed());

		QString delta_real = _loss_table->item(i, cdeltareal)->text().trimmed();
		QString delta_image = _loss_table->item(i, cdeltaimag)->text().trimmed();
		if (odelta != loType->currentIndex()) {
			delta_real = "None";
			delta_image = "None";
		}
		lossStr[4] << delta_real;
		lossStr[5] << delta_image;

		QString obj_real = _loss_table->item(i, cobjreal)->text().trimmed();
		QString obj_image = _loss_table->item(i, cobjimag)->text().trimmed();
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
	QComboBox *selectCombox = qobject_cast<QComboBox *>(_loss_table->cellWidget(row, col));

	QLineEdit* delta_real_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(row, cdeltareal));
	QLineEdit* delta_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(row, cdeltaimag));
	QLineEdit* obj_image_edit = qobject_cast<QLineEdit *>(_loss_table->cellWidget(row, cobjimag));
	//setting edit when optimize type is delta
	if (col == clossoptimaltype) {
		QTableWidgetItem* delta_real_item = _loss_table->item(row, cdeltareal);
		QTableWidgetItem* delta_image_item = _loss_table->item(row, cdeltaimag);
		if (odelta == selectCombox->currentIndex()) {
			//可编辑
			_loss_table->setCanEdit(delta_real_item);
			_loss_table->setCanEdit(delta_image_item);
		}
		else {
			//不可编辑
			_loss_table->setCannotEdit(delta_real_item);
			_loss_table->setCannotEdit(delta_image_item);
		}
	}
	if (col == closstype) {
		if (2 == selectCombox->currentIndex()) {
			//可编辑
			_loss_table->setCanEdit(_loss_table->item(row, cobjimag));
		}
		else {
			_loss_table->setCannotEdit(_loss_table->item(row, cobjimag));
		}
	}
}


lossTemplate::~lossTemplate() {}