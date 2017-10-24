#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include <QFile>
#include "../Antenna/problemWidgetTemplate.h"

class wizardFreFarField : public QWizardPage{
    Q_OBJECT
public:
	wizardFreFarField(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
    ~wizardFreFarField();

protected:
	bool isComplete() const override;

private:
	QJsonObject _obj;
	parsProblem* _atn_problem;
	problemTemplate _temp_widget;
};