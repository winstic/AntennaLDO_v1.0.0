#pragma once

#include <QWizardPage>
#include "../Templates/frequencyTemplate.h"
#include "../Templates/thetaPhiTemplate.h"

class wizardFreFarField : public QWizardPage{
    Q_OBJECT
public:
	wizardFreFarField(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent = 0);
    //~wizardFreFarField();
	QList<iTemplate*> getTemplatesWidget() const;

protected:
	bool isComplete() const override;

private:
	void initLayout();

private:
	QJsonObject* _obj;
	parsProblem* _atn_problem;
	frequencyTemplate* _frequency_widget;
	thetaPhiTemplate* _theta_phi_widget;
	QLabel* _hint;
};