#include "designtab.h"
#include "../Utility/global.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"

designTab::designTab(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QDialog(parent),
_atn_problem(atn_problem), _obj(obj), _temp_widget(atn_problem, obj){
	setWindowTitle(tr("天线设计"));
    _tab_widget = new QTabWidget(this);
    setMinimumSize(880, 580);
    //remove help menu
    setWindowFlags(windowFlags() &~ Qt::WindowContextHelpButtonHint);
    _first_tab = new QWidget(this);
    _second_tab = new QWidget(this);
    _save_all_button = new QPushButton("保存所有", this);
    connect(_save_all_button, SIGNAL(clicked(bool)), this, SLOT(slot_saveAllButton(bool)));

	initLayout();
}

void designTab::initLayout() {
	//first tab
	QGroupBox frequency_group_box(tr("频率设置"));
	QGroupBox far_field_group_box(tr("远场范围设置"));
	QGridLayout frequency_layout;
	_temp_widget.initFrequencyWidget(&frequency_layout);
	frequency_group_box.setLayout(&frequency_layout);
	QGridLayout far_field_layout;
	_temp_widget.initFarFieldWidget(&far_field_layout);
	far_field_group_box.setLayout(&far_field_layout);
	QVBoxLayout v_layout;
	v_layout.addWidget(&frequency_group_box);
	v_layout.addWidget(&far_field_group_box);
	v_layout.setSpacing(50);
	v_layout.setContentsMargins(2, 20, 2, 2);
	_first_tab->setLayout(&v_layout);
	//second tab
	QHBoxLayout layout;
	_temp_widget.initVariablesValueWidget(&layout);
	_second_tab->setLayout(&layout);

	_tab_widget->addTab(_first_tab, QIcon(""), tr("性能参数设置"));
	//firstTab->setWindowTitle(tr("设置频率信息并指定远场范围"));
	_tab_widget->addTab(_second_tab, QIcon(""), tr("模型设置"));
	//secondTab->setWindowTitle(tr("模型设置"));
	//QScrollArea *scrollArea = new QScrollArea;
	//scrollArea->setWidget(tabWidget);
	QHBoxLayout button_layout;
	button_layout.addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	button_layout.addWidget(_save_all_button);
	QVBoxLayout layout;
	layout.addWidget(_tab_widget);
	layout.addLayout(&button_layout);
	setLayout(&layout);
}

void designTab::slot_saveAllButton(bool isChecked){
	QJsonObject mfre_obj, mfar_field_obj, mvars_value_obj;
	//Frequency Setting json object
	mfre_obj.insert("FreStart", QString("[%1]").arg(_temp_widget._frequency_low_edit->text().trimmed()));
	mfre_obj.insert("FreEnd", QString("[%1]").arg(_temp_widget._frequency_up_edit->text().trimmed()));
	mfre_obj.insert("FreNumber", QString("[%1]").arg(_temp_widget._frequency_num_edit->text().trimmed()));
	mfre_obj.insert("SweepType", QString("[%1]").arg(_temp_widget._sweep_type_combox->currentIndex()));
	mfre_obj.insert("PM", QString("[%1]").arg(_temp_widget._polarization_combox->currentIndex()));
	//Farfield Setting json object
	mfar_field_obj.insert("ThetaLower", QString("[%1]").arg(_temp_widget._theta_low_edit->text().trimmed()));
	mfar_field_obj.insert("ThetaUpper", QString("[%1]").arg(_temp_widget._theta_up_edit->text().trimmed()));
	mfar_field_obj.insert("ThetaStep", QString("[%1]").arg(_temp_widget._theta_step_edit->text().trimmed()));
	mfar_field_obj.insert("PhiLower", QString("[%1]").arg(_temp_widget._phi_low_edit->text().trimmed()));
	mfar_field_obj.insert("PhiUpper", QString("[%1]").arg(_temp_widget._phi_up_edit->text().trimmed()));
	mfar_field_obj.insert("PhiStep", QString("[%1]").arg(_temp_widget._phi_step_edit->text().trimmed()));

	//Variables Setting json object
	QString var_key, var_value;
	for (int i = 0; i < _vars_table->rowCount(); i++) {
		var_key = _vars_table->item(i, varnote)->whatsThis().trimmed();
		var_value = _vars_table->cellWidget(i, varvalue)->findChild<QLineEdit *>()->text().trimmed();
		mvars_value_obj.insert(var_key, var_value);
	}

    _obj.insert("FreSetting", mfre_obj);
    _obj.insert("ThetaPhiStep", mfar_field_obj);
    _obj.insert("varsValue", mvars_value_obj);

    if(parseJson::write(QString("%1/%2_conf.json").arg(dataPool::global::getGCurrentDesignPath()).arg(_atn_problem->name), &_obj))
        this->close();
	else {
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
	}
}