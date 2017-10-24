#pragma once

#include <QLineEdit>
#include <QWizard>
#include <QWizardPage>
#include <QDir>
#include <QFile>
#include "../Utility/global.h"
#include "wizardIntroduce.h"
#include "wizardAddSetting.h"

class projectWizard :public QWizard{
    Q_OBJECT

public:
    projectWizard(parsProblem* antenna_problem, QWidget *parent = 0);
	~projectWizard();

protected:
	//after click finish trigger accept() slot function
    void accept() override;

private:
    wizardIntroduce* _introduce;
    wizardAddSetting* _add_setting;
	parsProblem* _antenna_problem;
};
