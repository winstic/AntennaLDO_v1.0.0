#include "antennaCell.h"
#include "../Utility/macrodefined.h"

antennaCell::antennaCell(parsProblem* atn_problem, QWidget* parent) : QWidget(parent), _atn_problem(atn_problem) {
	_atn_name_label = new QLabel(QString("%1 Antenna").arg(_atn_problem->name));
	_atn_photo = new QLabel;
	QPixmap mPixmap = QPixmap(_atn_problem->pImage);
	_atn_photo->setPixmap(mPixmap.scaled(ANTENNA_IMGWH, ANTENNA_IMGWH, Qt::KeepAspectRatio));
	_vlayout = new QVBoxLayout;
	_hlayout = new QHBoxLayout;
	_vlayout->addWidget(_atn_photo);
	_vlayout->addWidget(_atn_name_label);	
	_vlayout->setStretch(0, 2);
	_vlayout->setStretch(1, 1);
	_hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	_hlayout->addLayout(_vlayout);
	_hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	setLayout(_hlayout);
	setMouseTracking(true);
}

parsProblem* antennaCell::getAntennaProblem() const {
	if (_atn_problem == nullptr)
		qCritical("uninitialize problem data struct");
	return _atn_problem;
}

antennaCell::~antennaCell() {
	delete _atn_name_label;
	_atn_name_label = nullptr;
	delete _atn_photo;
	_atn_photo = nullptr;
	delete _vlayout;
	_vlayout = nullptr;
	delete _hlayout;
	_hlayout = nullptr;
}