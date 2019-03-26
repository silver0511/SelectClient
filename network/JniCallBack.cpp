//
// Created by shiyunjie on 17/3/7.
//

#include "JniCallBack.h"

extern JavaVM* g_jvm;

JniCallBack::JniCallBack()
{
    m_jnievn = 0;
    m_callback_class = 0;
    m_callback_object = 0;
    m_on_recvdata_id = 0;
    m_on_error_id = 0;
    m_on_close_id = 0;
    m_on_connected_id = 0;
    m_on_connect_fail_id = 0;
    m_open = false;
}

JniCallBack::~JniCallBack()
{
    if(m_jnievn && m_callback_object)
    {
        m_jnievn->DeleteGlobalRef(m_callback_object);
    }

    m_jnievn = 0;
    m_callback_class = 0;
    m_callback_object = 0;
    m_on_recvdata_id = 0;
    m_on_error_id = 0;
    m_on_close_id = 0;
    m_on_connected_id = 0;
    m_on_connect_fail_id = 0;
    m_open = false;
}

bool JniCallBack::Init(JNIEnv *jni_env, jobject jni_callback)
{
    if(!jni_env || !jni_callback)
    {
        cout << "JniCallBack::Init invalid param" << endl;
        return false;
    }

    if(m_open)
    {
        cout << "JniCallBack::Init is opened" << endl;
        return false;
    }


    m_jnievn = jni_env;

    m_callback_class = m_jnievn->GetObjectClass(jni_callback);
    m_callback_object = m_jnievn->NewGlobalRef(jni_callback);
    if(!m_callback_class || !m_callback_object)
    {
        cout << "JniCallBack::Init m_jni_callback init failed" << endl;
        return false;
    }

    //init method
    m_on_recvdata_id = m_jnievn->GetMethodID(m_callback_class, "OnRecvData", "(II[BI)I");
    m_on_error_id = m_jnievn->GetMethodID(m_callback_class, "OnError", "(I)I");
    m_on_close_id = m_jnievn->GetMethodID(m_callback_class, "OnClose", "(IZ)I");
    m_on_connected_id = m_jnievn->GetMethodID(m_callback_class, "OnConnected", "(I)I");
    m_on_connect_fail_id = m_jnievn->GetMethodID(m_callback_class, "OnConnectFailure", "(I)I");

    if(!m_on_recvdata_id ||
            !m_on_error_id ||
            !m_on_close_id ||
            !m_on_connected_id ||
            !m_on_connect_fail_id)
    {
        cout << "JniCallBack::Init method init failed" << endl;
        return false;
    }

    m_open = true;
    return m_open;
}

int JniCallBack::OnRecvData(unsigned short packet_id, int socket, unsigned char *buffer,
                            unsigned short buf_len)
{
    if(!m_open)
    {
        cout << "JniCallBack::OnRecvData not init" << endl;
        return -1;
    }

    JNIEnv* jni_env = NULL;
    int status = g_jvm->GetEnv((void**)&jni_env, JNI_VERSION_1_6);
    if(status < 0)
    {
        g_jvm->AttachCurrentThread(&jni_env, NULL);
    }

    if(!jni_env)
    {
        cout << "JniCallBack::OnRecvData jni_env is null" << endl;
        return -1;
    }

    jbyte *j_data = (jbyte*)buffer;
    jbyteArray j_array = jni_env->NewByteArray(buf_len);
    jni_env->SetByteArrayRegion(j_array, 0, buf_len, j_data);

    jni_env->CallIntMethod(m_callback_object,
                           m_on_recvdata_id,
                           (jint)packet_id,
                           (jint)socket,
                           j_array,
                           (jint)buf_len);

    if(status < 0)
    {
        g_jvm->DetachCurrentThread();
    }
    return 1;
}


int JniCallBack::OnError(int err_type)
{
    if(!m_open)
    {
        cout << "JniCallBack::OnError not init" << endl;
        return -1;
    }

    JNIEnv* jni_env = NULL;
    int status = g_jvm->GetEnv((void**)&jni_env, JNI_VERSION_1_6);
    if(status < 0)
    {
        g_jvm->AttachCurrentThread(&jni_env, NULL);
    }

    if(!jni_env)
    {
        cout << "JniCallBack::OnError jni_env is null" << endl;
        return -1;
    }

    jni_env->CallIntMethod(m_callback_object, m_on_error_id, (jint)err_type);

    if(status < 0)
    {
        g_jvm->DetachCurrentThread();
    }

    return 1;
}

int JniCallBack::OnClose(int socket, bool client_closed)
{
    if(!m_open)
    {
        cout << "JniCallBack::OnClose not init" << endl;
        return -1;
    }

    JNIEnv* jni_env = NULL;
    int status = g_jvm->GetEnv((void**)&jni_env, JNI_VERSION_1_6);
    if(status < 0)
    {
        g_jvm->AttachCurrentThread(&jni_env, NULL);
    }

    if(!jni_env)
    {
        cout << "JniCallBack::OnClose jni_env is null" << endl;
        return -1;
    }

    jni_env->CallIntMethod(m_callback_object, m_on_close_id, (jint)socket, (jboolean)client_closed);
    if(status < 0)
    {
        g_jvm->DetachCurrentThread();
    }

    return 1;
}

int JniCallBack::OnConnected(int socket)
{
    if(!m_open)
    {
        cout << "JniCallBack::OnConnected not init" << endl;
        return -1;
    }

    JNIEnv* jni_env = NULL;
    int status = g_jvm->GetEnv((void**)&jni_env, JNI_VERSION_1_6);
    if(status < 0)
    {
        g_jvm->AttachCurrentThread(&jni_env, NULL);
    }

    if(!jni_env)
    {
        cout << "JniCallBack::OnConnected jni_env is null" << endl;
        return -1;
    }

    jni_env->CallIntMethod(m_callback_object, m_on_connected_id, (jint)socket);

    if(status < 0)
    {
        g_jvm->DetachCurrentThread();
    }

    return 1;
}

int JniCallBack::OnConnectFailure(int socket)
{
    if(!m_open)
    {
        cout << "JniCallBack::OnConnectFailure not init" << endl;
        return -1;
    }

    JNIEnv* jni_env = NULL;
    int status = g_jvm->GetEnv((void**)&jni_env, JNI_VERSION_1_6);
    if(status < 0)
    {
        g_jvm->AttachCurrentThread(&jni_env, NULL);
    }

    if(!jni_env)
    {
        cout << "JniCallBack::OnConnectFailure jni_env is null" << endl;
        return -1;
    }

    jni_env->CallIntMethod(m_callback_object, m_on_connect_fail_id, (jint)socket);

    if(status < 0)
    {
        g_jvm->DetachCurrentThread();
    }

    return 1;
}

