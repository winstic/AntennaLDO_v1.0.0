#pragma execution_character_set("utf-8")
#include "designWizard.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Templates/iTemplate.h"

designWizard::designWizard(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent) : QWizard(parent),
_atn_problem(atn_problem), _obj(obj){
	setWindowTitle("设计向导");
	setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);

	_design_performance = new wizardFreFarField(_atn_problem, _obj);
	_design_variables = new wizardDesignVariables(_atn_problem, _obj);	

	setWizardStyle(QWizard::ClassicStyle);
	setButtonText(QWizard::BackButton, "<上一步");
	setButtonText(QWizard::NextButton, "下一步>");
	setButtonText(QWizard::CancelButton, "取消");
	setButtonText(QWizard::FinishButton, "完成");
	addPage(_design_performance);
	addPage(_design_variables);
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
}

void designWizard::accept(){
	QList<iTemplate*> design_list;
	design_list.append(_design_performance->getTemplatesWidget());
	design_list.append(_design_variables->getTemplatesWidget());
	for (iTemplate* iter : design_list)
		iter->updateJObj();
	QDialog::accept();
}

/*designWizard::~designWizard() {}*/