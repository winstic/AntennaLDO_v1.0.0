#pragma once

#include <QWizardPage>
#include "../Templates/varsDefaultValueTemplate.h"

class wizardDesignVariables : public QWizardPage{
    Q_OBJECT
public:
    wizardDesignVariables(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent = 0);
	//~wizardDesignVariables();
	QList<iTemplate*> getTemplatesWidget() const;
protected:
	bool isComplete() const override;

private:
	QJsonObject* _obj;
	parsProblem* _atn_problem;
	varsDefaultValueTemplate* _vars_value_widget;
};
