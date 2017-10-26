#include "../Utility/macrodefined.h"
#include "wizardDesignVariables.h"


wizardDesignVariables::wizardDesignVariables(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent): QWizardPage(parent), 
_atn_problem(atn_problem), _obj(obj), _temp_widget(atn_problem, obj){

	setTitle(tr("模型设置"));
	setSubTitle(tr("模型设置"));

    //layout
	QHBoxLayout layout;
	_temp_widget.initVariablesValueWidget(&layout);
	setLayout(&layout);

	registerField(QString("%1*").arg(var_key), value_edit);
}

bool wizardDesignVariables::isComplete() const {
	return true;
}

wizardDesignVariables::~wizardDesignVariables(){}