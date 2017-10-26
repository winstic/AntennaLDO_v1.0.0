#include "../Utility/macrodefined.h"
#include "wizardOptimizeVariables.h"

wizardOptimizeVariables::wizardOptimizeVariables(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent)
	: QWizardPage(parent), _atn_problem(atn_problem), _obj(obj), _temp_widget(atn_problem, obj) {

	setTitle(tr("优化变量设置"));
	setSubTitle(tr("设置需要优化的变量参数"));

	//layout
	QHBoxLayout layout;
	_temp_widget.initVariablesRangeWidget(&layout);
	setLayout(&layout);
}

bool wizardOptimizeVariables::isComplete() const {
	return true;
}

wizardOptimizeVariables::~wizardOptimizeVariables() {}