#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "../Utility/commonStyle.h"
#include "frequencyTemplate.h"

frequencyTemplate::frequencyTemplate(parsProblem* atn_problem, QJsonObject* obj, unsigned int index, iTemplate *parent) : iTemplate(parent),
_atn_problem(atn_problem), _obj(obj), _index(index) {
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

	initSweepNDPMComBox();
	initRegex();
	initDefaultData(); 
	initLayout();

	//传递signal_checkValid用于在引导页面刷新isComplete校验
	connect(_frequency_low_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_textChanged(QString)));
	connect(_frequency_up_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_textChanged(QString)));
	connect(_frequency_num_edit, SIGNAL(textChanged(QString)), this, SLOT(slot_textChanged(QString)));
}

void frequencyTemplate::initSweepNDPMComBox() {
	_sweep_type_combox->addItem("等差");
	_sweep_type_combox->addItem("等比");

	_polarization_combox->addItem("总增益");
	_polarization_combox->addItem("左旋");
	_polarization_combox->addItem("右旋");
	_polarization_combox->addItem("水平");
	_polarization_combox->addItem("垂直");
}

void frequencyTemplate::initRegex() {
	//setting some input rules
	QRegExpValidator *nonNegFloatValid = getNonNegativeFloatReg();    //non negative float
	QRegExpValidator *posIntValid = getPositiveIntReg();		//positive int
	_frequency_low_edit->setValidator(nonNegFloatValid);
	_frequency_up_edit->setValidator(nonNegFloatValid);
	_frequency_num_edit->setValidator(posIntValid);
}

void frequencyTemplate::initDefaultData() {
	//setting default data
	QJsonObject frequency_obj = parseJson::getSubJsonObj(*_obj, "FreSetting");
	if (frequency_obj.isEmpty()) {
		return;
	}
	QString fre_start = "", fre_end = "", fre_number = "";
	int pm_int = 0, sweep_type_int = 0;
	_fre_start_list = dataPool::str2list(frequency_obj.value("FreStart").toString().simplified());
	_fre_end_list = dataPool::str2list(frequency_obj.value("FreEnd").toString().simplified());
	_fre_number_list = dataPool::str2list(frequency_obj.value("FreNumber").toString().simplified());
	_pm_list = dataPool::str2list(frequency_obj.value("PM").toString().simplified());
	_sweep_type_list = dataPool::str2list(frequency_obj.value("SweepType").toString().simplified());
	unsigned int length = _fre_start_list.size();
	if (_fre_end_list.size() != length || _fre_number_list.size() != length || _pm_list.size() != length
		|| _sweep_type_list.size() != length || _index >= length) {
		qCritical("问题json文件频率数据设置有误, 请仔细核对。");
		return;
	}

	fre_start = _fre_start_list[_index];
	fre_end = _fre_end_list[_index];
	fre_number = _fre_number_list[_index];
	pm_int = _pm_list[_index].toInt();
	sweep_type_int = _sweep_type_list[_index].toInt();

	_frequency_low_edit->setText(fre_start);
	_frequency_up_edit->setText(fre_end);
	_frequency_num_edit->setText(fre_number);
	_polarization_combox->setCurrentIndex(pm_int);
	_sweep_type_combox->setCurrentIndex(sweep_type_int);
}

void frequencyTemplate::initLayout() {
	//layout
	QGridLayout* frequency_layout = new QGridLayout;
	_frequency_low_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_frequency_low_label, 0, 0);
	frequency_layout->addWidget(_frequency_low_edit, 0, 1);
	QLabel* hz_label = new QLabel("MHz");
	frequency_layout->addWidget(hz_label, 0, 2);
	_frequency_up_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_frequency_up_label, 1, 0);
	frequency_layout->addWidget(_frequency_up_edit, 1, 1);
	QLabel* hz_label2 = new QLabel("MHz");
	frequency_layout->addWidget(hz_label2, 1, 2);
	_frequency_num_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_frequency_num_label, 2, 0);
	frequency_layout->addWidget(_frequency_num_edit, 2, 1);
	_sweep_type_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_sweep_type_label, 3, 0);
	frequency_layout->addWidget(_sweep_type_combox, 3, 1);
	_polarization_label->setAlignment(Qt::AlignRight);
	frequency_layout->addWidget(_polarization_label, 4, 0);
	frequency_layout->addWidget(_polarization_combox, 4, 1);
	_layout = frequency_layout;
}

QLayout* frequencyTemplate::getLayout() {
	return _layout;
}

