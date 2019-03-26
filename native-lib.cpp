#include <jni.h>
#include <string>
#include "./network/ITcpManager.h"
#include "./network/JniCallBack.h"
bool g_open = false;
JavaVM* g_jvm = NULL;
JniCallBack  *jni_callback = NULL;
ITcpManager* tcpManager = NULL;

extern "C"
{
    int JNI_OnLoad(JavaVM* jvm, void* reserved)
    {
        g_jvm = jvm;
        g_open = false;
        JNIEnv *env;
        if (jvm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
        {
            cout << "JNI_OnLoad failed" << endl;
            return -1;
        }

        cout << "JNI_OnLoad Success" << endl;

        return JNI_VERSION_1_6;
    }

    void JNI_OnUnLoad(JavaVM* jvm, void* reserved)
    {
        JNIEnv *env;
        g_open = false;
        g_jvm = NULL;
        if (jvm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
        {
            return;
        }

        cout << "JNI_OnUnLoad Success" << endl;

    }

    /**
     *初始化
     */
    JNIEXPORT jboolean JNICALL
    Java_com_qbao_newim_network_JniNetWork_create(JNIEnv *env, jobject /* this */)
    {
        jni_callback = new JniCallBack();
        CreateNetTCP("TcpManager", (void **) &tcpManager);
        g_open = true;
        return true;
    }

    /**
     *开始连接
     */
    JNIEXPORT jboolean JNICALL
    Java_com_qbao_newim_network_JniNetWork_Connect(JNIEnv *env,
            jclass j_class, jstring host, jint port, jboolean domain)
    {
        if(!g_open)
        {
            return false;
        }

        jclass clsstring = env->FindClass("java/lang/String");
        jstring strencode = env->NewStringUTF("utf-8");
        jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
        char* lsz_host = NULL;
        jbyteArray jb_array_host= (jbyteArray)env->CallObjectMethod(host, mid, strencode);
        jsize host_len = env->GetArrayLength(jb_array_host);
        jbyte* j_byte_host = env->GetByteArrayElements(jb_array_host, JNI_FALSE);
        if(host_len <= 0)
        {
            cout << "JNI Connect host_len <= 0" << endl;
            return -1;
        }

        lsz_host = (char*)malloc(host_len + 1);
        memcpy(lsz_host, j_byte_host, host_len);
        lsz_host[host_len] = 0;
        tcpManager->Connect(lsz_host, port, domain);

        env->ReleaseByteArrayElements(jb_array_host, j_byte_host, 0);
        free(lsz_host);
        return true;
    }

    /**
     * 关闭连接
     */
    JNIEXPORT jboolean JNICALL
    Java_com_qbao_newim_network_JniNetWork_Close(JNIEnv *env, jobject)
    {
        if(!g_open)
        {
            return false;
        }
        if (tcpManager == NULL) {
            return false;
        }
        tcpManager->Close();
        return true;
    }

    /**
     * 检查更新
     */
    JNIEXPORT jboolean JNICALL
    Java_com_qbao_newim_network_JniNetWork_Update(JNIEnv *env, jobject)
    {
        if(!g_open)
        {
            return false;
        }

        if (tcpManager == NULL)
        {
            return false;
        }
        tcpManager->Update();
        return true;
    }

    /**
     * 发送消息
     */
    JNIEXPORT jint JNICALL
    Java_com_qbao_newim_network_JniNetWork_SendPack(JNIEnv *env, jclass,
                                                    jint package_id, jbyteArray buff, jint len)
    {
        if(!g_open)
        {
            return false;
        }

        if(buff == NULL || len <= 0)
        {
            return tcpManager->SendPack(package_id, NULL, len);
        }

        jbyte lpBuffer[8192];
        env->GetByteArrayRegion(buff,0,len,lpBuffer);
        return tcpManager->SendPack(package_id, (char*)lpBuffer, len);
    }

    /**
     * 回调
     */
    JNIEXPORT jboolean JNICALL
    Java_com_qbao_newim_network_JniNetWork_InitCallBack(JNIEnv *env, jobject, jobject j_callback_obj)
    {
        bool result = false;
        if(!g_open)
        {
            return result;
        }
        result = jni_callback->Init(env, j_callback_obj);
        if(!result)
        {
            return result;
        }
        tcpManager->InitCallBack(jni_callback);
        return result;
    }
}
