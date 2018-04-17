#pragma execution_character_set("utf-8")
#include "../Utility/macrodefined.h"
#include "../Utility/parseJson.h"
#include "modeInfo.h"

modelInfo::modelInfo(parsProblem* atn_problem, QWidget *parent) : QDialog(parent), _atn_problem(atn_problem) {
	//remove help menu
	this->setWindowFlags(windowFlags() &~Qt::WindowContextHelpButtonHint);
	setMinimumSize(SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	
	_obj = parseJson::getJsonObj(QString("%1/%2_conf.json").arg(_atn_problem->path).arg(_atn_problem->name));
	_vars_range_table = new tableTemplate();
	_vars_range_table->setFixedSize(400, 300);
	//_vars_range_table->setFixedWidth(400);
	_vars_table = new tableTemplate();

	initDefalultParametersRange();
	initDefalultParameters();
	initLayout();
}

void modelInfo::initLayout() {
	//title
	QLabel *title_label = new QLabel(QString("%1天线").arg(_atn_problem->name),this);
	setFirstTitle(title_label);

	//introduce
	QLabel *summary_title = new QLabel("1. 天线简介", this);
	setSecondTitle(summary_title);
	QLabel *atn_summary = new QLabel(this);
	atn_summary->setText(_atn_problem->info);
	
	QLabel *parameters_range_title = new QLabel("2. 默认几何结构参数范围", this);
	setSecondTitle(parameters_range_title);

	QLabel *default_example_title = new QLabel("3. 天线示例", this);
	setSecondTitle(default_example_title);

	QLabel *vswr_image_label = new QLabel(this);
	//vswr_image_label->setFixedSize(800, 600);
	QPixmap vswr_pixmap(QString("./images/%1/vswr.jpg").arg(_atn_problem->name));
	vswr_image_label->setPixmap(vswr_pixmap);
	//vswr_image_label->setStyleSheet("background:white;");
	vswr_image_label->setAlignment(Qt::AlignCenter);
	QLabel *vswr_info = new QLabel("天线驻波比", this);
	vswr_info->setAlignment(Qt::AlignHCenter);

	QLabel *gain_image_label = new QLabel(this);
	QPixmap gain_pixmap(QString("./images/%1/gain.jpg").arg(_atn_problem->name));
	//gain_pixmap = gain_pixmap.scaled(800, 600, Qt::KeepAspectRatio);
	gain_image_label->setPixmap(gain_pixmap);
	//gain_image_label->setStyleSheet("background:white;");
	gain_image_label->setAlignment(Qt::AlignCenter);
	QLabel *gain_info = new QLabel("theta=0时天线增益", this);
	gain_info->setAlignment(Qt::AlignHCenter);

	QWidget *w = new QWidget();
	QScrollArea *scroll_area = new QScrollArea();
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(title_label);
	layout->addWidget(summary_title);
	layout->addSpacing(20);
	layout->addWidget(atn_summary);
	layout->addSpacing(20);

	layout->addWidget(parameters_range_title);
	layout->addSpacing(20);
	QHBoxLayout *hlayout = new QHBoxLayout();
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
	hlayout->addWidget(_vars_range_table);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	layout->addLayout(hlayout);
	layout->addSpacing(20);

	layout->addWidget(default_example_title);
	layout->addSpacing(20);
	layout->addWidget(_vars_table);
	layout->addSpacing(20);

	layout->addWidget(vswr_image_label);
	layout->addWidget(vswr_info);
	layout->addSpacing(20);
	layout->addWidget(gain_image_label);
	layout->addWidget(gain_info);
	layout->setContentsMargins(20, 2, 20, 2);
	
	w->setLayout(layout);
	scroll_area->setWidget(w);
	scroll_area->setWidgetResizable(true);

	_main_layout = new QVBoxLayout();
	_main_layout->addWidget(scroll_area);
	setLayout(_main_layout);
}

void modelInfo::initDefalultParametersRange() {
	QJsonObject variables_obj = parseJson::getSubJsonObj(_obj, "variables");
	if (variables_obj.isEmpty()) {
		return;
	}
	_vars_range_table->setColumnCount(3);
	QStringList header;
	header << "变量" << "最小值" << "最大值";
	_vars_range_table->setHorizontalHeaderLabels(header);
	_vars_range_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	_vars_range_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	_vars_range_table->horizontalHeader()->resizeSection(0, 120);        //setting first column width is 120
	//_vars_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_vars_range_table->verticalHeader()->setVisible(false);
	_vars_range_table->verticalHeader()->setDefaultSectionSize(40); //设置行高
	_vars_range_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit

	QJsonObject var_obj;
	QString var_key;
	QStringList var_value;
	int row_number = 0, value_list_length;

	_vars_range_table->setRowCount(variables_obj.count());
	for (QJsonObject::iterator iter = variables_obj.begin(); iter != variables_obj.end(); ++iter) {
		// iter format: "W1":{"note" : "介质板宽度W1(m)", "W1" : "0.025"}
		var_key = iter.key();
		var_obj = iter.value().toObject();   //like {"note": "上贴片坐标y1(m)", "y1": "[-0.0115,0]"}
											 //get note infomation
		QString key_note = var_obj.value("note").toString().trimmed();
		_vars_range_table->insert2table(row_number, varnote, key_note);

		var_value = dataPool::str2list(var_obj.value(var_key).toString().trimmed());
		value_list_length = var_value.length();

		_vars_range_table->insert2table(row_number, varmin, var_value[0]);
		if (value_list_length == 1) {
			_vars_range_table->insert2table(row_number, varmax, var_value[0]);
		}
		else if (value_list_length >= 2 && value_list_length <= 3) {
			_vars_range_table->insert2table(row_number, varmax, var_value[1]);
		}
		else {
			qCritical("get 'variables' json object field.");
			QMessageBox::critical(0, QString("警告"), QString("问题json文件格式不正确（'variables'变量未设上下限）。"));
			return;
		}
		row_number++;
	}
}

void modelInfo::initDefalultParameters() {
	QJsonObject vars_value_obj = parseJson::getSubJsonObj(_obj, "varsValue");
	if (vars_value_obj.isEmpty()) {
		return;
	}
	_vars_table->setFixedHeight(80);
	_vars_table->setRowCount(2);
	_vars_table->setColumnCount(vars_value_obj.count());
	QStringList header;
	header << "变量" << "参数值";
	_vars_table->setVerticalHeaderLabels(header);
	_vars_table->verticalHeader()->setFixedWidth(50);
	_vars_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_vars_table->horizontalHeader()->setVisible(false);
	_vars_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit

	
	int row_number = 0;
	for (QJsonObject::iterator iter = vars_value_obj.begin(); iter != vars_value_obj.end(); ++iter) {
		// iter format: "W1":{"W1" : "0.025"}
		_vars_table->insert2table(0, row_number, iter.key());
		_vars_table->insert2table(1, row_number, iter.value().toString());
		row_number++;
	}
}

void modelInfo::setFirstTitle(QLabel *label) {
	QFont ft;
	ft.setBold(true);
	ft.setPointSize(24);
	label->setFont(ft);
	label->setAlignment(Qt::AlignHCenter);
}
void modelInfo::setSecondTitle(QLabel *label) {
	QFont ft;
	ft.setBold(true);
	ft.setPointSize(18);
	label->setFont(ft);
}

modelInfo::~modelInfo() {
	delete _main_layout;
	_main_layout = nullptr;
}