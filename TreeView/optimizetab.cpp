#include "optimizetab.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"

optimizeTab::optimizeTab(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QDialog(parent),
_atn_problem(atn_problem), _obj(obj){
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

	//frequencySetting
	_frequency_low_label = new QLabel("频段上限:", this);
	_frequency_up_label = new QLabel("频段下限:", this);
	_frequency_num_label = new QLabel("频点个数:", this);
	_sweep_type_label = new QLabel("扫频方式:", this);
	_polarization_label = new QLabel("极化方式:", this);
	_frequency_low_edit = new QLineEdit(this);
	_frequency_up_edit = new QLineEdit(this);
	_frequency_num_edit = new QLineEdit(this);
	_sweep_type_combox = new QComboBox(this);
	_polarization_combox = new QComboBox(this);
	_frequency_group_box = new QGroupBox(tr("频率设置"), this);

	//farFieldSetting
	_theta_low_label = new QLabel("起始θ:", this);
	_theta_up_label = new QLabel("终止θ:", this);
	_theta_step_label = new QLabel("θ步长:", this);
	_phi_low_label = new QLabel("起始φ:", this);
	_phi_up_label = new QLabel("终止φ:", this);
	_phi_step_label = new QLabel("φ步长:", this);
	_theta_low_edit = new QLineEdit(this);
	_theta_up_edit = new QLineEdit(this);
	_theta_step_edit = new QLineEdit(this);
	_phi_low_edit = new QLineEdit(this);
	_phi_up_edit = new QLineEdit(this);
	_phi_step_edit = new QLineEdit(this);
	_far_field_group_box = new QGroupBox(tr("远场范围设置"), this);

    //gain axial loss setting
	_gain_table = new QTableWidget(this);
	_axial_table = new QTableWidget(this);
	_loss_table = new QTableWidget(this);

    //variables setting
	_vars_table = new QTableWidget(this);

    //algorithm setting
	_alg_label = new QLabel(tr("选择算法:"), this);
	_alg_combox = new QComboBox(this);
	_alg_vars_table = new QTableWidget(this);


	_thread_label = new QLabel(tr("启动核数"), this);
	_thread_edit = new QLineEdit(this);
	_single_thread_checkbox = new QCheckBox(this);
	_single_thread_checkbox->setText(QString("单机计算"));
	_single_thread_checkbox->setCheckState(Qt::Checked);
	_multi_thread_checkbox = new QCheckBox(this);
	_multi_thread_checkbox->setText(QString("多机计算"));
	_multi_thread_checkbox->setCheckState(Qt::Unchecked);

	_node_label = new QLabel(QString("node"), this);
	_node_edit = new QLineEdit(this);
	_node_vars_table = new QTableWidget(this);
	_core_label = new QLabel(QString("cores"), this);
	_core_edit = new QLineEdit(this);

	_single_group = new QGroupBox(this);
	_multi_group = new QGroupBox(this);

	_add_button = new QPushButton(QString("添加"), this);
	_del_button = new QPushButton(QString("删除"), this);

    //!first tab widget
	initSweepNDPMComBox();
	setRegexp();
	frequencySetting();
	farFieldSetting();
	initFirstTabLayout();
    //!
    //!second tab widget
    gainSetting();
    axialSetting();
    lossSetting();
	initSecondTabLayout();
    //!

    //!third tab widget
    wizardDialog();
    //!

    //!fourth tab widget
	initAlgComboItem();
    QJsonObject global_obj = parseJson::getJsonObj(QString("%1/global_conf.json").arg(dataPool::global::getGCurrentOptimizePath()));
    QString alg_name = global_obj.value("ALGORITHM_NAME").toString().trimmed();
    _alg_combox->setCurrentText(alg_name);
	_algorithm = dataPool::getAlgorithmByName(alg_name);
	_thread_edit->setText(QString::number(global_obj.value("ThreadNum").toString().trimmed().toInt() - 1));

	setCurrAlgorithmParameters();
    initFourthTabLayout();
	connect(_single_thread_checkbox, SIGNAL(stateChanged(int)), this, SLOT(slot_singleCheckBoxStateChange(int)));
	connect(_multi_thread_checkbox, SIGNAL(stateChanged(int)), this, SLOT(slot_multiCheckBoxStateChange(int)));
	connect(_alg_combox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_algName(int)));
	connect(_add_button, SIGNAL(clicked(bool)), this, SLOT(slot_addNodeButton()));
	connect(_del_button, SIGNAL(clicked(bool)), this, SLOT(slot_delNodeButton()));
    //!
    _tab_widget->addTab(_first_tab, QIcon(""), tr("性能参数设置"));
    //firstTab->setWindowTitle(tr("设置频率信息并指定远场范围"));
	_tab_widget->addTab(_second_tab, QIcon(""), tr("增益轴比设置"));
    //secondTab->setWindowTitle(tr("模型设置"));
    //QScrollArea *scrollArea = new QScrollArea;
    //scrollArea->setWidget(tabWidget);
	_tab_widget->addTab(_third_tab, QIcon(""), tr("模型设置"));
	_tab_widget->addTab(_fourth_tab, QIcon(""), tr("算法设置"));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    buttonLayout->addWidget(_save_all_button);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_tab_widget);
    layout->addLayout(buttonLayout);

    this->setLayout(layout);
}

//!first tab widget
void optimizeTab::initSweepNDPMComBox() {
	_sweep_type_combox->addItem("等差");
	_sweep_type_combox->addItem("等比");

	_polarization_combox->addItem("总增益");
	_polarization_combox->addItem("左旋");
	_polarization_combox->addItem("右旋");
	_polarization_combox->addItem("水平");
	_polarization_combox->addItem("垂直");
}

