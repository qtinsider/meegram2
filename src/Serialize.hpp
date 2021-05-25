#pragma once

#include <nlohmann/json.hpp>

#include <QStringList>
#include <QVariant>

namespace nlohmann {
template <>
struct adl_serializer<QString>
{
    static void to_json(json &j, const QString &value)
    {
        j = value.toStdString();
    }

    static void from_json(const json &j, QString &value)
    {
        if (j.is_null())
        {
            value = QString();
        }
        else
        {
            value = QString::fromStdString(j.get<std::string>());
        }
    }
};

template <>
struct adl_serializer<QByteArray>
{
    static void to_json(json &j, const QByteArray &value)
    {
        j = value.constData();
    }

    static void from_json(const json &j, QByteArray &value)
    {
        if (j.is_null())
        {
            value = QByteArray();
        }
        else
        {
            value = QByteArray(j.get<std::string>().c_str());
        }
    }
};

template <>
struct adl_serializer<QVariant>
{
    static void to_json(json &j, const QVariant &value)
    {
        if (!value.isValid())
        {
            j = nullptr;
        }
        else if (value.type() == QVariant::Map)
        {
            QMapIterator<QString, QVariant> it(value.toMap());
            while (it.hasNext())
            {
                it.next();
                j.emplace(it.key().toStdString(), it.value());
            }
        }
        else if (value.type() == QVariant::List)
        {
            QListIterator<QVariant> it(value.toList());

            while (it.hasNext())
            {
                j.emplace_back(it.next());
            }
        }
        else if (value.type() == QVariant::StringList)
        {
            QListIterator<QString> it(value.toStringList());

            while (it.hasNext())
            {
                j.emplace_back(it.next());
            }
        }
        else if (value.type() == QVariant::String)
        {
            j = value.toString();
        }
        else if (value.type() == QVariant::ByteArray)
        {
            j = value.toByteArray();
        }
        else if (value.type() == QVariant::Double)
        {
            j = value.toDouble();
        }
        else if (value.type() == QVariant::Bool)
        {
            j = value.toBool();
        }
        else if (value.type() == QVariant::LongLong)
        {
            j = value.toLongLong();
        }
        else if (value.type() == QVariant::Int)
        {
            j = value.toInt();
        }
    }

    static void from_json(const json &j, QVariant &value)
    {
        if (j.is_null())
        {
            value = QVariant();
        }
        else if (j.is_array())
        {
            QVariantList result = value.toList();
            result.reserve(j.size());
            for (auto it = j.begin(); it != j.end(); ++it)
            {
                result.push_back(*it);
            }

            value.setValue(result);
        }
        else if (j.is_object())
        {
            QVariantMap result = value.toMap();
            for (auto it = j.begin(); it != j.end(); ++it)
            {
                result.insert(QString::fromStdString(it.key()), it.value());
            }
            value.setValue(result);
        }
        else if (j.is_string())
        {
            value = QString::fromStdString(j.get<std::string>());
        }
        else if (j.is_boolean())
        {
            value = j.get<bool>();
        }
        else if (j.is_number_float())
        {
            value = j.get<double>();
        }
        else if (j.is_number_integer())
        {
            value = QString::number(j.get<qint64>());
        }
    }
};

template <typename T>
struct adl_serializer<QList<T>>
{
    static void to_json(json &j, const QList<T> &value)
    {
        QListIterator<T> it(value);

        while (it.hasNext())
        {
            j.emplace_back(it.next());
        }
    }

    static void from_json(const json &j, QList<T> &value)
    {
        for (auto it = j.begin(); it != j.end(); ++it)
        {
            value.push_back(*it);
        }
    }
};

template <typename K, typename V>
struct adl_serializer<QMap<K, V>>
{
    static void to_json(json &j, const QMap<K, V> &value)
    {
        QMapIterator<K, V> it(value);

        while (it.hasNext())
        {
            it.next();
            j.emplace(it.key().toStdString(), it.value());
        }
    }

    static void from_json(const json &j, QMap<K, V> &value)
    {
        for (auto it = j.begin(); it != j.end(); ++it)
        {
            value.insert(QString::fromStdString(it.key()), it.value());
        }
    }
};

}  // namespace nlohmann
