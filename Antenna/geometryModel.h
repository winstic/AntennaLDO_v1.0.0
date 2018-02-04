#pragma once

#include <QtWidgets>
#include "../Templates/variablesTemplate.h"

class geometryModel : public QDialog {
	Q_OBJECT
public:
	geometryModel(parsProblem* atn_problem, bool is_running = false, QWidget *parent = 0);
	~geometryModel();

public slots:
	void slot_saveAllButton(bool);

private:
	void initLayout();

private:
	parsProblem* _atn_problem;
	QJsonObject _problem_obj;
	variablesTemplate* _variables_widget;
	QPushButton* _save_all_button;
	QLabel* _hint;

	bool _is_running;
};