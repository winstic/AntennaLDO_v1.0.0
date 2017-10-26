#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include <QFile>

class wizardFreFarField : public QWizardPage{
    Q_OBJECT
public:
	wizardFreFarField(QJsonObject& frequency_obj, QJsonObject& far_field_obj, QWidget *parent = 0);
    ~wizardFreFarField();

protected:
	bool isComplete() const override;

private:
	void initLayout();
	void initComBoBox();
	void setRegexp();
    void frequencySetting();
    void farFieldSetting();

private:
    QJsonObject _frequency_obj;
	QJsonObject _far_field_obj;
    //frequencySetting
    QLabel* _frequency_low_label;
    QLabel* _frequency_up_label;
    QLabel* _frequency_num_label;
    QLabel* _sweep_type_label;
    QLabel* _polarization_label;    
    QLineEdit* _frequency_low_edit;
    QLineEdit* _frequency_up_edit;
    QLineEdit* _frequency_num_edit;
    QComboBox* _sweep_type_combox;
    QComboBox* _polarization_combox;
    QGroupBox* _frequency_group_box;

    //farFieldSetting
    QLabel* _theta_low_label;
    QLabel* _theta_up_label;
    QLabel* _theta_step_label;
    QLabel* _phi_low_label;
    QLabel* _phi_up_label;
    QLabel* _phi_step_label;
    QLineEdit* _theta_low_edit;
    QLineEdit* _theta_up_edit;
    QLineEdit* _theta_step_edit;
    QLineEdit* _phi_low_edit;
    QLineEdit* _phi_up_edit;
    QLineEdit* _phi_step_edit;
    QGroupBox* _far_field_group_box;
};