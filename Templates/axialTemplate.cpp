#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "gainAXialDelegateTemplate.h"
#include "axialTemplate.h"

axialTemplate::axialTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj), _theta_start(-180), _theta_end(180), _theta_step(5),
_phi_start(-180), _phi_end(180), _phi_step(5), _index(index) {
	_axial_table = new tableTemplate();
	_axial_table->setColumnCount(8);
	QStringList header;
	header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "轴比(dB)" << "权值";
	_axial_table->setHorizontalHeaderLabels(header);
	_axial_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//_axial_table->setShowGrid(false);                               //setting no grid line
	_add_button = new QPushButton("增加");
	_del_button = new QPushButton("删除");
	_axial_table->setEnabled(false);
	_add_button->setEnabled(false);
	_del_button->setEnabled(false);

	initDefaultData();
	initLayout();
	connect(_axial_table, SIGNAL(itemSelectionChanged()), this, SLOT(slot_selectChanged()));
	connect(_add_button, SIGNAL(clicked(bool)), this, SLOT(slot_addSetting(bool)));
	connect(_del_button, SIGNAL(clicked(bool)), this, SLOT(slot_delSetting(bool)));
}

void axialTemplate::initDefaultData() {
	QJsonObject axial_obj = parseJson::getSubJsonObj(*_obj, "AxialratioSetting");
	if (axial_obj.isEmpty()) {
		qCritical("get 'AxialratioSetting' json object field.");
		QMessageBox::critical(0, QString("警告"), QString("读取问题配置文件失败！"));
		return;
	}
	QSignalMapper* axial_signals_map = new QSignalMapper;
	QStringList theta_lower_lists, theta_upper_lists, phi_lower_lists, phi_upper_lists, optimal_type_lists, delta_lists, axial_lists, weight_lists;
	theta_lower_lists = dataPool::strlist2list(axial_obj.value("Theta_Lower_axial").toString());
	theta_upper_lists = dataPool::strlist2list(axial_obj.value("Theta_Upper_axial").toString());
	phi_lower_lists = dataPool::strlist2list(axial_obj.value("Phi_Lower_axial").toString());
	phi_upper_lists = dataPool::strlist2list(axial_obj.value("Phi_Upper_axial").toString());
	optimal_type_lists = dataPool::strlist2list(axial_obj.value("optimaltype_axial").toString());
	delta_lists = dataPool::strlist2list(axial_obj.value("delta_axial").toString());
	axial_lists = dataPool::strlist2list(axial_obj.value("axialobj").toString());
	weight_lists = dataPool::strlist2list(axial_obj.value("weight_axial").toString());

	QStringList str_list_theta_lower, str_list_theta_upper, str_list_phi_lower, str_list_phi_upper, str_list_optimal_type,
		str_list_delta, str_list_axial, str_list_weight;
	if (_index < theta_lower_lists.size())
		str_list_theta_lower = dataPool::str2list(theta_lower_lists[_index]);
	if (_index < theta_upper_lists.size())
		str_list_theta_upper = dataPool::str2list(theta_upper_lists[_index]);
	if (_index < phi_lower_lists.size())
		str_list_phi_lower = dataPool::str2list(phi_lower_lists[_index]);
	if (_index < phi_upper_lists.size())
		str_list_phi_upper = dataPool::str2list(phi_upper_lists[_index]);
	if (_index < optimal_type_lists.size())
		str_list_optimal_type = dataPool::str2list(optimal_type_lists[_index]);
	if (_index < delta_lists.size())
		str_list_delta = dataPool::str2list(delta_lists[_index]);
	if (_index < axial_lists.size())
		str_list_axial = dataPool::str2list(axial_lists[_index]);
	if (_index < weight_lists.size())
		str_list_weight = dataPool::str2list(weight_lists[_index]);

	_axial_table->setRowCount(str_list_theta_lower.length());
	for (int i = 0; i < str_list_theta_lower.length(); i++) {
		_axial_table->insert2table(i, cthetalower, str_list_theta_lower[i]);
		_axial_table->insert2table(i, cthetaupper, str_list_theta_upper[i]);
		_axial_table->insert2table(i, cphilower, str_list_phi_lower[i]);
		_axial_table->insert2table(i, cphiupper, str_list_phi_upper[i]);

		QComboBox* optimal_type = new QComboBox;
		initOptimalTypeComBox(optimal_type);
		if (optimal_type->findText(str_list_optimal_type[i]) != -1);
			optimal_type->setCurrentText(str_list_optimal_type[i]);
		_axial_table->setCellWidget(i, coptimaltype, optimal_type);
		//map combobox signal
		connect(optimal_type, SIGNAL(currentIndexChanged(int)), axial_signals_map, SLOT(map()));
		axial_signals_map->setMapping(optimal_type, QString("%1-%2").arg(i).arg(coptimaltype));

		_axial_table->insert2table(i, cdelta, str_list_delta[i]);

		//setting cannot edit when optimize type is not delta		
		if (optimal_type->currentIndex() != odelta) {
			_axial_table->setCannotEdit(_axial_table->item(i, cdelta));
		}

		_axial_table->insert2table(i, cobjvalue, str_list_axial[i]);
		_axial_table->insert2table(i, cweight, str_list_weight[i]);
	}
	connect(axial_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_changeOptimaltype(QString)));
}

