#ifndef FINDCHARACTERS_H
#define FINDCHARACTERS_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui {
class FindCharacters;
}
QT_END_NAMESPACE

class FindCharacters : public QWidget
{
    Q_OBJECT

public:
    FindCharacters(QWidget *parent = nullptr);
    ~FindCharacters();

public slots:
    void update_text_area(QString result);

private:

    enum OperStates{
        FETCH_EPISODE,
        PROCESS_EPISODE_DATA,
        FETCH_CHARACTER,
        PROCESS_CHARACTER_DATA,
        UPDATE_UI,
        CLEAR_RESOURCES
    } operState;

    Ui::FindCharacters *ui;
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QStringList http_responses_data;
    QList<QUrl> charUrlList;
    QStringList charNames;

    void update_process_state();
    void execute_http_request(QUrl url);
    void parse_http_request_result(QNetworkReply *reply);
    QJsonObject to_json(QString jsonString);

private slots:
    void on_find_characters_clicked();

};
#endif // FINDCHARACTERS_H
