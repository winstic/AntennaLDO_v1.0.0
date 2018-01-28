#include "tableTemplate.h"

tableTemplate::tableTemplate(QTableWidget* parent) : QTableWidget(parent) {
	//_table = new QTableWidget;
	QFont ft;
	ft.setBold(true);
	this->horizontalHeader()->setFont(ft);
	
	this->horizontalHeader()->setHighlightSections(false);		//点击表时不对表头行光亮（获取焦点） 
	this->verticalHeader()->setHighlightSections(false);
	this->horizontalHeader()->setSectionsClickable(false);
	//this->verticalHeader()->setVisible(false);              //setting no vertical header	
	//this->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
	//this->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); 
	//this->setStyleSheet("QTableCornerButton::section{background:skyblue;}");
	//this->setFocusPolicy(Qt::NoFocus);
	this->verticalHeader()->setFixedWidth(30);
	//this->setFrameShape(QFrame::NoFrame);                   //setting no frame
															  //_table->setShowGrid(false);                               //setting no grid line
	this->setSelectionBehavior(QAbstractItemView::SelectRows);	
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);     //多行选择
	//this->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	this->setStyleSheet("selection-background-color:skyblue;");   //setting selected background
	//this->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	//this->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit
	//connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slot_selectChanged()));
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

void tableTemplate::setWarningBackGroundColor(QTableWidgetItem* item) {
	item->setBackgroundColor(QColor(255, 0, 0));
}

//矩形排序，快排
int compareRectParameter(rectParameter rect1, rectParameter rect2) {
	//from left to right,
	//rect1 > rect2 return 1
	//rect1 == rect2 return 0
	//rect1 < rect2 return -1
	//double compare
	int ret = 0;
	if (abs(rect1.theta_low - rect2.theta_low) <= 0.000001) {
		if (abs(rect1.theta_up - rect2.theta_up) <= 0.000001) {
			if (abs(rect1.phi_low - rect2.phi_low) <= 0.000001) {
				if (abs(rect1.phi_up - rect2.phi_up) <= 0.000001)
					ret = 0;
				else if (rect1.phi_up < rect2.phi_up)
					ret = -1;
				else
					ret = 1;
			}
			else if (rect1.phi_low < rect2.phi_low)
				ret = -1;
			else
				ret = 1;
		}
		else if (rect1.theta_up < rect2.theta_up)
			ret = -1;
		else 
			ret = 1;
	}
	else if (rect1.theta_low < rect2.theta_low) 
		ret = -1;
	else 
		ret = 1;
	return ret;
}
//quick Sort
void quickSort(QVector<rectParameter>& datas, int start, int finish) {
	rectParameter pivot = datas[start];
	int i = start, j = finish;
	while (i < j) {
		while (i < j && (compareRectParameter(datas[j], pivot) == 1)) --j;
		if (i < j) datas[i++] = datas[j];
		while (i < j && (compareRectParameter(datas[i], pivot) == -1)) ++i;
		if (i < j) datas[j--] = datas[i];
	}
	datas[i] = pivot;
	if (start < i - 1)
		quickSort(datas, start, i - 1);
	if (finish > i + 1)
		quickSort(datas, i + 1, finish);
}

bool tableTemplate::checkRectangleCross(const double theta_step, const double phi_step) {
	//bool is_valid = true;
	QVector<rectParameter> datas;
	int rect_size = this->rowCount();
	for (int i = 0; i < rect_size; ++i) {
		rectParameter tmp;
		tmp.row_number = i;
		tmp.theta_low = this->item(i, 0)->text().trimmed().toDouble();
		tmp.theta_up = this->item(i, 1)->text().trimmed().toDouble();
		tmp.phi_low = this->item(i, 2)->text().trimmed().toDouble();
		tmp.phi_up = this->item(i, 3)->text().trimmed().toDouble();
		datas.append(tmp);
	}
	quickSort(datas, 0, rect_size - 1);
	for (int j = 0; j < rect_size - 1; ++j) {
		rectParameter rect1 = datas[j];
		for (int k = j + 1; k < rect_size; ++k) {
			rectParameter rect2 = datas[k];
			if (abs(rect1.theta_up - rect2.theta_low) <= 0.000001) {
				if ((rect1.phi_up > rect2.phi_low) || (abs(rect1.phi_up - rect2.phi_low) <= 0.000001)) {
					//两个矩形上下边界有重叠部分,将下一个矩形的上边界下移step
					rect2.theta_low += theta_step;
					//调节后如果新矩形矛盾，则校验不通过
					if (rect2.theta_low > rect2.theta_up) {
						setWarningRows(datas[j].row_number, datas[k].row_number);
						return false;
					}
				}
			}
			else if (rect1.theta_up > rect2.theta_low) {
				if (abs(rect1.phi_up - rect2.phi_low) <= 0.000001) {
					//两个矩形左右边界有重叠部分,将下一个矩形的左边界右移step
					rect2.phi_low += phi_step;
					//调节后如果新矩形矛盾，则校验不通过
					if (rect2.phi_low > rect2.phi_up) {
						setWarningRows(datas[j].row_number, datas[k].row_number);
						return false;
					}
				}
				else if (rect1.phi_up > rect2.phi_low) {
					//两个矩形交叉，校验不通过
					setWarningRows(datas[j].row_number, datas[k].row_number);
					return false;
				}
			}
			else
				break;
		}
	}
	return true;
}
void tableTemplate::setWarningRows(const int row1, const int row2) {
	for (int i = 0; i < 4; ++i) {
		setWarningBackGroundColor(this->item(row1, i));
		setWarningBackGroundColor(this->item(row2, i));
	}
}


/*void tableTemplate::slot_selectChanged() {
	//点击表格之外时取消选中
	int i = this->selectedItems().size();
	if (this->selectedItems().size() == 0)
		this->clearSelection();
}*/