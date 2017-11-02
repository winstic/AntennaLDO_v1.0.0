#pragma once

#include <QThread>
#include <QProcess>
#include "../Utility/global.h"

class optimizeRun : public QThread
{
public:
    optimizeRun(parsProblem* atn_problem, parsAlgorithm* algorithm, QProcess* &optRunP);
    ~optimizeRun(){
        this->quit();
    }

protected:
    void run();

private:
    QProcess *p;
	parsProblem* _atn_problem;
	parsAlgorithm* _algorithm;
	QString _optimize_path;
};
