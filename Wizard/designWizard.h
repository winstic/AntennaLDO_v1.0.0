#pragma once

#include <QWizard>
#include "wizardFreFarField.h"
#include "wizardDesignVariables.h"

class designWizard : public QWizard{
    Q_OBJECT

public:
    designWizard(parsProblem* atn_problem, QJsonObject& obj, QWidget* parent = 0);
	~designWizard();

protected:
	//after click finish trigger accept() slot function
	void accept() override;

private:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	wizardFreFarField* _design_performance;
    wizardDesignVariables* _design_variables;    
};