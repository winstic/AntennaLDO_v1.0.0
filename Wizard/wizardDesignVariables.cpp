#include "../Utility/macrodefined.h"
#include "wizardDesignVariables.h"


wizardDesignVariables::wizardDesignVariables(QJsonObject& vars_value_obj, QJsonObject& vars_range_obj, parsProblem* atn_problem, QWidget *parent)
	: QWizardPage(parent), _vars_value_obj(vars_value_obj), _vars_range_obj(vars_range_obj), 
	_atn_problem(atn_problem), _is_all_valid(false){

    _vars_table = new QTableWidget(this);
	tableStyleSetting();
	_is_all_valid = initDefaultVars();
	_is_all_valid &= wizardDialogWithTable();
}

void wizardDesignVariables::tableStyleSetting() {
	setTitle(tr("模型设置"));
	setSubTitle(tr("模型设置"));
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
	_vars_table->horizontalHeader()->resizeSection(0, 120);        //setting first column width is 120
	_vars_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_vars_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_vars_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
	_vars_table->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //setting header background
	_vars_table->setEditTriggers(QAbstractItemView::NoEditTriggers);       //no edit
}

bool wizardDesignVariables::initDefaultVars(){
    for(QJsonObject::iterator iter = _vars_value_obj.begin(); iter != _vars_value_obj.end(); ++ iter){
        _default_vars[iter.key()] = iter.value().toString().trimmed();
    }
    return true;
}

bool wizardDesignVariables::wizardDialogWithTable(){	
    QJsonObject var_obj;
    QString var_key;
    QStringList var_value;
    int row_number = 0, value_list_length;
    double real_value;
	QSignalMapper signals_map_slider, signals_map_unit;	//use signalmaper manage signals in table
    QRegExp rx("^(-?\\d+)(\\.\\d+)?$");
	QRegExpValidator float_valid(rx);      //float

    _vars_table->setRowCount(_vars_range_obj.count());

    for(QJsonObject::iterator iter = _vars_range_obj.begin(); iter != _vars_range_obj.end(); ++ iter){
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
		registerField(QString("%1*").arg(var_key), value_edit);
		value_edit->setValidator(&float_valid);

        if(value_list_length == 1){
			//no range
			value_edit->setText(var_value[0]);
			value_edit->setReadOnly(true);
			_vars_table->setCellWidget(row_number, varvalue, value_edit);
        }
        else{
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
            if(up_value == low_value){
				var_slider->setValue(100);
				var_slider->setEnabled(false);
            }
            else{
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
    if(pm.width() > INTRODUCEIMAGEPIX)
		atn_image_label->setPixmap(pm.scaledToWidth(INTRODUCEIMAGEPIX));
    else
		atn_image_label->setPixmap(pm);
    //!
    QHBoxLayout h_layout;
	h_layout.addWidget(_vars_table, 1);
	h_layout.addWidget(atn_image_label, 1);

    setLayout(&h_layout);
    return true;
}

void wizardDesignVariables::initUnitComBo(QComboBox* comb){
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

QString wizardDesignVariables::getSliderSheet(){
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

void wizardDesignVariables::insert2table(const int &row, const int &clomun, const QString &item_value){
    QTableWidgetItem *table_item = new QTableWidgetItem(item_value);
	table_item->setTextAlignment(Qt::AlignCenter);
    _vars_table->setItem(row, clomun, table_item);
}

double wizardDesignVariables::unitConversion(double source_data, int pre_unit, int cur_unit){
    //assert(preunit != unitlambda && curunit != unitlambda);
    return source_data * qPow(10, pre_unit - cur_unit);
}

//slots function
void wizardDesignVariables::slot_LinetextChange(QString text){
    //qDebug() << text;
}

void wizardDesignVariables::slot_sliderValueChange(QString s_parameter){
    QStringList parameterList = s_parameter.split("#");
    int row_number = parameterList.at(0).toInt();        //table rowth
    double upper = parameterList.at(1).toDouble();
    double lower = parameterList.at(2).toDouble();
    //get widget
    QSlider* select_slider = _vars_table->cellWidget(row_number, varvalue)->findChild<QSlider *>();
    QLineEdit* select_line_edit = _vars_table->cellWidget(row_number, varvalue)->findChild<QLineEdit *>();
    int s_value = select_slider->value();
    //get new edit value by slider value
    double new_line_edit_value = (upper - lower)/100.0 * s_value + lower;
	select_line_edit->setText(QString::number(new_line_edit_value));
}

void wizardDesignVariables::slot_unitchange(QString pos){
    Q_ASSERT(!_vars_unit.isEmpty());
    int row = pos.toInt();
    int current_unit_data = _vars_unit[row];
    QComboBox* select_combox = _vars_table->cellWidget(row, varunit)->findChild<QComboBox *>();
    int new_unit_data = select_combox->currentData(ROLE_MARK_UNIT).toInt();
    //unit conversion
    if(current_unit_data != MARK_UNIT_LAMBDA && new_unit_data != MARK_UNIT_LAMBDA &&
		new_unit_data != current_unit_data){
        //get QLineEdit widget
        QLineEdit* curr_line_edit = _vars_table->cellWidget(row, varvalue)->findChild<QLineEdit *>();
        double pre_value = curr_line_edit->text().trimmed().toDouble();
        double curr_value = unitConversion(pre_value, current_unit_data, new_unit_data);
		curr_line_edit->setText(QString::number(curr_value));
    }
    //update unit item user data
	_vars_unit[row] = new_unit_data;
}

bool wizardDesignVariables::isComplete() const {
	return _is_all_valid;
}

wizardDesignVariables::~wizardDesignVariables(){}