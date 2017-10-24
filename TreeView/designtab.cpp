#include "designtab.h"
#include "../Utility/global.h"
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"

designTab::designTab(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent) : QDialog(parent),
_atn_problem(atn_problem), _obj(obj){
    _tab_widget = new QTabWidget(this);
    setMinimumSize(880, 580);
    //remove help menu
    setWindowFlags(windowFlags() &~ Qt::WindowContextHelpButtonHint);
    _first_tab = new QWidget(this);
    _second_tab = new QWidget(this);
    _save_all_button = new QPushButton("保存所有", this);
    connect(_save_all_button, SIGNAL(clicked(bool)), this, SLOT(slot_saveAllButton(bool)));

    //!first tabwidget
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

    initSweepNDPMComBox();
	setRegexp();
    frequencySetting();
    farFieldSetting();
    initFirstTabLayout();
    //!

    //!second tab widget
	_vars_table = new QTableWidget(this);
	tableStyleSetting();
	initDefaultVars();
    initVariablesTable();
    //!
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

    this->setWindowTitle(tr("天线设计"));    
    this->setLayout(&layout);
}

//!first tab widget
void designTab::initSweepNDPMComBox(){
	_sweep_type_combox->addItem("等差");
	_sweep_type_combox->addItem("等比");

	_polarization_combox->addItem("总增益");
	_polarization_combox->addItem("左旋");
	_polarization_combox->addItem("右旋");
	_polarization_combox->addItem("水平");
	_polarization_combox->addItem("垂直");
}