void optimizeTab::setRegexp() {
	//set regexp
	//QRegExpValidator *posFloatValid = new QRegExpValidator(QRegExp("^(-?\\d+)(\\.\\d+)?$"));    //positive float
	QRegExpValidator *nonNegFloatValid = new QRegExpValidator(QRegExp("^(\\d+)(\\.\\d+)?$"));    //non negative float
	QRegExpValidator *floatValid = new QRegExpValidator(QRegExp("^-?(180|1?[0-7]?\\d(\\.\\d+)?)$"));      //float [-180. 180]
	QRegExpValidator *posIntValid = new QRegExpValidator(QRegExp("^[0-9]*[1-9][0-9]*$"));   //positive int
	_frequency_low_edit->setValidator(nonNegFloatValid);
	_frequency_up_edit->setValidator(nonNegFloatValid);
	_frequency_num_edit->setValidator(posIntValid);
	_theta_low_edit->setValidator(floatValid);
	_theta_up_edit->setValidator(floatValid);
	_theta_step_edit->setValidator(nonNegFloatValid);
	_phi_low_edit->setValidator(floatValid);
	_phi_up_edit->setValidator(floatValid);
	_phi_step_edit->setValidator(nonNegFloatValid);
}

void optimizeTab::frequencySetting() {
	QJsonObject frequency_obj = parseJson::getSubJsonObj(_obj, "FreSetting");
	if (frequency_obj.isEmpty()) {
		qCritical("get 'FreSetting' json object field.");
		return;
	}
	QStringList str_list;
	str_list = dataPool::str2list(frequency_obj.value("FreStart").toString().trimmed());
	_frequency_low_edit->setText(str_list[0]);
	str_list = dataPool::str2list(frequency_obj.value("FreEnd").toString().trimmed());
	_frequency_up_edit->setText(str_list[0]);
	str_list = dataPool::str2list(frequency_obj.value("FreNumber").toString().trimmed());
	_frequency_num_edit->setText(str_list[0]);
	_sweep_type_combox->setCurrentIndex(0);
	_sweep_type_combox->setEnabled(false);
	_polarization_combox->setCurrentIndex(0);
	_polarization_combox->setEnabled(false);
}

void optimizeTab::farFieldSetting() {
	QJsonObject far_field_obj = parseJson::getSubJsonObj(_obj, "ThetaPhiStep");
	if (far_field_obj.isEmpty()) {
		qCritical("get 'ThetaPhiStep' json object field.");
		return;
	}
	QStringList str_list;
	str_list = dataPool::str2list(far_field_obj.value("ThetaLower").toString().trimmed());
	_theta_low_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("ThetaUpper").toString().trimmed());
	_theta_up_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("ThetaStep").toString().trimmed());
	_theta_step_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("PhiLower").toString().trimmed());
	_phi_low_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("PhiUpper").toString().trimmed());
	_phi_up_edit->setText(str_list[0]);
	str_list = dataPool::str2list(far_field_obj.value("PhiStep").toString().trimmed());
	_phi_step_edit->setText(str_list[0]);
}

void optimizeTab::initFirstTabLayout() {
	//!groupBoxFrequency layout
	QGridLayout frequency_layout;
	_frequency_low_label->setAlignment(Qt::AlignRight);
	frequency_layout.addWidget(_frequency_low_label, 0, 0);
	frequency_layout.addWidget(_frequency_low_edit, 0, 1);

	QLabel hz_label("MHz");
	frequency_layout.addWidget(&hz_label, 0, 2);
	_frequency_up_label->setAlignment(Qt::AlignRight);
	frequency_layout.addWidget(_frequency_up_label, 1, 0);
	frequency_layout.addWidget(_frequency_up_edit, 1, 1);

	QLabel hz_label2("MHz");
	frequency_layout.addWidget(&hz_label2, 1, 2);
	_frequency_num_label->setAlignment(Qt::AlignRight);
	frequency_layout.addWidget(_frequency_num_label, 2, 0);
	frequency_layout.addWidget(_frequency_num_edit, 2, 1);

	_sweep_type_label->setAlignment(Qt::AlignRight);
	frequency_layout.addWidget(_sweep_type_label, 3, 0);
	frequency_layout.addWidget(_sweep_type_combox, 3, 1);

	_polarization_label->setAlignment(Qt::AlignRight);
	frequency_layout.addWidget(_polarization_label, 4, 0);
	frequency_layout.addWidget(_polarization_combox, 4, 1);

	_frequency_group_box->setLayout(&frequency_layout);
	//!

	//!groupBoxFarField layout
	QGridLayout far_field_layout;
	far_field_layout.addWidget(_theta_low_label, 0, 0);
	far_field_layout.addWidget(_theta_low_edit, 0, 1);
	far_field_layout.addWidget(_theta_up_label, 0, 2);
	far_field_layout.addWidget(_theta_up_edit, 0, 3);
	far_field_layout.addWidget(_theta_step_label, 0, 4);
	far_field_layout.addWidget(_theta_step_edit, 0, 5);
	far_field_layout.addWidget(_phi_low_label, 1, 0);
	far_field_layout.addWidget(_phi_low_edit, 1, 1);
	far_field_layout.addWidget(_phi_up_label, 1, 2);
	far_field_layout.addWidget(_phi_up_edit, 1, 3);
	far_field_layout.addWidget(_phi_step_label, 1, 4);
	far_field_layout.addWidget(_phi_step_edit, 1, 5);
	_far_field_group_box->setLayout(&far_field_layout);
	//!
	//!global layout
	QVBoxLayout v_layout;
	v_layout.addWidget(_frequency_group_box);
	v_layout.addWidget(_far_field_group_box);
	v_layout.setSpacing(50);
	v_layout.setContentsMargins(2, 20, 2, 2);
	_first_tab->setLayout(&v_layout);
	//!
}
//!
//!second tab widget
void optimizeTab::gainSetting(){
	_gain_table->setColumnCount(8);
	QStringList header;
	header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "增益(dB)" << "权值";
	_gain_table->setHorizontalHeaderLabels(header);
	//gainTable->setStyleSheet("QTableView {selection-background-color: grey;}");
	_gain_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QJsonObject gain_obj = parseJson::getSubJsonObj(_obj, "GainSetting");
    if(gain_obj.isEmpty()){
		qCritical("get 'GainSetting' json object field.");
		return;
    }
	QSignalMapper gain_signals_map;
	QStringList str_list_theta_lower = dataPool::str2list(gain_obj.value("Theta_Lower_gain").toString());
	QStringList str_list_theta_upper = dataPool::str2list(gain_obj.value("Theta_Upper_gain").toString());
	QStringList str_list_phi_lower = dataPool::str2list(gain_obj.value("Phi_Lower_gain").toString());
	QStringList str_list_phi_upper = dataPool::str2list(gain_obj.value("Phi_Upper_gain").toString());
	QStringList str_list_optimal_type = dataPool::str2list(gain_obj.value("optimaltype_gain").toString());
	QStringList str_list_delta = dataPool::str2list(gain_obj.value("delta_gain").toString());
	QStringList str_list_gain = dataPool::str2list(gain_obj.value("gainobj").toString());
	QStringList str_list_weight = dataPool::str2list(gain_obj.value("weight_gain").toString());
	_gain_table->setRowCount(str_list_theta_lower.length());
	for (int i = 0; i < str_list_theta_lower.length(); i++) {
		insert2table(_gain_table, i, cthetalower, str_list_theta_lower[i]);
		insert2table(_gain_table, i, cthetaupper, str_list_theta_upper[i]);
		insert2table(_gain_table, i, cphilower, str_list_phi_lower[i]);
		insert2table(_gain_table, i, cphiupper, str_list_phi_upper[i]);

		QComboBox optimal_type;
		initOptimalTypeComBox(&optimal_type);
		optimal_type.setCurrentText(str_list_optimal_type[i]);
		_gain_table->setCellWidget(i, coptimaltype, &optimal_type);
		//qDebug() << gainTable->item(i, coptimaltype)->text();
		//map combobox signal
		connect(&optimal_type, SIGNAL(currentIndexChanged(int)), &gain_signals_map, SLOT(map()));
		gain_signals_map.setMapping(&optimal_type, QString("%1-%2").arg(i).arg(coptimaltype));

		insert2table(_gain_table, i, cdelta, str_list_delta[i]);
		//setting cannot edit when optimize type is not delta
		if (optimal_type.currentIndex() != 2)
			_gain_table->item(i, cdelta)->setFlags(Qt::NoItemFlags);

		insert2table(_gain_table, i, cobjvalue, str_list_gain[i]);
		insert2table(_gain_table, i, cweight, str_list_weight[i]);
	}
	connect(&gain_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_gainChangeOptimaltype(QString)));
}

