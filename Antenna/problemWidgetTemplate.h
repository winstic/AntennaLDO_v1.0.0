#pragma once

#include <QtWidgets>
#include "../Utility/global.h"
#include "mTableWidget.h"

class problemTemplate : public QWidget {
	Q_OBJECT
public:
	problemTemplate(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
	~problemTemplate();
	void initFrequencyWidget(QLayout* layout);
	void initFarFieldWidget(QLayout* layout);
	void initGainWidget(QLayout* layout);
	void initAxialWidget(QLayout* layout);
	void initLossWidget(QLayout* layout);
	void initVariablesRangeWidget(QLayout* layout);
	void initVariablesValueWidget(QLayout* layout);

	//update _obj
	void updateFrequencyJObject();
	void updateFarFieldJObject();
	void updateGainJObject();
	void updateAxialJObject();
	void updateLossJObject();
	void updateVariablesValueJObject();
	void updateVariablesRangeJObject();

private:	
	void initSweepNDPMComBox();
	void initOptimalTypeComBox(QComboBox *combox);
	void initLossTypeComBox(QComboBox *combox);	
	void initSliderSheet(QString& sheet);
	double unitConversion(double sourceData, int preunit, int curunit);
	void initUnitComBo(QComboBox *comb);

public slots:
	void slot_gainChangeOptimaltype(QString);
	void slot_axialChangeOptimaltype(QString);
	void slot_lossChangeType(QString);
	void slot_unitChange4Optimize(QString);
	void slot_LinetextChange(QString);
	void slot_sliderValueChange(QString);
	void slot_unitChange4Design(QString);

public:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	//frequency
	static QLabel* _frequency_low_label;
	static QLabel* _frequency_up_label;
	static QLabel* _frequency_num_label;
	static QLabel* _sweep_type_label;
	static QLabel* _polarization_label;
	static QLineEdit* _frequency_low_edit;
	static QLineEdit* _frequency_up_edit;
	static QLineEdit* _frequency_num_edit;
	static QComboBox* _sweep_type_combox;
	static QComboBox* _polarization_combox;
	//farField
	static QLabel* _theta_low_label;
	static QLabel* _theta_up_label;
	static QLabel* _theta_step_label;
	static QLabel* _phi_low_label;
	static QLabel* _phi_up_label;
	static QLabel* _phi_step_label;
	static QLineEdit* _theta_low_edit;
	static QLineEdit* _theta_up_edit;
	static QLineEdit* _theta_step_edit;
	static QLineEdit* _phi_low_edit;
	static QLineEdit* _phi_up_edit;
	static QLineEdit* _phi_step_edit;
	//gain axial loss
	mTable* _gain_table;
	mTable* _axial_table;
	mTable* _loss_table;
	//model vars value setting
	mTable* _vars_value_table;		//for design model
	QMap<QString, QString> _default_vars;
	//variables setting
	mTable* _vars_range_table;		//for optimize model
	QMap<int, int> _vars_unit;				//save combobox unit
};