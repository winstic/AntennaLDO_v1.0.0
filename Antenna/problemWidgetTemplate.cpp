#include "problemWidgetTemplate.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"

problemTemplate::problemTemplate(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QWidget(parent),
_atn_problem(atn_problem), _obj(obj) {}

void problemTemplate::initFrequencyWidget(QLayout* layout) {
	//initizlize necessary parameters
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
	initSweepNDPMComBox();

	//setting some input rules
	QRegExpValidator nonNegFloatValid(QRegExp("^(\\d+)(\\.\\d+)?$"));    //non negative float
	QRegExpValidator posIntValid(QRegExp("^[0-9]*[1-9][0-9]*$"));		//positive int
	_frequency_low_edit->setValidator(&nonNegFloatValid);
	_frequency_up_edit->setValidator(&nonNegFloatValid);
	_frequency_num_edit->setValidator(&posIntValid);

	//setting default data
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

	//layout
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
	layout = &frequency_layout;
}

void problemTemplate::initSweepNDPMComBox() {
	_sweep_type_combox->addItem("等差");
	_sweep_type_combox->addItem("等比");

	_polarization_combox->addItem("总增益");
	_polarization_combox->addItem("左旋");
	_polarization_combox->addItem("右旋");
	_polarization_combox->addItem("水平");
	_polarization_combox->addItem("垂直");
}

void problemTemplate::initFarFieldWidget(QLayout* layout) {
	//initizlize necessary parameters
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

	//setting some input rules
	QRegExpValidator nonNegFloatValid(QRegExp("^(\\d+)(\\.\\d+)?$"));    //non negative float
	QRegExpValidator floatValid(QRegExp("^-?(180|1?[0-7]?\\d(\\.\\d+)?)$"));      //float [-180. 180]
	_theta_low_edit->setValidator(&floatValid);
	_theta_up_edit->setValidator(&floatValid);
	_theta_step_edit->setValidator(&nonNegFloatValid);
	_phi_low_edit->setValidator(&floatValid);
	_phi_up_edit->setValidator(&floatValid);
	_phi_step_edit->setValidator(&nonNegFloatValid);

	//setting default data
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

	//layout
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
	layout = &far_field_layout;
}

void problemTemplate::initGainWidget(QLayout* layout) {
	//initialize necessary parameters
	_gain_table = new QTableWidget(this);
	_gain_table->setColumnCount(8);
	QStringList header;
	header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "增益(dB)" << "权值";
	_gain_table->setHorizontalHeaderLabels(header);
	//gainTable->setStyleSheet("QTableView {selection-background-color: grey;}");
	_gain_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//setting default data
	QJsonObject gain_obj = parseJson::getSubJsonObj(_obj, "GainSetting");
	if (gain_obj.isEmpty()) {
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

	//layout
	layout->addWidget(_gain_table);
}

void problemTemplate::initAxialWidget(QLayout* layout) {
	//initialize necessary parameters
	_axial_table = new QTableWidget(this);
	_axial_table->setColumnCount(8);
	QStringList header;
	header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "轴比(dB)" << "权值";
	_axial_table->setHorizontalHeaderLabels(header);
	_axial_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//setting default data
	QJsonObject axial_obj = parseJson::getSubJsonObj(_obj, "AxialratioSetting");
	if (axial_obj.isEmpty()) {
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

	//layout
	layout->addWidget(_axial_table);
}

void problemTemplate::initLossWidget(QLayout* layout) {
	//initialize necessary parameters
	_loss_table = new QTableWidget(this);
	_loss_table->setColumnCount(9);
	QStringList header;
	header << "Z0实部" << "Z0虚部" << "损失方式" << "优化方式" << "误差实部" << "误差虚部" << "值实部" << "值虚部" << "权值";
	_loss_table->setHorizontalHeaderLabels(header);
	_loss_table->resizeColumnToContents(6);
	_loss_table->resizeColumnToContents(7);
	_loss_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//setting default data
	QJsonObject loss_obj = parseJson::getSubJsonObj(_obj, "VSWRSetting");
	if (loss_obj.isEmpty()) {
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

	//layout
	layout->addWidget(_loss_table);
}

void problemTemplate::initOptimalTypeComBox(QComboBox *combox) {
	combox->addItem("max");
	combox->addItem("min");
	combox->addItem("delta");
	combox->addItem("None");
	combox->setCurrentIndex(3);
}

void problemTemplate::initLossTypeComBox(QComboBox *combox) {
	combox->addItem(tr("驻波比"));
	combox->addItem(tr("S11(dB)"));
	combox->addItem(tr("阻抗"));
	combox->addItem(tr("None"));
	combox->setCurrentIndex(3);
}

void problemTemplate::initVariablesRangeWidget(QLayout* layout) {
	//initialize necessary parameters
	_vars_range_table = new QTableWidget(this);
	_vars_range_table->setColumnCount(4);
	QStringList header;
	header << "变量" << "最小值" << "最大值" << "单位";
	_vars_range_table->setHorizontalHeaderLabels(header);
	_vars_range_table->horizontalHeader()->setSectionsClickable(false);
	//varTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_vars_range_table->horizontalHeader()->setSectionResizeMode(varnote, QHeaderView::Stretch);
	_vars_range_table->horizontalHeader()->setSectionResizeMode(varunit, QHeaderView::ResizeToContents);
	_vars_range_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	_vars_range_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_vars_range_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_vars_range_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
	_vars_range_table->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	_vars_range_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit

	//setting default data
	QJsonObject variables_obj = parseJson::getSubJsonObj(_obj, "variables");
	if (variables_obj.isEmpty()) {
		qCritical("get 'variables' json object field.");
		return;
	}
	QJsonObject var_obj;
	QString var_key;
	QStringList var_value;
	int row_number = 0, value_list_length;
	QSignalMapper signals_map_unit;	//use signalmaper manage signals in table

	_vars_range_table->setRowCount(variables_obj.count());

	for (QJsonObject::iterator iter = variables_obj.begin(); iter != variables_obj.end(); ++iter) {
		// iter format: "W1":{"note" : "介质板宽度W1(m)", "W1" : "0.025"}
		var_key = iter.key();
		var_obj = iter.value().toObject();   //like {"note": "上贴片坐标y1(m)", "y1": "[-0.0115,0]"}
											 //get note infomation
		QString key_note = var_obj.value("note").toString().trimmed();
		insert2table(_vars_range_table, row_number, varnote, key_note);
		_vars_range_table->item(row_number, varnote)->setWhatsThis(var_key);

		var_value = dataPool::str2list(var_obj.value(var_key).toString().trimmed());
		value_list_length = var_value.length();
		insert2table(_vars_range_table, row_number, varmin, var_value[0]);
		if (value_list_length == 1)
			insert2table(_vars_range_table, row_number, varmax, var_value[0]);
		else
			insert2table(_vars_range_table, row_number, varmax, var_value[1]);

		QComboBox *unit_combox = new QComboBox();
		initUnitComBo(unit_combox);
		_vars_range_table->setCellWidget(row_number, varunit, unit_combox);
		//map combobox signal
		connect(unit_combox, SIGNAL(currentIndexChanged(int)), &signals_map_unit, SLOT(map()));
		signals_map_unit.setMapping(unit_combox, QString("%1-%2").arg(row_number).arg(varunit));
		//in 'rownumber'th row of table, save default unitComBo current data
		_vars_unit.insert(row_number, unit_combox->currentData(ROLE_MARK_UNIT).toInt());
		row_number++;
	}
	connect(&signals_map_unit, SIGNAL(mapped(QString)), this, SLOT(slot_unitChange4Optimize(QString)));

	//!add picture
	QLabel* atn_image_label = new QLabel(this);
	QPixmap pm = QPixmap(_atn_problem->pImage);
	if (pm.width() > INTRODUCEIMAGEPIX)
		atn_image_label->setPixmap(pm.scaledToWidth(INTRODUCEIMAGEPIX));
	else
		atn_image_label->setPixmap(pm);

	//layout
	QHBoxLayout h_layout;
	h_layout.addWidget(_vars_range_table, 1);
	h_layout.addWidget(atn_image_label, 1);
	layout = &h_layout;
}

void problemTemplate::initVariablesValueWidget(QLayout* layout) {
	//initialize necessary parameters
	_vars_value_table = new QTableWidget(this);
	_vars_value_table->setColumnCount(3);
	QStringList header;
	header << "变量" << "参数值" << "单位";
	_vars_value_table->setHorizontalHeaderLabels(header);
	_vars_value_table->horizontalHeader()->setSectionsClickable(false);
	_vars_value_table->horizontalHeader()->setSectionResizeMode(varvalue, QHeaderView::Stretch);
	_vars_value_table->horizontalHeader()->setSectionResizeMode(varunit, QHeaderView::ResizeToContents);
	_vars_value_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	//varTable->verticalHeader()->setDefaultSectionSize(0);      //setting row spacing
	_vars_value_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	_vars_value_table->setShowGrid(false);                               //setting no grid line
	_vars_value_table->verticalHeader()->setVisible(false);              //setting no vertical header
	_vars_value_table->horizontalHeader()->resizeSection(0, 120);        //setting first column width is 150
	_vars_value_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_vars_value_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_vars_value_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
	_vars_value_table->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	_vars_value_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit

	//setting default parameters
	QJsonObject vars_value_obj = parseJson::getSubJsonObj(_obj, "varsValue");
	if (vars_value_obj.isEmpty()) {
		qCritical("get 'varsValue' json object field.");
		return;
	}
	for (QJsonObject::iterator iter = vars_value_obj.begin(); iter != vars_value_obj.end(); ++iter) {
		_default_vars[iter.key()] = iter.value().toString().trimmed();
	}
	QJsonObject vars_range_obj = parseJson::getSubJsonObj(_obj, "variables");
	if (vars_range_obj.isEmpty()) {
		qCritical("get 'variables' json object field.");
		return;
	}
	QJsonObject var_obj;
	QString var_key;
	QStringList var_value;
	int row_number = 0, value_list_length;
	double real_value;
	QSignalMapper signals_map_slider, signals_map_unit;	//use signalmaper manage signals in table
	QRegExp rx("^(-?\\d+)(\\.\\d+)?$");
	QRegExpValidator float_valid(rx);      //float

	_vars_value_table->setRowCount(vars_range_obj.count());

	for (QJsonObject::iterator iter = vars_range_obj.begin(); iter != vars_range_obj.end(); ++iter) {
		// iter format: "W1":{"note" : "介质板宽度W1(m)", "W1" : "0.025"}
		var_key = iter.key();
		var_obj = iter.value().toObject();   //like {"note": "上贴片坐标y1(m)", "y1": "[-0.0115,0]"}
											 //get note infomation
		QString key_note = var_obj.value("note").toString().trimmed();
		insert2table(_vars_value_table, row_number, varnote, key_note);
		// init text edit and layout
		var_value = dataPool::str2list(var_obj.value(var_key).toString().trimmed());
		value_list_length = var_value.length();
		QLineEdit *value_edit = new QLineEdit(this);
		value_edit->setValidator(&float_valid);
		if (value_list_length == 1) {
			//no range
			value_edit->setText(var_value[0]);
			value_edit->setReadOnly(true);
			_vars_value_table->setCellWidget(row_number, varvalue, value_edit);
		}
		else {
			QWidget *cell_widget = new QWidget(this);
			QVBoxLayout *v_layout = new QVBoxLayout(this);
			QSlider *var_slider = new QSlider(Qt::Horizontal, this);
			QString sheet;
			initSliderSheet(sheet);
			var_slider->setStyleSheet(sheet);
			var_slider->setMinimum(0);
			var_slider->setMaximum(100);
			var_slider->setSingleStep(1);
			//init slider
			double low_value = QString(var_value[0]).trimmed().toDouble();
			double up_value = QString(var_value[1]).trimmed().toDouble();
			real_value = _default_vars[var_key].trimmed().toDouble();
			if (up_value == low_value) {
				var_slider->setValue(100);
				var_slider->setEnabled(false);
			}
			else {
				int sliderValue = 100 * (real_value - low_value) / (up_value - low_value);
				var_slider->setValue(sliderValue);
			}
			//!conversion slider value and edit value
			value_edit->setText(QString::number(real_value));
			v_layout->addWidget(value_edit);
			v_layout->addWidget(var_slider);
			//design inner space
			v_layout->setSpacing(0);
			// design outer space
			//vLayout->setMargin(0);
			cell_widget->setLayout(v_layout);
			//test begin
			//QLineEdit *findEdit = cellWidget->findChild<QLineEdit *>();
			//qDebug() << "findEdit: " << findEdit->text();
			//test end
			_vars_value_table->setCellWidget(row_number, varvalue, cell_widget);

			connect(var_slider, SIGNAL(valueChanged(int)), &signals_map_slider, SLOT(map()));
			signals_map_slider.setMapping(var_slider, QString("%1#%2#%3").arg(row_number).arg(up_value).arg(low_value));
		}
		connect(value_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_LinetextChange(QString)));

		//valueEdit->installEventFilter(this);        //install filter in this dialog(在对话框上为QLineEdit安装过滤器)
		QWidget* unit_widget = new QWidget(this);
		QHBoxLayout* unit_layout = new QHBoxLayout(this);
		QComboBox* unit_combox = new QComboBox(this);
		unit_combox->setFixedHeight(25);
		initUnitComBo(unit_combox);
		unit_layout->addWidget(unit_combox);
		unit_widget->setLayout(unit_layout);
		_vars_value_table->setCellWidget(row_number, varunit, unit_widget);

		//map combobox signal
		connect(unit_combox, SIGNAL(currentIndexChanged(int)), &signals_map_unit, SLOT(map()));
		signals_map_unit.setMapping(unit_combox, QString("%1").arg(row_number));
		//in 'rownumber'th row, save default unitComBo current data
		_vars_unit.insert(row_number, unit_combox->currentData(ROLE_MARK_UNIT).toInt());
		row_number++;
	}

	connect(&signals_map_slider, SIGNAL(mapped(QString)), this, SLOT(slot_sliderValueChange(QString)));
	connect(&signals_map_unit, SIGNAL(mapped(QString)), this, SLOT(slot_unitChange4Design(QString)));

	//!add picture
	QLabel* atn_image_label = new QLabel(this);
	QPixmap pm = QPixmap(_atn_problem->pImage);
	if (pm.width() > INTRODUCEIMAGEPIX)
		atn_image_label->setPixmap(pm.scaledToWidth(INTRODUCEIMAGEPIX));
	else
		atn_image_label->setPixmap(pm);
	//!
	QHBoxLayout h_layout;
	h_layout.addWidget(_vars_value_table, 1);
	h_layout.addWidget(atn_image_label, 1);
	layout = &h_layout;
}

void problemTemplate::initSliderSheet(QString& sheet) {
	sheet =  QString("  \
         QSlider\
         {     \
            spacing: 0px;\
            min-height:8px;\
            max-height:8px;\
         }\
         QSlider::add-page:Horizontal\
         {     \
            background-color: rgb(222, 231, 234);\
            height:8px;\
         }\
         QSlider::sub-page:Horizontal \
        {\
            background-color: rgb(37, 168, 198);\
            height:8px;\
         }\
        QSlider::groove:Horizontal \
        {\
            background:transparent;\
            height:8px;\
        }\
        QSlider::handle:Horizontal \
        {\
             height: 13px;\
            width:13px;\
            border-image: url(./images/dot_16px.png);\
             margin: 0px; \
         }\
        ");
}

double problemTemplate::unitConversion(double source_data, int pre_unit, int curr_unit) {
	return source_data * qPow(10, pre_unit - curr_unit);
}

void problemTemplate::initUnitComBo(QComboBox *comb) {
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

void problemTemplate::insert2table(QTableWidget *table, const int &row, const int &clomun, const QString &item_value) {
	QTableWidgetItem *tableItem = new QTableWidgetItem(item_value);
	tableItem->setTextAlignment(Qt::AlignCenter);
	table->setItem(row, clomun, tableItem);
}

//slots function
void problemTemplate::slot_gainChangeOptimaltype(QString pos) {
	QStringList coordinates = pos.split("-");
	int row = coordinates[0].toInt();
	int col = coordinates[1].toInt();
	QComboBox *selectCombox = (QComboBox *)_gain_table->cellWidget(row, col);
	//setting edit when optimize type is delta
	if (selectCombox->currentIndex() == 2)
		_gain_table->item(row, cdelta)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	else
		_gain_table->item(row, cdelta)->setFlags(Qt::NoItemFlags);
}

void problemTemplate::slot_axialChangeOptimaltype(QString pos) {
	QStringList coordinates = pos.split("-");
	int row = coordinates[0].toInt();
	int col = coordinates[1].toInt();
	QComboBox *selectCombox = (QComboBox *)_axial_table->cellWidget(row, col);
	//setting edit when optimize type is delta
	if (selectCombox->currentIndex() == 2) {
		qDebug() << selectCombox->currentIndex();
		_axial_table->item(row, cdelta)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	}
	else
		_axial_table->item(row, cdelta)->setFlags(Qt::NoItemFlags);
}

void problemTemplate::slot_lossChangeType(QString pos) {
	QStringList coordinates = pos.split("-");
	int row = coordinates[0].toInt();
	int col = coordinates[1].toInt();
	QComboBox *selectCombox = (QComboBox *)_loss_table->cellWidget(row, col);
	//setting edit when optimize type is delta
	if (col == clossoptimaltype) {
		if (2 == selectCombox->currentIndex()) {
			_loss_table->item(row, cdeltareal)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			_loss_table->item(row, cdeltaimag)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		}
		else {
			_loss_table->item(row, cdeltareal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(row, cdeltaimag)->setFlags(Qt::NoItemFlags);
		}
	}
	if (col == closstype) {
		if (2 == selectCombox->currentIndex())
			_loss_table->item(row, cobjimag)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		else
			_loss_table->item(row, cobjimag)->setFlags(Qt::NoItemFlags);
	}
}

void problemTemplate::slot_unitChange4Optimize(QString pos) {
	Q_ASSERT(!_vars_unit.isEmpty());
	QStringList coordinates = pos.split("-");
	int row = coordinates.at(0).toInt();
	int col = coordinates.at(1).toInt();
	int currentUnitData = _vars_unit[row];
	QComboBox *selectCombox = (QComboBox *)_vars_range_table->cellWidget(row, col);
	qDebug() << selectCombox->currentText();
	int newUnitData = selectCombox->currentData(ROLE_MARK_UNIT).toInt();
	//qDebug() << varTable->item(row, varnote)->whatsThis();
	if (currentUnitData != MARK_UNIT_LAMBDA && newUnitData != MARK_UNIT_LAMBDA &&
		newUnitData != currentUnitData) {
		double preValueMin = _vars_range_table->item(row, varmin)->text().trimmed().toDouble();
		double preValueMax = _vars_range_table->item(row, varmax)->text().trimmed().toDouble();
		double currentValueMin = unitConversion(preValueMin, currentUnitData, newUnitData);
		double currentValueMax = unitConversion(preValueMax, currentUnitData, newUnitData);
		qDebug() << currentValueMin << currentValueMax;
		insert2table(_vars_range_table, row, varmin, QString::number(currentValueMin));
		insert2table(_vars_range_table, row, varmax, QString::number(currentValueMax));
	}
	_vars_unit[row] = newUnitData;
}

void problemTemplate::slot_LinetextChange(QString text) {
	//qDebug() << text;
}

void problemTemplate::slot_sliderValueChange(QString s_parameter) {
	QStringList parameterList = s_parameter.split("#");
	int row_number = parameterList.at(0).toInt();        //table rowth
	double upper = parameterList.at(1).toDouble();
	double lower = parameterList.at(2).toDouble();
	//get widget
	QSlider* select_slider = _vars_value_table->cellWidget(row_number, varvalue)->findChild<QSlider *>();
	QLineEdit* select_line_edit = _vars_value_table->cellWidget(row_number, varvalue)->findChild<QLineEdit *>();
	int s_value = select_slider->value();
	//get new edit value by slider value
	double new_line_edit_value = (upper - lower) / 100.0 * s_value + lower;
	select_line_edit->setText(QString::number(new_line_edit_value));
}

void problemTemplate::slot_unitChange4Design(QString pos) {
	Q_ASSERT(!_vars_unit.isEmpty());
	int row = pos.toInt();
	int current_unit_data = _vars_unit[row];
	QComboBox* select_combox = _vars_value_table->cellWidget(row, varunit)->findChild<QComboBox *>();
	int new_unit_data = select_combox->currentData(ROLE_MARK_UNIT).toInt();
	//unit conversion
	if (current_unit_data != MARK_UNIT_LAMBDA && new_unit_data != MARK_UNIT_LAMBDA &&
		new_unit_data != current_unit_data) {
		//get QLineEdit widget
		QLineEdit* curr_line_edit = _vars_value_table->cellWidget(row, varvalue)->findChild<QLineEdit *>();
		double pre_value = curr_line_edit->text().trimmed().toDouble();
		double curr_value = unitConversion(pre_value, current_unit_data, new_unit_data);
		curr_line_edit->setText(QString::number(curr_value));
	}
	//update unit item user data
	_vars_unit[row] = new_unit_data;
}

problemTemplate::~problemTemplate(){}