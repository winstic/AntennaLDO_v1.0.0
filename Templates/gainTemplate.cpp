#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "gainAxialDelegateTemplate.h"
#include "lineEditDelegateTemplate.h"
#include "gainTemplate.h"

gainTemplate::gainTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj), _theta_start(-180), _theta_end(180), _theta_step(5), 
_phi_start(-180), _phi_end(180), _phi_step(5), _index(index) {
	_gain_table = new tableTemplate();
	_gain_table->setColumnCount(8);
	QStringList header;
	header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "增益(dB)" << "权值";
	_gain_table->setHorizontalHeaderLabels(header);
	_gain_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//_gain_table->setShowGrid(false);                               //setting no grid line
	_add_button = new QPushButton("增加");
	_del_button = new QPushButton("删除");
	_gain_table->setEnabled(false);
	_add_button->setEnabled(false);
	_del_button->setEnabled(false);

	initDefaultData();
	initLayout();
	connect(_gain_table, SIGNAL(itemSelectionChanged()), this, SLOT(slot_selectChanged()));
	connect(_add_button, SIGNAL(clicked(bool)), this, SLOT(slot_addSetting(bool)));
	connect(_del_button, SIGNAL(clicked(bool)), this, SLOT(slot_delSetting(bool)));
}

void gainTemplate::initDefaultData() {
	QJsonObject gain_obj = parseJson::getSubJsonObj(*_obj, "GainSetting");
	if (gain_obj.isEmpty()) {
		qCritical("get 'GainSetting' json object field.");
		QMessageBox::critical(0, QString("警告"), QString("读取问题配置文件失败！"));
		return;
	}
	QSignalMapper* gain_signals_map = new QSignalMapper;
	QStringList theta_lower_lists, theta_upper_lists, phi_lower_lists, phi_upper_lists, optimal_type_lists, delta_lists, gain_lists, weight_lists;
	theta_lower_lists = dataPool::strlist2list(gain_obj.value("Theta_Lower_gain").toString());
	theta_upper_lists = dataPool::strlist2list(gain_obj.value("Theta_Upper_gain").toString());
	phi_lower_lists = dataPool::strlist2list(gain_obj.value("Phi_Lower_gain").toString());
	phi_upper_lists = dataPool::strlist2list(gain_obj.value("Phi_Upper_gain").toString());
	optimal_type_lists = dataPool::strlist2list(gain_obj.value("optimaltype_gain").toString());
	delta_lists = dataPool::strlist2list(gain_obj.value("delta_gain").toString());
	gain_lists = dataPool::strlist2list(gain_obj.value("gainobj").toString());
	weight_lists = dataPool::strlist2list(gain_obj.value("weight_gain").toString());

	QStringList str_list_theta_lower, str_list_theta_upper, str_list_phi_lower, str_list_phi_upper, str_list_optimal_type,
		str_list_delta, str_list_gain, str_list_weight;

	if(_index < theta_lower_lists.size())
		str_list_theta_lower = dataPool::str2list(theta_lower_lists[_index]);
	if(_index < theta_upper_lists.size())
		str_list_theta_upper = dataPool::str2list(theta_upper_lists[_index]);
	if(_index < phi_lower_lists.size())
		str_list_phi_lower = dataPool::str2list(phi_lower_lists[_index]);
	if(_index < phi_upper_lists.size())
		str_list_phi_upper = dataPool::str2list(phi_upper_lists[_index]);
	if (_index < optimal_type_lists.size())
		str_list_optimal_type = dataPool::str2list(optimal_type_lists[_index]);
	if (_index < delta_lists.size())
		str_list_delta = dataPool::str2list(delta_lists[_index]);
	if (_index < gain_lists.size())
		str_list_gain = dataPool::str2list(gain_lists[_index]);
	if (_index < weight_lists.size())
		str_list_weight = dataPool::str2list(weight_lists[_index]);

	_gain_table->setRowCount(str_list_theta_lower.length());
	for (int i = 0; i < str_list_theta_lower.length(); i++) {
		_gain_table->insert2table(i, cthetalower, str_list_theta_lower[i]);
		_gain_table->insert2table(i, cthetaupper, str_list_theta_upper[i]);
		_gain_table->insert2table(i, cphilower, str_list_phi_lower[i]);
		_gain_table->insert2table(i, cphiupper, str_list_phi_upper[i]);

		QComboBox* optimal_type = new QComboBox;
		//optimal_type->setEditable(true);
		initOptimalTypeComBox(optimal_type);
		if (optimal_type->findText(str_list_optimal_type[i]) != -1);
			optimal_type->setCurrentText(str_list_optimal_type[i]);
		_gain_table->setCellWidget(i, coptimaltype, optimal_type);
		//map combobox signal
		connect(optimal_type, SIGNAL(currentIndexChanged(int)), gain_signals_map, SLOT(map()));
		gain_signals_map->setMapping(optimal_type, QString("%1-%2").arg(i).arg(coptimaltype));

		_gain_table->insert2table(i, cdelta, str_list_delta[i]);
		//setting cannot edit when optimize type is not delta		
		if (optimal_type->currentIndex() != odelta) {
			_gain_table->setCannotEdit(_gain_table->item(i, cdelta));
		}	
		
		_gain_table->insert2table(i, cobjvalue, str_list_gain[i]);
		_gain_table->insert2table(i, cweight, str_list_weight[i]);
	}
	connect(gain_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_ChangeOptimaltype(QString)));
}

