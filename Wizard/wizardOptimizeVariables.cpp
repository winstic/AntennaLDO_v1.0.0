#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "wizardOptimizeVariables.h"

wizardOptimizeVariables::wizardOptimizeVariables(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent)
	: QWizardPage(parent), _atn_problem(atn_problem), _obj(obj){
	setTitle("几何结构变量范围设置");
	_variables_widget = new variablesTemplate(_atn_problem, _obj);
	//layout
	QLayout* layout = _variables_widget->getLayout();
	layout->setContentsMargins(2, 20, 10, 2);
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