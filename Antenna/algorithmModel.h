#pragma once

#include <QtWidgets>
#include "../Templates/algorithmTemplate.h"

class algorithmModel : public QDialog {
	Q_OBJECT
public:
	algorithmModel(parsProblem* atn_problem, QJsonObject* algorithm_obj, parsAlgorithm* algorithm, QWidget *parent = 0);
	~algorithmModel();

public slots:
	void slot_saveAllButton(bool);

private:
	void initLayout();

private:
	parsProblem* _atn_problem;
	parsAlgorithm* _algorithm;
	QJsonObject* _algorithm_obj;
	algorithmTemplate* _algorithm_widget;
	QPushButton* _save_all_button;
	QLabel* _hint;
};
