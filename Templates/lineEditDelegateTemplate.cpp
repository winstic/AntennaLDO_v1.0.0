#include "lineEditDelegateTemplate.h"

lineEditDelegate::lineEditDelegate(QList<int> &combobox_columns, QObject* parent) : QItemDelegate(parent),
_line_edit_columns(combobox_columns) {
	_itemplate = new iTemplate;
}

void lineEditDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QItemDelegate::paint(painter, option, index);
}


QWidget* lineEditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	if (_line_edit_columns.contains(index.column())) {
		QLineEdit *edit = new QLineEdit(parent);
		QRegExpValidator* floatValidReg = _itemplate->getFloatReg();    //float
		edit->setValidator(floatValidReg);
		connect(edit, SIGNAL(textChanged(QString)), this, SLOT(slot_commitAndCloseEditor(QString)));
		return edit;
	}
	else {
		return QItemDelegate::createEditor(parent, option, index);
	}
}

void lineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	int icol = index.column();
	if (_line_edit_columns.contains(icol)) {
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
		edit->setText(text);
	}
	else {
		QItemDelegate::setEditorData(editor, index);
	}
}

void lineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	if (_line_edit_columns.contains(index.column())) {
		QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
		model->setData(index, edit->text());
	}
	else {
		QItemDelegate::setModelData(editor, model, index);
	}
}

void lineEditDelegate::slot_commitAndCloseEditor(QString) {
	QLineEdit *edit = qobject_cast<QLineEdit *>(sender());
	emit commitData(edit);
	emit closeEditor(edit);
}