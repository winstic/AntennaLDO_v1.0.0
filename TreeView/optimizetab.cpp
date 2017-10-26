#include "optimizetab.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"

optimizeTab::optimizeTab(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QDialog(parent),
_atn_problem(atn_problem), _obj(obj), _temp_pro_widget(atn_problem, obj), _temp_alg_widget(atn_problem){
    setWindowTitle(tr("天线优化"));
	setMinimumSize(880, 580);
	//remove help menu
	setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);

    _tab_widget = new QTabWidget(this);
	_first_tab = new QWidget(this);
	_second_tab = new QWidget(this);
	_third_tab = new QWidget(this);
	_fourth_tab = new QWidget(this);
	_save_all_button = new QPushButton("保存所有", this);
    connect(_save_all_button, SIGNAL(clicked(bool)), this, SLOT(slot_saveAllButton(bool)));

    //!fourth tab widget
    QJsonObject global_obj = parseJson::getJsonObj(QString("%1/global_conf.json").arg(dataPool::global::getGCurrentOptimizePath()));
    QString alg_name = global_obj.value("ALGORITHM_NAME").toString().trimmed();
    _alg_combox->setCurrentText(alg_name);
	_algorithm = dataPool::getAlgorithmByName(alg_name);
	setCurrAlgorithmParameters();
}

void optimizeTab::initLayout() {
	//first tab
	QGroupBox frequency_group_box(tr("频率设置"));
	QGroupBox far_field_group_box(tr("远场范围设置"));
	QGridLayout frequency_layout;
	_temp_pro_widget.initFrequencyWidget(&frequency_layout);
	frequency_group_box.setLayout(&frequency_layout);
	QGridLayout far_field_layout;
	_temp_pro_widget.initFarFieldWidget(&far_field_layout);
	far_field_group_box.setLayout(&far_field_layout);
	QVBoxLayout v_layout;
	v_layout.addWidget(&frequency_group_box);
	v_layout.addWidget(&far_field_group_box);
	v_layout.setSpacing(50);
	v_layout.setContentsMargins(2, 20, 2, 2);
	_first_tab->setLayout(&v_layout);
	//second tab
	QGroupBox group_box_gain(tr("增益设置"));
	QHBoxLayout gain_layout;
	_temp_pro_widget.initGainWidget(&gain_layout);
	group_box_gain.setLayout(&gain_layout);

	QGroupBox group_box_axial(tr("轴比设置"));
	QHBoxLayout axial_layout;
	_temp_pro_widget.initAxialWidget(&axial_layout);
	group_box_axial.setLayout(&axial_layout);

	QGroupBox group_box_loss(tr("回波损失"));
	QHBoxLayout loss_layout;
	_temp_pro_widget.initLossWidget(&loss_layout);
	group_box_loss.setLayout(&loss_layout);

	QVBoxLayout v_layout;
	v_layout.addWidget(&group_box_gain);
	v_layout.addWidget(&group_box_axial);
	v_layout.addWidget(&group_box_loss);
	_second_tab->setLayout(&v_layout);
	//third tab
	QHBoxLayout h_layout3;
	_temp_pro_widget.initVariablesRangeWidget(&h_layout3);
	_third_tab->setLayout(&h_layout3);
	//fourth tab
	QHBoxLayout	h_layout4;
	_temp_alg_widget.initAlgorithmWidget(&h_layout4);
	_fourth_tab->setLayout(&h_layout4);
	//layout
	_tab_widget->addTab(_first_tab, QIcon(""), tr("性能参数设置"));
	_tab_widget->addTab(_second_tab, QIcon(""), tr("增益轴比设置"));
	_tab_widget->addTab(_third_tab, QIcon(""), tr("模型设置"));
	_tab_widget->addTab(_fourth_tab, QIcon(""), tr("算法设置"));

	QHBoxLayout button_layout;
	button_layout.addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	button_layout.addWidget(_save_all_button);

	QVBoxLayout layout;
	layout.addWidget(_tab_widget);
	layout.addLayout(&button_layout);
	this->setLayout(&layout);
}

