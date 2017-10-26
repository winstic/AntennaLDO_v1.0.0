#pragma once

#include <QtWidgets>
#include "../Utility/global.h"
#include "../Antenna/problemWidgetTemplate.h"
#include "../Antenna/algorithmWidgetTemplate.h"

class optimizeTab : public QDialog{
    Q_OBJECT
public:
    optimizeTab(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent = 0);
    ~optimizeTab(){}

public slots:
    void slot_saveAllButton(bool);

private:
	void initLayout();

private:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	problemTemplate _temp_pro_widget;
	algorithmTemplate _temp_alg_widget;
    QTabWidget* _tab_widget;
    QWidget* _first_tab;
    QWidget* _second_tab;
    QWidget* _third_tab;
    QWidget* _fourth_tab;   
    QPushButton* _save_all_button;
};