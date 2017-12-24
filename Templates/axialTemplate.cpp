#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "axialTemplate.h"

axialTemplate::axialTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj) {
	_axial_table = new tableTemplate();
	_axial_table->setColumnCount(8);
	QStringList header;
	header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "轴比(dB)" << "权值";
	_axial_table->setHorizontalHeaderLabels(header);
	_axial_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	initDefaultData();
	initLayout();
}

void axialTemplate::initDefaultData() {
	QJsonObject axial_obj = parseJson::getSubJsonObj(*_obj, "AxialratioSetting");
	if (axial_obj.isEmpty()) {
		qCritical("get 'AxialratioSetting' json object field.");
		return;
	}
	QSignalMapper* axial_signals_map = new QSignalMapper;
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
		_axial_table->insert2table(i, cthetalower, strListThetaLower[i]);
		_axial_table->insert2table(i, cthetaupper, strListThetaUpper[i]);
		_axial_table->insert2table(i, cphilower, strListPhiLower[i]);
		_axial_table->insert2table(i, cphiupper, strListPhiUpper[i]);

		QComboBox* optimal_type = new QComboBox;
		initOptimalTypeComBox(optimal_type);
		optimal_type->setCurrentText(strListOptimaltype[i]);
		_axial_table->setCellWidget(i, coptimaltype, optimal_type);
		//map combobox signal
		connect(optimal_type, SIGNAL(currentIndexChanged(int)), axial_signals_map, SLOT(map()));
		axial_signals_map->setMapping(optimal_type, QString("%1-%2").arg(i).arg(coptimaltype));

		_axial_table->insert2table(i, cdelta, strListDelta[i]);
		//setting cannot edit when optimize type is delta
		if (optimal_type->currentIndex() != 2)
			_axial_table->item(i, cdelta)->setFlags(Qt::NoItemFlags);

		_axial_table->insert2table(i, cobjvalue, strListGainobj[i]);
		_axial_table->insert2table(i, cweight, strListWeight[i]);
	}
	connect(axial_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_changeOptimaltype(QString)));
}

void axialTemplate::initLayout() {
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(_axial_table);
	_layout = layout;
}

QLayout* axialTemplate::getLayout() {
	return _layout;
}

//update json obj
void axialTemplate::updateJObj() {
	QJsonObject maxial_obj;
	//update axial obj
	QStringList axialStr[8];
	for (int i = 0; i < _axial_table->rowCount(); i++) {
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
	_obj->insert("AxialratioSetting", maxial_obj);
}

void axialTemplate::slot_changeOptimaltype(QString pos) {
	QStringList coordinates = pos.split("-");
	int row = coordinates[0].toInt();
	int col = coordinates[1].toInt();
	QComboBox *selectCombox = (QComboBox *)_axial_table->cellWidget(row, col);
	//setting edit when optimize type is delta
	if (selectCombox->currentIndex() == 2) {
		_axial_table->item(row, cdelta)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	}
	else
		_axial_table->item(row, cdelta)->setFlags(Qt::NoItemFlags);
}


axialTemplate::~axialTemplate() {}