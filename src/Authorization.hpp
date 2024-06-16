#pragma once

#include <QObject>
#include <QVariant>

class Client;
class TdManager;

class Authorization : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TdManager *manager READ manager WRITE setManager)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
public:
    explicit Authorization(QObject *parent = nullptr);

    TdManager *manager() const;
    void setManager(TdManager *manager);

    bool loading() const;

    Q_INVOKABLE void checkCode(const QString &code) noexcept;
    Q_INVOKABLE void checkPassword(const QString &password) noexcept;
    Q_INVOKABLE void logOut() noexcept;
    Q_INVOKABLE void registerUser(const QString &firstName, const QString &lastName) noexcept;
    Q_INVOKABLE void setPhoneNumber(const QString &phoneNumber) noexcept;
    Q_INVOKABLE void resendCode() noexcept;
    Q_INVOKABLE void deleteAccount(const QString &reason) noexcept;

signals:
    void codeRequested(const QString &phoneNumber, const QVariantMap &type, const QVariantMap &nextType, int timeout);
    void passwordRequested(const QString &passwordHint, bool hasRecoveryEmailAddress, const QString &recoveryEmailAddressPattern);
    void registrationRequested(const QString &text, int minUserAge, bool showPopup);
    void ready();
    void loggingOut();

    void error(const QString &errorString);

    void loadingChanged();

public slots:
    QString formatTime(int totalSeconds) const noexcept;

private slots:
    void handleResult(const QVariantMap &object);

private:
    void setLoading(bool value);

    bool m_loading{true};

    Client *m_client;
    TdManager *m_manager;
};
