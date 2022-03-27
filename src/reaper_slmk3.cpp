#define REAPERAPI_IMPLEMENT
#include <iostream>
#include <string>
#include "reaper/reaper_plugin.h"
#include "reaper/reaper_plugin_functions.h"

#define LOCALIZE_IMPORT_PREFIX "csurf_"
#include "reaper/localize-import.h"
#include "reaper/csurf.h"
#include "WDL/setthreadname.h"

HWND g_hwnd;

void debug(std::string message) {
  std::cout << "\n[DEBUG] " + message + "\n";
}

extern reaper_csurf_reg_t csurf_novation_slmk3_reg;

void (*VkbStuffMessage)(MIDI_event_t *evt, bool wantCurChan);
MediaTrack* (*GetLastTouchedTrack)();
const char* (*GetOscCfgDir)();
bool (*WDL_ChooseDirectory)(HWND parent, const char *text, const char *initialdir, char *fn, int fnsize, bool preservecwd);
char* (*WDL_ChooseFileForOpen)(HWND, const char*, const char*, const char*, const char*, const char*, bool, bool, const char*, void*,
#ifdef _WIN32
  HINSTANCE hInstance
#else
 struct SWELL_DialogResourceIndex*
#endif
);

int (*Plugin_Register)(const char *name, void *infostruct);
MediaTrack *(*GetTrack)(ReaProject *proj, int tridx);

