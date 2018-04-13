#pragma execution_character_set("utf-8")
#include <quuid.h>
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "problemManage.h"

problemManage::problemManage(QWidget *parent) : QDialog(parent) {
	//remove help menu
	setMinimumSize(SUBWINDOW_WIDTH / 2, SUBWINDOW_HEIGHT);
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);

	//_exist_problem_label = new QLabel("�Ѿ����ڵ���������", this);
	//_remove_problem_label = new QLabel("�Ѿ��Ƴ�����������", this);
	_exist_problem_table = new tableTemplate();
	_remove_problem_table = new tableTemplate();
	_exist_problem_table->setFocusPolicy(Qt::NoFocus);
	_remove_problem_table->setFocusPolicy(Qt::NoFocus);
	_exist_problem_table->verticalHeader()->setVisible(false);
	_remove_problem_table->verticalHeader()->setVisible(false);
	_exist_problem_table->setSelectionMode(QAbstractItemView::SingleSelection);
	_remove_problem_table->setSelectionMode(QAbstractItemView::SingleSelection);
	_exist_problem_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_remove_problem_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_exist_problem_table->setColumnCount(1);
	_remove_problem_table->setColumnCount(1);

	_add_button = new QPushButton("���������>>", this);
	_remove_button = new QPushButton("�Ƴ�", this);
	_remove_button->setEnabled(false);
	_recovery_button = new QPushButton("�ָ�", this);
	_recovery_button->setEnabled(false);
	_delete_button = new QPushButton("ɾ��", this);
	_delete_button->setEnabled(false);
	_confirm_button = new QPushButton("ȷ��", this);
	_cancel_button = new QPushButton("ȡ��", this);

	_readme_text = new QTextEdit(this);
	_readme_text->setFixedHeight(SUBWINDOW_HEIGHT / 2);
	_readme_text->setReadOnly(true);
	_select_directory_label = new QLabel("ѡ���������⣺", this);
	_select_directory_text = new QLineEdit(this);
	_select_directory_button = new QPushButton("���", this);

	initExistProblem();
	initRemoveProblem();
	initLayout();

	connect(_exist_problem_table, SIGNAL(itemSelectionChanged()), this, SLOT(slot_existSelectChanged()));
	connect(_remove_problem_table, SIGNAL(itemSelectionChanged()), this, SLOT(slot_removeSelectChanged()));
	connect(_add_button, SIGNAL(clicked(bool)), this, SLOT(slot_add(bool)));
	connect(_remove_button, SIGNAL(clicked(bool)), this, SLOT(slot_remove(bool)));
	connect(_recovery_button, SIGNAL(clicked(bool)), this, SLOT(slot_reconvery(bool)));
	connect(_delete_button, SIGNAL(clicked(bool)), this, SLOT(slot_delete(bool)));
	connect(_confirm_button, SIGNAL(clicked(bool)), this, SLOT(slot_confirm(bool)));
	connect(_cancel_button, SIGNAL(clicked(bool)), this, SLOT(slot_cancel(bool)));
}

void problemManage::initExistProblem() {
	_exist_problem_table->clear();
	QStringList header1;
	header1 << "�Ѿ����ڵ���������";
	_exist_problem_table->setHorizontalHeaderLabels(header1);
	_exist_problem_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	QStringList problem_list;
	QVector<parsProblem>::iterator iter;
	for (iter = dataPool::global::g_problems.begin(); iter != dataPool::global::g_problems.end(); ++iter) {
		if (iter->oper == "i")
			problem_list.append(iter->name);
	}
	_exist_problem_table->setRowCount(problem_list.size());
	for (unsigned int m = 0; m < problem_list.size(); ++m) {
		_exist_problem_table->insert2table(m, 0, problem_list[m]);
	}
}

void problemManage::initRemoveProblem() {
	_remove_problem_table->clear();
	QStringList header2;
	header2 << "�Ѿ��Ƴ�����������";
	_remove_problem_table->setHorizontalHeaderLabels(header2);
	_remove_problem_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	QStringList problem_list;
	QVector<parsProblem>::iterator iter;
	for (iter = dataPool::global::g_problems.begin(); iter != dataPool::global::g_problems.end(); ++iter) {
		if (iter->oper == "d")
			problem_list.append(iter->name);
	}
	_remove_problem_table->setRowCount(problem_list.size());
	for (unsigned int m = 0; m < problem_list.size(); ++m) {
		_remove_problem_table->insert2table(m, 0, problem_list[m]);
	}
}

