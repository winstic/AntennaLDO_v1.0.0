#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "../Utility/commonStyle.h"
#include "../Utility/parseJson.h"
#include "sliderTemplate.h"
#include "varsDefaultValueTemplate.h"

varsDefaultValueTemplate::varsDefaultValueTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent) 
	: iTemplate(parent), _atn_problem(atn_problem), _obj(obj), _is_valid(true) {
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
		checkInfo->code = eOther;
		checkInfo->message = "问题json文件格式不正确。";
		_is_valid = false;
		emit signal_checkValid();
		return;
	}
	for (QJsonObject::iterator iter = vars_value_obj.begin(); iter != vars_value_obj.end(); ++iter) {
		_default_vars[iter.key()] = iter.value().toString().trimmed();
	}
	QJsonObject vars_range_obj = parseJson::getSubJsonObj(*_obj, "variables");
	if (vars_range_obj.isEmpty()) {
		qCritical("get 'variables' json object field.");
		checkInfo->code = eOther;
		checkInfo->message = "问题json文件格式不正确。";
		_is_valid = false;
		emit signal_checkValid();
		return;
	}
	QJsonObject var_obj;
	QString var_key;
	QStringList var_value;
	int row_number = 0;
	double real_value;	
	QSignalMapper* signals_map_slider = new QSignalMapper;	//use signalmaper manage signals in table
	QSignalMapper* signals_map_line = new QSignalMapper;	
	QSignalMapper* signals_map_unit = new QSignalMapper;	
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
		if (var_value.length() != 2) {
			qCritical("get 'variables' json unregular.");
			checkInfo->code = eOther;
			checkInfo->message = "问题json文件格式不规则（'variables'变量未设上下限）。";
			_is_valid = false;
			emit signal_checkValid();
			return;
		}
		QLineEdit *value_edit = new QLineEdit(this);		
		value_edit->setValidator(getFloatReg());
		
		QWidget *cell_widget = new QWidget(this);
		QVBoxLayout *v_layout = new QVBoxLayout(this);
		//init slider
		double low_value = QString(var_value[0]).trimmed().toDouble();
		double up_value = QString(var_value[1]).trimmed().toDouble();
		sliderTemplate *value_slider = new sliderTemplate(low_value, up_value);
			
		real_value = _default_vars[var_key].trimmed().toDouble();
		if (up_value == low_value) {
			value_slider->setValue(MAXSLIDERNUMBER);
			value_slider->setEnabled(false);
			value_edit->setEnabled(false);
		}
		else {
			int sliderValue = MINSLIDERNUMBER + (MAXSLIDERNUMBER - MINSLIDERNUMBER) * (real_value - low_value) / (up_value - low_value);
			value_slider->setValue(sliderValue);
		}
		//!conversion slider value and edit value
		value_edit->setText(QString::number(real_value));
		value_edit->setProperty(VARLOW, low_value);
		value_edit->setProperty(VARUP, up_value);
		v_layout->addWidget(value_edit);
		v_layout->addWidget(value_slider);
		//design inner space
		v_layout->setSpacing(0);
		// design outer space
		v_layout->setMargin(0);
		cell_widget->setLayout(v_layout);
		_vars_table->setCellWidget(row_number, varvalue, cell_widget);			
		connect(value_slider, SIGNAL(valueChanged(int)), signals_map_slider, SLOT(map()));
		signals_map_slider->setMapping(value_slider, QString("%1").arg(row_number));

		connect(value_edit, SIGNAL(textChanged(QString)), signals_map_line, SLOT(map()));
		signals_map_line->setMapping(value_edit, QString("%1").arg(row_number));

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

		//当文本框输入改变时，触发校验信号；
		connect(value_edit, SIGNAL(textChanged(QString)), this, SIGNAL(signal_checkValid()));
		row_number++;
	}

	connect(signals_map_slider, SIGNAL(mapped(QString)), this, SLOT(slot_sliderValueChange(QString)));
	//connect(signals_map_line, SIGNAL(mapped(QString)), this, SLOT(slot_LinetextChange(QString)));
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

QLayout* varsDefaultValueTemplate::getLayout() {
	return _layout;
}

//check input
bool varsDefaultValueTemplate::checkInputValid() {
	if (!_is_valid) return false;
	QLineEdit* curr_edit;
	QString var_value;
	for (int i = 0; i < _vars_table->rowCount(); i++) {
		curr_edit = _vars_table->cellWidget(i, varvalue)->findChild<QLineEdit *>();
		var_value = curr_edit->text().trimmed();
		if (var_value.isEmpty() || var_value.isNull()) {
			checkInfo->code = eNull;
			checkInfo->message = "几何结构参数不能为空。";
			commonStyle::setLineEditWarningStyle(curr_edit);
			return false;
		}
		if (var_value == "-") {
			checkInfo->code = eInvalid;
			checkInfo->message = "几何结构参数输入不完整。";
			commonStyle::setLineEditWarningStyle(curr_edit);
			return false;
		}
		double var_value_d = var_value.toDouble();
		double var_value_low_d = curr_edit->property(VARLOW).toDouble();
		double var_value_up_d = curr_edit->property(VARUP).toDouble();
		//注意浮点数比较
		if ((var_value_d + 0.000001) < var_value_low_d || (var_value_d - 0.000001) > var_value_up_d) {
			checkInfo->code = eInvalid;
			checkInfo->message = QString("几何结构参数超出上下界范围[%1,%2]。").arg(var_value_low_d).arg(var_value_up_d);
			commonStyle::setLineEditWarningStyle(curr_edit);
			return false;
		}
		commonStyle::clearLineEditWarningStyle(curr_edit);
	}
	return true;
}

