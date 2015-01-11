/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/inotify.h>

/* 宏定义begin */
//清0宏
#define MEM_ZERO(pDest, destSize) memset(pDest, 0, destSize)

//LOG宏定义
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, "test", __VA_ARGS__)
#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "test",__VA_ARGS__)
#define LOG_WARN(...) __android_log_print(ANDROID_LOG_WARN, "test", __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, "test",__VA_ARGS__)

JNIEXPORT jstring JNICALL Java_com_ypan_umtest_SelfUninstall_startWork(
		JNIEnv *env, jclass thiz, jstring exepath, jstring exename,
		jstring monitorpath, jstring url, jint version) {
	const char *mexepath = (*env)->GetStringUTFChars(env, exepath, NULL);
	const char *mexename = (*env)->GetStringUTFChars(env, exename, NULL);
	const char *mmonitorpath = (*env)->GetStringUTFChars(env, monitorpath,
			NULL);
	const char *murl = (*env)->GetStringUTFChars(env, url, NULL);
	int mversion = version;

	//初始化log
	LOG_INFO("um-jni init OK mexepath = %s", mexepath);
	LOG_INFO("um-jni init OK mexename = %s", mexename);
	LOG_INFO("um-jni init OK mmonitorpath = %s", mmonitorpath);
	LOG_INFO("um-jni init OK murl = %s", murl);
	LOG_INFO("um-jni init OK mversion = %d", mversion);
	//fork子进程，以执行轮询任务
	pid_t pid = fork();
	if (pid < 0) {
		//出错log
		LOG_ERROR("um-jni fork failed !!!");
	} else if (pid == 0) {
		LOG_ERROR("um-jni start um procress !!!");
		execl(mexepath, mexename, mmonitorpath, murl, mversion, (char *) NULL);
	} else {
		//父进程直接退出，使子进程被init进程领养，以避免子进程僵死
	}

	return (*env)->NewStringUTF(env, "Hello from JNI !");
}

