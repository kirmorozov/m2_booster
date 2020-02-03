#include <phpcpp.h>


class DataObject : public Php::Base, public Php::ArrayAccess
{
protected:
    std::map<std::string, Php::Value> _data;
    static std::map<std::string, std::string> _underscoreCache;
public:
    DataObject() {};
    virtual ~DataObject() {};

    /**
     *  php "constructor"
     *  @param  params
     */
    void __construct(Php::Parameters &params)
    {
        if (!params.empty()) {
            _data = params[0];
        } //_value = params[0];
    };

    Php::Value addData(Php::Parameters &params) {

        for (auto &iter : params[0])
        {
            _data[iter.first] = iter.second;
            // output key and value
            // Php::out << iter.first << ": " << iter.second << std::endl;
        }

        return this;
    };

    Php::Value setData(Php::Parameters &params) {
        if(params.size() == 1 && params[0].isArray()) {
            _data = params[0];
        }
        if(params.size() == 2 && params[0].isString()) {
            std::string name = params[0];
            Php::Value value = params[1];
            _data[name] = value;
            // Php::out << name;
        }

        return this;
    };

    Php::Value unsetData(Php::Parameters &params) {
        if(params.empty()) {
            _data.clear();
        } else if (params[0].isString()) {
            std::string key = params[0];
            _data.erase(key);
        } else if (params[0].isArray()) {
            std::vector<std::string> keys = params[0];
            for(auto key : keys) {
                _data.erase(key);
            }
        }
        return this;
    }

    Php::Value _vall() {
        //int s = _data.size();
        return (int)_data.size();
    };

    /**
     *  Method from the Php::ArrayAccess interface that is
     *  called to check if a certain key exists in the map
     *  @param  key
     *  @return bool
     */
    virtual bool offsetExists(const Php::Value &key) override
    {
        return _data.find(key) != _data.end();
    }

    /**
     *  Set a member
     *  @param  key
     *  @param  value
     */
    virtual void offsetSet(const Php::Value &key, const Php::Value &value) override
    {
        _data[key] = value.stringValue();
    }

    /**
     *  Retrieve a member
     *  @param  key
     *  @return value
     */
    virtual Php::Value offsetGet(const Php::Value &key) override
    {
        return _data[key];
    }

    /**
     *  Remove a member
     *  @param key
     */
    virtual void offsetUnset(const Php::Value &key) override
    {
        _data.erase(key);
    }
};

/**
 *  Counter class that can be used for counting
 */
class Counter : public Php::Base
{
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
    Php::Value increment(Php::Parameters &params)
    {
        return _value += params.empty() ? 1 : (int)params[0];
    }

    /**
     *  Decrement operation
     *  This method gets one optional parameter holding the change
     *  @param  int     Optional decrement value
     *  @return int     New value
     */
    Php::Value decrement(Php::Parameters &params)
    {
        return _value -= params.empty() ? 1 : (int)params[0];
    }

    /**
     *  Method to retrieve the current value
     *  @return int
     */
    Php::Value value() const
    {
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
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("morozov_m2_booster", "1.0");
        
        // @todo    add your own functions, classes, namespaces to the extension
        // description of the class so that PHP knows which methods are accessible
        Php::Class<Counter> counter("Some\\Counter");

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

        Php::Class<DataObject> data_obj("Magento\\Framework\\DataObject");
        data_obj.method<&DataObject::__construct>("__construct",{
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

        data_obj.method<&DataObject::_vall>("_vall", {
        });


        // add the class to the extension
        extension.add(std::move(counter));
        extension.add(std::move(data_obj));

        // return the extension
        return extension;
    }
}
