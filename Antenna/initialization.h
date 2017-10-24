#pragma once
#include "../Utility/global.h"
#include "../Utility/parseJson.h"

void setProblemParameters(QJsonObject& obj) {	
	parsProblem* tmp;
	QJsonObject mpro_obj;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		mpro_obj = iter.value().toObject();
		tmp->name = iter.key();
		tmp->id = mpro_obj.value("id").toString().trimmed().toInt();
		tmp->path = mpro_obj.value("path").toString().trimmed();
		tmp->pImage = mpro_obj.value("image").toString().trimmed();
		if (tmp->pImage == "" || tmp->pImage == NULL)
			tmp->pImage = "./images/antenna.png";
		tmp->info = mpro_obj.value("info").toString().trimmed();
		tmp->type = mpro_obj.value("type").toString().trimmed().toInt();
		tmp->oper = mpro_obj.value("oper").toString().trimmed().toInt();
		dataPool::g_problems.push_back(tmp);
	}
}

void setAlgorithmParameters(QJsonObject& obj) {
	parsAlgorithm* tmp;
	QJsonObject malg_obj;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		malg_obj = iter.value().toObject();
		tmp->name = iter.key();
		tmp->id = malg_obj.value("id").toString().trimmed().toInt();
		tmp->path = malg_obj.value("path").toString().trimmed();
		tmp->info = malg_obj.value("info").toString().trimmed();
		tmp->oper = malg_obj.value("oper").toString().trimmed().toInt();
		dataPool::g_algorithms.push_back(tmp);
	}
}

void setProNdAlgAssociateParameters(QJsonObject& obj) {
	alg4pro massociate;
	QJsonObject mbind_obj;
	for (QJsonObject::iterator iter = obj.begin(); iter != obj.end(); ++iter) {
		mbind_obj = iter.value().toObject();
		massociate = qMakePair(mbind_obj.value("algorithmID").toString().trimmed().toInt(), mbind_obj.value("problemID").toString().trimmed().toInt());
		dataPool::g_associates[massociate] = iter.key().toInt();
	}
}

void loadData() {
	const QString problem_json_file = "../Utility/problems.json";
	const QString algorithm_json_file = "../Utility/algorithms.json";
	const QString associate_json_file = "../Utility/associates.json";
	QJsonObject pro_obj, alg_obj, associate_obj;
	pro_obj = parseJson::getJsonObj(problem_json_file);
	alg_obj = parseJson::getJsonObj(algorithm_json_file);
	associate_obj = parseJson::getJsonObj(associate_json_file);
	if (pro_obj.isEmpty()) 
		qFatal("something wrong about problems.json.");
	if (alg_obj.isEmpty())
		qFatal("something wrong about algorithms.json.");
	if (associate_obj.isEmpty())
		qFatal("something wrong about associates.json.");

	setProblemParameters(pro_obj);
	setAlgorithmParameters(alg_obj);
	setProNdAlgAssociateParameters(associate_obj);
}