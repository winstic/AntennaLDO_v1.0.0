#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Antenna/problemWidgetTemplate.h"

class wizardOptimizeAXL : public QWizardPage{
    Q_OBJECT
	Q_PROPERTY(QJsonObject galJsonObj WRITE updateJsonObj)
public:
    wizardOptimizeAXL(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
    ~wizardOptimizeAXL(){}
	void initializePage() override;

protected:
	bool isComplete() const override;


signals:
	void signal_galJsonObjChanged();

private:
	QJsonObject _obj;
	parsProblem* _atn_problem;
	problemTemplate _temp_widget;
	QJsonObject _gal_obj;	//register field
};