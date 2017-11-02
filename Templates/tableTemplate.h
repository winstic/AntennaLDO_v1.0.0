#pragma once
#include <QtWidgets>

class tableTemplate : public QTableWidget{
	Q_OBJECT
public:
	tableTemplate(QTableWidget* parent = 0);
	~tableTemplate() {};
	void insert2table(const int &row, const int &clomun, const QString &item_value);
public:
	QTableWidget* _table;
};