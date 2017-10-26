#include "wizardOptimizeAlg.h"
#include "../Utility/parseJson.h"

wizardOptimizeAlg::wizardOptimizeAlg(parsProblem* atn_problem, QWidget *parent) : QWizardPage(parent),
_atn_problem(atn_problem), _temp_widget(atn_problem){
    setTitle(tr("优化算法设置"));
    setSubTitle(tr("设置算法参数"));

   //layout
	QHBoxLayout	h_layout;
	_temp_widget.initAlgorithmWidget(&h_layout);
	setLayout(&h_layout);

	registerField("gAlg_json_obj", this, "gAlgJsonObj", SIGNAL(signal_gAlgJsonObjChanged()));
}

void wizardOptimizeAlg::initializePage() {
	updateGAlgJsonObj();
	emit signal_gAlgJsonObjChanged();
}

void wizardOptimizeAlg::updateGAlgJsonObj(){
    QJsonObject uglobal_obj, ualg_obj;
	uglobal_obj.insert("ALGORITHM_NAME", _temp_widget._algorithm->name);
	uglobal_obj.insert("PROBLEM_NAME", _atn_problem->name);
	_gAlg_obj.insert("global", uglobal_obj);

    QString varKey, varValue, varNote;
    for(int i = 0; i < _temp_widget._alg_vars_table->rowCount(); ++i){
        varKey = _temp_widget._alg_vars_table->item(i, keyFlag)->whatsThis().trimmed();
        varNote = _temp_widget._alg_vars_table->item(i, keyFlag)->text().trimmed();
        varValue = QString("%1").arg(_temp_widget._alg_vars_table->item(i, valueFlag)->text().trimmed());
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