#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Utility/global.h"

class wizardDesignVariables : public QWizardPage{
    Q_OBJECT
public:
    wizardDesignVariables(QJsonObject& vars_value_obj, QJsonObject& vars_range_obj, parsProblem* atn_problem, QWidget *parent = 0);
	~wizardDesignVariables();

protected:
	bool isComplete() const override;

public slots:
    void slot_LinetextChange(QString text);
    void slot_sliderValueChange(QString);
    void slot_unitchange(QString);

private:
	void tableStyleSetting();
	bool initDefaultVars();
	bool wizardDialogWithTable();

    void initUnitComBo(QComboBox* comb);    
    QString getSliderSheet();
    
    double unitConversion(double sourceData, int preunit, int curunit);
    void insert2table(const int &row, const int &clomun, const QString &itemValue);

private:
	parsProblem* _atn_problem;
    QJsonObject _vars_value_obj;
	QJsonObject _vars_range_obj;
    QTableWidget* _vars_table;
	QMap<int, int> _vars_unit;				//save combobox unit
    QMap<QString, QString> _default_vars;    
    enum VARCOLUMN{varnote = 0, varvalue, varunit};
	bool _is_all_valid;
};
