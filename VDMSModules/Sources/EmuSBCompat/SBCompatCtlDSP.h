#ifndef __SBCOMPATCTLDSP_H_
#define __SBCOMPATCTLDSP_H_

class CSBCompatCtlMixer;

//
// This interface abstracts some of the hardware details in the DSP's
//  world
//
class ISBDSPHWEmulationLayer {
  public:
    enum transfer_t { TT_PLAYBACK, TT_RECORD };
    enum codec_t { CODEC_PCM, CODEC_PCM_SIGNED, CODEC_ADPCM_2, CODEC_ADPCM_3, CODEC_ADPCM_4 };

  public:
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

    virtual void generateInterrupt(void) = 0;

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
      DSP_S_HIGHSPEED
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

  protected:
    bool processCommand(unsigned char command);
    const char* getCopyright(void);

  protected:
    inline void setNumSamples(int numSamples)
      { m_numSamples = numSamples; }
    inline void setSampleRate(int sampleRate)
      { m_useTimeConstant = false; m_sampleRate = sampleRate; }
    inline void setTimeConstant(int timeConstant)
      { m_useTimeConstant = true; m_timeConstant = timeConstant; }
    inline void setNumChannels(int numChannels)
      { m_numChannels = numChannels; }

    inline int getNumSamples(void)
      { return m_numSamples; }
    inline int getSampleRate(void)
      { return m_useTimeConstant ? (1000000 / ((256 - m_timeConstant) * getNumChannels())) : m_sampleRate; }
    inline int getNumChannels(void)
      { return m_numChannels; }

  protected:
    std::queue<unsigned char> m_bufOut;
    std::vector<unsigned char> m_bufIn;
    DSPState_t m_state;
    unsigned char m_lastCommand;

    bool m_isRstAsserted, m_isSpeakerEna;
    bool m_useTimeConstant;
    int m_sampleRate, m_timeConstant, m_numChannels;
    int m_numSamples;

    bool m_is8BitIRQPending, m_is16BitIRQPending;

    unsigned char m_testRegister;

  protected:
    ISBDSPHWEmulationLayer* m_hwemu;
    CSBCompatCtlMixer* m_sbmix;
};

#endif //__SBCOMPATCTLDSP_H_
