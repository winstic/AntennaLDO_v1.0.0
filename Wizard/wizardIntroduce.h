#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Utility/global.h"

class wizardIntroduce :public QWizardPage{
    Q_OBJECT
public:
    wizardIntroduce(parsProblem* atn_problem, QWidget* parent = 0);
    ~wizardIntroduce();

protected:
	bool isComplete() const override;

private:
	void initIntroduceLayout();

private:
	parsProblem* _atn_problem;
    QTextEdit* _atn_info_text;
    QLabel* _atn_image_label;
};