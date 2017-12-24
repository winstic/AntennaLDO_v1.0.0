#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "wizardDesignVariables.h"


wizardDesignVariables::wizardDesignVariables(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent): QWizardPage(parent), 
_atn_problem(atn_problem), _obj(obj){

	setTitle("几何结构模型设置");
	_vars_value_widget = new varsDefaultValueTemplate(atn_problem, _obj);
    //layout
	QLayout* layout = _vars_value_widget->getLayout();
	layout->setContentsMargins(2, 20, 10, 2);
	setLayout(layout);
}

bool wizardDesignVariables::isComplete() const {
	return true;
}

QList<iTemplate*> wizardDesignVariables::getTemplatesWidget() const {
	return QList<iTemplate*> {_vars_value_widget};
}

/*wizardDesignVariables::~wizardDesignVariables(){
	delete _vars_value_widget;
	_vars_value_widget = nullptr;
}*/