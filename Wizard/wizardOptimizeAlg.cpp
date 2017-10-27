#include "../Utility/parseJson.h"
#include "wizardOptimizeAlg.h"

wizardOptimizeAlg::wizardOptimizeAlg(parsProblem* atn_problem, QJsonObject global_obj, QJsonObject algorithm_obj, QWidget *parent) 
	: QWizardPage(parent), _atn_problem(atn_problem), _global_obj(global_obj), _algorithm_obj(algorithm_obj){
    setTitle(tr("优化算法设置"));
    setSubTitle(tr("设置算法参数"));

	_algorithm_widget = new algorithmTemplate(_atn_problem, _global_obj, _algorithm_obj);

   //layout
	QLayout* layout = _algorithm_widget->getLayout();
	setLayout(layout);
}

bool wizardOptimizeAlg::isComplete() const{
    return true;
}

QList<iTemplate*> wizardOptimizeAlg::getTemplatesWidget() const {
	return QList<iTemplate*> {_algorithm_widget};
}

wizardOptimizeAlg::~wizardOptimizeAlg() {
	delete _algorithm_widget;
	_algorithm_widget = nullptr;
}