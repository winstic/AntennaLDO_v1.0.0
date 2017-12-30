#pragma execution_character_set("utf-8")
#include "designtab.h"
#include "../Utility/global.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"

designTab::designTab(parsProblem* atn_problem, QJsonObject* obj, QWidget *parent) : QDialog(parent),
_atn_problem(atn_problem), _obj(obj){
	setWindowTitle("天线设计");    
    setMinimumSize(880, 580);
    //remove help menu
    setWindowFlags(windowFlags() &~ Qt::WindowContextHelpButtonHint);
	_tab_widget = new QTabWidget(this);
    _first_tab = new QWidget(this);
    _second_tab = new QWidget(this);   
	_frequency_widget = new frequencyTemplate(_atn_problem, _obj);
	_theta_phi_widget = new thetaPhiTemplate(_atn_problem, _obj);
	_vars_value_widget = new varsDefaultValueTemplate(_atn_problem, _obj);
	_save_all_button = new QPushButton(QString("保存所有"), this);
	_hint = new QLabel(this);
	QFont font;
	font.setPixelSize(20);
	_hint->setFont(font);

	_tab_widget->addTab(_first_tab, QIcon(""), "性能参数设置");
	//firstTab->setWindowTitle(tr("设置频率信息并指定远场范围"));
	_tab_widget->addTab(_second_tab, QIcon(""), "模型设置");
	//secondTab->setWindowTitle(tr("模型设置"));
	//QScrollArea *scrollArea = new QScrollArea;
	//scrollArea->setWidget(tabWidget);
	initLayout();

    connect(_save_all_button, SIGNAL(clicked(bool)), this, SLOT(slot_saveAllButton(bool)));	
}

void designTab::initLayout() {
	//first tab
	QGroupBox* frequency_group_box = new QGroupBox("频率设置");
	QGroupBox* far_field_group_box = new QGroupBox("远场范围设置");
	QLayout* frequency_layout = _frequency_widget->getLayout();
	frequency_group_box->setLayout(frequency_layout);
	QLayout* far_field_layout = _theta_phi_widget->getLayout();
	far_field_group_box->setLayout(far_field_layout);
	QVBoxLayout* v_layout = new QVBoxLayout;
	v_layout->addWidget(frequency_group_box);
	v_layout->addWidget(far_field_group_box);
	v_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	v_layout->setSpacing(100);
	v_layout->setContentsMargins(10, 20, 10, 2);
	_first_tab->setLayout(v_layout);
	//second tab
	QLayout* layout = _vars_value_widget->getLayout();
	layout->setContentsMargins(10, 20, 10, 2);
	_second_tab->setLayout(layout);

	/*QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
	hlayout->addWidget(_hint);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));*/

	QHBoxLayout* button_layout = new QHBoxLayout;
	//在按钮左侧添加伸缩，让按钮居右
	button_layout->addWidget(_hint);
	button_layout->addStretch();
	button_layout->addWidget(_save_all_button);
	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->addWidget(_tab_widget);
	//vlayout->addLayout(hlayout);
	vlayout->addLayout(button_layout);
	setLayout(vlayout);
}

void designTab::slot_saveAllButton(bool isChecked){
	_hint->clear();
	QList<checkInfo*> cios;
	cios.append(_frequency_widget->checkInputValid());
	cios.append(_theta_phi_widget->checkInputValid());
	for (checkInfo* cio : cios) {
		if (cio->code != 0) {
			_hint->setText(cio->message);
			return;
		}
	}
	QList<iTemplate*> templates{ _frequency_widget, _theta_phi_widget, _vars_value_widget };
	for (iTemplate* iter : templates)
		iter->updateJObj();

    if(parseJson::write(QString("%1/%2_conf.json").arg(dataPool::global::getGCurrentDesignPath()).arg(_atn_problem->name), _obj))
        this->close();
	else {
		qCritical("save failed in design tabWidget.");
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
	}
}