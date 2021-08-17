// Options:
#define PICO_DSP true
/* ------------------------------------------------------------
name: "SNESverb"
Code generated with Faust 2.33.1 (https://faust.grame.fr)
Compilation options: -a /usr/local/share/faust/esp32/esp32.cpp -lang cpp -es 1 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

/************************************************************************
 IMPORTANT NOTE : this file contains two clearly delimited sections :
 the ARCHITECTURE section (in two parts) and the USER section. Each section
 is governed by its own copyright and license. Please check individually
 each section for license and copyright information.
 *************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2019-2020 GRAME, Centre National de Creation Musicale &
 Aalborg University (Copenhagen, Denmark)
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 
 ************************************************************************
 ************************************************************************/

#include "SNESverb.h"

/************************** BEGIN meta.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __meta__
#define __meta__

/**
 The base class of Meta handler to be used in dsp::metadata(Meta* m) method to retrieve (key, value) metadata.
 */
struct Meta
{
    virtual ~Meta() {};
    virtual void declare(const char* key, const char* value) = 0;
};

#endif
/**************************  END  meta.h **************************/
/************************** BEGIN dsp.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>
#include <vector>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'openTabBox', 'addButton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Return the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void init(int sample_rate) = 0;

        /**
         * Init instance state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceInit(int sample_rate) = 0;
    
        /**
         * Init instance constant state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceConstants(int sample_rate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (like delay lines...) but keep the control parameter values */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with successive in/out audio buffers.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         *
         */
        virtual void compute(double /*date_usec*/, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = nullptr):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int sample_rate) { fDSP->init(sample_rate); }
        virtual void instanceInit(int sample_rate) { fDSP->instanceInit(sample_rate); }
        virtual void instanceConstants(int sample_rate) { fDSP->instanceConstants(sample_rate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class, used with LLVM and Interpreter backends
 * to create DSP instances from a compiled DSP program.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
        virtual std::string getCompileOptions() = 0;
        virtual std::vector<std::string> getLibraryList() = 0;
        virtual std::vector<std::string> getIncludePathnames() = 0;
    
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/************************** END dsp.h **************************/
/************************** BEGIN MapUI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_MAPUI_H
#define FAUST_MAPUI_H

#include <vector>
#include <map>
#include <string>

/************************** BEGIN UI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2020 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __UI_H__
#define __UI_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

template <typename REAL>
struct UIReal
{
    UIReal() {}
    virtual ~UIReal() {}
    
    // -- widget's layouts
    
    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;
    
    // -- active widgets
    
    virtual void addButton(const char* label, REAL* zone) = 0;
    virtual void addCheckButton(const char* label, REAL* zone) = 0;
    virtual void addVerticalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addHorizontalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addNumEntry(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    
    // -- passive widgets
    
    virtual void addHorizontalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    virtual void addVerticalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    
    // -- soundfiles
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;
    
    // -- metadata declarations
    
    virtual void declare(REAL* zone, const char* key, const char* val) {}
    
    // To be used by LLVM client
    virtual int sizeOfFAUSTFLOAT() { return sizeof(FAUSTFLOAT); }
};

struct UI : public UIReal<FAUSTFLOAT>
{
    UI() {}
    virtual ~UI() {}
};

#endif
/**************************  END  UI.h **************************/
/************************** BEGIN PathBuilder.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_PATHBUILDER_H
#define FAUST_PATHBUILDER_H

#include <vector>
#include <string>
#include <algorithm>

/*******************************************************************************
 * PathBuilder : Faust User Interface
 * Helper class to build complete hierarchical path for UI items.
 ******************************************************************************/

class PathBuilder
{

    protected:
    
        std::vector<std::string> fControlsLevel;
       
    public:
    
        PathBuilder() {}
        virtual ~PathBuilder() {}
    
        std::string buildPath(const std::string& label) 
        {
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res += fControlsLevel[i];
                res += "/";
            }
            res += label;
            std::replace(res.begin(), res.end(), ' ', '_');
            return res;
        }
    
        std::string buildLabel(std::string label)
        {
            std::replace(label.begin(), label.end(), ' ', '_');
            return label;
        }
    
        void pushLabel(const std::string& label) { fControlsLevel.push_back(label); }
        void popLabel() { fControlsLevel.pop_back(); }
    
};

#endif  // FAUST_PATHBUILDER_H
/**************************  END  PathBuilder.h **************************/

/*******************************************************************************
 * MapUI : Faust User Interface
 * This class creates a map of complete hierarchical path and zones for each UI items.
 ******************************************************************************/

class MapUI : public UI, public PathBuilder
{
    
    protected:
    
        // Complete path map
        std::map<std::string, FAUSTFLOAT*> fPathZoneMap;
    
        // Label zone map
        std::map<std::string, FAUSTFLOAT*> fLabelZoneMap;
    
        std::string fNullStr = "";
    
    public:
        
        MapUI() {}
        virtual ~MapUI() {}
        
        // -- widget's layouts
        void openTabBox(const char* label)
        {
            pushLabel(label);
        }
        void openHorizontalBox(const char* label)
        {
            pushLabel(label);
        }
        void openVerticalBox(const char* label)
        {
            pushLabel(label);
        }
        void closeBox()
        {
            popLabel();
        }
        
        // -- active widgets
        void addButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        
        // -- passive widgets
        void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
    
        // -- soundfiles
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
        
        // -- metadata declarations
        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {}
        
        // set/get
        void setParamValue(const std::string& path, FAUSTFLOAT value)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                *fPathZoneMap[path] = value;
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                *fLabelZoneMap[path] = value;
            }
        }
        
        FAUSTFLOAT getParamValue(const std::string& path)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                return *fPathZoneMap[path];
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                return *fLabelZoneMap[path];
            } else {
                return FAUSTFLOAT(0);
            }
        }
    
        // map access 
        std::map<std::string, FAUSTFLOAT*>& getMap() { return fPathZoneMap; }
        
        int getParamsCount() { return int(fPathZoneMap.size()); }
        
        const std::string& getParamAddress(int index)
        {
            if (index < 0 || index > int(fPathZoneMap.size())) {
                return fNullStr;
            } else {
                auto it = fPathZoneMap.begin();
                while (index-- > 0 && it++ != fPathZoneMap.end()) {}
                return it->first;
            }
        }
    
        const std::string& getParamAddress(FAUSTFLOAT* zone)
        {
            for (const auto& it : fPathZoneMap) {
                if (it.second == zone) return it.first;
            }
            return fNullStr;
        }
    
        FAUSTFLOAT* getParamZone(const std::string& str)
        {
            if (fPathZoneMap.find(str) != fPathZoneMap.end()) {
                return fPathZoneMap[str];
            }
            if (fLabelZoneMap.find(str) != fLabelZoneMap.end()) {
                return fLabelZoneMap[str];
            }
            return nullptr;
        }
    
        FAUSTFLOAT* getParamZone(int index)
        {
            if (index < 0 || index > int(fPathZoneMap.size())) {
                return nullptr;
            } else {
                auto it = fPathZoneMap.begin();
                while (index-- > 0 && it++ != fPathZoneMap.end()) {}
                return it->second;
            }
        }
    
        static bool endsWith(const std::string& str, const std::string& end)
        {
            size_t l1 = str.length();
            size_t l2 = end.length();
            return (l1 >= l2) && (0 == str.compare(l1 - l2, l2, end));
        }
};


