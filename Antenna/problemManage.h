#pragma once

#include <QtWidgets>
#include "../Utility/global.h"
#include "../Templates/tableTemplate.h"

class problemManage : public QDialog {
	Q_OBJECT
public:
	problemManage(QWidget *parent = 0);
	~problemManage();

public slots:
	void slot_existSelectChanged();
	void slot_removeSelectChanged();
	void slot_add(bool);
	void slot_remove(bool);
	void slot_reconvery(bool);
	void slot_delete(bool);
	void slot_confirm(bool);
	void slot_cancel(bool);

private:
	void initLayout();
	void initExistProblem();
	void initRemoveProblem();

private:
	tableTemplate* _exist_problem_table;
	tableTemplate* _remove_problem_table;
	QPushButton* _add_button;
	QPushButton* _remove_button;
	QPushButton* _recovery_button;
	QPushButton* _delete_button;
	QPushButton* _confirm_button;
	QPushButton* _cancel_button;
	QTextEdit* _readme_text;
	QLineEdit* _select_directory_text;
	QLabel* _select_directory_label;
	QPushButton* _select_directory_button;

	//layout
	QVBoxLayout* _layout;
	QHBoxLayout* _manage_problem_layout;
	QVBoxLayout* _manage_exist_problem_layout;
	QHBoxLayout* _exist_problem_layout;
	QHBoxLayout* _remove_problem_layout;
	QVBoxLayout* _recovery_delete_button_layout;
	QHBoxLayout* _add_button_layout;
	QHBoxLayout* _confirm_cancel_button_layout;
	QGroupBox* _add_new_problem_box;
	QVBoxLayout* _add_new_problem_layout;
	QHBoxLayout* _select_directory_layout;
};
