#include "wizardOptimizeAlg.h"
#include "../Utility/parseJson.h"

wizardOptimizeAlg::wizardOptimizeAlg(parsProblem* atn_problem, QWidget *parent) : QWizardPage(parent),
_atn_problem(atn_problem), _algorithm(nullptr){
    setTitle(tr("优化算法设置"));
    setSubTitle(tr("设置算法参数"));

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
	_core_label = new QLabel(QString("cores"),this);
    _core_edit = new QLineEdit(this);
    
	_single_group = new QGroupBox(this);
	_multi_group = new QGroupBox(this);
    
	_add_button = new QPushButton(QString("添加"), this);
    _del_button = new QPushButton(QString("删除"), this);

	initAlgComboItem();
	_alg_combox->setCurrentIndex(0);
	_algorithm = dataPool::getAlgorithmByID(_alg_combox->currentData().toInt());

	initAlgVarsTableStyle();
	initNodesTableStyle();
	initDefaultThreadNum();
	setCurrAlgorithmParameters();
    initLayout();

    connect(_single_thread_checkbox, SIGNAL(stateChanged(int)), this, SLOT(slot_singleCheckBoxStateChange(int)));
    connect(_multi_thread_checkbox, SIGNAL(stateChanged(int)), this, SLOT(slot_multiCheckBoxStateChange(int)));
    connect(_alg_combox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_algName(int)));
    connect(_add_button, SIGNAL(clicked(bool)), this, SLOT(slot_addNodeButton()));
    connect(_del_button, SIGNAL(clicked(bool)), this, SLOT(slot_delNodeButton()));

	registerField("gAlg_json_obj", this, "gAlgJsonObj", SIGNAL(signal_gAlgJsonObjChanged()));
}

void wizardOptimizeAlg::initAlgVarsTableStyle() {
	_alg_vars_table->setColumnCount(2);
	_alg_vars_table->horizontalHeader()->setVisible(false);
	_alg_vars_table->horizontalHeader()->setSectionResizeMode(valueFlag, QHeaderView::Stretch);
	_alg_vars_table->verticalHeader()->setVisible(false);
	_alg_vars_table->setFrameShape(QFrame::NoFrame);                   //setting no frame
	_alg_vars_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_alg_vars_table->setSelectionMode(QAbstractItemView::SingleSelection);     //select signal row every time
	_alg_vars_table->setStyleSheet("selection-background-color:lightblue;");   //setting selected background
}

