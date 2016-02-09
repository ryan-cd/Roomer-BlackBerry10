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

    //Kick off BBM registration
    const QString uuid(QLatin1String("fd34de4e-8671-442a-8c42-8c4743463400"));
    BBMHandler *bbmHandler = new BBMHandler(uuid, app);
    bbmHandler->registerApplication();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    //Expose BBM registration handler to main.qml
    qml->setContextProperty("bbmHandler", bbmHandler);
	//Expose the ApplicationUI in main.qml
	qml->setContextProperty("app", this);

	// Create Active Frame (shown when app is miminized)
	ActiveFrameQML *activeFrame = new ActiveFrameQML();
	Application::instance()->setCover(activeFrame);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    ListView *roomList = root->findChild<ListView*>("roomList");
    this->roomListView = roomList;

    // Set created root object as the application scene
    app->setScene(root);

    bool REFRESH_CONNECT = QObject::connect(this, SIGNAL(refresh()), this, SLOT(onRefresh()));
    Q_ASSERT(REFRESH_CONNECT);
    Q_UNUSED(REFRESH_CONNECT);

    refresh();
}

void ApplicationUI::onRefresh()
{
    setUpRoomListModel(this->roomListView);
    RequestHeaders* requestHeaders = new RequestHeaders();
    requestHeaders->getRequest();
    bool DATA_CONNECT = QObject::connect(requestHeaders, SIGNAL(dataComplete(QMap<QString, QVariant>)), this, SLOT(onDataComplete(QMap<QString, QVariant>)));
    Q_ASSERT(DATA_CONNECT);
    Q_UNUSED(DATA_CONNECT);
}

void ApplicationUI::setUpRoomListModel(ListView *roomList)
{
    bb::data::JsonDataAccess jda;
    GroupDataModel *roomModel = new GroupDataModel(QStringList() << "building" << "room");

    roomModel->setGrouping(ItemGrouping::ByFullValue);
    roomModel->setParent(this);

    QVariantMap map;
    map["building"] = "Info"; map["room"] = "Server Message"; map["time"] = "Retrieving..."; roomModel->insert(map);

    roomList->setDataModel(roomModel);
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

    //qDebug() << result;

    //server error message
    if(result.keys().takeAt(0) == "code")
    {
        map["building"] = "Info";
        map["room"] = "Server Message";
        map["time"] = result.values().takeAt(1);
        roomModel->insert(map);
    }
    //app error message
    else if(result.keys().takeAt(0) == "appError")
    {
        map["building"] = "Device Error";
        map["room"] = "Info";
        map["time"] = result.values().takeAt(0);
        roomModel->insert(map);
    }
    //regular server response
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
    refreshDone();
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