void problemManage::initLayout() {
	_layout = new QVBoxLayout;
	_manage_problem_layout = new QHBoxLayout;
	_manage_exist_problem_layout = new QVBoxLayout;
	_exist_problem_layout = new QHBoxLayout;
	_remove_problem_layout = new QHBoxLayout;
	_recovery_delete_button_layout = new QVBoxLayout;
	_add_button_layout = new QHBoxLayout;
	_confirm_cancel_button_layout = new QHBoxLayout;
	_add_new_problem_box = new QGroupBox;
	_add_new_problem_layout = new QVBoxLayout;
	_select_directory_layout = new QHBoxLayout;

	_exist_problem_layout->addWidget(_exist_problem_table);
	_exist_problem_layout->addWidget(_remove_button);	
	
	_recovery_delete_button_layout->addWidget(_recovery_button);
	_recovery_delete_button_layout->addWidget(_delete_button);
	_remove_problem_layout->addWidget(_remove_problem_table);
	_remove_problem_layout->addLayout(_recovery_delete_button_layout);
	
	_add_button_layout->addStretch();
	_add_button_layout->addWidget(_add_button);
	
	_confirm_cancel_button_layout->addStretch();
	_confirm_cancel_button_layout->addWidget(_confirm_button);
	_confirm_cancel_button_layout->addWidget(_cancel_button);

	//���������������Ⲽ��
	_manage_exist_problem_layout->addLayout(_exist_problem_layout);
	_manage_exist_problem_layout->addSpacing(20);
	_manage_exist_problem_layout->addLayout(_remove_problem_layout);
	_manage_exist_problem_layout->addSpacing(20);
	_manage_exist_problem_layout->addLayout(_add_button_layout);

	//�����������Ⲽ��	
	_select_directory_layout->addWidget(_select_directory_label);
	_select_directory_layout->addWidget(_select_directory_text);
	_select_directory_layout->addWidget(_select_directory_button);
	_add_new_problem_layout->addWidget(_readme_text);
	_add_new_problem_layout->addStretch();
	_add_new_problem_layout->addLayout(_select_directory_layout);
	_add_new_problem_layout->addStretch();
	_add_new_problem_box->setLayout(_add_new_problem_layout);

	_manage_problem_layout->addLayout(_manage_exist_problem_layout);
	_layout->addLayout(_manage_problem_layout);
	_layout->addSpacing(20);
	_layout->addLayout(_confirm_cancel_button_layout);
	_layout->setContentsMargins(10, 20, 10, 20);
	setLayout(_layout);
}

//slots
void problemManage::slot_existSelectChanged() {
	int select_number = _exist_problem_table->selectedItems().size();
	if (select_number == 0) {
		_remove_button->setEnabled(false);
		_exist_problem_table->clearSelection();
	}
	else {
		_remove_button->setEnabled(true);
	}
}
void problemManage::slot_removeSelectChanged() {
	int select_number = _remove_problem_table->selectedItems().size();
	if (select_number == 0) {
		_recovery_button->setEnabled(false);
		_delete_button->setEnabled(false);
		_remove_problem_table->clearSelection();
	}
	else {
		_recovery_button->setEnabled(true);
		_delete_button->setEnabled(true);
	}
}
void problemManage::slot_add(bool) {
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	_manage_problem_layout->addWidget(_add_new_problem_box);
	_add_button->setEnabled(false);
}
void problemManage::slot_remove(bool) {
	int curr_row = _exist_problem_table->currentRow();
	if (curr_row != -1) {
		QString select_item = _exist_problem_table->item(curr_row, 0)->text().trimmed();
		int new_row_index = _remove_problem_table->rowCount();
		_remove_problem_table->insertRow(new_row_index);
		_remove_problem_table->insert2table(new_row_index, 0, select_item);
		_exist_problem_table->removeRow(curr_row);
		_exist_problem_table->clearSelection();
	}
}
void problemManage::slot_reconvery(bool) {
	int curr_row = _remove_problem_table->currentRow();
	if (curr_row != -1) {
		QString select_item = _remove_problem_table->item(curr_row, 0)->text().trimmed();
		int new_row_index = _exist_problem_table->rowCount();
		_exist_problem_table->insertRow(new_row_index);
		_exist_problem_table->insert2table(new_row_index, 0, select_item);
		_remove_problem_table->removeRow(curr_row);
		_remove_problem_table->clearSelection();
	}
}
void problemManage::slot_delete(bool) {
	int curr_row = _remove_problem_table->currentRow();
	if (curr_row != -1) {
		QString select_item = _remove_problem_table->item(curr_row, 0)->text().trimmed();
		//ɾ��ѡ����
		QMessageBox::StandardButton rb = QMessageBox::question(NULL, "ɾ��", "ɾ��ѡ����?��ע�ⲻ�ɻָ���", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (rb == QMessageBox::Yes) {	
			_remove_problem_table->removeRow(select_item.toInt());
			_remove_problem_table->clearSelection();
		}
	}
}
void problemManage::slot_confirm(bool) {

}
void problemManage::slot_cancel(bool) {
	this->close();
}

problemManage::~problemManage(){
	delete _layout;
	_layout = nullptr;
	/*delete _manage_problem_layout;
	_manage_problem_layout = nullptr;
	delete _manage_exist_problem_layout;
	_manage_exist_problem_layout = nullptr;
	delete _exist_problem_layout;
	_exist_problem_layout = nullptr;
	delete _remove_problem_layout;
	_remove_problem_layout = nullptr;
	delete _recovery_delete_button_layout;
	_recovery_delete_button_layout = nullptr;
	delete _add_button_layout;
	_add_button_layout = nullptr;
	delete _confirm_cancel_button_layout;
	_confirm_cancel_button_layout = nullptr;
	delete _add_new_problem_box;
	_add_new_problem_box = nullptr;
	delete _add_new_problem_layout;
	_add_new_problem_layout = nullptr;
	delete _select_directory_layout;
	_select_directory_layout = nullptr;
	delete _exist_problem_table;
	_exist_problem_table = nullptr;
	delete _remove_problem_table;
	_remove_problem_table = nullptr;*/
}