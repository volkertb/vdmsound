#ifndef __MIDICONST_H__
#define __MIDICONST_H__

#define IS_MIDI_EVENT(data)             (((data) & 0x80) == 0x80)
#define IS_SYSTEM_EVENT(data)           (((data) & 0xf0) == 0xf0)
#define IS_REALTIME_EVENT(data)         (((data) & 0xf8) == 0xf8)

#define GET_EVENT(data)                 ((data) & 0xf0)
#define GET_CHANNEL(data)               ((data) & 0x0f)


/////////////////////////////////////////////////////////////////////////////
// Classes of events
/////////////////////////////////////////////////////////////////////////////

#define MIDI_EVENT_NOTE_OFF             0x80
#define MIDI_EVENT_NOTE_ON              0x90
#define MIDI_EVENT_POLY_KEY_AFTERTOUCH  0xa0
#define MIDI_EVENT_CONTROL_CHANGE       0xb0
#define MIDI_EVENT_PROGRAM_CHANGE       0xc0
#define MIDI_EVENT_CHANNEL_AFTERTOUCH   0xd0
#define MIDI_EVENT_PITCH_BEND_CHANGE    0xe0
#define MIDI_EVENT_SYSTEM               0xf0


/////////////////////////////////////////////////////////////////////////////
// Specific events
/////////////////////////////////////////////////////////////////////////////

// System events
#define MIDI_EVENT_SYSTEM_SYSEX         0xf0
#define MIDI_EVENT_SYSTEM_RESERVED1     0xf1
#define MIDI_EVENT_SYSTEM_SONG_POS_PTR  0xf2
#define MIDI_EVENT_SYSTEM_SONG_SELECT   0xf3
#define MIDI_EVENT_SYSTEM_RESERVED2     0xf4
#define MIDI_EVENT_SYSTEM_RESERVED3     0xf5
#define MIDI_EVENT_SYSTEM_TUNE_REQ      0xf6
#define MIDI_EVENT_SYSTEM_EOX           0xf7

// Realtime events
#define MIDI_EVENT_SYS_RT_TIMING_CLK    0xf8
#define MIDI_EVENT_SYS_RT_RESERVED1     0xf9
#define MIDI_EVENT_SYS_RT_START         0xfa
#define MIDI_EVENT_SYS_RT_CONTINUE      0xfb
#define MIDI_EVENT_SYS_RT_STOP          0xfc
#define MIDI_EVENT_SYS_RT_RESERVED2     0xfd
#define MIDI_EVENT_SYS_RT_ACT_SENSING   0xfe
#define MIDI_EVENT_SYS_RT_RESET         0xff

// For MIDI files only
#define MIDI_EVENT_SYSTEM_METAEVENT     0xff

#define MIDI_META_SEQUENCE_NUMBER       0x00
#define MIDI_META_TEXT                  0x01
#define MIDI_META_COPYRIGHT             0x02
#define MIDI_META_SEQTRACK_NAME         0x03
#define MIDI_META_INSTRUMENT_NAME       0x04
#define MIDI_META_LYRIC                 0x05
#define MIDI_META_MARKER                0x06
#define MIDI_META_CUE_POINT             0x07
#define MIDI_META_CHANNEL               0x20
#define MIDI_META_END_OF_TRACK          0x2f
#define MIDI_META_TEMPO                 0x51
#define MIDI_META_SMPTE_OFFSET          0x54
#define MIDI_META_TIME_SIGNATURE        0x58
#define MIDI_META_KEY_SIGNATURE         0x59
#define MIDI_META_SEQUENCER             0x7f


/////////////////////////////////////////////////////////////////////////////
// Event lengths
/////////////////////////////////////////////////////////////////////////////

extern int MIDI_evt_len[256];

#endif // __MIDICONST_H__
