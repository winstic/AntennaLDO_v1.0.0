#pragma once

#include <QtWidgets>
#include <QWizardPage>

class wizardOptimizeAXL : public QWizardPage{
    Q_OBJECT
	Q_PROPERTY(QJsonObject galJsonObj WRITE updateJsonObj)
public:
    wizardOptimizeAXL(QJsonObject& gain_obj, QJsonObject& axial_obj, QJsonObject& loss_obj, QWidget *parent = 0);
    ~wizardOptimizeAXL(){}
	void initializePage() override;

protected:
	bool isComplete() const override;

public slots:
    void slot_gainChangeOptimaltype(QString);
    void slot_axialChangeOptimaltype(QString);
    void slot_lossChangeType(QString pos);

signals:
	void signal_galJsonObjChanged();

private:
    bool gainSetting();
    bool axialSetting();
    bool lossSetting();
    void initLayout();
    void initOptimalTypeComBox(QComboBox *combox);
    void initLossTypeComBox(QComboBox *combox);
    void insert2table(QTableWidget *table, const int &row, const int &clomun, const QString &itemValue);
	void updateJsonObj();

private:
    QJsonObject _gain_obj;
	QJsonObject _axial_obj;
	QJsonObject _loss_obj;
    QTableWidget* _gain_table;
    QTableWidget* _axial_table;
    QTableWidget* _loss_table;
    enum GACOLUMN{cthetalower = 0, cthetaupper, cphilower, cphiupper, coptimaltype, cdelta, cobjvalue, cweight};
    enum LOSSCOLUMN{cz0real = 0, cz0imag, closstype, clossoptimaltype, cdeltareal, cdeltaimag, cobjreal, cobjimag, clossweight};
	QJsonObject _gal_obj;	//register field
};