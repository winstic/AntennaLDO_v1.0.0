#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Utility/global.h"
#include "../Antenna/problemWidgetTemplate.h"

class wizardDesignVariables : public QWizardPage{
    Q_OBJECT
public:
    wizardDesignVariables(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
	~wizardDesignVariables();

protected:
	bool isComplete() const override;

private:
	QJsonObject _obj;
	parsProblem* _atn_problem;
	problemTemplate _temp_widget;
};
