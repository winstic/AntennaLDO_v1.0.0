#include "../Utility/macrodefined.h"
#include "wizardOptimizeVariables.h"

wizardOptimizeVariables::wizardOptimizeVariables(QJsonObject& vars_range_obj, parsProblem* atn_problem, QWidget *parent)
	: QWizardPage(parent), _vars_range_obj(vars_range_obj), _atn_problem(atn_problem), _is_all_valid(false) {

    _vars_table = new QTableWidget(this);
	tableStyleSetting();
	_is_all_valid = wizardDialogWithTable();
	//when the property "varJsonObj" is changed,emit the signal, 
	//then you can get vars_obj through field("var_json_obj").toJsonObject in other page
	registerField("variables_json_obj", this, "varJsonObj", SIGNAL(signal_varsJsonObjChanged()));
}

void wizardOptimizeVariables::tableStyleSetting() {
	setTitle(tr("优化变量设置"));
	setSubTitle(tr("设置需要优化的变量参数"));
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
}

bool wizardOptimizeVariables::wizardDialogWithTable(){
    QJsonObject var_obj;
    QString var_key;
    QStringList var_value;
    int row_number = 0, value_list_length;
	QSignalMapper signals_map_unit;	//use signalmaper manage signals in table

	_vars_table->setRowCount(_vars_range_obj.count());

    for(QJsonObject::iterator iter = _vars_range_obj.begin(); iter != _vars_range_obj.end(); ++ iter){
        // iter format: "W1":{"note" : "介质板宽度W1(m)", "W1" : "0.025"}
		var_key = iter.key();
		var_obj = iter.value().toObject();   //like {"note": "上贴片坐标y1(m)", "y1": "[-0.0115,0]"}
        //get note infomation
        QString key_note = var_obj.value("note").toString().trimmed();
        insert2table(row_number, varnote, key_note);
		_vars_table->item(row_number, varnote)->setWhatsThis(var_key);

		var_value = dataPool::str2list(var_obj.value(var_key).toString().trimmed());
		value_list_length = var_value.length();
        insert2table(row_number, varmin, var_value[0]);
        if(value_list_length == 1)
            insert2table(row_number, varmax, var_value[0]);
        else
            insert2table(row_number, varmax, var_value[1]);

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

	setLayout(&h_layout);
    return true;
}

void wizardOptimizeVariables::initUnitComBo(QComboBox *comb){
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

void wizardOptimizeVariables::initializePage() {
	updateJsonObj();
	emit signal_varsJsonObjChanged();
}

void wizardOptimizeVariables::updateJsonObj() {
	QString var_key, var_value, var_note;
	for (int i = 0; i < _vars_table->rowCount(); i++) {
		var_key = _vars_table->item(i, varnote)->whatsThis().trimmed();
		var_note = _vars_table->item(i, varnote)->text().trimmed();
		var_value = QString("[%1,%2]").arg(_vars_table->item(i, varmin)->text().trimmed())
			.arg(_vars_table->item(i, varmax)->text().trimmed());
		QJsonObject itemobj;
		itemobj.insert("note", var_note);
		itemobj.insert(var_key, var_value);
		_variables_obj.insert(var_key, itemobj);
	}
}

void wizardOptimizeVariables::insert2table(const int &row, const int &clomun, const QString &itemValue){
    QTableWidgetItem *table_item = new QTableWidgetItem(itemValue);
	table_item->setTextAlignment(Qt::AlignCenter);
	_vars_table->setItem(row, clomun, table_item);
}

double wizardOptimizeVariables::unitConversion(double source_data, int pre_unit, int curr_unit){
    //assert(preunit != unitlambda && curunit != unitlambda);
    return source_data * qPow(10, pre_unit - curr_unit);
}

//slots function
void wizardOptimizeVariables::slot_unitchange(QString pos){
    Q_ASSERT(!_vars_unit.isEmpty());
    QStringList coordinates = pos.split("-");
    int row = coordinates.at(0).toInt();
    int col = coordinates.at(1).toInt();
    int current_unit_data = _vars_unit[row];
    QComboBox *select_combox = (QComboBox *)_vars_table->cellWidget(row, col);
    int new_unit_data = select_combox->currentData(ROLE_MARK_UNIT).toInt();
    //qDebug() << varTable->item(row, varnote)->whatsThis();
    if(current_unit_data != MARK_UNIT_LAMBDA && new_unit_data != MARK_UNIT_LAMBDA &&
		new_unit_data != current_unit_data){
        double pre_value_min = _vars_table->item(row, varmin)->text().trimmed().toDouble();
        double pre_value_max = _vars_table->item(row, varmax)->text().trimmed().toDouble();
        double current_value_min = unitConversion(pre_value_min, current_unit_data, new_unit_data);
        double current_value_max = unitConversion(pre_value_max, current_unit_data, new_unit_data);
        insert2table(row, varmin, QString::number(current_value_min));
        insert2table(row, varmax, QString::number(current_value_max));
    }
	_vars_unit[row] = new_unit_data;
}

bool wizardOptimizeVariables::isComplete() const {
	return _is_all_valid;
}

wizardOptimizeVariables::~wizardOptimizeVariables() {}