void axialTemplate::initLayout() {
	QHBoxLayout* layout = new QHBoxLayout;
	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->addWidget(_add_button);
	vlayout->addWidget(_del_button);

	layout->addWidget(_axial_table);
	layout->addLayout(vlayout);
	_layout = layout;
}

QLayout* axialTemplate::getLayout() {
	return _layout;
}

bool axialTemplate::checkInputValid() {
	_axial_table->clearSelection();
	for (int i = 0; i < _axial_table->rowCount(); ++i) {
		_axial_table->item(i, cthetalower)->setBackgroundColor(QColor(255, 255, 255));
		_axial_table->item(i, cthetaupper)->setBackgroundColor(QColor(255, 255, 255));
		_axial_table->item(i, cphilower)->setBackgroundColor(QColor(255, 255, 255));
		_axial_table->item(i, cphiupper)->setBackgroundColor(QColor(255, 255, 255));
		_axial_table->item(i, cdelta)->setBackgroundColor(QColor(255, 255, 255));
		_axial_table->item(i, cobjvalue)->setBackgroundColor(QColor(255, 255, 255));
		_axial_table->item(i, cweight)->setBackgroundColor(QColor(255, 255, 255));
	}
	//_axial_table->setStyleSheet("background-color: white;");
	for (int i = 0; i < _axial_table->rowCount(); i++) {
		QString theta_low_value = _axial_table->item(i, cthetalower)->text().trimmed();
		QString theta_up_value = _axial_table->item(i, cthetaupper)->text().trimmed();
		QString phi_low_value = _axial_table->item(i, cphilower)->text().trimmed();
		QString phi_up_value = _axial_table->item(i, cphiupper)->text().trimmed();
		QComboBox *goType = qobject_cast<QComboBox *>(_axial_table->cellWidget(i, coptimaltype));
		QString delta_value = _axial_table->item(i, cdelta)->text().trimmed();
		QString obj_value = _axial_table->item(i, cobjvalue)->text().trimmed();
		QString weight_value = _axial_table->item(i, cweight)->text().trimmed();

		if (theta_low_value.toDouble() + 0.000001 < _theta_start) {
			qCritical("too small theta low value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "起始theta值过小。";
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cthetalower));
			emit signal_checkValid();
			return false;
		}
		if (theta_up_value.toDouble() - 0.000001 > _theta_end) {
			qCritical("too large theta up value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "终止theta值过大。";
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cthetaupper));
			emit signal_checkValid();
			return false;
		}
		if (theta_up_value.toDouble() < theta_low_value.toDouble()) {
			qCritical("invalid range of theta.");
			checkInfo->code = eInvalid;
			checkInfo->message = "theta范围设置有误。";
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cthetalower));
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cthetaupper));
			emit signal_checkValid();
			return false;
		}
		if (phi_low_value.toDouble() + 0.000001 < _phi_start) {
			qCritical("too small phi low value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "起始phi值过小。";
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cphilower));
			emit signal_checkValid();
			return false;
		}
		if (phi_up_value.toDouble() - 0.000001 > _phi_end) {
			qCritical("too large phi up value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "终止phi值过大。";
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cphiupper));
			emit signal_checkValid();
			return false;
		}
		if (phi_up_value.toDouble() < phi_low_value.toDouble()) {
			qCritical("invalid range of phi.");
			checkInfo->code = eInvalid;
			checkInfo->message = "phi范围设置有误。";
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cphilower));
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cphiupper));
			emit signal_checkValid();
			return false;
		}

		if (goType->currentIndex() == odelta && (delta_value.isEmpty() || delta_value.isNull() || delta_value == "None")) {
			qCritical("invalid delat value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "请设置delta误差值。";
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cdelta));
			emit signal_checkValid();
			return false;
		}
		if (obj_value.isEmpty() || obj_value.isNull()) {
			qCritical("invalid gain object value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "请设置轴比的目标值。";
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cobjvalue));
			emit signal_checkValid();
			return false;
		}
		if (weight_value.isEmpty() || weight_value.isNull()) {
			qCritical("invalid weight value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "请设置轴比的权值。";
			_axial_table->setWarningBackGroundColor(_axial_table->item(i, cweight));
			emit signal_checkValid();
			return false;
		}
	}
	if (!_axial_table->checkRectangleCross(_theta_step, _phi_step)) {
		qCritical("invalid setting the range of theta and phi.");
		checkInfo->code = eInvalid;
		checkInfo->message = "theta、phi范围设置有交叉，请修正。";
		emit signal_checkValid();
		return false;
	}
	return true;
}