void optimizeTab::slot_saveAllButton(bool){
	QJsonObject mfre_obj, mfar_field_obj, mgain_obj, maxial_obj, mloss_obj, mvars_obj, mglobal_obj, malgorithm_obj;
	//Frequency Setting json object
	mfre_obj.insert("FreStart", QString("[%1]").arg(_temp_pro_widget._frequency_low_edit->text().trimmed()));
	mfre_obj.insert("FreEnd", QString("[%1]").arg(_temp_pro_widget._frequency_up_edit->text().trimmed()));
	mfre_obj.insert("FreNumber", QString("[%1]").arg(_temp_pro_widget._frequency_num_edit->text().trimmed()));
	mfre_obj.insert("SweepType", QString("[%1]").arg(_temp_pro_widget._sweep_type_combox->currentIndex()));
	mfre_obj.insert("PM", QString("[%1]").arg(_temp_pro_widget._polarization_combox->currentIndex()));

	//Farfield Setting json object
	mfar_field_obj.insert("ThetaLower", QString("[%1]").arg(_temp_pro_widget._theta_low_edit->text().trimmed()));
	mfar_field_obj.insert("ThetaUpper", QString("[%1]").arg(_temp_pro_widget._theta_up_edit->text().trimmed()));
	mfar_field_obj.insert("ThetaStep", QString("[%1]").arg(_temp_pro_widget._theta_step_edit->text().trimmed()));
	mfar_field_obj.insert("PhiLower", QString("[%1]").arg(_temp_pro_widget._phi_low_edit->text().trimmed()));
	mfar_field_obj.insert("PhiUpper", QString("[%1]").arg(_temp_pro_widget._phi_up_edit->text().trimmed()));
	mfar_field_obj.insert("PhiStep", QString("[%1]").arg(_temp_pro_widget._phi_step_edit->text().trimmed()));

	int i;
	//Gain Setting json object
	QStringList gainStr[8];
	for (i = 0; i < _temp_pro_widget._gain_table->rowCount(); i++) {
		gainStr[0] << _temp_pro_widget._gain_table->item(i, cthetalower)->text().trimmed();
		gainStr[1] << _temp_pro_widget._gain_table->item(i, cthetaupper)->text().trimmed();
		gainStr[2] << _temp_pro_widget._gain_table->item(i, cphilower)->text().trimmed();
		gainStr[3] << _temp_pro_widget._gain_table->item(i, cphiupper)->text().trimmed();
		QComboBox *goType = static_cast<QComboBox *>(_temp_pro_widget._gain_table->cellWidget(i, coptimaltype));
		if (3 == goType->currentIndex())
			gainStr[4] << goType->currentText().trimmed();
		else
			gainStr[4] << QString("'%1'").arg(goType->currentText().trimmed());
		gainStr[5] << _temp_pro_widget._gain_table->item(i, cdelta)->text().trimmed();
		gainStr[6] << _temp_pro_widget._gain_table->item(i, cobjvalue)->text().trimmed();
		gainStr[7] << _temp_pro_widget._gain_table->item(i, cweight)->text().trimmed();
	}
	mgain_obj.insert("Theta_Lower_gain", QString("[[%1]]").arg(gainStr[0].join(",")));
	mgain_obj.insert("Theta_Upper_gain", QString("[[%1]]").arg(gainStr[1].join(",")));
	mgain_obj.insert("Phi_Lower_gain", QString("[[%1]]").arg(gainStr[2].join(",")));
	mgain_obj.insert("Phi_Upper_gain", QString("[[%1]]").arg(gainStr[3].join(",")));
	mgain_obj.insert("optimaltype_gain", QString("[[%1]]").arg(gainStr[4].join(",")));
	mgain_obj.insert("delta_gain", QString("[[%1]]").arg(gainStr[5].join(",")));
	mgain_obj.insert("gainobj", QString("[[%1]]").arg(gainStr[6].join(",")));
	mgain_obj.insert("weight_gain", QString("[[%1]]").arg(gainStr[7].join(",")));

	//Axial Setting json object
	QStringList axialStr[8];
	for (i = 0; i < _temp_pro_widget._axial_table->rowCount(); i++) {
		axialStr[0] << _temp_pro_widget._axial_table->item(i, cthetalower)->text().trimmed();
		axialStr[1] << _temp_pro_widget._axial_table->item(i, cthetaupper)->text().trimmed();
		axialStr[2] << _temp_pro_widget._axial_table->item(i, cphilower)->text().trimmed();
		axialStr[3] << _temp_pro_widget._axial_table->item(i, cphiupper)->text().trimmed();
		QComboBox *aoType = static_cast<QComboBox *>(_temp_pro_widget._axial_table->cellWidget(i, coptimaltype));
		if (3 == aoType->currentIndex())
			axialStr[4] << aoType->currentText().trimmed();
		else
			axialStr[4] << QString("'%1'").arg(aoType->currentText().trimmed());
		axialStr[5] << _temp_pro_widget._axial_table->item(i, cdelta)->text().trimmed();
		axialStr[6] << _temp_pro_widget._axial_table->item(i, cobjvalue)->text().trimmed();
		axialStr[7] << _temp_pro_widget._axial_table->item(i, cweight)->text().trimmed();
	}
	maxial_obj.insert("Theta_Lower_axial", QString("[[%1]]").arg(axialStr[0].join(",")));
	maxial_obj.insert("Theta_Upper_axial", QString("[[%1]]").arg(axialStr[1].join(",")));
	maxial_obj.insert("Phi_Lower_axial", QString("[[%1]]").arg(axialStr[2].join(",")));
	maxial_obj.insert("Phi_Upper_axial", QString("[[%1]]").arg(axialStr[3].join(",")));
	maxial_obj.insert("optimaltype_axial", QString("[[%1]]").arg(axialStr[4].join(",")));
	maxial_obj.insert("delta_axial", QString("[[%1]]").arg(axialStr[5].join(",")));
	maxial_obj.insert("axialobj", QString("[[%1]]").arg(axialStr[6].join(",")));
	maxial_obj.insert("weight_axial", QString("[[%1]]").arg(axialStr[7].join(",")));

	//Loss Setting json object
	QStringList lossStr[11];
	for (i = 0; i < _temp_pro_widget._loss_table->rowCount(); i++) {
		lossStr[0] << _temp_pro_widget._loss_table->item(i, cz0real)->text().trimmed();
		lossStr[1] << _temp_pro_widget._loss_table->item(i, cz0imag)->text().trimmed();
		QComboBox *lossType = static_cast<QComboBox *>(_temp_pro_widget._loss_table->cellWidget(i, closstype));
		lossStr[2] << QString::number(lossType->currentIndex());
		QComboBox *loType = static_cast<QComboBox *>(_temp_pro_widget._loss_table->cellWidget(i, clossoptimaltype));
		if (3 == loType->currentIndex())
			lossStr[3] << loType->currentText().trimmed();
		else
			lossStr[3] << QString("'%1'").arg(loType->currentText().trimmed());
		lossStr[4] << _temp_pro_widget._loss_table->item(i, cdeltareal)->text().trimmed();
		lossStr[5] << _temp_pro_widget._loss_table->item(i, cdeltaimag)->text().trimmed(); \
			if (0 == lossType->currentIndex()) {
				lossStr[6] << _temp_pro_widget._loss_table->item(i, cobjreal)->text().trimmed();
				lossStr[7] << "None";
				lossStr[8] << "None";
			}
			else if (1 == lossType->currentIndex()) {
				lossStr[6] << "None";
				lossStr[7] << _temp_pro_widget._loss_table->item(i, cobjreal)->text().trimmed();
				lossStr[8] << "None";
			}
			else if (2 == lossType->currentIndex()) {
				lossStr[6] << "None";
				lossStr[7] << "None";
				lossStr[8] << _temp_pro_widget._loss_table->item(i, cobjreal)->text().trimmed();
			}
			else {
				lossStr[6] << "None";
				lossStr[7] << "None";
				lossStr[8] << "None";
			}
			lossStr[9] << _temp_pro_widget._loss_table->item(i, cobjimag)->text().trimmed();
			lossStr[10] << _temp_pro_widget._loss_table->item(i, clossweight)->text().trimmed();
	}
	mloss_obj.insert("R0_real", QString("[[%1]]").arg(lossStr[0].join(",")));
	mloss_obj.insert("R0_imag", QString("[[%1]]").arg(lossStr[1].join(",")));
	mloss_obj.insert("ReturnLossType", QString("[[%1]]").arg(lossStr[2].join(",")));
	mloss_obj.insert("optimaltype_vswr", QString("[[%1]]").arg(lossStr[3].join(",")));
	mloss_obj.insert("delta_real", QString("[[%1]]").arg(lossStr[4].join(",")));
	mloss_obj.insert("delta_imag", QString("[[%1]]").arg(lossStr[5].join(",")));
	mloss_obj.insert("vswrobj", QString("[[%1]]").arg(lossStr[6].join(",")));
	mloss_obj.insert("S11", QString("[[%1]]").arg(lossStr[7].join(",")));
	mloss_obj.insert("R1_real", QString("[[%1]]").arg(lossStr[8].join(",")));
	mloss_obj.insert("R1_imag", QString("[[%1]]").arg(lossStr[9].join(",")));
	mloss_obj.insert("weight_vswr", QString("[[%1]]").arg(lossStr[10].join(",")));

	//Variables Setting json object
	QString varKey, varValue, varNote;
	for (int i = 0; i < _temp_pro_widget._vars_range_table->rowCount(); i++) {
		varKey = _temp_pro_widget._vars_range_table->item(i, varnote)->whatsThis().trimmed();
		varNote = _temp_pro_widget._vars_range_table->item(i, varnote)->text().trimmed();
		varValue = QString("[%1,%2]").arg(_temp_pro_widget._vars_range_table->item(i, varmin)->text().trimmed())
			.arg(_temp_pro_widget._vars_range_table->item(i, varmax)->text().trimmed());
		QJsonObject itemobj;
		itemobj.insert("note", varNote);
		itemobj.insert(varKey, varValue);
		mvars_obj.insert(varKey, itemobj);
	}

	//global and algorithm setting
	/*QJsonObject node_thread_obj;
	for (int i = 0; i < _node_vars_table->rowCount(); ++i) {
		QString nodeOrIp, core, strUID;
		QJsonObject ncitemobj;
		strUID = QUuid::createUuid().toString();
		nodeOrIp = _node_vars_table->item(i, nodeFlag)->text().trimmed();
		core = _node_vars_table->item(i, coreFlag)->text().trimmed();
		ncitemobj.insert("node", nodeOrIp);
		ncitemobj.insert("threads", core);
		node_thread_obj.insert(strUID, ncitemobj);
	}*/
	//mglobal_obj.insert("ThreadNum", QString::number(_thread_edit->text().trimmed().toInt() + 1));
	mglobal_obj.insert("ALGORITHM_NAME", _temp_alg_widget._algorithm->name);
	mglobal_obj.insert("PROBLEM_NAME", _atn_problem->name);
	//mglobal_obj.insert("NodeAndThread", node_thread_obj);
	mglobal_obj.insert("outfilepath", QString("%1/outfilepath").arg(sysParam["CurrentOptimizePath"]));
	mglobal_obj.insert("outhfsspath", QString("%1/outhfsspath").arg(sysParam["CurrentOptimizePath"]));

	QString algKey, algValue, algNote;
	for (int i = 0; i < _temp_alg_widget._alg_vars_table->rowCount(); i++) {
		algKey = _temp_alg_widget._alg_vars_table->item(i, keyFlag)->whatsThis().trimmed();
		algNote = _temp_alg_widget._alg_vars_table->item(i, keyFlag)->text().trimmed();
		algValue = QString("%1").arg(_temp_alg_widget._alg_vars_table->item(i, valueFlag)->text().trimmed());
		QJsonObject itemobj;
		itemobj.insert(algKey, algValue);
		itemobj.insert("note", algNote);
		malgorithm_obj.insert(algKey, itemobj);
	}

    _obj.insert("FreSetting", mfre_obj);
    _obj.insert("ThetaPhiStep", mfar_field_obj);
    _obj.insert("GainSetting", mgain_obj);
    _obj.insert("AxialratioSetting", maxial_obj);
    _obj.insert("VSWRSetting", mloss_obj);
    _obj.insert("variables", mvars_obj);
    
	QString current_optimize_path = dataPool::global::getGCurrentOptimizePath();
	QString problem_json_path = QString("%1/%2_conf.json").arg(current_optimize_path).arg(_atn_problem->name);
    QString global_json_path = QString("%1/global_conf.json").arg(current_optimize_path);
    QString algorithm_json_path = QString("%1/%2_conf.json").arg(current_optimize_path).arg(_temp_alg_widget._algorithm->name);

    if((parseJson::write(problem_json_path, &_obj)
              && parseJson::write(global_json_path, &mglobal_obj)
              && parseJson::write(algorithm_json_path, &malgorithm_obj)))
        this->close();
    else
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
}