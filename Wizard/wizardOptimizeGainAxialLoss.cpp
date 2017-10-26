#include "../Utility/global.h"
#include "wizardOptimizeGainAxialLoss.h"
#include "../Utility/macrodefined.h"

wizardOptimizeAXL::wizardOptimizeAXL(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QWizardPage(parent), 
_atn_problem(atn_problem), _obj(obj), _temp_widget(atn_problem, obj){
    setTitle(tr("增益轴比回波损失设置"));
    setSubTitle(tr("设置增益轴比回波损失等参数"));

	//layout
	QGroupBox group_box_gain(tr("增益设置"));
	QHBoxLayout gain_layout;
	_temp_widget.initGainWidget(&gain_layout);
	group_box_gain.setLayout(&gain_layout);

	QGroupBox group_box_axial(tr("轴比设置"));
	QHBoxLayout axial_layout;
	_temp_widget.initAxialWidget(&axial_layout);
	group_box_axial.setLayout(&axial_layout);

	QGroupBox group_box_loss(tr("回波损失"));
	QHBoxLayout loss_layout;
	_temp_widget.initLossWidget(&loss_layout);
	group_box_loss.setLayout(&loss_layout);

	QVBoxLayout v_layout;
	v_layout.addWidget(&group_box_gain);
	v_layout.addWidget(&group_box_axial);
	v_layout.addWidget(&group_box_loss);
	setLayout(&v_layout);

	registerField("gal_json_obj", this, "galJsonObj", SIGNAL(signal_galJsonObjChanged()));
}


bool wizardOptimizeAXL::isComplete() const{
    return true;
}

void wizardOptimizeAXL::initializePage() {
	updateJsonObj();
	emit signal_galJsonObjChanged();
}