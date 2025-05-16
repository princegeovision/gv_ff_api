#ifndef FF_STREAM_FEATURE_STRATEGY_H_
#define FF_STREAM_FEATURE_STRATEGY_H_

//CXX11
#include <chrono>

#include <map>

namespace gv
{

    //This class is base class of all feature
    class FeatureStrategy 
    {
    public:
        //virtual int write(char* inputData);
        //virtual int read(char* outputData);

        FeatureStrategy() noexcept;
        virtual ~FeatureStrategy() = default;
    private:
        bool        b_fs_log_;
    };

    class FeatureWrite : public FeatureStrategy {
    public:
        //int write(char* inputData) override;//when base-has-virtual
        int write(char* inputData);
    };

    class FeatureReader : public FeatureWrite {
    public:
        //int read(char* outputData) override;//when base-has-virtual
        int read(char* outputData);
    };
};//end of namespace gv
#endif  // FF_STREAM_FEATURE_STRATEGY_H_
