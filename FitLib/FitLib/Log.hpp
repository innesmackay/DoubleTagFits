#ifndef LOG_H
#define LOG_H

#include "TString.h"

class Log {

public:
    Log(){ _script = ""; };
    Log(TString script){ _script = script; };
    ~Log(){};

    void info(TString message){ this->general(message, "log"); };
    void warning(TString message){ this->general(message, "warning"); };
    void error(TString message){ this->general(message, "error"); };
    void success(TString message){ this->general(message, "success"); };
    void debug(TString message){ this->general(message, "debug"); };
    void empty(){ printf("\n"); }
    void star_line(TString type){ this->general("*************************************************", type); }

private:
    TString _script;

    void general(TString message, TString type){
        TString preset;
        if (_script == ""){ preset = ""; }
        else{ preset = "::"; }

        if (type == "log"){ printf("\033[94m" + _script + preset + "LOG " + message + "\033[0m \n"); }
        else if (type == "warning"){ printf("\033[93m" + _script + preset + "WARNING " + message + "\033[0m \n"); }
        else if (type == "error"){ printf("\033[91m" + _script + preset + "ERROR " + message + "\033[0m \n"); }
        else if (type == "success"){ printf("\033[92m" + _script + preset + "SUCCESS " + message + "\033[0m \n"); }
        else if (type == "debug"){ printf("\033[35m\e[1m" + _script + preset + "DEBUG " + message + "\e[0m\033[0m \n"); }
        else{ printf("\033[91mLog::ERROR Message type does not exist \033[0m \n"); }

        return;
    }

};

#endif //  Log_H