#endif // FAUST_MAPUI_H
/**************************  END  MapUI.h **************************/
/************************** BEGIN esp32audio.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2020 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __esp32audio__
#define __esp32audio__

#include <utility>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"

/************************** BEGIN audio.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __audio__
#define __audio__

#include <set>
#include <utility>

class dsp;

typedef void (* shutdown_callback)(const char* message, void* arg);

typedef void (* compute_callback)(void* arg);

class audio {
    
    protected:
    
        shutdown_callback fShutdown;    // Shutdown callback
        void* fShutdownArg;             // Shutdown callback data
    
        std::set<std::pair<compute_callback, void*> > fComputeCallbackList;
    
    public:
    
        audio():fShutdown(nullptr), fShutdownArg(nullptr) {}
        virtual ~audio() {}

        /**
         * Init the DSP.
         * @param name - the DSP name to be given to the audio driven
         * (could appear as a JACK client for instance)
         * @param dsp - the dsp that will be initialized with the driver sample rate
         *
         * @return true is sucessful, false in case of driver failure.
         **/
        virtual bool init(const char* name, dsp* dsp) = 0;
    
        /**
         * Start audio processing.
         * @return true is sucessful, false if case of driver failure.
         **/
        virtual bool start() = 0;
    
        /**
         * Stop audio processing.
         **/
        virtual void stop() = 0;
    
        void setShutdownCallback(shutdown_callback cb, void* arg)
        {
            fShutdown = cb;
            fShutdownArg = arg;
        }
    
        void addControlCallback(compute_callback cb, void* arg)
        {
            fComputeCallbackList.insert(std::make_pair(cb, arg));
        }
    
        bool removeControlCallback(compute_callback cb, void* arg)
        {
            return (fComputeCallbackList.erase(std::make_pair(cb, arg)) == 1);
        }
    
        void runControlCallbacks()
        {
            for (const auto& it : fComputeCallbackList) {
                it.first(it.second);
            }
        }
    
        // Return buffer size in frames.
        virtual int getBufferSize() = 0;
    
        // Return the driver sample rate in Hz.
        virtual int getSampleRate() = 0;

        // Return the driver hardware inputs number.
        virtual int getNumInputs() = 0;
    
        // Return the driver hardware outputs number.
        virtual int getNumOutputs() = 0;
    
        /**
         * @return Returns the average proportion of available CPU
         * being spent inside the audio callbacks (between 0.0 and 1.0).
         **/
        virtual float getCPULoad() { return 0.f; }
};
					