void designTab::setRegexp() {
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

void designTab::frequencySetting(){
    QJsonObject frequency_obj = parseJson::getSubJsonObj(_obj, "FreSetting");
    if(frequency_obj.isEmpty()){
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

void designTab::farFieldSetting(){
    QJsonObject far_field_obj = parseJson::getSubJsonObj(_obj, "ThetaPhiStep");
    if(far_field_obj.isEmpty()){
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

void designTab::initFirstTabLayout(){
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

//!sceond widget
void designTab::tableStyleSetting(){
	_vars_table->setColumnCount(3);
    QStringList header;
    header << "变量" << "参数值" << "单位";
	_vars_table->setHorizontalHeaderLabels(header);
	_vars_table->horizontalHeader()->setSectionsClickable(false);
	_vars_table->horizontalHeader()->setSectionResizeMode(varvalue, QHeaderView::Stretch);
	_vars_table->horizontalHeader()->setSectionResizeMode(varunit, QHeaderView::ResizeToContents);
	_vars_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //varTable->verticalHeader()->setDefaultSectionSize(0);      //setting row spacing
	_vars_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	_vars_table->setShowGrid(false);                               //setting no grid line
	_vars_table->verticalHeader()->setVisible(false);              //setting no vertical header
	_vars_table->horizontalHeader()->resizeSection(0, 120);        //setting first column width is 150
	_vars_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_vars_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_vars_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
	_vars_table->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	_vars_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit
}

void designTab::initDefaultVars(){
    QJsonObject vars_value_obj = parseJson::getSubJsonObj(_obj, "varsValue");
    if(vars_value_obj.isEmpty()){
		qCritical("get 'varsValue' json object field.");
		return;
    }
	for (QJsonObject::iterator iter = vars_value_obj.begin(); iter != vars_value_obj.end(); ++iter) {
		_default_vars[iter.key()] = iter.value().toString().trimmed();
	}
}

void designTab::initUnitComBo(QComboBox* comb){
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

void designTab::initVariablesTable(){
    QJsonObject vars_range_obj = parseJson::getSubJsonObj(_obj, "variables");
    if(vars_range_obj.isEmpty()){
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

	_vars_table->setRowCount(vars_range_obj.count());

	for (QJsonObject::iterator iter = vars_range_obj.begin(); iter != vars_range_obj.end(); ++iter) {
		// iter format: "W1":{"note" : "介质板宽度W1(m)", "W1" : "0.025"}
		var_key = iter.key();
		var_obj = iter.value().toObject();   //like {"note": "上贴片坐标y1(m)", "y1": "[-0.0115,0]"}
											 //get note infomation
		QString key_note = var_obj.value("note").toString().trimmed();
		insert2table(row_number, varnote, key_note);
		// init text edit and layout
		var_value = dataPool::str2list(var_obj.value(var_key).toString().trimmed());
		value_list_length = var_value.length();
		QLineEdit *value_edit = new QLineEdit(this);
		value_edit->setValidator(&float_valid);
		if (value_list_length == 1) {
			//no range
			value_edit->setText(var_value[0]);
			value_edit->setReadOnly(true);
			_vars_table->setCellWidget(row_number, varvalue, value_edit);
		}
		else {
			QWidget *cell_widget = new QWidget(this);
			QVBoxLayout *v_layout = new QVBoxLayout(this);
			QSlider *var_slider = new QSlider(Qt::Horizontal, this);
			var_slider->setStyleSheet(getSliderSheet());
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
			_vars_table->setCellWidget(row_number, varvalue, cell_widget);

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
		_vars_table->setCellWidget(row_number, varunit, unit_widget);

		//map combobox signal
		connect(unit_combox, SIGNAL(currentIndexChanged(int)), &signals_map_unit, SLOT(map()));
		signals_map_unit.setMapping(unit_combox, QString("%1").arg(row_number));
		//in 'rownumber'th row, save default unitComBo current data
		_vars_unit.insert(row_number, unit_combox->currentData(ROLE_MARK_UNIT).toInt());
		row_number++;
	}

	connect(&signals_map_slider, SIGNAL(mapped(QString)), this, SLOT(slot_sliderValueChange(QString)));
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

	_second_tab->setLayout(&h_layout);
}

QString designTab::getSliderSheet(){
    return QString("  \
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

void designTab::insert2table(const int &row, const int &clomun, const QString &item_value) {
	QTableWidgetItem *table_item = new QTableWidgetItem(item_value);
	table_item->setTextAlignment(Qt::AlignCenter);
	_vars_table->setItem(row, clomun, table_item);
}

double designTab::unitConversion(double source_data, int pre_unit, int cur_unit) {
	//assert(preunit != unitlambda && curunit != unitlambda);
	return source_data * qPow(10, pre_unit - cur_unit);
}

//slots function
void designTab::slot_LinetextChange(QString text){
    //qDebug() << text;
}

void designTab::slot_sliderValueChange(QString s_parameter){
	QStringList parameterList = s_parameter.split("#");
	int row_number = parameterList.at(0).toInt();        //table rowth
	double upper = parameterList.at(1).toDouble();
	double lower = parameterList.at(2).toDouble();
	//get widget
	QSlider* select_slider = _vars_table->cellWidget(row_number, varvalue)->findChild<QSlider *>();
	QLineEdit* select_line_edit = _vars_table->cellWidget(row_number, varvalue)->findChild<QLineEdit *>();
	int s_value = select_slider->value();
	//get new edit value by slider value
	double new_line_edit_value = (upper - lower) / 100.0 * s_value + lower;
	select_line_edit->setText(QString::number(new_line_edit_value));
}

void designTab::slot_unitchange(QString pos){
	Q_ASSERT(!_vars_unit.isEmpty());
	int row = pos.toInt();
	int current_unit_data = _vars_unit[row];
	QComboBox* select_combox = _vars_table->cellWidget(row, varunit)->findChild<QComboBox *>();
	int new_unit_data = select_combox->currentData(ROLE_MARK_UNIT).toInt();
	//unit conversion
	if (current_unit_data != MARK_UNIT_LAMBDA && new_unit_data != MARK_UNIT_LAMBDA &&
		new_unit_data != current_unit_data) {
		//get QLineEdit widget
		QLineEdit* curr_line_edit = _vars_table->cellWidget(row, varvalue)->findChild<QLineEdit *>();
		double pre_value = curr_line_edit->text().trimmed().toDouble();
		double curr_value = unitConversion(pre_value, current_unit_data, new_unit_data);
		curr_line_edit->setText(QString::number(curr_value));
	}
	//update unit item user data
	_vars_unit[row] = new_unit_data;
}

void designTab::slot_saveAllButton(bool isChecked){
	QJsonObject mfre_obj, mfar_field_obj, mvars_value_obj;
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
