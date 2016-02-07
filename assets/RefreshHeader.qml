import bb.cascades 1.3

Container {
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
}