void optimizeTab::axialSetting(){
	_axial_table->setColumnCount(8);
	QStringList header;
	header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "轴比(dB)" << "权值";
	_axial_table->setHorizontalHeaderLabels(header);
	_axial_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QJsonObject axial_obj = parseJson::getSubJsonObj(_obj, "AxialratioSetting");
    if(axial_obj.isEmpty()){
		qCritical("get 'AxialratioSetting' json object field.");
		return;
    }
	QSignalMapper axial_signals_map;
	QStringList strListThetaLower = dataPool::str2list(axial_obj.value("Theta_Lower_axial").toString());
	QStringList strListThetaUpper = dataPool::str2list(axial_obj.value("Theta_Upper_axial").toString());
	QStringList strListPhiLower = dataPool::str2list(axial_obj.value("Phi_Lower_axial").toString());
	QStringList strListPhiUpper = dataPool::str2list(axial_obj.value("Phi_Upper_axial").toString());
	QStringList strListOptimaltype = dataPool::str2list(axial_obj.value("optimaltype_axial").toString());
	QStringList strListDelta = dataPool::str2list(axial_obj.value("delta_axial").toString());
	QStringList strListGainobj = dataPool::str2list(axial_obj.value("axialobj").toString());
	QStringList strListWeight = dataPool::str2list(axial_obj.value("weight_axial").toString());
	_axial_table->setRowCount(strListThetaLower.length());
	for (int i = 0; i < strListThetaLower.length(); i++) {
		insert2table(_axial_table, i, cthetalower, strListThetaLower[i]);
		insert2table(_axial_table, i, cthetaupper, strListThetaUpper[i]);
		insert2table(_axial_table, i, cphilower, strListPhiLower[i]);
		insert2table(_axial_table, i, cphiupper, strListPhiUpper[i]);

		QComboBox optimal_type;
		initOptimalTypeComBox(&optimal_type);
		optimal_type.setCurrentText(strListOptimaltype[i]);
		_axial_table->setCellWidget(i, coptimaltype, &optimal_type);
		//map combobox signal
		connect(&optimal_type, SIGNAL(currentIndexChanged(int)), &axial_signals_map, SLOT(map()));
		axial_signals_map.setMapping(&optimal_type, QString("%1-%2").arg(i).arg(coptimaltype));

		insert2table(_axial_table, i, cdelta, strListDelta[i]);
		//setting cannot edit when optimize type is delta
		if (optimal_type.currentIndex() != 2)
			_axial_table->item(i, cdelta)->setFlags(Qt::NoItemFlags);

		insert2table(_axial_table, i, cobjvalue, strListGainobj[i]);
		insert2table(_axial_table, i, cweight, strListWeight[i]);
	}
	connect(&axial_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_axialChangeOptimaltype(QString)));
}