//check input
bool frequencyTemplate::checkInputValid() {
	unsigned int length = _fre_start_list.size();
	if (_fre_end_list.size() != length || _fre_number_list.size() != length || _pm_list.size() != length
		|| _sweep_type_list.size() != length || _index >= length) {
		checkInfo->code = eInvalid;
		checkInfo->message = "问题json文件频率数据设置有误, 请仔细核对。";
		qCritical("问题json文件频率数据设置有误, 请仔细核对。");
		return false;
	}

	QString frequency_low = _frequency_low_edit->text().trimmed();
	QString frequency_up = _frequency_up_edit->text().trimmed();
	QString frequency_num = _frequency_num_edit->text().trimmed();

	QList<QLineEdit *> fres{ _frequency_low_edit, _frequency_up_edit, _frequency_num_edit };
	for (QLineEdit* item : fres) {
		if (item->text().trimmed().isEmpty() || item->text().trimmed().isNull()) {
			checkInfo->code = eNull;
			checkInfo->message = "设置参数不能为空。";
			commonStyle::setLineEditWarningStyle(item);
			return false;
		}
	}
	double frequency_low_d = frequency_low.toDouble();
	double frequency_up_d = frequency_up.toDouble();
	if (frequency_low_d > frequency_up_d) {
		checkInfo->code = eInvalid;
		checkInfo->message = "请设置正确的频率范围。";
		commonStyle::setLineEditWarningStyle(_frequency_low_edit);
		commonStyle::setLineEditWarningStyle(_frequency_up_edit);
		return false;
	}

	_fre_start_list[_index] = _frequency_low_edit->text().trimmed();
	_fre_end_list[_index] = _frequency_up_edit->text().trimmed();
	_fre_number_list[_index] = _frequency_num_edit->text().trimmed();
	_pm_list[_index] = QString::number(_polarization_combox->currentIndex());
	_sweep_type_list[_index] = QString::number(_sweep_type_combox->currentIndex());

	//更新设置的最大频率
	QVector<freRange> tfr;
	_atn_problem->frequencyRanges.swap(tfr);
	for (int i = 0; i < length; ++i) {
		freRange tmpfr(_fre_start_list[i].toDouble(), _fre_end_list[i].toDouble());
		_atn_problem->frequencyRanges.append(tmpfr);
	}
	if (!checkMutiFrequencyCross(_atn_problem->frequencyRanges)) {
		checkInfo->code = eInvalid;
		checkInfo->message = "当前频段与已设置频段有交叉。";
		commonStyle::setLineEditWarningStyle(_frequency_low_edit);
		commonStyle::setLineEditWarningStyle(_frequency_up_edit);
		_atn_problem->frequencyRanges.swap(tfr);
		return false;
	}
	_atn_problem->fillMaxFrequency();
	commonStyle::clearLineEditWarningStyle(_frequency_low_edit);
	commonStyle::clearLineEditWarningStyle(_frequency_up_edit);
	commonStyle::clearLineEditWarningStyle(_frequency_num_edit);
	return true;
}

bool frequencyTemplate::checkMutiFrequencyCross(QVector<freRange> mutiFre) {
	auto comp = [](freRange a, freRange b) {
		if (qAbs(a.first - b.first) <= 0.000001) 
			return (a.second < b.second);
		else if (a.first < b.first) 
			return true;
		else 
			return false;
	};
	qSort(mutiFre.begin(), mutiFre.end(), comp);
	for (size_t i = 0; i < mutiFre.size() - 1; ++i) {
		if ((mutiFre[i + 1].first - mutiFre[i].second) < 0.000001)
			return false;
	}
	return true;
}

//update json obj
void frequencyTemplate::updateJObj() {
	QJsonObject mfrequency_obj;
	mfrequency_obj.insert("FreStart", QString("[%1]").arg(_fre_start_list.join(",")));
	mfrequency_obj.insert("FreEnd", QString("[%1]").arg(_fre_end_list.join(",")));
	mfrequency_obj.insert("FreNumber", QString("[%1]").arg(_fre_number_list.join(",")));
	mfrequency_obj.insert("PM", QString("[%1]").arg(_pm_list.join(",")));
	mfrequency_obj.insert("SweepType", QString("[%1]").arg(_sweep_type_list.join(",")));
	_obj->insert("FreSetting", mfrequency_obj);
}

//slots
void frequencyTemplate::slot_textChanged(QString) {
	commonStyle::clearLineEditWarningStyle(_frequency_low_edit);
	commonStyle::clearLineEditWarningStyle(_frequency_up_edit);
	commonStyle::clearLineEditWarningStyle(_frequency_num_edit);
	emit signal_checkValid();
}

frequencyTemplate::~frequencyTemplate(){
	delete _layout;
	_layout = nullptr;
}