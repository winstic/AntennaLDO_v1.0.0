#pragma once
#include <QtWidgets>
#include "iTemplate.h"
class lineEditDelegate : public QItemDelegate {
	Q_OBJECT
public:
	lineEditDelegate(QList<int> &line_edit_columns, QObject* parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private slots:
	void slot_commitAndCloseEditor(QString);

private:
	iTemplate* _itemplate;
	QList<int> _line_edit_columns;		//设置line edit委托的所有列
};