extern "C" {
REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *rec) {

  if (rec != nullptr) {
    debug("Plugin entrypoint executed.");
    g_hwnd = rec->hwnd_main;
    int errcnt = 0;
#define IMPAPI(x) if (!((*((void **)&(x)) = (void *)rec->GetFunc(#x)))) errcnt++;

    debug("Plugin entrypoint executed.");

    IMPAPI(update_disk_counters)
    IMPAPI(DB2SLIDER)
    IMPAPI(SLIDER2DB)
    IMPAPI(GetNumMIDIInputs)
    IMPAPI(GetNumMIDIOutputs)
    IMPAPI(CreateMIDIInput)
    IMPAPI(CreateMIDIOutput)
    IMPAPI(GetMIDIOutputName)
    IMPAPI(GetMIDIInputName)
    IMPAPI(CSurf_TrackToID)
    IMPAPI(CSurf_TrackFromID)
    IMPAPI(CSurf_NumTracks)
    IMPAPI(CSurf_SetTrackListChange)
    IMPAPI(CSurf_SetSurfaceVolume)
    IMPAPI(CSurf_SetSurfacePan)
    IMPAPI(CSurf_SetSurfaceMute)
    IMPAPI(CSurf_SetSurfaceSelected)
    IMPAPI(CSurf_SetSurfaceSolo)
    IMPAPI(CSurf_SetSurfaceRecArm)
    IMPAPI(CSurf_GetTouchState)
    IMPAPI(CSurf_SetAutoMode)
    IMPAPI(CSurf_SetPlayState)
    IMPAPI(CSurf_SetRepeatState)
    IMPAPI(CSurf_OnVolumeChange)
    IMPAPI(CSurf_OnPanChange)
    IMPAPI(CSurf_OnWidthChange)
    IMPAPI(CSurf_OnVolumeChangeEx)
    IMPAPI(CSurf_OnPanChangeEx)
    IMPAPI(CSurf_OnWidthChangeEx)
    IMPAPI(CSurf_OnMuteChange)
    IMPAPI(CSurf_OnMuteChangeEx)
    IMPAPI(CSurf_OnSelectedChange)
    IMPAPI(CSurf_OnSoloChange)
    IMPAPI(CSurf_OnSoloChangeEx)
    IMPAPI(CSurf_OnFXChange)
    IMPAPI(CSurf_OnRecArmChange)
    IMPAPI(CSurf_OnRecArmChangeEx)
    IMPAPI(CSurf_OnInputMonitorChange)
    IMPAPI(CSurf_OnInputMonitorChangeEx)
    IMPAPI(CSurf_OnPlay)
    IMPAPI(CSurf_OnStop)
    IMPAPI(CSurf_OnPause)
    IMPAPI(CSurf_OnRewFwd)
    IMPAPI(CSurf_OnRecord)
    IMPAPI(CSurf_GoStart)
    IMPAPI(CSurf_GoEnd)
    IMPAPI(CSurf_OnArrow)
    IMPAPI(CSurf_OnScroll)
    IMPAPI(CSurf_OnZoom)
    IMPAPI(CSurf_OnTrackSelection)
    IMPAPI(CSurf_ResetAllCachedVolPanStates)
    IMPAPI(CSurf_ScrubAmt)
    IMPAPI(CSurf_OnSendVolumeChange)
    IMPAPI(CSurf_OnSendPanChange)
    IMPAPI(CSurf_OnRecvVolumeChange)
    IMPAPI(CSurf_OnRecvPanChange)
    IMPAPI(CSurf_OnPlayRateChange)
    IMPAPI(CSurf_OnTempoChange)
    IMPAPI(CSurf_OnOscControlMessage)

    IMPAPI(Master_GetPlayRate)
    IMPAPI(Master_NormalizePlayRate)
    IMPAPI(Master_GetTempo)
    IMPAPI(Master_NormalizeTempo)

    IMPAPI(VkbStuffMessage)

    IMPAPI(TrackList_UpdateAllExternalSurfaces)
    IMPAPI(kbd_OnMidiEvent)
    IMPAPI(GetMasterMuteSoloFlags)
    IMPAPI(ClearAllRecArmed)
    IMPAPI(SetTrackAutomationMode)
    IMPAPI(GetTrackAutomationMode)
    IMPAPI(AnyTrackSolo)
    IMPAPI(SoloAllTracks)
    IMPAPI(MuteAllTracks)
    IMPAPI(BypassFxAllTracks)
    IMPAPI(GetTrackInfo)
    IMPAPI(GetSetMediaTrackInfo)
    IMPAPI(GetMasterTrackVisibility)
    IMPAPI(SetMixerScroll)
    IMPAPI(GetMixerScroll)
    IMPAPI(IsTrackSelected)
    IMPAPI(SetTrackSelected)
    IMPAPI(SetOnlyTrackSelected)
    IMPAPI(SetAutomationMode)
    IMPAPI(UpdateTimeline)
    IMPAPI(Main_UpdateLoopInfo)
    IMPAPI(Loop_OnArrow)
    IMPAPI(GetPlayState)
    IMPAPI(GetPlayPosition)
    IMPAPI(GetCursorPosition)
    IMPAPI(format_timestr_pos)
    IMPAPI(TimeMap2_timeToBeats)
    IMPAPI(Track_GetPeakInfo)
    IMPAPI(Track_GetPeakHoldDB)
    IMPAPI(SetProjectMarkerByIndex2)
    IMPAPI(AddProjectMarker2)
    IMPAPI(GetTrackUIVolPan)
    IMPAPI(GetTrackUIPan)
    IMPAPI(GetTrackSendUIVolPan)
    IMPAPI(GetTrackSendName)
    IMPAPI(GetTrackReceiveUIVolPan)
    IMPAPI(GetTrackReceiveName)
    IMPAPI(GetSetRepeat)
    IMPAPI(GoToMarker)
    IMPAPI(GoToRegion)
    IMPAPI(EnumProjectMarkers3)
    IMPAPI(CountProjectMarkers)
    IMPAPI(GetLastMarkerAndCurRegion)
    IMPAPI(mkvolpanstr)
    IMPAPI(mkvolstr)
    IMPAPI(mkpanstr)
    IMPAPI(MoveEditCursor)
    IMPAPI(adjustZoom)
    IMPAPI(GetHZoomLevel)
    IMPAPI(Main_OnCommand)
    IMPAPI(KBD_OnMainActionEx)
    IMPAPI(kbd_pushPopSoftTakeover)
    IMPAPI(NamedCommandLookup)
    IMPAPI(GetMainHwnd)
    IMPAPI(GetToggleCommandState)
    IMPAPI(MIDIEditor_LastFocused_OnCommand)

    IMPAPI(TrackFX_GetCount)
    IMPAPI(TrackFX_GetRecCount)
    IMPAPI(TrackFX_GetInstrument)
    IMPAPI(TrackFX_GetEQ)
    IMPAPI(TrackFX_SetEQParam)
    IMPAPI(TrackFX_GetEQParam)
    IMPAPI(TrackFX_SetEQBandEnabled)
    IMPAPI(TrackFX_GetEQBandEnabled)

    IMPAPI(TrackFX_GetNumParams)
    IMPAPI(TrackFX_GetParam)
    IMPAPI(TrackFX_GetParamEx)
    IMPAPI(TrackFX_SetParam)
    IMPAPI(TrackFX_EndParamEdit)
    IMPAPI(TrackFX_GetParamName)
    IMPAPI(TrackFX_GetFormattedParamValue)
    IMPAPI(TrackFX_GetParamFromIdent)

    IMPAPI(TrackFX_GetParamNormalized)
    IMPAPI(TrackFX_SetParamNormalized)

    IMPAPI(TrackFX_GetFXName)
    IMPAPI(TrackFX_NavigatePresets)
    IMPAPI(TrackFX_GetPreset)
    IMPAPI(TrackFX_SetPreset)

    IMPAPI(TrackFX_GetChainVisible)
    IMPAPI(TrackFX_GetFloatingWindow)
    IMPAPI(TrackFX_Show)
    IMPAPI(TrackFX_GetEnabled)
    IMPAPI(TrackFX_SetEnabled)
    IMPAPI(TrackFX_GetOpen)
    IMPAPI(TrackFX_SetOpen)

    IMPAPI(GetLastTouchedTrack)
    IMPAPI(GetLastTouchedFX)
    IMPAPI(GetFocusedFX)

    IMPAPI(TimeMap2_beatsToTime)
    IMPAPI(parse_timestr_pos)
    IMPAPI(SetEditCurPos)

    IMPAPI(GetTrackGUID)
    IMPAPI(GetOscCfgDir)
    IMPAPI(get_ini_file)
    IMPAPI(GetResourcePath)
    IMPAPI(GetAppVersion)
    IMPAPI(RecursiveCreateDirectory)
    IMPAPI(WDL_ChooseFileForOpen)
    IMPAPI(WDL_ChooseDirectory)

    IMPAPI(GetNumTracks)
    IMPAPI(format_timestr)
    IMPAPI(guidToString)
    IMPAPI(Undo_OnStateChangeEx)
    IMPAPI(Undo_BeginBlock)
    IMPAPI(Undo_EndBlock)
    IMPAPI(CSurf_FlushUndo)
    IMPAPI(ToggleTrackSendUIMute)
    IMPAPI(GetTrackSendUIMute)
    IMPAPI(GetTrackReceiveUIMute)
    IMPAPI(SetTrackSendUIPan)
    IMPAPI(SetTrackSendUIVol)
    IMPAPI(GetSetTrackSendInfo)
    IMPAPI(GetTrackNumSends)

    IMPAPI(GetTrack)
    IMPAPI(GetTrackMIDILyrics)
    IMPAPI(GetProjExtState)
    IMPAPI(GetExtState)
    IMPAPI(SetProjExtState)
    IMPAPI(SetExtState)
    IMPAPI(GetSet_LoopTimeRange2)

    IMPAPI(GetTrackName)
    IMPAPI(TrackFX_GetParameterStepSizes)

    rec->Register("csurf", &csurf_novation_slmk3_reg);
    //rec->Register("custom_action", &)

    debug("Returned 1.");
    return 1;
  } else {
    debug("Extension unloaded");
    return 0;
  }
}
}

#ifndef _WIN32 // MAC resources
#include "WDL/swell/swell-dlggen.h"
#include "reaper/res.rc_mac_dlg"
#undef BEGIN
#undef END
#include "WDL/swell/swell-menugen.h"
#include "reaper/res.rc_mac_menu"
#endif


midi_Output *CreateThreadedMIDIOutput(midi_Output *output) {
  if (!output) {
    return output;
  }
  return new threadedMIDIOutput(output);
}