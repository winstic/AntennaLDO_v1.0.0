#pragma once

#include <QtWidgets>
#include "../Utility/global.h"

class optimizeTab : public QDialog{
    Q_OBJECT
public:
    optimizeTab(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
    ~optimizeTab(){}

public slots:
    void slot_saveAllButton(bool);
    void slot_gainChangeOptimaltype(QString);
    void slot_axialChangeOptimaltype(QString);
    void slot_lossChangeType(QString);
    void slot_unitchange(QString);
    void slot_algName(const int index);
    void slot_singleCheckBoxStateChange(const int state);
    void slot_multiCheckBoxStateChange(const int state);
    void slot_addNodeButton(/*bool check*/);
    void slot_delNodeButton(/*bool check*/);

private:
    //!first tab widget function
	void initSweepNDPMComBox();
	void setRegexp();
    void frequencySetting();
    void farFieldSetting();
    void initFirstTabLayout();
    //!
    //!second tab widget function
    void gainSetting();
    void axialSetting();
    void lossSetting();
    void initOptimalTypeComBox(QComboBox *combox);
    void initLossTypeComBox(QComboBox *combox);
    void initSecondTabLayout();
    //!

    //!third tab widget function
    void wizardDialog();
    double unitConversion(double sourceData, int preunit, int curunit);
    void initUnitComBo(QComboBox *comb);
    //!

    //!fourth tab wodget function
	void initAlgVarsTableStyle();
	void initNodesTableStyle();
	void initAlgComboItem();
    void setCurrAlgorithmParameters();
    void initFourthTabLayout();
    //!

    void insert2table(QTableWidget *table, const int &row, const int &clomun, const QString &itemValue);

private:
	parsProblem* _atn_problem;
    QTabWidget* _tab_widget;
    QWidget* _first_tab;
    QWidget* _second_tab;
    QWidget* _third_tab;
    QWidget* _fourth_tab;
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

    //gain axial loss setting
	QTableWidget* _gain_table;
	QTableWidget* _axial_table;
	QTableWidget* _loss_table;
    enum GACOLUMN{cthetalower = 0, cthetaupper, cphilower, cphiupper, coptimaltype, cdelta, cobjvalue, cweight};
    enum LOSSCOLUMN{cz0real = 0, cz0imag, closstype, clossoptimaltype, cdeltareal, cdeltaimag, cobjreal, cobjimag, clossweight};

    //variables setting
    QTableWidget* _vars_table;
    QMap<int, int> _vars_unit;
    enum VARCOLUMN{varnote = 0, varmin, varmax, varunit};

    //algorithm setting
	parsProblem* _atn_problem;
	parsAlgorithm* _algorithm;
	QLabel* _alg_label;
	QComboBox* _alg_combox;
	QTableWidget* _alg_vars_table;
	enum VARCOLUMN { keyFlag = 0, valueFlag };
	//threads
	QLabel* _thread_label;
	QLineEdit* _thread_edit;
	QCheckBox* _single_thread_checkbox;
	QCheckBox* _multi_thread_checkbox;
	//nodes
	QLabel* _node_label;
	QLabel* _core_label;
	QTableWidget* _node_vars_table;
	QLineEdit* _node_edit;
	QLineEdit* _core_edit;
	enum NODESCOLUMN { nodeFlag = 0, coreFlag };
	//button
	QPushButton* _add_button;
	QPushButton* _del_button;
	//group
	QGroupBox* _single_group;
	QGroupBox* _multi_group;
};