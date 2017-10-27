#include "../Utility/global.h"
#include "wizardOptimizeGainAxialLoss.h"
#include "../Utility/macrodefined.h"

wizardOptimizeAXL::wizardOptimizeAXL(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QWizardPage(parent), 
_atn_problem(atn_problem), _obj(obj){
    setTitle(tr("增益轴比回波损失设置"));
    setSubTitle(tr("设置增益轴比回波损失等参数"));

	_gain_widget = new gainTemplate(atn_problem, obj);
	_axial_widget = new axialTemplate(atn_problem, obj);
	_loss_widget = new lossTemplate(atn_problem, obj);

	//layout
	QGroupBox group_box_gain(tr("增益设置"));
	QLayout* gain_layout = _gain_widget->getLayout();
	group_box_gain.setLayout(gain_layout);

	QGroupBox group_box_axial(tr("轴比设置"));
	QLayout* axial_layout = _axial_widget->getLayout();
	group_box_axial.setLayout(axial_layout);

	QGroupBox group_box_loss(tr("回波损失"));
	QLayout* loss_layout = _loss_widget->getLayout();
	group_box_loss.setLayout(loss_layout);

	QVBoxLayout v_layout;
	v_layout.addWidget(&group_box_gain);
	v_layout.addWidget(&group_box_axial);
	v_layout.addWidget(&group_box_loss);
	setLayout(&v_layout);
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