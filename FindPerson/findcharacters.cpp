#include "findcharacters.h"
#include "./ui_findcharacters.h"

FindCharacters::FindCharacters(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FindCharacters)
    , operState{FETCH_EPISODE}
{
    ui->setupUi(this);

    QObject::connect(
        &manager,
        &QNetworkAccessManager::finished,
        this,
        &FindCharacters::parse_http_request_result
    );
}

FindCharacters::~FindCharacters()
{
    delete ui;
}

void FindCharacters::on_find_characters_clicked()
{
    update_process_state();
}

void FindCharacters::update_text_area(QString result)
{
    ui->textEdit->clear();
    ui->textEdit->append(result);
}

void FindCharacters::update_process_state()
{
    switch (operState) {
    case OperStates::FETCH_EPISODE:
        {
            QString episodeId = ui->lineEdit->text();
            QString url = "https://rickandmortyapi.com/api/episode/" + episodeId;

            execute_http_request(QUrl(url));
        }
        break;

    case OperStates::PROCESS_EPISODE_DATA:
        {
            /*
             * Extract the characteres list
             */

            QJsonObject epJson = to_json(http_responses_data.front());
            http_responses_data.pop_front();

            if (epJson.value("characters") != QJsonValue::Undefined) {
                QJsonArray charList = epJson.value("characters").toArray();
                for (QJsonValueRef c : charList) {
                    charUrlList.push_back(QUrl(c.toString()));
                }
            }
        }

        operState = OperStates::FETCH_CHARACTER;

    case OperStates::FETCH_CHARACTER:

        for (QUrl url : charUrlList) {
            execute_http_request(QUrl(url));
        }

        operState = OperStates::PROCESS_CHARACTER_DATA;

    case OperStates::PROCESS_CHARACTER_DATA:

        if (charUrlList.size() != http_responses_data.size()) {
            break;
        }

        /*
         * Extract the character name
         */

        for (QString data : http_responses_data) {
            QJsonObject charJson = to_json(data);
            if (charJson.value("name") != QJsonValue::Undefined) {
                charNames.push_back(charJson.value("name").toString());
            }
        }

        operState = OperStates::UPDATE_UI;

    case OperStates::UPDATE_UI:

        ui->textEdit->clear();

        for (QString name : charNames) {
            ui->textEdit->append(name + QString("\n"));
        }

    case OperStates::CLEAR_RESOURCES:

        http_responses_data.clear();
        charUrlList.clear();
        charNames.clear();

    default:
        operState = OperStates::FETCH_EPISODE;
        break;
    }
}

void FindCharacters::execute_http_request(QUrl url)
{
    request.setUrl(url);

    manager.get(request);
}

void FindCharacters::parse_http_request_result(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    auto url = reply->url().toString();

    if (url.contains("https://rickandmortyapi.com/api/episode")) {

        http_responses_data.append(reply->readAll());

        operState = OperStates::PROCESS_EPISODE_DATA;

    } else if (url.contains("https://rickandmortyapi.com/api/character")) {

        http_responses_data.append(reply->readAll());
    }

    update_process_state();
}

QJsonObject FindCharacters::to_json(QString jsonString) {
    QJsonObject result;

    if (jsonString.isEmpty()) {
        return result;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (!doc.isNull()) {
        result = doc.object();
    }

    return result;
}


