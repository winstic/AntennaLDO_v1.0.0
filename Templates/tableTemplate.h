#pragma once
#include <QtWidgets>

//theta、phi设置所构成的矩形类型（table中每一行即为一个矩形）
struct rectParameter {
	double theta_low;
	double theta_up;
	double phi_low;
	double phi_up;
	int row_number;		//排序前在原始表格中的行号
};

class tableTemplate : public QTableWidget{
	Q_OBJECT
public:
	tableTemplate(QTableWidget* parent = 0);
	~tableTemplate() {};
	void insert2table(const int &row, const int &clomun, const QString &item_value);
	void setCanEdit(QTableWidgetItem* item);
	void setCannotEdit(QTableWidgetItem* item);
	void setWarningBackGroundColor(QTableWidgetItem* item);
	bool checkRectangleCross(const double theta_step, const double phi_step);

private:
	void setWarningRows(const int row1, const int row2);

//public slots:
	//void slot_selectChanged();
};