void gainTemplate::initLayout() {
	QHBoxLayout* layout = new QHBoxLayout;
	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->addWidget(_add_button);
	vlayout->addWidget(_del_button);
	
	layout->addWidget(_gain_table);
	layout->addLayout(vlayout);
	_layout = layout;
}

QLayout* gainTemplate::getLayout() {
	return _layout;
}

bool gainTemplate::checkInputValid() {
	for (int i = 0; i < _gain_table->rowCount(); i++) {
		QString theta_low_value = _gain_table->item(i, cthetalower)->text().trimmed();
		QString theta_up_value = _gain_table->item(i, cthetaupper)->text().trimmed();
		QString phi_low_value = _gain_table->item(i, cphilower)->text().trimmed();
		QString phi_up_value = _gain_table->item(i, cphiupper)->text().trimmed();
		QComboBox *goType = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, coptimaltype));
		QString delta_value = _gain_table->item(i, cdelta)->text().trimmed();
		QString obj_value = _gain_table->item(i, cobjvalue)->text().trimmed();
		QString weight_value = _gain_table->item(i, cweight)->text().trimmed();

		if (theta_low_value.toDouble() + 0.000001 < _theta_start) {
			qCritical("too small theta low value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "起始theta值过小。";
			_gain_table->item(i, cthetalower)->setSelected(true);
			emit signal_checkValid();
			return false;
		}
		if (theta_up_value.toDouble() - 0.000001 > _theta_end) {
			qCritical("too large theta up value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "终止theta值过大。";
			_gain_table->item(i, cthetaupper)->setSelected(true);
			emit signal_checkValid();
			return false;
		}
		if (phi_low_value.toDouble() + 0.000001 < _phi_start) {
			qCritical("too small phi low value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "起始phi值过小。";
			_gain_table->item(i, cphilower)->setSelected(true);
			emit signal_checkValid();
			return false;
		}
		if (phi_up_value.toDouble() - 0.000001 > _phi_end) {
			qCritical("too large phi up value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "终止phi值过大。";
			_gain_table->item(i, cphiupper)->setSelected(true);
			emit signal_checkValid();
			return false;
		}

		if (goType->currentIndex() == odelta && (delta_value.isEmpty() || delta_value.isNull() || delta_value == "None")) {
			qCritical("invalid delat value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "请设置delta误差值。";
			_gain_table->item(i, cdelta)->setSelected(true);
			emit signal_checkValid();
			return false;
		}
		if (obj_value.isEmpty() || obj_value.isNull()) {
			qCritical("invalid gain object value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "请设置增益的目标值。";
			_gain_table->item(i, cobjvalue)->setSelected(true);
			emit signal_checkValid();
			return false;
		}
		if (weight_value.isEmpty() || weight_value.isNull()) {
			qCritical("invalid weight value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "请设置增益目标的权值。";
			_gain_table->item(i, cweight)->setSelected(true);
			emit signal_checkValid();
			return false;
		}
	}
	return true;
}

//update json obj
void gainTemplate::updateJObj() {
	QJsonObject mgain_obj;
	int i;
	//update gain obj
	QStringList gain_str[8];
	for (i = 0; i < _gain_table->rowCount(); i++) {
		gain_str[0] << _gain_table->item(i, cthetalower)->text().trimmed();
		gain_str[1] << _gain_table->item(i, cthetaupper)->text().trimmed();
		gain_str[2] << _gain_table->item(i, cphilower)->text().trimmed();
		gain_str[3] << _gain_table->item(i, cphiupper)->text().trimmed();

		QComboBox *goType = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, coptimaltype));
		if (3 == goType->currentIndex())
			gain_str[4] << goType->currentText().trimmed();
		else
			gain_str[4] << QString("'%1'").arg(goType->currentText().trimmed());

		QString delta_value = _gain_table->item(i, cdelta)->text().trimmed();
		if (delta_value.isEmpty() || delta_value.isNull())
			delta_value = "None";
		gain_str[5] << delta_value;
		gain_str[6] << _gain_table->item(i, cobjvalue)->text().trimmed();
		gain_str[7] << _gain_table->item(i, cweight)->text().trimmed();
	}
	mgain_obj.insert("Theta_Lower_gain", QString("[[%1]]").arg(gain_str[0].join(",")));
	mgain_obj.insert("Theta_Upper_gain", QString("[[%1]]").arg(gain_str[1].join(",")));
	mgain_obj.insert("Phi_Lower_gain", QString("[[%1]]").arg(gain_str[2].join(",")));
	mgain_obj.insert("Phi_Upper_gain", QString("[[%1]]").arg(gain_str[3].join(",")));
	mgain_obj.insert("optimaltype_gain", QString("[[%1]]").arg(gain_str[4].join(",")));
	mgain_obj.insert("delta_gain", QString("[[%1]]").arg(gain_str[5].join(",")));
	mgain_obj.insert("gainobj", QString("[[%1]]").arg(gain_str[6].join(",")));
	mgain_obj.insert("weight_gain", QString("[[%1]]").arg(gain_str[7].join(",")));
	_obj->insert("GainSetting", mgain_obj);
}

void gainTemplate::slot_ChangeOptimaltype(QString pos) {
	QStringList coordinates = pos.split("-");
	int row = coordinates[0].toInt();
	int col = coordinates[1].toInt();
	QComboBox* selectCombox = qobject_cast<QComboBox *>(_gain_table->cellWidget(row, col));
	//setting edit when optimize type is delta
	QTableWidgetItem* item = _gain_table->item(row, cdelta);
	if (selectCombox->currentIndex() == odelta) {
		//可编辑	
		_gain_table->setCanEdit(item);
	}
	else {
		//不可编辑		
		_gain_table->setCannotEdit(item);
	}
}

void gainTemplate::slot_confirmFarField(QString info) {
	if (info.isEmpty() || info.isNull()) {
		_gain_table->setEnabled(false);
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
		_gain_table->setItemDelegate(new gainAxialDelegate(combobox_columns, edit_columns, lows, ups, steps));

		_gain_table->setEnabled(true);
		_add_button->setEnabled(true);
	}
}

void gainTemplate::slot_selectChanged() {
	int select_number = _gain_table->selectedItems().size();
	if (select_number == 0) {
		_del_button->setEnabled(false);
		_gain_table->clearSelection();
	}
	else {
		_del_button->setEnabled(true);
	}
}

void gainTemplate::slot_addSetting(bool) {
	int new_row_index = _gain_table->rowCount();
	_gain_table->insertRow(new_row_index);

	_gain_table->insert2table(new_row_index, cthetalower, QString::number(_theta_start));
	_gain_table->insert2table(new_row_index, cthetaupper, QString::number(_theta_start));
	_gain_table->insert2table(new_row_index, cphilower, QString::number(_phi_start));
	_gain_table->insert2table(new_row_index, cphiupper, QString::number(_phi_start));

	QComboBox* optimal_type = new QComboBox;
	//optimal_type->setEditable(true);
	initOptimalTypeComBox(optimal_type);
	_gain_table->setCellWidget(new_row_index, coptimaltype, optimal_type);
	//map combobox signal
	QSignalMapper* gain_signals_map = new QSignalMapper;
	connect(optimal_type, SIGNAL(currentIndexChanged(int)), gain_signals_map, SLOT(map()));
	gain_signals_map->setMapping(optimal_type, QString("%1-%2").arg(new_row_index).arg(coptimaltype));

	
	_gain_table->insert2table(new_row_index, cdelta, "None");
	_gain_table->setCannotEdit(_gain_table->item(new_row_index, cdelta));
	_gain_table->insert2table(new_row_index, cobjvalue, "0.0");
	_gain_table->insert2table(new_row_index, cweight, "1.0");

	connect(gain_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_ChangeOptimaltype(QString)));
}

void gainTemplate::slot_delSetting(bool) {
	QMessageBox::StandardButton rb = QMessageBox::question(NULL, "删除", "删除选中行?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (rb == QMessageBox::Yes){
		int select_row_index = _gain_table->currentRow();
		if (select_row_index != -1)
			_gain_table->removeRow(select_row_index);
		_gain_table->clearSelection();
	}
}

gainTemplate::~gainTemplate() {}