//update json obj
void varsDefaultValueTemplate::updateJObj() {
	QJsonObject mvars_value_obj;
	QString var_key;
	double var_value;
	int unit_data;
	for (int i = 0; i < _vars_table->rowCount(); i++) {
		var_key = _vars_table->item(i, varnote)->whatsThis().trimmed();
		var_value = _vars_table->cellWidget(i, varvalue)->findChild<QLineEdit *>()->text().trimmed().toDouble();
		unit_data = _vars_table->cellWidget(i, 2)->findChild<QComboBox *>()->currentData(ROLE_MARK_UNIT).toInt();
		if(unit_data == MARK_UNIT_LAMBDA)
			var_value = unitConversion(var_value, unit_data, MARK_UNIT_M, _atn_problem->max_frequency);
		else if(unit_data != MARK_UNIT_M)
			var_value = unitConversion(var_value, unit_data, MARK_UNIT_M);
		mvars_value_obj.insert(var_key, QString::number(var_value));
	}
	_obj->insert("varsValue", mvars_value_obj);
}

//slots
void varsDefaultValueTemplate::slot_LinetextChange(QString pos) {
	int row_number = pos.toInt();        //table rowth
	//get widget
	QSlider* select_slider = _vars_table->cellWidget(row_number, varvalue)->findChild<QSlider *>();
	QLineEdit* select_line_edit = _vars_table->cellWidget(row_number, varvalue)->findChild<QLineEdit *>();
	double lower = select_line_edit->property(VARLOW).toDouble();
	double upper = select_line_edit->property(VARUP).toDouble();
	double line_value = select_line_edit->text().trimmed().toDouble();

	int slider_value = MINSLIDERNUMBER + (MAXSLIDERNUMBER - MINSLIDERNUMBER) * (line_value - lower) / (upper - lower);
	select_slider->setValue(slider_value);
}

void varsDefaultValueTemplate::slot_sliderValueChange(QString pos) {
	int row_number = pos.toInt();        //table rowth
	//get widget
	QSlider* select_slider = _vars_table->cellWidget(row_number, varvalue)->findChild<QSlider *>();
	QLineEdit* select_line_edit = _vars_table->cellWidget(row_number, varvalue)->findChild<QLineEdit *>();
	double lower = select_line_edit->property(VARLOW).toDouble();
	double upper = select_line_edit->property(VARUP).toDouble();
	int s_value = select_slider->value();
	//get new edit value by slider value
	double new_line_edit_value = (upper - lower) / (MAXSLIDERNUMBER - MINSLIDERNUMBER) * s_value + lower;
	select_line_edit->setText(QString::number(new_line_edit_value));
}

void varsDefaultValueTemplate::slot_unitChange(QString pos) {
	Q_ASSERT(!_vars_unit.isEmpty());
	int row = pos.toInt();
	QComboBox* select_combox = _vars_table->cellWidget(row, 2)->findChild<QComboBox *>();
	QLineEdit* curr_line_edit = _vars_table->cellWidget(row, varvalue)->findChild<QLineEdit *>();

	int current_unit_data = _vars_unit[row];
	int new_unit_data = select_combox->currentData(ROLE_MARK_UNIT).toInt();
	//unit conversion
	//when "newUnitData == currentUnitData", do nothing
	if (new_unit_data == current_unit_data)
		return;
	
	double pre_value, curr_value, low_value, up_value;
	pre_value = curr_line_edit->text().trimmed().toDouble();
	if (current_unit_data != MARK_UNIT_LAMBDA && new_unit_data != MARK_UNIT_LAMBDA ) {		
		curr_value = unitConversion(pre_value, current_unit_data, new_unit_data);
		low_value = unitConversion(curr_line_edit->property(VARLOW).toDouble(), current_unit_data, new_unit_data);
		up_value = unitConversion(curr_line_edit->property(VARUP).toDouble(), current_unit_data, new_unit_data);
	}
	else {
		//unit conversion with lambda
		curr_value = unitConversion(pre_value, current_unit_data, new_unit_data, _atn_problem->max_frequency);
		low_value = unitConversion(curr_line_edit->property(VARLOW).toDouble(), current_unit_data, new_unit_data, _atn_problem->max_frequency);
		up_value = unitConversion(curr_line_edit->property(VARUP).toDouble(), current_unit_data, new_unit_data, _atn_problem->max_frequency);
	}
	//注意顺序，当单位改变时会间接导致文本框输入值改变，会触发校验信号；所以先更新上下限值。
	curr_line_edit->setProperty(VARLOW, low_value);
	curr_line_edit->setProperty(VARUP, up_value);
	curr_line_edit->setText(QString::number(curr_value));
	//update unit item user data
	_vars_unit[row] = new_unit_data;
}


varsDefaultValueTemplate::~varsDefaultValueTemplate() {}