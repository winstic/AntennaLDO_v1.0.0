#include "../Utility/global.h"
#include "wizardOptimizeGainAxialLoss.h"

wizardOptimizeAXL::wizardOptimizeAXL(QJsonObject& gain_obj, QJsonObject& axial_obj, QJsonObject& loss_obj, QWidget *parent)
	: QWizardPage(parent), _gain_obj(gain_obj), _axial_obj(axial_obj), _loss_obj(loss_obj){
    setTitle(tr("增益轴比回波损失设置"));
    setSubTitle(tr("设置增益轴比回波损失等参数"));

    _gain_table = new QTableWidget(this);
    _axial_table = new QTableWidget(this);
    _loss_table = new QTableWidget(this);

    gainSetting();
    axialSetting();
    lossSetting();
    initLayout();

	registerField("gal_json_obj", this, "galJsonObj", SIGNAL(signal_galJsonObjChanged()));
}

bool wizardOptimizeAXL::gainSetting(){
    _gain_table->setColumnCount(8);
    QStringList header;
    header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "增益(dB)" << "权值";
	_gain_table->setHorizontalHeaderLabels(header);
    //gainTable->setStyleSheet("QTableView {selection-background-color: grey;}");
	_gain_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	QSignalMapper gain_signals_map;
    QStringList str_list_theta_lower = dataPool::str2list(_gain_obj.value("Theta_Lower_gain").toString());
    QStringList str_list_theta_upper = dataPool::str2list(_gain_obj.value("Theta_Upper_gain").toString());
    QStringList str_list_phi_lower = dataPool::str2list(_gain_obj.value("Phi_Lower_gain").toString());
    QStringList str_list_phi_upper = dataPool::str2list(_gain_obj.value("Phi_Upper_gain").toString());
    QStringList str_list_optimal_type = dataPool::str2list(_gain_obj.value("optimaltype_gain").toString());
    QStringList str_list_delta = dataPool::str2list(_gain_obj.value("delta_gain").toString());
    QStringList str_list_gain = dataPool::str2list(_gain_obj.value("gainobj").toString());
    QStringList str_list_weight = dataPool::str2list(_gain_obj.value("weight_gain").toString());
    _gain_table->setRowCount(str_list_theta_lower.length());
    for(int i = 0; i < str_list_theta_lower.length(); i++){
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
        if(optimal_type.currentIndex() != 2)
			_gain_table->item(i, cdelta)->setFlags(Qt::NoItemFlags);

        insert2table(_gain_table, i, cobjvalue, str_list_gain[i]);
        insert2table(_gain_table, i, cweight, str_list_weight[i]);
    }
    connect(&gain_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_gainChangeOptimaltype(QString)));
    return true;
}

bool wizardOptimizeAXL::axialSetting(){
    _axial_table->setColumnCount(8);
    QStringList header;
    header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "轴比(dB)" << "权值";
	_axial_table->setHorizontalHeaderLabels(header);
	_axial_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	QSignalMapper axial_signals_map;
    QStringList strListThetaLower = dataPool::str2list(_axial_obj.value("Theta_Lower_axial").toString());
    QStringList strListThetaUpper = dataPool::str2list(_axial_obj.value("Theta_Upper_axial").toString());
    QStringList strListPhiLower = dataPool::str2list(_axial_obj.value("Phi_Lower_axial").toString());
    QStringList strListPhiUpper = dataPool::str2list(_axial_obj.value("Phi_Upper_axial").toString());
    QStringList strListOptimaltype = dataPool::str2list(_axial_obj.value("optimaltype_axial").toString());
    QStringList strListDelta = dataPool::str2list(_axial_obj.value("delta_axial").toString());
    QStringList strListGainobj = dataPool::str2list(_axial_obj.value("axialobj").toString());
    QStringList strListWeight = dataPool::str2list(_axial_obj.value("weight_axial").toString());
	_axial_table->setRowCount(strListThetaLower.length());
    for(int i = 0; i < strListThetaLower.length(); i++){
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
        if(optimal_type.currentIndex() != 2)
			_axial_table->item(i, cdelta)->setFlags(Qt::NoItemFlags);

        insert2table(_axial_table, i, cobjvalue, strListGainobj[i]);
        insert2table(_axial_table, i, cweight, strListWeight[i]);
    }
    connect(&axial_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_axialChangeOptimaltype(QString)));
    return true;
}

