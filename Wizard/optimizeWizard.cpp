#include "optimizeWizard.h"
#include "../Utility/parseJson.h"
#include "../Templates/iTemplate.h"

optimizeWizard::optimizeWizard(parsProblem* atn_problem, QJsonObject& problem_obj, QJsonObject& algorithm_obj, parsAlgorithm* palgorithm, QWidget *parent)
	: QWizard(parent),_atn_problem(atn_problem), _problem_obj(problem_obj), _algorithm_obj(algorithm_obj), _algorithm(palgorithm) {
	_optimize_pre_far = new wizardFreFarField(_atn_problem, _problem_obj, this);
	_optimize_axl = new wizardOptimizeAXL(_atn_problem, _problem_obj, this);
	_optimize_vars = new wizardOptimizeVariables(_atn_problem, _problem_obj, this);
	_optimize_alg = new wizardOptimizeAlg(_atn_problem, _algorithm_obj, _algorithm, this);
	//remove help menu
	setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setWindowTitle("优化向导");
	setOption(QWizard::NoBackButtonOnStartPage);
	setButtonText(QWizard::NextButton, "下一步>");
	setButtonText(QWizard::CancelButton, "取消");
	setButtonText(QWizard::FinishButton, "完成");
	setMinimumSize(880, 580);
	
	addPage(_optimize_pre_far);
	addPage(_optimize_axl);
	addPage(_optimize_vars);
	addPage(_optimize_alg);
}

void optimizeWizard::accept() {
	for (iTemplate* iter : _optimize_pre_far->getTemplatesWidget(), _optimize_axl->getTemplatesWidget(),
					_optimize_vars->getTemplatesWidget(), _optimize_alg->getTemplatesWidget())
		iter->updateJObj();
	QDialog::accept();
}

optimizeWizard::~optimizeWizard() {}