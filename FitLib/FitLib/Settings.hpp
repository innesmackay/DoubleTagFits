#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <fstream>
#include <map>

#include "TString.h"
#include "Log.hpp"

/**
 * Class to reading the fit configuration from a text file
 * The text file contains a list of keys and values 
 * which are read into a dictionary.
*/
class Settings {

  public:
    /** Empty constructor function */
    Settings(){}

    /** Constructor function with name of file to read */
    Settings(std::string filename){ m_log = Log("Settings"); m_filename = filename; }

    /** Empty deconstructor function */
    ~Settings(){};

    /** Read the config into a dictionary {key, value} */
    void read(){
        m_log.info(("Reading settings file from " + m_filename).c_str());
        std::ifstream infile; infile.open(m_filename);
        std::string line;
        while (std::getline(infile, line)){
            line = line.substr(0, line.find("*")); // ignore comments in the config file
            if (line[0] == '*') continue;
            std::string delimiter = " ";
            std::string var = line.substr(0, line.find(delimiter)); 
            std::string val = line.substr(line.find(delimiter) + 1, line.size());
            m_var_map.insert({var, val});
        }
        infile.close();
        return;
    }

    /** Update the value of a setting */
    void update_value(std::string key, std::string value){
        m_var_map[key] = value;
        return;
    }

    /** Check if a key exists in the dictionary */
    bool key_exists(std::string key){
        bool found = false;
        if (m_var_map.count(key) > 0){ found = true; }
        return found;
    }

    /** Read a boolean from the settings */
    bool getB(std::string key){
        bool val = false;
        if ( !key_exists(key) ){ m_log.warning( ("Can't read key: " + key + "\nReturning false").c_str() ); }
        else{
            if (m_var_map[key] == "true"){ val = true; }
        }
        return val;
    }

    /** Read a string from the settings */
    std::string get(std::string key){ return m_var_map[key]; }

    /** Read a TString from the settings */
    TString getT(std::string key){ return m_var_map[key].c_str(); }

    /** Read a double from the settings */
    double getD(std::string key){ return std::stod(m_var_map[key]); }

    /** Read an integer from the settings */
    double getI(std::string key){ return std::stoi(m_var_map[key]); }

    /** Return the dictionary */
    std::map<std::string, std::string> get_map(){ return m_var_map; }

    /** Get the name of the settings file */
    std::string getFilename(){ return m_filename; }

  private:
    /** Name of the settings file */
    std::string m_filename;

    /** Dictionary */
    std::map<std::string, std::string> m_var_map;

    /** Logging class */
    Log m_log;

};

#endif //  Settings_H
