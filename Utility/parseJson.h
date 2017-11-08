#pragma once

#include <QJsonObject>

namespace parseJson {
	QJsonObject getJsonObj(const QString &path);
	QJsonObject getSubJsonObj(const QJsonObject &obj, const QString &key);
	bool write(const QString &path, QJsonObject* obj);
	void updateConfigFile(const QString& key, const QString& value);
}