import QtQuick 1.0


Item    {
    id: _rangeSliderRangeView
    width: 573
    height: 250
    smooth: true

    //**********************************//
    // background color of entire object
    //**********************************//
    Rectangle {
        id: _doubleSliderBackgrond
        width: parent.width
        height: parent.height
        color: Qt.rgba(200/255, 200/255, 200/255, 1)

        //**********************************//
        //background color of slider area
        //**********************************//
        Rectangle {
            id: _doubleSliderArea
            radius: 4
            smooth: true
            y: 5; x:5;
            width: _rangeSliderRangeView.width - 5
            height: _rangeSliderRangeView.height - 5

            gradient: Gradient {
                    GradientStop { position: 0.0; color: Qt.rgba(20/255, 20/255, 20/255, 1) }
                     GradientStop { position: 1.0; color: Qt.rgba(67/255, 67/255, 67/255, 1) }
                 }
        }

        //**********************************//
        //low end range button
        //**********************************//
        Rectangle {
           id: _rangeSliderLowButton
           width:  10
           height: 10
           y: _doubleSliderArea.height - 60
           x: 5
           smooth: true
           radius: 2
           transform: Rotation { origin.x: 5; origin.y: 5; angle: 45}
           border.color: "black"
           border.width: 1
           property variant loSnapPoints: [_blackKeyMarker0.x, _blackKeyMarker1.x, _blackKeyMarker2.x, _blackKeyMarker3.x,
               _blackKeyMarker4.x,_blackKeyMarker5.x,_blackKeyMarker6.x,_blackKeyMarker7.x,_blackKeyMarker8.x,_blackKeyMarker9.x,
               _whiteKeyMarker0.x, _whiteKeyMarker1.x, _whiteKeyMarker2.x,_whiteKeyMarker3.x,
               _whiteKeyMarker4.x,_whiteKeyMarker5.x,_whiteKeyMarker6.x,_whiteKeyMarker7.x,
               _whiteKeyMarker8.x,_whiteKeyMarker9.x,_whiteKeyMarker10.x,_whiteKeyMarker11.x,
               _whiteKeyMarker12.x,_whiteKeyMarker13.x,_whiteKeyMarker14.x,_whiteKeyMarker15.x]

           gradient: Gradient {
                   GradientStop { position: 0.0; color: "white" }
                    GradientStop { position: 1.0; color: "gray" }
                }

           MouseArea {
                        anchors.fill: parent
                        drag.target: _rangeSliderLowButton
                        drag.axis: Drag.XAxis
                        drag.minimumX: _doubleSliderArea.x
                        drag.maximumX: _rangeSliderHighButton.x - 10

                        onPositionChanged: {
                            if(drag.active){
                                //console.log(_rangeSliderLowButton.x)
                            }
                        }

                        onReleased: {


                            console.log("X", _rangeSliderLowButton.x)

                            if(_rangeSliderLowButton.x < _rangeSliderLowButton.loSnapPoints[24]){

                                var min = _rangeSliderRangeView.width;
                                var distance = 0;
                                var snapTo = _rangeSliderLowButton.loSnapPoints[1]

                                for(var i=0; i<_rangeSliderLowButton.loSnapPoints.length; i++){
                                    distance = _rangeSliderLowButton.x - 7 - _rangeSliderLowButton.loSnapPoints[i];



                                    if(distance < 0){
                                        distance *= -1;
                                    }

                                    console.log(i, "Distance :", distance)

                                    if(distance < min){
                                        min = distance;
                                        snapTo = _rangeSliderLowButton.loSnapPoints[i]
                                    }

                                    //console.log(i, ":", _rangeSliderHighButton.hiSnapPoints[i])

                                }


                               // console.log("DIST", distance)
                                //console.log("MIN", min)

                                _rangeSliderLowButton.x = snapTo + 6;


                            }


                        }
                    }
        }


        //**********************************//
        //high end range button
        //**********************************//
        Rectangle {
           id: _rangeSliderHighButton
           width:  10
           height: 10
           y: _doubleSliderArea.height - 60
           x: parent.width - width - 5;
           color: "white"
           smooth: true
           radius: 2
           transform: Rotation { origin.x: 5; origin.y: 5; angle: 45}
           border.color: "black"
           border.width: 1
           property variant hiSnapPoints: [_blackKeyMarker0.endPoint, _blackKeyMarker1.endPoint, _blackKeyMarker2.endPoint, _blackKeyMarker3.endPoint,
               _blackKeyMarker4.endPoint,_blackKeyMarker5.endPoint,_blackKeyMarker6.endPoint,_blackKeyMarker7.endPoint,_blackKeyMarker8.endPoint,_blackKeyMarker9.endPoint,
               _whiteKeyMarker0.x, _whiteKeyMarker1.x, _whiteKeyMarker2.x,_whiteKeyMarker3.x,
               _whiteKeyMarker4.x,_whiteKeyMarker5.x,_whiteKeyMarker6.x,_whiteKeyMarker7.x,
               _whiteKeyMarker8.x,_whiteKeyMarker9.x,_whiteKeyMarker10.x,_whiteKeyMarker11.x,
               _whiteKeyMarker12.x,_whiteKeyMarker13.x,_whiteKeyMarker14.x,_whiteKeyMarker15.x]


           gradient: Gradient {
                   GradientStop { position: 0.0; color: "white" }
                    GradientStop { position: 1.0; color: "gray" }
                }

           MouseArea {
                        anchors.fill: parent
                        drag.target: _rangeSliderHighButton
                        drag.axis: Drag.XAxis
                        drag.minimumX: _rangeSliderLowButton.x + 10
                        drag.maximumX: _doubleSliderArea.width - 5

                        onReleased: {


                            console.log("X", _rangeSliderHighButton.x)

                            if(_rangeSliderHighButton.x > _rangeSliderHighButton.hiSnapPoints[10]){

                                var min = _rangeSliderRangeView.width;
                                var distance = 0;
                                var snapTo = _rangeSliderHighButton.hiSnapPoints[1]

                                for(var i=0; i<_rangeSliderHighButton.hiSnapPoints.length; i++){
                                    distance = _rangeSliderHighButton.x - 7 - _rangeSliderHighButton.hiSnapPoints[i];



                                    if(distance < 0){
                                        distance *= -1;
                                    }

                                    console.log(i, "Distance :", distance)

                                    if(distance < min){
                                        min = distance;
                                        snapTo = _rangeSliderHighButton.hiSnapPoints[i]
                                    }

                                    //console.log(i, ":", _rangeSliderHighButton.hiSnapPoints[i])

                                }


                               // console.log("DIST", distance)
                                //console.log("MIN", min)

                                _rangeSliderHighButton.x = snapTo + 7;


                            }


                        }
                    }
        }

        //**********************************//
        //KEYBOARD
        //**********************************//
        Rectangle {
            id: _keyBoard
            height: 150;
            width: _doubleSliderArea.width - 13;
            x: _doubleSliderArea.x + 6;
            y: _doubleSliderArea.y + 10;

                Rectangle {
                    id: _whiteKey0
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 0
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey1
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 1
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey2
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 2
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey3
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 3
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey4
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 4
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey5
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 5
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey6
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 6
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey7
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 7
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey8
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 8
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey9
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 9
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey10
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 10
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey11
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 11
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey12
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 12
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey13
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 13
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _whiteKey14
                    height: _keyBoard.height
                    width: _keyBoard.width/15;
                    color: "white"
                    border.width: 1
                    border.color: "black"

                    property int offset: 14
                    x:width*offset; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey0
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 0
                    x:width*offset + width*1.5; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey1
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 2
                    x:width*offset + width*1.5; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey2
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 6
                    x:width*offset + width*1.5; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey3
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 8
                    x:width*offset + width*1.5; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey4
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 10
                    x:width*offset + width*1.5; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey5
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 14
                    x:width*offset + width*1.5; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey6
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 16
                    x:width*offset + width*1.5; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey7
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 20
                    x:width*offset + width*1.5; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey8
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 22
                    x:width*offset + width*1.5; //- border.width*offset;
                }

                Rectangle {
                    id: _blackKey9
                    height: _keyBoard.height*2/3
                    width: _keyBoard.width/30;
                    color: "black"
                    border.width: 1
                    border.color: "black"

                    property int offset: 24
                    x:width*offset + width*1.5; //- border.width*offset;
                }





        }

        //**********************************//
        //HIGHLIGHT
        //**********************************//
        Rectangle {
            id: _doubleSliderFiller
            smooth: true
            y: _keyBoard.y - 4; x: _rangeSliderLowButton.x + 3;
            width: (_rangeSliderHighButton.x + 2) - (_rangeSliderLowButton.x);
            height: _keyBoard.height + 9;
            color: Qt.rgba(200/255, 200/255, 200/255, 1)
            radius: 4


            gradient: Gradient {
                    GradientStop { position: 0.0; color: Qt.rgba(200/255, 200/255, 200/255, 0.5) }
                     GradientStop { position: 1.0; color: Qt.rgba(67/255, 67/255, 67/255, 0.5) }
                 }

            border.color: "lightgreen"
            border.width: 2


            onWidthChanged: {
                if(width < _blackKey0.width){
                //console.log(width)
                    border.color = "red"
                } else {
                    border.color = "lightgreen"
                }
            }


        }

        //**********************************//
        //SNAPGRID
        //**********************************//
        Rectangle {
            id: _snapGrid
            width: _keyBoard.width
            height: 20
            y:_doubleSliderArea.height - 70;
            x: _keyBoard.x
            color: Qt.rgba(0,0,0,0);
            smooth: true
            property int blackMarkerHeight: 3
            property int whiteMarkerHeight: 5



            Rectangle {
                id: _blackKeyMarker0
                smooth: true
                width: _blackKey0.width - 3
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey0.x;
                border.color: "black"
                property variant endPoint: width + x


                //radius: 3
            }

            Rectangle {
                id: _blackKeyMarker1
                smooth: true
                width: _blackKey0.width - 2
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey1.x;
                border.color: "black"
                property variant endPoint: width + x


                //radius: 3
            }

            Rectangle {
                id: _blackKeyMarker2
                smooth: true
                width: _blackKey0.width - 2
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey2.x;
                border.color: "black"
                property variant endPoint: width + x


                //radius: 3
            }

            Rectangle {
                id: _blackKeyMarker3
                smooth: true
                width: _blackKey0.width - 3
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey3.x;
                border.color: "black"
                property variant endPoint: width + x

                //radius: 3
            }

            Rectangle {
                id: _blackKeyMarker4
                smooth: true
                width: _blackKey0.width - 2
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey4.x;
                border.color: "black"
                property variant endPoint: width + x

                //radius: 3
            }

            Rectangle {
                id: _blackKeyMarker5
                smooth: true
                width: _blackKey0.width - 2
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey5.x;
                border.color: "black"
                property variant endPoint: width + x

                //radius: 3
            }

            Rectangle {
                id: _blackKeyMarker6
                smooth: true
                width: _blackKey0.width - 3
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey6.x;
                border.color: "black"
                property variant endPoint: width + x

                //radius: 3
            }

            Rectangle {
                id: _blackKeyMarker7
                smooth: true
                width: _blackKey0.width - 2
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey7.x;
                border.color: "black"
                property variant endPoint: width + x

                //radius: 3
            }

            Rectangle {
                id: _blackKeyMarker8
                smooth: true
                width: _blackKey0.width - 2
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey8.x;
                border.color: "black"
                property variant endPoint: width + x

                //radius: 3
            }

            Rectangle {
                id: _blackKeyMarker9
                smooth: true
                width: _blackKey0.width - 3
                height: parent.blackMarkerHeight
                color:  Qt.rgba(40/255, 40/255, 40/255, 1)
                x: 3 + _blackKey9.x;
                border.color: "black"
                property variant endPoint: width + x

                //radius: 3
            }





            Rectangle {
                id: _whiteKeyMarker0
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker1
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker2
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*2;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker3
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*3;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker4
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*4;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker5
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*5;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker6
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*6;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker7
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*7;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker8
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*8;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker9
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*9;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker10
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*10;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker11
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*11;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker12
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*12;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }

            Rectangle {
                id: _whiteKeyMarker13
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*13;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }
            Rectangle {
                id: _whiteKeyMarker14
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: 1 + _whiteKey0.width*14;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3

            }
            Rectangle {
                id: _whiteKeyMarker15
                smooth: true
                width: 1
                height: parent.whiteMarkerHeight
                color:  "white"
                x: _whiteKey0.width*15 - 1;
                gradient: Gradient { GradientStop { position: 0.0; color: "white" }GradientStop { position: 1.0; color: "gray" }}
                //radius: 3
            }


        }




    }
}

