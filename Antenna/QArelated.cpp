#pragma execution_character_set("utf-8")
#include <quuid.h>
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "QArelated.h"

QArelated::QArelated(parsProblem* atn_problem, QWidget *parent) : QDialog(parent), _atn_problem(atn_problem) {
	//remove help menu
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);

	_problem_label = new QLabel("问题选择", this);
	_problem_combobox = new QComboBox(this);
	_problem_combobox->setMinimumSize(200, 25);
	if (_atn_problem == nullptr)
		_problem_combobox->setCurrentText("");
	else
		_problem_combobox->setCurrentText(_atn_problem->name);
	_related_algorithm_table = new tableTemplate();
	_unrelated_algorithm_table = new tableTemplate();
	_related_algorithm_table->setFocusPolicy(Qt::NoFocus);
	_unrelated_algorithm_table->setFocusPolicy(Qt::NoFocus);
	_related_algorithm_table->verticalHeader()->setVisible(false);
	_unrelated_algorithm_table->verticalHeader()->setVisible(false);
	_related_algorithm_table->setSelectionMode(QAbstractItemView::SingleSelection);
	_unrelated_algorithm_table->setSelectionMode(QAbstractItemView::SingleSelection);
	_related_algorithm_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_unrelated_algorithm_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_related_algorithm_table->setColumnCount(1);
	_unrelated_algorithm_table->setColumnCount(1);
	QStringList header1, header2;
	header1 << "已关联算法";	
	_related_algorithm_table->setHorizontalHeaderLabels(header1);
	_related_algorithm_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	header2 << "可选与其关联的算法";	
	_unrelated_algorithm_table->setHorizontalHeaderLabels(header2);
	_unrelated_algorithm_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	_add_button = new QPushButton("<<添加", this);
	_add_button->setEnabled(false);
	_remove_button = new QPushButton("移除>>", this);
	_remove_button->setEnabled(false);
	_save_button = new QPushButton("保存", this);
	initProblemCombobox();
	fillRelatedAlgorithm();
	fillUnRelatedAlgorithm();
	initLayout();

	connect(_problem_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_problemChanged(int)));
	connect(_related_algorithm_table, SIGNAL(itemSelectionChanged()), this, SLOT(slot_relatedSelectChanged()));
	connect(_unrelated_algorithm_table, SIGNAL(itemSelectionChanged()), this, SLOT(slot_unrelatedSelectChanged()));
	connect(_add_button, SIGNAL(clicked(bool)), this, SLOT(slot_add(bool)));
	connect(_remove_button, SIGNAL(clicked(bool)), this, SLOT(slot_remove(bool)));
	connect(_save_button, SIGNAL(clicked(bool)), this, SLOT(slot_save(bool)));
}

void QArelated::initProblemCombobox() {
	QVector<parsProblem>::iterator iter;
	for (iter = dataPool::global::g_problems.begin(); iter != dataPool::global::g_problems.end(); ++iter) {
		_problem_combobox->addItem(iter->name);
	}
}

void QArelated::fillRelatedAlgorithm() {
	_related_algorithm_table->clear();
	QStringList header1;
	header1 << "已关联算法";
	_related_algorithm_table->setHorizontalHeaderLabels(header1);
	QStringList algorithm_list;
	parsProblem* atn_problem = dataPool::global::getProblemByName(_problem_combobox->currentText().trimmed());
	QMap<alg4pro, QString>::iterator iter;
	for (iter = dataPool::global::g_associates.begin(); iter != dataPool::global::g_associates.end(); ++iter) {
		if (atn_problem->id == iter.key().second) {
			parsAlgorithm* algorithm = dataPool::global::getAlgorithmByID(iter.key().first);
			algorithm_list.append(algorithm->name);
		}
	}
	_related_algorithm_table->setRowCount(algorithm_list.length());
	for (int i = 0; i < algorithm_list.size(); ++i)
		_related_algorithm_table->insert2table(i, 0, algorithm_list[i]);
}

void QArelated::fillUnRelatedAlgorithm() {
	_unrelated_algorithm_table->clear();
	QStringList header2;
	header2 << "可选与其关联的算法";
	_unrelated_algorithm_table->setHorizontalHeaderLabels(header2);
	QStringList algorithm_list;
	QVector<parsAlgorithm>::iterator iter;
	for (iter = dataPool::global::g_algorithms.begin(); iter != dataPool::global::g_algorithms.end(); ++iter) {
		int i = 0;
		QString algname = _related_algorithm_table->item(i, 0)->text();
		for (; i < _related_algorithm_table->rowCount(); ++i) {
			if (iter->name == _related_algorithm_table->item(i, 0)->text().trimmed())
				break;
		}
		if(i == _related_algorithm_table->rowCount())
			algorithm_list.append(iter->name);
	}
	_unrelated_algorithm_table->setRowCount(algorithm_list.size());
	for (int i = 0; i < algorithm_list.size(); ++i)
		_unrelated_algorithm_table->insert2table(i, 0, algorithm_list[i]);
}

