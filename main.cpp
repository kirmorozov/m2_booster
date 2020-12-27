/**
 *  Copyright 2020 Kirill Morozov <kir.morozov@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <phpcpp.h>
#include <strstream>
#include <unordered_map>

using dataType = std::map<std::string, Php::Value>;

static std::map <std::string, std::string> _underscoreCache;

class DataObject : public Php::Base, public Php::ArrayAccess, public Php::PropertyPtrPtr {
protected:
    Php::Value _self;
    Php::Object _selfData;
    dataType _data;
    bool _hasDataChanges = false;


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


    std::string _toCamelCase(std::string str)
    {
        std::strstream res;
        for(int i=0; i < str.length(); i++) {
            // check for spaces in the sentence
            if (str[i] == '_') {
                // conversion into upper case
                if (str[i]>='a' && str[i]<='z') {
                    str[i + 1] = str[i + 1] - 32;
                }
                continue;
            }
                // If not _, copy character
            else
                if(i==0 ) {
                    str[i] -= 32;
                }
                res << str[i];
        }
        // return string to main
        res << std::ends;
        return res.str();
    }

    std::string _fromCamelCase(std::string str)
    {
        auto it = _underscoreCache.find(str);
        if (it != _underscoreCache.end()) return it->second;

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
        _underscoreCache[str] = res.str();
        return _underscoreCache[str];
    }

    std::string _fromCamelCase(std::string_view str)
    {
        std::strstream res;
        char tmpchar;
        // Traverse the string
        for(int i=0; i < str.length(); i++)
        {
            tmpchar = str[i]+32;
            // Convert to lowercase if its
            // an uppercase character
            if (str[i]>='A' && str[i]<='Z')
            {
                //str[i]=str[i]+32;
                // Print space before it
                // if its an uppercase character
                if (i != 0)
                    res << "_";

                // Print the character
                res << tmpchar;
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


    std::string replace_string (const std::string& source,
                          const std::string& toReplace,
                          const std::string& replaceWith)
    {
        size_t pos = 0;
        size_t cursor = 0;
        int repLen = toReplace.length();
        std::strstream builder;

        do
        {
            pos = source.find(toReplace, cursor);

            if (std::string::npos != pos)
            {
                //copy up to the match, then append the replacement
                builder << source.substr(cursor, pos - cursor);
                builder << replaceWith;

                // skip past the match
                cursor = pos + repLen;
            }
        }
        while (std::string::npos != pos);

        //copy the remainder
        builder << source.substr(cursor);

        return (builder.str());
    }

public:
    inline static const std::string CLASS_NAME =  "Magento\\Framework\\DataObject";
    class _DataWrapper: public Php::Base, public Php::ArrayAccess {
    protected:
        dataType* _dataRef;
    public:
        inline static const std::string CLASS_NAME =  "Magento\\Framework\\DataObject\\__wrapper";

        _DataWrapper(dataType* ref) {
            _dataRef = ref;
        }

        virtual ~_DataWrapper() {}

        bool offsetExists(const Php::Value &key) {
            return (_dataRef->find(key) != _dataRef->end());
        }

        void offsetSet(const Php::Value &key, const Php::Value &value)  {
            (*_dataRef)[key] = value;
        }

        Php::Value offsetGet(const Php::Value &key) {
            if (_dataRef->find(key) != _dataRef->end()) {
                return (*_dataRef)[key];
            }
            return nullptr;
        }

        void offsetUnset(const Php::Value &key) {
            _dataRef->erase(key);
        }

    };
    _DataWrapper* _data_wrapper;

    DataObject() {
        _data_wrapper = new _DataWrapper(&_data);
    };



    virtual ~DataObject() {
//        delete _data_wrapper;
    };

    /**
     *  php "constructor"
     *  @param  params
     */
    void __construct(Php::Parameters &params) {
        _self = Php::Value(this);
        _selfData = Php::Object(_DataWrapper::CLASS_NAME.c_str(), _data_wrapper);
//        _self["_data"] = _selfData;

        if (!params.empty()) {
            ___set_data(params[0]);
//            _data = params[0];
        } //_value = params[0];
    };

    Php::Value addData(Php::Parameters &params) {

//        Php::Value self(this);
//        Php::Value data = self["_data"];
        for (auto &iter : params[0]) {
            _data[iter.first] = iter.second;
        }

        return this;
    };

    Php::Value setData(Php::Parameters &params) {
        if (params.size() == 1 && params[0].isArray()) {
            ___set_data(params[0]);
//            _data = params[0];
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
            _hasDataChanges = true;
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
        Php::Value data;
        if (params.empty()) {
            return _data;
        }
        std::string key = params[0];
        // Php::out << key << "<<key" <<std::endl;
        if (key.find('/') != std::string::npos) {

            data = getDataByPath(key);
        } else {
            data = _getData(key);
        }

        if(params.size() ==2 && !params[1].isNull()) {
            _getData_by_index(data, params[1]);
        }

        return data;
    }

    Php::Value _getData_by_index(Php::Value &data, Php::Value &index) {
        if (data.isArray()) {
            Php::Value nextData = data[index];
            data = nextData;
        } else if (data.isString()) {
            std::vector <std::string> new_data;
            split_string(data, new_data, '\n');
            int idx = index;
            if (new_data.size() > idx) {
                data = new_data[idx];
            } else {
                data = nullptr;
            }
        } else if (data.isObject() && data.instanceOf(CLASS_NAME)) {
            auto * obj = data.implementation<DataObject>();
            data = obj->offsetGet(index);
        } else {
            data = nullptr;
        }
        return data;
    }

    Php::Value getDataByPath(Php::Parameters &params) {
        std::string key = params[0];
        return this->getDataByPath(key);
    }

    Php::Value getDataByPath(std::string &path) {
        std::vector<std::string> keys;
        split_string(path, keys, '/');
        auto it = keys.begin();

        Php::Value data = _data[*it];
        //Php::out << "Started: " << (*it) << std::endl;
        for (++it;it != keys.end();++it) {
            auto key = *it;
            //Php::out <<"Next key"<< key << std::endl;
            if (data.isArray()) {
                //auto newData = data[key];
                // Php::out <<"Next value"<< data[key] << std::endl;
                data = data[key];
            } else if (data.isObject() && data.instanceOf(CLASS_NAME)){
                DataObject * obj = data.implementation<DataObject>();
                data = obj->offsetGet(key);
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

    Php::Value setDataUsingMethod(Php::Parameters &params) {
        std::string method = _toCamelCase(params[0]);
        method.insert(0, "set");
        Php::Array call({this, method});
        // Php::out << "setDataUsingMethod: " << method << std::endl;

        if (params.size() > 1) {
            return call(params[1]);
        } else {
            return call();
        }
    }

    Php::Value getDataUsingMethod(Php::Parameters &params) {
        std::string method = _toCamelCase(params[0]);
        method.insert(0, "get");
        // Php::out << "getDataUsingMethod: " << method << std::endl;
        Php::Array call({this, method});
        return call();
    }

    Php::Value hasData(Php::Parameters &params) {
        if (params.empty()) {
            return (_data.size()>0);
        }
        return offsetExists(params[0]);
    }

    Php::Value toArray(Php::Parameters &params) {
        Php::Value val;
        if (!params.empty())
        {
            val = params[0];
        } else {
            return _data;
        }
        return _toArray(val);

    }
    Php::Value _toArray(Php::Value &keys) {
        Php::Array result;

        for (auto &iter : keys)
        {
            auto key = iter.second;
            if (_data.find(key) != _data.end()) {
                result[key] = _data[key];
            } else {
                result[key] = nullptr;
            }
            // output key and value
            // Php::out << iter.first << ": " << iter.second << std::endl;
        }

        return result;
    }


    Php::Value toXml(Php::Parameters &params) {
        return nullptr;
    }

    Php::Value toJson(Php::Parameters &params) {
        Php::Value val;
        if (!params.empty())
        {
            val = _toArray(params[0]);

        } else {
            val = _data;
        }

        return Php::call("json_encode", val);
    }

    Php::Value toString(Php::Parameters &params) {
        std::strstream res;
        bool firstItem = true;
        if (params.size() ==0 || params[0] == "" ) {
            for(auto &iter :  _data) {
                if (firstItem) {
                    firstItem = false;
                } else {
                    res << ", ";
                }
                res << iter.second;
            }
        } else {
            std::string format = params[0];
            Php::Array matches;
            //Php::call("preg_match_all", "/{{([a-z0-9_]+)}}/is", format, matches);
//            Php::Array toReplace = matches[1];
//            for(auto &it : toReplace) {
//                std::string var = it.second;
//                var += "}}";
//                var.insert(0, "set");
//                format = replace_string(format, var, it.second);
//            }
            res << format;


        }
        res << std::ends;
        return res.str();
    }

    Php::Value isEmpty(Php::Parameters &params) {
        return _data.size() == 0;
    }


    Php::Value __call(const char *name, Php::Parameters &params)
    {
        auto action = std::string_view(name).substr(0,3);
        auto key = std::string(name).substr(3);
        // Php::out << "CallKey" << key << std::endl;
        if (action == "get") {
//            Php::Parameters args;
            auto ukey = _fromCamelCase(key);
            //auto index = (params.size()>0) ? params[0] : nullptr;
            auto data = _getData(ukey);
            if(params.size() >0 && !params[0].isNull()) {
                _getData_by_index(data, params[0]);
            }
//            Php::Parameters args{key,index};
            return data; // this->getData();
        } else if (action == "set") {
            auto ukey = _fromCamelCase(key);
            // Php::out << "CallUKey" << ukey << std::endl;
            _data[ukey] = (params.size()>0) ? params[0] : nullptr;
            return this;
        } else if (action == "uns") {
            auto ukey = _fromCamelCase(key);
            _data.erase(ukey);
            return this;
        } else if (action == "has") {
            auto ukey = _fromCamelCase(key);
            return (_data.find(ukey) != _data.end());
        }

        return nullptr;
    }

    Php::Value _underscore(Php::Parameters &params) {
        return _fromCamelCase(params[0]);
    }


    Php::Value _vall() {
        //int s = _data.size();
        return (int) _data.size();
    };


    Php::Value serialize(Php::Parameters &params) {
        int size =  params.size();
        bool emptyKeys = false;
        std::set<std::string> keys;
        if (size < 1) {
            emptyKeys = true;
        } else {
            for (auto &iter : params[0])
            {
                keys.insert(iter.second);
            }
        }

        std::string valueSeparator = (size >= 2) ? std::string(params[1]) : "=";
        std::string fieldSeparator = (size >= 3) ? std::string(params[2]) : " ";
        std::string quote = (size >= 4) ? std::string(params[3]) : "\"";
        std::strstream res;

        int isFirst = true;
        for (auto &iter : _data)
        {
            if (isFirst) {
                isFirst = false;
            } else {
                res << fieldSeparator;
            }

            auto key = iter.first;
            if (emptyKeys || keys.find(key) != keys.end()) {
                res << key << valueSeparator << quote << iter.second << quote;
            }
        }
        res << std::ends;

        return res.str();
    }

    Php::Value hasDataChanges() {
        return _hasDataChanges;
    }

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

//    void __set(const Php::Value &key, const Php::Value &value) {
//        std::string k = key;
//        if (k == "_data") {
//            ___set_data(value);
//        } else {
//            _data[key] = value;
//        }
//    }
//
//    Php::Value __get(const Php::Value &key)
//    {
//        std::string k = key;
//        return _getData(k);
//    }

    Php::Value getPropertyPtrPtr(const Php::Value &member, int type)
    {
        if ("_data" == member) {
            return  _selfData;
        }

        return nullptr;
    }

    void ___set_data(const Php::Value &value) {
        _data = value;
//        for (auto &iter : value) {
//            _data[iter.first] = iter.second;
//        }
    }

    Php::Value ___get_data() {
        return _selfData;
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

    //Php::Namespace myNamespace("myNamespace");

    Php::Class <DataObject::_DataWrapper> data_obj_wrapper((DataObject::_DataWrapper::CLASS_NAME).c_str());
    extension.add(std::move(data_obj_wrapper));

    Php::Class <DataObject> data_obj((DataObject::CLASS_NAME).c_str());
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

    data_obj.method<&DataObject::toArray>("convertToArray", {
            Php::ByVal("keys", Php::Type::Array, false)
    });

    data_obj.method<&DataObject::toXml>("toXml", {
            Php::ByVal("keys", Php::Type::Array, false)
    });

    data_obj.method<&DataObject::toJson>("toJson", {
            Php::ByVal("keys", Php::Type::Array, false)
    });

    data_obj.method<&DataObject::toJson>("convertToJson", {
            Php::ByVal("keys", Php::Type::Array, false)
    });

    data_obj.method<&DataObject::toString>("toString", {
            Php::ByVal("format", Php::Type::String, false)
    });

    data_obj.method<&DataObject::serialize>("serialize", {
            Php::ByVal("keys", Php::Type::Array, false),
            Php::ByVal("valueSeparator", Php::Type::String, false),
            Php::ByVal("fieldSeparator", Php::Type::String, false),
            Php::ByVal("quote", Php::Type::String, false)
    });

    data_obj.method<&DataObject::hasDataChanges>("hasDataChanges");

    data_obj.method<&DataObject::offsetExists>("offsetExists", {
            Php::ByVal("key", Php::Type::String, true)
    });

    data_obj.method<&DataObject::offsetSet>("offsetSet", {
            Php::ByVal("key", Php::Type::String, true),
            Php::ByVal("value", Php::Type::Null, true)
    });

    data_obj.method<&DataObject::offsetGet>("offsetGet", {
            Php::ByVal("key", Php::Type::String, true)
    });

    data_obj.method<&DataObject::offsetUnset>("offsetUnset", {
            Php::ByVal("key", Php::Type::String, true)
    });

    data_obj.method<&DataObject::isEmpty>("isEmpty");


    data_obj.method<&DataObject::_underscore>("_underscore", Php::Protected, {
            Php::ByVal("format", Php::Type::String, false)
    });


    data_obj.method<&DataObject::_vall>("_vall", {
    });

//    data_obj.property("_data", nullptr, Php::Protected);
    data_obj.property("_data", &DataObject::___get_data, &DataObject::___set_data);

    // add the class to the extension
    extension.add(std::move(data_obj));
//    Php::Interface asd();

//    extension.onRequest([]() {
//
//        // re-initialize the counter
//        // Php::eval("define('SOME_CONSTANT',123123);");
//        Php::define("DYNAMIC_CONSTANT", 12345);
////        Php::eval("namespace OK { class OK {}}");
//    });


    // return the extension
    return extension;
}
}
