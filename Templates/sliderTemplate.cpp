#include "sliderTemplate.h"
#include <QPalette> 

sliderTemplate::sliderTemplate(const double& value_low, const double& value_up, QSlider *parent) : QSlider(parent), 
_value_low(value_low), _value_up(value_up), _min_number(0), _max_number(100), _step_number(1){
	_display_label = new QLabel(this);
	_display_label->setFixedSize(QSize(20, 20));
	//设置游标背景为白色  
	//_display_label->setAutoFillBackground(true);
	QPalette palette;
	palette.setColor(QPalette::Background, Qt::red);
	_display_label->setPalette(palette);
	_display_label->setAlignment(Qt::AlignCenter);
	//_display_label->setVisible(false);
	_display_label->move(0, 3);

	this->setOrientation(Qt::Horizontal);
	this->setMinimum(_min_number);
	//var_slider->setProperty()
	this->setMaximum(_max_number);
	this->setSingleStep(_step_number);
	initSliderSheet();
}

void sliderTemplate::initSliderSheet() {
	QString sheet = QString("  \
         QSlider\
         {     \
            spacing: 0px;\
            min-height:8px;\
            max-height:8px;\
         }\
         QSlider::add-page:Horizontal\
         {     \
            background-color: rgb(222, 231, 234);\
            height:8px;\
         }\
         QSlider::sub-page:Horizontal \
        {\
            background-color: rgb(37, 168, 198);\
            height:8px;\
         }\
        QSlider::groove:Horizontal \
        {\
            background:transparent;\
            height:8px;\
        }\
        QSlider::handle:Horizontal \
        {\
            height: 13px;\
            width:13px;\
            border-image: url(./images/dot_16px.png);\
             margin: 0px; \
         }\
        ");
	setStyleSheet(sheet);
}

void sliderTemplate::mousePressEvent(QMouseEvent *event){
	if (!_display_label->isVisible())	{
		_display_label->setVisible(true);
	}
	double pox = event->pos().x() / (double)width();
	setValue(pox * (_max_number - _min_number) + _min_number);
	double curr_value = 1.0 * this->value() / (_max_number - _min_number) * (_value_up - _value_low) + _value_low;
	_display_label->setText(QString::number(curr_value));

	QSlider::mousePressEvent(event);
}

void sliderTemplate::mouseReleaseEvent(QMouseEvent *event){
	if (_display_label->isVisible())	{
		_display_label->setVisible(false);
	}
	QSlider::mouseReleaseEvent(event);
}

void sliderTemplate::mouseMoveEvent(QMouseEvent *event){
	if (!_display_label->isVisible()) {
		_display_label->setVisible(true);
	}
	double curr_value = 1.0 * this->value() / (_max_number - _min_number) * (_value_up - _value_low) + _value_low;
	_display_label->setText(QString::number(curr_value));
	_display_label->move((this->width() - _display_label->width())*this->value() / (this->maximum() - this->minimum()), 3);
	QSlider::mouseMoveEvent(event);
}

sliderTemplate::~sliderTemplate() {}