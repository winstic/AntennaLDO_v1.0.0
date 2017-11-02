#pragma once

#include <QWizardPage>
#include "../Templates/variablesTemplate.h"

class wizardOptimizeVariables : public QWizardPage{
    Q_OBJECT
public:
    wizardOptimizeVariables(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
	~wizardOptimizeVariables();
	QList<iTemplate*> getTemplatesWidget() const;

protected:
	bool isComplete() const override;

private:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	variablesTemplate* _variables_widget;
};