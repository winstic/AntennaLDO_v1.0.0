#pragma once
#include "../Templates/frequencyTemplate.h"
#include "../Templates/thetaPhiTemplate.h"
#include "../Templates/gainTemplate.h"
#include "../Templates/axialTemplate.h"
#include "../Templates/lossTemplate.h"

class performanceTab : public QDialog {
	Q_OBJECT
public:
	performanceTab(parsProblem* atn_problem, unsigned int index = 0, bool is_running = false, QWidget *parent = 0);
	~performanceTab();

	public slots:
	void slot_confirmButton(bool);
	void slot_cancelButton(bool);

private:
	void initLayout();

private:
	parsProblem* _atn_problem;
	QJsonObject _problem_obj;
	QTabWidget* _tab_widget;
	QWidget* _first_tab;
	QWidget* _second_tab;
	QWidget* _third_tab;
	QPushButton* _confirm_button;
	QPushButton* _cancel_button;
	frequencyTemplate* _frequency_widget;
	thetaPhiTemplate* _theta_phi_widget;
	gainTemplate* _gain_widget;
	axialTemplate* _axial_widget;
	lossTemplate* _loss_widget;
	QLabel* _hint;
	QVBoxLayout* _layout;
	bool _is_running;
};