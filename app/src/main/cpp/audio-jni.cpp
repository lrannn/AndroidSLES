//
// Created by lrannn on 2017/6/9.
//

#include <jni.h>
#include <assert.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <stdio.h>

/**
 * ##NAME表示将传入的NAME直接与前面的拼接
 * 如果是#NAME的话表示进行字符串化？
 */
#define LIBRARY_FUNC(RETURN_TYPE, NAME, ...) \
   extern "C"{ \
   JNIEXPORT RETURN_TYPE \
   Java_com_mass_audio_opensles_MainActivity_ ## NAME \
   (JNIEnv* jniEnv,jobject thiz,##__VA_ARGS__); \
} \
   JNIEXPORT RETURN_TYPE \
   Java_com_mass_audio_opensles_MainActivity_ ## NAME \
   (JNIEnv* jniEnv,jobject thiz, ##__VA_ARGS__)\


void check(SLresult res);

SLObjectItf engineObj = NULL; //引擎对象
SLEngineItf engineItf; //引擎接口，根据引擎接口来创建其他对象

SLObjectItf outputMixObj = NULL;
SLEnvironmentalReverbItf outputMixItf;


// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
        SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

LIBRARY_FUNC(void, createEngin) {
    SLresult result;

    //创建音频引擎
    result = slCreateEngine(&engineObj, 0, NULL, 0, NULL, NULL);
    check(result);

    // 初始化
    result = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
    check(result);

    //得到引擎接口
    check((*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engineItf));

    //创建输出混音对象
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    check((*engineItf)->CreateOutputMix(engineItf, &outputMixObj, 1, ids, req));

    //Realize
    check((*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE));
    //得到reverb的外部调用接口，设置属性
    result = (*outputMixObj)->GetInterface(outputMixObj, SL_IID_ENVIRONMENTALREVERB, &outputMixItf);
    if (result == SL_RESULT_SUCCESS) {
        (*outputMixItf)->SetEnvironmentalReverbProperties(outputMixItf, &reverbSettings);
    }
}


LIBRARY_FUNC(void, createBufferQueueAudioPlayer, jint sample_rate, jint buffer_size) {
    SLresult result;

    //  初始化AudioSource
    SLDataLocator_AndroidSimpleBufferQueue loc_bufqueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                           2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2/*numOfChannels*/, sample_rate,
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                   SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&loc_bufqueue, &format_pcm};

    // 配置AudioSink -------------
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObj};
    SLDataSink audioSnk = {&loc_outmix, NULL};


}


void check(SLresult result) {
    result == SL_RESULT_SUCCESS;
    (void) result;
}



