#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "performanceModel.h"

performanceTab::performanceTab(parsProblem* atn_problem, unsigned int index, bool is_running, QWidget *parent)	: QDialog(parent),
_atn_problem(atn_problem), _is_running(is_running) {
	setWindowTitle("天线优化");
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	//remove help menu
	setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	//setStyleSheet("background-color: white");

	_tab_widget = new QTabWidget(this);
	_first_tab = new QWidget();
	_second_tab = new QWidget();
	_third_tab = new QWidget();
	_save_all_button = new QPushButton(QString("保存所有"), this);
	_hint = new QLabel(this);
	commonStyle::setHintStyle(_hint);

	_problem_obj = parseJson::getJsonObj(QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name));
	if (_problem_obj.isEmpty()) {
		qCritical("get problem json object field.");
		QMessageBox::critical(0, QString("警告"), QString("读取问题配置文件失败！"));
	}
	else {
		_frequency_widget = new frequencyTemplate(_atn_problem, &_problem_obj, index);
		_theta_phi_widget = new thetaPhiTemplate(_atn_problem, &_problem_obj, index);
		_gain_widget = new gainTemplate(_atn_problem, &_problem_obj, index);
		_axial_widget = new axialTemplate(_atn_problem, &_problem_obj, index);
		_loss_widget = new lossTemplate(_atn_problem, &_problem_obj, index);


		_tab_widget->addTab(_first_tab, QIcon(""), "频率参数设置");
		_tab_widget->addTab(_second_tab, QIcon(""), "增益轴比设置");
		_tab_widget->addTab(_third_tab, QIcon(""), "回波损失设置");

		connect(_save_all_button, SIGNAL(clicked(bool)), this, SLOT(slot_saveAllButton(bool)));
		connect(_theta_phi_widget, SIGNAL(signal_confirmFarField(QString)), _gain_widget, SLOT(slot_confirmFarField(QString)));
		connect(_theta_phi_widget, SIGNAL(signal_confirmFarField(QString)), _axial_widget, SLOT(slot_confirmFarField(QString)));
		initLayout();
	}	
}

void performanceTab::initLayout() {
	_frequency_widget->traversalWidgets(_frequency_widget->children(), !_is_running);
	_theta_phi_widget->traversalWidgets(_theta_phi_widget->children(), !_is_running);
	//_gain_widget->traversalWidgets(_gain_widget->children(), !_is_running);
	//_axial_widget->traversalWidgets(_axial_widget->children(), !_is_running);
	_loss_widget->traversalWidgets(_loss_widget->children(), !_is_running);
	_save_all_button->setEnabled(!_is_running);

	//first tab
	QLayout* frequency_layout = _frequency_widget->getLayout();
	//设置tab页面内容与边界的距离
	frequency_layout->setContentsMargins(100, 20, 50, 100);
	_first_tab->setLayout(frequency_layout);
	//second tab
	QGroupBox* group_box_far_field = new QGroupBox("先设置远场范围");
	QLayout* far_field_layout = _theta_phi_widget->getLayout();
	group_box_far_field->setLayout(far_field_layout);

	QGroupBox* group_box_gain = new QGroupBox("增益设置");
	QLayout* gain_layout = _gain_widget->getLayout();
	group_box_gain->setLayout(gain_layout);

	QGroupBox* group_box_axial = new QGroupBox("轴比设置");
	QLayout* axial_layout = _axial_widget->getLayout();
	group_box_axial->setLayout(axial_layout);

	QVBoxLayout* v_layout = new QVBoxLayout;
	v_layout->addWidget(group_box_far_field);
	v_layout->addWidget(group_box_gain);
	v_layout->addWidget(group_box_axial);
	v_layout->setContentsMargins(2, 20, 10, 2);
	_second_tab->setLayout(v_layout);
	//third tab
	QLayout* loss_layout = _loss_widget->getLayout();
	loss_layout->setContentsMargins(10, 20, 10, 2);
	_third_tab->setLayout(loss_layout);
	//layout
	QHBoxLayout* button_layout = new QHBoxLayout;
	//在按钮左侧添加伸缩，让按钮居右
	button_layout->addWidget(_hint);
	button_layout->addStretch();
	button_layout->addWidget(_save_all_button);

	_layout = new QVBoxLayout;
	_layout->addWidget(_tab_widget);
	_layout->addLayout(button_layout);
	setLayout(_layout);
}

void performanceTab::slot_saveAllButton(bool) {
	_hint->clear();
	QList<iTemplate*> templates{ _frequency_widget, _theta_phi_widget, _gain_widget, _axial_widget, _loss_widget };
	for (iTemplate* iter : templates) {
		if (!iter->checkInputValid()) {
			_hint->setText(iter->checkInfo->message);
			return;
		}
	}
	for (iTemplate* iter : templates)
		iter->updateJObj();


	if (parseJson::write(QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name), &_problem_obj))
		this->close();
	else {
		qCritical("save failed in freformance tabWidget.");
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
	}
}

performanceTab::~performanceTab() {
	delete _layout;
	_layout = nullptr;
}