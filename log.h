#ifndef __SYLAR_LOH_H__
#define __SYLAR_LOG_H__
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>


namespace sylar {

//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();

private:
    const char* m_file = nullptr;      //文件名
    int32_t m_line = 0;                //行号
    uint32_t m_elapse= 0;              //程序启动开始到现在的毫秒数 
    uint32_t m_threadId = 0;           //线程id
    uint32_t m_fiberId = 0;            //协程id
    uint64_t m_time;                   //时间戳
    std::string m_content;

};

//日志级别
class LogLevel{
public:
    enum Level{
        DEBUG =1,
        INFO =2,
        WARN =3,
        ERROR = 4,
        FATAL = 5
    };
};

//日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {};
    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr val) { m_formatter = val;} 
    LogFormatter::ptr getFormatter() const { return m_formatter;}

protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};

//日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    void init();
    std::string format(LogEvent::ptr event);
    LogFormatter(const std::string& pattern);
private:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem () {};
        virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
        
    };
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
} ;


//日志器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;
    
    Logger(const std::string& name = "root");

    void log(LogLevel::Level  level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const { return m_level;};
    void setLevel(LogLevel::Level val) {m_level = val;}; 

private:
    std::string m_name;
    LogLevel::Level m_level;
    std::list<LogAppender::ptr> m_appenders;           //appender集合
};

//输出到控制台的Appender
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;

    void log(LogLevel::Level level, LogEvent::ptr event) override;
};


//定义输出到文件的Appender
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(LogLevel::Level level, LogEvent::ptr event) override;

    //重新打开文件，文件打开成功返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};


}
#endif