void wizardOptimizeAlg::initNodesTableStyle() {
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

void wizardOptimizeAlg::initDefaultThreadNum() {
	//get thread num from global_conf.
	QString global_json_path = QString("%1/global_conf.json").arg(dataPool::global::getGDEA4ADPath());
	QJsonObject global_obj = parseJson::getJsonObj(global_json_path);
	_thread_edit->setText(QString::number(global_obj.value("ThreadNum").toString().trimmed().toInt() - 1));
}

void wizardOptimizeAlg::setCurrAlgorithmParameters(){
    QString alg_json_path = QString("%1/%2_conf.json").arg(_algorithm->path).arg(_algorithm->name);
    QJsonObject alg_obj = parseJson::getJsonObj(alg_json_path);
    if(alg_obj.isEmpty()){
		qCritical(dataPool::str2char(QString("cannot parse algorithm json file [%1]").arg(alg_json_path)));
        QMessageBox::critical(0, QString("Error"), QString("error: Cannot parse algorithm json file"));
        return;
    }
    _alg_vars_table->setRowCount(alg_obj.count());
    QString var_key, var_value, value_note;
    QJsonObject var_obj; //format:{"generation": "1000", "note": "最大代数", "instruction": "最大迭代次数"}
    int row_number = 0;

    for(QJsonObject::iterator iter = alg_obj.begin(); iter != alg_obj.end(); ++ iter){
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

void wizardOptimizeAlg::initLayout(){    
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

    setLayout(&v_layout);
}

void wizardOptimizeAlg::initAlgComboItem(){
	int problem_id = _atn_problem->id;
	QMap<alg4pro, unsigned int>::iterator iter;		//like((algid, proid), associateId) 
	for (iter = dataPool::g_associates.begin(); iter != dataPool::g_associates.end(); ++iter) {
		if (iter.key().second == problem_id) {
			parsAlgorithm* algorithm = dataPool::getAlgorithmByID(iter.key().first);
			_alg_combox->addItem(algorithm->name, algorithm->id);	//so you can get algorithm_id through _alg_combox->Data()
		}
	}
}

void wizardOptimizeAlg::initializePage() {
	updateGAlgJsonObj();
	emit signal_gAlgJsonObjChanged();
}

void wizardOptimizeAlg::updateGAlgJsonObj(){
    QJsonObject uglobal_obj, ualg_obj, unode_thread_obj;
	for (int i = 0; i < _node_vars_table->rowCount(); ++i) {
		QString nodeOrIp, core, strUID;
		QJsonObject ncitemobj;
		strUID = QUuid::createUuid().toString();
		nodeOrIp = _node_vars_table->item(i, nodeFlag)->text().trimmed();
		core = _node_vars_table->item(i, coreFlag)->text().trimmed();
		ncitemobj.insert("node", nodeOrIp);
		ncitemobj.insert("threads", core);
		unode_thread_obj.insert(strUID, ncitemobj);
	}
    QString strNumber = _thread_edit->text().trimmed();
    int intNumber = 0;
    if(strNumber != NULL)
        intNumber = strNumber.toInt();
	uglobal_obj.insert("ThreadNum", QString::number(intNumber+1));
	uglobal_obj.insert("ALGORITHM_NAME", _algorithm->name);
	uglobal_obj.insert("PROBLEM_NAME", _atn_problem->name);
	uglobal_obj.insert("NodeAndThread", unode_thread_obj);
	_gAlg_obj.insert("global", uglobal_obj);

    QString varKey, varValue, varNote;
    for(int i = 0; i < _alg_vars_table->rowCount(); ++i){
        varKey = _alg_vars_table->item(i, keyFlag)->whatsThis().trimmed();
        varNote = _alg_vars_table->item(i, keyFlag)->text().trimmed();
        varValue = QString("%1").arg(_alg_vars_table->item(i, valueFlag)->text().trimmed());
        QJsonObject itemobj;
        itemobj.insert(varKey, varValue);
        itemobj.insert("note", varNote);
		ualg_obj.insert(varKey, itemobj);
    }
	_gAlg_obj.insert("algorithm", ualg_obj);
}

bool wizardOptimizeAlg::isComplete() const{
    return true;
}

void wizardOptimizeAlg::insert2table(QTableWidget *table, const int &row, const int &clomun, const QString &itemValue){
    QTableWidgetItem *tableItem = new QTableWidgetItem(itemValue);
    tableItem->setTextAlignment(Qt::AlignCenter);
    table->setItem(row, clomun, tableItem);
}

//signals function
//when we change the algorithm combox item, update algorithm instance immediately, and get some infomation at the same time. 
void wizardOptimizeAlg::slot_algName(const int index){
	_algorithm = dataPool::getAlgorithmByID(_alg_combox->itemData(index).toInt());
	setCurrAlgorithmParameters();
}

void wizardOptimizeAlg::slot_singleCheckBoxStateChange(const int state){
    //state: enum explain{Qt::Unchecked=0, Qt::PartiallyChecked, Qt::Checked}
    //so usually using 0(unchecked), 2(checked);
    //qDebug() << state;
    if(state == 0){
        _single_group->setEnabled(false);
        _multi_thread_checkbox->setCheckState(Qt::Checked);
    }
    else if(state == 2){
        //set another checkBox(multiCheckBox unChecked)
        _single_group->setEnabled(true);
		_multi_thread_checkbox->setCheckState(Qt::Unchecked);
    }
}

void wizardOptimizeAlg::slot_multiCheckBoxStateChange(const int state){
    if(state == 0){
        //set another checkBox(multiCheckBox Checked)
        _multi_group->setEnabled(false);
        _single_thread_checkbox->setCheckState(Qt::Checked);
    }
    else if(state == 2){
        //set another checkBox(multiCheckBox unChecked)
        _multi_group->setEnabled(true);
		_single_thread_checkbox->setCheckState(Qt::Unchecked);
    }
}

void wizardOptimizeAlg::slot_addNodeButton(){
    QString nodeinfo, coreinfo;
    int rowIndex = _node_vars_table->rowCount();
    //qDebug() << rowIndex;
	_node_vars_table->insertRow(rowIndex);
    nodeinfo = _node_edit->text().trimmed();
    coreinfo = _core_edit->text().trimmed();
    insert2table(_node_vars_table, rowIndex, nodeFlag, nodeinfo);
    insert2table(_node_vars_table, rowIndex, coreFlag, coreinfo);
}

void wizardOptimizeAlg::slot_delNodeButton(){
    int selectRow = _node_vars_table->currentRow();
    //qDebug() << selectRow;
    if(selectRow != -1)
		_node_vars_table->removeRow(selectRow);
}
