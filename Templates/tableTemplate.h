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

tableTemplate::tableTemplate(QTableWidget* parent) : QTableWidget(parent){
	_table = new QTableWidget(this);
	_table->horizontalHeader()->setSectionsClickable(false);
	_table->verticalHeader()->setVisible(false);              //setting no vertical header
	_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	//_table->setShowGrid(false);                               //setting no grid line
	_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
	_table->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit
}

void tableTemplate::insert2table(const int &row, const int &clomun, const QString &item_value) {
	QTableWidgetItem *tableItem = new QTableWidgetItem(item_value);
	tableItem->setTextAlignment(Qt::AlignCenter);
	_table->setItem(row, clomun, tableItem);
}