#pragma execution_character_set("utf-8")
#include "../Utility/global.h"
#include "wizardOptimizeGainAxialLoss.h"
#include "../Utility/macrodefined.h"

wizardOptimizeAXL::wizardOptimizeAXL(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent) : QWizardPage(parent), 
_atn_problem(atn_problem), _obj(obj){
    setTitle("增益、轴比、回波损失参数设置");

	_gain_widget = new gainTemplate(atn_problem, _obj);
	_axial_widget = new axialTemplate(atn_problem, _obj);
	_loss_widget = new lossTemplate(atn_problem, _obj);

	//layout
	QGroupBox* group_box_gain = new QGroupBox("增益设置");
	QLayout* gain_layout = _gain_widget->getLayout();
	group_box_gain->setLayout(gain_layout);

	QGroupBox* group_box_axial = new QGroupBox("轴比设置");
	QLayout* axial_layout = _axial_widget->getLayout();
	group_box_axial->setLayout(axial_layout);

	QGroupBox* group_box_loss = new QGroupBox("回波损失");
	QLayout* loss_layout = _loss_widget->getLayout();
	group_box_loss->setLayout(loss_layout);

	QVBoxLayout* v_layout = new QVBoxLayout;
	v_layout->addWidget(group_box_gain);
	v_layout->addWidget(group_box_axial);
	v_layout->addWidget(group_box_loss);
	v_layout->setContentsMargins(2, 20, 10, 2);
	setLayout(v_layout);
}


bool wizardOptimizeAXL::isComplete() const{
    return true;
}

QList<iTemplate*> wizardOptimizeAXL::getTemplatesWidget() const {
	return QList<iTemplate*> {_gain_widget, _axial_widget, _loss_widget};
}

wizardOptimizeAXL::~wizardOptimizeAXL() {
	delete _gain_widget;
	_gain_widget = nullptr;
	delete _axial_widget;
	_axial_widget = nullptr;
	delete _loss_widget;
	_loss_widget = nullptr;
}