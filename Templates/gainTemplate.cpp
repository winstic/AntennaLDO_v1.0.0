#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "gainTemplate.h"

gainTemplate::gainTemplate(parsProblem* atn_problem, QJsonObject* obj, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj) {
	_gain_table = new tableTemplate();
	_gain_table->setColumnCount(8);
	QStringList header;
	header << "起始θ" << "终止θ" << "起始φ" << "终止φ" << "优化方式" << "误差值" << "增益(dB)" << "权值";
	_gain_table->setHorizontalHeaderLabels(header);
	_gain_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	initDefaultData();
	initLayout();
}

void gainTemplate::initDefaultData() {
	QJsonObject gain_obj = parseJson::getSubJsonObj(*_obj, "GainSetting");
	if (gain_obj.isEmpty()) {
		qCritical("get 'GainSetting' json object field.");
		return;
	}
	QSignalMapper* gain_signals_map = new QSignalMapper;
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
		_gain_table->insert2table(i, cthetalower, str_list_theta_lower[i]);
		_gain_table->insert2table(i, cthetaupper, str_list_theta_upper[i]);
		_gain_table->insert2table(i, cphilower, str_list_phi_lower[i]);
		_gain_table->insert2table(i, cphiupper, str_list_phi_upper[i]);

		QComboBox* optimal_type = new QComboBox;
		initOptimalTypeComBox(optimal_type);
		optimal_type->setCurrentText(str_list_optimal_type[i]);
		_gain_table->setCellWidget(i, coptimaltype, optimal_type);
		//qDebug() << gainTable->item(i, coptimaltype)->text();
		//map combobox signal
		connect(optimal_type, SIGNAL(currentIndexChanged(int)), gain_signals_map, SLOT(map()));
		gain_signals_map->setMapping(optimal_type, QString("%1-%2").arg(i).arg(coptimaltype));

		_gain_table->insert2table(i, cdelta, str_list_delta[i]);
		//setting cannot edit when optimize type is not delta
		if (optimal_type->currentIndex() != 2)
			_gain_table->item(i, cdelta)->setFlags(Qt::NoItemFlags);

		_gain_table->insert2table(i, cobjvalue, str_list_gain[i]);
		_gain_table->insert2table(i, cweight, str_list_weight[i]);
	}
	connect(gain_signals_map, SIGNAL(mapped(QString)), this, SLOT(slot_ChangeOptimaltype(QString)));
}

void gainTemplate::initLayout() {
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(_gain_table);
	_layout = layout;
}

QLayout* gainTemplate::getLayout() {
	return _layout;
}

//update json obj
void gainTemplate::updateJObj() {
	QJsonObject mgain_obj;
	int i;
	//update gain obj
	QStringList gain_str[8];
	for (i = 0; i < _gain_table->rowCount(); i++) {
		gain_str[0] << _gain_table->item(i, cthetalower)->text().trimmed();
		gain_str[1] << _gain_table->item(i, cthetaupper)->text().trimmed();
		gain_str[2] << _gain_table->item(i, cphilower)->text().trimmed();
		gain_str[3] << _gain_table->item(i, cphiupper)->text().trimmed();
		QComboBox *goType = static_cast<QComboBox *>(_gain_table->cellWidget(i, coptimaltype));
		if (3 == goType->currentIndex())
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
	_obj->insert("GainSetting", mgain_obj);
}

void gainTemplate::slot_ChangeOptimaltype(QString pos) {
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


gainTemplate::~gainTemplate() {}