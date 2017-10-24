#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Utility/global.h"

class wizardOptimizeVariables : public QWizardPage{
    Q_OBJECT
	Q_PROPERTY(QJsonObject varJsonObj WRITE updateJsonObj)
public:
    wizardOptimizeVariables(QJsonObject& vars_range_obj, parsProblem* atn_problem, QWidget *parent = 0);
	~wizardOptimizeVariables();
	void initializePage() override;

protected:
	bool isComplete() const override;

signals:
	void signal_varsJsonObjChanged();

public slots:
    void slot_unitchange(QString);

private:
	void tableStyleSetting();
	bool wizardDialogWithTable();
	void initUnitComBo(QComboBox *comb);
	void insert2table(const int &row, const int &clomun, const QString &itemValue);
    double unitConversion(double sourceData, int preunit, int curunit);
	void updateJsonObj();

private:
	parsProblem* _atn_problem;
	QJsonObject _vars_range_obj;
	QJsonObject _variables_obj;	//register field
    QTableWidget* _vars_table;
    QMap<int, int> _vars_unit;	//save combobox unit
    enum VARCOLUMN{varnote = 0, varmin, varmax, varunit};
	bool _is_all_valid;
};