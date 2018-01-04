#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "comboboxDelegateTemplate.h"
#include "variablesTemplate.h"

variablesTemplate::variablesTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent)
	: iTemplate(parent), _atn_problem(atn_problem), _obj(obj), _is_valid(true) {
	_atn_image_label = new QLabel(this);
	_vars_table = new tableTemplate();
	_vars_table->setColumnCount(4);
	QStringList header;
	header << "变量" << "最小值" << "最大值" << "单位";
	_vars_table->setHorizontalHeaderLabels(header);
	_vars_table->horizontalHeader()->setSectionResizeMode(varmin, QHeaderView::Stretch);
	_vars_table->horizontalHeader()->setSectionResizeMode(varmax, QHeaderView::Stretch);
	_vars_table->horizontalHeader()->setSectionResizeMode(varunit, QHeaderView::ResizeToContents);
	_vars_table->horizontalHeader()->resizeSection(varnote, 120);        //setting first column width is 120
	_vars_table->verticalHeader()->setDefaultSectionSize(40); //设置行高
	_vars_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	_vars_table->setShowGrid(false);                               //setting no grid line
	_vars_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit

	initDefaultData();
	initLayout();
}

void variablesTemplate::initDefaultData() {
	QJsonObject variables_obj = parseJson::getSubJsonObj(*_obj, "variables");
	if (variables_obj.isEmpty()) {
		qCritical("get 'variables' json object field.");
		checkInfo->code = eOther;
		checkInfo->message = "问题json文件格式不正确。";
		_is_valid = false;
		emit signal_checkValid();
		return;
	}
	QJsonObject var_obj;
	QString var_key;
	QStringList var_value;
	int row_number = 0, value_list_length;
	QSignalMapper* signals_map_unit = new QSignalMapper;	//use signalmaper manage signals in table

	_vars_table->setRowCount(variables_obj.count());
	//_vars_table->setItemDelegate(new comboboxDelegate(0));
	for (QJsonObject::iterator iter = variables_obj.begin(); iter != variables_obj.end(); ++iter) {
		// iter format: "W1":{"note" : "介质板宽度W1(m)", "W1" : "0.025"}
		var_key = iter.key();
		var_obj = iter.value().toObject();   //like {"note": "上贴片坐标y1(m)", "y1": "[-0.0115,0]"}
											 //get note infomation
		QString key_note = var_obj.value("note").toString().trimmed();
		_vars_table->insert2table(row_number, varnote, key_note);
		_vars_table->item(row_number, varnote)->setWhatsThis(var_key);

		var_value = dataPool::str2list(var_obj.value(var_key).toString().trimmed());
		value_list_length = var_value.length();
		QLineEdit *low_edit = new QLineEdit;
		QLineEdit *up_edit = new QLineEdit;
		low_edit->setValidator(getFloatReg());
		up_edit->setValidator(getFloatReg());

		low_edit->setText(var_value[0]);
		//_vars_table->insert2table(row_number, varmin, var_value[0]);
		if (value_list_length == 1) {
			up_edit->setText(var_value[0]);
		}
		else if (value_list_length == 2) {
			up_edit->setText(var_value[1]);
		}
		else {
			qCritical("get 'variables' json object field.");
			checkInfo->code = eOther;
			checkInfo->message = "问题json文件格式不正确。";
			_is_valid = false;
			emit signal_checkValid();
			return;
		}
		_vars_table->setCellWidget(row_number, varmin, low_edit);
		_vars_table->setCellWidget(row_number, varmax, up_edit);

		QComboBox *unit_combox = new QComboBox();
		initUnitComBo(unit_combox);
		_vars_table->setCellWidget(row_number, varunit, unit_combox);
		//map combobox signal
		connect(unit_combox, SIGNAL(currentIndexChanged(int)), signals_map_unit, SLOT(map()));
		signals_map_unit->setMapping(unit_combox, QString("%1-%2").arg(row_number).arg(varunit));
		//in 'rownumber'th row of table, save default unitComBo current data
		_vars_unit.insert(row_number, unit_combox->currentData(ROLE_MARK_UNIT).toInt());

		//当文本框输入改变时，触发校验信号；
		connect(low_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
		connect(up_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
		row_number++;
	}
	connect(signals_map_unit, SIGNAL(mapped(QString)), this, SLOT(slot_unitChange(QString)));

	//!add picture
	QPixmap pm = QPixmap(_atn_problem->pImage);
	if (pm.width() > INTRODUCEIMAGEPIX)
		_atn_image_label->setPixmap(pm.scaledToWidth(INTRODUCEIMAGEPIX));
	else
		_atn_image_label->setPixmap(pm);
}

void variablesTemplate::initLayout() {
	QHBoxLayout* h_layout = new QHBoxLayout;
	h_layout->addWidget(_vars_table, 1);
	h_layout->addWidget(_atn_image_label, 1);
	_layout = h_layout;
}

QLayout* variablesTemplate::getLayout() {
	return _layout;
}

//check input
void variablesTemplate::setCheckNullInfo() {
	checkInfo->code = eNull;
	checkInfo->message = "几何结构参数不能为空。";
}
void variablesTemplate::setCheckUncompleteInfo() {
	checkInfo->code = eInvalid;
	checkInfo->message = "几何结构参数输入不完整。";
}

bool variablesTemplate::checkInputValid() {
	if (!_is_valid) return false;
	QLineEdit* curr_low_edit, *curr_up_edit;
	QString var_low_value, var_up_value;
	for (int i = 0; i < _vars_table->rowCount(); i++) {
		curr_low_edit = static_cast<QLineEdit *>(_vars_table->cellWidget(i, varmin));
		curr_up_edit = static_cast<QLineEdit *>(_vars_table->cellWidget(i, varmax));
		var_low_value = curr_low_edit->text().trimmed();
		var_up_value = curr_up_edit->text().trimmed();

		if (var_low_value.isEmpty() || var_low_value.isNull()) {
			setCheckNullInfo();
			commonStyle::setLineEditWarningStyle(curr_low_edit);
			return false;
		}
		if (var_up_value.isEmpty() || var_up_value.isNull()) {
			setCheckNullInfo();
			commonStyle::setLineEditWarningStyle(curr_up_edit);
			return false;
		}
		if (var_low_value == "-") {
			setCheckUncompleteInfo();
			commonStyle::setLineEditWarningStyle(curr_low_edit);
			return false;
		}
		if (var_up_value == "-") {
			setCheckUncompleteInfo();
			commonStyle::setLineEditWarningStyle(curr_up_edit);
			return false;
		}

		double var_value_low_d = curr_low_edit->text().trimmed().toDouble();
		double var_value_up_d = curr_up_edit->text().trimmed().toDouble();
		//注意浮点数比较
		if (var_value_low_d > var_value_up_d) {
			checkInfo->code = eInvalid;
			checkInfo->message = QString("几何结构参数范围设置有误。");
			commonStyle::setLineEditWarningStyle(curr_low_edit);
			commonStyle::setLineEditWarningStyle(curr_up_edit);
			return false;
		}
		commonStyle::clearLineEditWarningStyle(curr_low_edit);
		commonStyle::clearLineEditWarningStyle(curr_up_edit);
	}
	return true;
}

//update json obj
void variablesTemplate::updateJObj() {
	QJsonObject mvariables_obj;
	QString var_key, var_note;
	double low_value, up_value;
	int unit_data;
	for (int i = 0; i < _vars_table->rowCount(); i++) {
		QComboBox *selectCombox = static_cast<QComboBox *>(_vars_table->cellWidget(i, varunit));
		QLineEdit *low_edit = static_cast<QLineEdit *>(_vars_table->cellWidget(i, varmin));
		QLineEdit *up_edit = static_cast<QLineEdit *>(_vars_table->cellWidget(i, varmax));
		var_key = _vars_table->item(i, varnote)->whatsThis().trimmed();
		var_note = _vars_table->item(i, varnote)->text().trimmed();

		low_value = low_edit->text().trimmed().toDouble();
		up_value = up_edit->text().trimmed().toDouble();
		unit_data = selectCombox->currentData(ROLE_MARK_UNIT).toInt();
		//conversion unit to 'm'
		if (unit_data == MARK_UNIT_LAMBDA) {
			low_value = unitConversion(low_value, unit_data, MARK_UNIT_M, _atn_problem->max_frequency);
			up_value = unitConversion(up_value, unit_data, MARK_UNIT_M, _atn_problem->max_frequency);
		}
		else if (unit_data != MARK_UNIT_M) {
			low_value = unitConversion(low_value, unit_data, MARK_UNIT_M);
			up_value = unitConversion(up_value, unit_data, MARK_UNIT_M);
		}

		QJsonObject itemobj;
		itemobj.insert("note", var_note);
		itemobj.insert(var_key, QString("[%1,%2]").arg(low_value).arg(up_value));
		mvariables_obj.insert(var_key, itemobj);
	}
	_obj->insert("variables", mvariables_obj);
}

//slots
void variablesTemplate::slot_unitChange(QString pos) {
	Q_ASSERT(!_vars_unit.isEmpty());
	QStringList coordinates = pos.split("-");
	int row = coordinates.at(0).toInt();
	int col = coordinates.at(1).toInt();
	int currentUnitData = _vars_unit[row];
	QComboBox *selectCombox = qobject_cast<QComboBox *>(_vars_table->cellWidget(row, col));
	QLineEdit *low_edit = qobject_cast<QLineEdit *>(_vars_table->cellWidget(row, varmin));
	QLineEdit *up_edit = qobject_cast<QLineEdit *>(_vars_table->cellWidget(row, varmax));

	int newUnitData = selectCombox->currentData(ROLE_MARK_UNIT).toInt();
	//when "newUnitData == currentUnitData", do nothing
	if (newUnitData == currentUnitData)
		return;

	double pre_min_value, pre_max_value, curr_min_value, curr_max_value;
	pre_min_value = low_edit->text().trimmed().toDouble();
	pre_max_value = up_edit->text().trimmed().toDouble();

	if (currentUnitData != MARK_UNIT_LAMBDA && newUnitData != MARK_UNIT_LAMBDA ){	
		//unit conversion without lambda
		curr_min_value = unitConversion(pre_min_value, currentUnitData, newUnitData);
		curr_max_value = unitConversion(pre_max_value, currentUnitData, newUnitData);
	}
	else {
		//unit conversion with lambda
		curr_min_value = unitConversion(pre_min_value, currentUnitData, newUnitData, _atn_problem->max_frequency);
		curr_max_value = unitConversion(pre_max_value, currentUnitData, newUnitData, _atn_problem->max_frequency);
	}
	low_edit->setText(QString::number(curr_min_value));
	up_edit->setText(QString::number(curr_max_value));
	//update current unit
	_vars_unit[row] = newUnitData;
}


variablesTemplate::~variablesTemplate() {}