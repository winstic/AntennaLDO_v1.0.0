#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Utility/global.h"
#include "../Antenna/algorithmWidgetTemplate.h"

class wizardOptimizeAlg : public QWizardPage{
    Q_OBJECT
	Q_PROPERTY(QJsonObject gAlgJsonObj WRITE updateGAlgJsonObj)
public:
    wizardOptimizeAlg(parsProblem* atn_problem, QWidget *parent = 0);
    ~wizardOptimizeAlg(){}
	void initializePage() override;

protected:
	bool isComplete() const override;

public slots:
    void slot_algName(const int index);

signals:
	void signal_gAlgJsonObjChanged();

private:
	void updateGAlgJsonObj();

private:
	parsProblem* _atn_problem;
	algorithmTemplate _temp_widget;
    enum VARCOLUMN{keyFlag = 0, valueFlag};
	QJsonObject _gAlg_obj;
};