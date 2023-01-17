// Options:
//#define HAS_MAIN
#define PICO_DSP true
/* ------------------------------------------------------------
name: "djembe6"
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

#include "djembe6.h"

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
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
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
                    i2s_read((i2s_port_t)0, &samples_data_in, AUDIO_MAX_CHAN*sizeof(float)*fBufferSize, &bytes_read, portMAX_DELAY);
                    
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
                TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
                TIMERG0.wdt_feed=1;
                TIMERG0.wdt_wprotect=0;
                
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
                i2s_write((i2s_port_t)0, &samples_data_out, AUDIO_MAX_CHAN*sizeof(float)*fBufferSize, &bytes_written, portMAX_DELAY);
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
        #elif LYRA_T
            pin_config = {
                .bck_io_num = 5,
                .ws_io_num = 25,
                .data_out_num = 26,
                .data_in_num = 35
            };
         #elif PICO_DSP
            pin_config = {
                .bck_io_num = 23,
                .ws_io_num = 25,
                .data_out_num = 26,
                .data_in_num = 27
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
            i2s_driver_install((i2s_port_t)0, &i2s_config, 0, nullptr);
            i2s_set_pin((i2s_port_t)0, &pin_config);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
            REG_WRITE(PIN_CTRL, 0xFFFFFFF0);
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
                return (xTaskCreatePinnedToCore(audioTaskHandler, "Faust DSP Task", 4096, (void*)this, 24, &fHandle, 0) == pdPASS);
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

static float mydsp_faustpower2_f(float value) {
	return (value * value);
}

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
	int iVec0[2];
	int iRec3[2];
	float fRec2[3];
	float fRec1[3];
	FAUSTFLOAT fHslider1;
	int iRec5[2];
	int iVec1[2];
	int iRec4[2];
	FAUSTFLOAT fHslider2;
	float fRec0[3];
	float fRec6[3];
	float fRec7[3];
	float fRec8[3];
	float fRec9[3];
	float fRec10[3];
	float fRec11[3];
	float fRec12[3];
	float fRec13[3];
	float fRec14[3];
	float fRec15[3];
	float fRec16[3];
	float fRec17[3];
	float fRec18[3];
	float fRec19[3];
	float fRec20[3];
	float fRec21[3];
	float fRec22[3];
	float fRec23[3];
	float fRec24[3];
	int fSampleRate;
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("compile_options", "-a /usr/local/share/faust/esp32/esp32.cpp -lang cpp -es 1 -single -ftz 0");
		m->declare("envelopes.lib/ar:author", "Yann Orlarey, Stéphane Letz");
		m->declare("envelopes.lib/author", "GRAME");
		m->declare("envelopes.lib/copyright", "GRAME");
		m->declare("envelopes.lib/license", "LGPL with exception");
		m->declare("envelopes.lib/name", "Faust Envelope Library");
		m->declare("envelopes.lib/version", "0.1");
		m->declare("filename", "djembe6.dsp");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/highpass:author", "Julius O. Smith III");
		m->declare("filters.lib/highpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "0.3");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.3");
		m->declare("name", "djembe6");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "0.0");
		m->declare("platform.lib/name", "Embedded Platform Library");
		m->declare("platform.lib/version", "0.1");
	}

	virtual int getNumInputs() {
		return 0;
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
		fHslider1 = FAUSTFLOAT(0.5f);
		fHslider2 = FAUSTFLOAT(130.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			iVec0[l0] = 0;
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			iRec3[l1] = 0;
		}
		for (int l2 = 0; (l2 < 3); l2 = (l2 + 1)) {
			fRec2[l2] = 0.0f;
		}
		for (int l3 = 0; (l3 < 3); l3 = (l3 + 1)) {
			fRec1[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			iRec5[l4] = 0;
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			iVec1[l5] = 0;
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			iRec4[l6] = 0;
		}
		for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) {
			fRec0[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 3); l8 = (l8 + 1)) {
			fRec6[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 3); l9 = (l9 + 1)) {
			fRec7[l9] = 0.0f;
		}
		for (int l10 = 0; (l10 < 3); l10 = (l10 + 1)) {
			fRec8[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 3); l11 = (l11 + 1)) {
			fRec9[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 3); l12 = (l12 + 1)) {
			fRec10[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 3); l13 = (l13 + 1)) {
			fRec11[l13] = 0.0f;
		}
		for (int l14 = 0; (l14 < 3); l14 = (l14 + 1)) {
			fRec12[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 3); l15 = (l15 + 1)) {
			fRec13[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 3); l16 = (l16 + 1)) {
			fRec14[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 3); l17 = (l17 + 1)) {
			fRec15[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 3); l18 = (l18 + 1)) {
			fRec16[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 3); l19 = (l19 + 1)) {
			fRec17[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 3); l20 = (l20 + 1)) {
			fRec18[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 3); l21 = (l21 + 1)) {
			fRec19[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 3); l22 = (l22 + 1)) {
			fRec20[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 3); l23 = (l23 + 1)) {
			fRec21[l23] = 0.0f;
		}
		for (int l24 = 0; (l24 < 3); l24 = (l24 + 1)) {
			fRec22[l24] = 0.0f;
		}
		for (int l25 = 0; (l25 < 3); l25 = (l25 + 1)) {
			fRec23[l25] = 0.0f;
		}
		for (int l26 = 0; (l26 < 3); l26 = (l26 + 1)) {
			fRec24[l26] = 0.0f;
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
		ui_interface->openVerticalBox("djembe6");
		ui_interface->declare(&fHslider2, "0", "");
		ui_interface->addHorizontalSlider("freq", &fHslider2, 130.0f, 50.0f, 20000.0f, 0.100000001f);
		ui_interface->declare(&fHslider0, "2", "");
		ui_interface->addHorizontalSlider("stPos", &fHslider0, 0.5f, 0.100000001f, 1.0f, 0.00999999978f);
		ui_interface->declare(&fHslider1, "3", "");
		ui_interface->addHorizontalSlider("Sharp", &fHslider1, 0.5f, 0.100000001f, 1.0f, 0.00999999978f);
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = float(fHslider0);
		float fSlow1 = std::tan((6.54498435e-05f * ((15000.0f * fSlow0) + 500.0f)));
		float fSlow2 = (1.0f / fSlow1);
		float fSlow3 = (((fSlow2 + 1.41421354f) / fSlow1) + 1.0f);
		float fSlow4 = (0.699999988f / fSlow3);
		float fSlow5 = std::tan((6.54498435e-05f * ((500.0f * fSlow0) + 40.0f)));
		float fSlow6 = (1.0f / fSlow5);
		float fSlow7 = (1.0f / (((fSlow6 + 1.41421354f) / fSlow5) + 1.0f));
		float fSlow8 = mydsp_faustpower2_f(fSlow5);
		float fSlow9 = (1.0f / fSlow8);
		float fSlow10 = (((fSlow6 + -1.41421354f) / fSlow5) + 1.0f);
		float fSlow11 = (2.0f * (1.0f - fSlow9));
		float fSlow12 = (0.0f - (2.0f / fSlow8));
		float fSlow13 = (1.0f / fSlow3);
		float fSlow14 = (((fSlow2 + -1.41421354f) / fSlow1) + 1.0f);
		float fSlow15 = (2.0f * (1.0f - (1.0f / mydsp_faustpower2_f(fSlow1))));
		float fSlow16 = (1.0f / std::max<float>(1.0f, (96.0f * float(fHslider1))));
		float fSlow17 = float(fHslider2);
		float fSlow18 = (0.0f - (1.9995203f * std::cos((0.000130899687f * fSlow17))));
		float fSlow19 = (0.0f - (1.99949515f * std::cos((0.000130899687f * (fSlow17 + 200.0f)))));
		float fSlow20 = (0.0f - (1.99946702f * std::cos((0.000130899687f * (fSlow17 + 400.0f)))));
		float fSlow21 = (0.0f - (1.99943566f * std::cos((0.000130899687f * (fSlow17 + 600.0f)))));
		float fSlow22 = (0.0f - (1.9994005f * std::cos((0.000130899687f * (fSlow17 + 800.0f)))));
		float fSlow23 = (0.0f - (1.99936044f * std::cos((0.000130899687f * (fSlow17 + 1000.0f)))));
		float fSlow24 = (0.0f - (1.99931479f * std::cos((0.000130899687f * (fSlow17 + 1200.0f)))));
		float fSlow25 = (0.0f - (1.99926209f * std::cos((0.000130899687f * (fSlow17 + 1400.0f)))));
		float fSlow26 = (0.0f - (1.9992007f * std::cos((0.000130899687f * (fSlow17 + 1600.0f)))));
		float fSlow27 = (0.0f - (1.99912798f * std::cos((0.000130899687f * (fSlow17 + 1800.0f)))));
		float fSlow28 = (0.0f - (1.99904084f * std::cos((0.000130899687f * (fSlow17 + 2000.0f)))));
		float fSlow29 = (0.0f - (1.99893427f * std::cos((0.000130899687f * (fSlow17 + 2200.0f)))));
		float fSlow30 = (0.0f - (1.99880111f * std::cos((0.000130899687f * (fSlow17 + 2400.0f)))));
		float fSlow31 = (0.0f - (1.99862993f * std::cos((0.000130899687f * (fSlow17 + 2600.0f)))));
		float fSlow32 = (0.0f - (1.99840164f * std::cos((0.000130899687f * (fSlow17 + 2800.0f)))));
		float fSlow33 = (0.0f - (1.99808204f * std::cos((0.000130899687f * (fSlow17 + 3000.0f)))));
		float fSlow34 = (0.0f - (1.99760294f * std::cos((0.000130899687f * (fSlow17 + 3200.0f)))));
		float fSlow35 = (0.0f - (1.99680448f * std::cos((0.000130899687f * (fSlow17 + 3400.0f)))));
		float fSlow36 = (0.0f - (1.99520874f * std::cos((0.000130899687f * (fSlow17 + 3600.0f)))));
		float fSlow37 = (0.0f - (1.99042892f * std::cos((0.000130899687f * (fSlow17 + 3800.0f)))));
		for (int i0 = 0; (i0 < count); i0 = (i0 + 1)) {
			iVec0[0] = 1;
			iRec3[0] = ((1103515245 * iRec3[1]) + 12345);
			fRec2[0] = ((4.65661287e-10f * float(iRec3[0])) - (fSlow7 * ((fSlow10 * fRec2[2]) + (fSlow11 * fRec2[1]))));
			fRec1[0] = ((fSlow7 * (((fSlow9 * fRec2[0]) + (fSlow12 * fRec2[1])) + (fSlow9 * fRec2[2]))) - (fSlow13 * ((fSlow14 * fRec1[2]) + (fSlow15 * fRec1[1]))));
			iRec5[0] = ((iVec0[1] + iRec5[1]) % 5000);
			int iTemp0 = (iRec5[0] < 1);
			iVec1[0] = iTemp0;
			iRec4[0] = (((iRec4[1] + (iRec4[1] > 0)) * (iTemp0 <= iVec1[1])) + (iTemp0 > iVec1[1]));
			float fTemp1 = (fSlow16 * float(iRec4[0]));
			float fTemp2 = (fSlow4 * ((fRec1[2] + (fRec1[0] + (2.0f * fRec1[1]))) * std::max<float>(0.0f, std::min<float>(fTemp1, (2.0f - fTemp1)))));
			fRec0[0] = (fTemp2 - ((fSlow18 * fRec0[1]) + (0.999520421f * fRec0[2])));
			fRec6[0] = (fTemp2 - ((fSlow19 * fRec6[1]) + (0.999495149f * fRec6[2])));
			fRec7[0] = (fTemp2 - ((fSlow20 * fRec7[1]) + (0.999467134f * fRec7[2])));
			fRec8[0] = (fTemp2 - ((fSlow21 * fRec8[1]) + (0.999435782f * fRec8[2])));
			fRec9[0] = (fTemp2 - ((fSlow22 * fRec9[1]) + (0.999400556f * fRec9[2])));
			fRec10[0] = (fTemp2 - ((fSlow23 * fRec10[1]) + (0.999360621f * fRec10[2])));
			fRec11[0] = (fTemp2 - ((fSlow24 * fRec11[1]) + (0.999314964f * fRec11[2])));
			fRec12[0] = (fTemp2 - ((fSlow25 * fRec12[1]) + (0.999262273f * fRec12[2])));
			fRec13[0] = (fTemp2 - ((fSlow26 * fRec13[1]) + (0.999200821f * fRec13[2])));
			fRec14[0] = (fTemp2 - ((fSlow27 * fRec14[1]) + (0.999128163f * fRec14[2])));
			fRec15[0] = (fTemp2 - ((fSlow28 * fRec15[1]) + (0.999041021f * fRec15[2])));
			fRec16[0] = (fTemp2 - ((fSlow29 * fRec16[1]) + (0.998934567f * fRec16[2])));
			fRec17[0] = (fTemp2 - ((fSlow30 * fRec17[1]) + (0.99880147f * fRec17[2])));
			fRec18[0] = (fTemp2 - ((fSlow31 * fRec18[1]) + (0.998630345f * fRec18[2])));
			fRec19[0] = (fTemp2 - ((fSlow32 * fRec19[1]) + (0.998402238f * fRec19[2])));
			fRec20[0] = (fTemp2 - ((fSlow33 * fRec20[1]) + (0.998082995f * fRec20[2])));
			fRec21[0] = (fTemp2 - ((fSlow34 * fRec21[1]) + (0.99760437f * fRec21[2])));
			fRec22[0] = (fTemp2 - ((fSlow35 * fRec22[1]) + (0.996807098f * fRec22[2])));
			fRec23[0] = (fTemp2 - ((fSlow36 * fRec23[1]) + (0.995214462f * fRec23[2])));
			fRec24[0] = (fTemp2 - ((fSlow37 * fRec24[1]) + (0.990451753f * fRec24[2])));
			float fTemp3 = (0.0500000007f * ((((((((((((((((((((fRec0[0] + (0.25f * (fRec6[0] - fRec6[2]))) + (0.111111112f * (fRec7[0] - fRec7[2]))) + (0.0625f * (fRec8[0] - fRec8[2]))) + (0.0399999991f * (fRec9[0] - fRec9[2]))) + (0.027777778f * (fRec10[0] - fRec10[2]))) + (0.0204081628f * (fRec11[0] - fRec11[2]))) + (0.015625f * (fRec12[0] - fRec12[2]))) + (0.0123456791f * (fRec13[0] - fRec13[2]))) + (0.00999999978f * (fRec14[0] - fRec14[2]))) + (0.00826446246f * (fRec15[0] - fRec15[2]))) + (0.0069444445f * (fRec16[0] - fRec16[2]))) + (0.00591715984f * (fRec17[0] - fRec17[2]))) + (0.00510204071f * (fRec18[0] - fRec18[2]))) + (0.00444444455f * (fRec19[0] - fRec19[2]))) + (0.00390625f * (fRec20[0] - fRec20[2]))) + (0.00346020772f * (fRec21[0] - fRec21[2]))) + (0.00308641978f * (fRec22[0] - fRec22[2]))) + (0.00277008303f * (fRec23[0] - fRec23[2]))) + (0.00249999994f * (fRec24[0] - fRec24[2]))) - fRec0[2]));
			output0[i0] = FAUSTFLOAT(fTemp3);
			output1[i0] = FAUSTFLOAT(fTemp3);
			iVec0[1] = iVec0[0];
			iRec3[1] = iRec3[0];
			fRec2[2] = fRec2[1];
			fRec2[1] = fRec2[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			iRec5[1] = iRec5[0];
			iVec1[1] = iVec1[0];
			iRec4[1] = iRec4[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec12[2] = fRec12[1];
			fRec12[1] = fRec12[0];
			fRec13[2] = fRec13[1];
			fRec13[1] = fRec13[0];
			fRec14[2] = fRec14[1];
			fRec14[1] = fRec14[0];
			fRec15[2] = fRec15[1];
			fRec15[1] = fRec15[0];
			fRec16[2] = fRec16[1];
			fRec16[1] = fRec16[0];
			fRec17[2] = fRec17[1];
			fRec17[1] = fRec17[0];
			fRec18[2] = fRec18[1];
			fRec18[1] = fRec18[0];
			fRec19[2] = fRec19[1];
			fRec19[1] = fRec19[0];
			fRec20[2] = fRec20[1];
			fRec20[1] = fRec20[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fRec22[2] = fRec22[1];
			fRec22[1] = fRec22[0];
			fRec23[2] = fRec23[1];
			fRec23[1] = fRec23[0];
			fRec24[2] = fRec24[1];
			fRec24[1] = fRec24[0];
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

djembe6::djembe6(int sample_rate, int buffer_size)
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

djembe6::~djembe6()
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

bool djembe6::start()
{
#ifdef MIDICTRL
    if (!fMIDIInterface->run()) return false;
#endif
    return fAudio->start();
}

void djembe6::stop()
{
#ifdef MIDICTRL
    fMIDIInterface->stop();
#endif
    fAudio->stop();
}

void djembe6::setParamValue(const string& path, float value)
{
    fUI->setParamValue(path, value);
}

float djembe6::getParamValue(const string& path)
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
    djembe6* DSP = new djembe6(48000, 32);
    DSP->start();
    
    // Waiting forever
    vTaskSuspend(nullptr);
}
#endif

/********************END ARCHITECTURE SECTION (part 2/2)****************/

#endif
