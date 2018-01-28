#pragma once

#include <QtWidgets>
#include "../Utility/global.h"
#include "../Templates/tableTemplate.h"

class QArelated : public QDialog {
	Q_OBJECT
public:
	QArelated(parsProblem* atn_problem, QWidget *parent = 0);
	~QArelated();

public slots:
	void slot_problemChanged(int);
	void slot_relatedSelectChanged();
	void slot_unrelatedSelectChanged();
	void slot_add();
	void slot_remove();
	void slot_save();

private:
	void initProblemCombobox();
	void initLayout();
	void fillRelatedAlgorithm();
	void fillUnRelatedAlgorithm();

private:
	parsProblem* _atn_problem;
	QLabel* _problem_label;
	QComboBox* _problem_combobox;
	tableTemplate* _related_algorithm_table;
	tableTemplate* _unrelated_algorithm_table;
	QPushButton* _add_button;
	QPushButton* _remove_button;
	QPushButton* _save_button;
};
