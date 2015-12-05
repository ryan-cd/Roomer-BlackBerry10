/* Copyright (c) 2012, 2013, 2014 BlackBerry Limited.
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

// Content Page
Page {
    // These aliases are used to connect the data to the this page.
    property alias contentImageURL: stampImage.imageSource
    property alias contentDescription: stampDescription.text

    // Main Container
    Container {
        background: backgroundPaint.imagePaint

        attachedObjects: [
            ImagePaintDefinition {
                id: backgroundPaint
                imageSource: "asset:///images/Scribble_light_256x256.amd"
                repeatPattern: RepeatPattern.XY
            }
        ]

        layout: DockLayout {
        }

        // Stamp Container with image and descriptive text setup
        Container {
            leftPadding: ui.du(3)
            rightPadding: leftPadding
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

            layout: StackLayout {
            }

            // This is the Large stamp image. Source is updated as an item is selected
            // in the main.qml ListView onTriggered signal handler.
            ImageView {
                id: stampImage
                imageSource: "asset:///images/BlueNoseBig.png"
                preferredWidth: ui.du(55)
                preferredHeight: ui.du(45)
                horizontalAlignment: HorizontalAlignment.Center
                accessibility.name: "Stamp Image"

                // Since the large stamp images vary in size, the AspectFit property is used to
                // make sure that the image fits in the preferred size and preserves the aspect ratio.
                scalingMethod: ScalingMethod.AspectFit

            }

            Label {
                id: stampDescription
                topMargin: ui.du(3)
                text: "This is where the stamp Description goes."
                multiline: true
                textStyle.base: SystemDefaults.TextStyles.BodyText
                textStyle.color: Color.create("#262626")

                accessibility {
                    name: "Stamp information label."
                    description: stampDescription.text
                }
            }
        } // Container
    } // top Container
}// Page
