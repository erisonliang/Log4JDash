﻿#include <string.h>
#include <vector>
#include <stdio.h>
#include <rapidxml\rapidxml.hpp>
#include <Windows.h>
extern "C"
{
#include "Log4JParserC.h"
}

const char LogFragmentStart_[] = "<log4j:event";

const char TagEvent_[] = "log4j:event";
const size_t TagEventSize_ = sizeof TagEvent_ / sizeof TagEvent_[0] - 1U;

const char AttrLevel_[] = "level";
const size_t AttrLevelSize_ = sizeof AttrLevel_ / sizeof AttrLevel_[0] - 1U;

const char AttrLogger_[] = "logger";
const size_t AttrLoggerSize_ = sizeof AttrLogger_ / sizeof AttrLogger_[0] - 1U;

const char AttrThread_[] = "thread";
const size_t AttrThreadSize_ = sizeof AttrThread_ / sizeof AttrThread_[0] - 1U;

const char AttrTimestamp_[] = "timestamp";
const size_t AttrTimestampSize_ = sizeof AttrTimestamp_ / sizeof AttrTimestamp_[0] - 1U;

const char TagMessage_[] = "log4j:message";
const size_t TagMessageSize_ = sizeof TagMessage_ / sizeof TagMessage_[0] - 1U;

const char TagThrowable_[] = "log4j:throwable";
const size_t TagThrowableSize_ = sizeof TagThrowable_ / sizeof TagThrowable_[0] - 1U;

const char TagProperties_[] = "log4j:properties";
const size_t TagPropertiesSize_ = sizeof TagProperties_ / sizeof TagProperties_[0] - 1U;

const char TagData_[] = "log4j:data";
const size_t TagDataSize_ = sizeof TagData_ / sizeof TagData_[0] - 1U;

const char AttrDataName_[] = "name";
const size_t AttrDataNameSize_ = sizeof AttrDataName_ / sizeof AttrDataName_[0] - 1U;

const char AttrDataValue_[] = "value";
const size_t AttrDataValueSize_ = sizeof AttrDataValue_ / sizeof AttrDataValue_[0] - 1U;

static void GetAttributeValue_ (const rapidxml::xml_attribute<char> *source, const char **value, size_t *size);
static void GetNodeValue_ (const rapidxml::xml_node<char> *source, const char **value, size_t *size);
static int64_t ParseTimestamp_ (const char *value, const size_t valueSize);

LOG4JPARSERC_API void __cdecl Log4JEventLevel (const Log4JEvent log4JEvent, const char **value, size_t *size)
{
    auto node = (rapidxml::xml_node<char> *) log4JEvent;
    auto xml = node->first_attribute (AttrLevel_, AttrLevelSize_);
    GetAttributeValue_ (xml, value, size);
}

LOG4JPARSERC_API void __cdecl Log4JEventLogger (const Log4JEvent log4JEvent, const char **value, size_t *size)
{
    auto node = (rapidxml::xml_node<char> *) log4JEvent;
    auto xml = node->first_attribute (AttrLogger_, AttrLoggerSize_);
    GetAttributeValue_ (xml, value, size);
}

LOG4JPARSERC_API void __cdecl Log4JEventThread (const Log4JEvent log4JEvent, const char **value, size_t *size)
{
    auto node = (rapidxml::xml_node<char> *) log4JEvent;
    auto xml = node->first_attribute (AttrThread_, AttrThreadSize_);
    GetAttributeValue_ (xml, value, size);
}

LOG4JPARSERC_API int64_t __cdecl Log4JEventTimestamp (const Log4JEvent log4JEvent)
{
    auto node = (rapidxml::xml_node<char> *) log4JEvent;
    auto xml = node->first_attribute (AttrTimestamp_, AttrTimestampSize_);
    auto result = ParseTimestamp_ (xml->value (), xml->value_size ());

    return result;
}

