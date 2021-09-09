import QtQuick 2.5
import QtQml 2.1

//import QtQuick.Controls 2.5

Item {

    width: 540
    height: 540
    visible: true

    Rectangle
    {
        id: mainwindow
        //objectName: "mainwindow"
        width: 540
        height: 540;
        color: Qt.rgba(0.2,0.2,0.2,1)
        property int editable: 0

    }

    //-------- Global Mouse Area
    MouseArea
    {
        id: mousearea
        //objectName: "mousearea"
        anchors.fill: parent
        propagateComposedEvents: true
        hoverEnabled: true         //this line will enable mouseArea.containsMouse

        // send x/y coordinates when we click-drag
        onPositionChanged: {
            if (mousearea.pressed) TableInterface.slotMouseClicked(Math.floor(xLoc.x/4), (127 - Math.floor(yLoc.y/4)))
        }

        // send x/y when we initially click
        onPressed: TableInterface.slotMouseClicked(Math.floor(xLoc.x/4), (127 - Math.floor(yLoc.y/4)))

        // send the table when we release
        onReleased: TableInterface.slotSendTablesToBoard();

    }

    //-------------------------------------- Table Frame
    Rectangle
    {
        id: tableFrame
        //objectName: "tableFrame"
        width:  512
        height: 514
        x: 14
        y: 14
        color: Qt.rgba(0.3, 0.3, 0.3)
//        color: Qt.rgba(0.392156862745098,0.392156862745098,0.392156862745098,1.)
//        gradient: Gradient {
//            GradientStop { position: 0.0; color: Qt.rgba(0.6,0.6,0.6) }
//            //GradientStop { position: 0.33; color: "gray" }
//            GradientStop { position: 1.0; color: Qt.rgba(0.25,0.25,0.25) }
//        }

        property int unitSize: 4
        //border.color: "black"

        //-------------- Compensation for table frame size weirdness
        Rectangle
        {
            width: 512
            height: 2
            color: Qt.rgba(0.2,0.2,0.2,1);
        }

        function setCol (col, val)
        {
            repeater.colVal[col] = val;
        }

        //-------- Creates 128 Steps
        Repeater
        {
            id: repeater
            //objectName: "repeater"
            model:  128

            //--------- Create Columns
            Rectangle
            {
                id:     column
                //objectName: "column"
                width:  tableFrame.unitSize
                height: tableFrame.height
                x:      index*tableFrame.unitSize
                y:      0
                color:  Qt.rgba(0,0,0,0)
                property int colVal:0
                property color blockColor: Qt.rgba(0,0,0,0.5)

                //--------- Individual blocks, which slide within columns
                Rectangle
                {
                    id:     block
                    //objectName: "block"
                    /*gradient: Gradient {
                        GradientStop { position: 0.0; color: Qt.rgba(0,1,0) }
                        GradientStop { position: 1.0; color: Qt.rgba(0,0.5,0) }
                    }*/
                    width:  tableFrame.unitSize
                    height: tableFrame.unitSize
                    border.width: 0
                    border.color: blockColor
                    x:      0
                    color: parent.blockColor

                    y: (127 - parent.colVal) * tableFrame.unitSize

                    //--------------- Set Y Values of block in column
//                    y:
//                    {
//                        if(Math.floor(xLoc.x/4) === index && mousearea.pressed && mainwindow.editable)
//                        {
//                            TableInterface.slotEditUserTable(index, Math.floor(yLoc.y/4))
//                            return yLoc.y
//                        }
//                        else
//                        {
//                            return y
//                        }
//                    }

                    //onPosChanged: console.log("block moved");
                }
            }
        }

        //---------------------------- Coordinate Lines
        Rectangle
        {
            id: yLoc
            //objectName: "yLoc"
            x: 0
            y:
            {
                if(Math.floor(mousearea.mouseY/tableFrame.unitSize)*tableFrame.unitSize < tableFrame.height + tableFrame.y && Math.floor(mousearea.mouseY/tableFrame.unitSize)*tableFrame.unitSize >= tableFrame.y)
                {
                    return Math.floor(mousearea.mouseY/tableFrame.unitSize)*tableFrame.unitSize - tableFrame.y
                }
                else
                {
                    return y
                }
            }

            width: tableFrame.width
            height: tableFrame.unitSize
            color:
            {
                var opacity = mainwindow.editable * 0.2;
                Qt.rgba(.8, .8, .8, opacity)
            }
            //border.width: 1
            //border.color: Qt.rgba(1,1,1,0.25);
        }

        Rectangle
        {
            id: xLoc
            //objectName: "xLoc"
            x:
            {
                if(Math.floor(mousearea.mouseX/tableFrame.unitSize)*tableFrame.unitSize < tableFrame.width + tableFrame.x && Math.floor(mousearea.mouseX/tableFrame.unitSize)*tableFrame.unitSize >= tableFrame.x)
                {
                    return Math.floor(mousearea.mouseX/tableFrame.unitSize)*tableFrame.unitSize - tableFrame.x - tableFrame.unitSize/2;
                }
                else
                {
                    return x
                }
            }
            y: 2
            width: tableFrame.unitSize
            height: tableFrame.height - 2
            color:
            {
                var opacity = mainwindow.editable * 0.2;
                Qt.rgba(.8, .8, .8, opacity)
            }
            //border.width: 1
            //border.color: Qt.rgba(1,1,1,0.25);
        }

        //---------------------------------- Text Box
        Rectangle
        {
            id: coordinateBox
            //objectName: "coordinateBox"
            width: 50
            height: 15
            x:
            {
                if((mousearea.mouseX/tableFrame.unitSize)*tableFrame.unitSize < tableFrame.width/2)
                {
                    return Math.floor(mousearea.mouseX/tableFrame.unitSize)*tableFrame.unitSize + tableFrame.unitSize*2
                }
                else
                {
                    return Math.floor(mousearea.mouseX/tableFrame.unitSize)*tableFrame.unitSize - width - tableFrame.x - tableFrame.unitSize*2
                }
            }

            y:
            {
                if((mousearea.mouseY/tableFrame.unitSize)*tableFrame.unitSize < tableFrame.height/2)
                {
                    return Math.floor(mousearea.mouseY/tableFrame.unitSize)*tableFrame.unitSize + tableFrame.unitSize*2
                }
                else
                {
                    return Math.floor(mousearea.mouseY/tableFrame.unitSize)*tableFrame.unitSize - height - tableFrame.unitSize - tableFrame.y
                }
            }
            color:
            {
                var opacity = mainwindow.editable * 0.5;

                if(xLoc.x <= tableFrame.width && xLoc.x >= 0 && yLoc.y <= tableFrame.height && yLoc.y >= 0)
                {
                    return Qt.rgba(1, 1, 1, opacity)
                }
                else
                {
                    return Qt.rgba(1, 0, 0, opacity)
                }
            }


            Text
            {
                id: coordinateText
                //objectName: "coordinateText"
                x:2
                y:2
                text: Math.floor(xLoc.x/4) + ", " + (127 - Math.floor(yLoc.y/4))
                font.pointSize: 12
                color:
                {
                    var opacity = mainwindow.editable * 0.5;
                    Qt.rgba(0, 0, 0, opacity)
                }
            }
        }
    }

    Connections
    {
        target: TableInterface
        function onSignalUpdateCol(col, val)
        {
          console.log("draw column: " + col + " value: " + val)
          repeater.itemAt(col).colVal = val
        }

        //target: TableInterface
        function onSignalSetTableColorWriteEnabled(flag)
        {
            mainwindow.editable = flag;
            var thisColor = (flag) ? Qt.rgba(0, 1, 0, .5) : Qt.rgba(1, 0, 0, .5)
            for (var i = 0; i < 128; i++)
            {
                repeater.itemAt(i).blockColor = thisColor
            }
        }
    }

}
