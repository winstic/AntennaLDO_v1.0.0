#pragma execution_character_set("utf-8")
#include "optimizetab.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"

optimizeTab::optimizeTab(parsProblem* atn_problem, QJsonObject* problem_obj, parsAlgorithm* palgorithm, QWidget *parent)
	: QDialog(parent), _atn_problem(atn_problem), _problem_obj(problem_obj), _algorithm(palgorithm), _algorithm_obj(nullptr){
    setWindowTitle("天线优化");
	setMinimumSize(880, 580);
	//remove help menu
	setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);

    _tab_widget = new QTabWidget(this);
	_first_tab = new QWidget(this);
	_second_tab = new QWidget(this);
	_third_tab = new QWidget(this);
	_fourth_tab = new QWidget(this);
	_save_all_button = new QPushButton(QString("保存所有"), this);

	_frequency_widgete = new frequencyTemplate(_atn_problem, _problem_obj);
	_theta_phi_widget = new thetaPhiTemplate(_atn_problem, _problem_obj);
	_gain_widget = new gainTemplate(_atn_problem, _problem_obj);
	_axial_widget = new axialTemplate(_atn_problem, _problem_obj);
	_loss_widget = new lossTemplate(_atn_problem, _problem_obj);
	_variables_widget = new variablesTemplate(_atn_problem, _problem_obj);
	//if _algorithm not nullptr, get _algorithm infomation
	_algorithm_widget = new algorithmTemplate(_atn_problem, &_algorithm_obj, &_algorithm);

	_tab_widget->addTab(_first_tab, QIcon(""), "性能参数设置");
	_tab_widget->addTab(_second_tab, QIcon(""), "增益轴比设置");
	_tab_widget->addTab(_third_tab, QIcon(""), "模型设置");
	_tab_widget->addTab(_fourth_tab, QIcon(""), "算法设置");
	initLayout();
    connect(_save_all_button, SIGNAL(clicked(bool)), this, SLOT(slot_saveAllButton(bool)));
}

void optimizeTab::initLayout() {
	//first tab
	QGroupBox* frequency_group_box = new QGroupBox("频率设置");
	QGroupBox* far_field_group_box = new QGroupBox("远场范围设置");
	QLayout* frequency_layout = _frequency_widgete->getLayout();
	frequency_group_box->setLayout(frequency_layout);
	QLayout* far_field_layout = _theta_phi_widget->getLayout();
	far_field_group_box->setLayout(far_field_layout);
	QVBoxLayout* v_layout1 = new QVBoxLayout;
	v_layout1->addWidget(frequency_group_box);
	v_layout1->addWidget(far_field_group_box);
	v_layout1->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	v_layout1->setSpacing(100);
	//设置tab页面内容与边界的距离
	v_layout1->setContentsMargins(10, 20, 10, 2);
	_first_tab->setLayout(v_layout1);
	//second tab
	QGroupBox* group_box_gain = new QGroupBox("增益设置");
	QLayout* gain_layout = _gain_widget->getLayout();
	group_box_gain->setLayout(gain_layout);

	QGroupBox* group_box_axial = new QGroupBox("轴比设置");
	QLayout* axial_layout = _axial_widget->getLayout();
	group_box_axial->setLayout(axial_layout);

	QGroupBox* group_box_loss = new QGroupBox("回波损失");
	QLayout* loss_layout = _loss_widget->getLayout();
	group_box_loss->setLayout(loss_layout);

	QVBoxLayout* v_layout2 = new QVBoxLayout;
	v_layout2->addWidget(group_box_gain);
	v_layout2->addWidget(group_box_axial);
	v_layout2->addWidget(group_box_loss);
	v_layout2->setContentsMargins(10, 20, 10, 2);
	_second_tab->setLayout(v_layout2);
	//third tab
	QLayout* h_layout3 = _variables_widget->getLayout();
	h_layout3->setContentsMargins(10, 20, 10, 2);
	_third_tab->setLayout(h_layout3);
	//fourth tab
	QLayout* h_layout4 = _algorithm_widget->getLayout();
	h_layout4->setContentsMargins(10, 20, 10, 2);
	_fourth_tab->setLayout(h_layout4);
	//layout
	QHBoxLayout* button_layout = new QHBoxLayout;
	//在按钮左侧添加伸缩，让按钮居右
	button_layout->addStretch();
	button_layout->addWidget(_save_all_button);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(_tab_widget);
	layout->addLayout(button_layout);
	setLayout(layout);
}

void optimizeTab::slot_saveAllButton(bool){
	QList<iTemplate*> templates{ _frequency_widgete, _theta_phi_widget, _gain_widget, _axial_widget, _loss_widget,
		_variables_widget, _algorithm_widget};
	for (iTemplate* iter : templates)
		iter->updateJObj();
	QString current_optimize_path = dataPool::global::getGCurrentOptimizePath();
	QString problem_json_path = QString("%1/%2_conf.json").arg(current_optimize_path).arg(_atn_problem->name);
    QString global_json_path = QString("%1/global_conf.json").arg(current_optimize_path);
    QString algorithm_json_path = QString("%1/%2_conf.json").arg(current_optimize_path).arg(_algorithm->name);
	
	QJsonObject global_obj = parseJson::getJsonObj(global_json_path);	
	if (global_obj.isEmpty()) {
		qCritical("something wrong in file: '%s'", qUtf8Printable(global_json_path));
		QMessageBox::critical(0, QString("Error"), QString("error:something wrong in file [%1]").arg(global_json_path));
		return;
	}
	global_obj.insert("ALGORITHM_NAME", _algorithm->name);
	global_obj.insert("PROBLEM_NAME", _atn_problem->name);

    if((parseJson::write(problem_json_path, _problem_obj)
              && parseJson::write(global_json_path, &global_obj)
              && parseJson::write(algorithm_json_path, _algorithm_obj)))
        this->close();
	else {
		qCritical("save failed in optimize tabWidget.");
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
	}
}