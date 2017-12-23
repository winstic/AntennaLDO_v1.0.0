﻿#include "tableTemplate.h"

tableTemplate::tableTemplate(QTableWidget* parent) : QTableWidget(parent) {
	//_table = new QTableWidget;
	this->horizontalHeader()->setSectionsClickable(false);
	this->verticalHeader()->setVisible(false);              //setting no vertical header
	this->setFrameShape(QFrame::NoFrame);                   //setting no frame
															  //_table->setShowGrid(false);                               //setting no grid line
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	this->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
	this->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	//this->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit
}

void tableTemplate::insert2table(const int &row, const int &clomun, const QString &item_value) {
	QTableWidgetItem *tableItem = new QTableWidgetItem(item_value);
	tableItem->setTextAlignment(Qt::AlignCenter);
	this->setItem(row, clomun, tableItem);
}