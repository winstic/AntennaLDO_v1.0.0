#include "../Utility/macrodefined.h"
#include "wizardOptimizeVariables.h"

wizardOptimizeVariables::wizardOptimizeVariables(parsProblem* atn_problem, QJsonObject& obj, QWidget *parent)
	: QWizardPage(parent), _atn_problem(atn_problem), _obj(obj), _temp_widget(atn_problem, obj) {

	setTitle(tr("优化变量设置"));
	setSubTitle(tr("设置需要优化的变量参数"));

	//layout
	QHBoxLayout layout;
	_temp_widget.initVariablesRangeWidget(&layout);
	setLayout(&layout);

	//when the property "varJsonObj" is changed,emit the signal, 
	//then you can get vars_obj through field("var_json_obj").toJsonObject in other page
	registerField("variables_json_obj", this, "varJsonObj", SIGNAL(signal_varsJsonObjChanged()));
}


void wizardOptimizeVariables::initializePage() {
	updateJsonObj();
	emit signal_varsJsonObjChanged();
}

void wizardOptimizeVariables::updateJsonObj() {
	QString var_key, var_value, var_note;
	for (int i = 0; i < _temp_widget._vars_range_table->rowCount(); i++) {
		var_key = _temp_widget._vars_range_table->item(i, varnote)->whatsThis().trimmed();
		var_note = _temp_widget._vars_range_table->item(i, varnote)->text().trimmed();
		var_value = QString("[%1,%2]").arg(_temp_widget._vars_range_table->item(i, varmin)->text().trimmed())
			.arg(_temp_widget._vars_range_table->item(i, varmax)->text().trimmed());
		QJsonObject itemobj;
		itemobj.insert("note", var_note);
		itemobj.insert(var_key, var_value);
		_variables_obj.insert(var_key, itemobj);
	}
}

bool wizardOptimizeVariables::isComplete() const {
	return true;
}

wizardOptimizeVariables::~wizardOptimizeVariables() {}