#include "../Utility/parseJson.h"
#include "algorithmTemplate.h"

algorithmTemplate::algorithmTemplate(parsProblem* atn_problem, QJsonObject algorithm_obj, parsAlgorithm* palgorithm, iTemplate *parent)
	: iTemplate(parent), _atn_problem(atn_problem), _algorithm(palgorithm), _algorithm_obj(algorithm_obj){
	_alg_label = new QLabel(tr("选择算法:"), this);
	_alg_label->setFixedWidth(80);
	_alg_combox = new QComboBox(this);
	_alg_vars_table = new tableTemplate();
	_alg_vars_table->setColumnCount(2);
	_alg_vars_table->horizontalHeader()->setSectionResizeMode(valueFlag, QHeaderView::Stretch);
	initAlgComboItem();
	initLayout();
	if (_algorithm == 0 || _algorithm == nullptr) {
		//set default algorithm
		_alg_combox->setCurrentIndex(0);
		_algorithm = dataPool::global::getAlgorithmByID(_alg_combox->currentData().toInt());
	}
	else
		_alg_combox->setCurrentText(_algorithm->name);
	initDefaultData();
	connect(_alg_combox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_algName(int)));
}

void algorithmTemplate::initLayout() {
	QVBoxLayout v_layout;
	QHBoxLayout h_layout;
	h_layout.addWidget(_alg_label);
	h_layout.addWidget(_alg_combox);
	v_layout.addLayout(&h_layout);
	v_layout.addWidget(_alg_vars_table);
	_layout = &v_layout;
}

void algorithmTemplate::initDefaultData() {
	QString alg_json_path = QString("%1/%2_conf.json").arg(_algorithm->path).arg(_algorithm->name);
	QJsonObject alg_obj = parseJson::getJsonObj(alg_json_path);
	if (alg_obj.isEmpty()) {
		qCritical("cannot parse algorithm json file: '%s'", qUtf8Printable(alg_json_path));
		QMessageBox::critical(0, QString("Error"), QString("error: Cannot parse algorithm json file"));
		return;
	}
	_alg_vars_table->setRowCount(alg_obj.count());
	QString var_key, var_value, value_note;
	QJsonObject var_obj; //format:{"generation": "1000", "note": "最大代数", "instruction": "最大迭代次数"}
	int row_number = 0;

	for (QJsonObject::iterator iter = alg_obj.begin(); iter != alg_obj.end(); ++iter) {
		var_key = iter.key();
		var_obj = iter.value().toObject();
		var_value = var_obj.value(var_key).toString().trimmed();
		value_note = var_obj.value("note").toString().trimmed();
		//valueInstruction = varObj.value("instruction").toString().trimmed();

		_alg_vars_table->insert2table(row_number, keyFlag, value_note);
		QTableWidgetItem *flagItem = _alg_vars_table->item(row_number, keyFlag);
		flagItem->setWhatsThis(var_key);
		//set this column cannot edit
		flagItem->setFlags(flagItem->flags() & (~Qt::ItemIsEditable));
		_alg_vars_table->insert2table(row_number, valueFlag, var_value);
		row_number++;
	}
}

void algorithmTemplate::initAlgComboItem() {
	int problem_id = _atn_problem->id;
	QMap<alg4pro, unsigned int>::iterator iter;		//like((algid, proid), associateId) 
	for (iter = dataPool::global::g_associates.begin(); iter != dataPool::global::g_associates.end(); ++iter) {
		if (iter.key().second == problem_id) {
			parsAlgorithm* algorithm = dataPool::global::getAlgorithmByID(iter.key().first);
			_alg_combox->addItem(algorithm->name, algorithm->id);	//so you can get algorithm_id through _alg_combox->Data()
		}
	}
}

QLayout* algorithmTemplate::getLayout() {
	return _layout;
}
//update _obj
void algorithmTemplate::updateJObj() {
	QString varKey, varValue, varNote;
	for (int i = 0; i < _alg_vars_table->rowCount(); ++i) {
		varKey = _alg_vars_table->item(i, keyFlag)->whatsThis().trimmed();
		varNote = _alg_vars_table->item(i, keyFlag)->text().trimmed();
		varValue = QString("%1").arg(_alg_vars_table->item(i, valueFlag)->text().trimmed());
		QJsonObject itemobj;
		itemobj.insert(varKey, varValue);
		itemobj.insert("note", varNote);
		_algorithm_obj.insert(varKey, itemobj);
	}
}

//slots function
void algorithmTemplate::slot_algName(const int index) {
	_algorithm = dataPool::global::getAlgorithmByID(_alg_combox->itemData(index).toInt());
	initDefaultData();
}