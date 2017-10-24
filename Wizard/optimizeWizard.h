#pragma once

#include <QWizard>
#include <QWizardPage>
#include "wizardFreFarField.h"
#include "wizardOptimizeGainAxialLoss.h"
#include "wizardOptimizeVariables.h"
#include "wizardOptimizeAlg.h"

class optimizeWizard : public QWizard{
    Q_OBJECT
public:
    optimizeWizard(QString problem_json_file, parsProblem* atn_problem, QWidget *parent = 0);
	~optimizeWizard();
	QJsonObject* getNewestJsonObject();

protected:
	//after click finish trigger accept() slot function
	void accept() override;

private:
	QString _problem_json_file;
	parsProblem* _atn_problem;
	QJsonObject _obj;
	QJsonObject _sub_frequency_obj;
	QJsonObject _sub_far_field_obj;
	QJsonObject _sub_gain_obj;
	QJsonObject _sub_axial_obj;
	QJsonObject _sub_loss_obj;
	QJsonObject _sub_vars_range_obj;

	wizardFreFarField* _optimize_pre_far;
    wizardOptimizeAXL* _optimize_axl;
    wizardOptimizeVariables* _optimize_vars;
    wizardOptimizeAlg* _optimize_alg;

	QJsonObject _newest_json_obj;  //save all json data after click Finish.
};
