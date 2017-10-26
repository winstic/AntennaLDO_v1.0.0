#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Utility/global.h"
#include "../Antenna/problemWidgetTemplate.h"

class wizardOptimizeVariables : public QWizardPage{
    Q_OBJECT
public:
    wizardOptimizeVariables(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
	~wizardOptimizeVariables();

protected:
	bool isComplete() const override;

private:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	problemTemplate _temp_widget;
};