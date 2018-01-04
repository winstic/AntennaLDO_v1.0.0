#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "comboboxDelegateTemplate.h"
#include "gainTemplate.h"

gainTemplate::gainTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj), _theta_start(-180), _theta_end(180), _theta_step(5), 
_phi_start(-180), _phi_end(180), _phi_step(5), _is_valid(true), _index(index) {
	_gain_table = new tableTemplate();
	_gain_table->setColumnCount(8);
	QStringList header;
	header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "增益(dB)" << "权值";
	_gain_table->setHorizontalHeaderLabels(header);
	_gain_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_gain_table->setShowGrid(false);                               //setting no grid line
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
		checkInfo->code = eOther;
		checkInfo->message = "问题json文件格式不正确。";
		_is_valid = false;
		emit signal_checkValid();
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
	_gain_table->setItemDelegate(new comboboxDelegate(0));
	for (int i = 0; i < str_list_theta_lower.length(); i++) {
		QComboBox* theta_low_comb = new QComboBox;
		QComboBox* theta_up_comb = new QComboBox;
		QComboBox* phi_low_comb = new QComboBox;
		QComboBox* phi_up_comb = new QComboBox;
		theta_low_comb->addItem(str_list_theta_lower[i]);
		theta_up_comb->addItem(str_list_theta_upper[i]);
		phi_low_comb->addItem(str_list_phi_lower[i]);
		phi_up_comb->addItem(str_list_phi_upper[i]);

		_gain_table->setCellWidget(i, cthetalower, theta_low_comb);
		_gain_table->setCellWidget(i, cthetaupper, theta_up_comb);
		_gain_table->setCellWidget(i, cphilower, phi_low_comb);
		_gain_table->setCellWidget(i, cphiupper, phi_up_comb);

		QComboBox* optimal_type = new QComboBox;
		initOptimalTypeComBox(optimal_type);
		if (optimal_type->findText(str_list_optimal_type[i]) != -1);
			optimal_type->setCurrentText(str_list_optimal_type[i]);
		_gain_table->setCellWidget(i, coptimaltype, optimal_type);
		//map combobox signal
		connect(optimal_type, SIGNAL(currentIndexChanged(int)), gain_signals_map, SLOT(map()));
		gain_signals_map->setMapping(optimal_type, QString("%1-%2").arg(i).arg(coptimaltype));

		QRegExpValidator* floatValidReg = getFloatReg();    //float
		QLineEdit* delta_value_edit = new QLineEdit;
		delta_value_edit->setValidator(floatValidReg);
		//setting cannot edit when optimize type is not delta		
		if (optimal_type->currentIndex() != odelta) {
			delta_value_edit->setText("----");
			delta_value_edit->setEnabled(false);
		}
		else
			delta_value_edit->setText(str_list_delta[i]);		
		_gain_table->setCellWidget(i, cdelta, delta_value_edit);		

		QLineEdit* obj_value_edit = new QLineEdit;
		QLineEdit* weight_value_edit = new QLineEdit;
		obj_value_edit->setValidator(floatValidReg);
		weight_value_edit->setValidator(floatValidReg);
		obj_value_edit->setText(str_list_gain[i]);
		weight_value_edit->setText(str_list_weight[i]);
		_gain_table->setCellWidget(i, cobjvalue, obj_value_edit);
		_gain_table->setCellWidget(i, cweight, weight_value_edit);
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
	if (!_is_valid) return false;
	for (int i = 0; i < _gain_table->rowCount(); i++) {
		QComboBox *goType = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, coptimaltype));
		QLineEdit* delta_edit = qobject_cast<QLineEdit *>(_gain_table->cellWidget(i, cdelta));
		QLineEdit* obj_edit = qobject_cast<QLineEdit *>(_gain_table->cellWidget(i, cobjvalue));
		QLineEdit* weight_edit = qobject_cast<QLineEdit *>(_gain_table->cellWidget(i, cweight));
		QString delta_value = delta_edit->text().trimmed();
		QString obj_value = obj_edit->text().trimmed();
		QString weight_value = weight_edit->text().trimmed();

		if (goType->currentIndex() == odelta && (delta_value.isEmpty() || delta_value.isNull() || delta_value == "----")) {
			qCritical("invalid delat value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "请设置有效的delta误差值。";
			_is_valid = false;
			commonStyle::setLineEditWarningStyle(delta_edit);
			emit signal_checkValid();
			return false;
		}
		if (obj_value.isEmpty() || obj_value.isNull()) {
			qCritical("invalid gain object value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "请设置有效的增益目标值。";
			_is_valid = false;
			commonStyle::setLineEditWarningStyle(obj_edit);
			emit signal_checkValid();
			return false;
		}
		if (weight_value.isEmpty() || weight_value.isNull()) {
			qCritical("invalid weight value.");
			checkInfo->code = eInvalid;
			checkInfo->message = "请设置有效的权值。";
			_is_valid = false;
			commonStyle::setLineEditWarningStyle(weight_edit);
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
		QComboBox* theta_low_comb = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, cthetalower));
		QComboBox* theta_up_comb = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, cthetaupper));
		QComboBox* phi_low_comb = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, cphilower));
		QComboBox* phi_up_comb = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, cphiupper));
		gain_str[0] << theta_low_comb->currentText().trimmed();
		gain_str[1] << theta_up_comb->currentText().trimmed();
		gain_str[2] << phi_low_comb->currentText().trimmed();
		gain_str[3] << phi_up_comb->currentText().trimmed();

		QComboBox *goType = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, coptimaltype));
		if (3 == goType->currentIndex())
			gain_str[4] << goType->currentText().trimmed();
		else
			gain_str[4] << QString("'%1'").arg(goType->currentText().trimmed());

		QLineEdit* delta_edit = qobject_cast<QLineEdit *>(_gain_table->cellWidget(i, cdelta));
		QLineEdit* obj_edit = qobject_cast<QLineEdit *>(_gain_table->cellWidget(i, cobjvalue));
		QLineEdit* weight_edit = qobject_cast<QLineEdit *>(_gain_table->cellWidget(i, cweight));
		QString delta_value = delta_edit->text().trimmed();
		if (delta_value.isEmpty() || delta_value.isNull() || delta_value == "----")
			delta_value = "None";
		gain_str[5] << delta_value;
		gain_str[6] << obj_edit->text().trimmed();
		gain_str[7] << weight_edit->text().trimmed();
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
	QLineEdit* delta_edit = qobject_cast<QLineEdit *>(_gain_table->cellWidget(row, cdelta));
	//setting edit when optimize type is delta
	if (selectCombox->currentIndex() == odelta) {
		delta_edit->setEnabled(true);
	}
	else {
		delta_edit->setEnabled(false);
	}
}

