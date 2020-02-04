#include <phpcpp.h>
#include <strstream>

template<class Container>
void split_string(const std::string &str, Container &cont,
                  char delim = ' ') {
    std::size_t current, previous = 0;
    current = str.find(delim);
    while (current != std::string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
}

class DataObject : public Php::Base, public Php::ArrayAccess {
protected:
    std::map <std::string, Php::Value> _data;
    static std::map <std::string, std::string> _underscoreCache;
public:
    DataObject() {};

    virtual ~DataObject() {};

    /**
     *  php "constructor"
     *  @param  params
     */
    void __construct(Php::Parameters &params) {
        if (!params.empty()) {
            _data = params[0];
        } //_value = params[0];
    };

    Php::Value addData(Php::Parameters &params) {

        for (auto &iter : params[0]) {
            _data[iter.first] = iter.second;
            // output key and value
            // Php::out << iter.first << ": " << iter.second << std::endl;
        }

        return this;
    };

    Php::Value setData(Php::Parameters &params) {
        if (params.size() == 1 && params[0].isArray()) {
            _data = params[0];
        }
        if (params.size() == 1 && params[0].isString()) {
            _data[params[0]] = nullptr;
        }
        if (params.size() == 2 && params[0].isString()) {
            std::string name = params[0];
            Php::Value value = params[1];
            _data[name] = value;
            // Php::out << "setData: " << name << " val:" << _data.size() << std::endl;
        }

        return this;
    };

    Php::Value unsetData(Php::Parameters &params) {
        if (params.empty()) {
            _data.clear();
        } else if (params[0].isString()) {
            std::string key = params[0];
            _data.erase(key);
        } else if (params[0].isArray()) {
            std::vector <std::string> keys = params[0];
            for (auto key : keys) {
                _data.erase(key);
            }
        }
        return this;
    }

    Php::Value getData(Php::Parameters &params) {
        if (params.empty()) {
            return _data;
        }
        std::string key = params[0];
        if (key.find('/') != std::string::npos) {
            return this->getDataByPath(key);
        } else {
            return this->_getData(key);
        }

        if (params[0].isArray()) {
            std::vector <std::string> keys = params[0];
            for (auto key : keys) {
                _data.erase(key);
            }
        }
        return this;
    }

    Php::Value getDataByPath(Php::Parameters &params) {
        std::string key = params[0];
        return this->getDataByPath(key);
    }

    Php::Value getDataByPath(std::string &path) {
        std::vector <std::string> keys;
        split_string(path, keys);

        std::map <std::string, Php::Value> res_data = _data;
        Php::Value data = _data;
        for (auto key : keys) {
            if (data.isArray() && (res_data.find(key) != res_data.end())) {
                data = res_data[key];
                if (data.isArray()) {
                    res_data = data;
                }
//            } else if (data.isObject() && instanceof<DataObject>data){
//                data = data->getDataByKey(key);
//                if (data.isArray()) {
//                    res_data = data;
//                }
            } else {
                return nullptr;
            }
        }
        return data;
    }

    Php::Value getDataByKey(Php::Parameters &params) {
        std::string key = params[0];
        return this->_getData(key);
    }

    Php::Value _getData(Php::Parameters &params) {
        std::string key = params[0];
        return this->_getData(key);
    }

    Php::Value _getData(std::string &key) {
        if (_data.find(key) != _data.end()) {
            return _data[key];
        }
        return nullptr;
    }

    std::string _toCamelCase(std::string str)
    {
        std::strstream res;
        for(int i=0; i < str.length(); i++) {
            // check for spaces in the sentence
            if (str[i] == '_') {
                // conversion into upper case
                res << toupper(str[i + 1]);
                continue;
            }
                // If not _, copy character
            else
                res << str[i];
        }
        // return string to main
        res << std::ends;
        return res.str();
    }

    std::string _fromCamelCase(std::string str)
    {
        std::strstream res;
        // Traverse the string
        for(int i=0; i < str.length(); i++)
        {
            // Convert to lowercase if its
            // an uppercase character
            if (str[i]>='A' && str[i]<='Z')
            {
                str[i]=str[i]+32;
                // Print space before it
                // if its an uppercase character
                if (i != 0)
                    res << "_";

                // Print the character
                res << str[i];
            } else if (str[i] >='0' && str[i] <='9'
                && i>0 && !(str[i-1] >='0' && str[i-1] <='9'))
            {
                res << "_" << str[i];
            }
                // if lowercase character
                // then just print
            else
                res << str[i];
        }
        res << std::ends;
        return res.str();
    }

    Php::Value setDataUsingMethod(Php::Parameters &params) {
        return this;
    }

    Php::Value getDataUsingMethod(Php::Parameters &params) {
        return nullptr;
    }

    Php::Value hasData(Php::Parameters &params) {
        if (params.empty()) {
            return (_data.size()>0);
        }
        return offsetExists(params[0]);
    }

    Php::Value toArray(Php::Parameters &params) {
        return nullptr;
    }

    Php::Value toXml(Php::Parameters &params) {
        return nullptr;
    }

    Php::Value toJson(Php::Parameters &params) {
        return nullptr;
    }

    Php::Value toString(Php::Parameters &params) {
        return nullptr;
    }

    Php::Value isEmpty(Php::Parameters &params) {
        return _data.size() == 0;
    }


    Php::Value __call(const char *name, Php::Parameters &params)
    {
        return nullptr;
    }

    Php::Value _underscore(Php::Parameters &params) {
        return _fromCamelCase(params[0]);
    }


    Php::Value _vall() {
        //int s = _data.size();
        return (int) _data.size();
    };


    /**
     *  Method from the Php::ArrayAccess interface that is
     *  called to check if a certain key exists in the map
     *  @param  key
     *  @return bool
     */
    virtual bool offsetExists(const Php::Value &key) override {
        return _data.find(key) != _data.end();
    }

    Php::Value offsetExists(Php::Parameters &params) {
        return offsetExists(params[0]);
    }

    /**
     *  Set a member
     *  @param  key
     *  @param  value
     */
    virtual void offsetSet(const Php::Value &key, const Php::Value &value) override {
        _data[key] = value;
    }

    Php::Value offsetSet(Php::Parameters &params) {
        offsetSet(params[0], params[1]);
        return nullptr;
    }

    /**
     *  Retrieve a member
     *  @param  key
     *  @return value
     */
    virtual Php::Value offsetGet(const Php::Value &key) override {
        if (_data.find(key) != _data.end()) {
            return _data[key];
        }
        return nullptr;
    }

    Php::Value offsetGet(Php::Parameters &params) {
        return offsetGet(params[0]);
    }

    /**
     *  Remove a member
     *  @param key
     */
    virtual void offsetUnset(const Php::Value &key) override {
        _data.erase(key);
    }
    Php::Value offsetUnset(Php::Parameters &params) {
        offsetUnset(params[0]);
        return nullptr;
    }
};

/**
 *  Counter class that can be used for counting
 */
class Counter : public Php::Base {
private:
    /**
     *  The internal value
     *  @var    int
     */
    int _value = 0;

public:
    /**
     *  C++ constructor and destructor
     */
    Counter() {}

    virtual ~Counter() {}

    /**
     *  Increment operation
     *  This method gets one optional parameter holding the change
     *  @param  int     Optional increment value
     *  @return int     New value
     */
    Php::Value increment(Php::Parameters &params) {
        return _value += params.empty() ? 1 : (int) params[0];
    }

    /**
     *  Decrement operation
     *  This method gets one optional parameter holding the change
     *  @param  int     Optional decrement value
     *  @return int     New value
     */
    Php::Value decrement(Php::Parameters &params) {
        return _value -= params.empty() ? 1 : (int) params[0];
    }

    /**
     *  Method to retrieve the current value
     *  @return int
     */
    Php::Value value() const {
        return _value;
    }
};

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {

/**
 *  Function that is called by PHP right after the PHP process
 *  has started, and that returns an address of an internal PHP
 *  strucure with all the details and features of your extension
 *
 *  @return void*   a pointer to an address that is understood by PHP
 */
PHPCPP_EXPORT void *get_module() {
    // static(!) Php::Extension object that should stay in memory
    // for the entire duration of the process (that's why it's static)
    static Php::Extension extension("morozov_m2_booster", "1.0");

    // @todo    add your own functions, classes, namespaces to the extension
    // description of the class so that PHP knows which methods are accessible
    Php::Class <Counter> counter("Some\\Counter");

    // register the increment method, and specify its parameters
    counter.method<&Counter::increment>("increment", {
            Php::ByVal("change", Php::Type::Numeric, false)
    });

    // register the decrement, and specify its parameters
    counter.method<&Counter::decrement>("decrement", {
            Php::ByVal("change", Php::Type::Numeric, false)
    });

    // register the value method
    counter.method<&Counter::value>("value", {});

    //Php::Namespace myNamespace("myNamespace");

    Php::Class <DataObject> data_obj("Magento\\Framework\\DataObject");
    data_obj.method<&DataObject::__construct>("__construct", {
            Php::ByVal("arr", Php::Type::Array, false)
    });
    // register the decrement, and specify its parameters
//        data_obj.method<&DataObject::addData>("addData");

    data_obj.method<&DataObject::addData>("addData", {
            Php::ByVal("arr", Php::Type::Array, true)
    });

    data_obj.method<&DataObject::setData>("setData", {
            Php::ByVal("name", Php::Type::Null, true),
            Php::ByVal("value", Php::Type::Null, false)
    });

    data_obj.method<&DataObject::unsetData>("unsetData", {
            Php::ByVal("key", Php::Type::Null, false)
    });

    data_obj.method<&DataObject::getData>("getData", {
            Php::ByVal("key", Php::Type::String, false)
    });

    data_obj.method<&DataObject::getDataByPath>("getDataByPath", {
            Php::ByVal("key", Php::Type::String, true)
    });

    data_obj.method<&DataObject::getDataByKey>("getDataByKey", {
            Php::ByVal("key", Php::Type::String, true)
    });

    data_obj.method<&DataObject::setDataUsingMethod>("setDataUsingMethod", {
            Php::ByVal("key", Php::Type::String, true),
            Php::ByVal("args", Php::Type::Null, false)

    });

    data_obj.method<&DataObject::getDataUsingMethod>("getDataUsingMethod", {
            Php::ByVal("key", Php::Type::String, true),
            Php::ByVal("args", Php::Type::Null, false)
    });

    data_obj.method<&DataObject::hasData>("hasData", {
            Php::ByVal("key", Php::Type::String, false)
    });

    data_obj.method<&DataObject::toArray>("toArray", {
            Php::ByVal("keys", Php::Type::Array, false)
    });

    data_obj.method<&DataObject::toXml>("toXml", {
            Php::ByVal("keys", Php::Type::Array, false)
    });

    data_obj.method<&DataObject::toJson>("toJson", {
            Php::ByVal("keys", Php::Type::Array, false)
    });

    data_obj.method<&DataObject::toString>("toString", {
            Php::ByVal("format", Php::Type::String, false)
    });

    data_obj.method<&DataObject::offsetExists>("offsetExists", {
            Php::ByVal("format", Php::Type::String, true)
    });

    data_obj.method<&DataObject::offsetSet>("offsetSet", {
            Php::ByVal("format", Php::Type::String, true),
            Php::ByVal("format", Php::Type::Null, true)
    });

    data_obj.method<&DataObject::offsetGet>("offsetGet", {
            Php::ByVal("format", Php::Type::String, true)
    });

    data_obj.method<&DataObject::offsetUnset>("offsetUnset", {
            Php::ByVal("format", Php::Type::String, true)
    });

    data_obj.method<&DataObject::isEmpty>("isEmpty");


    data_obj.method<&DataObject::_underscore>("_underscore", Php::Protected, {
            Php::ByVal("format", Php::Type::String, false)
    });


    data_obj.method<&DataObject::_vall>("_vall", {
    });


    // add the class to the extension
    extension.add(std::move(counter));
    extension.add(std::move(data_obj));

    // return the extension
    return extension;
}
}