void optimizeTab::lossSetting(){
	_loss_table->setColumnCount(9);
	QStringList header;
	header << "Z0实部" << "Z0虚部" << "损失方式" << "优化方式" << "误差实部" << "误差虚部" << "值实部" << "值虚部" << "权值";
	_loss_table->setHorizontalHeaderLabels(header);
	_loss_table->resizeColumnToContents(6);
	_loss_table->resizeColumnToContents(7);
	_loss_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QJsonObject loss_obj = parseJson::getSubJsonObj(_obj, "VSWRSetting");
    if(loss_obj.isEmpty()){
		qCritical("get 'VSWRSetting' json object field.");
		return;
    }
	QSignalMapper loss_signals_map;
	QStringList strListR0Real = dataPool::str2list(loss_obj.value("R0_real").toString());
	QStringList strListR0imag = dataPool::str2list(loss_obj.value("R0_imag").toString());
	QStringList strListReturnLossType = dataPool::str2list(loss_obj.value("ReturnLossType").toString());
	QStringList strListOptimaltype = dataPool::str2list(loss_obj.value("optimaltype_vswr").toString());
	QStringList strListDeltaReal = dataPool::str2list(loss_obj.value("delta_real").toString());
	QStringList strListDeltaImag = dataPool::str2list(loss_obj.value("delta_imag").toString());
	QStringList strListVswrobj = dataPool::str2list(loss_obj.value("vswrobj").toString());
	QStringList strListS11 = dataPool::str2list(loss_obj.value("S11").toString());
	QStringList strListR1Real = dataPool::str2list(loss_obj.value("R1_real").toString());
	QStringList strListR1Imag = dataPool::str2list(loss_obj.value("R1_imag").toString());
	QStringList strListWeight = dataPool::str2list(loss_obj.value("weight_vswr").toString());
	_loss_table->setRowCount(strListR0Real.length());
	for (int i = 0; i < strListR0Real.length(); i++) {
		insert2table(_loss_table, i, cz0real, strListR0Real[i]);
		insert2table(_loss_table, i, cz0imag, strListR0imag[i]);

		QComboBox loss_type;;
		initLossTypeComBox(&loss_type);
		loss_type.setCurrentIndex(QString(strListReturnLossType[i]).toInt());
		_loss_table->setCellWidget(i, closstype, &loss_type);
		//map combobox signal
		connect(&loss_type, SIGNAL(currentIndexChanged(int)), &loss_signals_map, SLOT(map()));
		loss_signals_map.setMapping(&loss_type, QString("%1-%2").arg(i).arg(closstype));

		QComboBox optimal_type;
		initOptimalTypeComBox(&optimal_type);
		optimal_type.setCurrentText(strListOptimaltype[i]);
		_loss_table->setCellWidget(i, clossoptimaltype, &optimal_type);
		//map combobox signal
		connect(&optimal_type, SIGNAL(currentIndexChanged(int)), &loss_signals_map, SLOT(map()));
		loss_signals_map.setMapping(&optimal_type, QString("%1-%2").arg(i).arg(clossoptimaltype));

		insert2table(_loss_table, i, cdeltareal, strListDeltaReal[i]);
		insert2table(_loss_table, i, cdeltaimag, strListDeltaImag[i]);
		//setting cannot edit when optimize type is delta
		if (2 != optimal_type.currentIndex()) {
			_loss_table->item(i, cdeltareal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(i, cdeltaimag)->setFlags(Qt::NoItemFlags);
		}
		if (0 == loss_type.currentIndex()) {
			//loss type is vswr
			insert2table(_loss_table, i, cobjreal, strListVswrobj[i]);
			_loss_table->setItem(i, cobjimag, new QTableWidgetItem("None"));
			_loss_table->item(i, cobjimag)->setFlags(Qt::NoItemFlags);
		}
		else if (1 == loss_type.currentIndex()) {
			//loss type is S11
			insert2table(_loss_table, i, cobjreal, strListS11[i]);
			_loss_table->setItem(i, cobjimag, new QTableWidgetItem("None"));
			_loss_table->item(i, cobjimag)->setFlags(Qt::NoItemFlags);
		}
		else if (2 == loss_type.currentIndex()) {
			//loss type is R
			insert2table(_loss_table, i, cobjreal, strListR1Real[i]);
			insert2table(_loss_table, i, cobjimag, strListR1Imag[i]);
		}
		else {
			//loss type is None
			_loss_table->setItem(i, cobjreal, new QTableWidgetItem("None"));
			_loss_table->setItem(i, cobjimag, new QTableWidgetItem("None"));
			_loss_table->item(i, cobjreal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(i, cobjimag)->setFlags(Qt::NoItemFlags);
		}
		insert2table(_loss_table, i, clossweight, strListWeight[i]);
	}
	connect(&loss_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_lossChangeType(QString)));
}

void optimizeTab::initOptimalTypeComBox(QComboBox *combox){
    combox->addItem("max");
    combox->addItem("min");
    combox->addItem("delta");
    combox->addItem("None");
    combox->setCurrentIndex(3);
}

void optimizeTab::initLossTypeComBox(QComboBox *combox){
    combox->addItem(tr("驻波比"));
    combox->addItem(tr("S11(dB)"));
    combox->addItem(tr("阻抗"));
    combox->addItem(tr("None"));
    combox->setCurrentIndex(3);
}

void optimizeTab::initSecondTabLayout(){
	QGroupBox group_box_gain(tr("增益设置"));
	QHBoxLayout gain_layout;
	gain_layout.addWidget(_gain_table);
	group_box_gain.setLayout(&gain_layout);

	QGroupBox group_box_axial(tr("轴比设置"));
	QHBoxLayout axial_layout;
	axial_layout.addWidget(_axial_table);
	group_box_axial.setLayout(&axial_layout);

	QGroupBox group_box_loss(tr("回波损失"));
	QHBoxLayout loss_layout;
	loss_layout.addWidget(_loss_table);
	group_box_loss.setLayout(&loss_layout);

	QVBoxLayout v_layout;
	v_layout.addWidget(&group_box_gain);
	v_layout.addWidget(&group_box_axial);
	v_layout.addWidget(&group_box_loss);
    _second_tab->setLayout(&v_layout);
}
//!

//!third tab widget
void optimizeTab::wizardDialog(){
	_vars_table->setColumnCount(4);
	QStringList header;
	header << "变量" << "最小值" << "最大值" << "单位";
	_vars_table->setHorizontalHeaderLabels(header);
	_vars_table->horizontalHeader()->setSectionsClickable(false);
	//varTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_vars_table->horizontalHeader()->setSectionResizeMode(varnote, QHeaderView::Stretch);
	_vars_table->horizontalHeader()->setSectionResizeMode(varunit, QHeaderView::ResizeToContents);
	_vars_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	_vars_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_vars_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_vars_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
	_vars_table->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	_vars_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit

    QJsonObject variables_obj = parseJson::getSubJsonObj(_obj, "variables");
    if(variables_obj.isEmpty()){
		qCritical("get 'variables' json object field.");
		return;
    }
	QJsonObject var_obj;
	QString var_key;
	QStringList var_value;
	int row_number = 0, value_list_length;
	QSignalMapper signals_map_unit;	//use signalmaper manage signals in table

	_vars_table->setRowCount(variables_obj.count());

	for (QJsonObject::iterator iter = variables_obj.begin(); iter != variables_obj.end(); ++iter) {
		// iter format: "W1":{"note" : "介质板宽度W1(m)", "W1" : "0.025"}
		var_key = iter.key();
		var_obj = iter.value().toObject();   //like {"note": "上贴片坐标y1(m)", "y1": "[-0.0115,0]"}
											 //get note infomation
		QString key_note = var_obj.value("note").toString().trimmed();
		insert2table(_vars_table, row_number, varnote, key_note);
		_vars_table->item(row_number, varnote)->setWhatsThis(var_key);

		var_value = dataPool::str2list(var_obj.value(var_key).toString().trimmed());
		value_list_length = var_value.length();
		insert2table(_vars_table, row_number, varmin, var_value[0]);
		if (value_list_length == 1)
			insert2table(_vars_table, row_number, varmax, var_value[0]);
		else
			insert2table(_vars_table, row_number, varmax, var_value[1]);

		QComboBox *unit_combox = new QComboBox();
		initUnitComBo(unit_combox);
		_vars_table->setCellWidget(row_number, varunit, unit_combox);
		//map combobox signal
		connect(unit_combox, SIGNAL(currentIndexChanged(int)), &signals_map_unit, SLOT(map()));
		signals_map_unit.setMapping(unit_combox, QString("%1-%2").arg(row_number).arg(varunit));
		//in 'rownumber'th row of table, save default unitComBo current data
		_vars_unit.insert(row_number, unit_combox->currentData(ROLE_MARK_UNIT).toInt());
		row_number++;
	}
	connect(&signals_map_unit, SIGNAL(mapped(QString)), this, SLOT(slot_unitchange(QString)));

	//!add picture
	QLabel* atn_image_label = new QLabel(this);
	QPixmap pm = QPixmap(_atn_problem->pImage);
	if (pm.width() > INTRODUCEIMAGEPIX)
		atn_image_label->setPixmap(pm.scaledToWidth(INTRODUCEIMAGEPIX));
	else
		atn_image_label->setPixmap(pm);
	//!
	QHBoxLayout h_layout;
	h_layout.addWidget(_vars_table, 1);
	h_layout.addWidget(atn_image_label, 1);
    _third_tab->setLayout(&h_layout);
}

double optimizeTab::unitConversion(double sourceData, int preunit, int curunit){
    return sourceData * qPow(10, preunit - curunit);
}

void optimizeTab::initUnitComBo(QComboBox *comb){
    comb->addItem("mm");
    comb->setItemData(0, MARK_UNIT_MM, ROLE_MARK_UNIT);
    comb->addItem("cm");
    comb->setItemData(1, MARK_UNIT_CM, ROLE_MARK_UNIT);
    comb->addItem("λ");
    comb->setItemData(2, MARK_UNIT_LAMBDA, ROLE_MARK_UNIT);
    comb->addItem("m");
    comb->setItemData(3, MARK_UNIT_M, ROLE_MARK_UNIT);
    comb->addItem("km");
    comb->setItemData(4, MARK_UNIT_KM, ROLE_MARK_UNIT);
    comb->setCurrentIndex(3);
}
//!

//!fourth tab widget
void optimizeTab::initAlgVarsTableStyle() {
	_alg_vars_table->setColumnCount(2);
	_alg_vars_table->horizontalHeader()->setVisible(false);
	_alg_vars_table->horizontalHeader()->setSectionResizeMode(valueFlag, QHeaderView::Stretch);
	_alg_vars_table->verticalHeader()->setVisible(false);
	_alg_vars_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	_alg_vars_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_alg_vars_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_alg_vars_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
}

void optimizeTab::initNodesTableStyle() {
	_node_vars_table->setColumnCount(2);
	QStringList header;
	header << "IP地址/主机名" << "启动核数";
	_node_vars_table->setHorizontalHeaderLabels(header);
	_node_vars_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//nodesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	_node_vars_table->verticalHeader()->setVisible(false);
	_node_vars_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_node_vars_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_node_vars_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
}

void optimizeTab::setCurrAlgorithmParameters(){
	QString alg_json_path = QString("%1/%2_conf.json").arg(_algorithm->path).arg(_algorithm->name);
	QJsonObject alg_obj = parseJson::getJsonObj(alg_json_path);
	if (alg_obj.isEmpty()) {
		qCritical(dataPool::str2char(QString("cannot parse algorithm json file [%1]").arg(alg_json_path)));
		QMessageBox::critical(0, QString("Error"), QString("error: Cannot parse algorithm json file"));
		return;
	}
	_alg_vars_table->setRowCount(alg_obj.count());
	QString var_key, var_value, value_note;
	QJsonObject var_obj; //format:{"generation": "1000", "note": "最大代数", "instruction": "最大迭代次数"}
	int row_number = 0;

	for (QJsonObject::iterator iter = alg_obj.begin(); iter != alg_obj.end(); ++iter) {
		var_key = iter.key();
		var_obj = iter.value().toObject();
		var_value = var_obj.value(var_key).toString().trimmed();
		value_note = var_obj.value("note").toString().trimmed();
		//valueInstruction = varObj.value("instruction").toString().trimmed();

		insert2table(_alg_vars_table, row_number, keyFlag, value_note);
		QTableWidgetItem *flagItem = _alg_vars_table->item(row_number, keyFlag);
		flagItem->setWhatsThis(var_key);
		//set this column cannot edit
		flagItem->setFlags(flagItem->flags() & (~Qt::ItemIsEditable));
		insert2table(_alg_vars_table, row_number, valueFlag, var_value);
		row_number++;
	}
}

void optimizeTab::initAlgComboItem() {
	int problem_id = _atn_problem->id;
	QMap<alg4pro, unsigned int>::iterator iter;		//like((algid, proid), associateId) 
	for (iter = dataPool::g_associates.begin(); iter != dataPool::g_associates.end(); ++iter) {
		if (iter.key().second == problem_id) {
			parsAlgorithm* algorithm = dataPool::getAlgorithmByID(iter.key().first);
			_alg_combox->addItem(algorithm->name, algorithm->id);	//so you can get algorithm_id through _alg_combox->Data()
		}
	}
}

void optimizeTab::initFourthTabLayout(){
	QHBoxLayout h_layout, h_layout_hint, v_layout;
	QHBoxLayout	h_layout_temp1, h_layout_temp2, h_layout_temp3, h_layout_temp4, h_layout_temp5, v_layout_temp1, v_layout_temp2;
	QGroupBox lgroup(QString("算法参数设置"));
	QGroupBox rgroup(QString("分布式并行"));

	//algorithm name
	_alg_label->setFixedWidth(80);
	h_layout_temp1.addWidget(_alg_label);
	h_layout_temp1.addWidget(_alg_combox);
	v_layout.addLayout(&h_layout_temp1);

	h_layout_temp2.addWidget(_alg_vars_table);
	lgroup.setLayout(&h_layout_temp2);

	//thread number
	_thread_label->setFixedWidth(80);
	h_layout_temp3.addWidget(_thread_label);
	h_layout_temp3.addWidget(_thread_edit);
	_single_group->setLayout(&h_layout_temp3);

	h_layout_temp4.addWidget(_node_label);
	h_layout_temp4.addWidget(_node_edit);
	h_layout_temp4.addWidget(_core_label);
	h_layout_temp4.addWidget(_core_edit);
	h_layout_temp4.addWidget(_add_button);
	v_layout_temp1.addLayout(&h_layout_temp4);


	h_layout_temp5.addWidget(_node_vars_table);
	h_layout_temp5.addWidget(_del_button);
	v_layout_temp1.addLayout(&h_layout_temp5);
	_multi_group->setLayout(&v_layout_temp1);

	_single_group->setEnabled(true);
	_multi_group->setEnabled(false);


	v_layout_temp2.addWidget(_single_thread_checkbox);
	v_layout_temp2.addWidget(_single_group);
	v_layout_temp2.addWidget(_multi_thread_checkbox);
	v_layout_temp2.addWidget(_multi_group);


	rgroup.setLayout(&v_layout_temp2);
	h_layout.addWidget(&lgroup);
	h_layout.addWidget(&rgroup);

	QVBoxLayout vlayout;
    vlayout.addSpacing(20);
    vlayout.addLayout(&h_layout);

    vlayout.setContentsMargins(2, 50, 2, 50);
    _fourth_tab->setLayout(&vlayout);
}
//!

void optimizeTab::insert2table(QTableWidget *table, const int &row, const int &clomun, const QString &itemValue){
    QTableWidgetItem *tableItem = new QTableWidgetItem(itemValue);
    tableItem->setTextAlignment(Qt::AlignCenter);
    table->setItem(row, clomun, tableItem);
}

//slots function
void optimizeTab::slot_gainChangeOptimaltype(QString pos){
    QStringList coordinates = pos.split("-");
    int row = coordinates[0].toInt();
    int col = coordinates[1].toInt();
    QComboBox *selectCombox = (QComboBox *)_gain_table->cellWidget(row, col);
    //setting edit when optimize type is delta
    if(selectCombox->currentIndex() == 2)
		_gain_table->item(row, cdelta)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    else
		_gain_table->item(row, cdelta)->setFlags(Qt::NoItemFlags);
}

void optimizeTab::slot_axialChangeOptimaltype(QString pos){
    QStringList coordinates = pos.split("-");
    int row = coordinates[0].toInt();
    int col = coordinates[1].toInt();
    QComboBox *selectCombox = (QComboBox *)_axial_table->cellWidget(row, col);
    //setting edit when optimize type is delta
    if(selectCombox->currentIndex() == 2){
        qDebug() << selectCombox->currentIndex();
		_axial_table->item(row, cdelta)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    }
    else
		_axial_table->item(row, cdelta)->setFlags(Qt::NoItemFlags);
}

void optimizeTab::slot_lossChangeType(QString pos){
    QStringList coordinates = pos.split("-");
    int row = coordinates[0].toInt();
    int col = coordinates[1].toInt();
    QComboBox *selectCombox = (QComboBox *)_loss_table->cellWidget(row, col);
    //setting edit when optimize type is delta
    if(col == clossoptimaltype){
        if(2 == selectCombox->currentIndex()){
			_loss_table->item(row, cdeltareal)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			_loss_table->item(row, cdeltaimag)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        }
        else{
			_loss_table->item(row, cdeltareal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(row, cdeltaimag)->setFlags(Qt::NoItemFlags);
        }
    }
    if(col == closstype){
        if(2 == selectCombox->currentIndex())
			_loss_table->item(row, cobjimag)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        else
            _loss_table->item(row, cobjimag)->setFlags(Qt::NoItemFlags);
    }
}

void optimizeTab::slot_unitchange(QString pos){
    Q_ASSERT(!_vars_unit.isEmpty());
    QStringList coordinates = pos.split("-");
    int row = coordinates.at(0).toInt();
    int col = coordinates.at(1).toInt();
    int currentUnitData = _vars_unit[row];
    QComboBox *selectCombox = (QComboBox *)_vars_table->cellWidget(row, col);
    qDebug() << selectCombox->currentText();
    int newUnitData = selectCombox->currentData(ROLE_MARK_UNIT).toInt();
    //qDebug() << varTable->item(row, varnote)->whatsThis();
    if(currentUnitData != MARK_UNIT_LAMBDA && newUnitData != MARK_UNIT_LAMBDA &&
            newUnitData != currentUnitData){
        double preValueMin = _vars_table->item(row, varmin)->text().trimmed().toDouble();
        double preValueMax = _vars_table->item(row, varmax)->text().trimmed().toDouble();
        double currentValueMin = unitConversion(preValueMin, currentUnitData, newUnitData);
        double currentValueMax = unitConversion(preValueMax, currentUnitData, newUnitData);
        qDebug() << currentValueMin << currentValueMax;
        insert2table(_vars_table, row, varmin, QString::number(currentValueMin));
        insert2table(_vars_table, row, varmax, QString::number(currentValueMax));
    }
	_vars_unit[row] = newUnitData;
}

void optimizeTab::slot_algName(const int index){
	_algorithm = dataPool::getAlgorithmByID(_alg_combox->itemData(index).toInt());
	setCurrAlgorithmParameters();
}

void optimizeTab::slot_singleCheckBoxStateChange(const int state){
	//state: enum explain{Qt::Unchecked=0, Qt::PartiallyChecked, Qt::Checked}
	//so usually using 0(unchecked), 2(checked);
	//qDebug() << state;
	if (state == 0) {
		_single_group->setEnabled(false);
		_multi_thread_checkbox->setCheckState(Qt::Checked);
	}
	else if (state == 2) {
		//set another checkBox(multiCheckBox unChecked)
		_single_group->setEnabled(true);
		_multi_thread_checkbox->setCheckState(Qt::Unchecked);
	}
}

void optimizeTab::slot_multiCheckBoxStateChange(const int state){
	if (state == 0) {
		//set another checkBox(multiCheckBox Checked)
		_multi_group->setEnabled(false);
		_single_thread_checkbox->setCheckState(Qt::Checked);
	}
	else if (state == 2) {
		//set another checkBox(multiCheckBox unChecked)
		_multi_group->setEnabled(true);
		_single_thread_checkbox->setCheckState(Qt::Unchecked);
	}
}

void optimizeTab::slot_addNodeButton(){
	QString nodeinfo, coreinfo;
	int rowIndex = _node_vars_table->rowCount();
	//qDebug() << rowIndex;
	_node_vars_table->insertRow(rowIndex);
	nodeinfo = _node_edit->text().trimmed();
	coreinfo = _core_edit->text().trimmed();
	insert2table(_node_vars_table, rowIndex, nodeFlag, nodeinfo);
	insert2table(_node_vars_table, rowIndex, coreFlag, coreinfo);
}

void optimizeTab::slot_delNodeButton(){
	int selectRow = _node_vars_table->currentRow();
	//qDebug() << selectRow;
	if (selectRow != -1)
		_node_vars_table->removeRow(selectRow);
}


void optimizeTab::slot_saveAllButton(bool){
	QJsonObject mfre_obj, mfar_field_obj, mgain_obj, maxial_obj, mloss_obj, mvars_obj, mglobal_obj, malgorithm_obj;
	//Frequency Setting json object
	mfre_obj.insert("FreStart", QString("[%1]").arg(_frequency_low_edit->text().trimmed()));
	mfre_obj.insert("FreEnd", QString("[%1]").arg(_frequency_up_edit->text().trimmed()));
	mfre_obj.insert("FreNumber", QString("[%1]").arg(_frequency_num_edit->text().trimmed()));
	mfre_obj.insert("SweepType", QString("[%1]").arg(_sweep_type_combox->currentIndex()));
	mfre_obj.insert("PM", QString("[%1]").arg(_polarization_combox->currentIndex()));

	//Farfield Setting json object
	mfar_field_obj.insert("ThetaLower", QString("[%1]").arg(_theta_low_edit->text().trimmed()));
	mfar_field_obj.insert("ThetaUpper", QString("[%1]").arg(_theta_up_edit->text().trimmed()));
	mfar_field_obj.insert("ThetaStep", QString("[%1]").arg(_theta_step_edit->text().trimmed()));
	mfar_field_obj.insert("PhiLower", QString("[%1]").arg(_phi_low_edit->text().trimmed()));
	mfar_field_obj.insert("PhiUpper", QString("[%1]").arg(_phi_up_edit->text().trimmed()));
	mfar_field_obj.insert("PhiStep", QString("[%1]").arg(_phi_step_edit->text().trimmed()));

	int i;
	//Gain Setting json object
	QStringList gainStr[8];
	for (i = 0; i < _gain_table->rowCount(); i++) {
		gainStr[0] << _gain_table->item(i, cthetalower)->text().trimmed();
		gainStr[1] << _gain_table->item(i, cthetaupper)->text().trimmed();
		gainStr[2] << _gain_table->item(i, cphilower)->text().trimmed();
		gainStr[3] << _gain_table->item(i, cphiupper)->text().trimmed();
		QComboBox *goType = static_cast<QComboBox *>(_gain_table->cellWidget(i, coptimaltype));
		if (3 == goType->currentIndex())
			gainStr[4] << goType->currentText().trimmed();
		else
			gainStr[4] << QString("'%1'").arg(goType->currentText().trimmed());
		gainStr[5] << _gain_table->item(i, cdelta)->text().trimmed();
		gainStr[6] << _gain_table->item(i, cobjvalue)->text().trimmed();
		gainStr[7] << _gain_table->item(i, cweight)->text().trimmed();
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
	for (i = 0; i < _axial_table->rowCount(); i++) {
		axialStr[0] << _axial_table->item(i, cthetalower)->text().trimmed();
		axialStr[1] << _axial_table->item(i, cthetaupper)->text().trimmed();
		axialStr[2] << _axial_table->item(i, cphilower)->text().trimmed();
		axialStr[3] << _axial_table->item(i, cphiupper)->text().trimmed();
		QComboBox *aoType = static_cast<QComboBox *>(_axial_table->cellWidget(i, coptimaltype));
		if (3 == aoType->currentIndex())
			axialStr[4] << aoType->currentText().trimmed();
		else
			axialStr[4] << QString("'%1'").arg(aoType->currentText().trimmed());
		axialStr[5] << _axial_table->item(i, cdelta)->text().trimmed();
		axialStr[6] << _axial_table->item(i, cobjvalue)->text().trimmed();
		axialStr[7] << _axial_table->item(i, cweight)->text().trimmed();
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
	for (i = 0; i < _loss_table->rowCount(); i++) {
		lossStr[0] << _loss_table->item(i, cz0real)->text().trimmed();
		lossStr[1] << _loss_table->item(i, cz0imag)->text().trimmed();
		QComboBox *lossType = static_cast<QComboBox *>(_loss_table->cellWidget(i, closstype));
		lossStr[2] << QString::number(lossType->currentIndex());
		QComboBox *loType = static_cast<QComboBox *>(_loss_table->cellWidget(i, clossoptimaltype));
		if (3 == loType->currentIndex())
			lossStr[3] << loType->currentText().trimmed();
		else
			lossStr[3] << QString("'%1'").arg(loType->currentText().trimmed());
		lossStr[4] << _loss_table->item(i, cdeltareal)->text().trimmed();
		lossStr[5] << _loss_table->item(i, cdeltaimag)->text().trimmed(); \
			if (0 == lossType->currentIndex()) {
				lossStr[6] << _loss_table->item(i, cobjreal)->text().trimmed();
				lossStr[7] << "None";
				lossStr[8] << "None";
			}
			else if (1 == lossType->currentIndex()) {
				lossStr[6] << "None";
				lossStr[7] << _loss_table->item(i, cobjreal)->text().trimmed();
				lossStr[8] << "None";
			}
			else if (2 == lossType->currentIndex()) {
				lossStr[6] << "None";
				lossStr[7] << "None";
				lossStr[8] << _loss_table->item(i, cobjreal)->text().trimmed();
			}
			else {
				lossStr[6] << "None";
				lossStr[7] << "None";
				lossStr[8] << "None";
			}
			lossStr[9] << _loss_table->item(i, cobjimag)->text().trimmed();
			lossStr[10] << _loss_table->item(i, clossweight)->text().trimmed();
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
	for (int i = 0; i < _vars_table->rowCount(); i++) {
		varKey = _vars_table->item(i, varnote)->whatsThis().trimmed();
		varNote = _vars_table->item(i, varnote)->text().trimmed();
		varValue = QString("[%1,%2]").arg(_vars_table->item(i, varmin)->text().trimmed())
			.arg(_vars_table->item(i, varmax)->text().trimmed());
		QJsonObject itemobj;
		itemobj.insert("note", varNote);
		itemobj.insert(varKey, varValue);
		mvars_obj.insert(varKey, itemobj);
	}

	//global and algorithm setting
	QJsonObject node_thread_obj;
	for (int i = 0; i < _node_vars_table->rowCount(); ++i) {
		QString nodeOrIp, core, strUID;
		QJsonObject ncitemobj;
		strUID = QUuid::createUuid().toString();
		nodeOrIp = _node_vars_table->item(i, nodeFlag)->text().trimmed();
		core = _node_vars_table->item(i, coreFlag)->text().trimmed();
		ncitemobj.insert("node", nodeOrIp);
		ncitemobj.insert("threads", core);
		node_thread_obj.insert(strUID, ncitemobj);
	}
	mglobal_obj.insert("ThreadNum", QString::number(_thread_edit->text().trimmed().toInt() + 1));
	mglobal_obj.insert("ALGORITHM_NAME", _algorithm->name);
	mglobal_obj.insert("PROBLEM_NAME", _atn_problem->name);
	mglobal_obj.insert("NodeAndThread", node_thread_obj);
	mglobal_obj.insert("outfilepath", QString("%1/outfilepath").arg(sysParam["CurrentOptimizePath"]));
	mglobal_obj.insert("outhfsspath", QString("%1/outhfsspath").arg(sysParam["CurrentOptimizePath"]));

	QString algKey, algValue, algNote;
	for (int i = 0; i < _alg_vars_table->rowCount(); i++) {
		algKey = _alg_vars_table->item(i, keyFlag)->whatsThis().trimmed();
		algNote = _alg_vars_table->item(i, keyFlag)->text().trimmed();
		algValue = QString("%1").arg(_alg_vars_table->item(i, valueFlag)->text().trimmed());
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
    QString algorithm_json_path = QString("%1/%2_conf.json").arg(current_optimize_path).arg(_algorithm->name);

    if((parseJson::write(problem_json_path, &_obj)
              && parseJson::write(global_json_path, &mglobal_obj)
              && parseJson::write(algorithm_json_path, &malgorithm_obj)))
        this->close();
    else
		QMessageBox::critical(0, QString("Error"), QString("save failed."));
}