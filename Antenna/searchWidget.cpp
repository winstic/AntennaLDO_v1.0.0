#pragma execution_character_set("utf-8")
#include "searchWidget.h"
#include "../Utility/macrodefined.h"

searchWidget::searchWidget(QWidget *parent) : QWidgetAction(parent){
	_search_edit = new QLineEdit();
	_search_edit->setFixedWidth(SEARCH_EDIT_WIDTH);
	_search_edit->setPlaceholderText("搜索天线模型");
	_search_edit->setClearButtonEnabled(true);
    QWidget* search_widget = new QWidget;
	search_widget->setWindowFlags(Qt::FramelessWindowHint);
	QHBoxLayout *lay = new QHBoxLayout();
	lay->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	lay->addWidget(_search_edit);
	search_widget->setLayout(lay);
	setDefaultWidget(search_widget);
}