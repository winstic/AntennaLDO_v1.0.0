#pragma once

#include "../Templates/frequencyTemplate.h"
#include "../Templates/thetaPhiTemplate.h"
#include "../Templates/gainTemplate.h"
#include "../Templates/axialTemplate.h"
#include "../Templates/lossTemplate.h"
#include "../Templates/variablesTemplate.h"
#include "../Templates/algorithmTemplate.h"

class optimizeTab : public QDialog{
    Q_OBJECT
public:
    optimizeTab(parsProblem* atn_problem, QJsonObject& problem_obj, parsAlgorithm* palgorithm, QWidget *parent = 0);
    ~optimizeTab(){}

public slots:
    void slot_saveAllButton(bool);

private:
	void initLayout();

private:
	parsProblem* _atn_problem;
	parsAlgorithm* _algorithm;
	QJsonObject _problem_obj;
	QJsonObject _algorithm_obj;
    QTabWidget* _tab_widget;
    QWidget* _first_tab;
    QWidget* _second_tab;
    QWidget* _third_tab;
    QWidget* _fourth_tab;   
    QPushButton* _save_all_button;
	frequencyTemplate* _frequency_widgete;
	thetaPhiTemplate* _theta_phi_widget;
	gainTemplate* _gain_widget;
	axialTemplate* _axial_widget;
	lossTemplate* _loss_widget;
	variablesTemplate* _variables_widget;
	algorithmTemplate* _algorithm_widget;
};