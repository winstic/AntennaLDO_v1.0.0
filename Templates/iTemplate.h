#pragma once
#include <QtWidgets>
#include "../Utility/macrodefined.h"

//0 : normal; 1: null input; 2:invalid input; 3:others
struct errorInfo {
	int code;
	QString message;
	errorInfo() : code(0), message("") {}
};

class iTemplate : public QWidget {
	Q_OBJECT
public:
	iTemplate(QWidget* parent = 0);
	virtual ~iTemplate() {};
	virtual void updateJObj() {};
	virtual QLayout* getLayout() { return nullptr; }
	virtual bool checkInputValid() { return true; }
	void initOptimalTypeComBox(QComboBox *combox);
	void initUnitComBo(QComboBox *comb);
	double unitConversion(double source_data, int pre_unit, int curr_unit, double max_frequency = 0.0);
	QRegExpValidator* getFloatReg();
	QRegExpValidator* getNonNegativeFloatReg();
	QRegExpValidator* getPositiveIntReg();
	//[-180,180]
	QRegExpValidator* getAngleReg();
	errorInfo* checkInfo;
};