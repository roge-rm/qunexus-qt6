// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#ifndef SYSEXENCDECODE_H
#define SYSEXENCDECODE_H

#include <QVariant>
#include <QtWidgets> // was QT Gui
#include "Qn_preset_struct.h"
#include "KMI_mdm.h"
#include "presetmanager.h"

//********************************************
//*****	Macros
//********************************************

// these macros help us combine and retrieve 2, 4, and 6 bit numbers to/from 8 bits
// This is critical to cram more info into bulging preset memory pages

#define LEFT2BITS(x)        (x >> 6)
#define LEFT4BITS(x)        (x >> 4)
#define LEFT6BITS(x)        (x >> 2)

#define SETLEFT2(x, y)      (x = (y << 6) + RIGHT6BITS(x))
#define SETLEFT4(x, y)      (x = (y << 4) + RIGHT4BITS(x))

#define RIGHT2BITS(x)       (x & 3)
#define RIGHT4BITS(x)       (x & 15)
#define RIGHT6BITS(x)       (x & 63)
#define SETRIGHT4(x, y)     (x = (x & 240) + RIGHT4BITS(y))
#define SETRIGHT6(x, y)     (x = (x & 192) + RIGHT6BITS(y))

#define COMBINE26(x, y)     ((x << 6) + y)
#define COMBINE62(x, y)     ((x << 2) + y)
#define COMBINE44(x, y)     ((x << 4) + y)

#define GET2BITS(x, y)      ( ((3 << (y*2)) & x) >> (y*2) )
#define PUT2BITS(x, y, z)   ( x = x | (y << (z*2)) )

//**********************************************

class SysExEncDecode : public QWidget
{
    Q_OBJECT

public:
    explicit SysExEncDecode(QWidget *parent = 0);

    QWidget *thisParent;

    // this byte array holds the 8bit-7bit encoded sysex payload
    QByteArray encodedSysexByteArray;

    // this is no longer used and should be safely removed
    struct QUNEXUS qn;
    SEQUENCER_PRESET qnSeq; // used to encode/prep sequencer parameters (which use mixed bit space) for SysEx transmit
                            // matches qnSeq in firmware

    // class pointers
    MidiDeviceManager*  QuNexus_MDM;
    PresetManager*      presetManager;

    QVariantMap*    editBufferPresetMap;
    QVariantMap     paramTypeMap;

    ///////////////////////////////////////
    ///////////_midi_sx_encode ////////////
    ///////////////////////////////////////
    //------ _midi_sx_encode vars
    unsigned int size;
    unsigned char midi_hi_bits;
    unsigned char midi_hi_count;
    unsigned int crc;
    unsigned char sx_xmit_active;

    unsigned int SX_PACKET_START;
    unsigned int SX_ENCODE_LEN;

    QMap<QString, unsigned char> sx_ident;

    unsigned char sysex_start;
    unsigned char sysex_end;

    //------ _midi_sx_encode funcs
    void d2h(QVariant d);
    void sysex_out(unsigned char ch);
    void midi_buffer_put_core(unsigned char ch);
    void midi_chunk_init();
    void midi_sx_encode_char(unsigned char ch);
    void midi_sx_encode_int(int i);
    void midi_sx_encode_crc_char(unsigned char ch);
    void midi_sx_encode_crc_int(int i);

    void midi_sx_flush();
    void send_sx_stop();

    void crc_byte(unsigned char ch);
    void crc_init();
    void lineInit();

    // 2.0 funcs
    void encodeSequencerPreset(QVariantMap preset); // encodes preset into the qnSeq struct


    ///////////////////////////////////////
    /////////////_structList //////////////
    ///////////////////////////////////////
    //------ _strucList vars
    QStringList keyLayerC;
    QStringList keyLayerJSON;
    QMap<QString, unsigned char> dictionary; //holds key,value pairs to convert strings to specified ints


    ///////////////////////////////////////
    //////// qn enum translators //////////
    ///////////////////////////////////////

    QStringList Curves;
    //QStringList CV_Out_Source;
    QStringList CV_Out_Type;
    QStringList CV_Scaling;
    QStringList KB_Source;
    QStringList LED_Refresh;
    QStringList Key_Priority;
    QStringList Preset_Struct_is_V2;
    QStringList MIDI_Out;
    QStringList CV_MIDI_Out;
    QStringList MIDI_CH;
    QStringList Toggle_Modes;
    QStringList SEQ_clock_source;
    QStringList SEQ_track_mode;
    QStringList SEQ_track_polyphony;
    QStringList SEQ_track_arp_pattern;
    QStringList SEQ_track_seq_division;
    QStringList SEQ_track_division;
    QStringList SEQ_track_dot;

    // range testing
    bool    outOfRangeFlag = 0;
    QString OORParams;

    ///////////////////////////////////////
    /////////////QuNexusSysex//////////////
    ///////////////////////////////////////
    //----- QuNexusSysEx Vars
    unsigned int PRESET_LENGTH;  //<<------------------------------  DON'T FORGET TO CHANGE THIS WHEN PRESETS GET CHANGED
    unsigned char PRESET_START;
    unsigned int PRESET_LENGTH_MSB;
    unsigned int PRESET_LENGTH_LSB;
    unsigned int sumByte;

    //---- QuNexusSysEx Funcs
    void encode(unsigned char ch);
    void encodeInt(int i);

    //---- Globlas
    QByteArray globals;

signals:
    // encode
    void signalEncodedPreset(QByteArray);
    void signalEncodedGlobal(QByteArray bytes);

    // decode
    void signalSendGlobals(QString, QByteArray);
    void signalPresetProcessed(unsigned char);

    // store/restore globals
    void signalGlobalsReceived();
    void signalGlobalsReceivedDoFwUd();
    void signalRestoreGlobals();

public slots:

    // encode
    void slotSysExEncodePreset(QVariantMap preset, unsigned char presetNumChar);
    void slotSysExEncodeGlobal(QString global, QList<int> vals);

    // decode sysex
    void slotProcessSysEx(QByteArray sysExMessageByteArray);

    // decode globals
    void slotProcessGlobals(QByteArray sysExMessageByteArray);
    void slotDecodeGlobals(QByteArray globalsByteArray);
    bool slotIsGlobals(QByteArray ba);
    void slotRestoreGlobals(QByteArray globalsByteArray);

    // presets
    bool slotIsPreset(QByteArray ba);
    void slotProcessPreset(QByteArray presetPacket);

    bool slotTestQListRange(QStringList *thisList, int thisIndex, int thisJSONCounter); // tests QStringLists for out of range errors
};



#endif // SYSEXENCODE_H
