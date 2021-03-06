#ifndef __MPU401CTLCONST_H_
#define __MPU401CTLCONST_H_

/////////////////////////////////////////////////////////////////////////////
// I/O
/////////////////////////////////////////////////////////////////////////////

#define MPU_STATUS_DSR   (1 << 7)
#define MPU_STATUS_DRR   (1 << 6)
#define MPU_STATUS_PAD   (0xff & (~(MPU_STATUS_DRR | MPU_STATUS_DSR)))

#define MK_MPU_STATUS(dsr, drr)\
  (((dsr) ? 0 : MPU_STATUS_DSR) | ((drr) ? 0 : MPU_STATUS_DRR) | MPU_STATUS_PAD)


/////////////////////////////////////////////////////////////////////////////
// Commands
/////////////////////////////////////////////////////////////////////////////

/** Copyright notice for MPU-401 intelligent-mode command constants *********

  MPU-401 MIDI Interface Module v1.0
  Copyright (c) 1991, Robin Davies. All Rights Reserved.

    Robin Davies
    224 3rd Avenue
    Ottawa, Ontario
    Canada. K1S 2K3.

  updated by:

  Larry Troxler, Compuserve 73520,1736

****************************************************************************/

// Start/Stop Commands

#define CMD_MIDI_STOP                       0x01
#define CMD_MIDI_START                      0x02
#define CMD_MIDI_CONTINUE                   0x03

#define CMD_PLAY_STOP                       0x04
#define CMD_PLAY_START                      0x08
#define CMD_PLAY_CONTINUE                   0x0c

#define CMD_RECORD_STOP                     0x10
#define CMD_RECORD_START                    0x20

// Commands

#define CMD_DISABLE_ALL_NOTES_OFF           0x30
#define CMD_DISABLE_REAL_TIME_OUT           0x32
#define CMD_TIMING_BYTE_ALWAYS              0x34
#define CMD_MODE_MESS_ON                    0x35
#define CMD_EXCLUSIVE_THRU_ON               0x37
#define CMD_COMMON_TO_HOST_ON               0x38
#define CMD_REAL_TIME_TO_HOST_ON            0x39
#define CMD_UART_MODE                       0x3f

#define CMD_INT_CLOCK                       0x80
#define CMD_FSK_CLOCK                       0x81
#define CMD_MIDI_CLOCK                      0x82
#define CMD_METRONOME_ON                    0x83
#define CMD_METRONOME_OFF                   0x84
#define CMD_METRONOME_W_ACCENTS             0x85
#define CMD_BENDER_OFF                      0x86
#define CMD_BENDER_ON                       0x87
#define CMD_MIDI_THRU_OFF                   0x88
#define CMD_MIDI_THRU_ON                    0x89
#define CMD_DATA_IN_STOP_MODE_OFF           0x8a
#define CMD_DATA_IN_STOP_MODE_ON            0x8b
#define CMD_SEND_MEASURE_END_OFF            0x8c
#define CMD_SEND_MEASURE_END_ON             0x8d
#define CMD_CONDUCTOR_OFF                   0x8e
#define CMD_CONDUCTOR_ON                    0x8f
#define CMD_REAL_TIME_AFFECTION_OFF         0x90
#define CMD_REAL_TIME_AFFECTION_ON          0x91
#define CMD_FSK_TO_INTERNAL                 0x92
#define CMD_FSK_TO_MIDI                     0x93
#define CMD_CLOCK_TO_HOST_OFF               0x94
#define CMD_CLOCK_TO_HOST_ON                0x95
#define CMD_EXCLUSIVE_TO_HOST_OFF           0x96
#define CMD_EXCLUSIVE_TO_HOST_ON            0x97

#define CMD_RESET_RELATIVE_TEMPO            0xb1
#define CMD_CLEAR_PLAY_COUNTERS             0xb8
#define CMD_CLEAR_PLAY_MAP                  0xb9
#define CMD_CLEAR_RECORD_COUNTER            0xba
#define CMD_TIMEBASE_48                     0xc2
#define CMD_TIMEBASE_72                     0xc3
#define CMD_TIMEBASE_96                     0xc4
#define CMD_TIMEBASE_120                    0xc5
#define CMD_TIMEBASE_144                    0xc6
#define CMD_TIMEBASE_168                    0xc7
#define CMD_TIMEBASE_192                    0xc8

#define CMD_REQUEST_TO_SEND_DATA            0xd0 /* + track #! */
#define CMD_REQUEST_TO_SEND_SYSTEM_MSG      0xdf

#define CMD_SET_TEMPO                       0xe0
#define CMD_RELATIVE_TEMPO                  0xe1
#define CMD_RELATIVE_TEMPO_GRADUATION       0xe2
#define CMD_MIDI_METRONOME                  0xe4
#define CMD_MEASURE_LENGTH                  0xe6
#define CMD_INTERNAL_CLOCK_LENGTH_TO_HOST        /* ? */
#define CMD_ACTIVE_TRACK_MASK               0xec
#define CMD_SEND_PLAY_COUNTER_MASK          0xed
#define CMD_MIDI_CHANNEL_MASK_LO            0xee
#define CMD_MIDI_CHANNEL_MASK_HI            0xef

#define CMD_EOX                             0xf7
#define CMD_TIMING_OVERFLOW                 0xf8
#define CMD_MPU_MARK                        0xfc
#define CMD_RESET                           0xff

// Commands that return data

#define CMD_REQUEST_PLAY_COUNTER            0xa0
#define CMD_REQUEST_AND_CLEAR_PLAY_COUNTER  0xab
#define CMD_REQUEST_VERSION                 0xac
#define CMD_REQUEST_REVISION                0xad
#define CMD_REQUEST_TEMPO                   0xaf


/////////////////////////////////////////////////////////////////////////////
// Messages
/////////////////////////////////////////////////////////////////////////////

#define MSG_CMD_ACK                         0xfe

#endif //__MPU401CTLCONST_H_
