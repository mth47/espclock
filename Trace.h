/* mthomas
   used to define several trace and debugging helpers
   as well as to store a summary of traces, e.g. for daily mails (not finished yet)
*/

#ifndef _TRACE_H
#define _TRACE_H

#include <stdarg.h>

#define TRACE_SIZE_IN_BYTES 255
static char TRACE_BUFFER [TRACE_SIZE_IN_BYTES];

enum eTraceTag
{
  T_DEBUG = 0,
  T_INFO,
  T_WARNING,
  T_ERROR,
  // STATUS traces are stored and summerized in order to provide a daily status message
  T_STATUS_INFO,
  T_STATUS_WARNING,
  T_STATUS_ERROR,
};

class Trace_Internal
{
  public:

    Trace_Internal()
    {
      ClearSummary();
      m_bInit = false;
    }

    virtual ~Trace_Internal() {}

    virtual bool Init(size_t baudrate) = 0;
    virtual size_t Log(enum eTraceTag tag, const char *format, ... ) = 0;
    virtual size_t Log(const char *format, ... ) = 0;

  protected:

    bool CreateTraceMessage(const char *format, va_list& args, bool bAdd = false)
    {
      if (!m_bInit)
        return false;

      if (!bAdd)
        memset(TRACE_BUFFER + 0, 0, TRACE_SIZE_IN_BYTES);

      size_t len = strlen(TRACE_BUFFER);

      vsnprintf(TRACE_BUFFER + len, TRACE_SIZE_IN_BYTES - len - 1, format, args);

      return true;
    }

    bool StartTraceMessageWithTag(enum eTraceTag tag)
    {
      if (!m_bInit)
        return false;

      memset(TRACE_BUFFER + 0, 0, TRACE_SIZE_IN_BYTES);

      switch (tag)
      {
        case T_DEBUG:
          strcpy(TRACE_BUFFER, "DEBUG: ");
          break;
        case T_INFO:
        case T_STATUS_INFO:
          strcpy(TRACE_BUFFER, "INFO: ");
          break;
        case T_WARNING:
        case T_STATUS_WARNING:
          strcpy(TRACE_BUFFER, "WARNING: ");
          break;
        case T_ERROR:
        case T_STATUS_ERROR:
        default:
          strcpy(TRACE_BUFFER, "ERROR: ");
          break;
      }

      return true;
    }

    bool m_bInit;

    String* GetSummary () { return &m_Summary; }
    void AddToSummary (const char* s) { m_Summary += s; }
    void AddToSummary (const String& s) { m_Summary += s; }
    void ClearSummary() { m_Summary = ""; }

    private:

    // at some point it might make sense to save everything in a file
    String m_Summary;
};


#ifdef TRACE

class Trace : private Trace_Internal
{
  public:

    Trace() : Trace_Internal(), m_res(0)
    {
      // initialization of globals is to early. serial comm would be closed before setup method is called anyway
      //InitSerialComm();
    }

    bool Init(size_t baudrate)
    {
      if (m_bInit)
        return true;

      m_bInit = InitSerialComm(baudrate);
      if(m_bInit)
        Serial.println("Tracing initialized.");
      else
        Serial.println("Tracing disabled.");
 
      return m_bInit;
    }

    virtual ~Trace()
    {
      TraceTag(T_INFO);
      Serial.println("Serial communication disabled");

      Serial.flush();
      Serial.end();
    }

    size_t Log(const char *format, ... )
    {
      if (!m_bInit)
        return 0;

      va_list args;
      va_start (args, format);

      if (!CreateTraceMessage(format, args, false))
      {
        va_end (args);
        Serial.println("ERROR: Failed to create trace");
        Serial.flush();
        return 0;
      }

      va_end (args);

      m_res = Serial.println(TRACE_BUFFER);
      Serial.flush();
      return m_res;
    }

    size_t Log(enum eTraceTag tag, const char *format, ... )
    {
      if (!m_bInit)
        return 0;

#ifndef _DEBUG
      if (T_DEBUG == tag)
        return m_res = 0;
#endif

      StartTraceMessageWithTag(tag);

      va_list args;
      va_start (args, format);
      if (!CreateTraceMessage(format, args, true))
      {
        va_end (args);
        Serial.println("ERROR: Failed to create trace");
        Serial.flush();
        return 0;
      }

      va_end (args);
      m_res = Serial.println(TRACE_BUFFER);
      Serial.flush();
      return m_res;
    }

  private:

    size_t m_res;

    void TraceTag(enum eTraceTag tag)
    {
      if (!m_bInit)
        return;

      switch (tag)
      {
        case T_DEBUG:
#ifdef _DEBUG
          Serial.print("DEBUG: ");
#endif
          break;
        case T_INFO:
        case T_STATUS_INFO:
          Serial.print("INFO: ");
          break;
        case T_WARNING:
        case T_STATUS_WARNING:
          Serial.print("WARNING: ");
          break;
        case T_ERROR:
        case T_STATUS_ERROR:
        default:
          Serial.print("ERROR: ");
          break;
      }
    }

    bool InitSerialComm(size_t baudrate)
    {
      Serial.begin(baudrate);

      TraceTag(T_INFO);

      memset(TRACE_BUFFER + 0, 0, TRACE_SIZE_IN_BYTES);
      snprintf(TRACE_BUFFER, TRACE_SIZE_IN_BYTES, "Serial communication enabled (%d)", baudrate);
      m_res = Serial.println(TRACE_BUFFER);

      Serial.flush();

#ifdef _DEBUG
      Serial.println("Debug Traces enabled.");
#else
      Serial.println("Debug Traces disabled.");
#endif
      
      return (0 != m_res);
    }

    
};

#else

class Trace : private Trace_Internal
{
  public:

    Trace() : Trace_Internal() { }

    bool Init(size_t baudrate)
    {
      return (m_bInit = true);
    }

    virtual ~Trace()  { }

    size_t Log(const char *format, ... ) 
    {
      return 0;
    }

    size_t Log(enum eTraceTag tag, const char *format, ... )
    {
      switch (tag)
      {
        case T_STATUS_INFO:
          break;
        case T_STATUS_WARNING:
          break;
        case T_STATUS_ERROR:
          break;
        case T_DEBUG:
        case T_INFO:
        case T_WARNING:
        case T_ERROR:
        default:
          break;
      }
      return 0;
    }

};

#endif // TRACE

#endif // _TRACE_H


