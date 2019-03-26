//
// Created by shiyunjie on 17/3/7.
//

#ifndef QBNETWORK_JNICALLBACK_H
#define QBNETWORK_JNICALLBACK_H

#include <jni.h>
#include <iostream>
#include "IProtocolCallBack.h"
using namespace std;
class JniCallBack : public IProtocolCallBack
{
public:
    JniCallBack();
    ~JniCallBack();
    virtual bool Init(JNIEnv *jni_env, jobject jni_callback);
    // $_FUNCTION_BEGIN ******************************
    // NAME:    OnRecvData
    // PARAM:   packet_id,socket,buffer,buf_len
    // RETURN:  int 1(success) -1(fail)
    // DETAIL:  recv buffer callback
    // $_FUNCTION_END ********************************
    virtual int OnRecvData(unsigned short packet_id, int socket, unsigned char * buffer, unsigned short buf_len);
    // $_FUNCTION_BEGIN ******************************
    // NAME:    OnError
    // PARAM:   err_type
    // RETURN:  int 1(success) -1(fail)
    // DETAIL:  recv and send error callback
    // $_FUNCTION_END ********************************
    virtual int OnError(int err_type);
    // $_FUNCTION_BEGIN ******************************
    // NAME:    OnClose
    // PARAM:   socket,client_closed
    // RETURN:  int 1(success) -1(fail)
    // DETAIL:  server closed connect if client_closed else client close socket
    // $_FUNCTION_END ********************************
    virtual int OnClose(int socket, bool client_closed);
    // $_FUNCTION_BEGIN ******************************
    // NAME:    OnConnected
    // PARAM:   socket
    // RETURN:  int 1(success) -1(fail)
    // DETAIL:  connect callback as soon as established
    // $_FUNCTION_END ********************************
    virtual int  OnConnected(int socket);
    // $_FUNCTION_BEGIN ******************************
    // NAME:    OnConnectFailure
    // PARAM:   socket(0)
    // RETURN:  int 1(success) -1(fail)
    // DETAIL:  connect establish failure callback
    // $_FUNCTION_END ********************************
    virtual int OnConnectFailure(int socket);

private:
    JNIEnv*     m_jnievn;
    jclass      m_callback_class;
    jobject     m_callback_object;
    jmethodID   m_on_recvdata_id;
    jmethodID   m_on_error_id;
    jmethodID   m_on_close_id;
    jmethodID   m_on_connected_id;
    jmethodID   m_on_connect_fail_id;
    bool        m_open;
};


#endif //QBNETWORK_JNICALLBACK_H
