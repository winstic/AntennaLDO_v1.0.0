#pragma once

#include <QWizardPage>
#include "../Templates/algorithmTemplate.h"

class wizardOptimizeAlg : public QWizardPage{
    Q_OBJECT
public:
    wizardOptimizeAlg(parsProblem* atn_problem, QJsonObject global_obj, QJsonObject algorithm_obj, QWidget *parent = 0);
	~wizardOptimizeAlg();
	QList<iTemplate*> getTemplatesWidget() const;

protected:
	bool isComplete() const override;

private:
	parsProblem* _atn_problem;
	QJsonObject _global_obj;
	QJsonObject _algorithm_obj;
	algorithmTemplate* _algorithm_widget;
};