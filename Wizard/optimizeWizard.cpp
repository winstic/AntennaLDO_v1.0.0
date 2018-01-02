#pragma execution_character_set("utf-8")
#include "optimizeWizard.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Templates/iTemplate.h"

optimizeWizard::optimizeWizard(parsProblem* atn_problem, QJsonObject* problem_obj, QJsonObject** algorithm_obj, parsAlgorithm** palgorithm, QWidget *parent)
	: QWizard(parent),_atn_problem(atn_problem), _problem_obj(problem_obj), _algorithm_obj(algorithm_obj), _algorithm(palgorithm) {
	setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setWindowTitle("优化向导");

	_optimize_pre_far = new wizardFreFarField(_atn_problem, _problem_obj);
	_optimize_axl = new wizardOptimizeAX(_atn_problem, _problem_obj);
	_optimize_vars = new wizardOptimizeVariables(_atn_problem, _problem_obj);
	_optimize_alg = new wizardOptimizeAlg(_atn_problem, _algorithm_obj, _algorithm);	
	
	setWizardStyle(QWizard::ModernStyle);
	setButtonText(QWizard::BackButton, "<上一步");
	setButtonText(QWizard::NextButton, "下一步>");
	setButtonText(QWizard::CancelButton, "取消");
	setButtonText(QWizard::FinishButton, "完成");
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	
	addPage(_optimize_pre_far);
	addPage(_optimize_axl);
	addPage(_optimize_vars);
	addPage(_optimize_alg);
}

void optimizeWizard::accept() {
	QList<iTemplate*> design_list;
	design_list.append(_optimize_pre_far->getTemplatesWidget());
	design_list.append(_optimize_axl->getTemplatesWidget());
	design_list.append(_optimize_vars->getTemplatesWidget());
	design_list.append(_optimize_alg->getTemplatesWidget());
	for (iTemplate* iter : design_list)
		iter->updateJObj();
	QDialog::accept();
}

optimizeWizard::~optimizeWizard() {}