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
import bb.cascades 1.3
import Network.RequestHeaders 1.0
import "controls"
import my.library 1.0

TabbedPane {
    attachedObjects: [
        ComponentDefinition {
            id: helpSheetDefinition
            HelpSheet {

            }
        },
        ComponentDefinition {
            id: settingsSheetDefinition
            SettingsSheet {

            }
        }
    ]

    Menu.definition: MenuDefinition {
        // Add a Help action
        helpAction: HelpActionItem {
            onTriggered: {
                var help = helpSheetDefinition.createObject(app)
                help.open();
            }
        }

        // Add any remaining actions
        actions: [
            ActionItem {
                title: qsTr("Invite To Download") + Retranslate.onLanguageChanged
                imageSource: "asset:///images/invite.png"
                enabled: bbmHandler.allowed
                onTriggered: {
                    bbmHandler.sendInvite();
                }
            }

        ]

        // Add a Settings action
        settingsAction: SettingsActionItem {
            onTriggered: {
                var settings = settingsSheetDefinition.createObject(app)
                settings.open();
            }
        }
    }

    paneProperties: NavigationPaneProperties {

    }

    Tab {
        title: qsTr("Home") + Retranslate.onLanguageChanged
        imageSource: "asset:///images/home.png"
        delegate: Delegate {
            Home {
            }
        }
    }
    Tab {
        title: qsTr("McMaster")
        imageSource: "asset:///images/get.png"
        
        Page {
            titleBar: TitleBar {
                title: qsTr("requestinfo") + Retranslate.onLanguageChanged
            }
            Container {
                
                layout: DockLayout {}
                
                // The background image
                ImageView {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    
                    imageSource: "asset:///images/background.png"
                }
                //! [0]
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    leftPadding: ui.du(5.6)
                    
                    TextArea {
                        id: headers
                        
                        visible: false
                        editable: false
                        backgroundVisible: false
                        
                        text: qsTr("Retrieving Headers")
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText;
                            color: Color.White
                        }
                    }
                }
                
                NetworkActivity {
                    id: progressIndicator
                    
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    
                    title: qsTr("Retrieving Headers")
                }
                
                attachedObjects: [
                    QTimer {
                        id: timer
                        interval: 1000
                        onTimeout: {
                            netheaders.getRequest();
                        }
                    },
                    RequestHeaders {
                        id : netheaders
                        onComplete :{
                            progressIndicator.active = false;
                            progressIndicator.visible = false;
                            
                            headers.text = info;
                            headers.visible = true;
                            
                            timer.stop();
                        }
                    }
                ]
                
                onCreationCompleted: {
                    progressIndicator.active = true;
                    timer.start();
                }
                //! [0]
            }
        }
    }
    Tab {
        title: qsTr("BBM") + Retranslate.onLanguageChanged
        imageSource: "asset:///images/bbm.png"
        delegate: Delegate {
            BBM {
            }
        }
    }
    Tab {
        title: qsTr("Invoke") + Retranslate.onLanguageChanged
        imageSource: "asset:///images/share.png"
        delegate: Delegate {
            Invoke {
            }
        }
    }
    Tab {
        title: qsTr("Toasts") + Retranslate.onLanguageChanged
        imageSource: "asset:///images/toast.png"
        delegate: Delegate {
            Toasts {
            }
        }
    }
    Tab {
        title: qsTr("Spinners") + Retranslate.onLanguageChanged
        imageSource: "asset:///images/spinner.png"
        delegate: Delegate {
            Spinners {
            }
        }

    }
}
