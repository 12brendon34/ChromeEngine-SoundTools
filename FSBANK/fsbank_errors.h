#pragma once

#include "fsbank.h"

static const char* FSBank_ErrorString(FSBANK_RESULT result)
{
    switch (result)
    {
    case FSBANK_OK:                                 return "No errors.";
    case FSBANK_ERR_CACHE_CHUNKNOTFOUND:            return "An expected chunk is missing from the cache, perhaps try deleting cache files.";
    case FSBANK_ERR_CANCELLED:                      return "The build process was cancelled during compilation by the user.";
    case FSBANK_ERR_CANNOT_CONTINUE:                return "The build process cannot continue due to previously ignored errors.";
    case FSBANK_ERR_ENCODER:                        return "Encoder for chosen format has encountered an unexpected error.";
    case FSBANK_ERR_ENCODER_INIT:                   return "Encoder initialization failed.";
    case FSBANK_ERR_ENCODER_NOTSUPPORTED:           return "Encoder for chosen format is not supported on this platform.";
    case FSBANK_ERR_FILE_OS:                        return "An operating system based file error was encountered.";
    case FSBANK_ERR_FILE_NOTFOUND:                  return "A specified file could not be found.";
    case FSBANK_ERR_FMOD:                           return "Internal error from FMOD sub-system.";
    case FSBANK_ERR_INITIALIZED:                    return "Already initialized.";
    case FSBANK_ERR_INVALID_FORMAT:                 return "The format of the source file is invalid.";
    case FSBANK_ERR_INVALID_PARAM:                  return "An invalid parameter has been passed to this function.";
    case FSBANK_ERR_MEMORY:                         return "Run out of memory.";
    case FSBANK_ERR_UNINITIALIZED:                  return "Not initialized yet.";
    case FSBANK_ERR_WRITER_FORMAT:                  return "Chosen encode format is not supported by this FSB version.";
    case FSBANK_WARN_CANNOTLOOP:                    return "Source file is too short for seamless looping. Looping disabled.";
    case FSBANK_WARN_IGNORED_FILTERHIGHFREQ:        return "FSBANK_BUILD_FILTERHIGHFREQ flag ignored: feature only supported by XMA format.";
    case FSBANK_WARN_IGNORED_DISABLESEEKING:        return "FSBANK_BUILD_DISABLESEEKING flag ignored: feature only supported by XMA format.";
    case FSBANK_WARN_FORCED_DONTWRITENAMES:         return "FSBANK_BUILD_FSB5_DONTWRITENAMES flag forced: cannot write names when source is from memory.";
    case FSBANK_ERR_ENCODER_FILE_NOTFOUND:          return "External encoder dynamic library not found.";
    case FSBANK_ERR_ENCODER_FILE_BAD:               return "External encoder dynamic library could not be loaded, possibly incorrect binary format, incorrect architecture, or file corruption.";
    case FSBANK_WARN_IGNORED_ALIGN4K:               return "FSBANK_BUILD_ALIGN4K flag ignored: feature only supported by Opus, Vorbis, and FADPCM formats.";
    default:                                        return "Unknown error.";
    }
}
static const wchar_t* FSBank_WErrorString(FSBANK_RESULT result)
{
    switch (result)
    {
    case FSBANK_OK:                                 return L"No errors.";
    case FSBANK_ERR_CACHE_CHUNKNOTFOUND:            return L"An expected chunk is missing from the cache, perhaps try deleting cache files.";
    case FSBANK_ERR_CANCELLED:                      return L"The build process was cancelled during compilation by the user.";
    case FSBANK_ERR_CANNOT_CONTINUE:                return L"The build process cannot continue due to previously ignored errors.";
    case FSBANK_ERR_ENCODER:                        return L"Encoder for chosen format has encountered an unexpected error.";
    case FSBANK_ERR_ENCODER_INIT:                   return L"Encoder initialization failed.";
    case FSBANK_ERR_ENCODER_NOTSUPPORTED:           return L"Encoder for chosen format is not supported on this platform.";
    case FSBANK_ERR_FILE_OS:                        return L"An operating system based file error was encountered.";
    case FSBANK_ERR_FILE_NOTFOUND:                  return L"A specified file could not be found.";
    case FSBANK_ERR_FMOD:                           return L"Internal error from FMOD sub-system.";
    case FSBANK_ERR_INITIALIZED:                    return L"Already initialized.";
    case FSBANK_ERR_INVALID_FORMAT:                 return L"The format of the source file is invalid.";
    case FSBANK_ERR_INVALID_PARAM:                  return L"An invalid parameter has been passed to this function.";
    case FSBANK_ERR_MEMORY:                         return L"Run out of memory.";
    case FSBANK_ERR_UNINITIALIZED:                  return L"Not initialized yet.";
    case FSBANK_ERR_WRITER_FORMAT:                  return L"Chosen encode format is not supported by this FSB version.";
    case FSBANK_WARN_CANNOTLOOP:                    return L"Source file is too short for seamless looping. Looping disabled.";
    case FSBANK_WARN_IGNORED_FILTERHIGHFREQ:        return L"FSBANK_BUILD_FILTERHIGHFREQ flag ignored: feature only supported by XMA format.";
    case FSBANK_WARN_IGNORED_DISABLESEEKING:        return L"FSBANK_BUILD_DISABLESEEKING flag ignored: feature only supported by XMA format.";
    case FSBANK_WARN_FORCED_DONTWRITENAMES:         return L"FSBANK_BUILD_FSB5_DONTWRITENAMES flag forced: cannot write names when source is from memory.";
    case FSBANK_ERR_ENCODER_FILE_NOTFOUND:          return L"External encoder dynamic library not found.";
    case FSBANK_ERR_ENCODER_FILE_BAD:               return L"External encoder dynamic library could not be loaded, possibly incorrect binary format, incorrect architecture, or file corruption.";
    case FSBANK_WARN_IGNORED_ALIGN4K:               return L"FSBANK_BUILD_ALIGN4K flag ignored: feature only supported by Opus, Vorbis, and FADPCM formats.";
    default:                                        return L"Unknown error.";
    }
}

