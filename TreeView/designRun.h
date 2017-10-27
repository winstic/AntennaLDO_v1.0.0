#pragma once

#include <QThread>
#include <QFile>
#include <QProcess>
#include "../Utility/global.h"

class designRun : public QThread{
public:
    designRun(parsProblem* atn_problem, QJsonObject& obj);
    ~designRun(){
        this->quit();
    }

protected:
	//override run function
	void run();

private:
    bool registerHfssVars();
    bool updateVbs();
    QString M2GHz(QString mhz);

private:
	parsProblem* _atn_problem;
	QJsonObject _obj;
	QString _design_path;
    QMap<QString, QString> vbsVars;
};