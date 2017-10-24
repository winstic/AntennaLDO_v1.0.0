#pragma once

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonParseError>
#include <QString>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include "global.h"

namespace parseJson {

QJsonObject getJsonObj(const QString &path) {
	QJsonObject obj;
	QFile file(path);
	if (!file.open(QFile::ReadOnly | QFile::Text))
		qCritical(dataPool::str2char(QString("Cannot read file %1").arg(path)));	
	else {
		QByteArray byteArray = file.readAll();
		file.close();
		QJsonParseError jsonError;
		QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArray, &jsonError);
		if (jsonError.error == QJsonParseError::NoError) {
			if (jsonDocument.isObject()) {
				obj = jsonDocument.object();
			}
		}
	}
	return obj;
}

QJsonObject getSubJsonObj(const QJsonObject &obj, const QString &key) {
	QJsonObject subObj;
	if (obj.contains(key)) {
		QJsonValue value = obj.value(key);
		if (value.isObject())
			subObj = value.toObject();
	}
	return subObj;
}

bool write(const QString &path, QJsonObject* obj) {
	if (obj == nullptr) {
		qCritical("json object is empty when update to file");
		return false;
	}
	QFile file(path);
	if (!file.open(QFile::WriteOnly)) {
		qCritical(dataPool::str2char(QString("Cannot write file [%1]").arg(path)));
		return false;
	}
	QJsonDocument jsonDocument;
	jsonDocument.setObject(*obj);
	QByteArray byteArray = jsonDocument.toJson(QJsonDocument::Indented);
	QTextStream out(&file);
	out.setCodec("UTF-8");
	out << QString(byteArray);
	file.close();
	qInfo(dataPool::str2char(QString("write file [%1] successed.").arg(path)));
	return true;
}
}