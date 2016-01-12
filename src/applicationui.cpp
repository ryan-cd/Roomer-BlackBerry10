/* Copyright (c) 2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "applicationui.hpp"

#include "bbm/BBMHandler.hpp"
#include "activeFrameQML.h"
#include "RequestHeaders.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/data/JsonDataAccess>
#include <bb/cascades/ListView>
#include <bb/cascades/GroupDataModel>

using namespace bb::cascades;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
{
    //add custom object RequestHeaders class as a qml type
    qmlRegisterType<RequestHeaders>("Network.RequestHeaders", 1, 0, "RequestHeaders");

    //add a QTimer class as a qml type
    qmlRegisterType<QTimer>("my.library", 1, 0, "QTimer");

    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    if(!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }
    // initial load
    onSystemLanguageChanged();

    //Kick off BBM Registration.
	//TODO: Define your own UUID here. You can generate one here: http://www.guidgenerator.com/
	const QString uuid(QLatin1String("fd34de4e-8671-442a-8c42-8c4743463400"));
	BBMHandler *bbmHandler = new BBMHandler(uuid, app);
	bbmHandler->registerApplication();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    //Expose the BBM Registration handler to main.qml.
	qml->setContextProperty("bbmHandler", bbmHandler);
	//Expose the ApplicationUI in main.qml
	qml->setContextProperty("app", this);

	// Create Active Frame (shown when app is miminized)
	ActiveFrameQML *activeFrame = new ActiveFrameQML();
	Application::instance()->setCover(activeFrame);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    ListView *stampList = root->findChild<ListView*>("stampList");
    setUpRoomListModel(stampList);
    this->roomListView = stampList;

    // Set created root object as the application scene
    app->setScene(root);

    RequestHeaders* requestHeaders = new RequestHeaders();
    requestHeaders->getRequest();
    bool ok = QObject::connect(requestHeaders, SIGNAL(complete(QString)), this, SLOT(onComplete(QString)));
    bool ok2 = QObject::connect(requestHeaders, SIGNAL(dataComplete(QMap<QString, QVariant>)), this, SLOT(onDataComplete(QMap<QString, QVariant>)));
    Q_ASSERT(ok2);
    Q_UNUSED(ok2);
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void ApplicationUI::setUpStampListModel(ListView *stampList)
{
    bb::data::JsonDataAccess jda;
    QVariantList mainList = jda.load("app/native/assets/stamps.json").value<QVariantList>();
    if(jda.hasError()) {
        bb::data::DataAccessError error = jda.error();
        qDebug() << "JSON loading error: " << error.errorType() << ": " << error.errorMessage();
        return;
    }

    GroupDataModel *stampModel = new GroupDataModel(QStringList() << "region");
    stampModel->setParent(this);
    stampModel->insertList(mainList);
    stampModel->setGrouping(ItemGrouping::ByFullValue);

    stampList->setDataModel(stampModel);
}

void ApplicationUI::setUpRoomListModel(ListView *roomList)
{
    bb::data::JsonDataAccess jda;
    GroupDataModel *roomModel = new GroupDataModel(QStringList() << "building" << "room");

    roomModel->setGrouping(ItemGrouping::ByFullValue);
    roomModel->setParent(this);

    QVariantMap map;
    map["building"] = "ITB"; map["room"] = "137"; map["time"] = "1111111"; roomModel->insert(map);
    map["building"] = "ETB"; map["room"] = "227"; map["time"] = "1222111"; roomModel->insert(map);

    roomList->setDataModel(roomModel);
}

void ApplicationUI::onComplete(QString result) {
    //qDebug() << result;
    qDebug() << "onComplete";
}

void ApplicationUI::onDataComplete(QMap<QString, QVariant> result) {
    QVariantMap map;
    GroupDataModel *roomModel = new GroupDataModel(QStringList() << "building");
    roomModel->setGrouping(ItemGrouping::ByFullValue);
    roomModel->setParent(this);
    QList<QVariant> building;
    QList<QVariant> room;
    float timeOpen;
    int hoursOpen;
    int minutesOpen;
    QString time;

    qDebug() << result;
    qDebug() << result.keys().takeAt(0) << result.keys().takeAt(1) << result.values().takeAt(0) << result.values().takeAt(1).toString();

    if(result.keys().takeAt(0) == "code")
    {
        map["building"] = "Info";
        map["room"] = "Server Message";
        map["time"] = result.values().takeAt(1);
        roomModel->insert(map);
    }
    else
    {
        for(int i = 0; i < result.keys().count(); i++)
        {
            building = result.values().takeAt(i).toList();
            for(int j = 0; j < building.count(); j++)
            {
                room = building[j].toList();
                map["building"] = result.keys().takeAt(i);
                map["room"] = room[0];

                timeOpen = (float) QDateTime::currentDateTime().secsTo(QDateTime::fromTime_t(room[1].toInt())) / 3600;
                hoursOpen = floorf(timeOpen);
                minutesOpen = roundf(60 * (timeOpen - hoursOpen));
                time = QString::number(hoursOpen) + "h" + QString::number(minutesOpen) + "m";
                map["time"] = time;
                roomModel->insert(map);
            }
        }
    }
    this->roomListView->setDataModel(roomModel);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Boilerplate_Cascades_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

QByteArray ApplicationUI::encodeQString(const QString& toEncode) const {
	return toEncode.toUtf8();
}
