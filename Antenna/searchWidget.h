#pragma once

#include <QtWidgets>

class  searchWidget :public QWidgetAction
{
public:
	searchWidget(QWidget *parent = 0);
	~searchWidget() {}
	inline QLineEdit* getSearchEdit() { return _search_edit; }
	inline QPushButton* getSearchButton() { return _search_button; }

private:
	QLineEdit* _search_edit;
	QPushButton* _search_button;
};