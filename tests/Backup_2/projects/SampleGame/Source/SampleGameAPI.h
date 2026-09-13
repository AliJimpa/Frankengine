#pragma once

// ============================================================
//  SampleGame's own API macro - exactly like an Unreal plugin's
//  generated <MODULE>_API. This macro name (SAMPLEGAME_API) and
//  its guard (SAMPLEGAME_EXPORTS) belong ONLY to this project.
//  Any other project defines its own equivalent pair with its
//  own name - never reuse this one for a different project.
//
//  SampleGame.cpp #defines SAMPLEGAME_EXPORTS before including
//  this header, which is what makes THAT file the exporting side.
//  Nothing else in the engine ever defines SAMPLEGAME_EXPORTS, so
//  everywhere else this resolves to dllimport by default.
// ============================================================

#ifdef SAMPLEGAME_EXPORTS
    #define SAMPLEGAME_API __declspec(dllexport)
#else
    #define SAMPLEGAME_API __declspec(dllimport)
#endif