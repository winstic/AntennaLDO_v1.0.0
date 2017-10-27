#include "../Utility/macrodefined.h"
#include "wizardOptimizeVariables.h"

wizardOptimizeVariables::wizardOptimizeVariables(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent)
	: QWizardPage(parent), _atn_problem(atn_problem), _obj(obj){
	setTitle(tr("优化变量设置"));
	setSubTitle(tr("设置需要优化的变量参数"));
	_variables_widget = new variablesTemplate(_atn_problem, _obj);
	//layout
	QLayout* layout = _variables_widget->getLayout();
	setLayout(layout);
}

bool wizardOptimizeVariables::isComplete() const {
	return true;
}

QList<iTemplate*> wizardOptimizeVariables::getTemplatesWidget() const {
	return QList<iTemplate*> {_variables_widget};
}

wizardOptimizeVariables::~wizardOptimizeVariables() {
	delete _variables_widget;
	_variables_widget = nullptr;
}