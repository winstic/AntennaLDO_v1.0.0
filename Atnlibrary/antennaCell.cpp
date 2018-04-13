#include "antennaCell.h"
#include "../Utility/macrodefined.h"

antennaCell::antennaCell(parsProblem* atn_problem, QFrame* parent) : QFrame(parent), _atn_problem(atn_problem) {
	_atn_name_label = new QLabel(QString("%1 Antenna").arg(_atn_problem->name));
	_atn_photo = new QLabel;
	QPixmap mPixmap = QPixmap(_atn_problem->pImage);
	_atn_photo->setPixmap(mPixmap.scaled(ANTENNA_IMGWH, ANTENNA_IMGWH, Qt::KeepAspectRatio));

	//QGroupBox* group_box = new QGroupBox;
	_vlayout = new QVBoxLayout;
	_hlayout = new QHBoxLayout;
	_vlayout->addWidget(_atn_photo);
	_vlayout->addWidget(_atn_name_label);	
	_vlayout->setStretch(0, 3);
	_vlayout->setStretch(1, 1);
	//group_box->setLayout(_vlayout);
	//_hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	_hlayout->addStretch();
	//_hlayout->addWidget(group_box);
	_hlayout->addLayout(_vlayout);
	_hlayout->addStretch();
	setLayout(_hlayout);
	setMouseTracking(true);
}

void antennaCell::paintEvent(QPaintEvent *event){
	/*QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
	painter.setBrush(QBrush(Qt::GlobalColor::black));
	painter.setPen(Qt::transparent);
	QRect rect = this->rect();
	rect.setWidth(rect.width() - 1);
	rect.setHeight(rect.height() - 1);
	painter.drawRoundedRect(rect, 15, 15);*/
	QWidget::paintEvent(event);
}

parsProblem* antennaCell::getAntennaProblem() const {
	if (_atn_problem == nullptr)
		qCritical("uninitialize problem data struct");
	return _atn_problem;
}

antennaCell::~antennaCell() {
	delete _hlayout;
	_hlayout = nullptr;
}