bool wizardOptimizeAXL::lossSetting(){
    _loss_table->setColumnCount(9);
    QStringList header;
    header << "Z0实部" << "Z0虚部" << "损失方式" << "优化方式" << "误差实部" << "误差虚部" << "值实部" << "值虚部" << "权值";
	_loss_table->setHorizontalHeaderLabels(header);
	_loss_table->resizeColumnToContents(6);
	_loss_table->resizeColumnToContents(7);
	_loss_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	QSignalMapper loss_signals_map;
    QStringList strListR0Real = dataPool::str2list(_loss_obj.value("R0_real").toString());
    QStringList strListR0imag = dataPool::str2list(_loss_obj.value("R0_imag").toString());
    QStringList strListReturnLossType = dataPool::str2list(_loss_obj.value("ReturnLossType").toString());
    QStringList strListOptimaltype = dataPool::str2list(_loss_obj.value("optimaltype_vswr").toString());
    QStringList strListDeltaReal = dataPool::str2list(_loss_obj.value("delta_real").toString());
    QStringList strListDeltaImag = dataPool::str2list(_loss_obj.value("delta_imag").toString());
    QStringList strListVswrobj = dataPool::str2list(_loss_obj.value("vswrobj").toString());
    QStringList strListS11 = dataPool::str2list(_loss_obj.value("S11").toString());
    QStringList strListR1Real = dataPool::str2list(_loss_obj.value("R1_real").toString());
    QStringList strListR1Imag = dataPool::str2list(_loss_obj.value("R1_imag").toString());
    QStringList strListWeight = dataPool::str2list(_loss_obj.value("weight_vswr").toString());
	_loss_table->setRowCount(strListR0Real.length());
    for(int i = 0; i < strListR0Real.length(); i++){
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
        if(2 != optimal_type.currentIndex()){
			_loss_table->item(i, cdeltareal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(i, cdeltaimag)->setFlags(Qt::NoItemFlags);
        }
        if(0 == loss_type.currentIndex()){
            //loss type is vswr
            insert2table(_loss_table, i, cobjreal, strListVswrobj[i]);
			_loss_table->setItem(i, cobjimag, new QTableWidgetItem("None"));
			_loss_table->item(i, cobjimag)->setFlags(Qt::NoItemFlags);
        }
        else if(1 == loss_type.currentIndex()){
            //loss type is S11
            insert2table(_loss_table, i, cobjreal, strListS11[i]);
			_loss_table->setItem(i, cobjimag, new QTableWidgetItem("None"));
			_loss_table->item(i, cobjimag)->setFlags(Qt::NoItemFlags);
        }
        else if(2 == loss_type.currentIndex()){
            //loss type is R
            insert2table(_loss_table, i, cobjreal, strListR1Real[i]);
            insert2table(_loss_table, i, cobjimag, strListR1Imag[i]);
        }
        else{
            //loss type is None
			_loss_table->setItem(i, cobjreal, new QTableWidgetItem("None"));
			_loss_table->setItem(i, cobjimag, new QTableWidgetItem("None"));
			_loss_table->item(i, cobjreal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(i, cobjimag)->setFlags(Qt::NoItemFlags);
        }
        insert2table(_loss_table, i, clossweight, strListWeight[i]);
    }
    connect(&loss_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_lossChangeType(QString)));
    return true;
}

void wizardOptimizeAXL::initOptimalTypeComBox(QComboBox *combox){
    combox->addItem("max");
    combox->addItem("min");
    combox->addItem("delta");
    combox->addItem("None");
    combox->setCurrentIndex(3);    
}

void wizardOptimizeAXL::initLossTypeComBox(QComboBox *combox){
    combox->addItem(tr("驻波比"));
    combox->addItem(tr("S11(dB)"));
    combox->addItem(tr("阻抗"));
    combox->addItem(tr("None"));
    combox->setCurrentIndex(3);
}

void wizardOptimizeAXL::initLayout(){
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
    setLayout(&v_layout);
}

void wizardOptimizeAXL::insert2table(QTableWidget *table, const int &row, const int &clomun, const QString &item_value){
    QTableWidgetItem *table_item = new QTableWidgetItem(item_value);
	table_item->setTextAlignment(Qt::AlignCenter);
    table->setItem(row, clomun, table_item);
}

bool wizardOptimizeAXL::isComplete() const{
    return true;
}

void wizardOptimizeAXL::initializePage() {
	updateJsonObj();
	emit signal_galJsonObjChanged();
}

void wizardOptimizeAXL::updateJsonObj(){
    QJsonObject mgain_obj, maxial_obj, mloss_obj;
    int i;
    //save gain setting
    QStringList gain_str[8];
    for(i = 0; i < _gain_table->rowCount(); i++){
		gain_str[0] << _gain_table->item(i, cthetalower)->text().trimmed();
		gain_str[1] << _gain_table->item(i, cthetaupper)->text().trimmed();
		gain_str[2] << _gain_table->item(i, cphilower)->text().trimmed();
		gain_str[3] << _gain_table->item(i, cphiupper)->text().trimmed();
        QComboBox *goType = static_cast<QComboBox *>(_gain_table->cellWidget(i, coptimaltype));
        if(3 == goType->currentIndex())
			gain_str[4] << goType->currentText().trimmed();
        else
			gain_str[4] << QString("'%1'").arg(goType->currentText().trimmed());
		gain_str[5] << _gain_table->item(i, cdelta)->text().trimmed();
		gain_str[6] << _gain_table->item(i, cobjvalue)->text().trimmed();
		gain_str[7] << _gain_table->item(i, cweight)->text().trimmed();
    }
	mgain_obj.insert("Theta_Lower_gain", QString("[[%1]]").arg(gain_str[0].join(",")));
	mgain_obj.insert("Theta_Upper_gain", QString("[[%1]]").arg(gain_str[1].join(",")));
	mgain_obj.insert("Phi_Lower_gain", QString("[[%1]]").arg(gain_str[2].join(",")));
	mgain_obj.insert("Phi_Upper_gain", QString("[[%1]]").arg(gain_str[3].join(",")));
	mgain_obj.insert("optimaltype_gain", QString("[[%1]]").arg(gain_str[4].join(",")));
	mgain_obj.insert("delta_gain", QString("[[%1]]").arg(gain_str[5].join(",")));
	mgain_obj.insert("gainobj", QString("[[%1]]").arg(gain_str[6].join(",")));
	mgain_obj.insert("weight_gain", QString("[[%1]]").arg(gain_str[7].join(",")));

    //save axial setting
    QStringList axialStr[8];
    for(i = 0; i < _axial_table->rowCount(); i++){
        axialStr[0] << _axial_table->item(i, cthetalower)->text().trimmed();
        axialStr[1] << _axial_table->item(i, cthetaupper)->text().trimmed();
        axialStr[2] << _axial_table->item(i, cphilower)->text().trimmed();
        axialStr[3] << _axial_table->item(i, cphiupper)->text().trimmed();
        QComboBox *aoType = static_cast<QComboBox *>(_axial_table->cellWidget(i, coptimaltype));
        if(3 == aoType->currentIndex())
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

    //save loss setting
    QStringList lossStr[11];
    for(i = 0; i < _loss_table->rowCount(); i++){
        lossStr[0] << _loss_table->item(i, cz0real)->text().trimmed();
        lossStr[1] << _loss_table->item(i, cz0imag)->text().trimmed();
        QComboBox *lossType = static_cast<QComboBox *>(_loss_table->cellWidget(i, closstype));
        lossStr[2] << QString::number(lossType->currentIndex());
        QComboBox *loType = static_cast<QComboBox *>(_loss_table->cellWidget(i, clossoptimaltype));
        if(3 == loType->currentIndex())
            lossStr[3] << loType->currentText().trimmed();
        else
            lossStr[3] << QString("'%1'").arg(loType->currentText().trimmed());
        lossStr[4] << _loss_table->item(i, cdeltareal)->text().trimmed();
        lossStr[5] << _loss_table->item(i, cdeltaimag)->text().trimmed();
        if(0 == lossType->currentIndex()){
            lossStr[6] << _loss_table->item(i, cobjreal)->text().trimmed();
            lossStr[7] << "None";
            lossStr[8] << "None";
        }
        else if(1 == lossType->currentIndex()){
            lossStr[6] << "None";
            lossStr[7] << _loss_table->item(i, cobjreal)->text().trimmed();
            lossStr[8] << "None";
        }
        else if(2 == lossType->currentIndex()){
            lossStr[6] << "None";
            lossStr[7] << "None";
            lossStr[8] << _loss_table->item(i, cobjreal)->text().trimmed();
        }
        else{
            lossStr[6] << "None";
            lossStr[7] << "None";
            lossStr[8] << "None";
        }
        lossStr[9] << _loss_table->item(i, cobjimag)->text().trimmed();
        lossStr[10] << _loss_table->item(i, clossweight)->text().trimmed();
    }
    qDebug() << lossStr[2];
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

	_gal_obj.insert("GainSetting", mgain_obj);
	_gal_obj.insert("AxialratioSetting", maxial_obj);
	_gal_obj.insert("VSWRSetting", mloss_obj);
}

//slots function
void wizardOptimizeAXL::slot_gainChangeOptimaltype(QString pos){
    QStringList coordinates = pos.split("-");
    int row = coordinates[0].toInt();
    int col = coordinates[1].toInt();
    QComboBox *select_combox = (QComboBox *)_gain_table->cellWidget(row, col);    
    //setting edit when optimize type is delta
    if(select_combox->currentIndex() == 2)
		_gain_table->item(row, cdelta)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    else
		_gain_table->item(row, cdelta)->setFlags(Qt::NoItemFlags);
}

void wizardOptimizeAXL::slot_axialChangeOptimaltype(QString pos){
    QStringList coordinates = pos.split("-");
    int row = coordinates[0].toInt();
    int col = coordinates[1].toInt();
    QComboBox *select_combox = (QComboBox *)_axial_table->cellWidget(row, col);
    //setting edit when optimize type is delta
    if(select_combox->currentIndex() == 2){
		_axial_table->item(row, cdelta)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    }
    else
		_axial_table->item(row, cdelta)->setFlags(Qt::NoItemFlags);
}

void wizardOptimizeAXL::slot_lossChangeType(QString pos){
    QStringList coordinates = pos.split("-");
    int row = coordinates[0].toInt();
    int col = coordinates[1].toInt();
    QComboBox* select_combox = (QComboBox *)_loss_table->cellWidget(row, col);
    //setting edit when optimize type is delta
    if(col == clossoptimaltype){
        if(2 == select_combox->currentIndex()){
			_loss_table->item(row, cdeltareal)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			_loss_table->item(row, cdeltaimag)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        }
        else{
			_loss_table->item(row, cdeltareal)->setFlags(Qt::NoItemFlags);
			_loss_table->item(row, cdeltaimag)->setFlags(Qt::NoItemFlags);
        }
    }
    if(col == closstype){
        if(2 == select_combox->currentIndex())
			_loss_table->item(row, cobjimag)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        else
			_loss_table->item(row, cobjimag)->setFlags(Qt::NoItemFlags);
    }
}