//update json obj
void axialTemplate::updateJObj() {
	QJsonObject maxial_obj;
	//update axial obj
	QStringList axialStr[8];
	for (int i = 0; i < _axial_table->rowCount(); i++) {
		axialStr[0] << _axial_table->item(i, cthetalower)->text().trimmed();
		axialStr[1] << _axial_table->item(i, cthetaupper)->text().trimmed();
		axialStr[2] << _axial_table->item(i, cphilower)->text().trimmed();
		axialStr[3] << _axial_table->item(i, cphiupper)->text().trimmed();

		QComboBox *aoType = qobject_cast<QComboBox *>(_axial_table->cellWidget(i, coptimaltype));
		if (onone == aoType->currentIndex())
			axialStr[4] << aoType->currentText().trimmed();
		else
			axialStr[4] << QString("'%1'").arg(aoType->currentText().trimmed());

		QString delta_value = _axial_table->item(i, cdelta)->text().trimmed();
		if (aoType->currentIndex() != odelta)
			delta_value = "None";
		axialStr[5] << delta_value;
		axialStr[6] << _axial_table->item(i, cobjvalue)->text().trimmed();
		axialStr[7] << _axial_table->item(i, cweight)->text().trimmed();
	}
	maxial_obj.insert("Theta_Lower_axial", QString("[[%1]]").arg(axialStr[0].join(",")));
	maxial_obj.insert("Theta_Upper_axial", QString("[[%1]]").arg(axialStr[1].join(",")));
	maxial_obj.insert("Phi_Lower_axial", QString("[[%1]]").arg(axialStr[2].join(",")));
	maxial_obj.insert("Phi_Upper_axial", QString("[[%1]]").arg(axialStr[3].join(",")));
	maxial_obj.insert("optimaltype_axial", QString("[[%1]]").arg(axialStr[4].join(",")));
	maxial_obj.insert("delta_axial", QString("[[%1]]").arg(axialStr[5].join(",")));
	maxial_obj.insert("axialobj", QString("[[%1]]").arg(axialStr[6].join(",")));
	maxial_obj.insert("weight_axial", QString("[[%1]]").arg(axialStr[7].join(",")));
	_obj->insert("AxialratioSetting", maxial_obj);
}

