import bb.cascades 1.3

Container {
    signal triggerRefresh()
    property bool refreshing: false
    property bool refreshCocked: false
    property int pullThreshold: 50
    id: refreshHeaderContainer
    
    ImageView {
        id: refreshImage
        imageSource: "asset:///images/spinner.png"
        horizontalAlignment: HorizontalAlignment.Center
    }
    Label {
        id: refreshText
        horizontalAlignment: HorizontalAlignment.Center
        text: qsTr("Pull down to refresh")
    }
    
    function onListViewTouch(event) {
        if(!refreshing) {
            refreshHeaderContainer.resetPreferredHeight();
            
            if(event.touchType == TouchType.Up) {
                if (refreshCocked) {
                    refreshCocked = false;
                    refreshing = true;
                    refreshHeaderContainer.visible = false;
                    refreshHeaderContainer.setPreferredHeight(0);
                    triggerRefresh();
                    refreshHeaderContainer.visible = true;
                    refreshing = false;
                }
            }
        }
    }
    attachedObjects: [
        LayoutUpdateHandler {
            id: refreshHandler
            onLayoutFrameChanged: {
                if(!refreshing) {
                    if(!refreshCocked && (layoutFrame.y >= pullThreshold)) {
                        refreshImage.rotationZ = 180;
                        refreshText.text = qsTr("Release to refresh");
                        refreshCocked = true;
                    }
                    else if (refreshCocked && layoutFrame.y < pullThreshold) {
                        refreshImage.rotationZ = 0.0;
                        refreshText.text = qsTr("Pull down to refresh");
                        refreshCocked = false;
                    }
                }
            }
        }
    ]
}
