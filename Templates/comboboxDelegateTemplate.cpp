#include "comboboxDelegateTemplate.h"

comboboxDelegate::comboboxDelegate(int combobox_column, QObject* parent) : QItemDelegate(parent) {
	_combobox_column = combobox_column;
	_low = -180;
	_up = 180;
	_step = 5;
}

QWidget* comboboxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	if (index.column() == _combobox_column) {
		QComboBox *comb = new QComboBox(parent);
		return comb;
	}
	else {
		return QItemDelegate::createEditor(parent, option, index);
	}
}