void gainTemplate::slot_confirmFarField(QString info) {
	QStringList parameters = info.split("#");
	_theta_start = parameters[0].toDouble();
	_theta_end = parameters[1].toDouble();
	_theta_step = parameters[2].toDouble();
	_phi_start = parameters[3].toDouble();
	_phi_end = parameters[4].toDouble();
	_phi_step = parameters[5].toDouble();

	for (int i = 0; i < _gain_table->rowCount(); i++) {
		QComboBox* theta_low_comb = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, cthetalower));
		QComboBox* theta_up_comb = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, cthetaupper));
		QComboBox* phi_low_comb = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, cphilower));
		QComboBox* phi_up_comb = qobject_cast<QComboBox *>(_gain_table->cellWidget(i, cphiupper));
		QString theta_low = theta_low_comb->currentText().trimmed();
		QString theta_up = theta_up_comb->currentText().trimmed();
		QString phi_low = phi_low_comb->currentText().trimmed();
		QString phi_up = phi_up_comb->currentText().trimmed();

		theta_low_comb->clear();
		theta_up_comb->clear();
		phi_low_comb->clear();
		phi_up_comb->clear();
		initAngleComboBox(theta_low_comb, _theta_start, _theta_end, _theta_step);
		initAngleComboBox(theta_up_comb, _theta_start, _theta_end, _theta_step);
		initAngleComboBox(phi_low_comb, _phi_start, _phi_end, _phi_step);
		initAngleComboBox(phi_up_comb, _phi_start, _phi_end, _phi_step);

		if (theta_low_comb->findText(theta_low) == -1) {
			qCritical("theta low setting error.");
			checkInfo->code = eInvalid;
			checkInfo->message = "问题json文件中起始theta未按步长设置。";
			_is_valid = false;
			commonStyle::setComboBoxWarningStyle(theta_low_comb);
			emit signal_checkValid();
			return;
		}
		if (theta_up_comb->findText(theta_up) == -1) {
			qCritical("theta up setting error.");
			checkInfo->code = eInvalid;
			checkInfo->message = "问题json文件中终止theta未按步长设置。";
			_is_valid = false;
			commonStyle::setComboBoxWarningStyle(theta_up_comb);
			emit signal_checkValid();
			return;
		}
		if (phi_low_comb->findText(phi_low) == -1) {
			qCritical("phi low setting error.");
			checkInfo->code = eInvalid;
			checkInfo->message = "问题json文件中起始phi未按步长设置。";
			_is_valid = false;
			commonStyle::setComboBoxWarningStyle(phi_low_comb);
			emit signal_checkValid();
			return;
		}
		if (phi_up_comb->findText(phi_up) == -1) {
			qCritical("phi up setting error.");
			checkInfo->code = eInvalid;
			checkInfo->message = "问题json文件中终止phi未按步长设置。";
			_is_valid = false;
			commonStyle::setComboBoxWarningStyle(phi_up_comb);
			emit signal_checkValid();
			return;
		}
		theta_low_comb->setCurrentText(theta_low);
		theta_up_comb->setCurrentText(theta_up);
		phi_low_comb->setCurrentText(phi_low);
		phi_up_comb->setCurrentText(phi_up);
	}
	_gain_table->setEnabled(true);
	_add_button->setEnabled(true);
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
	QComboBox* theta_low_comb = new QComboBox;
	QComboBox* theta_up_comb = new QComboBox;
	QComboBox* phi_low_comb = new QComboBox;
	QComboBox* phi_up_comb = new QComboBox;
	initAngleComboBox(theta_low_comb, _theta_start, _theta_end, _theta_step);
	initAngleComboBox(theta_up_comb, _theta_start, _theta_end, _theta_step);
	initAngleComboBox(phi_low_comb, _phi_start, _phi_end, _phi_step);
	initAngleComboBox(phi_up_comb, _phi_start, _phi_end, _phi_step);
	_gain_table->setCellWidget(new_row_index, cthetalower, theta_low_comb);
	_gain_table->setCellWidget(new_row_index, cthetaupper, theta_up_comb);
	_gain_table->setCellWidget(new_row_index, cphilower, phi_low_comb);
	_gain_table->setCellWidget(new_row_index, cphiupper, phi_up_comb);

	QComboBox* optimal_type = new QComboBox;
	initOptimalTypeComBox(optimal_type);
	_gain_table->setCellWidget(new_row_index, coptimaltype, optimal_type);
	//map combobox signal
	QSignalMapper* gain_signals_map = new QSignalMapper;
	connect(optimal_type, SIGNAL(currentIndexChanged(int)), gain_signals_map, SLOT(map()));
	gain_signals_map->setMapping(optimal_type, QString("%1-%2").arg(new_row_index).arg(coptimaltype));

	
	QRegExpValidator* floatValidReg = getFloatReg();    //float
	QLineEdit* delta_value_edit = new QLineEdit;
	delta_value_edit->setValidator(floatValidReg);
	delta_value_edit->setText("----");
	_gain_table->setCellWidget(new_row_index, cdelta, delta_value_edit);

	QLineEdit* obj_value_edit = new QLineEdit;
	QLineEdit* weight_value_edit = new QLineEdit;
	obj_value_edit->setValidator(floatValidReg);
	weight_value_edit->setValidator(floatValidReg);
	obj_value_edit->setText("0.0");
	weight_value_edit->setText("1.0");
	_gain_table->setCellWidget(new_row_index, cobjvalue, obj_value_edit);
	_gain_table->setCellWidget(new_row_index, cweight, weight_value_edit);
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