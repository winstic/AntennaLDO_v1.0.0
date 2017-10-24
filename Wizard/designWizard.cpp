#include "designWizard.h"
#include "../Utility/parseJson.h"

designWizard::designWizard(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QWizard(parent),
_atn_problem(atn_problem), _obj(obj){
	_design_performance = new wizardFreFarField(_atn_problem, _obj, this);
	_design_variables = new wizardDesignVariables(_atn_problem, _obj, this);
	//remove help menu
	setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	//this->setWizardStyle(QWizard::ModernStyle);
	setWindowTitle("设计向导");
	setOption(QWizard::NoBackButtonOnStartPage);
	//this->setOption(QWizard::NoCancelButton);
	setButtonText(QWizard::NextButton, "下一步>");
	setButtonText(QWizard::CancelButton, "取消");
	setButtonText(QWizard::FinishButton, "完成");
	addPage(_design_performance);
	addPage(_design_variables);
	setMinimumSize(880, 580);
}

void designWizard::accept(){
	QJsonObject frequency_obj, far_fiel_obj, vars_obj;
	//wizardDesignPerformance field 
	frequency_obj.insert("FreStart", QString("[%1]").arg(field("frequency_low").toString()));
	frequency_obj.insert("FreEnd", QString("[%1]").arg(field("frequency_up").toString()));
	frequency_obj.insert("FreNumber", QString("[%1]").arg(field("frequency_num").toString()));
	frequency_obj.insert("SweepType", QString("[%1]").arg(field("sweep_type").toInt()));
	frequency_obj.insert("PM", QString("[%1]").arg(field("polarization").toInt()));

	far_fiel_obj.insert("ThetaLower", QString("[%1]").arg(field("theta_low").toString()));
	far_fiel_obj.insert("ThetaUpper", QString("[%1]").arg(field("theta_up").toString()));
	far_fiel_obj.insert("ThetaStep", QString("[%1]").arg(field("theta_step").toString()));
	far_fiel_obj.insert("PhiLower", QString("[%1]").arg(field("phi_low").toString()));
	far_fiel_obj.insert("PhiUpper", QString("[%1]").arg(field("phi_up").toString()));
	far_fiel_obj.insert("PhiStep", QString("[%1]").arg(field("phi_step").toString()));
	//!

	//wizardDesignVariables field
	for (QJsonObject::iterator iter = _sub_vars_value_obj.begin(); iter != _sub_vars_value_obj.end(); ++iter) {
		QString var_key = iter.key();
		vars_obj.insert(var_key, field(var_key).toString());
	}
	//!

    if(frequency_obj.isEmpty() || far_fiel_obj.isEmpty() || vars_obj.isEmpty())
        return;

    _obj.insert("FreSetting", frequency_obj);
    _obj.insert("ThetaPhiStep", far_fiel_obj);
	_obj.insert("varsValue", vars_obj);

	QDialog::accept();
}

QJsonObject* designWizard::getNewestJsonobject() {
	if (_obj.isEmpty())
		return nullptr;
	return &_obj;
}

designWizard::~designWizard() {}