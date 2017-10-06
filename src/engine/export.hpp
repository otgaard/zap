//
// Created by Darren Otgaar on 2016/04/18.
//

#ifndef ZAP_EXPORT_HPP
#define ZAP_EXPORT_HPP

#ifdef _WIN32
    #ifdef SHARED_BUILD
        #define EXPORT __declspec(dllexport)
    #elif STATIC_BUILD
        #define EXPORT
    #else SHARED_IMPORT
        #define EXPORT __declspec(dllimport)
    #endif
#else
    #define EXPORT
#endif

#endif //ZAP_EXPORT_HPP
