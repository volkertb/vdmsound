#ifndef __SBCOMPATCTLDSP_H_
#define __SBCOMPATCTLDSP_H_

//
// This interface abstracts some of the hardware details in the DSP's
//  world
//
class ISBDSPHWEmulationLayer {
  public:
    virtual void logError(const char* message) = 0;
    virtual void logWarning(const char* message) = 0;
};

//
// This class implements the finite state machine logic behind the MIXER
//
class CSBCompatCtlDSP {
  public:
    CSBCompatCtlDSP(ISBDSPHWEmulationLayer* hwemu);
    ~CSBCompatCtlDSP(void);

  public:
    void reset(void);
    void reset(char data);
    void putCommand(char data);
    char getData(void);
    char getWrStatus(void);
    char getRdStatus(void);
    void ack8bitIRQ(void);
    void ack16bitIRQ(void);

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
    inline void setNumBits(int numBits)
      { m_numBits = numBits; }

    inline int getNumSamples(void)
      { return m_numSamples; }
    inline int getSampleRate(void)
      { return m_useTimeConstant ? (1000000 / ((256 - m_timeConstant) * getNumChannels())) : m_sampleRate; }
    inline int getNumChannels(void)
      { return m_numChannels; }
    inline int getNumBits(void)
      { return m_numBits; }

  protected:
    std::queue<unsigned char> m_bufOut;
    std::vector<unsigned char> m_bufIn;
    unsigned char m_lastCommand;

    bool m_isSpeakerEna, m_isRstAsserted;
    bool m_useTimeConstant;
    int m_sampleRate, m_timeConstant, m_numChannels, m_numBits;
    int m_numSamples;

    unsigned char m_testRegister;

  protected:
    ISBDSPHWEmulationLayer* m_hwemu;
};

#endif //__SBCOMPATCTLDSP_H_