void axialTemplate::slot_changeOptimaltype(QString pos) {
	QStringList coordinates = pos.split("-");
	int row = coordinates[0].toInt();
	int col = coordinates[1].toInt();
	QComboBox* selectCombox = qobject_cast<QComboBox *>(_axial_table->cellWidget(row, col));
	//setting edit when optimize type is delta
	if (selectCombox->currentIndex() == odelta) {
		//可编辑
		_axial_table->setCanEdit(_axial_table->item(row, cdelta));
	}
	else
		//不可编辑
		_axial_table->setCannotEdit(_axial_table->item(row, cdelta));
}

void axialTemplate::slot_confirmFarField(QString info) {
	if (info.isEmpty() || info.isNull()) {
		_axial_table->setEnabled(false);
		_add_button->setEnabled(false);
	}
	else {
		QStringList parameters = info.split("#");
		_theta_start = parameters[0].toDouble();
		_theta_end = parameters[1].toDouble();
		_theta_step = parameters[2].toDouble();
		_phi_start = parameters[3].toDouble();
		_phi_end = parameters[4].toDouble();
		_phi_step = parameters[5].toDouble();

		QList<int> combobox_columns{ cthetalower, cthetaupper, cphilower, cphiupper };
		QList<int> edit_columns{ cdelta, cobjvalue, cweight };
		QList<double> lows{ _theta_start , _theta_start , _phi_start ,_phi_start };
		QList<double> ups{ _theta_end , _theta_end , _phi_end ,_phi_end };
		QList<double> steps{ _theta_step , _theta_step , _phi_step ,_phi_step };
		_axial_table->setItemDelegate(new gainAxialDelegate(combobox_columns, edit_columns, lows, ups, steps));

		_axial_table->setEnabled(true);
		_add_button->setEnabled(true);
	}
}

void axialTemplate::slot_selectChanged() {
	int select_number = _axial_table->selectedItems().size();
	if (select_number == 0) {
		_del_button->setEnabled(false);
		_axial_table->clearSelection();
	}
	else {
		_del_button->setEnabled(true);
	}
}

void axialTemplate::slot_addSetting(bool) {
	int new_row_index = _axial_table->rowCount();
	_axial_table->insertRow(new_row_index);
	_axial_table->insert2table(new_row_index, cthetalower, QString::number(_theta_start));
	_axial_table->insert2table(new_row_index, cthetaupper, QString::number(_theta_start));
	_axial_table->insert2table(new_row_index, cphilower, QString::number(_phi_start));
	_axial_table->insert2table(new_row_index, cphiupper, QString::number(_phi_start));

	QComboBox* optimal_type = new QComboBox;
	initOptimalTypeComBox(optimal_type);
	_axial_table->setCellWidget(new_row_index, coptimaltype, optimal_type);
	//map combobox signal
	QSignalMapper* gain_signals_map = new QSignalMapper;
	connect(optimal_type, SIGNAL(currentIndexChanged(int)), gain_signals_map, SLOT(map()));
	gain_signals_map->setMapping(optimal_type, QString("%1-%2").arg(new_row_index).arg(coptimaltype));

	_axial_table->insert2table(new_row_index, cdelta, "None");
	_axial_table->setCannotEdit(_axial_table->item(new_row_index, cdelta));
	_axial_table->insert2table(new_row_index, cobjvalue, "0.0");
	_axial_table->insert2table(new_row_index, cweight, "1.0");

	connect(gain_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_ChangeOptimaltype(QString)));
}

void axialTemplate::slot_delSetting(bool) {
	QItemSelectionModel *selectionModel = _axial_table->selectionModel();
	QVector<int> del_indexs;
	foreach(QModelIndex index, selectionModel->selectedRows())
		del_indexs.append(index.row());

	//删除选中的所有行，注意从后往前删，否则选中行号会更新导致删除非选中行
	QMessageBox::StandardButton rb = QMessageBox::question(NULL, "删除", "删除选中行?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (rb == QMessageBox::Yes) {
		for (QVector<int>::reverse_iterator iter = del_indexs.rbegin(); iter != del_indexs.rend(); ++iter) {
			if (*iter != -1)
				_axial_table->removeRow(*iter);
		}
		_axial_table->clearSelection();
	}
}

axialTemplate::~axialTemplate() {}