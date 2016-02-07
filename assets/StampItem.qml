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

// This is another Stamp Container used by the list to present a small thumbnail image of the stamps.
Container {
    id: stampContainer
    
    layout: DockLayout {
    }
    
    // A colored Container is used to highlight the item on selection.
    Container {
        id: highlight
        background: Color.Black
        opacity: 0.0
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill 
        accessibilityMode: A11yMode.Collapsed       
    }
    
    Label {
        multiline: true
        text: "<html><b>" + ListItemData.room + "</b>: " + ListItemData.time+ "</html>" 
    }
    
    accessibility:CustomA11yObject  {
        role: A11yRole.ListItem
        name: "Stamp Image"
        description: "Select item for more information."
        
        // When in A11y mode we need to seed a signal to the ListView to trigger the navigation.
        ComponentA11ySpecialization {
            onActivationRequested: {
                if (event.type == A11yComponentActivationType.Release) {
                    stampContainer.ListItem.view.triggered(stampContainer.ListItem.indexPath)
                }
            }
        }
    }
    
    // Both the activation and selection of an item has the same visual appearance, we alter the opacity of the item.
    function setHighlight (highlighted) {
        if (highlighted) {
            highlight.opacity = 0.2;
        } else {
            highlight.opacity = 0.0;
        }
    }

    // Signal handler for ListItem activation
    ListItem.onActivationChanged: {
        setHighlight (ListItem.active);
    }

    // Signal handler for ListItem selection
    ListItem.onSelectionChanged: {
        setHighlight (ListItem.selected);
    }
}
