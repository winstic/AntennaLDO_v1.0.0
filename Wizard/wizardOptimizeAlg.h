#pragma once

#include <QtWidgets>
#include <QWizardPage>
#include "../Utility/global.h"

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
    void slot_singleCheckBoxStateChange(const int state);
    void slot_multiCheckBoxStateChange(const int state);
    void slot_addNodeButton(/*bool check*/);
    void slot_delNodeButton(/*bool check*/);

signals:
	void signal_gAlgJsonObjChanged();

private:
	void initAlgVarsTableStyle();
	void initNodesTableStyle();
	void initDefaultThreadNum();
    void initAlgComboItem();
    void setCurrAlgorithmParameters();
    void initLayout();
	void insert2table(QTableWidget *table, const int &row, const int &clomun, const QString &itemValue);
	void updateGAlgJsonObj();

private:
	parsProblem* _atn_problem;
	parsAlgorithm* _algorithm;
    QLabel* _alg_label;
    QComboBox* _alg_combox;
    QTableWidget* _alg_vars_table;
    enum VARCOLUMN{keyFlag = 0, valueFlag};
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
	enum NODESCOLUMN{nodeFlag = 0, coreFlag};
	//button
    QPushButton* _add_button;
    QPushButton* _del_button;
	//group
	QGroupBox* _single_group;
	QGroupBox* _multi_group;

	QJsonObject _gAlg_obj;
};