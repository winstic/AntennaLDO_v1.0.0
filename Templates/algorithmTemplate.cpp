#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "algorithmTemplate.h"

algorithmTemplate::algorithmTemplate(parsProblem* atn_problem, QJsonObject* global_obj, parsAlgorithm** palgorithm, QJsonObject** algorithm_obj, iTemplate *parent)
	: iTemplate(parent), _atn_problem(atn_problem), _global_obj(global_obj), _algorithm(palgorithm), _algorithm_obj(algorithm_obj), _is_valid(true) {
	_alg_label = new QLabel("选择算法:", this);
	_alg_label->setFixedWidth(80);
	_alg_combox = new QComboBox(this);
	_alg_combox->setMinimumSize(200, 25);
	_alg_vars_table = new tableTemplate();
	_alg_vars_table->horizontalHeader()->setVisible(false);
	_alg_vars_table->setColumnCount(2);
	_alg_vars_table->horizontalHeader()->setSectionResizeMode(valueFlag, QHeaderView::Stretch);
	initAlgComboItem();	
	if (_algorithm == nullptr) {
		qCritical("nullptr pointer");
		exit(1);
	}
	if (*_algorithm == nullptr || *_algorithm == 0) {
		//set default algorithm
		_alg_combox->setCurrentIndex(0);
		*_algorithm = dataPool::global::getAlgorithmByID(_alg_combox->currentData().toInt());
	}
	else
		_alg_combox->setCurrentText((*_algorithm)->name);
	initDefaultData();
	initLayout();
	connect(_alg_combox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_algName(int)));
}

void algorithmTemplate::initLayout() {
	QVBoxLayout* v_layout = new QVBoxLayout;
	QHBoxLayout* h_layout = new QHBoxLayout;
	h_layout->addWidget(_alg_label);
	h_layout->addWidget(_alg_combox);
	v_layout->addLayout(h_layout);
	v_layout->addWidget(_alg_vars_table);
	v_layout->setSpacing(20);
	_layout = v_layout;
}

void algorithmTemplate::initDefaultData() {
	QString alg_json_path = QString("%1/%2_conf.json").arg((*_algorithm)->path).arg((*_algorithm)->name);
	QJsonObject alg_obj = parseJson::getJsonObj(alg_json_path);
	if (alg_obj.isEmpty()) {
		qCritical("cannot parse algorithm json file: '%s'", qUtf8Printable(alg_json_path));
		checkInfo->code = eOther;
		checkInfo->message = "算法json文件格式不正确。";
		_is_valid = false;
		emit signal_checkValid();
		QMessageBox::critical(0, QString("错误"), QString("读取算法配置文件失败！"));
		return;
	}
	//+2 启动进程数，评估容忍时间
	_alg_vars_table->setRowCount(alg_obj.count() + 2);
	
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
		QLineEdit* var_value_edit = new QLineEdit;
		var_value_edit->setText(var_value);
		_alg_vars_table->setCellWidget(row_number, valueFlag, var_value_edit);
		row_number++;
	}
	_alg_vars_table->insert2table(row_number, keyFlag, "启动进程数");
	QLineEdit* thread_number = new QLineEdit;
	thread_number->setText(QString::number(_global_obj->value("ThreadNum").toString().trimmed().toInt() - 1));
	_alg_vars_table->setCellWidget(row_number, valueFlag, thread_number);
	row_number++;
	_alg_vars_table->insert2table(row_number, keyFlag, "评估容忍时间");
	QLineEdit* max_time = new QLineEdit;
	max_time->setText(QString::number(_global_obj->value("EVALUATE_TIMEOUT").toString().trimmed().toInt()));
	_alg_vars_table->setCellWidget(row_number, valueFlag, max_time);
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

bool algorithmTemplate::checkInputValid() {
	if (!_is_valid) return false;
	return true;
}

//update _obj
void algorithmTemplate::updateJObj() {
	//初始化*_algorithm_obj
	*_algorithm_obj = new QJsonObject;
	QString varKey, varValue, varNote;
	for (int i = 0; i < _alg_vars_table->rowCount(); ++i) {
		varKey = _alg_vars_table->item(i, keyFlag)->whatsThis().trimmed();
		varNote = _alg_vars_table->item(i, keyFlag)->text().trimmed();
		varValue = QString("%1").arg(_alg_vars_table->item(i, valueFlag)->text().trimmed());
		QJsonObject itemobj;
		itemobj.insert(varKey, varValue);
		itemobj.insert("note", varNote);
		(*_algorithm_obj)->insert(varKey, itemobj);
	}
}

//slots function
void algorithmTemplate::slot_algName(const int index) {
	*_algorithm = dataPool::global::getAlgorithmByID(_alg_combox->itemData(index).toInt());
	initDefaultData();
}