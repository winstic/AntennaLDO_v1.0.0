#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Utility/global.h"
#include "../Antenna/problemWidgetTemplate.h"

class wizardOptimizeVariables : public QWizardPage{
    Q_OBJECT
	Q_PROPERTY(QJsonObject varJsonObj WRITE updateJsonObj)
public:
    wizardOptimizeVariables(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
	~wizardOptimizeVariables();
	void initializePage() override;

protected:
	bool isComplete() const override;

signals:
	void signal_varsJsonObjChanged();

private:
	void updateJsonObj();

private:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	QJsonObject _variables_obj;	//register field
	problemTemplate _temp_widget;
    enum VARCOLUMN{varnote = 0, varmin, varmax, varunit};

};