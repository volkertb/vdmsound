#ifndef __SBCOMPATCTLMIXER_H_
#define __SBCOMPATCTLMIXER_H_

//
// This interface abstracts some of the hardware details in the mixer's
//  world, such as changing IRQ and DMA settings, altering device volume, etc.
//
class ISBMixerHWEmulationLayer {
  public:
    virtual void logError(const char* message) = 0;
    virtual void logWarning(const char* message) = 0;
    virtual void logInformation(const char* message) = 0;
};

//
// This class implements the finite state machine logic behind the MIXER
//
class CSBCompatCtlMixer {
  public:
    enum IRQSource_t {
      DSP_8BIT    = 0,
      DSP_SBMIDI  = 0,
      DSP_16BIT   = 1,
      MPU_401     = 2
    };

  public:
    CSBCompatCtlMixer(ISBMixerHWEmulationLayer* hwemu);
    ~CSBCompatCtlMixer(void);

  public:
    void reset(void);
    void setAddress(char data);
    void setValue(char value);
    char getValue(void);

    void setIRQSelect(int IRQ);
    void setDMASelect(int DMA8, int DMA16);
    void setIRQStatus(IRQSource_t source, bool isPending);

  protected:
    void reset(char data);

    void setMasterVol(int left, int right, int scale);
    void setDACLevel(int left, int right, int scale);
    void setLineLevel(int left, int right, int scale);
    void setCDALevel(int left, int right, int scale);
    void setMicLevel(int level, int scale);

    void getMasterVol(int* left, int* right, int scale);
    void getDACLevel(int* left, int* right, int scale);
    void getLineLevel(int* left, int* right, int scale);
    void getCDALevel(int* left, int* right, int scale);
    void getMicLevel(int* level, int scale);

  protected:
    int m_regIdx;
    char __dummy[256];        // use for compatibility: remembers values for registers that are not (yet) supported
    char m_IRQSelect, m_DMASelect, m_IRQStatus;
    char m_lastValue;
    int m_masterL, m_masterR, // master volume
        m_dacL, m_dacR,       // digital-to-analogue (DAC) volume
        m_lineL, m_lineR,     // line-in volume
        m_cdaL, m_cdaR,       // CD audio volume
        m_mic;                // microphone volume (mono)

  protected:
    ISBMixerHWEmulationLayer* m_hwemu;
};

#endif //__SBCOMPATCTLMIXER_H_
