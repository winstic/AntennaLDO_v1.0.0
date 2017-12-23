#pragma once

#include "../Templates/frequencyTemplate.h"
#include "../Templates/thetaPhiTemplate.h"
#include "../Templates/varsDefaultValueTemplate.h"

class designTab : public QDialog{
    Q_OBJECT
public:
    designTab(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent = 0);
    ~designTab(){}

public slots:
    void slot_saveAllButton(bool);

private:
	void initLayout();

private:
	parsProblem* _atn_problem;
	QJsonObject* _obj;
	frequencyTemplate* _frequency_widget;
	thetaPhiTemplate* _theta_phi_widget;
	varsDefaultValueTemplate* _vars_value_widget;
    QTabWidget* _tab_widget;
    QWidget* _first_tab;
    QWidget* _second_tab;    
	QPushButton* _save_all_button;
};