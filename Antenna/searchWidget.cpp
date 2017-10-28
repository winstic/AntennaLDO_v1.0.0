#include "searchWidget.h"

searchWidget::searchWidget(QWidget *parent) : QWidgetAction(parent){
    _search_button = new QPushButton("Search");
	_search_edit = new QLineEdit();

	_search_edit->setPlaceholderText("搜索天线模型");
	_search_edit->setClearButtonEnabled(true);
    QWidget search_widget;
	search_widget.setWindowFlags(Qt::FramelessWindowHint);
	QHBoxLayout *lay = new QHBoxLayout();
	lay->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	lay->addWidget(_search_edit);
	lay->addWidget(_search_button);
	lay->setStretch(0, 20);
	lay->setStretch(1, 5);
	lay->setStretch(2, 1);
	search_widget.setLayout(lay);
	this->setDefaultWidget(&search_widget);
}