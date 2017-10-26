#include "optimizeWizard.h"
#include "../Utility/parseJson.h"

optimizeWizard::optimizeWizard(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QWizard(parent),
_atn_problem(atn_problem), _obj(obj) {
	_optimize_pre_far = new wizardFreFarField(_atn_problem, _obj, this);
	_optimize_axl = new wizardOptimizeAXL(_atn_problem, _obj, this);
	_optimize_vars = new wizardOptimizeVariables(_atn_problem, _obj, this);
	_optimize_alg = new wizardOptimizeAlg(_atn_problem ,this);
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

QJsonObject* optimizeWizard::getNewestJsonObject() {
	if (_newest_json_obj.isEmpty())
		return nullptr;
	return &_newest_json_obj;
}

void optimizeWizard::accept() {
	QJsonObject frequency_obj, far_fiel_obj, gain_obj, axial_obj, loss_obj, variables_obj;
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
	//wizardOptimizeAXL field
	QJsonObject gal_obj = field("gal_json_obj").toJsonObject();
	gain_obj = parseJson::getSubJsonObj(gal_obj, "GainSetting");
	axial_obj = parseJson::getSubJsonObj(gal_obj, "AxialratioSetting");
	loss_obj = parseJson::getSubJsonObj(gal_obj, "VSWRSetting");
	//wizardOptimizeVariables field
	variables_obj = field("variables_json_obj").toJsonObject();
	//wizardOptimizeAlg field,save all json data after click Finish.
	_newest_json_obj = field("gAlg_json_obj").toJsonObject();		//contain "global" and "algorithm" json object
	
	if (frequency_obj.isEmpty() || far_fiel_obj.isEmpty() || gain_obj.isEmpty() || axial_obj.isEmpty() || 
		loss_obj.isEmpty()  || variables_obj.isEmpty() || _newest_json_obj.isEmpty())
		return;

	_obj.insert("FreSetting", frequency_obj);
	_obj.insert("ThetaPhiStep", far_fiel_obj);
	_obj.insert("GainSetting", gain_obj);
	_obj.insert("AxialratioSetting", axial_obj);
	_obj.insert("VSWRSetting", loss_obj);
	_obj.insert("variables", variables_obj);	

	_newest_json_obj.insert("problem", _obj);	//now _newest_json_obj like {"global":gobj, "problem":pobj, "algorithm":aobj}
	QDialog::accept();
}

optimizeWizard::~optimizeWizard() {}