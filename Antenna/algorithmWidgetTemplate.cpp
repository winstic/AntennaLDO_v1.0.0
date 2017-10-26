#include "algorithmWidgetTemplate.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"

algorithmTemplate::algorithmTemplate(parsProblem* atn_problem, QWidget *parent) : QDialog(parent),
_atn_problem(atn_problem){
}

void algorithmTemplate::initAlgorithmWidget(QLayout* layout) {
	//initialize necessary parameters
	_alg_label = new QLabel(tr("选择算法:"), this);
	_alg_label->setFixedWidth(80);
	_alg_combox = new QComboBox(this);
	_alg_vars_table = new mTable();
	_alg_vars_table->setColumnCount(2);
	_alg_vars_table->horizontalHeader()->setSectionResizeMode(valueFlag, QHeaderView::Stretch);
	initAlgComboItem();
	_alg_combox->setCurrentIndex(0);
	_algorithm = dataPool::getAlgorithmByID(_alg_combox->currentData().toInt());
	connect(_alg_combox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_algName(int)));
	//setting default data
	setCurrAlgorithmParameters();

	//layout
	QVBoxLayout v_layout;
	QHBoxLayout h_layout;

	h_layout.addWidget(_alg_label);
	h_layout.addWidget(_alg_combox);
	v_layout.addLayout(&h_layout);
	v_layout.addWidget(_alg_vars_table);
	layout = &v_layout;
}

void algorithmTemplate::initAlgComboItem() {
	int problem_id = _atn_problem->id;
	QMap<alg4pro, unsigned int>::iterator iter;		//like((algid, proid), associateId) 
	for (iter = dataPool::g_associates.begin(); iter != dataPool::g_associates.end(); ++iter) {
		if (iter.key().second == problem_id) {
			parsAlgorithm* algorithm = dataPool::getAlgorithmByID(iter.key().first);
			_alg_combox->addItem(algorithm->name, algorithm->id);	//so you can get algorithm_id through _alg_combox->Data()
		}
	}
}

void algorithmTemplate::setCurrAlgorithmParameters() {
	QString alg_json_path = QString("%1/%2_conf.json").arg(_algorithm->path).arg(_algorithm->name);
	QJsonObject alg_obj = parseJson::getJsonObj(alg_json_path);
	if (alg_obj.isEmpty()) {
		qCritical(dataPool::str2char(QString("cannot parse algorithm json file [%1]").arg(alg_json_path)));
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

//slots function
void algorithmTemplate::slot_algName(const int index) {
	_algorithm = dataPool::getAlgorithmByID(_alg_combox->itemData(index).toInt());
	setCurrAlgorithmParameters();
}