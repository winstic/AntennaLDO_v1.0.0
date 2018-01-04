#pragma execution_character_set("utf-8")
#include "iTemplate.h"

iTemplate::iTemplate(QWidget* parent) : QWidget(parent), checkInfo(new errorInfo){}

void iTemplate::initOptimalTypeComBox(QComboBox *combox) {
	combox->addItem("max");
	combox->addItem("min");
	combox->addItem("delta");
	combox->addItem("None");
	combox->setCurrentIndex(3);
}

void iTemplate::initUnitComBo(QComboBox *comb) {
	comb->addItem("λ");
	comb->setItemData(0, MARK_UNIT_LAMBDA, ROLE_MARK_UNIT);
	comb->addItem("mm");
	comb->setItemData(1, MARK_UNIT_MM, ROLE_MARK_UNIT);
	comb->addItem("cm");
	comb->setItemData(2, MARK_UNIT_CM, ROLE_MARK_UNIT);	
	comb->addItem("m");
	comb->setItemData(3, MARK_UNIT_M, ROLE_MARK_UNIT);
	comb->addItem("km");
	comb->setItemData(4, MARK_UNIT_KM, ROLE_MARK_UNIT);
	comb->setCurrentIndex(3);
}

void iTemplate::initAngleComboBox(QComboBox* comb, const double low, const double up, const double step) const{
	if (step >= -0.000001 && step <= 0.000001) {
		comb->addItem(QString::number(low));
		return;
	}
	double dangle = 0.0;
	for (dangle = low; dangle < up; dangle += step) {
		comb->addItem(QString::number(dangle));
	}
	comb->addItem(QString::number(up));
	//comb->setCurrentIndex(0);
}

double iTemplate::unitConversion(double source_data, int pre_unit, int curr_unit, double max_frequency) {
	double result = 0.0;
	if (max_frequency >= -0.000001 && max_frequency <= 0.000001) {
		//unit conversion without lambda
		result = source_data * qPow(10, (pre_unit - curr_unit));
	}
	else {
		//unit conversion with lambda
		//c=lambda*f; lambda(MHz), lambda(m)
		double lambda = (3.0 * qPow(10, 8)) / (max_frequency * qPow(10, 6));
		int unit_diff = pre_unit - curr_unit;
		if (pre_unit == MARK_UNIT_LAMBDA) {
			//plus MARK_UNIT_M means conversion 'lambda' to 'm'
			result = source_data * lambda * qPow(10, (pre_unit - curr_unit + MARK_UNIT_M));
		}
		else {
			//curr_unit == MARK_UNIT_LAMBDA
			result = source_data / lambda * qPow(10, (pre_unit - curr_unit - MARK_UNIT_M));
		}
	}
	return result;
}

//setting regular expression
QRegExpValidator* iTemplate::getFloatReg() {
	return new QRegExpValidator(QRegExp("^(-?\\d+)(\\.\\d+)?$"));
}

QRegExpValidator* iTemplate::getNonNegativeFloatReg() {
	//return new QRegExpValidator(QRegExp("^(\\d+)(\\.\\d+)?$"));
	return new QRegExpValidator(QRegExp("^\\d+(\\.\\d+)?$"));
}

QRegExpValidator* iTemplate::getPositiveIntReg() {
	return new QRegExpValidator(QRegExp("^[1-9][0-9]*$"));
	//return new QRegExpValidator(QRegExp("^[0-9]*[1-9][0-9]*$"));
}

QRegExpValidator* iTemplate::getAngleReg() {
	//return new QRegExpValidator(QRegExp("^-?(180|1?[0-7]?\\d(\\.\\d+)?)$"));
	return new QRegExpValidator(QRegExp("^-?(((([1-9]?[0-9])|(1[0-7][0-9]))(\\.\\d+)?)|180(\\.[0]+)?)$"));
}
