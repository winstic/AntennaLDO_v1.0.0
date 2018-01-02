#pragma once
#include <QtWidgets>

class commonStyle {
public:
	commonStyle(){}
	~commonStyle(){}
	static void setHintStyle(QLabel* hint);
	static void setLineEditWarningStyle(QLineEdit* edit);
	static void clearLineEditWarningStyle(QLineEdit* edit);
	static void setComboBoxWarningStyle(QComboBox* comb);
	static void clearComboBoxWarningStyle(QComboBox* comb);
};