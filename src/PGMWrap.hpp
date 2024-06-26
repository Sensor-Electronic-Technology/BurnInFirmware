#pragma once
#include <avr/pgmspace.h>

/**
 * @brief Attempt at wrapping PGM data in a class
 * Copied from https://github.com/Chris--A/PGMWrap
 * 
 * Plan was to modify to make it cleaner to save string
 * but was not worth the extra time
 * 
 * @tparam T 
 */

template<typename T> struct PGMWrap;
    
namespace pgmtools{    

    template<bool V,typename T,typename F> struct select{ typedef T type; };
    template<typename T,typename F> struct select<false,T,F>{ typedef F type; };
    
    template<typename T>
    struct PGMMultiByte{
        T PGMRead(const PGMWrap<T> *data){
            T ret;
            memcpy_P(&ret, data, sizeof(T));
            return ret;
        }
    };

    template<typename T>
    struct PGMSingleByte{
        T PGMRead(const PGMWrap<T> *data){ 
            return pgm_read_byte(data); 
            }
    };

    struct CharPGMReader{
        const char* PGMRead(const char *data){
            char buffer[strlen_P(data)+1];
            //memcpy_P(&ret, data, sizeof(T));
            strcpy_P(buffer, data);
            return buffer;
        }
    };
}

    template<typename T>
    struct PGMWrap{
        
        #ifdef ISCPP11
            constexpr PGMWrap( const T &in ) : t(in) {}
        #endif
        
        typedef typename pgmtools::select<
            sizeof(T) == 1,
            pgmtools::PGMSingleByte<T>,
            pgmtools::PGMMultiByte<T>
        >::type PGMReader;
        operator const T() const { 
            return PGMReader().PGMRead(this); 
        }
        bool operator ==( const T &in ){ 
            return PGMReader().PGMRead(this) == in;
        }
        T t;
    };

    typedef const PGMWrap<char> int8_p;                   //char
    typedef int8_p char_p;                                //char
    typedef const PGMWrap<unsigned char> uint8_p;         //unsigned char
    typedef const PGMWrap<int> int16_p;                   //int
    typedef const PGMWrap<unsigned int> uint16_p;         //unsigned int
    typedef const PGMWrap<char16_t> char16_p;             //char16_t
    typedef const PGMWrap<long> int32_p;                  //long
    typedef const PGMWrap<unsigned long> uint32_p;        //unsigned long
    #ifdef ISCPP11
      typedef const PGMWrap<char32_t> char32_p;           //char32_t
    #endif
    typedef const PGMWrap<long long> int64_p;             //long long
    typedef const PGMWrap<unsigned long long> uint64_p;   //unsigned long long
    typedef const PGMWrap<bool> bool_p;                   //bool
    typedef const PGMWrap<float> float_p;                 //float
    typedef const PGMWrap<double> double_p;               //double
    typedef const PGMWrap<long double> long_double_p;     //long double
    typedef const PGMWrap<size_t> size_p;                 //size_t