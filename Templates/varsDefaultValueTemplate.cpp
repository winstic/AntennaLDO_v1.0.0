#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "varsDefaultValueTemplate.h"

varsDefaultValueTemplate::varsDefaultValueTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent) 
	: iTemplate(parent), _atn_problem(atn_problem), _obj(obj) {
	_atn_image_label = new QLabel(this);
	_vars_table = new tableTemplate();
	_vars_table->setColumnCount(3);
	QStringList header;
	header << "变量" << "参数值" << "单位";
	_vars_table->setHorizontalHeaderLabels(header);
	_vars_table->horizontalHeader()->setSectionsClickable(false);
	_vars_table->horizontalHeader()->setSectionResizeMode(varvalue, QHeaderView::Stretch);
	_vars_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	_vars_table->horizontalHeader()->resizeSection(0, 120);        //setting first column width is 120
	_vars_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	_vars_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	_vars_table->setShowGrid(false);                               //setting no grid line
	_vars_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_vars_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_vars_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
	//_vars_table->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	_vars_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit

	initDefaultData();
	initLayout();
}

void varsDefaultValueTemplate::initDefaultData() {
	QJsonObject vars_value_obj = parseJson::getSubJsonObj(*_obj, "varsValue");
	if (vars_value_obj.isEmpty()) {
		qCritical("get 'varsValue' json object field.");
		return;
	}
	for (QJsonObject::iterator iter = vars_value_obj.begin(); iter != vars_value_obj.end(); ++iter) {
		_default_vars[iter.key()] = iter.value().toString().trimmed();
	}
	QJsonObject vars_range_obj = parseJson::getSubJsonObj(*_obj, "variables");
	if (vars_range_obj.isEmpty()) {
		qCritical("get 'variables' json object field.");
		return;
	}
	QJsonObject var_obj;
	QString var_key;
	QStringList var_value;
	int row_number = 0, value_list_length;
	double real_value;	
	QRegExp rx("^(-?\\d+)(\\.\\d+)?$");
	QRegExpValidator float_valid(rx);      //float
	QSignalMapper* signals_map_slider = new QSignalMapper;	//use signalmaper manage signals in table
	QSignalMapper* signals_map_unit = new QSignalMapper;	//use signalmaper manage signals in table
	_vars_table->setRowCount(vars_range_obj.count());

	for (QJsonObject::iterator iter = vars_range_obj.begin(); iter != vars_range_obj.end(); ++iter) {
		// iter format: "W1":{"note" : "介质板宽度W1(m)", "W1" : "0.025"}
		var_key = iter.key();
		var_obj = iter.value().toObject();   //like {"note": "上贴片坐标y1(m)", "y1": "[-0.0115,0]"}
											 //get note infomation
		QString key_note = var_obj.value("note").toString().trimmed();
		_vars_table->insert2table(row_number, varnote, key_note);
		_vars_table->item(row_number, varnote)->setWhatsThis(var_key);
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
			v_layout->setMargin(0);
			cell_widget->setLayout(v_layout);
			//test begin
			//QLineEdit *findEdit = cellWidget->findChild<QLineEdit *>();
			//qDebug() << "findEdit: " << findEdit->text();
			//test end
			_vars_table->setCellWidget(row_number, varvalue, cell_widget);			
			connect(var_slider, SIGNAL(valueChanged(int)), signals_map_slider, SLOT(map()));
			signals_map_slider->setMapping(var_slider, QString("%1#%2#%3").arg(row_number).arg(up_value).arg(low_value));
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
		_vars_table->setCellWidget(row_number, 2, unit_widget);

		//map combobox signal		
		connect(unit_combox, SIGNAL(currentIndexChanged(int)), signals_map_unit, SLOT(map()));
		signals_map_unit->setMapping(unit_combox, QString("%1").arg(row_number));
		//in 'rownumber'th row, save default unitComBo current data
		_vars_unit.insert(row_number, unit_combox->currentData(ROLE_MARK_UNIT).toInt());
		row_number++;
	}

	connect(signals_map_slider, SIGNAL(mapped(QString)), this, SLOT(slot_sliderValueChange(QString)));
	connect(signals_map_unit, SIGNAL(mapped(QString)), this, SLOT(slot_unitChange(QString)));

	//!add picture
	QPixmap pm = QPixmap(_atn_problem->pImage);
	if (pm.width() > INTRODUCEIMAGEPIX)
		_atn_image_label->setPixmap(pm.scaledToWidth(INTRODUCEIMAGEPIX));
	else
		_atn_image_label->setPixmap(pm);
}

void varsDefaultValueTemplate::initLayout() {
	QHBoxLayout* h_layout = new QHBoxLayout;
	h_layout->addWidget(_vars_table, 1);
	h_layout->addWidget(_atn_image_label, 1);
	_layout = h_layout;
}

void varsDefaultValueTemplate::initSliderSheet(QString& sheet) {
	sheet = QString("  \
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


QLayout* varsDefaultValueTemplate::getLayout() {
	return _layout;
}

//update json obj
void varsDefaultValueTemplate::updateJObj() {
	QJsonObject mvars_value_obj;
	QString var_key, var_value;
	for (int i = 0; i < _vars_table->rowCount(); i++) {
		var_key = _vars_table->item(i, varnote)->whatsThis().trimmed();
		var_value = _vars_table->cellWidget(i, varvalue)->findChild<QLineEdit *>()->text().trimmed();
		mvars_value_obj.insert(var_key, var_value);
	}
	_obj->insert("varsValue", mvars_value_obj);
}

//slots
void varsDefaultValueTemplate::slot_LinetextChange(QString text) {
	//qDebug() << text;
}

void varsDefaultValueTemplate::slot_sliderValueChange(QString s_parameter) {
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

void varsDefaultValueTemplate::slot_unitChange(QString pos) {
	Q_ASSERT(!_vars_unit.isEmpty());
	int row = pos.toInt();
	int current_unit_data = _vars_unit[row];
	QComboBox* select_combox = _vars_table->cellWidget(row, 2)->findChild<QComboBox *>();
	int new_unit_data = select_combox->currentData(ROLE_MARK_UNIT).toInt();
	//unit conversion
	//when "newUnitData == currentUnitData", do nothing
	if (new_unit_data == current_unit_data)
		return;
	//get QLineEdit widget
	QLineEdit* curr_line_edit = _vars_table->cellWidget(row, varvalue)->findChild<QLineEdit *>();
	double pre_value, curr_value;
	pre_value = curr_line_edit->text().trimmed().toDouble();
	if (current_unit_data != MARK_UNIT_LAMBDA && new_unit_data != MARK_UNIT_LAMBDA ) {		
		curr_value = unitConversion(pre_value, current_unit_data, new_unit_data);
	}
	else {
		//unit conversion with lambda
		curr_value = unitConversion(pre_value, current_unit_data, new_unit_data, _atn_problem->max_frequency);
	}
	curr_line_edit->setText(QString::number(curr_value));
	//update unit item user data
	_vars_unit[row] = new_unit_data;
}


varsDefaultValueTemplate::~varsDefaultValueTemplate() {}