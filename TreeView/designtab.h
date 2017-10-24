#pragma once

#include <QtWidgets>

class designTab : public QDialog{
    Q_OBJECT
public:
    designTab(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
    ~designTab(){}

public slots:
    void slot_saveAllButton(bool);
    void slot_LinetextChange(QString text);
    void slot_sliderValueChange(QString);
    void slot_unitchange(QString);

private:
    //!first tab widget function
    void initSweepNDPMComBox();
	void setRegexp();
    void frequencySetting();
    void farFieldSetting();
    void initFirstTabLayout();
    //!
    //!second tab widget function
    void tableStyleSetting();
    void initDefaultVars();
    void initUnitComBo(QComboBox* comb);
    void initVariablesTable();
    QString getSliderSheet();
    double unitConversion(double sourceData, int preunit, int curunit);
    void insert2table(const int &row, const int &clomun, const QString &itemValue);
    //!

private:
	parsProblem* _atn_problem;
    QTabWidget* _tab_widget;
    QWidget* _first_tab;
    QWidget* _second_tab;
    QJsonObject _obj;
	QPushButton* _save_all_button;

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

    //model variables setting
	QTableWidget* _vars_table;
	QMap<int, int> _vars_unit;				//save combobox unit
	QMap<QString, QString> _default_vars;
	enum VARCOLUMN { varnote = 0, varvalue, varunit };
};