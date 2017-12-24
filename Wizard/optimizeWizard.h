#pragma once

#include <QWizard>
#include <QWizardPage>
#include "wizardFreFarField.h"
#include "wizardOptimizeGainAxialLoss.h"
#include "wizardOptimizeVariables.h"
#include "wizardOptimizeAlg.h"

class optimizeWizard : public QWizard{
    Q_OBJECT
public:
    optimizeWizard(parsProblem* atn_problem, QJsonObject* problem_obj, QJsonObject** algorithm_obj, parsAlgorithm** palgorithm, QWidget *parent = 0);
	~optimizeWizard();

protected:
	//after click finish trigger accept() slot function
	void accept() override;

private:
	parsProblem* _atn_problem;
	parsAlgorithm** _algorithm;
	QJsonObject* _problem_obj;
	QJsonObject** _algorithm_obj;
	wizardFreFarField* _optimize_pre_far;
    wizardOptimizeAXL* _optimize_axl;
    wizardOptimizeVariables* _optimize_vars;
    wizardOptimizeAlg* _optimize_alg;
};
