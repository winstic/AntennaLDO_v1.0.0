#pragma once
#include <QtWidgets>
#include "../Utility/macrodefined.h"

class iTemplate : public QWidget {
	Q_OBJECT
public:
	iTemplate(QWidget* parent = 0);
	~iTemplate() {};
	virtual void updateJObj() {};
	virtual QLayout* getLayout() {};
	void initOptimalTypeComBox(QComboBox *combox);
	void initUnitComBo(QComboBox *comb);
	double unitConversion(double source_data, int pre_unit, int curr_unit);
};


void iTemplate::initOptimalTypeComBox(QComboBox *combox) {
	combox->addItem("max");
	combox->addItem("min");
	combox->addItem("delta");
	combox->addItem("None");
	combox->setCurrentIndex(3);
}

void iTemplate::initUnitComBo(QComboBox *comb) {
	comb->addItem("mm");
	comb->setItemData(0, MARK_UNIT_MM, ROLE_MARK_UNIT);
	comb->addItem("cm");
	comb->setItemData(1, MARK_UNIT_CM, ROLE_MARK_UNIT);
	comb->addItem("��");
	comb->setItemData(2, MARK_UNIT_LAMBDA, ROLE_MARK_UNIT);
	comb->addItem("m");
	comb->setItemData(3, MARK_UNIT_M, ROLE_MARK_UNIT);
	comb->addItem("km");
	comb->setItemData(4, MARK_UNIT_KM, ROLE_MARK_UNIT);
	comb->setCurrentIndex(3);
}

double iTemplate::unitConversion(double source_data, int pre_unit, int curr_unit) {
	return source_data * qPow(10, pre_unit - curr_unit);
}