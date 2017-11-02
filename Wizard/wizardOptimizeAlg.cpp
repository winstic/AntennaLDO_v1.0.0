#pragma execution_character_set("utf-8")
#include "../Utility/parseJson.h"
#include "wizardOptimizeAlg.h"

wizardOptimizeAlg::wizardOptimizeAlg(parsProblem* atn_problem, QJsonObject algorithm_obj, parsAlgorithm* palgorithm, QWidget *parent)
	: QWizardPage(parent), _atn_problem(atn_problem), _algorithm_obj(algorithm_obj), _algorithm(palgorithm){
    setTitle("优化算法设置");
    setSubTitle("设置算法参数");

	_algorithm_widget = new algorithmTemplate(_atn_problem, _algorithm_obj, _algorithm);

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