#ifndef __SBCOMPATCTLDSP_H_
#define __SBCOMPATCTLDSP_H_

class CSBCompatCtlMixer;

//
// This interface abstracts some of the hardware details in the DSP's
//  world
//
class ISBDSPHWEmulationLayer {
  public:
    enum transfer_t { TT_PLAYBACK, TT_RECORD, TT_E2CMD };
    enum codec_t { CODEC_PCM, CODEC_PCM_SIGNED, CODEC_ADPCM_2, CODEC_ADPCM_3, CODEC_ADPCM_4 };

  public:
    virtual short getDSPVersion(void) = 0;
    virtual void startTransfer(
        transfer_t type,        // DSP command 0xe2 (input)
        char E2Reply,           // reply byte to be written back using DMA
        bool isSynchronous = false) = 0;  // function only returns after DREQ has been asserted and byte transferred
    virtual void startTransfer(
        transfer_t type,        // playback (output) or recording (input)
        int numChannels,        // 1 = mono, 2 = stereo
        int samplesPerSecond,   // frequency (samples/s)
        int bitsPerSample,      // 8 or 16 (PCM), 2, 3 or 4 (ADPCM)
        int samplesPerBlock,    // length of SB "block" (in samples); interrupt is generated after every block
        codec_t codec,          // PCM, ADPCM, etc.
        bool isAutoInit,        // type of DMA transfer
        bool isSynchronous = false) = 0;  // function only returns after DREQ has been asserted
    virtual void stopTransfer(transfer_t type,
        bool isSynchronous = false) = 0;  // function only returns after DREQ has been deasserted
    virtual void pauseTransfer(transfer_t type) = 0;
    virtual void resumeTransfer(transfer_t type) = 0;

    virtual void generateInterrupt(int count = 1) = 0;

    virtual void logError(const char* message) = 0;
    virtual void logWarning(const char* message) = 0;
    virtual void logInformation(const char* message) = 0;
};

//
// This class implements the finite state machine logic behind the MIXER
//
class CSBCompatCtlDSP {
  protected:
    enum DSPState_t {
      DSP_S_NORMAL,
      DSP_S_HIGHSPEED,
      DSP_S_RESET
    };

  public:
    CSBCompatCtlDSP(ISBDSPHWEmulationLayer* hwemu, CSBCompatCtlMixer* sbmix);
    ~CSBCompatCtlDSP(void);

  public:
    void reset(void);
    void reset(char data);
    void putCommand(char data);
    char getData(void);
    char getWrStatus(void);
    char getRdStatus(void);
    void ack8BitIRQ(void);
    void ack16BitIRQ(void);
    void set8BitIRQ(void);
    void set16BitIRQ(void);
    bool get8BitIRQ(void);
    bool get16BitIRQ(void);

    void resetADPCM(void);

    int decode_PCM(
        unsigned char* buf,
        int bufSize,
        int bitsPerSample);
    int decode_PCM_SIGNED(
        unsigned char* buf,
        int bufSize,
        int bitsPerSample);
    int decode_ADPCM_2(
        unsigned char* buf,
        int bufSize,
        int maxSize);
    int decode_ADPCM_4(
        unsigned char* buf,
        int bufSize,
        int maxSize);

  protected:
    void stopAllDMA(bool isSynchronous);
    void ackAllIRQs(void);
    bool processCommand(unsigned char command);
    const char* getCopyright(void);
    int getNumChannels(void);

  protected:
    inline void setNumSampleBytes(int numSampleBytes)
      { m_numSampleBytes = numSampleBytes; }
    inline void setSampleRate(int sampleRate)
      { m_useTimeConstant = false; m_sampleRate = sampleRate; }
    inline void setTimeConstant(int timeConstant)
      { m_useTimeConstant = true; m_timeConstant = timeConstant; }

    inline int getNumSampleBytes(void)
      { return m_numSampleBytes; }
    inline int getSampleRate(void)
      { return m_useTimeConstant ? (1000000 / ((256 - m_timeConstant) * getNumChannels())) : m_sampleRate; }

  protected:
    inline void flushInputBuffer(void)
      { m_bufIn.clear(); }
    inline void flushOutputBuffer(void)
      { while (!m_bufOut.empty()) m_bufOut.pop(); }

  protected:
    std::vector<unsigned char> m_bufIn;   // DSP command queue (incoming)
    std::queue<unsigned char> m_bufOut;   // DSP reply queue (outgoing)
    DSPState_t m_state;
    unsigned char m_lastCommand;          // last successful DSP command

    char m_E2Value;                       // control values used bye the undocumented
    int m_E2Count;                        //  0xE2 challenge/response DSP command

    bool m_isSpeakerEna;                  // is the output muted or not (no effect on output on SB16)
    bool m_useTimeConstant;               // sample rate is defined using the alternate "time constant"/period method (as opposed to frequency)
    int m_sampleRate, m_timeConstant;     // sample rate defined either as frequency or period (time constant)
    int m_numSampleBytes;                 // number of bytes to be transferred via DMA playback/recording

    bool m_is8BitIRQPending, m_is16BitIRQPending;   // an IRQ is pending and needs to be acknowledged by the DOS application before the DMA transfer resumes

    unsigned char m_testRegister;         // storage used by the 0xE4/0xE8 DSP authentication commands

  protected:
    int m_ADPCMReference;                 // reference audio-sample value used in ADPCM decompression
    int m_ADPCMScale;                     // exponential-scaling factor used in ADPCM decompression

  protected:
    ISBDSPHWEmulationLayer* m_hwemu;
    CSBCompatCtlMixer* m_sbmix;
};

#endif //__SBCOMPATCTLDSP_H_
