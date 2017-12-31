#include "commonStyle.h"

void commonStyle::setHintStyle(QLabel* hint) {
	QFont font;
	font.setPixelSize(20);
	hint->setFont(font);
	hint->setStyleSheet("color:red;");
}

void commonStyle::setLineEditWarningStyle(QLineEdit* edit) {
	edit->setStyleSheet("border: 1px solid red;");
}
void commonStyle::clearLineEditWarningStyle(QLineEdit* edit) {
	edit->setStyleSheet("background-color: white;");
}