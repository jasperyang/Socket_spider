//
// Created by Jacob Timber on 11/18/16.
//

#include "Async_pirnt.h"

Async_print* Async_print::ac = NULL;


void Async_print::print(string str) {
    if (control == 1) {
        mtx.lock();
        cout<<str<<endl;
        mtx.unlock();
    } else {
        //nothing
    }
}

