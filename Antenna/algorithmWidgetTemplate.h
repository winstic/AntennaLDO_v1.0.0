#pragma once

#include <QtWidgets>
#include "../Utility/global.h"
#include "mTableWidget.h"

class algorithmTemplate : public QDialog {
	Q_OBJECT
public:
	algorithmTemplate(parsProblem* atn_problem, QWidget *parent = 0);
	~algorithmTemplate() {}
	void initAlgorithmWidget(QLayout* layout);

public slots:
	void slot_algName(const int index);

private:
	void initAlgComboItem();
	void setCurrAlgorithmParameters();

public:
	parsProblem* _atn_problem;
	parsAlgorithm* _algorithm;
	QLabel* _alg_label;
	QComboBox* _alg_combox;
	mTable* _alg_vars_table;
};