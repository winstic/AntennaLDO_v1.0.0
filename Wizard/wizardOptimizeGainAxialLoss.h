#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Antenna/problemWidgetTemplate.h"

class wizardOptimizeAXL : public QWizardPage{
    Q_OBJECT
public:
    wizardOptimizeAXL(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
    ~wizardOptimizeAXL(){}

protected:
	bool isComplete() const override;

private:
	QJsonObject _obj;
	parsProblem* _atn_problem;
	problemTemplate _temp_widget;
};