LOG4JPARSERC_API void __cdecl Log4JEventMessage (const Log4JEvent log4JEvent, const char **value, size_t *size)
{
    auto node = (rapidxml::xml_node<char> *) log4JEvent;
    auto xml = node->first_node (TagMessage_, TagMessageSize_);
    GetNodeValue_ (xml, value, size);
}

LOG4JPARSERC_API void __cdecl Log4JEventThrowable (const Log4JEvent log4JEvent, const char **value, size_t *size)
{
    auto node = (rapidxml::xml_node<char> *) log4JEvent;
    auto xml = node->first_node (TagThrowable_, TagThrowableSize_);
    GetNodeValue_ (xml, value, size);
}

LOG4JPARSERC_API size_t Log4JEventProperties (const Log4JEvent log4JEvent, size_t skip, Log4JEventProperty *properties, size_t propertiesSize)
{
    if (properties == nullptr)
    {
        propertiesSize = 0U;
    }
    size_t actualProperties = 0U;

    auto node = (rapidxml::xml_node<char> *) log4JEvent;
    auto propertiesNode = node->first_node (TagProperties_, TagPropertiesSize_);
    if (propertiesNode != nullptr)
    {
        auto dataNode = propertiesNode->first_node (TagData_, TagDataSize_);
        while (dataNode != nullptr)
        {
            ++actualProperties;

            if (skip != 0U)
            {
                --skip;
            }
            else if (propertiesSize != 0U)
            {
                auto name = dataNode->first_attribute (AttrDataName_, AttrDataNameSize_);
                GetAttributeValue_ (name, &properties->name, &properties->nameSize);

                auto value = dataNode->first_attribute (AttrDataValue_, AttrDataValueSize_);
                GetAttributeValue_ (value, &properties->value, &properties->valueSize);

                ++properties;
                --propertiesSize;
            }

            dataNode = dataNode->next_sibling (TagData_, TagDataSize_);
        }
    }

    return actualProperties;
}

void GetAttributeValue_ (const rapidxml::xml_attribute<char> *source, const char **value, size_t *size)
{
    if (source)
    {
        *value = source->value ();
        *size = source->value_size ();
    }
    else
    {
        *value = nullptr;
        *size = 0U;
    }
}

void GetNodeValue_ (const rapidxml::xml_node<char> *source, const char **value, size_t *size)
{
    if (source)
    {
        auto firstChild = source->first_node ();
        auto isCDataContainer = firstChild &&
                                !firstChild->next_sibling () &&
                                firstChild->type () == rapidxml::node_type::node_cdata;
        if (isCDataContainer)
        {
            *value = firstChild->value ();
            *size = firstChild->value_size ();
        }
        else
        {
            *value = source->value ();
            *size = source->value_size ();
        }
    }
    else
    {
        *value = nullptr;
        *size = 0U;
    }
}

int64_t ParseTimestamp_ (const char *value, const size_t valueSize)
{
    int64_t result = 0L;

    for (size_t i = 0; i < valueSize; ++i)
    {
        int64_t digit = value[i] - '0';
        result = result * 10L + digit;
    }

    return result;
}

struct Log4JEventSource_
{
    std::vector<const rapidxml::xml_document<char> *> *Docs;
    char *OwnXmlString;
};