#endif
/**************************  END  audio.h **************************/

#define MULT_S32 2147483647
#define DIV_S32 4.6566129e-10
#define clip(sample) std::max(-MULT_S32, std::min(MULT_S32, ((int32_t)(sample * MULT_S32))));

#define AUDIO_MAX_CHAN 2

class esp32audio : public audio {
    
    private:
    
        int fSampleRate;
        int fBufferSize;
        int fNumInputs;
        int fNumOutputs;
        float** fInChannel;
        float** fOutChannel;
        TaskHandle_t fHandle;
        dsp* fDSP;
        bool fRunning;
    
        template <int INPUTS, int OUTPUTS>
        void audioTask()
        {
            while (fRunning) {
                if (INPUTS > 0) {
                    // Read from the card
                    int32_t samples_data_in[AUDIO_MAX_CHAN*fBufferSize];
                    size_t bytes_read = 0;
                    i2s_read((i2s_port_t)1, &samples_data_in, AUDIO_MAX_CHAN*sizeof(float)*fBufferSize, &bytes_read, portMAX_DELAY);
                    
                    // Convert and copy inputs
                    if (INPUTS == AUDIO_MAX_CHAN) {
                        // if stereo
                        for (int i = 0; i < fBufferSize; i++) {
                            fInChannel[0][i] = (float)samples_data_in[i*AUDIO_MAX_CHAN]*DIV_S32;
                            fInChannel[1][i] = (float)samples_data_in[i*AUDIO_MAX_CHAN+1]*DIV_S32;
                        }
                    } else {
                        // otherwise only first channel
                        for (int i = 0; i < fBufferSize; i++) {
                            fInChannel[0][i] = (float)samples_data_in[i*AUDIO_MAX_CHAN]*DIV_S32;
                        }
                    }
                }
                
                // Call DSP
                fDSP->compute(fBufferSize, fInChannel, fOutChannel);
                
                // Convert and copy outputs
                int32_t samples_data_out[AUDIO_MAX_CHAN*fBufferSize];
                if (OUTPUTS == AUDIO_MAX_CHAN) {
                    // if stereo
                    for (int i = 0; i < fBufferSize; i++) {
                        samples_data_out[i*AUDIO_MAX_CHAN] = clip(fOutChannel[0][i]);
                        samples_data_out[i*AUDIO_MAX_CHAN+1] = clip(fOutChannel[1][i]);
                    }
                } else {
                    // otherwise only first channel
                    for (int i = 0; i < fBufferSize; i++) {
                        samples_data_out[i*AUDIO_MAX_CHAN] = clip(fOutChannel[0][i]);
                        samples_data_out[i*AUDIO_MAX_CHAN+1] = samples_data_out[i*AUDIO_MAX_CHAN];
                    }
                }
                
                // Write to the card
                size_t bytes_written = 0;
                i2s_write((i2s_port_t)1, &samples_data_out, AUDIO_MAX_CHAN*sizeof(float)*fBufferSize, &bytes_written, portMAX_DELAY);
            }
            
            // Task has to deleted itself beforee returning
            vTaskDelete(nullptr);
        }
    
