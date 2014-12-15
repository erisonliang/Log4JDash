#pragma once

#include "Log4JEvent.h"
#include "Log4JFilter.h"

typedef struct Log4JIterator_ Log4JIterator;

void Log4JIteratorDestroy (Log4JIterator *self);

bool Log4JIteratorMoveNext (Log4JIterator *self);

const Log4JEvent Log4JIteratorCurrent (const Log4JIterator *self);

void Log4JIteratorInitEventSource (Log4JIterator **self, const Log4JEventSource *source);

void Log4JIteratorInitFilter (Log4JIterator **self, Log4JIterator *inner, const Log4JFilter *filter);