static Log4JStatus Log4JEventSourceInitXmlStringImpl (Log4JEventSource **self, char *xmlString, bool ownString)
{
    *self = nullptr;
    auto ownedStringPtr = ownString ? xmlString : nullptr;

    auto status = Log4JStatus::E_SUCCESS;

    auto nextPart = xmlString;
    auto docs = new std::vector<const rapidxml::xml_document<char> *> ();
    while (nextPart)
    {
        auto doc = new rapidxml::xml_document<char> ();
        docs->push_back (doc);
        try
        {
            doc->parse<rapidxml::parse_default> (nextPart);
            nextPart = nullptr;
        }
        catch (const rapidxml::parse_error &ex)
        {
            status = Log4JStatus::E_DOCUMENT_ERRORS;
            nextPart = strstr (ex.where<char> (), LogFragmentStart_);
        }
        catch (...)
        {
            for (auto docsIter = docs->begin (); docsIter != docs->end (); ++docsIter)
            {
                delete *docsIter;
            }
            throw;
        }
    }

    Log4JEventSource *result = (Log4JEventSource *) malloc (sizeof *result);
    if (result == nullptr)
    {
        for (auto docsIter = docs->begin (); docsIter != docs->end (); ++docsIter)
        {
            delete *docsIter;
        }
        *self = nullptr;
        return Log4JStatus::E_MEMORY_ERROR;
    }
    *result = { docs, ownedStringPtr };

    *self = result;

    return status;
}

LOG4JPARSERC_API Log4JStatus __cdecl Log4JEventSourceInitXmlString (Log4JEventSource **self, char *xmlString)
{
    return Log4JEventSourceInitXmlStringImpl (self, xmlString, false);
}

LOG4JPARSERC_API void __cdecl Log4JEventSourceDestroy (Log4JEventSource *self)
{
    auto docs = self->Docs;
    for (auto docsIter = docs->begin (); docsIter != docs->end (); ++docsIter)
    {
        delete *docsIter;
    }
    docs->clear ();
    delete docs;

    if (self->OwnXmlString)
    {
        free (self->OwnXmlString);
    }

    *self = { nullptr, nullptr };
    free (self);
}

static size_t IndexOfCurrentDocument (const std::vector<const rapidxml::xml_document<char> *> *docs, const rapidxml::xml_node<char> *node)
{
    auto currentDocument = node->document ();
    auto docsSize = docs->size ();
    for (size_t i = 0; i < docsSize; ++i)
    {
        if (docs->at (i) == currentDocument)
        {
            return i;
        }
    }

    return docsSize;
}

LOG4JPARSERC_API Log4JEvent __cdecl Log4JEventSourceFirst (const Log4JEventSource *self)
{
    auto node = self->Docs->front ()->first_node (TagEvent_, TagEventSize_);
    return node;
}

LOG4JPARSERC_API Log4JEvent __cdecl Log4JEventSourceNext (const Log4JEventSource *self, const Log4JEvent event)
{
    auto docs = self->Docs;
    auto node = (rapidxml::xml_node<char> *) event;

    auto nextNode = node->next_sibling (TagEvent_, TagEventSize_);
    if (!nextNode)
    {
        auto currentDocumentIndex = IndexOfCurrentDocument (self->Docs, node);
        auto nextDocumentIndex = currentDocumentIndex + 1;
        if (nextDocumentIndex < docs->size ())
        {
            nextNode = docs->at (nextDocumentIndex)->first_node (TagEvent_, TagEventSize_);
        }
    }

    return nextNode;
}

LOG4JPARSERC_API Log4JEvent __cdecl Log4JEventSourceLast (const Log4JEventSource *self)
{
    auto node = self->Docs->back ()->last_node (TagEvent_, TagEventSize_);
    return node;
}

LOG4JPARSERC_API Log4JEvent __cdecl Log4JEventSourcePrev (const Log4JEventSource *self, const Log4JEvent event)
{
    auto docs = self->Docs;
    auto node = (rapidxml::xml_node<char> *) event;

    auto prevNode = node->previous_sibling (TagEvent_, TagEventSize_);
    if (!prevNode)
    {
        auto currentDocumentIndex = IndexOfCurrentDocument (self->Docs, node);
        auto prevDocumentIndex = currentDocumentIndex - 1;
        // if currentDocumentIndex == 0 then prevDocumentIndex would overflow to SIZE_MAX
        if (prevDocumentIndex < docs->size ())
        {
            prevNode = docs->at (prevDocumentIndex)->first_node (TagEvent_, TagEventSize_);
        }
    }

    return prevNode;
}