        void destroy()
        {
            for (int i = 0; i < fNumInputs; i++) {
                delete[] fInChannel[i];
            }
            delete [] fInChannel;
            
            for (int i = 0; i < fNumOutputs; i++) {
                delete[] fOutChannel[i];
            }
            delete [] fOutChannel;
        }
    
        static void audioTaskHandler(void* arg)
        {
            esp32audio* audio = static_cast<esp32audio*>(arg);
            
            if (audio->fNumInputs == 0 && audio->fNumOutputs == 1) {
                audio->audioTask<0,1>();
            } else if (audio->fNumInputs == 0 && audio->fNumOutputs == 2) {
                audio->audioTask<0,2>();
            } else if (audio->fNumInputs == 1 && audio->fNumOutputs == 1) {
                audio->audioTask<1,1>();
            } else if (audio->fNumInputs == 1 && audio->fNumOutputs == 2) {
                audio->audioTask<1,2>();
            } else if (audio->fNumInputs == 2 && audio->fNumOutputs == 1) {
                audio->audioTask<2,1>();
            } else if (audio->fNumInputs == 2 && audio->fNumOutputs == 2) {
                audio->audioTask<2,2>();
            }
        }
    
    public:
    
        esp32audio(int srate, int bsize):
        fSampleRate(srate),
        fBufferSize(bsize),
        fNumInputs(0),
        fNumOutputs(0),
        fInChannel(nullptr),
        fOutChannel(nullptr),
        fHandle(nullptr),
        fDSP(nullptr),
        fRunning(false)
        {
            i2s_pin_config_t pin_config;
        #if TTGO_TAUDIO
            pin_config = {
                .bck_io_num = 33,
                .ws_io_num = 25,
                .data_out_num = 26,
                .data_in_num = 27
            };
        #elif A1S_BOARD
            pin_config = {
                .bck_io_num = 27,
                .ws_io_num = 26,
                .data_out_num = 25,
                .data_in_num = 35
            };
         #elif PICO_DSP
            pin_config = {
                .bck_io_num = 23,
                .ws_io_num = 25,
                .data_out_num = 26,
                .data_in_num = 27
            };
        #elif LYRA_T
            pin_config = {
                .bck_io_num = 5,
                .ws_io_num = 25,
                .data_out_num = 26,
                .data_in_num = 35
            };
        #else // Default
            pin_config = {
                .bck_io_num = 33,
                .ws_io_num = 25,
                .data_out_num = 26,
                .data_in_num = 27
            };
        #endif
            
        #if A1S_BOARD
            i2s_config_t i2s_config = {
                .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
                .sample_rate = fSampleRate,
                .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
                .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
                .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
                .intr_alloc_flags = ESP_INTR_FLAG_LEVEL3, // high interrupt priority
                .dma_buf_count = 3,
                .dma_buf_len = fBufferSize,
                .use_apll = true
            };
        #else // default
            i2s_config_t i2s_config = {
                .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
                .sample_rate = fSampleRate,
                .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
                .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
                .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
                .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
                .dma_buf_count = 3,
                .dma_buf_len = fBufferSize,
                .use_apll = true
            };
        #endif
            i2s_driver_install((i2s_port_t)1, &i2s_config, 0, nullptr);
            i2s_set_pin((i2s_port_t)1, &pin_config);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
            REG_WRITE(PIN_CTRL, 0xFFFFFFFF);
        }
    
        virtual ~esp32audio()
        {
            destroy();
        }
    
        virtual bool init(const char* name, dsp* dsp)
        {
            destroy();
            
            fDSP = dsp;
            fNumInputs = fDSP->getNumInputs();
            fNumOutputs = fDSP->getNumOutputs();
            
            fDSP->init(fSampleRate);
            
            if (fNumInputs > 0) {
                fInChannel = new FAUSTFLOAT*[fNumInputs];
                for (int i = 0; i < fNumInputs; i++) {
                    fInChannel[i] = new FAUSTFLOAT[fBufferSize];
                }
            } else {
                fInChannel = nullptr;
            }
            
            if (fNumOutputs > 0) {
                fOutChannel = new FAUSTFLOAT*[fNumOutputs];
                for (int i = 0; i < fNumOutputs; i++) {
                    fOutChannel[i] = new FAUSTFLOAT[fBufferSize];
                }
            } else {
                fOutChannel = nullptr;
            }
            
            return true;
        }
    
