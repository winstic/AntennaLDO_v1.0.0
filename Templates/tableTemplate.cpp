#include "tableTemplate.h"

tableTemplate::tableTemplate(QTableWidget* parent) : QTableWidget(parent) {
	//_table = new QTableWidget;
	this->horizontalHeader()->setSectionsClickable(false);
	//this->verticalHeader()->setVisible(false);              //setting no vertical header	
	this->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
	this->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); 
	this->setStyleSheet("QTableCornerButton::section{background:skyblue;}");
	//this->setFocusPolicy(Qt::NoFocus);
	this->verticalHeader()->setFixedWidth(30);
	//this->setFrameShape(QFrame::NoFrame);                   //setting no frame
															  //_table->setShowGrid(false);                               //setting no grid line
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	this->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
	//this->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	//this->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slot_selectChanged()));
}

void tableTemplate::insert2table(const int &row, const int &clomun, const QString &item_value) {
	QTableWidgetItem *tableItem = new QTableWidgetItem(item_value);
	tableItem->setTextAlignment(Qt::AlignCenter);
	this->setItem(row, clomun, tableItem);
}

void tableTemplate::setCanEdit(QTableWidgetItem* item) {
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	item->setBackgroundColor(QColor(255, 255, 255));
}
void tableTemplate::setCannotEdit(QTableWidgetItem* item) {
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	item->setBackgroundColor(QColor(125, 125, 125));
}

void tableTemplate::slot_selectChanged() {
	//点击表格之外时取消选中
	int i = this->selectedItems().size();
	if (this->selectedItems().size() == 0)
		this->clearSelection();
}