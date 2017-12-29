#pragma once
#include <QtWidgets>
#include "../Utility/macrodefined.h"

class iTemplate : public QWidget {
	Q_OBJECT
public:
	iTemplate(QWidget* parent = 0);
	virtual ~iTemplate() {};
	virtual void updateJObj() {};
	virtual QLayout* getLayout() { return nullptr; }
	void initOptimalTypeComBox(QComboBox *combox);
	void initUnitComBo(QComboBox *comb);
	double unitConversion(double source_data, int pre_unit, int curr_unit, double max_frequency = 0.0);
	QRegExpValidator* getNonNegativeFloatReg();
	QRegExpValidator* getPositiveIntReg();
	//[-180,180]
	QRegExpValidator* getAngleReg();
};