        virtual bool start()
        {
            if (!fRunning) {
                fRunning = true;
                return (xTaskCreatePinnedToCore(audioTaskHandler, "Faust DSP Task", 8192, (void*)this, 24, &fHandle, 1) == pdPASS);
            } else {
                return true;
            }
        }
    
        virtual void stop()
        {
            if (fRunning) {
                fRunning = false;
                vTaskDelay(1/portTICK_PERIOD_MS);
                fHandle = nullptr;
            }
        }
    
        virtual int getBufferSize() { return fBufferSize; }
        virtual int getSampleRate() { return fSampleRate; }

        virtual int getNumInputs() { return AUDIO_MAX_CHAN; }
        virtual int getNumOutputs() { return AUDIO_MAX_CHAN; }
    
        // Returns the average proportion of available CPU being spent inside the audio callbacks (between 0 and 1.0).
        virtual float getCPULoad() { return 0.f; }
    
};
					
#endif
/**************************  END  esp32audio.h **************************/

/********************END ARCHITECTURE SECTION (part 1/2)****************/

/**************************BEGIN USER SECTION **************************/

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fHslider0;
	float fRec0[2];
	FAUSTFLOAT fHslider1;
	float fRec2[2];
	int IOTA;
	float fVec0[32768]; EXT_RAM_ATTR;
	FAUSTFLOAT fHslider2;
	float fRec7[2];
	float fRec3[2];
	float fRec4[2];
	float fRec5[2];
	float fRec6[2];
	float fRec1[2];
	int fSampleRate;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("compile_options", "-a /usr/local/share/faust/esp32/esp32.cpp -lang cpp -es 1 -single -ftz 0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("filename", "SNESverb.dsp");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.4");
		m->declare("name", "SNESverb");
		m->declare("platform.lib/name", "Embedded Platform Library");
		m->declare("platform.lib/version", "0.1");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
	}

	virtual int getNumInputs() {
		return 2;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.5f);
		fHslider1 = FAUSTFLOAT(0.10000000000000001f);
		fHslider2 = FAUSTFLOAT(0.14999999999999999f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec0[l0] = 0.0f;
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec2[l1] = 0.0f;
		}
		IOTA = 0;
		for (int l2 = 0; (l2 < 32768); l2 = (l2 + 1)) {
			fVec0[l2] = 0.0f;
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec7[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec3[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec4[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec5[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec6[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec1[l8] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("SNESverb");
		ui_interface->declare(&fHslider2, "1", "");
		ui_interface->addHorizontalSlider("delay", &fHslider2, FAUSTFLOAT(0.150000006f), FAUSTFLOAT(0.100000001f), FAUSTFLOAT(0.300000012f), FAUSTFLOAT(0.00999999978f));
		ui_interface->declare(&fHslider1, "2", "");
		ui_interface->addHorizontalSlider("feedback", &fHslider1, FAUSTFLOAT(0.100000001f), FAUSTFLOAT(0.100000001f), FAUSTFLOAT(0.300000012f), FAUSTFLOAT(0.00999999978f));
		ui_interface->addHorizontalSlider("width", &fHslider0, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.00999999978f));
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = (0.000918750011f * float(fHslider0));
		float fSlow1 = (0.000918750011f * float(fHslider1));
		float fSlow2 = (0.000918750011f * float(fHslider2));
		for (int i0 = 0; (i0 < count); i0 = (i0 + 1)) {
			fRec0[0] = (fSlow0 + (0.999081254f * fRec0[1]));
			float fTemp0 = float(input1[i0]);
			float fTemp1 = float(input0[i0]);
			fRec2[0] = (fSlow1 + (0.999081254f * fRec2[1]));
			float fTemp2 = (fTemp0 + (fTemp1 + (fRec2[0] * fRec1[1])));
			fVec0[(IOTA & 32767)] = fTemp2;
			fRec7[0] = (fSlow2 + (0.999081254f * fRec7[1]));
			float fTemp3 = (48000.0f * fRec7[0]);
			float fTemp4 = ((fRec3[1] != 0.0f) ? (((fRec4[1] > 0.0f) & (fRec4[1] < 1.0f)) ? fRec3[1] : 0.0f) : (((fRec4[1] == 0.0f) & (fTemp3 != fRec5[1])) ? 0.015625f : (((fRec4[1] == 1.0f) & (fTemp3 != fRec6[1])) ? -0.015625f : 0.0f)));
			fRec3[0] = fTemp4;
			fRec4[0] = std::max<float>(0.0f, std::min<float>(1.0f, (fRec4[1] + fTemp4)));
			fRec5[0] = (((fRec4[1] >= 1.0f) & (fRec6[1] != fTemp3)) ? fTemp3 : fRec5[1]);
			fRec6[0] = (((fRec4[1] <= 0.0f) & (fRec5[1] != fTemp3)) ? fTemp3 : fRec6[1]);
			float fTemp5 = fVec0[((IOTA - int(std::min<float>(24000.0f, std::max<float>(0.0f, fRec5[0])))) & 32767)];
			fRec1[0] = (fTemp5 + (fRec4[0] * (fVec0[((IOTA - int(std::min<float>(24000.0f, std::max<float>(0.0f, fRec6[0])))) & 32767)] - fTemp5)));
			float fTemp6 = (fTemp0 + (fTemp1 + (0.5f * fRec1[0])));
			output0[i0] = FAUSTFLOAT(((1.0f - fRec0[0]) * fTemp6));
			output1[i0] = FAUSTFLOAT((fRec0[0] * fTemp6));
			fRec0[1] = fRec0[0];
			fRec2[1] = fRec2[0];
			IOTA = (IOTA + 1);
			fRec7[1] = fRec7[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
			fRec1[1] = fRec1[0];
		}
	}

};

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/

using namespace std;

#ifdef MIDICTRL
list<GUI*> GUI::fGuiList;
ztimedmap GUI::gTimedZoneMap;
#endif

SNESverb::SNESverb(int sample_rate, int buffer_size)
{
#ifdef NVOICES
    int nvoices = NVOICES;
    fDSP = new mydsp_poly(new mydsp(), nvoices, true, true);
#else
    fDSP = new mydsp();
#endif
    
    fUI = new MapUI();
    fDSP->buildUserInterface(fUI);
    
    fAudio = new esp32audio(sample_rate, buffer_size);
    fAudio->init("esp32", fDSP);
    
#ifdef SOUNDFILE
    fSoundUI = new SoundUI("/sdcard/", sample_rate);
    fDSP->buildUserInterface(fSoundUI);
#endif
    
#ifdef MIDICTRL
    fMIDIHandler = new esp32_midi();
    fMIDIInterface = new MidiUI(fMIDIHandler);
    fDSP->buildUserInterface(fMIDIInterface);
#endif
}

SNESverb::~SNESverb()
{
    delete fDSP;
    delete fUI;
    delete fAudio;
#ifdef MIDICTRL
    delete fMIDIInterface;
    delete fMIDIHandler;
#endif
#ifdef SOUNDFILE
    delete fSoundUI;
#endif
}

bool SNESverb::start()
{
#ifdef MIDICTRL
    if (!fMIDIInterface->run()) return false;
#endif
    return fAudio->start();
}

void SNESverb::stop()
{
#ifdef MIDICTRL
    fMIDIInterface->stop();
#endif
    fAudio->stop();
}

void SNESverb::setParamValue(const string& path, float value)
{
    fUI->setParamValue(path, value);
}

float SNESverb::getParamValue(const string& path)
{
    return fUI->getParamValue(path);
}

// Entry point
#ifdef HAS_MAIN
extern "C" void app_main()
{
    // Init audio codec
    WM8978 wm8978;
    wm8978.init();
    wm8978.addaCfg(1,1);
    wm8978.inputCfg(1,0,0);
    wm8978.outputCfg(1,0);
    wm8978.micGain(30);
    wm8978.auxGain(0);
    wm8978.lineinGain(0);

    // Set gain
    wm8978.spkVolSet(60); // [0-63]
    
    wm8978.hpVolSet(40,40);
    wm8978.i2sCfg(2,0);
    
    // Allocate and start Faust DSP
    SNESverb* DSP = new SNESverb(48000, 32);
    DSP->start();
    
    // Waiting forever
    vTaskSuspend(nullptr);
}
#endif

/********************END ARCHITECTURE SECTION (part 2/2)****************/

#endif
