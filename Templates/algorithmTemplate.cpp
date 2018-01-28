#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "algorithmTemplate.h"

algorithmTemplate::algorithmTemplate(parsProblem* atn_problem, QJsonObject* global_obj, parsAlgorithm** palgorithm, QJsonObject** algorithm_obj, iTemplate *parent)
	: iTemplate(parent), _atn_problem(atn_problem), _global_obj(global_obj), _algorithm(palgorithm), _algorithm_obj(algorithm_obj){
	_alg_label = new QLabel("选择算法:", this);
	_alg_label->setFixedWidth(80);
	_alg_combox = new QComboBox(this);
	_alg_combox->setMinimumSize(200, 25);
	_alg_vars_table = new tableTemplate();
	_alg_vars_table->horizontalHeader()->setVisible(false);
	_alg_vars_table->setColumnCount(2);
	_alg_vars_table->horizontalHeader()->setSectionResizeMode(valueFlag, QHeaderView::Stretch);
	_thread_number_edit = new QLineEdit;
	_max_time_edit = new QLineEdit;
	_thread_number_edit->setValidator(getPositiveIntReg());
	_max_time_edit->setValidator(getPositiveIntReg());

	initAlgComboItem();	
	//set default algorithm
	if (_algorithm == nullptr) {
		qCritical("null pointer _algorithm.");
		exit(1);
	}
	if ((*_algorithm) != nullptr && _alg_combox->findText((*_algorithm)->name)) {
		_alg_combox->setCurrentText((*_algorithm)->name);
	}
	else {
		_alg_combox->setCurrentIndex(0);
		(*_algorithm) = dataPool::global::getAlgorithmByID(_alg_combox->currentData().toInt());
	}
	initDefaultData();
	initLayout();
	connect(_alg_combox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_algName(int)));
	connect(_thread_number_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_textChanged(QString)));
	connect(_max_time_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_textChanged(QString)));
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
		qCritical("get algorithm json object field.");
		QMessageBox::critical(0, QString("警告"), QString("读取算法配置文件失败！"));
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
		_alg_vars_table->item(row_number, keyFlag)->setWhatsThis(var_key);
		QTableWidgetItem *flagItem = _alg_vars_table->item(row_number, keyFlag);
		flagItem->setWhatsThis(var_key);
		//set this column cannot edit
		flagItem->setFlags(flagItem->flags() & (~Qt::ItemIsEditable));
		QLineEdit* var_value_edit = new QLineEdit;
		var_value_edit->setValidator(getPositiveIntReg());
		var_value_edit->setText(var_value);
		_alg_vars_table->setCellWidget(row_number, valueFlag, var_value_edit);
		row_number++;
	}
	_alg_vars_table->insert2table(row_number, keyFlag, "启动进程数");
	_thread_number_edit->setText(QString::number(_global_obj->value("ThreadNum").toString().trimmed().toInt()));
	_alg_vars_table->setCellWidget(row_number, valueFlag, _thread_number_edit);
	row_number++;
	_alg_vars_table->insert2table(row_number, keyFlag, "评估容忍时间");
	_max_time_edit->setText(QString::number(_global_obj->value("EVALUATE_TIMEOUT").toString().trimmed().toInt()));
	_alg_vars_table->setCellWidget(row_number, valueFlag, _max_time_edit);
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
	for (int i = 0; i < _alg_vars_table->rowCount() - 2; i++) {
		QLineEdit *value_edit = qobject_cast<QLineEdit *>(_alg_vars_table->cellWidget(i, valueFlag));
		QString alg_value = value_edit->text().trimmed();

		if (alg_value.isEmpty() || alg_value.isNull()) {
			qCritical("algorithm variables value is null.");
			checkInfo->code = eNull;
			checkInfo->message = "算法设置参数不能为空";
			commonStyle::setLineEditWarningStyle(value_edit);
			emit signal_checkValid();
			return false;
		}
		commonStyle::clearLineEditWarningStyle(value_edit);
	}
	QString thread_num = _thread_number_edit->text().trimmed();
	QString max_time = _max_time_edit->text().trimmed();
	if (thread_num.isEmpty() || thread_num.isNull()) {
		qCritical("global variables value is null.");
		checkInfo->code = eNull;
		checkInfo->message = "算法设置参数不能为空";
		commonStyle::setLineEditWarningStyle(_thread_number_edit);
		emit signal_checkValid();
		return false;
	}
	if (max_time.isEmpty() || max_time.isNull()) {
		qCritical("global variables value is null.");
		checkInfo->code = eNull;
		checkInfo->message = "算法设置参数不能为空";
		commonStyle::setLineEditWarningStyle(_max_time_edit);
		emit signal_checkValid();
		return false;
	}
	commonStyle::clearLineEditWarningStyle(_thread_number_edit);
	commonStyle::clearLineEditWarningStyle(_max_time_edit);
	return true;
}

//update _obj
void algorithmTemplate::updateJObj() {
	//初始化*_algorithm_obj
	*_algorithm_obj = new QJsonObject;
	QString varKey, varValue, varNote;
	int i = 0;
	for (int i = 0; i < _alg_vars_table->rowCount() - 2; ++i) {
		varKey = _alg_vars_table->item(i, keyFlag)->whatsThis().trimmed();
		varNote = _alg_vars_table->item(i, keyFlag)->text().trimmed();
		QLineEdit* value_edit = qobject_cast<QLineEdit *>(_alg_vars_table->cellWidget(i, valueFlag));
		varValue = QString("%1").arg(value_edit->text().trimmed());
		QJsonObject itemobj;
		itemobj.insert(varKey, varValue);
		itemobj.insert("note", varNote);
		(*_algorithm_obj)->insert(varKey, itemobj);
		connect(value_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_textChanged(QString)));
	}
	_global_obj->insert("ThreadNum", _thread_number_edit->text().trimmed());
	_global_obj->insert("EVALUATE_TIMEOUT", _max_time_edit->text().trimmed());
	_global_obj->insert("ALGORITHM_NAME", (*_algorithm)->name);
}

//slots function
void algorithmTemplate::slot_algName(const int index) {
	(*_algorithm) = dataPool::global::getAlgorithmByID(_alg_combox->itemData(index).toInt());
	initDefaultData();
}

void algorithmTemplate::slot_textChanged(QString) {
	QLineEdit *edit = qobject_cast<QLineEdit *>(sender());
	commonStyle::clearLineEditWarningStyle(edit);
	emit signal_checkValid();
}