void QArelated::initLayout() {
	QHBoxLayout* hlayout1 = new QHBoxLayout();
	QHBoxLayout* hlayout2 = new QHBoxLayout();
	QHBoxLayout* hlayout3 = new QHBoxLayout();
	QVBoxLayout* vlayout1 = new QVBoxLayout();
	QVBoxLayout* vlayout = new QVBoxLayout();
	hlayout1->addWidget(_problem_label);
	hlayout1->addWidget(_problem_combobox);
	hlayout1->addStretch();

	hlayout2->addWidget(_related_algorithm_table);
	vlayout1->addWidget(_add_button);
	vlayout1->addWidget(_remove_button);
	hlayout2->addLayout(vlayout1);
	hlayout2->addWidget(_unrelated_algorithm_table);

	hlayout3->addStretch();
	hlayout3->addWidget(_save_button);

	vlayout->addLayout(hlayout1);
	vlayout->addSpacing(20);
	vlayout->addLayout(hlayout2);
	vlayout->addSpacing(20);
	vlayout->addLayout(hlayout3);
	vlayout->setContentsMargins(10, 20, 10, 20);
	setLayout(vlayout);
}

void QArelated::updateAssociates() {
	//update g_associates		
	parsProblem* problem = dataPool::global::getProblemByName(_problem_combobox->currentText());

	//delete g_associates first
	QList<alg4pro> delete_keys;
	QMap<alg4pro, QString>::iterator iter;
	for (iter = dataPool::global::g_associates.begin(); iter != dataPool::global::g_associates.end(); ++iter) {
		if (problem->id == iter.key().second) {
			delete_keys.append(iter.key());
		}
	}
	for (int j = 0; j < delete_keys.length(); ++j) {
		dataPool::global::g_associates.remove(delete_keys[j]);
	}

	//then insert newest item
	for (int i = 0; i < _related_algorithm_table->rowCount(); ++i) {
		QUuid id = QUuid::createUuid();
		parsAlgorithm* algorithm = dataPool::global::getAlgorithmByName(_related_algorithm_table->item(i, 0)->text().trimmed());
		alg4pro massociate;
		massociate = qMakePair(algorithm->id, problem->id);
		dataPool::global::g_associates[massociate] = id.toString();
	}
}

void QArelated::slot_problemChanged(int) {
	fillRelatedAlgorithm();
	fillUnRelatedAlgorithm();
}

void QArelated::slot_relatedSelectChanged() {
	//tableTemplate* table_obj = qobject_cast<tableTemplate*>(sender());
	int select_number = _related_algorithm_table->selectedItems().size();
	if (select_number == 0) {
		_remove_button->setEnabled(false);
		_related_algorithm_table->clearSelection();
	}
	else {
		_remove_button->setEnabled(true);
	}
}
void QArelated::slot_unrelatedSelectChanged() {
	int select_number = _unrelated_algorithm_table->selectedItems().size();
	if (select_number == 0) {
		_add_button->setEnabled(false);
		_unrelated_algorithm_table->clearSelection();
	}
	else {
		_add_button->setEnabled(true);
	}
}

void QArelated::slot_add(bool) {
	int curr_row = _unrelated_algorithm_table->currentRow();
	if (curr_row != -1) {
		QString select_item = _unrelated_algorithm_table->item(curr_row, 0)->text().trimmed();
		int new_row_index = _related_algorithm_table->rowCount();
		_related_algorithm_table->insertRow(new_row_index);
		_related_algorithm_table->insert2table(new_row_index, 0, select_item);
		_unrelated_algorithm_table->removeRow(curr_row);
		_unrelated_algorithm_table->clearSelection();
		updateAssociates();
	}
}

void QArelated::slot_remove(bool) {
	int curr_row = _related_algorithm_table->currentRow();
	if (curr_row != -1) {
		QString select_item = _related_algorithm_table->item(curr_row, 0)->text().trimmed();
		int new_row_index = _unrelated_algorithm_table->rowCount();
		_unrelated_algorithm_table->insertRow(new_row_index);
		_unrelated_algorithm_table->insert2table(new_row_index, 0, select_item);
		_related_algorithm_table->removeRow(curr_row);
		_related_algorithm_table->clearSelection();
		updateAssociates();
	}
}

void QArelated::slot_save(bool) {
	const QString associate_json_file = "Utility/associates.json";
	QJsonObject* obj = new QJsonObject;
	QMap<alg4pro, QString>::iterator iter;
	for (iter = dataPool::global::g_associates.begin(); iter != dataPool::global::g_associates.end(); ++iter) {
		QJsonObject* tmpobj = new QJsonObject;
		tmpobj->insert("algorithmID", iter.key().first);
		tmpobj->insert("problemID", iter.key().second);
		obj->insert(iter.value(), *tmpobj);
	}
	if (parseJson::write(associate_json_file, obj))
		this->close();
	else {
		qCritical("save failed in associate json file.");
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
	}
}

QArelated::~QArelated() {}