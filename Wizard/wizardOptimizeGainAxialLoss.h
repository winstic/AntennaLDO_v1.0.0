#pragma once

#include <QWizardPage>
#include "../Templates/gainTemplate.h"
#include "../Templates/axialTemplate.h"
#include "../Templates/lossTemplate.h"

class wizardOptimizeAXL : public QWizardPage{
    Q_OBJECT
public:
    wizardOptimizeAXL(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
	~wizardOptimizeAXL();
	QList<iTemplate*> getTemplatesWidget() const;

protected:
	bool isComplete() const override;

private:
	QJsonObject _obj;
	parsProblem* _atn_problem;
	gainTemplate* _gain_widget;
	axialTemplate* _axial_widget;
	lossTemplate* _loss_widget;
};