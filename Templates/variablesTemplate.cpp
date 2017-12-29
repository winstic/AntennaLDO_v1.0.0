#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "variablesTemplate.h"

variablesTemplate::variablesTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent)
	: iTemplate(parent), _atn_problem(atn_problem), _obj(obj) {
	_atn_image_label = new QLabel(this);
	_vars_table = new tableTemplate();
	_vars_table->setColumnCount(4);
	QStringList header;
	header << "变量" << "最小值" << "最大值" << "单位";
	_vars_table->setHorizontalHeaderLabels(header);
	_vars_table->horizontalHeader()->setSectionResizeMode(varnote, QHeaderView::Stretch);
	_vars_table->horizontalHeader()->setSectionResizeMode(varunit, QHeaderView::ResizeToContents);
	initDefaultData();
	initLayout();
}

void variablesTemplate::initDefaultData() {
	QJsonObject variables_obj = parseJson::getSubJsonObj(*_obj, "variables");
	if (variables_obj.isEmpty()) {
		qCritical("get 'variables' json object field.");
		return;
	}
	QJsonObject var_obj;
	QString var_key;
	QStringList var_value;
	int row_number = 0, value_list_length;
	QSignalMapper* signals_map_unit = new QSignalMapper;	//use signalmaper manage signals in table

	_vars_table->setRowCount(variables_obj.count());

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
		_vars_table->insert2table(row_number, varmin, var_value[0]);
		if (value_list_length == 1)
			_vars_table->insert2table(row_number, varmax, var_value[0]);
		else
			_vars_table->insert2table(row_number, varmax, var_value[1]);

		QComboBox *unit_combox = new QComboBox();
		initUnitComBo(unit_combox);
		_vars_table->setCellWidget(row_number, varunit, unit_combox);
		//map combobox signal
		connect(unit_combox, SIGNAL(currentIndexChanged(int)), signals_map_unit, SLOT(map()));
		signals_map_unit->setMapping(unit_combox, QString("%1-%2").arg(row_number).arg(varunit));
		//in 'rownumber'th row of table, save default unitComBo current data
		_vars_unit.insert(row_number, unit_combox->currentData(ROLE_MARK_UNIT).toInt());
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

//update json obj
void variablesTemplate::updateJObj() {
	QJsonObject mvariables_obj;
	QString var_key, var_value, var_note;
	for (int i = 0; i < _vars_table->rowCount(); i++) {
		var_key = _vars_table->item(i, varnote)->whatsThis().trimmed();
		var_note = _vars_table->item(i, varnote)->text().trimmed();
		var_value = QString("[%1,%2]").arg(_vars_table->item(i, varmin)->text().trimmed())
			.arg(_vars_table->item(i, varmax)->text().trimmed());
		QJsonObject itemobj;
		itemobj.insert("note", var_note);
		itemobj.insert(var_key, var_value);
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
	QComboBox *selectCombox = (QComboBox *)_vars_table->cellWidget(row, col);
	int newUnitData = selectCombox->currentData(ROLE_MARK_UNIT).toInt();
	//when "newUnitData == currentUnitData", do nothing
	if (newUnitData == currentUnitData)
		return;

	double pre_min_value, pre_max_value, curr_min_value, curr_max_value;
	pre_min_value = _vars_table->item(row, varmin)->text().trimmed().toDouble();
	pre_max_value = _vars_table->item(row, varmax)->text().trimmed().toDouble();

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
	_vars_table->insert2table(row, varmin, QString::number(curr_min_value));
	_vars_table->insert2table(row, varmax, QString::number(curr_max_value));
	//update current unit
	_vars_unit[row] = newUnitData;
}


variablesTemplate::~variablesTemplate() {}